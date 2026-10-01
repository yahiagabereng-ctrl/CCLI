#!/usr/bin/env python3
"""Classify root project files, dispose build junk/duplicates, seed architecture.db."""

from __future__ import annotations

import json
import shutil
import sqlite3
import sys
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from project_file_classification import ROOT_CLASSIFICATION  # noqa: E402

ARCH = ROOT / "Architecture"
DB = ARCH / "architecture.db"
SCHEMA_PATCH = """
CREATE TABLE IF NOT EXISTS project_files (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    path            TEXT UNIQUE NOT NULL,
    class           TEXT NOT NULL,
    role            TEXT,
    action          TEXT,
    target          TEXT,
    on_disk         INTEGER DEFAULT 0,
    cleaned_at      TEXT,
    notes           TEXT
);

CREATE VIEW IF NOT EXISTS v_project_files_dispose AS
SELECT path, class, role, action, on_disk
FROM project_files
WHERE action IN ('delete') OR class IN ('DISPOSE_BUILD', 'DUPLICATE_OF_KB');

CREATE VIEW IF NOT EXISTS v_project_files_keep AS
SELECT path, class, role, action
FROM project_files
WHERE action = 'keep' OR class IN ('KEEP_SOURCE', 'KEEP_DELIVERABLE');
"""


def utc_now() -> str:
    return datetime.now(timezone.utc).isoformat()


def ensure_schema(conn: sqlite3.Connection) -> None:
    conn.executescript(SCHEMA_PATCH)
    # Append to schema.sql if not present
    schema_path = ARCH / "schema.sql"
    text = schema_path.read_text(encoding="utf-8")
    if "CREATE TABLE IF NOT EXISTS project_files" not in text:
        schema_path.write_text(text.rstrip() + "\n\n" + SCHEMA_PATCH + "\n", encoding="utf-8")


def relocate_file(src: Path, dest: Path) -> str:
    dest.parent.mkdir(parents=True, exist_ok=True)
    if dest.exists():
        src.unlink(missing_ok=True)
        return f"dest exists — removed root duplicate {src.name}"
    shutil.move(str(src), str(dest))
    return f"moved → {dest.relative_to(ROOT)}"


def clean_disk(apply: bool = True) -> list[dict]:
    results = []
    for row in ROOT_CLASSIFICATION:
        path = ROOT / row["path"]
        on_disk = path.exists()
        entry = {**row, "on_disk": on_disk, "result": "skipped"}
        if not apply:
            entry["result"] = "dry-run"
            results.append(entry)
            continue
        if not on_disk:
            entry["result"] = "already_absent"
            results.append(entry)
            continue

        action = row["action"]
        if action == "delete":
            path.unlink()
            entry["result"] = "deleted"
        elif action == "relocate":
            target = row.get("target") or ""
            # First concrete path before " or "
            dest_rel = target.split(" or ")[0].split(" preferred")[0].strip()
            if dest_rel.startswith("knowledge-base") or dest_rel.startswith("Architecture"):
                dest = ROOT / dest_rel
                # If target is a directory hint ending with /, keep basename
                if dest_rel.endswith("/") or dest.suffix == "":
                    dest = ROOT / dest_rel.rstrip("/") / path.name
                entry["result"] = relocate_file(path, dest)
            else:
                entry["result"] = f"manual relocate: {target}"
        else:
            entry["result"] = "kept"
        results.append(entry)
    return results


def seed_db(results: list[dict]) -> None:
    ARCH.mkdir(parents=True, exist_ok=True)
    conn = sqlite3.connect(DB)
    ensure_schema(conn)
    cur = conn.cursor()
    cur.execute("DELETE FROM project_files")
    now = utc_now()
    for r in results:
        cur.execute(
            """INSERT INTO project_files (path, class, role, action, target, on_disk, cleaned_at, notes)
               VALUES (?, ?, ?, ?, ?, ?, ?, ?)""",
            (
                r["path"],
                r["class"],
                r["role"],
                r["action"],
                r.get("target"),
                1 if (ROOT / r["path"]).exists() else 0,
                now,
                r.get("result"),
            ),
        )
    conn.commit()
    conn.close()


def write_report(results: list[dict]) -> Path:
    deleted = [r for r in results if r.get("result") == "deleted"]
    relocated = [r for r in results if "moved" in str(r.get("result", ""))]
    kept = [r for r in results if r.get("action") == "keep"]
    review = [r for r in results if r.get("action") == "review"]

    lines = [
        "# Project File Classification & Cleanup",
        "",
        f"**Document ID:** CCLI-FILE-CLASS-001  ",
        f"**Date:** {datetime.now(timezone.utc).date().isoformat()}  ",
        f"**Database:** `Architecture/architecture.db` → table `project_files`  ",
        "",
        "## Classification legend",
        "",
        "| Class | Meaning |",
        "|-------|---------|",
        "| KEEP_SOURCE | Authoritative source (.md .tex .mermaid .csv intentional) |",
        "| KEEP_DELIVERABLE | Published PDF/PNG |",
        "| DISPOSE_BUILD | LaTeX `.aux .log .out .toc` — delete |",
        "| DUPLICATE_OF_KB | Root copy also in knowledge-base — delete root |",
        "| RELOCATE_CANDIDATE | Unique PDF — move into KB tree |",
        "| SCRATCH | Review / optional remove |",
        "",
        "## Screenshot set (from Explorer)",
        "",
        "| File | Class | Action |",
        "|------|-------|--------|",
    ]
    screenshot = [
        "CCI_Architecture",
        "CCI_Clone_RE_Option",
        "CCI_Knowledge_Base_Brief",
        "CCI_NXP_IMX8MDQLQCEC",
        "CCI_Project_Roadmap",
        "CCI_SoM_Comparison",
        "_pe_inventory",
    ]
    for r in results:
        if any(r["path"].startswith(p) or r["path"] == "_pe_inventory.csv" for p in screenshot):
            lines.append(f"| `{r['path']}` | {r['class']} | {r['action']} → {r.get('result')} |")

    lines += [
        "",
        "## Cleanup summary",
        "",
        f"| Metric | Count |",
        f"|--------|------:|",
        f"| Classified | {len(results)} |",
        f"| Deleted | {len(deleted)} |",
        f"| Relocated | {len(relocated)} |",
        f"| Kept | {len(kept)} |",
        f"| Review | {len(review)} |",
        "",
        "### Deleted",
        "",
    ]
    for r in deleted:
        lines.append(f"- `{r['path']}` ({r['class']})")
    if not deleted:
        lines.append("- (none)")

    lines += ["", "### Relocated", ""]
    for r in relocated:
        lines.append(f"- `{r['path']}` — {r['result']}")
    if not relocated:
        lines.append("- (none)")

    lines += [
        "",
        "### Keep (essential)",
        "",
        "| Path | Class | Role |",
        "|------|-------|------|",
    ]
    for r in kept:
        lines.append(f"| `{r['path']}` | {r['class']} | {r['role']} |")

    lines += [
        "",
        "## SQL",
        "",
        "```sql",
        "SELECT path, class, action, notes FROM project_files ORDER BY class, path;",
        "SELECT * FROM v_project_files_dispose;",
        "SELECT * FROM v_project_files_keep;",
        "```",
        "",
        f"Generated: {utc_now()}",
        "",
    ]
    path = ARCH / "Project_File_Classification.md"
    path.write_text("\n".join(lines), encoding="utf-8")
    return path


def write_gitignore() -> None:
    gi = ROOT / ".gitignore"
    block = """
# LaTeX build artifacts
*.aux
*.log
*.out
*.toc
*.synctex.gz
*.fls
*.fdb_latexmk

# Knowledge-base build
knowledge-base/_build/

# Excel locks
~$*.xlsx
Architecture/.$*.drawio.dtmp

# Python
__pycache__/
*.pyc
.venv/

# Node (website)
website/node_modules/
website/dist/
website/ngrok.log

# OS
Thumbs.db
Desktop.ini
.DS_Store
"""
    if gi.exists():
        existing = gi.read_text(encoding="utf-8")
        if "*.aux" in existing:
            return
        gi.write_text(existing.rstrip() + "\n" + block, encoding="utf-8")
    else:
        gi.write_text(block.lstrip(), encoding="utf-8")


def main() -> int:
    apply = "--dry-run" not in sys.argv
    results = clean_disk(apply=apply)
    if apply:
        # Ensure DB exists
        if not DB.exists():
            from architecture_db import ArchitectureDB

            ArchitectureDB(DB).reset_and_seed(probe_rag=False)
        seed_db(results)
        report = write_report(results)
        write_gitignore()
        meta = {
            "cleaned_at": utc_now(),
            "deleted": sum(1 for r in results if r.get("result") == "deleted"),
            "relocated": sum(1 for r in results if "moved" in str(r.get("result", ""))),
            "report": str(report),
        }
        (ARCH / "_file_cleanup_meta.json").write_text(json.dumps(meta, indent=2), encoding="utf-8")
        print(json.dumps(meta, indent=2))
        print(f"Report: {report}")
    else:
        print(json.dumps([{k: r[k] for k in ('path', 'class', 'action', 'on_disk')} for r in results], indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

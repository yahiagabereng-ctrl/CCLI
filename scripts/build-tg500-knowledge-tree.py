#!/usr/bin/env python3
"""Render the TG-500 knowledge tree and derive the remaining-work report.

Source of truth: knowledge-base/08-engineering/CCI_TG500_Knowledge_Tree.md
Outputs (Architecture/):
  TG500_Knowledge_Tree.txt   full ASCII tree with per-leaf status
  TG500_Knowledge_Tree.json  machine-readable nodes + roll-up
  TG500_Remaining_Work.md    open items grouped by priority

Run after editing any node status so the summary block cannot drift (R-KT-002).
"""

from __future__ import annotations

import json
import re
import sys
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TREE_MD = ROOT / "knowledge-base" / "08-engineering" / "CCI_TG500_Knowledge_Tree.md"
ARCH = ROOT / "Architecture"

STATUS_ORDER = ["HAVE", "PART", "MISS", "DEF", "BLOCK"]
OPEN_STATUSES = {"MISS", "PART", "BLOCK"}

BRANCH_RE = re.compile(r"^###\s+(K\d)\s+—\s+(.+?)\s*$")
ROW_RE = re.compile(
    r"^\|\s*(K\d\.\d+)\s*\|\s*(.+?)\s*\|\s*`\[(\w+)\]`\s*\|\s*(.*?)\s*\|\s*(.*?)\s*\|\s*(.*?)\s*\|\s*$"
)
GAP_ROW_RE = re.compile(r"^\|\s*(.*?K\d\.\d+.*?)\s*\|.*\|\s*(.*?)\s*\|\s*$")
NODE_REF_RE = re.compile(r"K(\d)\.(\d+)(?:\s*[–-]\s*(\d+))?")
PRIORITY_RE = re.compile(r"\bP([012])\b")

TREE_GLYPH = {"HAVE": "+", "PART": "~", "MISS": "!", "DEF": ".", "BLOCK": "X"}


def strip_md(text: str) -> str:
    """Drop bold/code markers so plain-text output stays readable."""
    text = re.sub(r"\*\*(.+?)\*\*", r"\1", text)
    return text.replace("`", "").strip()


def parse_tree(md_path: Path) -> list[dict]:
    branches: list[dict] = []
    current: dict | None = None

    for line in md_path.read_text(encoding="utf-8").splitlines():
        branch_match = BRANCH_RE.match(line)
        if branch_match:
            current = {
                "id": branch_match.group(1),
                "title": strip_md(branch_match.group(2)),
                "nodes": [],
            }
            branches.append(current)
            continue

        row_match = ROW_RE.match(line)
        if row_match and current is not None:
            node_id, item, status, source, risk, gate = row_match.groups()
            current["nodes"].append(
                {
                    "id": node_id,
                    "item": strip_md(item),
                    "status": status,
                    "source_id": strip_md(source).replace("—", ""),
                    "risk": strip_md(risk).replace("—", ""),
                    "gate": strip_md(gate).replace("—", ""),
                }
            )

    return branches


def parse_gap_priorities(md_path: Path) -> dict[str, str]:
    """Section 3 assigns P0/P1/P2 per gap area; map those back onto node ids.

    Areas reference nodes inline, sometimes as ranges such as `K5.3-4`.
    """
    priorities: dict[str, str] = {}

    for line in md_path.read_text(encoding="utf-8").splitlines():
        gap_match = GAP_ROW_RE.match(line)
        if not gap_match:
            continue
        area, gap_text = gap_match.groups()
        priority_match = PRIORITY_RE.search(gap_text)
        if not priority_match:
            continue
        priority = f"P{priority_match.group(1)}"

        for branch, first, last in NODE_REF_RE.findall(area):
            start = int(first)
            end = int(last) if last else start
            for index in range(start, end + 1):
                priorities[f"K{branch}.{index}"] = priority

    return priorities


def counts(nodes: list[dict]) -> dict[str, int]:
    tally = {status: 0 for status in STATUS_ORDER}
    for node in nodes:
        tally[node["status"]] = tally.get(node["status"], 0) + 1
    return tally


def summarise(tally: dict[str, int]) -> str:
    return " · ".join(f"{tally[s]} {s}" for s in STATUS_ORDER if tally.get(s))


def priority_of(node: dict, gap_priorities: dict[str, str]) -> str:
    """Gate column and the section-3 gap table both carry priority; take the higher."""
    candidates = []

    gate = node["gate"].upper()
    if "P0" in gate:
        candidates.append("P0")
    elif "P1" in gate:
        candidates.append("P1")

    if node["id"] in gap_priorities:
        candidates.append(gap_priorities[node["id"]])

    if node["status"] == "BLOCK":
        candidates.append("P0")

    if candidates:
        return min(candidates)

    return "P2" if node["status"] == "MISS" else "P3"


def render_tree(branches: list[dict]) -> str:
    lines = ["CCI TG-500 Knowledge Tree", ""]
    all_nodes = [n for b in branches for n in b["nodes"]]

    for branch_idx, branch in enumerate(branches):
        last_branch = branch_idx == len(branches) - 1
        branch_stem = "`--" if last_branch else "|--"
        child_pad = "    " if last_branch else "|   "
        lines.append(f"{branch_stem} {branch['id']}  {branch['title']}")
        lines.append(f"{child_pad}    [{summarise(counts(branch['nodes']))}]")

        for node_idx, node in enumerate(branch["nodes"]):
            last_node = node_idx == len(branch["nodes"]) - 1
            node_stem = "`--" if last_node else "|--"
            glyph = TREE_GLYPH.get(node["status"], "?")
            label = f"{child_pad}{node_stem} [{glyph}] {node['id']}  {node['item']}"
            extras = []
            if node["source_id"]:
                extras.append(node["source_id"])
            if node["gate"]:
                extras.append(f"gate {node['gate']}")
            if extras:
                label += f"   ({'; '.join(extras)})"
            lines.append(label)

        if not last_branch:
            lines.append("|")

    total = counts(all_nodes)
    lines += [
        "",
        f"Legend: [+] HAVE  [~] PART  [!] MISS  [.] DEF  [X] BLOCK",
        f"Total: {summarise(total)} ({len(all_nodes)} leaf nodes)",
    ]
    return "\n".join(lines)


def render_remaining(branches: list[dict], gap_priorities: dict[str, str], generated_at: str) -> str:
    open_nodes = [
        {
            **node,
            "branch": branch["id"],
            "branch_title": branch["title"],
            "priority": priority_of(node, gap_priorities),
        }
        for branch in branches
        for node in branch["nodes"]
        if node["status"] in OPEN_STATUSES
    ]

    lines = [
        "# TG-500 Knowledge Tree — Remaining Work",
        "",
        f"Generated: {generated_at}",
        "",
        "Derived from `knowledge-base/08-engineering/CCI_TG500_Knowledge_Tree.md`.",
        "Regenerate with `python scripts/build-tg500-knowledge-tree.py`.",
        "",
    ]

    for priority in ["P0", "P1", "P2", "P3"]:
        bucket = [n for n in open_nodes if n["priority"] == priority]
        if not bucket:
            continue
        lines += [
            f"## {priority} — {len(bucket)} open",
            "",
            "| Node | Branch | Status | Knowledge item | Risk |",
            "|------|--------|--------|----------------|------|",
        ]
        for node in bucket:
            risk = node["risk"] or "—"
            lines.append(
                f"| {node['id']} | {node['branch']} | `{node['status']}` | {node['item']} | {risk} |"
            )
        lines.append("")

    blocked = [n for n in open_nodes if n["status"] == "BLOCK"]
    if blocked:
        lines += ["## Blocked on external party", ""]
        for node in blocked:
            lines.append(f"- **{node['id']}** {node['item']}")
        lines.append("")

    lines += [
        "## Roll-up",
        "",
        "| Branch | Title | Status |",
        "|--------|-------|--------|",
    ]
    for branch in branches:
        lines.append(f"| {branch['id']} | {branch['title']} | {summarise(counts(branch['nodes']))} |")

    return "\n".join(lines) + "\n"


def main() -> int:
    # Corpus text carries en-dashes and degree signs; the Windows console is cp1252.
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")

    if not TREE_MD.exists():
        print(f"Missing source: {TREE_MD}")
        return 1

    branches = parse_tree(TREE_MD)
    if not branches:
        print("No branches parsed — check heading format '### K0 — Title'")
        return 1

    gap_priorities = parse_gap_priorities(TREE_MD)

    ARCH.mkdir(parents=True, exist_ok=True)
    generated_at = datetime.now(timezone.utc).isoformat()

    all_nodes = [n for b in branches for n in b["nodes"]]
    total = counts(all_nodes)

    tree_txt = render_tree(branches)
    (ARCH / "TG500_Knowledge_Tree.txt").write_text(tree_txt + "\n", encoding="utf-8")
    (ARCH / "TG500_Remaining_Work.md").write_text(
        render_remaining(branches, gap_priorities, generated_at), encoding="utf-8"
    )
    (ARCH / "TG500_Knowledge_Tree.json").write_text(
        json.dumps(
            {
                "generated_at": generated_at,
                "source": str(TREE_MD.relative_to(ROOT)).replace("\\", "/"),
                "totals": total,
                "leaf_count": len(all_nodes),
                "branches": branches,
            },
            indent=2,
            ensure_ascii=False,
        ),
        encoding="utf-8",
    )

    print(tree_txt)
    print()
    print("Wrote:")
    for name in ("TG500_Knowledge_Tree.txt", "TG500_Knowledge_Tree.json", "TG500_Remaining_Work.md"):
        print(f"  Architecture/{name}")

    open_count = sum(total.get(s, 0) for s in OPEN_STATUSES)
    print(f"\nOpen items: {open_count} of {len(all_nodes)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

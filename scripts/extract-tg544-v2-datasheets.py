#!/usr/bin/env python3
"""Validate TG-544 V2 datasheet PDFs and extract text for RAG."""

from __future__ import annotations

import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
DS = REPO / "knowledge-base/08-engineering/reference/vendor/tespro/2026-10-22-tg544-v2-schematic/datasheets"
OUT = DS / "_extracts"


def is_pdf(path: Path) -> bool:
    head = path.read_bytes()[:8]
    return head.startswith(b"%PDF")


def extract_pdf(path: Path, out: Path) -> tuple[int, int]:
    import fitz

    doc = fitz.open(path)
    parts: list[str] = [f"# {path.name}\n"]
    printable = 0
    for i in range(len(doc)):
        t = doc[i].get_text() or ""
        printable += len(re.sub(r"\s", "", t))
        parts.append(f"\n----- PAGE {i + 1} of {len(doc)} -----\n{t}")
    text = "\n".join(parts)
    out.write_text(text, encoding="utf-8", errors="replace")
    return len(doc), printable


def main() -> int:
    OUT.mkdir(parents=True, exist_ok=True)
    rows: list[str] = []
    ok = warn = fail = 0

    for path in sorted(DS.glob("*")):
        if path.suffix.lower() != ".pdf":
            continue
        if not is_pdf(path):
            rows.append(f"FAIL\tNOT_PDF\t{path.name}\t{path.stat().st_size}")
            fail += 1
            continue
        out = OUT / f"{path.stem}.txt"
        try:
            pages, printable = extract_pdf(path, out)
        except Exception as exc:  # noqa: BLE001
            rows.append(f"FAIL\tEXTRACT\t{path.name}\t{exc}")
            fail += 1
            continue
        if printable < 200:
            rows.append(f"WARN\tLOW_TEXT\t{path.name}\tpages={pages}\tchars={printable}")
            warn += 1
        else:
            rows.append(f"OK\t\t{path.name}\tpages={pages}\tchars={printable}")
            ok += 1

    manifest = OUT / "INGEST_CHECK.txt"
    header = f"# TG544 V2 datasheets ingest check\nOK={ok} WARN={warn} FAIL={fail}\n\n"
    manifest.write_text(header + "\n".join(rows) + "\n", encoding="utf-8")
    print(manifest.read_text(encoding="utf-8"))
    return 1 if fail else 0


if __name__ == "__main__":
    raise SystemExit(main())

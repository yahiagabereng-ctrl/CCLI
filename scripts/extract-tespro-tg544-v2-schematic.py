#!/usr/bin/env python3
"""Extract text from TesPro TG544 V2 schematic PDF (vector or OCR fallback)."""

from __future__ import annotations

import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
PDF = (
    REPO
    / "knowledge-base/08-engineering/reference/vendor/tespro/2026-10-22-tg544-v2-schematic/TG544_V2_Schematic.pdf"
)
OUT_DIR = PDF.parent / "_extracts"
OUT_FULL = OUT_DIR / "TG544_V2_Schematic_full.txt"
OUT_INDEX = OUT_DIR / "TG544_V2_sheet_index.txt"

# Block labels printed on the composite TG-544 V2 PDF (functional "sheets").
SHEET_BLOCKS = [
    "POWER",
    "SIM Card",
    "Gigabit Ethernet",
    "SFP Fiber to RJ45",
    "TPM 2.0",
    "Isolated RS-485",
    "Isolated RS-232",
    "Isolated DI*6 & DO*6",
    "USB Type-A Port",
    "USB to UART",
    "TF Card Reader",
    "LED & Reset",
    "Hall",
    "Connector",
]


def extract_vector(doc) -> tuple[str, int]:
    parts: list[str] = []
    printable = 0
    for i in range(len(doc)):
        t = doc[i].get_text() or ""
        printable += len(re.sub(r"\s", "", t))
        parts.append(f"\n----- PAGE {i + 1} of {len(doc)} -----\n{t}")
    return "\n".join(parts), printable


def ocr_page(doc, page_idx: int, zoom: float = 2.5) -> str:
    import fitz

    page = doc[page_idx]
    mat = fitz.Matrix(zoom, zoom)
    pix = page.get_pixmap(matrix=mat, alpha=False)
    try:
        import pytesseract
        from PIL import Image
    except ImportError as exc:
        return f"[OCR skipped: {exc}]"
    img = Image.frombytes("RGB", (pix.width, pix.height), pix.samples)
    return pytesseract.image_to_string(img) or ""


def sheet_hints(text: str) -> list[str]:
    hints: list[str] = []
    pages = text.split("----- PAGE ")
    for chunk in pages[1:]:
        head = chunk.split("\n", 1)[0]
        page_no = head.strip().split()[0]
        found = [b for b in SHEET_BLOCKS if b in chunk]
        if found:
            hints.append(f"PAGE {page_no}: blocks = {', '.join(found)}")
        else:
            hints.append(f"PAGE {page_no}: (no block label matched)")
    return hints


def write_sheet_block_index(text: str, path: Path) -> None:
    lines = [
        "# TG-544 V2 functional sheet blocks (from PDF labels)",
        "",
        "| ID | Block | In extract |",
        "|----|-------|------------|",
    ]
    for i, block in enumerate(SHEET_BLOCKS, start=1):
        present = "YES" if block in text else "MISSING"
        sid = f"S{i:02d}"
        lines.append(f"| {sid} | {block} | {present} |")
    lines.append("")
    lines.append("Matrix: `lab/evidence/hw/TG544_V2_REQUIREMENT_SHEET_VERIFY_MATRIX.md`")
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    if not PDF.is_file():
        print(f"MISSING: {PDF}", file=sys.stderr)
        return 2
    import fitz

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    doc = fitz.open(PDF)
    text, printable = extract_vector(doc)
    if printable < 400:
        print(f"Low vector text ({printable} chars) — OCR per page…")
        ocr_parts: list[str] = []
        for i in range(len(doc)):
            ocr_parts.append(f"\n----- PAGE {i + 1} of {len(doc)} (OCR) -----\n")
            ocr_parts.append(ocr_page(doc, i))
        text = "\n".join(ocr_parts)
    OUT_FULL.write_text(text, encoding="utf-8", errors="replace")
    hints = sheet_hints(text)
    OUT_INDEX.write_text("\n".join(hints) + "\n", encoding="utf-8")
    write_sheet_block_index(text, OUT_DIR / "TG544_V2_sheet_blocks.md")
    print(f"Wrote {OUT_FULL} ({len(doc)} pages, {len(text)} chars)")
    print(f"Wrote {OUT_INDEX} ({len(hints)} lines)")
    print(f"Wrote {OUT_DIR / 'TG544_V2_sheet_blocks.md'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

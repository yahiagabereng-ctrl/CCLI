#!/usr/bin/env python3
"""OCR IEC 61850-8-1 PDF (image-only) to per-page text files."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

import fitz  # PyMuPDF
import pytesseract
from PIL import Image
import io

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_PDF = Path(r"C:\Users\yahia\OneDrive\Desktop\61850-1.pdf")
OUT_DIR = ROOT / "Architecture" / "_extracted_reg_analysis" / "_pdf_ocr" / "61850-8-1"

TESS = Path(r"C:\Program Files\Tesseract-OCR\tesseract.exe")
if TESS.is_file():
    pytesseract.pytesseract.tesseract_cmd = str(TESS)


def ocr_page(doc: fitz.Document, page_idx: int, zoom: float = 2.0) -> str:
    page = doc[page_idx]
    mat = fitz.Matrix(zoom, zoom)
    pix = page.get_pixmap(matrix=mat, alpha=False)
    img = Image.open(io.BytesIO(pix.tobytes("png")))
    text = pytesseract.image_to_string(img, config="--psm 6")
    return text.strip()


def iec_footer_page(text: str) -> int | None:
    m = re.search(r"-\s*(\d+)\s*-", text)
    if m:
        return int(m.group(1))
    return None


def main() -> int:
    pdf_path = Path(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_PDF
    if not pdf_path.is_file():
        print(f"PDF not found: {pdf_path}", file=sys.stderr)
        return 1

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    doc = fitz.open(pdf_path)
    toc: list[dict] = []

    for i in range(doc.page_count):
        out = OUT_DIR / f"p{i + 1:03d}.txt"
        if out.is_file() and out.stat().st_size > 80:
            text = out.read_text(encoding="utf-8", errors="replace")
        else:
            text = ocr_page(doc, i)
            out.write_text(text, encoding="utf-8")
        head = " ".join(text.split())[:120]
        toc.append(
            {
                "file_page": i + 1,
                "iec_page": iec_footer_page(text),
                "chars": len(text),
                "head": head,
            }
        )
        if (i + 1) % 10 == 0:
            print(f"OCR {i + 1}/{doc.page_count}")

    (OUT_DIR / "toc.json").write_text(json.dumps(toc, indent=2), encoding="utf-8")
    print(f"Done: {doc.page_count} pages -> {OUT_DIR}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

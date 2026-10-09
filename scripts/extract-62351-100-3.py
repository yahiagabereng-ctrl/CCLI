#!/usr/bin/env python3
"""Extract IEC TS 62351-100-3 PDF to text (text layer + OCR fallback per page)."""

from __future__ import annotations

import io
import json
import re
import sys
from pathlib import Path

import fitz
import pytesseract
from PIL import Image
from pypdf import PdfReader

REPO = Path(__file__).resolve().parents[1]
DEFAULT_PDF = REPO / "knowledge-base/08-engineering/reference/iec62351/IEC_TS_62351-100-3-2020.pdf"
OUT_DIR = REPO / "knowledge-base/08-engineering/reference/iec62351/_extract_62351-100-3"
TESS = Path(r"C:\Program Files\Tesseract-OCR\tesseract.exe")
MIN_CHARS = 120
OCR_ZOOM = 2.0


def printable_len(text: str) -> int:
    return len(re.sub(r"\s", "", text or ""))


def ocr_page(doc: fitz.Document, page_idx: int) -> str:
    if TESS.is_file():
        pytesseract.pytesseract.tesseract_cmd = str(TESS)
    page = doc[page_idx]
    mat = fitz.Matrix(OCR_ZOOM, OCR_ZOOM)
    pix = page.get_pixmap(matrix=mat, alpha=False)
    img = Image.open(io.BytesIO(pix.tobytes("png")))
    return pytesseract.image_to_string(img, lang="eng", config="--psm 6").strip()


def main() -> int:
    pdf_path = Path(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_PDF
    if not pdf_path.is_file():
        print(f"Missing PDF: {pdf_path}", file=sys.stderr)
        return 1

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    reader = PdfReader(str(pdf_path))
    doc = fitz.open(pdf_path)
    n = len(reader.pages)
    toc: list[dict] = []
    combined: list[str] = []

    for i in range(n):
        page_out = OUT_DIR / f"p{i + 1:03d}.txt"
        text = reader.pages[i].extract_text() or ""
        method = "text"
        if printable_len(text) < MIN_CHARS:
            text = ocr_page(doc, i)
            method = "ocr"
        page_out.write_text(text, encoding="utf-8")
        combined.append(f"\n----- PAGE {i + 1} ({method}) -----\n{text}")
        head = " ".join(text.split())[:100]
        toc.append({"page": i + 1, "method": method, "chars": len(text), "head": head})
        if (i + 1) % 10 == 0:
            print(f"  {i + 1}/{n} ({method})")

    doc.close()
    (OUT_DIR / "full.txt").write_text("\n".join(combined), encoding="utf-8")
    (OUT_DIR / "toc.json").write_text(json.dumps(toc, indent=2), encoding="utf-8")
    print(f"Done {n} pages -> {OUT_DIR}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

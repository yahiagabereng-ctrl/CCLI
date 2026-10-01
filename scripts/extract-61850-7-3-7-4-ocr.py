#!/usr/bin/env python3
"""OCR IEC 61850-7-3 / 7-4 PDFs (image-only doc88 exports) to per-page text."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

import fitz
import io
import pytesseract
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
DROP = ROOT / "knowledge-base" / "07-protocols" / "regulations" / "standards-drop-20260918"
OCR_ROOT = ROOT / "Architecture" / "_extracted_reg_analysis" / "_pdf_ocr"

TESS = Path(r"C:\Program Files\Tesseract-OCR\tesseract.exe")
if TESS.is_file():
    pytesseract.pytesseract.tesseract_cmd = str(TESS)

PDFS = {
    "61850-7-3": DROP / "IEC 61850-7-3-2020.pdf",
    "61850-7-4": DROP / "IEC 61850-7-4-2010.pdf",
}


def ocr_page(doc: fitz.Document, page_idx: int, zoom: float = 2.0) -> str:
    page = doc[page_idx]
    mat = fitz.Matrix(zoom, zoom)
    pix = page.get_pixmap(matrix=mat, alpha=False)
    img = Image.open(io.BytesIO(pix.tobytes("png")))
    return pytesseract.image_to_string(img, config="--psm 6").strip()


def iec_footer_page(text: str) -> int | None:
    m = re.search(r"-\s*(\d+)\s*-", text)
    return int(m.group(1)) if m else None


def ocr_pdf(slug: str, pdf_path: Path) -> int:
    out_dir = OCR_ROOT / slug
    out_dir.mkdir(parents=True, exist_ok=True)
    doc = fitz.open(pdf_path)
    page_count = doc.page_count
    toc: list[dict] = []
    for i in range(page_count):
        out = out_dir / f"p{i + 1:03d}.txt"
        if out.is_file() and out.stat().st_size > 80:
            text = out.read_text(encoding="utf-8", errors="replace")
        else:
            text = ocr_page(doc, i)
            out.write_text(text, encoding="utf-8")
        toc.append(
            {
                "file_page": i + 1,
                "iec_page": iec_footer_page(text),
                "chars": len(text),
                "head": " ".join(text.split())[:120],
            }
        )
        if (i + 1) % 10 == 0:
            print(f"{slug}: OCR {i + 1}/{page_count}")
    (out_dir / "toc.json").write_text(json.dumps(toc, indent=2), encoding="utf-8")
    doc.close()
    print(f"{slug}: Done {page_count} pages -> {out_dir}")
    return page_count


def main() -> int:
    targets = sys.argv[1:] if len(sys.argv) > 1 else list(PDFS.keys())
    for slug in targets:
        pdf = PDFS.get(slug)
        if pdf is None or not pdf.is_file():
            print(f"PDF not found for {slug}: {pdf}", file=sys.stderr)
            return 1
        ocr_pdf(slug, pdf)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""OCR IEC 62351-4 doc88 PDF (image scan) — priority scan + full page export."""

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
DEFAULT_PDF = Path(r"C:\Users\yahia\OneDrive\Desktop\IEC 62351-4 2020 - 道客巴巴.pdf")
OUT_DIR = ROOT / "Architecture" / "_extracted_reg_analysis" / "_pdf_ocr" / "62351-4"
TESS = Path(r"C:\Program Files\Tesseract-OCR\tesseract.exe")
if TESS.is_file():
    pytesseract.pytesseract.tesseract_cmd = str(TESS)


def ocr_page(doc: fitz.Document, page_idx: int, zoom: float = 2.0) -> str:
    page = doc[page_idx]
    mat = fitz.Matrix(zoom, zoom)
    pix = page.get_pixmap(matrix=mat, alpha=False)
    img = Image.open(io.BytesIO(pix.tobytes("png")))
    return pytesseract.image_to_string(img, config="--psm 6").strip()


def iec_footer_page(text: str) -> int | None:
    m = re.search(r"-\s*(\d+)\s*-", text)
    return int(m.group(1)) if m else None


def scan_content_pages(doc: fitz.Document, sample_every: int = 5) -> list[int]:
    """Find file pages that look like real IEC body (not doc88 wrapper)."""
    hits: list[int] = []
    for i in range(0, doc.page_count, sample_every):
        text = ocr_page(doc, i, zoom=1.2)
        low = text.lower()
        if "62351" in low or "iec 62351" in low or "annex g" in low or "g.6.2" in low:
            hits.append(i + 1)
    return hits


def main() -> int:
    pdf_path = Path(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_PDF
    mode = sys.argv[2] if len(sys.argv) > 2 else "scan"
    if not pdf_path.is_file():
        print(f"PDF not found: {pdf_path}", file=sys.stderr)
        return 1
    if not TESS.is_file():
        print(f"Tesseract not found: {TESS}", file=sys.stderr)
        return 1

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    doc = fitz.open(pdf_path)
    print(f"PDF: {pdf_path.name} pages={doc.page_count}")

    if mode == "scan":
        hits = scan_content_pages(doc)
        (OUT_DIR / "content_scan.json").write_text(
            json.dumps({"pdf": str(pdf_path), "sample_hits": hits}, indent=2),
            encoding="utf-8",
        )
        print(f"Content-like pages (sample): {hits[:30]}")
        return 0

    # full OCR — optional page range: ocr 101 110
    start = int(sys.argv[3]) if len(sys.argv) > 3 else 0
    end = int(sys.argv[4]) if len(sys.argv) > 4 else doc.page_count
    toc: list[dict] = []
    for i in range(start, min(end, doc.page_count)):
        out = OUT_DIR / f"p{i + 1:03d}.txt"
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
            print(f"OCR {i + 1}/{doc.page_count}")
    (OUT_DIR / "toc.json").write_text(json.dumps(toc, indent=2), encoding="utf-8")
    print(f"Done: pages {start + 1}-{end} -> {OUT_DIR}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

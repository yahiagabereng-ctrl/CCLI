#!/usr/bin/env python3
"""OCR scanned Certificate/*.pdf into Certificate/_extracts/*_ocr.txt and *_ocr.md."""

from __future__ import annotations

import io
import re
import sys
from pathlib import Path

import fitz  # PyMuPDF
import pytesseract
from PIL import Image
from pypdf import PdfReader

REPO = Path(__file__).resolve().parents[1]
CERT = REPO / "Certificate"
OUT = CERT / "_extracts"
TESSERACT = Path(r"C:\Program Files\Tesseract-OCR\tesseract.exe")
# Italian declarations + CEI text; English fallback for mixed certs
OCR_LANG = "ita+eng"
MIN_PRINTABLE = 200  # below this -> treat as scanned and OCR


def printable_len(text: str) -> int:
    return len(re.sub(r"\s", "", text or ""))


def pdf_text_layer(pdf: Path) -> tuple[int, str]:
    reader = PdfReader(str(pdf))
    chunks: list[str] = []
    for i, page in enumerate(reader.pages, 1):
        t = page.extract_text() or ""
        chunks.append(f"\n----- PAGE {i} (text layer) -----\n{t}")
    return len(reader.pages), "\n".join(chunks)


def ocr_pdf(pdf: Path, dpi: int = 300) -> str:
    if TESSERACT.is_file():
        pytesseract.pytesseract.tesseract_cmd = str(TESSERACT)
    doc = fitz.open(pdf)
    parts: list[str] = []
    for i, page in enumerate(doc, 1):
        mat = fitz.Matrix(dpi / 72, dpi / 72)
        pix = page.get_pixmap(matrix=mat, alpha=False)
        img = Image.open(io.BytesIO(pix.tobytes("png")))
        text = pytesseract.image_to_string(img, lang=OCR_LANG)
        text = re.sub(r"[ \t]+", " ", text)
        parts.append(f"\n----- PAGE {i} (OCR {OCR_LANG} @ {dpi}dpi) -----\n{text.strip()}")
    doc.close()
    return "\n".join(parts)


def md_from_ocr(pdf: Path, ocr_text: str) -> str:
    return "\n".join(
        [
            f"# OCR extract — {pdf.name}",
            "",
            f"**Source:** `Certificate/{pdf.name}`",
            f"**Method:** PyMuPDF render + Tesseract `{OCR_LANG}`",
            f"**Regenerate:** `python scripts/ocr-certificates.py`",
            "",
            "---",
            "",
            "```text",
            ocr_text.strip(),
            "```",
            "",
        ]
    )


def main() -> int:
    OUT.mkdir(parents=True, exist_ok=True)
    pdfs = sorted(CERT.glob("*.pdf"))
    if not pdfs:
        print(f"No PDFs in {CERT}")
        return 1

    ocr_count = 0
    for pdf in pdfs:
        pages, layer = pdf_text_layer(pdf)
        plen = printable_len(layer)
        safe = re.sub(r"[^\w\-]+", "_", pdf.stem)[:80]
        layer_path = OUT / f"{safe}.txt"
        layer_path.write_text(layer, encoding="utf-8", errors="replace")

        needs_ocr = plen < MIN_PRINTABLE
        if needs_ocr:
            print(f"OCR: {pdf.name} (text layer printable={plen})")
            ocr_text = ocr_pdf(pdf)
            ocr_path = OUT / f"{safe}_ocr.txt"
            md_path = OUT / f"{safe}_ocr.md"
            ocr_path.write_text(ocr_text, encoding="utf-8", errors="replace")
            md_path.write_text(md_from_ocr(pdf, ocr_text), encoding="utf-8", errors="replace")
            olen = printable_len(ocr_text)
            print(f"  -> {ocr_path.name} ({pages} pages, printable={olen})")
            print(f"  -> {md_path.name}")
            ocr_count += 1
        else:
            print(f"SKIP OCR (text layer OK): {pdf.name} printable={plen}")

    print(f"\nDone. OCR'd {ocr_count} of {len(pdfs)} PDFs.")
    return 0


if __name__ == "__main__":
    sys.exit(main())

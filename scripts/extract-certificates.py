#!/usr/bin/env python3
"""Extract Certificate/*.pdf text into Certificate/_extracts/*.txt for RAG prep."""

from __future__ import annotations

import re
from pathlib import Path

from pypdf import PdfReader

REPO = Path(__file__).resolve().parents[1]
CERT = REPO / "Certificate"
OUT = CERT / "_extracts"


def main() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    pdfs = sorted(CERT.glob("*.pdf"))
    if not pdfs:
        print(f"No PDFs in {CERT}")
        return
    for pdf in pdfs:
        reader = PdfReader(str(pdf))
        chunks: list[str] = []
        for i, page in enumerate(reader.pages, 1):
            try:
                t = page.extract_text() or ""
            except Exception as exc:  # noqa: BLE001
                t = f"[extract error: {exc}]"
            chunks.append(f"\n----- PAGE {i} -----\n{t}")
        text = "\n".join(chunks)
        safe = re.sub(r"[^\w\-]+", "_", pdf.stem)[:80]
        out = OUT / f"{safe}.txt"
        out.write_text(text, encoding="utf-8", errors="replace")
        printable = len(re.sub(r"\s", "", text))
        print(f"{pdf.name}: pages={len(reader.pages)} chars={len(text)} printable={printable} -> {out.name}")
        if printable < 200:
            print("  WARNING: likely scanned PDF - run: python scripts/ocr-certificates.py")


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Extract CEI TR 57-126 PDF text and Annex A SCL/CID into corpus paths."""
from __future__ import annotations

import re
import shutil
import xml.etree.ElementTree as ET
from pathlib import Path

import pypdf

ROOT = Path(__file__).resolve().parents[1]
SRC_PDF = ROOT / "CEI TR 57-126.pdf"
DST_PDF = ROOT / "knowledge-base" / "07-protocols" / "cei-tr-57-126.pdf"
RAW_TXT = ROOT / "Architecture" / "_extracted_reg_analysis" / "cei_tr_57-126_en.txt"
CID_OUT = ROOT / "apps" / "ccli" / "config" / "icd" / "cei-tr-57-126-example.cid"

PAGE_HEADER = re.compile(
    r"TECHNICAL REPORT\s+CEI TR 50\s*-\s*126:2024\s*-\s*11\s*\d*\s*",
    re.MULTILINE,
)
ANNEX_HEADER = re.compile(
    r"Annex\s+A\s*CC SCL file\s*Example of CCI CID file.*?Chapter\s+6\.\s*",
    re.DOTALL | re.IGNORECASE,
)


def extract_pdf_text(pdf_path: Path) -> str:
    reader = pypdf.PdfReader(str(pdf_path))
    parts: list[str] = []
    for i, page in enumerate(reader.pages, start=1):
        parts.append(f"--- PAGE {i} ---\n{(page.extract_text() or '')}")
    return "\n".join(parts)


def raw_xml_from_text(full_text: str) -> str:
    start = full_text.find("<?xml")
    end = full_text.rfind("</SCL>")
    if start < 0 or end < 0:
        raise RuntimeError("SCL XML not found in extracted text")
    return full_text[start : end + len("</SCL>")]


def fix_spaced_tag_names(xml: str) -> str:
    """Repair PDF artefacts such as ``<V al >`` and ``<E numV al>``."""
    xml = re.sub(r"<\s*E\s+numV\s+al\b", "<EnumVal", xml, flags=re.IGNORECASE)
    xml = re.sub(r"<\s*/\s*E\s+numV\s+al\s*>", "</EnumVal>", xml, flags=re.IGNORECASE)
    for _ in range(6):
        xml = re.sub(
            r"<\s*([A-Za-z])\s+([a-zA-Z]+)(\s|>|/)",
            lambda m: f"<{m.group(1)}{m.group(2)}{m.group(3)}",
            xml,
        )
        xml = re.sub(
            r"<\s*/\s*([A-Za-z])\s+([a-zA-Z]+)\s*>",
            lambda m: f"</{m.group(1)}{m.group(2)}>",
            xml,
        )
    return xml


def fix_val_text(xml: str) -> str:
    """Remove spurious spaces inside short enum / ctlModel values."""

    def _clean_val(match: re.Match[str]) -> str:
        value = re.sub(r"\s+", "", match.group(1))
        return f"<Val>{value}</Val>"

    return re.sub(r"<Val>([^<]*)</Val>", _clean_val, xml)


PAGE_MARKER = re.compile(r"--- PAGE \d+ ---\s*", re.MULTILINE)


def clean_scl(raw: str) -> str:
    xml = PAGE_HEADER.sub("", raw)
    xml = ANNEX_HEADER.sub("", xml)
    xml = PAGE_MARKER.sub("", xml)
    xml = fix_spaced_tag_names(xml)
    xml = fix_val_text(xml)
    xml = re.sub(r">\s+<", ">\n<", xml)
    return xml.strip() + "\n"


def validate_scl(xml: str) -> None:
    ET.fromstring(xml.encode("utf-8"))


def main() -> None:
    if not SRC_PDF.is_file():
        raise SystemExit(f"Missing source PDF: {SRC_PDF}")

    DST_PDF.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(SRC_PDF, DST_PDF)

    full_text = extract_pdf_text(SRC_PDF)
    RAW_TXT.parent.mkdir(parents=True, exist_ok=True)
    RAW_TXT.write_text(full_text, encoding="utf-8")

    raw_xml = raw_xml_from_text(full_text)
    cid = clean_scl(raw_xml)
    validate_scl(cid)

    CID_OUT.parent.mkdir(parents=True, exist_ok=True)
    CID_OUT.write_text(cid, encoding="utf-8")

    print(f"PDF -> {DST_PDF}")
    print(f"Text -> {RAW_TXT} ({RAW_TXT.stat().st_size} bytes)")
    print(f"CID  -> {CID_OUT} ({CID_OUT.stat().st_size} bytes)")


if __name__ == "__main__":
    main()

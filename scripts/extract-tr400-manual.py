#!/usr/bin/env python3
"""Extract TesPro TR400 User Manual PDF into CCI_TR400_Extract.md."""

from __future__ import annotations

import argparse
import re
import sys
from datetime import date
from pathlib import Path

try:
    import fitz  # PyMuPDF
except ImportError as exc:  # pragma: no cover
    raise SystemExit("PyMuPDF required: pip install pymupdf") from exc

REPO = Path(__file__).resolve().parents[1]
DEFAULT_PDF = REPO / "knowledge-base" / "08-engineering" / "reference" / "TR400_User_Manual_EN_v1.0.pdf"
DEFAULT_OUT = REPO / "knowledge-base" / "08-engineering" / "CCI_TR400_Extract.md"

SECTION_HINTS = (
    "introduction",
    "specification",
    "hardware",
    "interface",
    "ethernet",
    "serial",
    "rs485",
    "rs232",
    "di",
    "do",
    "gpio",
    "power",
    "install",
    "configuration",
    "tesproos",
    "openwrt",
    "firmware",
    "update",
    "tpm",
    "secure",
    "led",
    "antenna",
    "cellular",
    "gnss",
    "pin",
    "connector",
    "dimension",
    "troubleshoot",
    "appendix",
)


def normalize_line(line: str) -> str:
    line = line.replace("\uff5e", "~").replace("\u2013", "-").replace("\u2014", "-")
    line = re.sub(r"\s+", " ", line).strip()
    return line


def page_text(page: fitz.Page) -> str:
    text = page.get_text("text")
    lines = [normalize_line(ln) for ln in text.splitlines()]
    return "\n".join(ln for ln in lines if ln)


def guess_sections(pages: list[str]) -> list[tuple[str, int, str]]:
    sections: list[tuple[str, int, str]] = []
    for idx, text in enumerate(pages, start=1):
        for line in text.splitlines():
            if len(line) < 4 or len(line) > 80:
                continue
            lower = line.lower()
            if line.isupper() and len(line.split()) <= 8:
                sections.append((line.title(), idx, line))
                break
            if any(h in lower for h in SECTION_HINTS) and line[0].isupper():
                if re.match(r"^\d+(\.\d+)*\s+", line) or line.endswith(":"):
                    sections.append((line, idx, line))
                    break
    return sections


def keyword_hits(pages: list[str]) -> dict[str, list[tuple[int, str]]]:
    keys = (
        "TR-424",
        "TR-425",
        "TR-444",
        "TR-445",
        "TG-424",
        "TG-524",
        "RS485",
        "RS232",
        "DI",
        "DO",
        "GPIO",
        "TPM",
        "Secure Boot",
        "OpenWrt",
        "TesproOS",
        "WAN",
        "LAN",
        "12V",
        "12-36",
        "libgpiod",
        "tty",
        "eth",
    )
    hits: dict[str, list[tuple[int, str]]] = {k: [] for k in keys}
    for pno, text in enumerate(pages, start=1):
        for raw in text.splitlines():
            for key in keys:
                if key.lower() in raw.lower():
                    hits[key].append((pno, raw[:160]))
    return hits


def build_extract(pdf_path: Path, out_path: Path) -> None:
    size = pdf_path.stat().st_size
    if size < 1000:
        raise ValueError(
            f"PDF too small ({size} bytes): {pdf_path}\n"
            "OneDrive cloud-only placeholder? Right-click the file -> "
            "'Always keep on this device', then re-run."
        )

    doc = fitz.open(pdf_path)
    pages = [page_text(doc.load_page(i)) for i in range(doc.page_count)]
    sections = guess_sections(pages)
    hits = keyword_hits(pages)

    today = date.today().isoformat()
    lines: list[str] = [
        "# TesPro TR-400 User Manual — CCLI Engineering Extract",
        "",
        "**Document ID:** CCLI-HW-TR400-001",
        "**Revision:** 1.0",
        f"**Date:** {today}",
        "**RAG source_id:** `ccli-tr400-user-manual-extract`",
        "**PDF source_id:** `ccli-tr400-user-manual`",
        f"**Source:** `{pdf_path.name}` (v1.0 EN)",
        "**Corpus status:** **HAVE** — auto-extracted from OEM user manual",
        "**Programme link:** K1.2 · K1.6 · K3.1 · K5 · `ccli-tg500-lab-platform`",
        "",
        "---",
        "",
        "## Summary",
        "",
        f"TesPro **TR-400 series** user manual ingested from received hardware documentation "
        f"({doc.page_count} pages). This is the **product-facing OEM manual** for the lab DUT — "
        "use it to close **port map (K3.1)**, **DI/DO ratings (K5)**, **RS485 pinout (K5.1)**, "
        "and **SKU equivalence (K1.6)** vs frozen **TG-424 Pro / TG-524** programme names.",
        "",
        "### SKU naming (verify on device label)",
        "",
        "| Marketing / manual | CCLI frozen name | Status |",
        "|--------------------|------------------|--------|",
        "| **TR-400 series** (this manual) | TesPro industrial gateway line | **HAVE** manual |",
        "| TR-424 / TR-425 (web) | **TG-424 Pro** (supplier PO) | **OPEN** — confirm equivalence |",
        "| TG-524 / TG-525 (datasheet) | Product SKU in `ccli-soc-freeze` | **FROZEN** |",
        "",
        "---",
        "",
        "## Table of contents (detected headings)",
        "",
    ]

    if sections:
        lines.append("| Section | PDF page |")
        lines.append("|---------|----------|")
        for title, pno, _ in sections[:80]:
            safe = title.replace("|", "\\|")
            lines.append(f"| {safe} | {pno} |")
    else:
        lines.append("*(No headings auto-detected — see per-page dump below.)*")

    lines.extend(["", "---", "", "## Keyword index (auto-scan)", ""])
    for key, items in hits.items():
        if not items:
            continue
        lines.append(f"### `{key}`")
        for pno, snippet in items[:12]:
            lines.append(f"- p.{pno}: {snippet}")
        lines.append("")

    lines.extend(["---", "", "## Full text by page", ""])
    for pno, text in enumerate(pages, start=1):
        lines.append(f"### Page {pno}")
        lines.append("")
        lines.append("```text")
        lines.append(text if text else "(no text layer)")
        lines.append("```")
        lines.append("")

    lines.extend(
        [
            "---",
            "",
            "## CCLI requirements crosswalk (manual-driven)",
            "",
            "| ID | Topic | Manual section | Tree |",
            "|----|-------|----------------|------|",
            "| REQ-HW-003 | Ethernet WAN/LAN labels | TBD from manual | K3.1 |",
            "| REQ-HW-004 | RS485 A/B/G + baud | TBD from manual | K5.1 |",
            "| REQ-HW-005 | TPM / Secure Boot | TBD from manual | K6.1 |",
            "| REQ-IO-* | DI/DO count, voltage, pinout | TBD from manual | K5 |",
            "| REQ-VEND-002 | OpenWrt / TesproOS version | TBD from manual | K2 |",
            "",
            "---",
            "",
            "## Revision history",
            "",
            f"| Rev | Date | Notes |",
            f"|-----|------|-------|",
            f"| 1.0 | {today} | Auto-extract from `{pdf_path.name}` ({doc.page_count} pp.) |",
            "",
            "## Keywords",
            "",
            "`TR-400`, `TR400`, `TesPro`, `TG-424`, `TG-524`, `OpenWrt`, `TesproOS`, "
            "`RS485`, `DI`, `DO`, `TPM`, `ccli-tr400-user-manual`, `ccli-tr400-user-manual-extract`",
            "",
        ]
    )

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(lines), encoding="utf-8")
    print(f"Wrote {out_path} ({len(lines)} lines, {doc.page_count} pages)")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--pdf", type=Path, default=DEFAULT_PDF)
    parser.add_argument("--out", type=Path, default=DEFAULT_OUT)
    args = parser.parse_args()

    if not args.pdf.is_file():
        print(f"ERROR: PDF not found: {args.pdf}", file=sys.stderr)
        return 1
    try:
        build_extract(args.pdf, args.out)
    except ValueError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

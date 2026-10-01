#!/usr/bin/env python3
"""Extract competitor PDFs under competitors/competitors into _extracts/*.txt."""

from __future__ import annotations

import re
from pathlib import Path

from pypdf import PdfReader

REPO = Path(__file__).resolve().parents[1]
ROOT = REPO / "competitors" / "competitors"
OUT = REPO / "competitors" / "_extracts"

KEYWORDS = re.compile(
    r"ethernet|eth[_ ]?[ab]|lan|rs485|rs-485|modbus|61850|60870|"
    r"digital|opt[oi]|isolat|tpm|secure.?boot|gps|gnss|lte|"
    r"power|aliment|voltage|cpu|ram|emmc|i\.?mx|cortex|"
    r"porte|ingress|uscit|rel[eè]|10.?120|250\s*v|12.?24|"
    r"temperature|fibre|fiber|fx|bridge|firewall|switch|usb|serial|"
    r"iec\s*62443|cei\s*0-16|allegato|annex|di\b|do\b",
    re.I,
)


def main() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    for pdf in sorted(ROOT.rglob("*.pdf")):
        rel = pdf.relative_to(ROOT)
        reader = PdfReader(str(pdf))
        chunks: list[str] = []
        for i, page in enumerate(reader.pages, 1):
            try:
                t = page.extract_text() or ""
            except Exception as exc:  # noqa: BLE001
                t = f"[extract error: {exc}]"
            t = re.sub(r"[ \t]+", " ", t)
            chunks.append(f"\n----- PAGE {i} -----\n{t}")
        text = "\n".join(chunks)
        out = OUT / (str(rel).replace("\\", "_").replace("/", "_") + ".txt")
        out.write_text(text, encoding="utf-8", errors="replace")
        hits = [
            ln.strip()[:220]
            for ln in text.splitlines()
            if KEYWORDS.search(ln) and len(ln.strip()) > 8
        ]
        summary = f"{rel}: pages={len(reader.pages)} bytes={out.stat().st_size} hits={len(hits)}"
        print(summary.encode("ascii", "replace").decode("ascii"))
        hit_path = out.with_suffix(".hits.txt")
        hit_path.write_text("\n".join(hits[:120]), encoding="utf-8", errors="replace")
        print(f"  hits -> {hit_path.name}")
        print()


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Extract draw.io node text from Flowcharts/ into a RAG-searchable markdown corpus."""

from __future__ import annotations

import argparse
import html
import re
import xml.etree.ElementTree as ET
from datetime import date
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
FLOW = REPO / "Flowcharts"
DEFAULT_OUT = REPO / "knowledge-base" / "08-engineering" / "CCI_Flowcharts_Drawio_Extract.md"

ATTRS = ("mermaidBaseValue", "value", "label")


def _clean(text: str) -> str:
    if not text:
        return ""
    text = html.unescape(text)
    text = re.sub(r"<br\s*/?>", "\n", text, flags=re.I)
    text = re.sub(r"<[^>]+>", "", text)
    text = re.sub(r"\s+", " ", text).strip()
    return text


def extract_drawio(path: Path) -> list[str]:
    raw = path.read_text(encoding="utf-8", errors="replace")
    try:
        root = ET.fromstring(raw)
    except ET.ParseError:
        return []

    seen: set[str] = set()
    lines: list[str] = []
    for elem in root.iter():
        for attr in ATTRS:
            val = elem.attrib.get(attr)
            if not val:
                continue
            cleaned = _clean(val)
            if len(cleaned) < 4 or cleaned in seen:
                continue
            seen.add(cleaned)
            lines.append(cleaned)
    return lines


def build_corpus(flow_dir: Path) -> str:
    today = date.today().isoformat()
    parts = [
        "# CCI Flowcharts — draw.io text extract (RAG corpus)",
        "",
        "**RAG source_id:** `ccli-flowcharts-drawio-extract`  ",
        f"**Date:** {today}  ",
        "**Purpose:** Searchable text from regulation flowcharts in `Flowcharts/*.drawio`.  ",
        "**Master equations:** `Flowcharts/PARAMETERS_AND_EQUATIONS.md` · `ccli-flowcharts-params-equations`",
        "",
        "---",
        "",
    ]

    drawios = sorted(flow_dir.glob("*.drawio"))
    if not drawios:
        parts.append("_No draw.io files found._")
        return "\n".join(parts) + "\n"

    for path in drawios:
        nodes = extract_drawio(path)
        parts.append(f"## {path.name}")
        parts.append("")
        if not nodes:
            parts.append("_No extractable node text (draft or empty)._")
        else:
            for i, node in enumerate(nodes, 1):
                parts.append(f"{i}. {node}")
        parts.append("")
        parts.append("---")
        parts.append("")

    parts.append("## Keywords")
    parts.append("")
    parts.append(
        "flowchart, Wlim, WSd, W110, O.9.2.1, O.9.2.2, O.9.2.3, O.11, "
        "VArV, PFW, PFSP, VArSd, MSD, Q(V), cosphi, Smax, PF2, CEI 0-16, "
        "ccli-flowcharts-drawio-extract"
    )
    parts.append("")
    return "\n".join(parts)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--flow-dir", type=Path, default=FLOW)
    parser.add_argument("--out", type=Path, default=DEFAULT_OUT)
    args = parser.parse_args()

    corpus = build_corpus(args.flow_dir)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(corpus, encoding="utf-8")
    print(f"Wrote {args.out} ({len(corpus)} bytes, {len(list(args.flow_dir.glob('*.drawio')))} draw.io files)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

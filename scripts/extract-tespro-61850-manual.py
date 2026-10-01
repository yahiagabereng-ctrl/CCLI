#!/usr/bin/env python3
"""Extract TesPro IEC61850-User-Manual.docx into CCI_TesPro_61850_Manual_Extract.md."""

from __future__ import annotations

import argparse
import re
import sys
import xml.etree.ElementTree as ET
import zipfile
from datetime import date
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
DEFAULT_DOCX = (
    REPO
    / "knowledge-base"
    / "08-engineering"
    / "reference"
    / "vendor"
    / "tespro"
    / "IEC61850-User-Manual.docx"
)
DEFAULT_OUT = REPO / "knowledge-base" / "08-engineering" / "CCI_TesPro_61850_Manual_Extract.md"

W_NS = "http://schemas.openxmlformats.org/wordprocessingml/2006/main"
W_P = f"{{{W_NS}}}p"
W_T = f"{{{W_NS}}}t"

SECURITY_KEYWORDS = (
    "tls",
    "3782",
    "102",
    "security",
    "62351",
    "certificate",
    "cert",
    "auth",
    "aare",
    "aarq",
    "acse",
    "mms",
    "port",
    "ssl",
    "encrypt",
    "goose",
    "scl",
    "cid",
    "icd",
    "server",
    "client",
    "mqtt",
)


def normalize_line(line: str) -> str:
    line = line.replace("\uff5e", "~").replace("\u2013", "-").replace("\u2014", "-")
    line = line.replace("\u2019", "'").replace("\u2018", "'")
    line = re.sub(r"\s+", " ", line).strip()
    return line


def extract_paragraphs(docx_path: Path) -> list[str]:
    with zipfile.ZipFile(docx_path) as zf:
        root = ET.fromstring(zf.read("word/document.xml"))
    paras: list[str] = []
    for para in root.iter(W_P):
        texts = [node.text or "" for node in para.iter(W_T)]
        line = normalize_line("".join(texts))
        if line:
            paras.append(line)
    return paras


def is_heading(line: str) -> bool:
    return bool(re.match(r"^\d+(\.\d+)*\s+\S", line))


def build_sections(paras: list[str]) -> list[tuple[str, list[str]]]:
    sections: list[tuple[str, list[str]]] = []
    current_title = "Preamble"
    current_body: list[str] = []
    for line in paras:
        if is_heading(line):
            if current_body or current_title != "Preamble":
                sections.append((current_title, current_body))
            current_title = line
            current_body = []
        else:
            current_body.append(line)
    sections.append((current_title, current_body))
    return sections


def security_hits(paras: list[str]) -> list[tuple[int, str]]:
    hits: list[tuple[int, str]] = []
    for idx, line in enumerate(paras):
        lower = line.lower()
        if any(key in lower for key in SECURITY_KEYWORDS):
            hits.append((idx, line))
    return hits


def render_markdown(docx_path: Path, paras: list[str], sections: list[tuple[str, list[str]]]) -> str:
    hits = security_hits(paras)
    today = date.today().isoformat()
    rel = docx_path.relative_to(REPO).as_posix()
    lines: list[str] = [
        "# TesPro IEC 61850 Protocol Service — User Manual Extract",
        "",
        "**Document ID:** CCLI-VENDOR-TESPRO-61850-EXT-001",
        "**Revision:** 1.0",
        f"**Date:** {today}",
        "**RAG source_id:** `ccli-tespro-61850-manual-extract`",
        "**Source file:** `knowledge-base/08-engineering/reference/vendor/tespro/IEC61850-User-Manual.docx`",
        f"**DOCX source_id:** `ccli-tespro-61850-manual`",
        "**Parent:** `ccli-tespro-supplier-correspondence` · `ccli-tg500-lab-platform` · K4.1",
        "**Programme link:** Closes supplier item **V-005** — vendor IEC 61850 capability",
        "",
        "---",
        "",
        "## Summary",
        "",
        "TesPro **IEC 61850 Protocol Service** is a **northbound data-collection gateway feature**, not a",
        "CEI Allegato T **DSO-facing MMS server**. It uses **libiec61850** (`iec61850-mmsd` daemon) to",
        "**poll remote IEDs** (default MMS port **102**), collect points, and **upload JSON to MQTT/TCP**.",
        "TLS in this manual applies to **northbound MQTT/TCP only** — **not** 62351-4 secure MMS on Eth_A.",
        "",
        "**CCLI implication:** `apps/ccli` remains the DSO MMS server path. This manual does **not**",
        "document A-profile ACSE auth, port **3782**, or Annex T / `Wlim` control.",
        "",
        f"| Metric | Value |",
        f"|--------|-------|",
        f"| Paragraphs extracted | {len(paras)} |",
        f"| Source on disk | `{rel}` |",
        "",
        "---",
        "",
        "## Software packages (§1.0)",
        "",
        "| Package | Purpose |",
        "|---------|---------|",
        "| `libiec61850` | IEC 61850 / MMS protocol library |",
        "| `libopen62541` | Supporting library |",
        "| `iec61850-mmsd` | IEC 61850 MMS access **daemon** |",
        "| `iec61850-proto-tespro-combined` | Web UI + collection + upload service |",
        "",
        "Delivered as **add-on opkg packages** with TesproOS firmware.",
        "",
        "---",
        "",
        "## Architecture (inferred)",
        "",
        "```text",
        "Remote IED(s)  --MMS/TCP:102-->  iec61850-mmsd (on TG544)",
        "                                      |",
        "                                      v",
        "                              collect / RCB / points",
        "                                      |",
        "                                      v",
        "                         MQTT or TCP northbound (optional TLS)",
        "```",
        "",
        "**Not described:** MMS **server** on Eth_A, DSO client connect, 62351-3 TLS on 3782,",
        "62351-4 A-profile AARQ/AARE certificate authentication.",
        "",
        "---",
        "",
        "## Security / TLS findings (keyword scan)",
        "",
    ]
    if hits:
        lines.append("| # | Extract |")
        lines.append("|---|---------|")
        for idx, line in hits:
            safe = line.replace("|", "\\|")
            if len(safe) > 180:
                safe = safe[:177] + "..."
            lines.append(f"| {idx} | {safe} |")
    else:
        lines.append("_No security keywords found._")

    lines.extend(
        [
            "",
            "---",
            "",
            "## Full section extract",
            "",
        ]
    )
    for title, body in sections:
        lines.append(f"### {title}")
        lines.append("")
        for para in body:
            if para.endswith(":") and len(para) < 80:
                lines.append(f"**{para}**")
            elif re.match(r"^(Tip|Note):", para, re.I):
                lines.append(f"> {para}")
            else:
                lines.append(para)
            lines.append("")
        lines.append("")

    lines.extend(
        [
            "---",
            "",
            "## Requirements traceability",
            "",
            "| ID | Requirement | Manual evidence | Status |",
            "|----|-------------|-----------------|--------|",
            "| REQ-VEND-006 | CEI / Allegato T DSO MMS server | Not covered — collector only | **NOT MET** |",
            "| REQ-3514-TPROF | 62351-3 TLS MMS port 3782 | Not mentioned | **MISSING** |",
            "| REQ-3514-APROF | 62351-4 ACSE certificate auth | Not mentioned | **MISSING** |",
            "| REQ-61850-001 | MMS server on Eth_A (CCLI) | Different product path | **N/A** — use `apps/ccli` |",
            "| REQ-VEND-005 | Vendor 61850 stack identity | libiec61850 + mmsd + web UI | **HAVE** |",
            "",
            "---",
            "",
            "## Knowledge gaps",
            "",
            "| Area | Current | Required | Gap |",
            "|------|---------|----------|-----|",
            "| DSO MMS server | Not in manual | Eth_A :3782 TLS + Allegato T | **CCLI owns** |",
            "| 62351-4 A-profile | Not in manual | AARQ/AARE auth | **CCLI owns** |",
            "| Northbound MQTT TLS | Documented | Optional upload path | **HAVE** (out of CCI scope) |",
            "| Coexistence with `ccli` | Not documented | Port conflict policy | **OPEN** — ask TesPro |",
            "",
            "---",
            "",
            "## Risks",
            "",
            "| ID | Description | Impact | Mitigation |",
            "|----|-------------|--------|------------|",
            "| R-VEND-003 | Vendor IEC 61850 ≠ CEI Allegato T | False cert confidence | Keep libiec61850 in `apps/ccli` |",
            "| R-TG-001 | Manual describes **client/collector** role | Team assumes TesPro covers DSO | This extract + supplier V-005 close |",
            "",
            "---",
            "",
            "## Verification",
            "",
            "| Check | Pass criteria |",
            "|-------|---------------|",
            "| Packages on DUT | `opkg list-installed \\| grep iec61850` shows mmsd + proto service |",
            "| Web UI | Services → IEC 61850 Protocol page loads |",
            "| vs CCLI | `ccli` on :3782 can run without mmsd binding same Eth_A port |",
            "",
            "---",
            "",
            "## Keywords",
            "",
            "`TesPro`, `iec61850-mmsd`, `libiec61850`, `northbound`, `MQTT`, `MMS client`,",
            "`ccli-tespro-61850-manual`, `ccli-tespro-61850-manual-extract`, `V-005`",
            "",
        ]
    )
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--docx", type=Path, default=DEFAULT_DOCX)
    parser.add_argument("--out", type=Path, default=DEFAULT_OUT)
    args = parser.parse_args()
    if not args.docx.is_file():
        print(f"DOCX not found: {args.docx}", file=sys.stderr)
        return 1
    paras = extract_paragraphs(args.docx)
    sections = build_sections(paras)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(render_markdown(args.docx, paras, sections), encoding="utf-8")
    print(f"Wrote {args.out} ({len(paras)} paragraphs, {len(sections)} sections)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""Extract Chronos EMT432 corpus from vendor PDFs into CCI_Chronos_EMT432_Extract.md."""

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
REF = REPO / "knowledge-base" / "08-engineering" / "reference" / "vendor" / "chronos"
DEFAULT_OUT = REPO / "knowledge-base" / "08-engineering" / "CCI_Chronos_EMT432_Extract.md"
MAP_OUT = REPO / "apps" / "ccli" / "config" / "modbus" / "chronos_emt432_map.yaml"

STATIC_BASE = "https://static.chronos-tech.it"
PDF_URLS = {
    "EMT432_MI-ENG.pdf": f"{STATIC_BASE}/EMT_432_MI_ENG_3d382e57d1.pdf",
    "EMT432_MI-ITA.pdf": f"{STATIC_BASE}/EMT_432_MI_ITA_9fcf8aaff7.pdf",
    "EMT432_FL-ITA.pdf": f"{STATIC_BASE}/EMT_432_FL_ITA_8f0283606e.pdf",
    "EMT430_MR.pdf": f"{STATIC_BASE}/EMT_430_MR_cb7f1d080e.pdf",
}

KEY_REGISTERS = (
    "P_SUM",
    "Q_SUM",
    "P1",
    "Q1",
    "Frequency",
    "V_L1_L2_peak",
    "V_L1_N_peak",
    "modbus_RTU_address",
    "modbus_RTU_baudrate",
    "modbus_RTU_parity",
    "modbus_RTU_Stop_bit",
    "Measurement_Configuration_Register",
    "feature_version",
    "mounted_version",
)

# Curated from EMT430_MR Rev2 page 6 (parser misses multi-line descriptions).
CURATED_REGISTERS: dict[str, dict[str, str]] = {
    "P1": {
        "name": "P1",
        "description": "RMS active power line 1 [W]",
        "type": "float",
        "rw": "RO",
        "default": "0",
        "address": "40933",
        "offset": "932",
    },
    "P_SUM": {
        "name": "P_SUM",
        "description": "RMS sum active power [W]",
        "type": "float",
        "rw": "RO",
        "default": "0",
        "address": "40939",
        "offset": "938",
    },
    "Q1": {
        "name": "Q1",
        "description": "RMS reactive power line 1 [VAR]",
        "type": "float",
        "rw": "RO",
        "default": "0",
        "address": "40941",
        "offset": "940",
    },
    "Q_SUM": {
        "name": "Q_SUM",
        "description": "RMS sum reactive power [VAR]",
        "type": "float",
        "rw": "RO",
        "default": "0",
        "address": "40947",
        "offset": "946",
    },
    "Frequency": {
        "name": "Frequency",
        "description": "Frequency [Hz]",
        "type": "float",
        "rw": "RO",
        "default": "0",
        "address": "40973",
        "offset": "972",
    },
    "V_L1_L2_peak": {
        "name": "V_L1_L2_peak",
        "description": "Line voltage L1-L2 peak [V]",
        "type": "float",
        "rw": "R/W",
        "default": "0",
        "address": "40981",
        "offset": "980",
    },
    "V_L1_N_peak": {
        "name": "V_L1_N_peak",
        "description": "Star voltage L1-N peak [V]",
        "type": "float",
        "rw": "R/W",
        "default": "0",
        "address": "40975",
        "offset": "974",
    },
    "modbus_RTU_address": {
        "name": "modbus_RTU_address",
        "description": "Modbus RTU slave address",
        "type": "unsigned short",
        "rw": "R/W",
        "default": "1",
        "address": "40227",
        "offset": "226",
    },
    "modbus_RTU_baudrate": {
        "name": "modbus_RTU_baudrate",
        "description": "Baudrate code 0=1200..7=115200",
        "type": "unsigned short",
        "rw": "R/W",
        "default": "3",
        "address": "40229",
        "offset": "228",
    },
    "modbus_RTU_parity": {
        "name": "modbus_RTU_parity",
        "description": "0=NONE 1=EVEN",
        "type": "unsigned short",
        "rw": "R/W",
        "default": "0",
        "address": "40230",
        "offset": "229",
    },
    "modbus_RTU_Stop_bit": {
        "name": "modbus_RTU_Stop_bit",
        "description": "Stop bit code",
        "type": "unsigned short",
        "rw": "R/W",
        "default": "0",
        "address": "40231",
        "offset": "230",
    },
    "Measurement_Configuration_Register": {
        "name": "Measurement_Configuration_Register",
        "description": "Connection mode bits 1-2: 0=single phase",
        "type": "unsigned short",
        "rw": "R/W",
        "default": "518",
        "address": "40232",
        "offset": "231",
    },
    "feature_version": {
        "name": "feature_version",
        "description": "0=Base 1=Full",
        "type": "unsigned short",
        "rw": "RO",
        "default": "0",
        "address": "40015",
        "offset": "14",
    },
    "mounted_version": {
        "name": "mounted_version",
        "description": "0=5A 1=Rogowski/333mV",
        "type": "unsigned short",
        "rw": "RO",
        "default": "0",
        "address": "40011",
        "offset": "10",
    },
}


def merge_curated(regs: list[dict[str, str]]) -> list[dict[str, str]]:
    by_name = {r["name"]: r for r in regs}
    by_name.update(CURATED_REGISTERS)
    return list(by_name.values())


def pdf_text(path: Path) -> str:
    doc = fitz.open(path)
    return "\n".join(page.get_text("text") for page in doc)


def parse_modbus_registers(mr_text: str) -> list[dict[str, str]]:
    lines = [ln.strip() for ln in mr_text.splitlines()]
    regs: list[dict[str, str]] = []
    i = 0
    while i < len(lines):
        if lines[i] == "Modbus" and i + 1 < len(lines) and lines[i + 1] == "Address":
            i += 2
            continue
        if i + 6 < len(lines) and re.fullmatch(r"40\d{3}", lines[i + 6]):
            name, desc, reg_type, rw, default, addr = (
                lines[i],
                lines[i + 1],
                lines[i + 2],
                lines[i + 3],
                lines[i + 4],
                lines[i + 6],
            )
            if name and not name.startswith("EMT"):
                regs.append(
                    {
                        "name": name,
                        "description": desc,
                        "type": reg_type,
                        "rw": rw,
                        "default": default,
                        "address": addr,
                        "offset": str(int(addr) - 40001),
                    }
                )
            i += 7
            continue
        i += 1
    return regs


def extract_mi_sections(mi_text: str) -> dict[str, str]:
    sections: dict[str, str] = {}
    current = "header"
    buf: list[str] = []
    for line in mi_text.splitlines():
        upper = line.strip().upper()
        if upper in {
            "INTRODUCTION",
            "MEASUREMENTS AND LOGGING DATA",
            "CONNECTION DIAGRAMS",
            "SCHEMI DI COLLEGAMENTO",
            "FIRST CONNECTION",
            "PRIMA CONNESSIONE",
            "ORDER CODES",
            "CODICI D’ORDINE",
            "CODICI D'ORDINE",
        }:
            if buf:
                sections[current] = "\n".join(buf).strip()
            current = upper
            buf = []
            continue
        buf.append(line)
    if buf:
        sections[current] = "\n".join(buf).strip()
    return sections


def write_modbus_yaml(regs: list[dict[str, str]], path: Path) -> None:
    by_name = {r["name"]: r for r in merge_curated(regs)}

    def reg(name: str) -> dict[str, str] | None:
        return by_name.get(name)

    p = reg("P_SUM")
    q = reg("Q_SUM")
    p1 = reg("P1")
    q1 = reg("Q1")
    vll = reg("V_L1_L2_peak")
    freq = reg("Frequency")

    lines = [
        "# Chronos EMT432 / EMT430 family — Modbus RTU map (from EMT430_MR Rev2)",
        "# PDF: knowledge-base/08-engineering/reference/vendor/chronos/EMT430_MR.pdf",
        "# Addresses are PLC 4xxxx; ccli reg_* uses 0-based offset from 40001.",
        "schema: ccli-modbus-map/v1",
        "device: chronos_emt432",
        "vendor: Chronos srl",
        "product_url: https://chronos-tech.it/prodotti/chronos/emt432/",
        "notes:",
        "  - Default RTU: 9600 8N1, slave ID 1 (reg modbus_RTU_baudrate=3, parity=0).",
        "  - Float32 values use 2 consecutive holding registers (big-endian per bench verify).",
        "  - P/Q in W and VAR — scale 0.001 for kW/kvar in ccli if needed.",
        "  - Order code suffix I = TA 5A; V = 333mV/Rogowski.",
        "",
        "registers:",
    ]

    def add_block(key: str, r: dict[str, str] | None, unit: str, scale: str) -> None:
        if not r:
            return
        lines.extend(
            [
                f"  {key}:",
                f"    name: {r['name']}",
                f"    address: {r['address']}",
                f"    offset_40001: {r['offset']}",
                "    type: holding",
                "    count: 2",
                "    datatype: float32",
                "    byte_order: big_endian",
                f"    scale: {scale}",
                f"    unit: {unit}",
                f"    description: \"{r['description']}\"",
                "",
            ]
        )

    add_block("active_power_w", p, "W", "0.001")
    add_block("reactive_power_var", q, "VAR", "0.001")
    add_block("active_power_l1_w", p1, "W", "0.001")
    add_block("reactive_power_l1_var", q1, "VAR", "0.001")
    add_block("line_voltage_l1_l2_peak_v", vll, "V", "0.001")
    add_block("frequency_hz", freq, "Hz", "1.0")

    comm = [
        ("modbus_slave_id", reg("modbus_RTU_address")),
        ("modbus_baud_code", reg("modbus_RTU_baudrate")),
        ("modbus_parity_code", reg("modbus_RTU_parity")),
        ("modbus_stop_bit_code", reg("modbus_RTU_Stop_bit")),
    ]
    lines.append("  comm_config_holding:")
    for key, r in comm:
        if r:
            lines.append(f"    {key}: {{ address: {r['address']}, offset_40001: {r['offset']} }}")

    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def build_markdown(
    regs: list[dict[str, str]], mi_eng: str, mi_ita: str, fl_ita: str
) -> str:
    sections_eng = extract_mi_sections(mi_eng)
    merged = merge_curated(regs)
    by_name = {r["name"]: r for r in merged}
    key_rows = [by_name[n] for n in KEY_REGISTERS if n in by_name]

    baud_map = {
        "0": "1200",
        "1": "2400",
        "2": "4800",
        "3": "9600",
        "4": "19200",
        "5": "38400",
        "6": "57600",
        "7": "115200",
    }

    lines = [
        "# Chronos EMT432 — engineering extract",
        "",
        f"**Document ID:** CCLI-CHRONOS-EMT432-001  ",
        f"**Date:** {date.today().isoformat()}  ",
        "**RAG source_id:** `ccli-chronos-emt432`  ",
        "**Vendor:** [Chronos srl](https://chronos-tech.it/) · Selvazzano Dentro (PD), Italy  ",
        "**Product:** EMT432 three-phase network analyzer (Base / Full)  ",
        "**CCLI track:** P5-06 meter map · P5-04 accuracy · POC Modbus on TG544 A1/B1",
        "",
        "---",
        "",
        "## Corpus files (HAVE)",
        "",
        "| File | Role |",
        "|------|------|",
        "| `reference/vendor/chronos/EMT432_MI-ENG.pdf` | Installation manual EN rev 2 |",
        "| `reference/vendor/chronos/EMT432_MI-ITA.pdf` | Installation manual IT rev 2 (wiring) |",
        "| `reference/vendor/chronos/EMT432_FL-ITA.pdf` | Product flyer IT |",
        "| `reference/vendor/chronos/EMT430_MR.pdf` | Modbus register map Rev2 (EMT430/432 family) |",
        "| `apps/ccli/config/modbus/chronos_emt432_map.yaml` | CCLI register YAML (generated) |",
        "",
        "**Download URLs (static CDN):**",
        "",
    ]
    for name, url in PDF_URLS.items():
        lines.append(f"- `{name}` → {url}")

    lines.extend(
        [
            "",
            "---",
            "",
            "## Product summary",
            "",
            "| Item | Value |",
            "|------|-------|",
            "| Aux supply | 10–36 V DC or 13–26 V AC |",
            "| VT range L-N | 85–265 V AC |",
            "| VT range L-L | 150–450 V AC |",
            "| Current inputs | **I** = TA 5 A · **V** = 333 mV / Rogowski (internal integrator) |",
            "| Comms | RS485 Modbus RTU · Ethernet/WiFi Modbus TCP · NFC |",
            "| Accuracy (vendor) | Class 0.2S EN 62053-22 · ±0.1% rdg P @ 25°C |",
            "| Class II | **No PE bond** on instrument (EN 61140) |",
            "",
            "### Order code",
            "",
            "`EMT432` + `I` (5 A TA) or `V` (Rogowski/333 mV) + `BASE` or `FULL`.",
            "",
            "---",
            "",
            "## Modbus RTU defaults (EMT430_MR Rev2)",
            "",
            "| Parameter | Register | Default | CCLI lab note |",
            "|-----------|----------|---------|---------------|",
            "| Slave address | 40227 (`modbus_RTU_address`) | **1** | Match `modbus.slave_id` |",
            "| Baud code | 40229 | **3** → **9600** | Match `modbus.baud` |",
            "| Parity code | 40230 | **0** → NONE | Match `modbus.parity: N` |",
            "| Stop bits | 40231 | **0** → 1 bit | 8N1 |",
            "",
            "Baud code map: "
            + ", ".join(f"{k}={v}" for k, v in baud_map.items()),
            "",
            "### Measurement configuration (40232)",
            "",
            "Bits 1–2 connection mode:",
            "",
            "- `0` → **Single phase**",
            "- `1` → 3P 3-wire 2 TA (Aron)",
            "- `2` → 3P 3-wire 3 TA",
            "- `3` → 3P 4-wire 3 TA + N",
            "",
            "For **230 V 1P bench**: set single-phase + wire L1/N + one TA on I1.",
            "",
            "---",
            "",
            "## Key realtime registers (float32, 2 regs, RO)",
            "",
            "| Register | Modbus addr | Offset @40001 | Description | CCLI map |",
            "|----------|-------------|---------------|-------------|----------|",
        ]
    )

    ccli_map = {
        "P_SUM": "PdCMMXU1.TotW (kW, scale 0.001)",
        "Q_SUM": "PdCMMXU1.TotVAr (kvar, scale 0.001)",
        "P1": "1P bench TotW",
        "Q1": "1P bench TotVAr",
        "V_L1_L2_peak": "PPV proxy (verify vs RMS avg regs)",
        "Frequency": "Grid frequency",
    }
    for r in key_rows:
        ccli = ccli_map.get(r["name"], "—")
        lines.append(
            f"| `{r['name']}` | {r['address']} | {r['offset']} | {r['description'][:50]} | {ccli} |"
        )

    lines.extend(
        [
            "",
            "> **Verify on bench:** float byte order and whether libmodbus uses address `offset` or `address-1`.",
            "",
            "---",
            "",
            "## Lab wiring — 230 V monofase (no Rogowski)",
            "",
            "From MI-ITA § schemi di collegamento:",
            "",
            "- **Monofase, 2 fili, connessione con 1 TA** — L, N, I1 on one CT.",
            "- RS485: terminals **A+ / B- / GND** (3.5 mm 3-pole); multi-drop A/B/G.",
            "- **Class II — do not bond instrument PE**; avoid N–PE current.",
            "",
            "### CCLI bench topology",
            "",
            "```text",
            "230 V L+N → EMT432 L1 + N (+ TA 5A on I1 when available)",
            "RS485 A/B/G → TG544 A1/B1 (/dev/ttyS1)",
            "Optional: COM5 pymodbus slave remains plant Q command path (FC16)",
            "```",
            "",
            "---",
            "",
            "## Introduction excerpt (MI-ENG)",
            "",
            "```text",
        ]
    )
    intro = sections_eng.get("INTRODUCTION", mi_eng[:1500])
    lines.append(intro[:2000])
    lines.extend(["```", "", "---", "", "## Verification (P5-06)", ""])
    lines.extend(
        [
            "| Step | Pass |",
            "|------|------|",
            "| Read `machine_id_*` @ 40001 → spells EMT430/432 | Identity |",
            "| `feature_version` @ 40015 = 0 Base / 1 Full | SKU |",
            "| `P_SUM`/`Q_SUM` track load on 1P+CT | Metrology |",
            "| ccli `modbus: FC3` log @ 4 s | P5-02/P5-M07 |",
            "",
            f"**Total registers parsed from MR PDF:** {len(regs)}",
            "",
        ]
    )
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--ref-dir", type=Path, default=REF)
    parser.add_argument("--out", type=Path, default=DEFAULT_OUT)
    parser.add_argument("--map-out", type=Path, default=MAP_OUT)
    args = parser.parse_args()

    ref = args.ref_dir
    mr = ref / "EMT430_MR.pdf"
    mi_eng = ref / "EMT432_MI-ENG.pdf"
    mi_ita = ref / "EMT432_MI-ITA.pdf"
    fl_ita = ref / "EMT432_FL-ITA.pdf"

    for p in (mr, mi_eng, mi_ita, fl_ita):
        if not p.is_file() or p.read_bytes()[:4] != b"%PDF":
            print(f"Missing or invalid PDF: {p}", file=sys.stderr)
            return 1

    regs = merge_curated(parse_modbus_registers(pdf_text(mr)))
    md = build_markdown(regs, pdf_text(mi_eng), pdf_text(mi_ita), pdf_text(fl_ita))
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(md, encoding="utf-8")
    write_modbus_yaml(regs, args.map_out)
    print(f"Wrote {args.out}")
    print(f"Wrote {args.map_out} ({len(regs)} registers parsed)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

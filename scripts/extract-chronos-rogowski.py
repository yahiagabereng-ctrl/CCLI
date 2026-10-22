#!/usr/bin/env python3
"""Build Rogowski / TA corpus extract from Chronos TA_FL_ITA.pdf + EMT432 cross-ref."""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

try:
    import pypdf
except ImportError:
    print("Install: pip install pypdf", file=sys.stderr)
    sys.exit(1)

REPO = Path(__file__).resolve().parents[1]
DEFAULT_REF = REPO / "knowledge-base" / "08-engineering" / "reference" / "vendor" / "chronos"
OUT = REPO / "knowledge-base" / "08-engineering" / "CCI_Chronos_Rogowski_Bench_Extract.md"


def extract_frc_section(text: str) -> str:
    idx = text.find("serie FRC")
    if idx < 0:
        idx = text.find("FRC-350")
    if idx < 0:
        return ""
    chunk = text[idx : idx + 3500]
    return chunk.strip()


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--ref-dir", type=Path, default=DEFAULT_REF)
    ap.add_argument("--out", type=Path, default=OUT)
    args = ap.parse_args()

    pdf = args.ref_dir / "TA_FL_ITA.pdf"
    if not pdf.is_file():
        print(f"Missing {pdf} — run ingest-chronos-emt432.ps1 first", file=sys.stderr)
        return 1

    reader = pypdf.PdfReader(str(pdf))
    full = "\n".join((p.extract_text() or "") for p in reader.pages)
    frc = extract_frc_section(full)

    body = f"""# Chronos Rogowski (FRC) + EMT430/432 bench extract

**Document ID:** CCLI-CHRONOS-ROGOWSKI-001  
**Date:** 2026-10-21  
**RAG source_id:** `ccli-chronos-rogowski-bench`  
**Vendor:** [Chronos srl](https://chronos-tech.it/)  
**Parents:** `ccli-chronos-emt432` · `EMT430_MR.pdf` · `TA_FL_ITA.pdf`

---

## Corpus files (HAVE)

| File | Role |
|------|------|
| `reference/vendor/chronos/TA_FL_ITA.pdf` | CT / Rogowski catalog IT (FRC-350, KCT, CP, …) |
| `reference/vendor/chronos/EMT430_MR.pdf` | Modbus register map (same for TCP/RTU) |
| `apps/ccli/config/modbus/chronos_emt432_map.yaml` | CCLI register YAML |
| `apps/ccli/config/lab_tr400_emt432_lan3_tcp_102.yaml` | Bench overlay — meter **192.168.178.102** |

**Download (TA catalog):** https://static.chronos-tech.it/TA_FL_ITA_cb6650f79f.pdf

---

## Product pairing (Rogowski)

| Item | Requirement |
|------|-------------|
| Meter order code | **V** suffix = **333 mV / Rogowski** + **internal integrator** |
| Coil | e.g. **FRC-350** (350 mm spira) — catalog **TA_FL_ITA** |
| Wrong pairing | **FRC on I (5 A CT) meter** → no valid I/P |
| Verify on Modbus | `mounted_version` @ **40011** → **1** = Rogowski/333 mV |

EMT register and scaling details: [CCI_Chronos_EMT432_Extract.md](CCI_Chronos_EMT432_Extract.md).

---

## FRC series excerpt (from TA_FL_ITA)

```text
{frc if frc else "(FRC section not parsed — open TA_FL_ITA.pdf page Rogowski)"}
```

---

## Modbus registers (ccli — unchanged for Rogowski)

Offsets from **40001** (holding registers). Float32 × 2, **`float_word_swap: true`** on bench.

| Use | Register | Offset | Maps to ccli / MMS |
|-----|----------|--------|---------------------|
| P (1P) | P1 | 932 | TotW (×0.001 → kW) |
| Q (1P) | Q1 | 940 | TotVAr |
| V L1-N RMS | V_L1_N_rms | 906 | PPV (×0.001 → kV) |
| Connection mode | Measurement_Configuration_Register | 40232 (off 231) | bits 1–2: **0** = single phase |
| Input type | mounted_version | 40011 (off 10) | **1** = Rogowski |

TCP: **502**, unit **1**. Plant LAN3 example: meter **192.168.178.102/24**, GW **192.168.178.1** (DUT).

---

## Wiring checklist (1P + FRC-350)

1. **Pow+ / Pow−** = 18–36 V DC aux only (never mains on aux).
2. **L1 + N** for 1P; L2/L3 open.
3. **FRC-350** around conductor on L1 path; leads to meter **Rogowski / I1** inputs ( **V** meter — see EMT432 **MI-ITA** monofase diagram).
4. **Ethernet** → LAN3; set IP/GW on meter (UI / app / NFC).
5. PC plant NIC: `lab/set_pc_lan3_plant_178.cmd` → **192.168.178.10/24**.

---

## Verification

| Step | Command / check | Pass |
|------|-----------------|------|
| TCP probe | `python lab/modbus_emt432_tcp_probe.py --host 192.168.178.102` | connect OK · **333mV/Rog** · P1 plausible |
| DUT deploy | `lab_tr400_emt432_lan3_tcp_102.yaml` → `/etc/ccli/lab.yaml` | `grep host` → .102 |
| Bench script | `validate-bench-hw-configuration.ps1 -MeterHost 192.168.178.102 -ConfigYaml lab_tr400_emt432_lan3_tcp_102.yaml -ProbeMeter` | PASS |

**Wiring doc:** [lab/evidence/phase5/P5_ROGOWSKI_FRC350_BENCH.md](../../lab/evidence/phase5/P5_ROGOWSKI_FRC350_BENCH.md)

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-21 | Initial FRC-350 + .102 bench overlay |
"""
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(body, encoding="utf-8")
    print(f"Wrote {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

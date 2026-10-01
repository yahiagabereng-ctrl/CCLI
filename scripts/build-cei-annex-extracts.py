#!/usr/bin/env python3
"""Build English Annex O/T extracts from consolidated CEI 0-16 corpus."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ARCH = ROOT / "Architecture" / "_extracted_reg_analysis"
KB = ROOT / "knowledge-base" / "08-engineering"


def strip_legacy_header(lines: list[str]) -> list[str]:
    skip_prefixes = (
        "AiLux",
        "Extracted and consolidated",
        "Source document:",
        "Status:",
        "Mandatory Measurements",
        "Cybersecurity, IEC 61850",
    )
    while lines and (
        not lines[0].strip()
        or any(lines[0].startswith(p) for p in skip_prefixes)
    ):
        lines.pop(0)
    return lines


def main() -> None:
    o_body = (ARCH / "annex_o.txt").read_text(encoding="utf-8")
    t_body = (ARCH / "annex_t.txt").read_text(encoding="utf-8")

    o_core = "\n".join(strip_legacy_header(o_body.splitlines()))
    t_core = "\n".join(strip_legacy_header(t_body.splitlines()))

    o_header = """# CEI 0-16 Annex O — CCI Engineering Extract (English)

**Document ID:** CCLI-GRID-ANNEX-O-001  
**Revision:** 2.0  
**Date:** 2026-09-16  
**RAG source_id:** `cei-0-16-allegato-o`  
**Normative source:** CEI 0-16 consolidated working edition **2025-12** (`knowledge-base/07-protocols/0-16consolidata.pdf`, 691 pp.) — **Allegato O** (normative), doc. pp. 473–533 / PDF pp. 480–540  
**Prior English baseline:** CT 316 extract (2024-12); superseded for **Variant V5 (2025)** items below  
**Companion:** `CCI_Annex_T_Extract.md` · `CCI_CEI_0-16_Consolidated.md`  
**Programme link:** K7.1 · REQ-PF2-* · REQ-MET-* · REQ-IO-* (functional; no pin count)

---

## Summary

Annex O defines the **Central Plant Controller (CCI)** functional specification: PF1 observability, PF2 voltage/active-power control at the delivery point (POC/PdC), PF3 dispatch-market functions, control-loop timing, measurement accuracy, communication interfaces, data logger, and certification hooks.

**Corpus status:** **HAVE (English)** for core numerical requirements; **UPDATED** for **CEI 0-16 V5 (2025)** / **ARERA 385/2025/R/EEL** (see §2.1). Annex T covers IEC 61850 data model and cyber.

---

## 2.1 Variant V5 (2025) — ARERA 385/2025/R/EEL deltas *(English)*

Source: consolidated PDF preface (V5) + Allegato O clauses O.2, O.6, O.8, O.9, O.11, O.12 (Italian → English engineering summary).

| Topic | Prior baseline (2024 extract) | **V5 consolidated (2025-12)** |
|-------|------------------------------|-------------------------------|
| Regulatory driver | ARERA 36/2020/R/EEL, SOGL | **+ ARERA 385/2025/R/EEL** (Grid Code All. A.72 alignment, DER security) |
| PF2 active-power limitation on **DSO command** | PF2 class described as *optional* (ARERA timing TBD) | **Mandatory** for **wind and PV** plants with rated power **≥ 100 kW** (O.6 / O.9) |
| Aggregated P per source (O.8) | Required per generator thresholds | **Not mandatory** for wind/PV **100 kW ≤ P < 500 kW** where consumption is **auxiliaries only** |
| Measurement accuracy (O.13.2) | Class ≤ 0.2 % typical | For wind/PV **100–500 kW**, Grid Code **All. A.72** allows error **≤ 5 %** |
| Defence plan / Annex M | Referenced | **Annex M still applies ≥ 100 kW** until a later regulatory variant (385/2025 transitional note) |
| Priority O.11 | Teletrip + DSO limit mentioned | **Remote disconnect (teledistacco)** and **DSO active-power limitation** retain top priority below unit O/F regulation |
| Installation (O.12) | — | Example diagrams show **teledistacco predisposition** (CEI 0-16 §8.8.7.1) — physical connections to CCI |

**Applicability (O.2 — unchanged thresholds, clarified scope):**

- CCI mandatory for new MT connections when aggregated wind/PV per source **≥ 100 kW**, or dispatching participation, or **≥ 1000 kW** total (standard perimeter).
- When a CCI is installed, its functions apply to **all production units at the same delivery point (PdC)**, regardless of individual unit size.

**Not in this PDF:** product pin counts (5 DI / 3 DO) — those remain **AiLux product class**, not Annex O text.

---

"""

    t_header = """# CEI 0-16 Annex T — IEC 61850 / Cyber Engineering Extract (English)

**Document ID:** CCLI-GRID-ANNEX-T-001  
**Revision:** 2.0  
**Date:** 2026-09-16  
**RAG source_id:** `cei-0-16-allegato-t`  
**Normative source:** CEI 0-16 consolidated working edition **2025-12** (`knowledge-base/07-protocols/0-16consolidata.pdf`) — **Allegato T** (normative), doc. pp. 534–653 / PDF pp. 541–660  
**Prior English baseline:** CT 316 / CT 57 extract (2024-12)  
**Companion:** `CCI_Annex_O_Extract.md` · `CCI_CEI_0-16_Consolidated.md`  
**Programme link:** K7.2 · K4.3 ICD · REQ-61850-001

---

## Summary

Annex T specifies the **CCI–DSO (and Aggregator) IEC 61850 interface**: MMS timing (Type 3 ≈ 500 ms), logical device/node/data-object model, ACSI services, RBAC, PKI/TLS cyber profile, and logging. Plant-internal communication is **out of scope**.

**Corpus status:** **HAVE (English)** for normative body. **SCL/CID example is NOT inside the consolidated PDF** — see §1.1.

---

## 1.1 SCL / ICD reference — **CEI TR 57-126** *(INGESTED)*

Annex T §T.3 states (consolidated PDF, doc. p. 535):

> For the concrete implementation of the CCI communication mode, reference may be made to **CEI TR 57-126** — *"Example of SCL file for IEC 61850 communication of the CCI"* (*Esempio di file SCL per la comunicazione IEC 61850 del CCI*).

| Item | Status |
|------|--------|
| `0-16consolidata.pdf` | **ON DISK** — contains Annex T tables and cyber spec |
| **CEI TR 57-126** (SCL/CID example) | **INGESTED** — `source_id`: `cei-tr-57-126` → `CCI_TR_57-126_Extract.md` |
| Example CID | **ON DISK** — `apps/ccli/config/icd/cei-tr-57-126-example.cid` |

Also introduced in **Variant V1 (2022-11)**: Allegato T explicitly references this TR (preface, consolidated doc. p. 3).

---

"""

    o_core = o_core.replace(
        "PF2 — Optional: voltage control and active power limitation at the POC (required in the manner/timeframe set by ARERA; already implemented as real functions in the CCI AiLux product).",
        "PF2 — Voltage control and active power limitation at the POC: **mandatory PF2 sub-function** (DSO active-power limitation) for wind/PV **≥ 100 kW** per V5/ARERA 385/2025; other PF2 functions remain ARERA-scheduled optional unless mandated.",
    )

    t_core = t_core.replace(
        'Remaining gap: Annex T references a separate Technical Report "Example of SCL file for the IEC 61850 communication of CCI" for the concrete implementation of the CCI communication mode (§T.3.1). This SCL example has not been supplied and should be requested before finalising the IED configuration.',
        "**SCL/CID baseline:** **CEI TR 57-126** ingested — `CCI_TR_57-126_Extract.md` + `apps/ccli/config/icd/cei-tr-57-126-example.cid`. DSO-specific workbook may still differ from the reference CID.",
    )

    (KB / "CCI_Annex_O_Extract.md").write_text(o_header + o_core + "\n", encoding="utf-8")
    (KB / "CCI_Annex_T_Extract.md").write_text(t_header + t_core + "\n", encoding="utf-8")
    print("Wrote", KB / "CCI_Annex_O_Extract.md")
    print("Wrote", KB / "CCI_Annex_T_Extract.md")


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Build 61850-7-3 / 7-4 OCR corpus MD + engineering extracts from per-page OCR."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OCR_ROOT = ROOT / "Architecture" / "_extracted_reg_analysis" / "_pdf_ocr"
KB = ROOT / "knowledge-base" / "08-engineering"

SPECS = {
    "61850-7-3": {
        "title": "IEC 61850-7-3 — CCLI Engineering Extract (CDC / common data classes)",
        "doc_id": "CCLI-PROTO-61850-7-3-001",
        "source_id": "ccli-61850-7-3-extract",
        "capture_id": "ccli-61850-7-3-capture-plan",
        "ocr_id": "ccli-61850-7-3-ocr-corpus",
        "edition": "IEC 61850-7-3 **Edition 2.0 2020-07**",
        "pdf": "07-protocols/regulations/standards-drop-20260918/IEC 61850-7-3-2020.pdf",
        "norm": "IEC 61850-7-3 — *Basic communication structure — Common data classes*",
        "parents": "`ccli-61850-6-extract` · `ccli-61850-7-4-extract` · `ccli-61850-8-1-extract` · `cei-tr-57-126`",
    },
    "61850-7-4": {
        "title": "IEC 61850-7-4 — CCLI Engineering Extract (LN classes / compatibility)",
        "doc_id": "CCLI-PROTO-61850-7-4-001",
        "source_id": "ccli-61850-7-4-extract",
        "capture_id": "ccli-61850-7-4-capture-plan",
        "ocr_id": "ccli-61850-7-4-ocr-corpus",
        "edition": "IEC 61850-7-4 **Edition 2.0 2010-12**",
        "pdf": "07-protocols/regulations/standards-drop-20260918/IEC 61850-7-4-2010.pdf",
        "norm": "IEC 61850-7-4 — *Basic communication structure — Compatible logical node classes and data object classes*",
        "parents": "`ccli-61850-7-3-extract` · `ccli-61850-6-extract` · `cei-tr-57-126` · `cei-0-16-allegato-t`",
    },
}


def build_ocr_corpus(slug: str, meta: dict) -> Path:
    ocr_dir = OCR_ROOT / slug
    out = KB / f"CCI_{slug.upper()}_OCR_Corpus.md"
    pages = sorted(ocr_dir.glob("p*.txt"))
    parts = [
        f"# {meta['norm']}\n\n",
        f"**RAG source_id:** `{meta['ocr_id']}`  \n",
        "**Purpose:** Verbatim OCR of all PDF file pages for search/RAG; verify CDC/LN tables against licensed PDF.  \n",
        f"**OCR path:** `Architecture/_extracted_reg_analysis/_pdf_ocr/{slug}/`  \n",
        f"**Licensed PDF:** `{meta['pdf']}`  \n\n---\n\n",
    ]
    for p in pages:
        text = p.read_text(encoding="utf-8", errors="replace")
        parts.append(f"\n\n## File page {p.stem[1:]}\n\n```\n{text}\n```\n")
    out.write_text("".join(parts), encoding="utf-8")
    return out


def build_extract(slug: str, meta: dict) -> Path:
    ocr_dir = OCR_ROOT / slug
    n = len(list(ocr_dir.glob("p*.txt")))
    fname = f"CCI_{slug.upper()}_Extract.md"
    out = KB / fname

    if slug == "61850-7-3":
        body = _extract_7_3(meta, n)
    else:
        body = _extract_7_4(meta, n)

    out.write_text(body, encoding="utf-8")
    return out


def _extract_7_3(meta: dict, n: int) -> str:
    return f"""# {meta['title']}

**Document ID:** {meta['doc_id']}  
**Revision:** 1.0  
**Date:** 2026-09-30  
**Edition:** {meta['edition']}  
**RAG source_id:** `{meta['source_id']}`  
**Normative basis:** {meta['norm']}  
**Licensed PDF:** `{meta['pdf']}` ({n} file pages OCR)  
**Full text:** `{meta['ocr_id']}` → `CCI_61850-7-3_OCR_Corpus.md`  
**Capture status:** **HAVE (P0 corpus)** — PDF on disk + full OCR 2026-09-30  
**Parents:** {meta['parents']}  
**Programme link:** K4.3 ICD · libiec61850 `CDC_*` · P3-14 / P5-M CDC justification

---

## Summary

**61850-7-3** defines **Common Data Classes (CDC)** — the typed building blocks (`MV`, `CMV`, `APC`, `ENC`, `SPS`, …) and their **data attributes** (`mag`, `q`, `t`, `ctlModel`, …) referenced by **61850-6 SCL** `DOType/@cdc` and instantiated in server code via libiec61850 **`CDC_*_create`**.

**CCLI:** Runtime MMS model in `mms_adapter.cpp` uses **7-3 CDCs** programmatically; TR 57-126 CID **DataTypeTemplates** embed the same CDC names. This extract closes the **P3-14** corpus gap (CDC/DA justification).

---

## Requirements

| REQ-ID | Source | Requirement | CCLI |
|--------|--------|-------------|------|
| **REQ-873-CDC-001** | 7-3 | Every DO in SCL/CID shall declare a valid **CDC** | TR 57-126 templates; dynamic model |
| **REQ-873-MV-001** | CDC **MV** | Measured value: **mag** (+ optional **q**, **t**) | `PdCMMXU1.TotW`, `TotVAr` |
| **REQ-873-APC-001** | CDC **APC** | Controllable analogue set-point + **ctlModel** | `WlimDWMX1.WMaxSptPct`, `WSdDAGC1.WSptPct` |
| **REQ-873-ENC-001** | CDC **ENC** | Controllable enumerated + **ctlModel** | LN **Mod** on control LNs |
| **REQ-873-ENS-001** | CDC **ENS** | Status enumerated (no control) | `LLN0.Mod`, `Health` stubs |
| **REQ-873-Q-001** | **Quality** | Common **Quality** type for ST/MX | MMS quality sync on TotW/TotVAr |
| **REQ-873-CTL-001** | **ctlModel** | Direct / SBO-enhanced / status-only | Match Annex T + TR CID ctlModel |
| **REQ-873-NS-001** | Namespace | CDC semantics under **61850-7-4** LN namespace | `ldNs` / `lnNs` in CID |

---

## Batch A — CDC inventory (CCLI-relevant)

| CDC | Role in CCLI | libiec61850 API | TR 57-126 / runtime |
|-----|--------------|-----------------|---------------------|
| **MV** | POC P/Q measurements | `CDC_MV_create` | `PdCMMXU1.TotW`, `TotVAr`, `PPV` (P5-M) |
| **APC** | Active power control SP | `CDC_APC_create` | `WlimDWMX1`, `WSdDAGC1` |
| **ENC** | Mode / enable | `CDC_ENC_create` | `Wlim*.Mod`, `WSd*.Mod` |
| **ENS** | Status enum | `CDC_ENS_create` | `LLN0.Mod`, `Health` |
| **SPS** | Single-point status | (future DI mapping) | Alarms / signals |
| **DPC** | Double-point control | (future DO) | Curtailment path |

---

## Batch B — MV structure (TotW / TotVAr)

| DA | FC | CCLI use |
|----|-----|----------|
| **mag.f** | MX | Float magnitude — Modbus `p_kw` / `q_kvar` |
| **q** | MX | Quality — stale when Modbus fail |
| **t** | MX | UTC timestamp — 4 s report cadence |
| **units** | CF | kW / kVAr (configRev / SCL) |

---

## Batch C — Control CDCs (PF2 / DSO)

| ctlModel | Meaning | CCLI LNs |
|----------|---------|----------|
| **status-only** | Read-only mode | Status stubs |
| **direct-with-normal-security** | Oper without select | Lab PF2 paths (future) |
| **sbo-with-enhanced-security** | Select-before-operate | Annex T DSO control profile |

---

## Interface matrix

| Interface | Source | Destination | Protocol |
|-----------|--------|-------------|----------|
| Modbus meter | RS485 | `MV.mag` | FC3 → float |
| MMS client | Eth_A :3782 | CDC attributes | 61850-8-1 / 7-2 |
| SCL DOType | TR 57-126 CID | libiec61850 model | 61850-6 |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| Full CDC table OCR | Per-page OCR corpus | Spot-check **Table** pages in PDF | Verify garbled OCR at lab |
| CID → codegen | Hand `CDC_*` | genconfig from SCL | **OPEN** |
| PPV MV | Not implemented | Meter V registers | P5-M08 |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| PDF on disk | Junction path | File exists, non-zero |
| OCR | `{n}` pages | `toc.json` + corpus MD |
| P3-14 | Checklist row | Extract published |
| Runtime | MMS read | `TotW.mag.f`, `TotVAr.mag.f` |

---

## Keywords

`61850-7-3`, `CDC`, `MV`, `APC`, `ENC`, `Quality`, `{meta['source_id']}`, `{meta['ocr_id']}`
"""


def _extract_7_4(meta: dict, n: int) -> str:
    return f"""# {meta['title']}

**Document ID:** {meta['doc_id']}  
**Revision:** 1.0  
**Date:** 2026-09-30  
**Edition:** {meta['edition']}  
**RAG source_id:** `{meta['source_id']}`  
**Normative basis:** {meta['norm']}  
**Licensed PDF:** `{meta['pdf']}` ({n} file pages OCR)  
**Full text:** `{meta['ocr_id']}` → `CCI_61850-7-4_OCR_Corpus.md`  
**Capture status:** **HAVE (P0 corpus)** — PDF on disk + full OCR 2026-09-30  
**Parents:** {meta['parents']}  
**Programme link:** K4.3 ICD · TR 57-126 LN/DO names · **61850-7-420** DER profile

---

## Summary

**61850-7-4** defines **compatible logical node classes** (`MMXU`, `LLN0`, `LPHD`, …) and their **standardized data objects** (`TotW`, `TotVAr`, `Mod`, …). **61850-7-420** extends 7-4 for DER (`DPCC`, `DVAR`, `DPFW`, …) — declared via **`lnNs`** in TR 57-126 CID.

**CCLI:** We **conform** to 7-4/7-420 naming but **do not load** the CID at runtime; `mms_adapter.cpp` builds a **subset** matching `signal_map.yaml`. Base namespace **`IEC61850-7-4:2010`** on `LLN0.NamPlt.ldNs`.

---

## Requirements

| REQ-ID | Source | Requirement | CCLI |
|--------|--------|-------------|------|
| **REQ-874-LN-001** | 7-4 | LN **lnClass** + **prefix** + **inst** uniquely identify function | `PdCMMXU1`, `WlimDWMX1` |
| **REQ-874-MMXU-001** | **MMXU** | POC measurements: **TotW**, **TotVAr**, **PPV**, **A**, … | T.3.1.3 / O.8.3 |
| **REQ-874-DPCC-001** | 7-420 **DPCC** | Active power controller at POC | `PdC_Wi` / Wlim family |
| **REQ-874-DVAR-001** | 7-420 **DVAR** | Reactive power controller | P5-R01 `VArSdDVAR1` |
| **REQ-874-NS-001** | Namespace | `ldNs` = 7-4; DER LNs use **7-420** `lnNs` | Lab CID |
| **REQ-874-DS-001** | DataSets | FCDA reference **lnClass** + **doName** | `DS_R_PdC_Mis4sec` |

---

## Batch A — LN classes in TR 57-126 / CCLI MVP

| lnClass | Prefix (example) | Namespace | CCLI status |
|---------|------------------|-----------|-------------|
| **MMXU** | PdC | 7-4 | **IMPL** TotW, TotVAr |
| **MMXU** | GenPV, GenTer, … | 7-4 | STUB / future |
| **DPCC** / **DPCW** | Wlim, PdC_Wi | 7-420 | **PART** Wlim APC |
| **DAGC** | WSd | 7-420 | **PART** WSd APC |
| **DVAR** | VArSd | 7-420 | **OPEN** P5-R |
| **LLN0** | — | 7-4 | Reports, DataSets |
| **LPHD** | — | 7-4 | PhyNam / vendor |

---

## Batch B — PdC MMXU data objects (Annex T T.3.1.3)

| DO | CDC (7-3) | Mandatory | CCLI |
|----|-----------|-----------|------|
| **TotW** | MV | Yes | **IMPL** — 4 s URCB |
| **TotVAr** | MV | Yes | **IMPL** r21 |
| **PPV** | MV | Yes | **OPEN** P5-M08 |
| **A** | MV | Conditional | **OPEN** |

---

## Batch C — Namespace model (CID)

```
LLN0.NamPlt.ldNs  = IEC61850-7-4:2010     (base dictionary)
DER LN.NamPlt.lnNs = IEC61850-7-420:2019A (extension)
```

Runtime libiec61850 dynamic model uses **same object names** without parsing SCL.

---

## BOM matrix

| Block | Function | Candidate / PN | Status |
|-------|----------|----------------|--------|
| 7-4 dictionary | LN/DO names | IEC PDF + TR 57-126 | **INGESTED** |
| 7-420 DER LNs | PF2 control | CEI TR + Annex T | **PART** |
| libiec61850 | CDC runtime | third_party | **HAVE** |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-74-01 | Hand-built model ≠ full CID | DSO conformance fail | CID loader / genconfig |
| R-74-02 | 7-420 AMD drift | lnNs mismatch | Freeze TR 57-126 rev |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| PDF confirmed | Junction + SHA/size | Non-zero file |
| OCR corpus | Page count = {n} | Searchable MD |
| MMS browse | Client directory | `PdCMMXU1` present |
| P3-14 | Regulation checklist | **PASS** (corpus) |

---

## Keywords

`61850-7-4`, `MMXU`, `DPCC`, `DVAR`, `7-420`, `{meta['source_id']}`, `{meta['ocr_id']}`
"""


def main() -> None:
    for slug, meta in SPECS.items():
        c = build_ocr_corpus(slug, meta)
        e = build_extract(slug, meta)
        print(c, e, sep="\n")


if __name__ == "__main__":
    main()

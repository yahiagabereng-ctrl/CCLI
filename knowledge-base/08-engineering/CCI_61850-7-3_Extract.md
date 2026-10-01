# IEC 61850-7-3 — CCLI Engineering Extract (CDC / common data classes)

**Document ID:** CCLI-PROTO-61850-7-3-001  
**Revision:** 1.0  
**Date:** 2026-09-30  
**Edition:** IEC 61850-7-3 **Edition 2.0 2020-07**  
**RAG source_id:** `ccli-61850-7-3-extract`  
**Normative basis:** IEC 61850-7-3 — *Basic communication structure — Common data classes*  
**Licensed PDF:** `07-protocols/regulations/standards-drop-20260918/IEC 61850-7-3-2020.pdf` (151 file pages OCR)  
**Full text:** `ccli-61850-7-3-ocr-corpus` → `CCI_61850-7-3_OCR_Corpus.md`  
**Capture status:** **HAVE (P0 corpus)** — PDF on disk + full OCR 2026-09-30  
**Parents:** `ccli-61850-6-extract` · `ccli-61850-7-4-extract` · `ccli-61850-8-1-extract` · `cei-tr-57-126`  
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
| OCR | `151` pages | `toc.json` + corpus MD |
| P3-14 | Checklist row | Extract published |
| Runtime | MMS read | `TotW.mag.f`, `TotVAr.mag.f` |

---

## Keywords

`61850-7-3`, `CDC`, `MV`, `APC`, `ENC`, `Quality`, `ccli-61850-7-3-extract`, `ccli-61850-7-3-ocr-corpus`

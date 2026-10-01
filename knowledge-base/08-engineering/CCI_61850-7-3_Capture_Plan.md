# IEC 61850-7-3 — Capture Plan (CDC / common data classes)

**Document ID:** CCLI-PROTO-61850-7-3-001  
**Revision:** 1.0  
**Date:** 2026-09-30  
**RAG source_id:** `ccli-61850-7-3-capture-plan` → `ccli-61850-7-3-extract`  
**Normative basis:** IEC 61850-7-3:2020 — *Common data classes*  
**Licensed PDF:** `07-protocols/regulations/standards-drop-20260918/IEC 61850-7-3-2020.pdf` (~151 file pages)  
**Programme link:** K4.3 · P3-14 CDC justification · libiec61850 `CDC_*`

---

## Summary

**61850-7-3** defines **CDC** types and **data attributes** used in SCL `DOType` and in libiec61850 server construction. **CCLI** needs extracts for: **MV** (TotW/TotVAr/PPV), **APC/ENC** (Wlim/WSd), **Quality**, **ctlModel**, and **timestamp** attributes.

**PDF status:** **CONFIRMED ON DISK** 2026-09-30 (standards drop junction). Image-only export — **full OCR** via `scripts/extract-61850-7-3-7-4-ocr.py`.

---

## Capture batches

| Batch | Focus | Status |
|-------|--------|--------|
| **A** | Scope, CDC overview, **Quality** type | **COMPLETE** → extract §Batch A |
| **B** | **MV**, **CMV**, **SAV** measurement CDCs | **COMPLETE** → extract §Batch B |
| **C** | **APC**, **ENC**, **BSC**, control CDCs + **ctlModel** | **COMPLETE** → extract §Batch C |
| **D** | **SPS/DPS/INS** status CDCs | **PART** — alarm path future |
| **E** | OCR corpus + RAG ingest | **COMPLETE** 2026-09-30 |

---

## CCLI mapping

| CDC | Module |
|-----|--------|
| MV | `mms_adapter.cpp` — PdCMMXU1 |
| APC / ENC | Wlim / WSd control LNs |
| Quality | Modbus stale → MMS q |

---

## Verification

| Requirement | Test | Pass |
|-------------|------|------|
| PDF junction | File exists | **PASS** 2026-09-30 |
| OCR pages | `Architecture/_pdf_ocr/61850-7-3/` | Match PDF page count |
| P3-14 checklist | `ccli-61850-7-3-extract` published | **PASS** |

---

## Keywords

`61850-7-3`, `CDC`, `capture`, `ccli-61850-7-3-capture-plan`

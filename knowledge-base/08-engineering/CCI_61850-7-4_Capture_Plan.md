# IEC 61850-7-4 — Capture Plan (LN classes / compatibility)

**Document ID:** CCLI-PROTO-61850-7-4-001  
**Revision:** 1.0  
**Date:** 2026-09-30  
**RAG source_id:** `ccli-61850-7-4-capture-plan` → `ccli-61850-7-4-extract`  
**Normative basis:** IEC 61850-7-4:2010 — *Compatible logical node classes and data object classes*  
**Licensed PDF:** `07-protocols/regulations/standards-drop-20260918/IEC 61850-7-4-2010.pdf` (~185 file pages)  
**Programme link:** K4.3 · TR 57-126 · **61850-7-420** DER LNs · P5-M / P5-R

---

## Summary

**61850-7-4** defines **logical node classes** (`MMXU`, `LLN0`, …) and standard **data object** names. Italian CCI adds **61850-7-420** DER nodes (`DPCC`, `DVAR`, …) via **`lnNs`**. **CCLI** uses 7-4 names in CID + dynamic server; full plant model is **subset** of TR 57-126.

**PDF status:** **CONFIRMED ON DISK** 2026-09-30 (standards drop junction). Image-only export — **full OCR** via `scripts/extract-61850-7-3-7-4-ocr.py`.

---

## Capture batches

| Batch | Focus | Status |
|-------|--------|--------|
| **A** | Scope, namespace, LN naming | **COMPLETE** → extract §Batch A |
| **B** | **MMXU** + POC measurements (TotW, TotVAr, PPV) | **COMPLETE** → extract §Batch B |
| **C** | System / physical LNs (**LLN0**, **LPHD**) | **COMPLETE** — reports |
| **D** | Cross-ref **61850-7-420** (not in this PDF) | Via TR 57-126 CID `lnNs` |
| **E** | OCR corpus + RAG ingest | **COMPLETE** 2026-09-30 |

---

## CCLI mapping

| LN class | CCLI |
|----------|------|
| MMXU (PdC) | P5-M measurements |
| DPCC / DAGC | PF2 Wlim / WSd |
| DVAR | P5-R reactive |
| LLN0 | DataSets, URCB/BRCB |

---

## Verification

| Requirement | Test | Pass |
|-------------|------|------|
| PDF junction | File exists | **PASS** 2026-09-30 |
| OCR pages | `Architecture/_pdf_ocr/61850-7-4/` | Match PDF page count |
| MMS directory | Client lists `PdCMMXU1` | Lab |

---

## Keywords

`61850-7-4`, `MMXU`, `capture`, `ccli-61850-7-4-capture-plan`

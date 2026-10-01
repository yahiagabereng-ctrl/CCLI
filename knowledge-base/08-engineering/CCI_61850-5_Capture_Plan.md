# IEC 61850-5 — Capture Plan (CCLI / TG-524)

**Document ID:** CCLI-PROTO-61850-5-001  
**Revision:** 1.1  
**Date:** 2026-09-21  
**RAG source_id:** `ccli-61850-5-capture-plan` → `ccli-61850-5-extract`  
**Normative basis:** IEC 61850-5:2003 — *Communication requirements for functions and device models*  
**Licensed PDF:** `07-protocols/regulations/standards-drop-20260918/IEC 61850-5.pdf` (primary) · legacy `61850-5_screenshots.pdf` (supplement)  
**Companion:** `cei-0-16-allegato-t` · `CCI_Annex_T_Extract.md`  
**Programme link:** K4.1 · Annex T performance class mapping

---

## Summary

**61850-5:2003** defines **communication requirements** between logical nodes — **message types**, **performance classes P1–P3 / M1–M3**, and **transfer time** budgets. **Annex T** maps CCI MMS traffic to **Type 3 (low speed)** ≈ **500 ms** and optional GOOSE to **Type 1**.

**Note:** A **2013 edition** exists; Annex T cites §11.2.2/11.2.3 (interfaces) and performance via **§13.7**. This corpus uses **2003** PDF on disk — validate against 2013 if DSO tooling references newer numbering.

---

## Capture status roll-up

| Batch | Focus | Status |
|-------|-------|--------|
| **A** | §11.2.2 TCI/TMI interfaces · §11.2.3 auto control LNs | **COMPLETE** (PDF p.53–54) |
| **B** | §13.5–13.6 Message types · performance class intro | **COMPLETE** (PDF p.80–81) |
| **C** | §13.7 Types 1–3 — P1/P2/P3 · **500 ms Type 3** | **COMPLETE** (PDF p.82–83) |
| **D** | §13.7 Types 4–7 · metering classes (partial) | **PART** (PDF p.84–87) |
| **SKIP** | §14 scenarios · Annex A examples (pp. 188–205) | Informative |

---

## CCLI mapping (Annex T)

| Annex T requirement | 61850-5 source |
|---------------------|----------------|
| Plant data / measurements **Type 3** | §13.7.3 — **< 500 ms** total transmission |
| 4 s periodic MMS reports | Type 3 low speed (not Type 2 100 ms) |
| GOOSE subscribe **Type 1** if used | §13.7.1 — P1 trip 10 ms · P2/3 3 ms |
| TCI telecontrol interface | §11.2.2 — ITCI remote control subset |
| Performance class P5 wording in Annex T | Informative mapping → **Type 3** in 2003 ed. |

---

## Verification

| Item | Pass criteria |
|------|---------------|
| Batch C | Extract cites **500 ms** ↔ Annex T Type 3 |
| K4.1 | TR CID 4 s reports consistent with Type 3 class |
| Corpus | `ccli-61850-5-extract` **HAVE (P1 P0)** |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-09-16 | Initial capture from `61850-5_screenshots.pdf` OCR |

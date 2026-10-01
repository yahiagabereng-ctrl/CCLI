# IEC 61850-7-2 — Capture Plan (ACSI / MMS)

**Document ID:** CCLI-PROTO-61850-7-2-001  
**Revision:** 1.0  
**Date:** 2026-09-21  
**RAG source_id:** `ccli-61850-7-2-capture-plan` → `ccli-61850-7-2-extract`  
**Revision:** 1.1  
**Normative basis:** IEC 61850-7-2 — *Communication networks and systems for power utility automation — Part 7-2: Basic information and communication structure — Abstract communication service interface (ACSI)*  
**PDF (licensed):** `regulations/standards-drop-20260918/IEC 61850-7-2.pdf` (231 pp.)  
**Programme link:** K4.1 · REQ-61850-001 · libiec61850 MMS server

---

## Summary

**61850-7-2** defines **ACSI** — the abstract services (read, write, control, reporting, GOOSE/SV mapping at application layer) implemented by **61850-8-1** over MMS. **CCLI** needs extracts for: server association model, **Report** services (aligned with TR 57-126 `intgPd`), **Control** (`Oper`, `SBOw`), and **time-stamped** data for Allegato T.

**Dependency:** **61850-8-1** PDF still **MISSING** from drop — transport mapping capture blocked until procured.

---

## Capture batches

| Batch | Focus | Pages (TBD from PDF) | Status |
|-------|--------|----------------------|--------|
| **A** | Scope, ACSI model, association | Front matter + §5–7 | **COMPLETE** → extract §Batch A |
| **B** | **GetDirectory**, **GetDataValues**, **SetDataValues** | Core server | **COMPLETE** → extract §Batch B |
| **C** | **Report** (BRCB/URCB), integrity period | Links TR 57-126 Table 23 | **COMPLETE** → extract §Batch C |
| **D** | **Control** model (SBO/Direct) | PF2 / curtailment mapping | **COMPLETE** → extract §Batch D |
| **E** | GOOSE/SV ACSI mapping (high level) | Cross-ref 62351-6 | **COMPLETE** → extract §Batch E |

---

## CCLI mapping

| ACSI area | CCLI module |
|-----------|-------------|
| Server association | `adapters/iec61850_mms/mms_adapter.cpp` |
| Data model | `apps/ccli/config/icd/` + TR 57-126 CID |
| Reports | Annex T telemetry |
| Control | PF2 curtailment (future LN mapping) |

---

## Verification

| Requirement | Test | Pass |
|-------------|------|------|
| Batch A | Extract § scope in `CCI_61850-7-2_Extract.md` | Published |
| Batch C | `intgPd` / report rate cites 7-2 + 61850-6 Table 23 | Traceability row |

---

## Keywords

`61850-7-2`, `ACSI`, `MMS`, `capture`, `ccli-61850-7-2-capture-plan`

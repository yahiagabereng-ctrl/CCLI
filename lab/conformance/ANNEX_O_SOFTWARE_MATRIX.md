# Annex O → software (5-LN DUT traceability)

**Document ID:** CCLI-LAB-ANNEX-O-SW-001  
**Revision:** 1.1  
**Date:** 2026-10-22  
**RAG source_id:** `ccli-annex-o-software-matrix`  

**Full O+T matrix (55 × 6):** [ANNEX_OT_SW_GAP_MATRIX.md](ANNEX_OT_SW_GAP_MATRIX.md) — **source of truth** for clause → SW → 5-LN → Phase.  
**Cert book (Test · Cert · Lines):** [Full Validate.md](Full%20Validate.md)

This file remains a **short index** for RAG and onboarding. Do not duplicate rows here; edit the OT matrix instead.

---

## Quick 5-LN MMS map

| Annex O need (POC / control) | 5-LN LN / mechanism |
|------------------------------|---------------------|
| P, Q, V at PdC (PF1) | `PdCMMXU1` (TotW, TotVAr, PPV) ← Modbus EMT432 |
| DSO P limit | `WlimDWMX1` |
| External P set-point | `WSdDAGC1` |
| 4 s DSO visibility | `LLN0.urcb_PdC_Mis4sec` |
| Secure DSO path | Eth_A MMS TLS (Annex T transport) |

---

## Related

| Topic | Path |
|-------|------|
| Specimen pin | [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md) |
| Phase gates | [CCI_Phase_Regulation_Checklists.md](../../knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md) |
| Comms-loss | `lab/evidence/O13_1_isolation_2026-09-25/P3_05_COMMS_LOSS_FALLBACK.txt` |
| O.14 coverage | `lab/evidence/phase7/P7-04_EVENT_COVERAGE_MATRIX.md` |

---

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-20 | Initial coarse matrix |
| 1.1 | 2026-10-22 | Pointer to ANNEX_OT_SW_GAP_MATRIX.md (55×6) |

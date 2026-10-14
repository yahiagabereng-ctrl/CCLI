# Validation report — unified matrix vs **5-LN only**

**Document ID:** CCLI-PROD-5LN-VAL-001  
**Revision:** 1.0  
**Date:** 2026-10-14  
**RAG source_id:** `ccli-product-specimen-5ln-validation`  
**Validator:** `scripts/validate-5ln-cid.ps1`  
**Canonical LN set:** `ccli-dnv-product-readiness` · [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md)

---

## Executive summary

| Item | Status | Notes |
|------|--------|-------|
| Unified matrix **U-T1-*** (MMS core) | **PASS** | Objects match lab lineage names for the **5** cert LNs |
| Unified matrix **U-P5-01** (legacy P5 doc) | **FAIL → FIXED** | Old link assumed **31 LNs** + VArSd; use [P5_FULLCIRCLE_5LN_SCOPE.md](P5_FULLCIRCLE_5LN_SCOPE.md) |
| **Product CID** in repo | **FAIL** | Path `product_tr400_eth_a_5ln.cid` **missing** — block U-T1-02 |
| **PICS_FILLED_WORKBOOK** (U-DOC-01) | **PART** | Workbook is **31-LN lab**; cert submit needs **5-LN** Cover + trimmed ACSI model sheet |
| **MICS_DRAFT** (U-DOC-03) | **PART** | Trim to 5 LNs before lab intake |
| **stage-tsp-field-pack** lab CID | **WARN** | Pack still includes `lab_tg544_eth_a.cid` for reference — TSP Compare **must** use **5-LN** file only |
| Cyber + 104 + P7 rows | **PASS** | Independent of LN count (`ccli-lab-requirement-test-matrix-cyber` §5) |

Run machine check:

```powershell
powershell -File scripts\validate-5ln-cid.ps1
# Expect exit 2 until product CID exists; exit 1 if lab 31-LN CID passed by mistake
powershell -File scripts\validate-5ln-cid.ps1 -CidPath apps\ccli\config\icd\lab_tg544_eth_a.cid
# Must FAIL (proves validator)
```

---

## MMS name cross-check (lab CID lineage → 5-LN keep set)

Source: `apps/ccli/config/icd/lab_tg544_eth_a.cid` (rev 9) — **trim** to:

| # | Browse name | SCL location | Keep in 5-LN? |
|---|-------------|--------------|---------------|
| 1 | `LLN0` | `LN0 lnClass="LLN0"` | **M** · strip extra URCB/BRCB/GOOSE |
| 2 | `LPHD1` | `LN lnClass="LPHD" inst="1"` | **M** |
| 3 | `PdCMMXU1` | `prefix="PdC" lnClass="MMXU" inst="1"` | **M** |
| 4 | `WlimDWMX1` | `prefix="Wlim" lnClass="DWMX" inst="1"` | **M** |
| 5 | `WSdDAGC1` | `prefix="WSd" lnClass="DAGC" inst="1"` | **M** |

**U-T1-03 pass criterion:** GetNameList / browse shows **exactly these 5** names under `LD_Plant` (not “~31”).

**URCB (U-T1-05…06):** `urcb_PdC_Mis4sec` · dataset `DS_R_PdC_Mis4sec` — present on lab LLN0; cert CID must retain **only** this unbuffered RCB for first-pass PICS.

**Control (U-T1-07…08):** Paths match [CCI_TestSuitePro_Sequencer_Draft.md](../CCI_TestSuitePro_Sequencer_Draft.md) §2 — valid for 5-LN.

---

## Row-by-row matrix audit (5-LN applicability)

| U-ID | 5-LN verdict | Correction |
|------|--------------|------------|
| U-P0-06 | PASS | Import **product** CID only |
| U-P0-07 | PASS | Deploy **product** cfg SHA |
| U-DOC-01 | PART | Use **5-LN** PICS; do not sign 31-LN workbook as cert |
| U-DOC-03 | PART | MICS rows = 5 LNs only |
| U-T1-02 | BLOCKED | Until `validate-5ln-cid.ps1` PASS |
| U-T1-03 | PASS | Expect **5** names (listed above) |
| U-T1-04…10 | PASS | Same objects as lab first-pass |
| U-T1-05c | PASS/O | q in dataset; waive if no q member |
| U-P5-01 | PASS | Scope doc **P5_FULLCIRCLE_5LN_SCOPE** only |
| U-P5-02 | PASS | Plant mock/meter — no extra LNs |
| U-N/A-01 | PASS | No BRCB/GOOSE/File in **cert** CID |
| U-ACC-* | PASS | Programme rows — not LN-specific |

---

## PICS / Services (cert CID)

Per [CID_PICS_ALIGNMENT.md](CID_PICS_ALIGNMENT.md) **cert freeze** on 5-LN CID:

- Remove or zero: GOOSE, FileHandling, ConfLogControl, extra ConfReportControl **buffered**, ClientServices  
- PICS: BRCB · GOOSE · File · SG = **N/A** (matrix U-N/A-01)

---

## Verification

| REQ-ID | Test | Pass |
|--------|------|------|
| REQ-5LN-VAL-001 | `validate-5ln-cid.ps1` on product CID | Exit 0 |
| REQ-5LN-VAL-002 | TSP browse | 5 names exactly |
| REQ-5LN-VAL-003 | Compare vs 5-LN CID | 0 MISSING; no TSP-SCL-01 31-LN waiver pack |
| REQ-5LN-VAL-004 | P5 run | No step references VArSd / 31-LN count |

**Keywords:** `validation`, `5-LN`, `audit`, `ccli-product-specimen-5ln-validation`

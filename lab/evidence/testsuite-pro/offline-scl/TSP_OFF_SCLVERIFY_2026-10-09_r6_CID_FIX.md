# TSP SCL Verify — CID rev 6 fix (2026-10-09)

**CID:** `apps/ccli/config/icd/lab_tg544_eth_a.cid` (Header revision **6**)

## Problem (12 Errors before fix)

| Cluster | Count | Fix in rev 6 |
|---------|------:|--------------|
| CompareToStandardEnumMismatch (`on-blocked`) | 4 | EnumType **Mod** / **Beh** ord 2 → **`blocked`** (IEC 61850-7-4) |
| Services SupSubscription / no LGOS | 1 | **Removed** `<SupSubscription>` (align `CID_PICS_ALIGNMENT.md`) |
| SchemaValidation LN0 element order | 1 | **Order:** DataSet → ReportControl → **DOI** → GSEControl → `/LN0` (CEI-style DOI before GSE; not inside DataSet) |
| SchemaValidation DAI→SDI under PdC PPV/A | 6 | **Rewrote** `cVal` as SDI→SDI→SDI→DAI(f) under phsAB/BC/CA and phsA/B/C |

**LLN0 NamPlt configRev:** `20261009` (bump after structural CID change).

**Note:** A bad merge once placed DOI **inside** the first DataSet (TSP: invalid child `DataSet`). Repaired in working tree before push.

## Operator re-verify

1. TSP → close/reopen **`lab_tg544_eth_a.cid`** from repo (rev 6).
2. **SCL Verify → Run** — expect **Error: 0** (Warnings/Recommendations may remain ~60/13).
3. Export → `offline-scl/TSP_OFF_SCLVERIFY_2026-10-09_r6.xlsx`.
4. **DUT:** redeploy CID / restart **ccli** so runtime model matches (configRev in reports may change).

## Evidence

- Prior counts: `inbox/TSP_OFF_SCLVERIFY_2026-10-09.txt`
- Baseline r2: `TSP_OFF_SCLVERIFY_2026-09-26_r2_TRIAGE.md`

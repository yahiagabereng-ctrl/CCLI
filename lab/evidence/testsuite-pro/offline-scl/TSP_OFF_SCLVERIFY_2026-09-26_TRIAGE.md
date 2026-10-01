# TSP SCL Verify triage — 2026-09-26

# CCLI TSP evidence
tsp_version: 4.7.4.5037
cid: apps/ccli/config/icd/lab_tg544_eth_a.cid
ied: CCI016_01
gate: OFF
tool: SCLVERIFY
operator: Yahia
date: 2026-09-26

## Sources (HAVE)

| File | Path |
|------|------|
| Excel export (user) | `apps/ccli/config/icd/Report.xlsx` |
| Excel export (typo name) | `apps/ccli/config/icd/Reort.xls` |
| Evidence copies | `lab/evidence/testsuite-pro/offline-scl/TSP_OFF_SCLVERIFY_2026-09-26.xlsx` (+ `.xls`, `_from_xls.xlsx`) |

## Summary (TSP UI / Report.xlsx)

| Severity | Count |
|----------|------:|
| Error | 59 |
| Warning | 65 |
| Recommendation | 39 |
| Information | 5 |
| **Total** | **168** |

## Root cause (FIXED in CID)

Spaced-out enum literal corruption in `DataTypeTemplates` (inherited from CEI example):

| Broken (CID) | Correct |
|--------------|---------|
| `statu s-o n ly` | `status-only` |
| `d irect-with-n o rmal-secu rity` | `direct-with-normal-security` |
| `sbo-with-n o rmal-secu rity` | `sbo-with-normal-security` |
| `d irect-with-en h an ced-secu rity` | `direct-with-enhanced-security` |
| `sbo-with-en h an ced-secu rity` / `sbo-with-en han ced-secu rity` | `sbo-with-enhanced-security` |
| OriginatorCategoryKind `no t-su pp orted`, `bay-con tro l`, … | `not-supported`, `bay-control`, … |

**Cascade:** TSP treated invalid ctlModel Val as `status-only` → 13× ctlModel warnings + 13× sboTimeout + 13× operTimeout + 39 Recommendations (SBOw/Oper/Cancel unused) + majority of `CompareToStandardEnumMismatch` Errors.

Also added mandatory `configRev` on `LPL_5_NamPlt` + LLN0 `NamPlt` instance (`20260926`).

Patched files: `lab_tg544_eth_a.cid` (rev 3), `cei-tr-57-126-example.cid`.

## Disposition by cluster

| Cluster | Sev | Count (approx) | Disposition | Action |
|---------|-----|----------------|-------------|--------|
| CtlModelKind / ctlModel Val spacing | E/W/R | ~100+ | **FIXED** | Re-run SCL Verify |
| Missing `configRev` | E | 1 | **FIXED** | Re-run |
| Custom LN (DPCC, DECP, DGEN, DSTO, DWMX, DAGC, DVAR, DFPF, DVVR, DPMC, DPFW) | W | 15 | **ACCEPT** | CEI TR 57-126 / Figura 2 — not in IEC master model |
| Mod/Beh `on-blocked` vs master `blocked` | E | 4 | **ACCEPT (lab)** | Ed naming; CEI model uses `on-blocked` |
| Services: TimerActivated / ConfLogControl / GOOSE / SupSubscription without blocks/LGOS | W/E | 4 | **DEFER** | Trim Services claims or add stubs when product needs them |
| Duplicate Template id (APC/ENC/Mod↔Beh) | W | ~8 | **TSP quirk / review** | Likely adjacent-type false pairing; re-check after enum fix |
| Stats (Edition 2007B4, 4 RCBs, 0 GOOSE, 31 LN) | I | 5 | **OK** | Informative |

## Product impact

- **Before fix:** SBO-enhanced controls (`WMaxSptPct`, `WSptPct`, Wlim/WSd Mod, …) were not recognised as valid ctlModel → TSP (and any strict SCL consumer) would treat them as status-only.
- **After fix:** Re-import CID in TSP workspace and **Run SCL Verify** again; expect Error count to drop sharply. Remaining Warnings for custom LNs are expected for Annex T LN classes.

## Next

1. TSP → reload `lab_tg544_eth_a.cid` → SCL Verify → **Run**.
2. Export new report → `TSP_OFF_SCLVERIFY_2026-09-26_r2.xlsx` under this folder.
3. Optional: trim Services (`GOOSE`, `ConfLogControl`, `TimerActivatedControl`, `SupSubscription`) for lab-honest profile.

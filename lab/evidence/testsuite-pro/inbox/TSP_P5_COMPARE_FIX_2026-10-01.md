# TSP Compare Model — fix pack (r25)

**Date:** 2026-10-01  
**Prior export:** `Compare.xlsx` (77 rows)  
**Build:** 0.1.0-r25 (P5-COMPARE-FIX)

## Root causes fixed in firmware + CID

| Cluster | Count (before) | Fix |
|---------|----------------:|-----|
| ctlModel SBO vs direct | 6 | CID Wlim/WSd/VArSd → `direct-with-enhanced-security`; removed runtime override |
| BRCB ResvTms missing from file | 2 | `IedServerConfig_enableResvTmsForBRCB(false)` |
| PPV/A MissingFromDiscover | ~54 | CID `PdCMMXU1` DOI for PPV/A phases + regen `.cfg` |
| RCB OptFlds/TrgOps value | 9 | **Procedure:** Compare **before** URCB enable (step 5) |

## TSP re-test procedure

1. Deploy **0.1.0-r25** (`run-p5-fullcircle-cleartext.ps1`)
2. Modbus slave COM5 @ 450 kW
3. TSP Connect `:102` TLS off — load **`lab_tg544_eth_a.cid`**
4. Steps 2–4 reads (TotW/TotVAr/PPV)
5. **Compare Model now** (before Report Viewer / URCB)
6. Export new `Compare.xlsx` → expect **≤15 rows** (mostly RCB if URCB already enabled)

## Remaining expected waivers

- Live TotW/TotVAr vs static CID `<Val>` — LN-GAP-01
- Gen* static zeros — PLANT-GAP-01
- Mod/Beh boot values — LAB-CTL-01/02

## Files changed

- `apps/ccli/config/icd/lab_tg544_eth_a.cid`
- `apps/ccli/config/icd/lab_tg544_eth_a.cfg` (regenerated)
- `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp`

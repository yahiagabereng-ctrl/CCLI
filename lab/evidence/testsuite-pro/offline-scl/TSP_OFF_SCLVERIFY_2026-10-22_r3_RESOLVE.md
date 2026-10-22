# TSP SCL Verify — product 5-LN CID rev 3 (2026-10-22)

**CID:** `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid` (Header **revision 3**, `configRev` **20261022**)  
**Prior export:** `scl verify 10.22.2026.csv` (field pack rev 1) — **0 Error**, 19 Warning, 4 Recommendation, 5 Information.

## Resolved in rev 3 (engineering)

| TSP message | Fix |
|-------------|-----|
| TimerActivatedControls / no `operTm` | Removed `<TimerActivatedControl />` per [CID_PICS_ALIGNMENT.md](../../../conformance/CID_PICS_ALIGNMENT.md) cert freeze |
| Duplicate `EnumType` Mod/Beh, Health/PhyHealth | Single enum; `ENS_2_Beh` → `Mod`, `ENS_1_PhyHealth` → `Health` |
| Duplicate `DAType` APC/ENC Operate vs SelectWithValue | Removed `APCSelectWithValue_1` / `ENCSelectWithValue_1`; dropped unused `SBOw` (direct ctlModel) |
| `sboTimeout` / `operTimeout` not set (×4) | Added DOType DAs + instance `DAI` on Wlim/WSd APC/ENC control DOs (30000 / 5000 ms) |

## Accepted (cannot remove without dropping CEI LNs)

| TSP message | Disposition |
|-------------|-------------|
| Custom `lnClass=DWMX`, `DAGC` | **ACCEPT** — CEI TR 57-126 Figura 2 / Annex T |

## Operator re-verify

1. TSP → reload **`product_tr400_eth_a_5ln.cid`** from repo (rev 3).
2. **SCL Verify → Run** — expect **Error: 0**; warnings reduced (custom LN only, plus any FileHandling/ClientServices if still claimed).
3. Export CSV to `offline-scl/TSP_OFF_SCLVERIFY_2026-10-22_r3.csv`.
4. **DUT:** `push-5ln-product-cfg-to-dut.ps1` so runtime `configRev` matches.

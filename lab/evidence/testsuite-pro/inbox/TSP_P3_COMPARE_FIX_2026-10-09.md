# TSP Compare Model — fix pack (2026-10-09)

**Input evidence:** `TSP_P3_COMPARE_2026-10-09.xlsx` (489 rows from `New folder/ss.xlsx`)  
**CID/cfg:** `lab_tg544_eth_a` **rev 8** (3548437 + rev 7 LLN0) · DUT `ccli` r52 · `model=full_cid_cfg`

## Root cause (from export)

| Cluster | Count | Cause |
|---------|------:|-------|
| Gen* / St / SGG **PPV/A** dual Missing | ~420 | LNType **MMXU2** declared DEL/WYE; TSP Compare treats SCL **SDO** ≠ discovery **DO** → MissingFromDiscover **and** MissingFromFile for same path |
| `PdCMMXU1` PPV/A dual Missing | ~60 | Same SDO/DO artefact on **MMXU1** (kept — Annex T PdC needs PPV/A) |
| OptFlds **Value** | 8 | URCB `entryID=true` in CID vs runtime options; cfg had `options=255` |
| `NamPlt.configRev` Value | 1 | TSP File used edited CID (**20261009**); repo/DUT = **20261002** |

Advanced Client already showed `PdCMMXU1.PPV.phsAB` live — not a missing-DO firmware bug.

## Fixes applied

1. **CID `MMXU2`:** remove `PPV` / `A` (Gen* / St / SGG remain TotW/TotVAr only; datasets already TotW-only).  
2. **CID URCB `OptFields`:** `entryID="false"` (keep seqNum, timeStamp, dataSet, reasonCode, dataRef, configRef).  
3. **Regen** `lab_tg544_eth_a.cfg` — URCB `RC(... options=191)` (was 255); **one** `DO(PPV)` (PdC only).  
4. **Deploy** cfg to DUT `/etc/ccli/icd/` · restart `ccli` · confirm `full_cid_cfg` + `:3782`.

## Re-test on TSP (PC-B)

1. Import **repo** `lab_tg544_eth_a.cid` (Header **revision 8** · `configRev` **20261009**).  
2. Disconnect → Connect `:3782` (fresh discovery after DUT restart).  
3. URCB **disabled**.  
4. Compare Model → export `TSP_P3_COMPARE_AFTER_2026-10-09.xlsx`.

**Expect:** ~50–70 rows left (mostly `PdCMMXU1` PPV/A SDO/DO dual + optional BRCB OptFlds), not 489.  
**Waive** remaining PdC DEL/WYE dual-Missing (TSP artefact; browse proves presence).

## Related

- `TSP_P5_COMPARE_WAIVERS.md` · `PIXIT_DRAFT.md` OptFields · `SCL_DATASET_PLACEMENT.md`

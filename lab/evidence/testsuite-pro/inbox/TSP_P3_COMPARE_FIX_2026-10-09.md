# TSP Compare Model — fix pack (2026-10-09)

**Input evidence:** `TSP_P3_COMPARE_2026-10-09.xlsx` (489 rows from `New folder/ss.xlsx`)  
**CID/cfg:** `lab_tg544_eth_a` History **rev 9** · `configRev` **20261009** · DUT `ccli` r52 · `model=full_cid_cfg`  
**Pin:** [LAB_SPECIMEN_VERSION.md](../../../conformance/LAB_SPECIMEN_VERSION.md) · cfg SHA-256 **E3285E8F…D77E49**

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

## After re-test (2026-10-09)

**Evidence:** `TSP_P3_COMPARE_AFTER_2026-10-09.xlsx` (source: `New folder/Second error.xlsx`) · **69 errors** (+ header = 70 rows)

| Metric | Before (`ss.xlsx`) | After |
|--------|-------------------:|------:|
| Total error rows | 489 | **69** |
| MissingFromDiscover | 432 | **54** |
| MissingFromFile | 48 | **6** |
| Value | 9 | **9** |
| Gen* / St / SGG PPV/A rows | ~424 | **0** |

**Verdict:** **MMXU2 / Gen* Compare fix confirmed.** Remaining structure noise is **PdC only** (`PdCMMXU1.PPV` / `.A` — 6× MissingFromFile on `phs*` nodes + 48 deep-path MissingFromDiscover; same SDO/DO waiver as P5).

**Value rows (9) — fixed in CID History rev 9**

| Object | Fix |
|--------|-----|
| `LLN0.NamPlt.configRev` | **20261009** in CID + regen cfg (match TSP + DUT after deploy) |
| URCB ×6 `OptFlds` | CID `entryID=false` + cfg **`options=159`** (`gen-mms-model-cfg.ps1` patch) |
| BRCB ×2 `OptFlds` | CID `entryID=false` + cfg **`options=191`** |

**Re-test:** `git pull` → import repo CID **rev 9** → deploy cfg (SHA `E3285E8F…D77E49`) → restart `ccli` → Disconnect/Connect → Compare.  
**Expect:** **~60** structure rows (PdC PPV/A waive only), **0** Value rows.

## Related

- `TSP_P5_COMPARE_WAIVERS.md` · `PIXIT_DRAFT.md` OptFields · `SCL_DATASET_PLACEMENT.md`

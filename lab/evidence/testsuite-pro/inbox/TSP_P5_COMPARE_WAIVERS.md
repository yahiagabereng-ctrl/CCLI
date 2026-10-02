# TSP P5 — Compare Model waivers (lab)

**Gate:** P5_FULLCIRCLE  
**Date:** 2026-09-30  
**Build:** 0.1.0-r25 (P5-COMPARE-FIX) · prior r24 Compare export had **77** rows  
**CID:** `apps/ccli/config/icd/lab_tg544_eth_a.cid`  
**Matrix:** `lab/evidence/phase5/P5_LN_WIRE_STATUS.md`

## Scope

Test Suite Pro **Compare Model** against CID on live DUT (`192.168.10.1:102`, TLS off).  
Structure must match; **value** deltas on live DOs are expected and waived for lab.

## Pass criteria (structure)

| Check | Expected | Fail if |
|-------|----------|---------|
| LN count (GetNameList) | **31** | 9 (MVP deploy) |
| LN names vs CID | MATCH | any MISSING |
| DO tree vs CID | MATCH (genconfig from CID) | DO MISSING |
| DUT model mode | `model=full_cid_cfg` | `model=mvp_handbuilt` |

## Waived value deltas

| Object | CID / static | Live (r24) | Waiver ID | Reason |
|--------|--------------|------------|-----------|--------|
| `PdCMMXU1.TotW` | — | Modbus P (~450 kW) | LN-GAP-01 | Lab POC meter path |
| `PdCMMXU1.TotVAr` | — | Modbus Q (~45 kvar baseline) | LN-GAP-01 | Lab POC meter path |
| `PdCMMXU1.PPV.phsAB` | — | 20.0 kV (yaml) | LN-GAP-06 | No Modbus V reg yet (P5-06) |
| `PdCMMXU1` q | good | Questionable/Inaccurate | LN-GAP-07 | **CLOSED** — GNSS sky lock; re-Compare may show Good q |
| `PdCMMXU1.PPV` q | matches TotW | Good (stale) vs Questionable | LN-GAP-08 | PPV.q not in `refresh_time_quality()` |
| `WlimDWMX1.Mod` | off (CID) | **5** @ boot, **1** after Operate | LAB-CTL-01 | cfg default + lab direct-enhanced |
| `WSdDAGC1.Mod` | on (CID) | **1** @ boot (r24 cfg) | LAB-CTL-02 | cfg vs CID enum naming |
| `WSdDAGC1.WSptPct` | — | **20** (cfg) | LAB-VAL-01 | yaml seed |
| `GenPVMMXU1.TotW` | field PV | **0** static | PLANT-GAP-01 | No Sun2000 RS485 bus in lab |
| `Gen*`, `St*`, `Dis*` MMXU/DPCC | nameplate | cfg static | PLANT-GAP-02 | No plant sim beyond POC |
| `VArSdDVAR1` @ baseline | — | Mod=**5** until Operate | LAB-CTL-03 | cfg off until DSO command |
| `PFSPDFPF1`, `VArVDVVR1`, `PFWDPFW1` | — | Mod=**5**, no plant path | P5-DEFER-01 | Annex T Tab.88–91 deferred |
| ctlModel | ~~SBO (CID)~~ **fixed r25** | direct-enhanced | — | CID aligned 2026-10-01 |

## Offline SCL (related)

SCL Verify: **75** total, **5** errors — lab **PASS** with waivers per  
`lab/evidence/testsuite-pro/offline-scl/TSP_OFF_SCLVERIFY_2026-09-26_r2_TRIAGE.md`.

## r24 delta (P5-R01)

| Object | Before (r23) | After (r24) |
|--------|--------------|-------------|
| `VArSdDVAR1` Operate | stub — no plant effect | **OK** — FC16 Q write, `mms→plant Q≈21 kvar` @ 10% × 210 kVA |
| Control handlers | 2 (Wlim, WSd) | **3** (+ VArSd) |

## Operator note

Use URCB path `LLN0.RP.urcb_PdC_Mis4sec01` (not bare `urcb_PdC_Mis4sec01`).

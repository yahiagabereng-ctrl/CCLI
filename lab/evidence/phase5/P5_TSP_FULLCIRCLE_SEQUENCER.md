# P5 — TSP full-circle sequencer (Annex-traced)

**Gate:** P5_FULLCIRCLE  
**Date:** 2026-10-01  
**Build:** `0.1.0-r25` (P5-COMPARE-FIX)  
**Annex sequence:** [P5_ANNEX_TEST_SEQUENCE.md](P5_ANNEX_TEST_SEQUENCE.md)  
**Matrix:** [P5_LN_VALUE_MATRIX.md](P5_LN_VALUE_MATRIX.md)  
**Timing / LN audit:** [P5_TIMING_ANNEX_AUDIT_2026-10-01.md](P5_TIMING_ANNEX_AUDIT_2026-10-01.md)  
**DUT:** `192.168.10.1:102` TLS **OFF**  
**CID:** `apps/ccli/config/icd/lab_tg544_eth_a.cid`  
**Modbus:** `python -u lab/modbus_rtu_slave.py --port COM5 --power-kw 450 --trace`  
**Gate status:** **CLOSED** (lab) 2026-10-01 · build **0.1.0-r25**

> **Order:** Run Compare (step 12) **before** URCB enable (step 5) to avoid RCB OptFlds Compare noise.

### Verification client

| Tool | When | Notes |
|------|------|-------|
| **Test Suite Pro** | Licensed lab PC | Compare Model, Report Viewer, Sequencer export |
| **[IEDExplorer](https://sourceforge.net/projects/iedexplorer/)** | TSP license blocked | Connect/read/Operate/URCB — **no Compare**; reuse r25 Compare waivers |
| **Automated scripts** | CI / repeatability | `run-p5-fullcircle-cleartext.ps1` → `P5_FULLCIRCLE_CLEARTEXT*.txt` |

See [P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md](P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md).

---

## Metadata (fill before export)

```text
tsp_version:
pc_ip: 192.168.10.10
dut_ip: 192.168.10.1
dut_port: 102
tls: false
cid: apps/ccli/config/icd/lab_tg544_eth_a.cid
ccli_pkg: 0.1.0-r25
gate: P5_FULLCIRCLE
tool: SEQ
operator:
annex_seq: P5-ANNEX-SEQ-001
```

---

## Preconditions (P5-00)

- [x] `run-p5-fullcircle-cleartext.ps1` deployed (r25)
- [x] Modbus slave P=450 kW, Q=45 kvar
- [x] DUT: `mms: listening on 0.0.0.0:102 tls=off` (TesPro loopback)
- [x] DUT: `chronyc tracking` → \|offset\| ≤ 100 ms (**T.3.3.4.5**)
- [ ] Do **not** load TLS certs for this session

---

## Steps

| # | Annex ref | REQ-ID | TSP tool | Action | Object / note | Pass criteria |
|---|-----------|--------|----------|--------|---------------|---------------|
| 0 | — | — | DUT shell | `chronyc tracking` | UTC ±100 ms | **PASS** if ≤100 ms |
| 1 | **T.3.3.3** | — | Advanced Client | Connect | IED `CCI016_01` | Connected |
| 12b | **T.3.1** · TR | REQ-LN-003 | Advanced Client | GetNameList | 31 LNs under `LD_Plant` | FAIL if count=9 |
| 12 | TR 57-126 | REQ-P5-008 | Compare Model | vs CID | **Before step 5**; r25 ctlModel/BRCB | See Compare waivers |
| 2 | **T.3.1.3** | REQ-P5-002 | Advanced Client | Read | `PdCMMXU1.TotW` | ≈450 kW (±0.5) |
| 3 | **T.3.1.3** | REQ-P5-002 | Advanced Client | Read | `PdCMMXU1.TotVAr` | ≈45 kvar (±0.5) |
| 4 | **T.3.1.3** | REQ-P5-002 | Advanced Client | Read | `PdCMMXU1.PPV.phsAB` | 20.0 kV |
| 5 | **T.3.2.1** · **O.8.3** | REQ-MET-001 | Report Viewer | Enable URCB | `LLN0.RP.urcb_PdC_Mis4sec01` | IntgPd=4000; ~4 s reports |
| 5b | **O.8.3** | REQ-LN-004 | Report Viewer | Inspect payload | TotW/TotVAr/PPV same report | **t identical**; Δt 3900–4100 ms |
| 5c | **O.8.3** | REQ-MET-002 | Report Viewer | UTC seconds | 10+ integrity reports | Log sec mod 4 (**GAP** expected in lab) |
| 5d | **T.3.3.4.5** | REQ-LN-005 | Report Viewer | Quality fields | TotW.q, TotVAr.q, PPV.q | **Good** on TotW/TotVAr if GNSS locked; PPV waiver **LN-GAP-08** |
| 6 | **T.3.1.4** Tab.85 | REQ-LN-006 | Advanced Client | Read | `WlimDWMX1.Mod` | **5** @ baseline (Annex default) |
| 7 | **T.3.1.4** Tab.86 | REQ-LN-006 | Advanced Client | Read | `WSdDAGC1.Mod` | **1** r23 cfg (Annex default 5 — note) |
| 8 | **O.9.2.2** | REQ-P5-004 | Advanced Client | Direct Operate | `WlimDWMX1`: Mod=1, WMaxSptPct=10 | OK; DIO curtail |
| 9 | **O.9.2.3** | REQ-P5-005 | Advanced Client | Direct Operate | `WSdDAGC1`: Mod=1, WSptPct=20 | OK |
| 10 | **T.3.1.4** Tab.88–91 | — | Advanced Client | Read | stub Mod (PFSP, VArV, PFW); `VArSd.Mod` | all **5** @ baseline |
| 11 | **O.9.1.4** | REQ-P5-006 | Advanced Client | Direct Operate | `VArSdDVAR1`: VArTgtSptPct=10, Mod=1 | OK; Q≈21 kvar; TotVAr tracks |
| 13 | **T.3.1.3** | REQ-P5-002 | Manual | Modbus P sweep | 400→500 kW | TotW tracks |
| 14 | P3-05 / **O.11** | — | Advanced Client | Disconnect 15 s | — | Mod→5; PF2 yaml 42 kW |

---

## TesPro northbound (LuCI — after TSP or parallel)

**Guide:** [P5_TESPRO_NORTHBOUND.md](P5_TESPRO_NORTHBOUND.md) · Annex seq Part **H**

| # | Annex ref | REQ-ID | Tool | Action | Pass criteria |
|---|-----------|--------|------|--------|---------------|
| NB0 | — | REQ-NB-000 | LuCI | IEC 61850 service Running | `iec61850d` up |
| NB1 | **T.3.1.3** | REQ-NB-001 | LuCI Device | `CCLI_LAB` / `192.168.10.1:102` | Realtime **PdC_TotW≈450** |
| NB2 | — | REQ-NB-002 | LuCI + PC | Config frozen [P5_TESPRO_NORTHBOUND_2026-10-01.md](P5_TESPRO_NORTHBOUND_2026-10-01.md); **`install-mqtt-lab-broker.ps1`** (Admin) | LuCI **Connected** |
| NB3 | **T.3.1.3** | REQ-NB-003 | PC | `.\lab\tg544-openwrt\verify-tespro-northbound.ps1` | JSON `PdC_TotW` ≈ 450 kW |

**Status:** **PASS** 2026-10-01 — see [P5_TESPRO_NORTHBOUND_2026-10-01.md](P5_TESPRO_NORTHBOUND_2026-10-01.md).

---

## Annex clause quick reference

| Steps | Annex O | Annex T |
|-------|---------|---------|
| 5, 5b–d | **O.8.3** (4 s, t, q) | **T.3.2.1**, **T.3.1.3** |
| 0, 5d | — | **T.3.3.4.5** (UTC ±100 ms) |
| 2–4 | PF1 observability | **T.3.1.3** PdC MMXU1 |
| 8 | **O.9.2.2** Wlim | Tab.85 DWMX |
| 9 | **O.9.2.3** WSd | Tab.86 DAGC |
| 11 | **O.9.1.4** VArSd | Tab.88 DVAR |
| 12 | — | TR 57-126 CID |

**Not in TSP (product track):** O.7.3.1 TsP/TsQ · O.7.3.3 3 s spacing · O.8.3 :00/:04 grid scheduler

---

## Export

```text
lab/evidence/testsuite-pro/inbox/TSP_P5_FULLCIRCLE_CLEARTEXT_<YYYY-MM-DD>_<HHMMSS>.txt
```

Include in export: chrony snapshot, report Δt table, t sec mod 4 notes, Compare row count.

---

## Revert product profile

```powershell
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr 192.168.10.1 `
  -LabYaml apps\ccli\config\lab_tr400_phase4_eth_b.yaml `
  -Changes @("revert TLS :3782 product profile")
```

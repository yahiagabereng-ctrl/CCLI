# TSP P5 — Full circle cleartext sequencer (draft)

**Gate:** P5_FULLCIRCLE  
**Date:** 2026-09-30  
**Matrix:** `lab/evidence/phase5/P5_LN_VALUE_MATRIX.md`  
**DUT:** `192.168.10.1:102` TLS **OFF**  
**CID:** `apps/ccli/config/icd/lab_tg544_eth_a.cid`  
**Modbus:** `python -u lab/modbus_rtu_slave.py --port COM5 --power-kw 450 --trace`

## Metadata (fill before export)

```text
tsp_version:
pc_ip: 192.168.10.10
dut_ip: 192.168.10.1
dut_port: 102
tls: false
cid: apps/ccli/config/icd/lab_tg544_eth_a.cid
ccli_pkg: 0.1.0-r24
gate: P5_FULLCIRCLE
tool: SEQ
operator:
```

## Preconditions

- [x] `run-p5-fullcircle-cleartext.ps1` deployed cleartext yaml (r24 2026-09-30)
- [x] Modbus slave running (P=450, Q=45)
- [x] DUT log: `mms: listening on 192.168.10.1:102 tls=off`
- [x] Do **not** load TLS certs for this session

## Steps

| # | TSP tool | Action | Object / note | Pass criteria |
|---|----------|--------|---------------|---------------|
| 1 | Advanced Client | Connect | IED `CCI016_01` | Connected |
| 2 | Advanced Client | Read | `PdCMMXU1.TotW` | ≈450 kW (±0.5) |
| 3 | Advanced Client | Read | `PdCMMXU1.TotVAr` | ≈45 kvar (±0.5) |
| 4 | Advanced Client | Read | `PdCMMXU1.PPV.phsAB` | 20.0 kV |
| 5 | Report Viewer | Enable URCB | `LLN0.RP.urcb_PdC_Mis4sec01` | Reports ~4 s; RptId ends `urcb_PdC_Mis4sec` |
| 6 | Advanced Client | Read | `WlimDWMX1.Mod` | **5** (cfg default; CID says off) |
| 7 | Advanced Client | Read | `WSdDAGC1.Mod` | **1** (r23 cfg; CID on) — MVP-only was 5 |
| 8 | Advanced Client | Direct Operate | `WlimDWMX1`: Mod=1, WMaxSptPct=10 | OK; DIO curtail |
| 9 | Advanced Client | Direct Operate | `WSdDAGC1`: Mod=1, WSptPct=20 | OK |
| 10 | Advanced Client | Read | stub Mod (PFSP, VArV, PFW) | all **5**; `VArSd.Mod` **5** @ baseline |
| 11 | Advanced Client | Direct Operate | `VArSdDVAR1`: VArTgtSptPct=10, Mod=1 | OK; DUT `mms→plant Q≈21 kvar`; TotVAr tracks |
| 12 | Compare Model | vs CID | **r23: 31 LN structure OK**; value diffs on live DOs | See `P5_LN_WIRE_STATUS.md` |
| 12b | Advanced Client | GetNameList | 31 LNs under LD_Plant | FAIL if count=9 (MVP deploy) |
| 13 | Manual | Modbus P sweep | 400→500 kW | TotW tracks |
| 14 | Advanced Client | Disconnect 15 s | — | Mod→5; PF2 yaml 42 kW |

## Export

Save execution log as:

```text
lab/evidence/testsuite-pro/inbox/TSP_P5_FULLCIRCLE_CLEARTEXT_<YYYY-MM-DD>_<HHMMSS>.txt
```

## Revert product profile

```powershell
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr 192.168.10.1 `
  -LabYaml apps\ccli\config\lab_tr400_phase4_eth_b.yaml `
  -Changes @("revert TLS :3782 product profile")
```

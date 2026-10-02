# P5 — Cleartext MMS full circle (TSP / IEDExplorer)

**Date:** 2026-10-01  
**Gate status:** **CLOSED** (lab) · build **0.1.0-r25**  
**Purpose:** Functional MMS + Modbus + Operate **without TLS** (lab bypass)  
**DSO client:** TSP (r25 session) or [IEDExplorer](https://sourceforge.net/projects/iedexplorer/) if TSP license blocked — [P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md](P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md)
**Product path:** TLS `:3782` unchanged — `P5_MMS_TLS.pcapng`  
**Phase index:** [P5_README.md](P5_README.md)  
**Annex test sequence:** [P5_ANNEX_TEST_SEQUENCE.md](P5_ANNEX_TEST_SEQUENCE.md)  
**TSP sequencer:** [P5_TSP_FULLCIRCLE_SEQUENCER.md](P5_TSP_FULLCIRCLE_SEQUENCER.md)  
**Timing / LN audit:** [P5_TIMING_ANNEX_AUDIT_2026-10-01.md](P5_TIMING_ANNEX_AUDIT_2026-10-01.md)

## Deploy cleartext config

```powershell
$env:CCLI_TG544_PW = "<root>"
.\lab\tg544-openwrt\run-p5-fullcircle-cleartext.ps1
```

Config: `apps/ccli/config/lab_tr400_cleartext_tsp.yaml`  
- MMS `192.168.10.1:102` · `tls_enabled: false`  
- Modbus `/dev/rs485_2_uart` · `permissive_bypass: true` (lab actuation)

## Modbus slave (PC)

```powershell
python -u lab\modbus_rtu_slave.py --port COM5 --power-kw 450 --trace
```

## Test Suite Pro connection

| Field | Value |
|-------|--------|
| IED | `CCI016_01` |
| IP | `192.168.10.1` |
| Port | **102** |
| TLS / security | **Off** |
| CID | `apps/ccli/config/icd/lab_tg544_eth_a.cid` |

**Do not** use `C:\CCLI_tls\` certs for this session.

## LN / value matrix (canonical)

**`P5_LN_VALUE_MATRIX.md`** — every runtime DO, expected values, Modbus map, PF2 thresholds, CID gaps, TSP pass criteria.

**Execute:** [P5_TSP_FULLCIRCLE_SEQUENCER.md](P5_TSP_FULLCIRCLE_SEQUENCER.md) (Annex-traced; Compare **before** URCB).

## Sequencer steps (summary)

0. **chronyc tracking** on DUT — \|offset\| ≤ 100 ms (**T.3.3.4.5**)
1. **Connect** — TLS off, port **102**
2. **GetNameList** — **31** LNs (**T.3.1** / TR)
3. **Compare Model** — vs CID **before** URCB enable
4. **Read** `PdCMMXU1.TotW` / `TotVAr` / `PPV.phsAB` → **450 / 45 / 20** (**T.3.1.3**)
5. **EnableReport** — `LLN0.RP.urcb_PdC_Mis4sec01`; verify Δt ~4 s + t/q (**O.8.3**)
6. **Read** baseline Mod — Wlim/WSd/VArSd (**T.3.1.4** defaults)
7. **Direct Operate** Wlim / WSd / VArSd (**O.9.2.2**, **O.9.2.3**, **O.9.1.4**)
8. Modbus P sweep · disconnect 15 s fallback

## TesPro northbound MQTT (Part H — **PASS** 2026-10-01)

Frozen config and verify script: **[P5_TESPRO_NORTHBOUND_2026-10-01.md](P5_TESPRO_NORTHBOUND_2026-10-01.md)**

```powershell
.\lab\tg544-openwrt\install-mqtt-lab-broker.ps1   # Admin, once
.\lab\tg544-openwrt\verify-tespro-northbound.ps1    # exit 0
```

Export log → `lab/evidence/phase5/P5_TESPRO_NORTHBOUND_*.txt` or `testsuite-pro/inbox/TSP_P5_FULLCIRCLE_CLEARTEXT_*.txt`

## Revert to product TLS profile

```powershell
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr 192.168.10.1 `
  -LabYaml apps\ccli\config\lab_tr400_phase4_eth_b.yaml `
  -Changes @("revert TLS :3782 product profile")
```

## Evidence files

| File | Content |
|------|---------|
| `P5_README.md` | Phase 5 index |
| `P5_ANNEX_TEST_SEQUENCE.md` | Annex O/T → test steps |
| `P5_TSP_FULLCIRCLE_SEQUENCER.md` | TSP operator checklist |
| `P5_TIMING_ANNEX_AUDIT_2026-10-01.md` | Timing + LN alignment audit |
| `P5_LN_VALUE_MATRIX.md` | Complete LN/value matrix + TSP pass criteria |
| `P5_00_LN_MATRIX_AUDIT.md` | CID vs runtime audit |
| `P5_FULLCIRCLE_CLEARTEXT*.txt` | Auto script (clients + DUT logs) |
| `TSP_P5_FULLCIRCLE_CLEARTEXT_*.txt` | TSP export (inbox) |

# P5 — Test Suite Pro cleartext full circle

**Date:** 2026-09-30  
**Purpose:** Functional MMS + Modbus + Operate via TSP **without TLS** (lab bypass)  
**Product path:** TLS `:3782` unchanged — `P5_MMS_TLS.pcapng`

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

Sequencer copy: `lab/evidence/testsuite-pro/inbox/TSP_P5_FULLCIRCLE_SEQUENCER.md`

## Sequencer steps (summary)

1. **Connect** — TLS off, port **102**
2. **Read** `PdCMMXU1.TotW` / `TotVAr` / `PPV.phsAB` → **450 / 45 / 20**
3. **EnableReport** — `LLN0.RP.urcb_PdC_Mis4sec01` (genconfig name; RptId `…/LLN0.urcb_PdC_Mis4sec`, IntgPd 4000)
4. **Read** baseline `WlimDWMX1.Mod` / `WSdDAGC1.Mod` → **5** (LN-GAP-05: PF2 still uses yaml 42 kW)
5. **Direct Operate** `WlimDWMX1`: Mod=1, `WMaxSptPct=10` (or 70 per matrix §C)
6. **Direct Operate** `WSdDAGC1`: Mod=1, `WSptPct=20`
7. **Read** stub Mod (VArSd/PFSP/VArV/PFW) → all **5**; negative Operate on VArSd
8. **Compare Model** — expect gaps vs full CID (runtime 9 LN only)
9. DUT: `ubus call dido_v2 status` · curtail in `/tmp/ccli.log`

Export log → `lab/evidence/testsuite-pro/inbox/TSP_P5_FULLCIRCLE_CLEARTEXT_*.txt`

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
| `P5_LN_VALUE_MATRIX.md` | Complete LN/value matrix + TSP pass criteria |
| `P5_00_LN_MATRIX_AUDIT.md` | CID vs runtime audit |
| `P5_FULLCIRCLE_CLEARTEXT.txt` | Auto script (clients + DUT logs) |
| `TSP_P5_FULLCIRCLE_SEQUENCER.md` | TSP step list (inbox) |
| `TSP_P5_FULLCIRCLE_CLEARTEXT_*.txt` | Your TSP export (manual) |

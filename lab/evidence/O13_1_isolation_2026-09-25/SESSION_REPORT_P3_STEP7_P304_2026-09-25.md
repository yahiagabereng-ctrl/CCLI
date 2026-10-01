# Session report — Phase 3 Step 7 / P3-04 (Wlim → actuator)

**Date:** 2026-09-25  
**DUT:** TesPro TG544 · Eth_A `192.168.10.1:102`  
**Package:** `ccli-0.1.0-r6`

## Goal

Prove DSO MMS write of **`Wlim`** (O.9.2.2) slaves the same PF2 → **DIO1** path as Phase 1.

## What ran

| Step | Action | Result |
|------|--------|--------|
| Build | OpenWrt SDK → `ccli-0.1.0-r6.apk` / `ccli-bin` | OK |
| Deploy | Binary to `/usr/sbin/ccli` via LAN1 SSH | OK |
| Server | `--lab-demo` + `permissive_bypass` (lab) | MMS listen + Wlim handlers |
| Client | `mms_wlim_client … 10` (WMaxSptPct=10, Mod=1) | OPERATE OK |
| Apply | `mms→pf2` enter=21 / release=16 kW | PASS |
| Actuate | `io: DO curtail_on` · ubus ch1 `state=1` | PASS |

## Evidence

| File | Content |
|------|---------|
| `P3_04_WLIM_ACTUATOR.txt` | Verdict pack |
| `P3_04_WLIM_CLIENT.txt` | Client operate transcript |
| `P3_04_CCLI_SERVER.log` | Server Wlim + DO lines |
| `P3_04_DIO1_STATUS.txt` | ubus DIO1 state=1 |

## Checklist

| ID | Status |
|----|--------|
| P3-01 | PASS (prior) |
| P3-03 | PASS (prior) |
| **P3-04** | **PASS** |
| P3-06 TLS | OPEN (next for Phase 3 exit) |

## Next

1. **P3-06** — TLS 1.2+ on Eth_A MMS (cleartext still lab-only).  
2. Phase 3 exit = P3-01 + P3-04 + P3-06.  
3. Product follow-ups: SBO ctlModel, drop permissive_bypass, plant Modbus for live TotW.

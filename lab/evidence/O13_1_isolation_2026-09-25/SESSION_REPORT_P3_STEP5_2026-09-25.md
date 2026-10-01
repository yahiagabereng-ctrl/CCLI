# Session report — Phase 3 Step 5 (MMS client / 4 s TotW)

**Date:** 2026-09-25  
**DUT:** TG544 · MMS server `192.168.10.1:102` (P3-01 already PASS)  
**Client:** `lab/mms_lab_client.c` (libiec61850, WSL host build)

---

## Verdict

| Check | Result |
|-------|--------|
| Connect MMS | **PASS** |
| Browse `CCI016_01LD_Plant` / `WlimDWMX1` / `PdCMMXU1` | **PASS** |
| Read `PdCMMXU1.TotW` | **PASS** (value 0.0 — no live Modbus P) |
| RCB `urcb_PdC_Mis4sec` IntgPd=4000 | **PASS** |
| Periodic reports ~4 s | **PASS** — 6 reports, **avg 3961 ms** |
| **P3-03** | **PASS** |

---

## How to reproduce

```bash
# WSL
bash lab/tg544-openwrt/build-mms-lab-client.sh
./lab/tg544-openwrt/mms_lab_client 192.168.10.1 102 20
```

PC on LAN1 (`192.168.10.10`). TG `ccli` must be running with `mms.enabled: true`.

---

## Evidence files

- `lab/evidence/O13_1_isolation_2026-09-25/P3_03_MMS_CLIENT_STEP5.txt`
- `lab/evidence/O13_1_isolation_2026-09-25/P3_03_MMS_CLIENT_STEP5.log`
- Pointer: `lab/evidence/P3_03_MMS_CLIENT_STEP5_LATEST.txt`

---

## Phase 3 progress

| Step | Status |
|------|--------|
| 1 LAN1 access | PASS |
| 2 Lab CID IP | PART |
| 3 signal_map freeze | PART (P3-12) |
| 4 MMS listen Eth_A | **PASS (P3-01)** |
| 5 Client + 4 s TotW | **PASS (P3-03)** |
| 6–7 Wlim → actuator | **PASS** P3-04 — see `SESSION_REPORT_P3_STEP7_P304_2026-09-25.md` |
| TLS | OPEN (P3-06…) |

---

## Next

1. Drive non-zero TotW (Modbus slave / live P) — optional quality for P3-03  
2. **P3-06 TLS** — remaining Phase 3 exit gate (P3-01 + P3-04 PASS)  
3. TLS before external DSO  

*End of Step 5 report.*

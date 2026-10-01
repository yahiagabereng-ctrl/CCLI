# Session report — Phase 3 soft close (P3-05 / P3-08 / P3-09 / P3-11)

**Date:** 2026-09-25  
**DUT:** TG544 · `ccli-0.1.0-r15` · Eth_A `192.168.10.1:3782`  
**Annex:** T.3.3.4 cyber profile · O.9.2.2 no-comms · T.3.3.4.5 time

## Results

| Check | Status | Evidence |
|-------|--------|----------|
| P3-05 Operating Rule fallback | **PASS** (lab) | `P3_05_COMMS_LOSS_FALLBACK.txt` |
| P3-08 RBAC (DSO vs VIEWER) | **PASS** (lab) | `P3_08_SECURITY_RBAC.txt` |
| P3-09 CA + CRL | **PASS** (lab) | `P3_08_09_RBAC_PKI.txt` |
| P3-11 TimeQuality ±100 ms | **PASS** (lab) | chrony + `P3_11_CHRONY_TRACKING.txt` |

## Residuals (named — not exit blockers)

| Item | Status | Owner note |
|------|--------|------------|
| 62351-4 §13 native E2E | DEFERRED | libiec61850 MVP; PICS / K6.5 |
| P3-09 full SCEP/EST | DEFERRED | product PKI ceremony |
| P3-10 XCBR1.IDG | WAIVED (lab) | product model |
| P3-12 plant ICD workbook | PART | `signal_map.yaml` MVP only |
| P3-13/14 Ed.2 / 7-3 | PART/WAIVED | corpus |
| GNSS PPS preferred source | DEFERRED | C5; chrony uses NTP today |

## Phase 3 exit

**CLOSED** (lab) 2026-09-25 — see `SESSION_REPORT_P3_LAB_CLOSE_2026-09-25.md`.

## Next

**Phase 4** — IEC 60870-5-104 on Eth_B (P4-01…03).

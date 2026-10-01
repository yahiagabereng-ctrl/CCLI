# Session report — Phase 3 P3-07 (62351-4 mutual cert association)

**Date:** 2026-09-25  
**DUT:** TG544 · `192.168.10.1:3782` · `ccli-0.1.0-r8`  
**Clause:** Annex T · **IEC 62351-4** (lab MVP)

## Goal

Mutual certificate authentication for the MMS security association (checklist: Mutual cert auth).

## Results

| Check | Result |
|-------|--------|
| Server | `tls=on acse_tls_auth=on` |
| ACSE | `ACSE auth ACCEPT — TLS cert (863 octets)` |
| Allowed client | CONNECT OK |
| No client cert | CONNECT FAIL err=5 |
| **P3-07** | **PASS** (lab) |

## Evidence

- `P3_07_SECURITY_ASSOC.txt`
- `P3_07_TLS_MUTUAL_OK.txt`
- `P3_07_TLS_NOCLIENT_FAIL.txt`

## Deferred

Full **62351-4 §13** native E2E (`e2eMmsAC` / ClearToken) — not in libiec61850 MVP; track as product follow-up (PICS / K6.5).

## Next

P3-08 RBAC · P3-09 PKI · or **Phase 4** 104 on Eth_B.

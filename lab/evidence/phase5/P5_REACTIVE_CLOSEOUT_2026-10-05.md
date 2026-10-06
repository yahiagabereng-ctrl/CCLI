# P5-R — Reactive regulation closeout (lab)

**Date:** 2026-10-05  
**Gate:** P5-R (O.9.1 / O.7.3 / O.11 reactive tier)  
**DUT:** TesPro TG544 @ `192.168.10.1`  
**Build:** **0.1.0-r31** (P5-R02-PFSP)  
**Companion:** [P5_FINAL_CLOSEOUT.md](P5_FINAL_CLOSEOUT.md) (P5-M full-circle r25 + GOOSE r29)

---

## Requirements

| ID | Requirement | Status |
|----|-------------|--------|
| REQ-P5-R-001 | DSO VArSd Operate → plant Q (O.9.1.4) | **PASS** r25/r30 |
| REQ-P5-R-002 | DSO PFSP Operate → plant Q from cosφ (O.9.1.1) | **PASS** r31 |
| REQ-P5-R-003 | O.14 reactive Operate logging | **PASS** r30/r31 |
| REQ-P5-R-004 | External set-point spacing ≥ 3 s (O.7.3.3) | **PASS** r29+ gate |
| REQ-P5-R-005 | VArV / PFW curve engines (O.9.1.2/3) | **WAIVED** — TR Figura 2 Inactive |
| REQ-P5-R-006 | TsQ ≤ 10 s bench (O.7.3.1) | **PART** — unit logic; runtime bench → product |
| REQ-P5-R-007 | Full O.11 reactive vs active arbiter | **PART** — PFSP > VArSd same tick |

---

## P5-R register (final lab)

| Row | Clause | Evidence | Verdict |
|-----|--------|----------|---------|
| **P5-R01** | O.9.1.4 VArSd | `P5_FINAL_CLOSEOUT` · r25 full-circle | **PASS** |
| **P5-R02** | O.9.1.1 PFSP | [P5_R02_PFSP_EVENT_2026-10-05.txt](P5_R02_PFSP_EVENT_2026-10-05.txt) | **PASS** |
| **P5-R03** | O.9.1.3 VArV Q(V) | TR Figura 2 **Inactive** · stub LN on wire | **WAIVED** |
| **P5-R04** | O.9.1.2 PFW cosφ(P) | TR Figura 2 **Inactive** · stub LN on wire | **WAIVED** |
| **P5-R05** | O.7.3.1 TsQ | `dso_phase1_test` PFSP clamp; no runtime timer | **PART** → product |
| **P5-R06** | O.7.3.2/3 spacing | r29+ log `set-point spacing gate ON (3 s)` | **PASS** |
| **P5-R07** | O.11 indices 5–7 | PFSP priority over VArSd (`ccli_main.cpp`) | **PART** → product |
| **P5-R08** | O.10.3.2 VArSa MSD | Discretionary | **N/A** |
| **P5-R09** | O.14 logger | [P5_R09_VARSD_EVENT_2026-10-05.txt](P5_R09_VARSD_EVENT_2026-10-05.txt) | **PASS** |

### WAIVE rationale (R03 / R04)

Per [lab/CCI_Figura2_Parameters.md](../../CCI_Figura2_Parameters.md) Table 1: **VArSd, PFSP, VArV, PFW** are **Not operative / Inactive** in the TR Figura 2 reference plant. Lab implemented **VArSd + PFSP** for DSO path proof; **VArV** and **PFW** remain MMS structure stubs without curve engines — acceptable **WAIVE** per checklist exit rule (*PASS or WAIVE per Operating Rule / TR Figura 2*).

---

## P5-G register (unchanged from GOOSE session)

| Row | Verdict | Note |
|-----|---------|------|
| **P5-G01** subscribe | **PARKED** | LAN3 not cabled · [P5_GOOSE_SESSION_RECORD_2026-10-05.md](P5_GOOSE_SESSION_RECORD_2026-10-05.md) |
| **P5-G02** publish | **PASS** | Wire pcap + GoCB MMS |
| **P5-G03** merge | **PASS** (policy) | Runtime merge **OPEN** → product |

---

## Residual (product — not P5 lab blockers)

| Item | Owner | Track |
|------|-------|-------|
| VArV / PFW curve + slow ring | Firmware | Post-contract Operating Rule |
| TsQ runtime timer + bench | Lab/product | `regulation_bench` |
| Full O.11 reactive priority matrix | Firmware | `derive_*` extension |
| P5-G01 GOOSE subscribe | Lab | LAN3 wire day |
| P5-G03 runtime measurement merge | Firmware | `P5_G03_MERGE_POLICY.md` |
| P5-04 accuracy / P5-06 meter map | Metrology | Phase 5-M product |
| REQ-MET-002 grid alignment | Firmware | Boundary scheduler |

---

## Verification summary

| Requirement | Test | Pass criteria | Result |
|-------------|------|---------------|--------|
| REQ-P5-R-001 | `mms_varsd_client` + COM5 | FC16 Q + `varsd_operate result=ok` | **PASS** |
| REQ-P5-R-002 | `mms_pfsp_client` + COM5 | FC16 Q + `pfsp_operate result=ok` | **PASS** |
| REQ-P5-R-003 | `--event-dump` | mod/cosphi or pct/q_kvar/result | **PASS** |
| REQ-P5-R-004 | Deploy log | spacing gate ON 3 s | **PASS** |
| REQ-P5-R-005 | Figura 2 matrix | Inactive → WAIVE documented | **WAIVED** |

**P5-R lab gate: CLOSED** 2026-10-05 on build **0.1.0-r31**.

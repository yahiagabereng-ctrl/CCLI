# Annex O → software (5-LN DUT traceability)

**Document ID:** CCLI-LAB-ANNEX-O-SW-001  
**Revision:** 1.0  
**Date:** 2026-10-20  
**RAG source_id:** `ccli-annex-o-software-matrix`  
**Normative:** CEI 0-16 Allegato O (consolidated **2025-12**) — `cei-0-16-allegato-o`  
**Specimen:** [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md) · `product_tr400_eth_a_5ln.cid`  
**Cyber / comms detail:** Annex T — [CCI_Annex_T_Extract.md](../../knowledge-base/08-engineering/CCI_Annex_T_Extract.md)

**Legend — 5-LN DUT column**

| Symbol | Meaning |
|--------|---------|
| **PASS** | Exercised on bench with evidence |
| **PART** | Partial implementation or lab waiver |
| **—** | Not a runtime DUT check (documentation / architecture only) |
| **GAP** | Spec applies; software or evidence missing |
| **N/A** | Out of 5-LN cert scope or not claimed |

**Legend — Gap column**

| Value | Meaning |
|-------|---------|
| **None (spec)** | Requirement covered by docs / architecture narrative |
| **None (bench)** | Meets lab intent for current 5-LN session |
| Text | Open engineering or evidence item |

---

## Matrix

| Clause | Shall (summary) | SW (implementation / evidence) | 5-LN DUT | Gap |
|--------|-----------------|----------------------------------|----------|-----|
| **O.1–O.4** | Scope, CCI role, applicability thresholds, PF1/PF2/PF3 classes | Docs / architecture — `CCI_Annex_O_Extract.md`, `CCI_Product_SW_Freeze.md`, `PRODUCT_SPECIMEN_5LN.md` | — | None (spec) |
| **O.6** | PF1 mandatory; PF2/PF3 per plant class and ARERA | `ccli` service map; yaml profiles; PF2 FSM (local) | **PART** | Full PF2/PF3 catalogue not all wired on 5-LN bench |
| **O.7.3.1** | Fast ring TsP ≤ 60 s, TsQ ≤ 10 s, ±5 % at POC | Control / allocation logic (future O.11 path) | **GAP** | No closed-loop POC settling proof on 5-LN |
| **O.7.3.2** | Slow ring ΔT 10–600 s (default 60 s) | Q(V) / cosφ(P) schedulers — not in 5-LN MVP | **N/A** | Curve modules deferred |
| **O.7.3.3** | External set-point min interval **3 s** | MMS command rate / reject policy | **PART** | TSP spot-check only; no formal 3 s flood test |
| **O.7.4** | MC200 / MCΔT measurement blocks (61557-12) | Modbus → `PdCMMXU1`; 4 s URCB IntgPd | **PART** | MC200/MCΔT not 61557-12 certified blocks |
| **O.8.3** | P/Q/V to DSO every **4 s**, timestamp + quality | `urcb_PdC_Mis4sec` IntgPd=4000; report t/q | **PASS** | P5 / U-T1-05…06 evidence |
| **O.8.4–O.8.6** | TSO/aggregator observability, polygon, overwrite rules | 31-LN lab had GenPV/StMMXU; **5-LN trims** unit LNs | **N/A** | Cert model = POC measures only |
| **O.9.1.x** | Reactive curves (cosφ, Q(V), …) | `VArSdDVAR1` etc. — **not in 5-LN CID** | **N/A** | By PICS / specimen trim |
| **O.9.2.1** | P limit at ~110 % Vn | `WlimDWMX1` / PF2 thresholds in yaml | **PART** | 110 % Vn trip not fully demonstrated |
| **O.9.2.2** | P limitation on DSO command; **Operating Rule** if comms lost | `WlimDWMX1` + `comms_loss_fallback_s` | **PASS** | `P3_05_COMMS_LOSS_FALLBACK.txt` (15 s lab) |
| **O.9.2.3** | P modulation on DSO command | Slaved to set-point (O.10.3.1) | **GAP** | Not separated on 5-LN bench |
| **O.10.3.1** | Active power set-point (external) | `WSdDAGC1` · enhanced-security ctl | **PART** | T1-07/08 style; not full ARERA timing |
| **O.10.3.2** | Reactive set-point (external) | No reactive LN on 5-LN | **N/A** | — |
| **O.11** | Priority / compatibility among functions | O.11 arbitration — design in flowcharts | **GAP** | Software priority table not complete |
| **O.13.1.1.1** | External network (DSO Eth_A, operator Eth_B) | MMS **3782 TLS** · 104 **2404 TLS** · zone split | **PASS** | P3-07 Connect · P2 isolation |
| **O.13.1.1.2** | Local config / maintenance | SSH hardening · LuCI off · USB policy | **PART** | `CCLI_DUT_ADMIN_HARDENING.md` |
| **O.13.1.2–O.13.1.3** | Comms-loss behaviour / fallback timing | `mms.comms_loss_fallback_s` → Operating Rule | **PASS** | Same as O.9.2.2 evidence |
| **O.13.2** | Measurement accuracy at POC | EMT432 → TotW/TotVAr/PPV; bench 1P | **PART** | Metrology class / CT path OPEN |
| **O.13.5** | UTC time; uncertainty **≤ ±100 ms** | chrony + GNSS; MMS time quality | **PASS** | P5-00-03 / T.3.3.4.5 |
| **O.14** | Apparatus log ≥ 2048 events; categories | `EventStore` / `o14_catalog.hpp` | **PART** | `P7-04_EVENT_COVERAGE_MATRIX.md` gaps |
| **O.15 / §12** | Accredited certificates (61850, 62351, 62443, 61557-12, …) | Lab docs D1–D7 · freeze releases | — | **OPEN** — see `DNV_UCA_PRODUCT_READINESS.md` |

---

## 5-LN MMS objects vs Annex O (quick map)

| Annex O need (POC / control) | 5-LN LN / mechanism |
|------------------------------|---------------------|
| P, Q, V at PdC (PF1) | `PdCMMXU1` (TotW, TotVAr, PPV) ← Modbus EMT432 |
| DSO P limit | `WlimDWMX1` |
| External P set-point | `WSdDAGC1` |
| 4 s DSO visibility | `LLN0.urcb_PdC_Mis4sec` |
| Secure DSO path | Eth_A MMS TLS (Annex T transport) |

---

## Related evidence

| Topic | Path |
|-------|------|
| Comms-loss / Operating Rule | `lab/evidence/O13_1_isolation_2026-09-25/P3_05_COMMS_LOSS_FALLBACK.txt` |
| 31-LN Annex sequence (reference) | `lab/evidence/phase5/P5_ANNEX_TEST_SEQUENCE.md` |
| 5-LN TSP scope | [P5_FULLCIRCLE_5LN_SCOPE.md](P5_FULLCIRCLE_5LN_SCOPE.md) |
| O.14 coverage | `lab/evidence/phase7/P7-04_EVENT_COVERAGE_MATRIX.md` |
| Cert deliverables | [DNV_UCA_PRODUCT_READINESS.md](dnv/DNV_UCA_PRODUCT_READINESS.md) |
| **All annexes + O.15 book** | [Full Validate.md](Full%20Validate.md) |

---

## Maintenance

When a row moves from **GAP** → **PASS**, add the evidence filename and session date in the matrix revision history (below).

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-20 | Initial matrix (O.1–O.4 row + PF1/PF2/O.13/O.14/O.15 software map for 5-LN) |

# Phase 1 — Code vs regulation traceability matrix

**Document ID:** CCLI-LAB-PHASE1-TR-001  
**Revision:** 1.0  
**Date:** 2026-09-18  
**Scope:** Phase 1 only — Modbus RTU → PF2 FSM → DIO1/DIO2 on TesPro TG544  
**Platform:** `platform_tg500` · OpenWrt 25.12 / TesproOS  
**Companions:** [`PHASE1.md`](PHASE1.md) · [`PF2_Limits_and_Actions.md`](PF2_Limits_and_Actions.md) · [`CCI_Figura2_Parameters.md`](CCI_Figura2_Parameters.md) · [`CCI_Validation_and_Mocking_Strategy.md`](../knowledge-base/08-engineering/CCI_Validation_and_Mocking_Strategy.md) · [`CCI_Phase_Regulation_Checklists.md`](../knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md) (P0–P7 gates)

---

## How to use this matrix

Each row maps **regulation → requirement ID → code/config → test → status**.

| Status | Meaning |
|--------|---------|
| **PASS** | Implemented and verified (or bench PASS on record) |
| **PART** | Same intent, incomplete vs CEI (document gap) |
| **PEND** | Implemented; end-to-end bench sign-off not yet recorded |
| **NOT IMPL** | Out of Phase 1 scope; later phase |
| **N/A** | Not applicable to Phase 1 lab |

**Evidence column:** fill with date, APK version (`ccli-0.1.0-rN`), log excerpt, DMM reading, or test run ID.

**After each bench session:** update Status and Evidence; do not delete GAP rows — they drive Phase 2+.

---

## 1. Phase 1 exit criteria (roadmap)

| Exit ID | Criterion | Primary test | Matrix § |
|---------|-----------|--------------|----------|
| EXIT-P1-01 | Modbus RTU read P (kW) on RS485 | REQ-LAB-001, L2 Modbus | §2 |
| EXIT-P1-02 | PF2 curtail P > 900 kW ≥ 30 s | L1-P-03, REQ-LAB-002 | §3 |
| EXIT-P1-03 | Permissive gates DIO1 | REQ-LAB-003 | §4 |
| EXIT-P1-04 | Stale data → safe state | L1-C-01, REQ-LAB-004 | §3 |
| EXIT-P1-05 | DIO via ubus `dido_v2` | REQ-LAB-005, DIO smoke | §4 |
| EXIT-P1-06 | Deploy APK/binary + procd | `DEPLOY_CCLI.md` | §6 |

---

## 2. Modbus / metering input

| CEI / norm ref | REQ-ID | Code / config | Test ID | Status | Evidence / notes |
|----------------|--------|---------------|---------|--------|------------------|
| O.13.2 — POC measurement at PdC | REQ-SER-001 | `modbus_adapter.cpp` `poll_libmodbus()` | L2 Modbus | **PEND** | A2/B2 → `/dev/ttyS2`; PC slave `modbus_rtu_slave.py` |
| Annex T `MMXU1.PdC.TotW` (4 s) | REQ-MET-008 | — | — | **NOT IMPL** | Phase 5 metrology; lab uses 1 s Modbus poll |
| O.13.2.1 class ≤ 0.2 converter | REQ-MET-* | — | — | **N/A** | Lab simulator; no accuracy class |
| — (lab) | REQ-LAB-001 | `lab.yaml` `modbus.backend: rtu`, `device: /dev/ttyS2` | REQ-LAB-001 | **PEND** | Status ~5 s when bus healthy |
| Register map 40001 float32 BE | — | `modbus_adapter.cpp` `regs_to_float32_be()`; `modbus_rtu_slave.py` | Modbus sanity | **PASS** | Code aligned |
| Comm timeout / stale quality | REQ-PF2-002 | `modbus_adapter.cpp` on RTU fail → `DataQuality::Stale` | L1-C-01 | **PASS** | Unit + FSM test |
| Modbus `retries: 3` in YAML | REQ-SER-001 | Parsed in `ccli_config.cpp`; **not used** in adapter loop | — | **GAP** | Implement or remove from YAML |

**Key files:** `apps/ccli/adapters/modbus_master/modbus_adapter.cpp` · `apps/ccli/services/svc_modbus.cpp` · `apps/ccli/config/lab_tr400.yaml` `modbus:`

---

## 3. PF2 FSM — active power limitation (Annex O)

| CEI / norm ref | REQ-ID | Code / config | Test ID | Status | Evidence / notes |
|----------------|--------|---------------|---------|--------|------------------|
| O.9.2 — limitation function (concept) | REQ-PF2-001 | `pf2_fsm.cpp` + `core/dso/` | L1-P-03 / DSO-R03 | **PART** | **900 kW** or **WSd→42 kW** mock |
| O.9.2.2 — DSO P limit slaved to threshold | REQ-PF2-001 | `dso_active_power` yaml mock | `--regulation-check` R02 | **PART** | MMS Phase 3 |
| O.9.2.1 — 110 % V autonomous limit | REQ-PF2-001 | yaml `w110_*` mock | R04 unit | **PART** | No voltage input |
| O.9.2 — limitation mode (i) actuation | REQ-PF2-001 | `ccli_main.cpp` → `svc_io_apply_curtailment()` | L1-P-03, L2 | **PEND** | DIO1 → relay K1 |
| O.9.2 — debounce before actuation | REQ-PF2-001 | `pf2.debounce_s: 30` · `CurtailPending` state | L1-P-03 | **PASS** | `pf2_fsm_test.cpp` |
| O.9.2 — hysteresis / release | REQ-PF2-001 | `release_threshold_kw: 850` | L1-P-04 | **PASS** | FSM + unit tests |
| O.9.2 — min on / min off | REQ-PF2-001 | `min_on_s: 60`, `min_off_s: 30` | unit test | **PASS** | `pf2_fsm.cpp` |
| O.13 / comm loss → safe behaviour | REQ-PF2-002 | `is_stale()` → `SafeState` | L1-C-01 | **PASS** | `stale_data_s: 10` |
| O.13 — no stale-as-valid curtail | REQ-PF2-002 | `pf2_fsm.cpp` lines 34–37 | L1-C-01 | **PASS** | R-PF2-001 |
| O.11 — priority vs teledistacco | REQ-PF2-003 | — | — | **NOT IMPL** | DIO3 spare not wired |
| — (lab) | REQ-LAB-002 | `pf2.threshold_kw: 900` | `--lab-demo` | **PEND** | ~85 s to curtail with demo |
| — (lab) | REQ-LAB-004 | Stop slave → stale → DIO OFF | L1-C-01 | **PEND** | Bench required |
| PF2 event / reason strings | REQ-PF2-004 | `pf2_fsm.cpp` `state_.reason`; `EventRing` | logread | **PART** | Not full O.14 data logger |
| Open-loop demo (no RS485) | — | `ccli_main.cpp` `--lab-demo` forces simulator | `--lab-demo` | **PASS** | Triangle 400–950 kW |

**Key files:** `apps/ccli/core/pf2/pf2_fsm.cpp` · `apps/ccli/services/svc_pf2.cpp` · `apps/ccli/tests/unit/pf2_fsm_test.cpp` · `apps/ccli/test_vectors/l1_pf2_sequence.yaml`

---

## 3b. DSO equation mock (Phase 1 — `core/dso/`)

| Map ID | Clause | Code | Test | Status | Evidence |
|--------|--------|------|------|--------|----------|
| R01 | O.8.2 Eq (1) | `plant_envelope.hpp` | `dso_phase1_test` | **PASS** | S_calc ≈ 206.15 kVA |
| R02 | O.9.2.2 | `dso_active_power.cpp` + MMS Wlim | P3-04 | **PASS** (lab) | yaml mock → live Wlim Phase 3 |
| R03 | O.9.2.3 | `derive_active_power_kw` | `dso_phase1_test` | **PASS** | WSd 20 % → 42 kW |
| R04 | O.9.2.1 | yaml `w110_*` | `dso_phase1_test` | **PART** | no V meter |
| R05 | O.11 Eq (5)(6) | `derive_active_power_kw` min() | `dso_phase1_test` | **PASS** | 50/70 examples + 105 kW custom |
| R06 | O.7.3.1 | `settled_within_band_kw` | unit | **PART** | no TsP timer |
| R07–R09, R10–R13 | various | — | report row | **NOT IMPL** | Phase 3–5 |
| L04 | Lab Eq (18) | `modbus_rtu_slave.py --ramp` | manual | **PASS** | code present |
| — | — | `regulation_map.hpp` + `regulation_bench.cpp` | `--regulation-check` | **PASS** | all R01–L04 rows |
| — | — | `lab_tr400_phase1_regulation.yaml` | bench | **PEND** | deploy + live |

**Key files:** `apps/ccli/core/dso/` · `lab/PF2_REGULATION_PHASE1.md` · `apps/ccli/test_vectors/l2_dso_phase1.yaml`

---

## 4. Digital I/O and actuation

| CEI / norm ref | REQ-ID | Code / config | Test ID | Status | Evidence / notes |
|----------------|--------|---------------|---------|--------|------------------|
| O.9.2 mode (i) — reduce generation at units | REQ-IO-001 | `hal_gpio_ll.c` ubus `set_relay` ch1 | DIO smoke | **PASS** | 2026-09-15 DMM bench |
| O.9.3 — Annex M trip, no conflicting control | REQ-IO-* | `io.di_pf2_spare` gpio 3 — **no FSM hook** | — | **NOT IMPL** | Phase 6+ |
| O.12 — teledistacco predisposition | REQ-IO-* | Policy / wiring only | — | **N/A** | Not software Phase 1 |
| O.14 — log DI/DO interventions | REQ-PF2-004 | `svc_io.c`, `ccli_main.cpp` stderr | logread | **PART** | Strings only; no 2048-event store |
| 62443 CR 3.6 — safe DO on boot | REQ-IO-003 | `DrvGpio_Init()` safe state; `ccli.init` | io_safe_state_test | **PASS** | `io_safe_state_test.c` |
| 62443 CR 3.6 — permissive interlock | REQ-PF2-003 | `ccli_main.cpp` `commanded && permissive_ok` | REQ-LAB-003 | **PEND** | **Not in Allegato O/T** — lab policy |
| — (lab) | REQ-LAB-003 | `io.di_permissive.gpio: 2`, `do_curtail.gpio: 1` | DIO2 key / `tr400-io-verify.sh` | **PEND** | `curtail_blocked_permissive` log |
| — (lab) | REQ-LAB-005 | `platform_tg500/hal_gpio_ll.c` | DIO smoke | **PASS** | `dido_v2` not libgpiod |
| DIO3 / DIO4 spare | — | YAML only; no code path | — | **NOT IMPL** | Reserved |

**Key files:** `apps/ccli/platform/platform_tg500/hal_gpio_ll.c` · `apps/ccli/adapters/io/drv_gpio.c` · `apps/ccli/services/svc_io.c` · `lab/TR400_DIO_SMOKE_TEST.md`

---

## 5. Annex T / DSO path (Phase 1 out of scope — status after Phase 3 lab close)

| CEI / norm ref | REQ-ID | Code / config | Test ID | Status | Target phase |
|----------------|--------|---------------|---------|--------|--------------|
| T Table 85 `Wlim` % Smax | REQ-61850-001 | `mms_adapter.cpp` Wlim → PF2 | P3-04 | **PASS** (lab) | Phase 3 **CLOSED** |
| T `MMXU1.PdC.TotW` publish 4 s | REQ-MET-008 | urcb TotW intgPd=4000 | P3-03 | **PASS** (lab); live P → Phase 5 | Phase 3/5 |
| T §8.1 / T.3.3.4.5 UTC ±100 ms | REQ-TIME-* | chrony + ccli observe | P3-11 | **PASS** (lab) | Phase 3 **CLOSED** |
| 62351-3/4 TLS on MMS | REQ-SEC-003 | mbedtls TLS `:3782` | P3-06/07 | **PASS** (lab) | Phase 3 **CLOSED** |
| 60870-5-104 on Eth_B | REQ-104-001 | `iec104_adapter.cpp` stub | — | **NOT IMPL** | **Phase 4** ← next |
| XCBR1.IDG.Pos breaker status | — | — | P3-10 | **WAIVED** (lab) | Product model |

---

## 6. Network, security, deploy (Phase 1 touch only)

| Item | REQ-ID | Code / config | Test ID | Status | Notes |
|------|--------|---------------|---------|--------|-------|
| No Eth_A/B/plant separation yet | REQ-NET-001 | P2 zones lan1_sec/lan_sec/lan2_sec | P2-01…03 | **PASS** | Phase 2 **CLOSED** |
| Wi-Fi disabled product profile | REQ-NET-005 | TesproOS UCI | — | **PART** | Phase 2 residual |
| CCLI package on TG544 | REQ-BLD-002 | `package/ccli/` → `.apk` / `ccli-bin` | apk/sideload | **PASS** | r15 lab |
| procd autostart | REQ-BLD-002 | `/etc/init.d/ccli` · `ccli-chrony` | Startup | **PASS** | chrony + ccli |
| Runtime config | — | `/etc/ccli/lab.yaml` | — | **PASS** | From lab yaml |
| UCI pf2/io GPIO 5/27 | — | `/etc/config/ccli` | — | **GAP** | **Unused** — YAML is source of truth |

---

## 7. L1 test vector mapping

| Test ID | Stimulus | Expected | Code under test | Automated? |
|---------|----------|----------|-----------------|------------|
| L1-P-01 | P = 500 kW, good | No curtail | `pf2_fsm.cpp` | `pf2_fsm_test.cpp` |
| L1-P-02 | P = 700 kW, good | No curtail | `pf2_fsm.cpp` | `pf2_fsm_test.cpp` |
| L1-P-03 | P = 950 kW, hold 35 s | Curtail active | `pf2_fsm.cpp` | `pf2_fsm_test.cpp` + bench |
| L1-P-04 | P = 1000 kW, good | Curtail remains | `pf2_fsm.cpp` | partial |
| L1-C-01 | Stale / timeout quality | SafeState, no curtail | `pf2_fsm.cpp` + modbus | `pf2_fsm_test.cpp` |
| L1-C-02 | Modbus exception | Invalid quality | `modbus_adapter.cpp` | inject fault API only |
| L1-C-03 | Bad CRC RTU | Frame rejected | `modbus_adapter.cpp` | **PEND** bench |
| L1-IO-01 | Boot / init | DO de-energised | `drv_gpio.c` | `io_safe_state_test.c` |

Source: `apps/ccli/test_vectors/l1_pf2_sequence.yaml` · `CCI_Validation_and_Mocking_Strategy.md` §2.1

---

## 8. Lab → CEI migration (Phase 1 gaps → later work)

| Lab (Phase 1) | CEI target | Migration | Phase |
|---------------|------------|-----------|-------|
| `threshold_kw: 900` | DSO `Wlim` % **Smax** | MMS subscribe on Eth_A | 3 |
| Modbus `p_kw` | `MMXU1.PdC.TotW` + class 0.2 | Aggregator + real analyzer | 5 |
| DIO1 curtail relay | O.9.2 mode (i) + unit command path | Keep DIO; add Modbus/cmd | 4+ |
| DIO2 permissive | Not CEI | Document as SL-2 interlock | — |
| DIO3 spare | Annex M / teledistacco inhibit | Wire + FSM block | 6 |
| 10 s stale → safe | O.13 comms fallback (longer timers) | Align to Operating Rule | 3+ |
| `--lab-demo` | Test vector only | Disable in production | 7 |
| Event ring strings | O.14 data logger ≥ 2048 | syslog / persistent store | 7 |

---

## 9. Phase 1 sign-off checklist

Complete when **all PEND → PASS** or accepted as **GAP** with owner.

| # | Check | Test | Status | Date | Evidence |
|---|-------|------|--------|------|----------|
| 1 | Modbus RTU P read on A2/B2 | REQ-LAB-001 | ☐ | | |
| 2 | PF2 curtail @ 950 kW, 35 s hold | L1-P-03 | ☐ | | |
| 3 | DIO2 open blocks DIO1 | REQ-LAB-003 | ☐ | | |
| 4 | DIO2 closed (12 V key) allows DIO1 ON | REQ-LAB-003 | ☐ | | `TR400_HW_IO_CONFIG.md` |
| 5 | Stop Modbus → DIO1 OFF ≤ 10 s | L1-C-01 | ☐ | | |
| 6 | `--lab-demo` triangle crosses 900 kW | lab-demo | ☐ | | |
| 7 | DIO smoke (DIO1/DIO2) | TR400 smoke | ☑ | 2026-09-15 | `TR400_DIO_SMOKE_TEST.md` |
| 8 | APK installed + Startup `ccli` | deploy | ☑ | 2026-09-17 | `ccli-0.1.0-r3` |
| 9 | Unit tests PF2 + IO safe state | CI / native build | ☐ | | `pf2_fsm_test`, `io_safe_state_test` |

**Phase 1 regulatory sign-off statement (draft):**  
Phase 1 demonstrates **Annex O PF2-like** local active-power limitation with **fail-safe on stale data** and **physical curtailment output**, using **lab thresholds** and **Modbus RTU** instead of **Annex T MMS `Wlim`**. This is **engineering evidence**, not accredited CEI 0-16 certification.

---

## 10. Document history

| Rev | Date | Change |
|-----|------|--------|
| `1.0 | 2026-09-18 | Initial Phase 1 code vs regulation matrix |

# PF2 limits and actions — lab vs CEI (Allegato O / T)

**Document ID:** CCLI-LAB-PF2-LA-001  
**Revision:** 1.0  
**Date:** 2026-09-17  
**RAG source_id:** `ccli-pf2-limits-actions`  
**Platform:** TesPro TG544 / TR500 (`platform_tg500`)  
**Config:** `apps/ccli/config/lab_tr400.yaml` → `/etc/ccli/lab.yaml`  
**Code:** `apps/ccli/core/pf2/pf2_fsm.cpp`, `apps/ccli/services/ccli_main.cpp`, `apps/ccli/adapters/modbus_master/modbus_adapter.cpp`  
**Regulatory:** `knowledge-base/08-engineering/CCI_Annex_O_Extract.md`, `CCI_Annex_T_Extract.md`  
**Phase 1 equations / master map:** `lab/PF2_REGULATION_PHASE1.md` · **`lab/CCI_Figura2_Parameters.md`** · code `apps/ccli/core/dso/`  
**Companion:** `lab/PHASE1.md` (master runbook), **`lab/PHASE1_TRACEABILITY.md`** (code vs CEI sign-off), `knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md` (P0–P7 gates), `lab/TR400_DIO_SMOKE_TEST.md`, `lab/MODBUS_RTU_LAB.md`, `lab/tg544-openwrt/DEPLOY_CCLI.md`

---

## Summary

This document is the **single reference** for:

1. **Lab (Phase 1)** — thresholds, FSM states, DIO behaviour, and log strings **as implemented today**.
2. **CEI target** — normative limits and actions from **Allegato O** (functions/timing) and **Allegato T** (IEC 61850 data model).

**Critical split:** Lab uses a **local P (kW) threshold** and **open-loop** Modbus mock. Product CEI path uses **DSO `Wlim` (% Smax)** on Eth_A and **class ≤ 0.2** POC metering — not the same inputs.

---

## Signal flow (lab)

```text
Mock / Modbus ──► MeasurementStore (P kW) ──► PF2 FSM ──► curtailment command
                                                      │
Permissive DIO2 ──────────────────────────────────────┤
                                                      ▼
                                              DIO1 = PF2 AND permissive
                                                      │
                                                      ▼
                                              Relay K1 (plant actuation)
```

**DIO formula (code):** `DIO1 ON ⇔ pf2.curtailment_active() AND di_permissive OK`

---

# Part A — Lab (what runs today)

## A.1 Configuration limits (`lab_tr400.yaml`)

| Key | Value | Unit | Role |
|-----|-------|------|------|
| `pf2.threshold_kw` | **900** | kW | Enter curtail when P **>** this |
| `pf2.release_threshold_kw` | **850** | kW | Release curtail when P **<** this (hysteresis) |
| `pf2.debounce_s` | **30** | s | P must stay above enter threshold this long |
| `pf2.min_on_s` | **60** | s | Minimum time curtail stays commanded ON |
| `pf2.min_off_s` | **30** | s | Minimum time between curtail cycles after release |
| `pf2.stale_data_s` | **10** | s | No valid P → safe state |
| `modbus.poll_ms` | **1000** | ms | Measurement poll interval (simulator default) |
| Main loop | **100** | ms | `usleep(100000)` in `ccli_main.cpp` |
| Status log interval | **5** | s | Every 50 loop iterations |

## A.2 Mock / inverter input limits

| Source | Backend | P (kW) | Quality | Maps to | Status |
|--------|---------|--------|---------|---------|--------|
| Default simulator | `ModbusBackend::Simulator` | **500** fixed | Good | `MeasurementStore.p_kw` | **ACTIVE** |
| `--lab-demo` | Simulator + triangle inject | **400 → 950 → 400** / ~120 s | Good | Same | **ACTIVE** |
| pymodbus / RS485 slave | `Libmodbus` RTU + reg **40001** | User-defined / `--ramp` | Good if poll OK | Same | **WIRED** — `lab/modbus_rtu_slave.py` |
| Fault: timeout | `inject_comm_fault("L1-C-01")` | — | Stale | FSM → SafeState | Test only |
| Fault: exception | `inject_comm_fault("L1-C-02")` | — | Invalid | FSM → SafeState | Test only |
| Fault: bad CRC | `inject_comm_fault("L1-C-03")` | — | No update | Poll fails | Test only |

**Mock does not:** reduce P when DIO1 closes; accept DSO commands; read real inverter control registers.

## A.3 PF2 FSM states

| State ID | Meaning | `curtailment_active` |
|----------|---------|----------------------|
| `Normal` | P within band / idle | false |
| `CurtailPending` | P > threshold, debounce running | false |
| `CurtailActive` | Curtail commanded | true |
| `SafeState` | Stale/invalid data or fault | false |

## A.4 Limits → FSM → DIO → logs (master table)

| Condition | Limit / input | FSM state | PF2 command | DIO1 (curtail) | DIO2 (perm.) | Event / log (`reason` or DO detail) |
|-----------|---------------|-----------|-------------|----------------|--------------|-------------------------------------|
| Normal export | P ≤ **900** kW | Normal | OFF | **OFF** (~1.5 V) | 1 = allow | `ok` |
| Enter debounce | P > **900** kW, < **30** s | CurtailPending | OFF | OFF | 1 = allow | `debounce` |
| Curtail active | P > **900** kW, ≥ **30** s | CurtailActive | **ON** | **ON** (~0 V) if perm. OK | **1** required | `p_above_threshold`, DO `curtail_on` |
| Permissive block | PF2 ON, DIO2 open | CurtailActive | ON (cmd) | **OFF** (gated) | **0** | DO `curtail_blocked_permissive` |
| Hysteresis hold | P < **850** kW, < **60** s min-on | CurtailActive | ON | ON if perm. OK | 1 | `hysteresis_hold` |
| Release | P < **850** kW, ≥ debounce + min-on + min-off | Normal | OFF | OFF | — | `released`, DO `curtail_off` |
| Stale / bad data | age > **10** s or quality ≠ Good | SafeState | OFF | **OFF** (fail-safe) | — | `stale_or_invalid`, `stale_data_timeout` |
| Recovery | Good data returns | Normal | OFF | OFF | — | `recovered` |
| Boot / shutdown | — | — | OFF | **OFF** (safe default) | — | `svc_io` safe state |

## A.5 DIO electrical limits and actions

| Silk | ubus ch | Direction | CCLI key | Electrical (bench TG544) | Logical OK / active | Action |
|------|---------|-----------|----------|----------------------------|---------------------|--------|
| **DIO1** | 1 | DO | `io.do_curtail` | OFF ≈ **1.5 V**; ON ≈ **0 V** (open-drain sink) | ON = curtail energised | Drive relay **K1** → plant limit |
| **DIO2** | 2 | DI | `io.di_permissive` | Open → **0**; key closed (~12 V) → **1** | **1** = actuation allowed | Gates DIO1 |
| **DIO3** | 3 | DI | `io.di_pf2_spare` | UNTESTED | — | **No action in Phase 1 code** |
| **DIO4** | 4 | DO | `io.do_pf2_spare` | Same polarity as DIO1 | — | **No action in Phase 1 code** |

Polarity: `do_curtail.active_high: false`, `di_permissive.active_low: true` (TG544: gd32 **1** = permit — see `lab/TR400_HW_IO_CONFIG.md`).

## A.6 Log and audit strings

| Subsystem | Channel | String | When |
|-----------|---------|--------|------|
| PF2 | `events` / stderr | `debounce` | P above threshold, timer running |
| PF2 | | `p_above_threshold` | Curtail commanded |
| PF2 | | `hysteresis_hold` | Below release but min-on not met |
| PF2 | | `released` | Curtail cleared |
| PF2 | | `stale_or_invalid` | Bad quality |
| PF2 | | `stale_data_timeout` | Timestamp too old |
| PF2 | | `recovered` | Left SafeState |
| IO | `events` / stderr | `curtail_on` | DIO1 driven ON |
| IO | | `curtail_off` | DIO1 driven OFF |
| IO | | `curtail_blocked_permissive` | PF2 wants ON, DIO2 blocks |
| IO | stderr (5 s) | `io: curtail_do=ON\|OFF permissive=OK\|BLOCKED` | Status line |

## A.7 `--lab-demo` timeline (permissive closed, one ~120 s cycle)

| Time (approx) | Mock P (kW) | vs limits | FSM | DIO1 |
|---------------|-------------|-----------|-----|------|
| 0 s | 400 | < 900 | Normal | OFF |
| ~55 s | crosses 900 | debounce starts | CurtailPending | OFF |
| ~85 s | > 900 for 30 s | active | CurtailActive | **ON** |
| ~95 s | peak ~950 | > 900 | CurtailActive | ON |
| ~110 s | < 850 | min-on hold | CurtailActive | ON |
| ~145 s+ | < 850, timers met | released | Normal | OFF |
| 120 s | cycle repeats | — | — | — |

## A.8 Lab vs hardware gap

| Item | Software (PF2 + logs) | Real DIO on TG544 |
|------|----------------------|-------------------|
| `platform_tg500` HAL | **ubus dido_v2** (`hal_gpio_ll.c`) | DIO1/DIO2 on TG544 when `didoservice_v2` running |
| OEM GD32 schematic | — | `knowledge-base/08-engineering/reference/vendor/tespro/Schematic-GPIO.pdf` + `CCI_TR400_GPIO_Schematic_Extract.md` (DIO vs REQ-IO / O.9.2 / O.11) |
| Verify DIO wiring | RTU + PF2 logs | `lab/MODBUS_RTU_LAB.md`, `lab/TR400_DIO_SMOKE_TEST.md` |

## A.9 Quick lookup (lab)

| If… | Then… |
|-----|-------|
| P > **900** kW for ≥ **30** s | PF2 commands curtail |
| DIO2 open (0) | DIO1 stays OFF even if PF2 commands |
| DIO2 closed to GND (1) + PF2 commands | DIO1 ON |
| P < **850** kW after min-on **60** s + release debounce **30** s | DIO1 OFF |
| No valid P > **10** s | SafeState, DIO1 OFF |
| `--lab-demo` | P ramps 400–950 kW automatically |
| Default sim (no demo) | P = **500** kW → no curtail |

---

# Part B — CEI target (Allegato O / T)

Normative references: CEI 0-16 consolidated **2025-12**, Allegato **O** (CCI functions), Allegato **T** (IEC 61850 / cyber). English extracts in knowledge-base.

## B.1 Plant envelope (O.8.2 — Operating Rule)

| Symbol | Meaning | IEC 61850 (Annex T) | Lab Phase 1 |
|--------|---------|---------------------|-------------|
| **Pfed** (Pimm) | Max active power **export** to grid | `DPCC1.PdC_Wi.WRtg` | **`plant.p_imm_kw`** (TR 200 kW) |
| **Pass** | Max active power **import** | `DPCC1.PdC_Wa.WRtg` | **`plant.p_ass_kw`** (TR 200 kW) |
| **Qcap / Qind** | Reactive limits | `DPCC2.PdC_Qc/Qi.VArRtg` | **`plant.q_*_kvar`** (TR 50 kVAr); PF2 FSM ignores Q |
| **Smax** | Apparent power reference (p.u. base) | `DPCC3.PdC_VA.VARtg` | **`plant.smax_kva`** in yaml; DSO mock uses **% Smax** → kW (`core/dso/`) |

## B.2 Active power limitation functions (O.9.2)

| Case | CEI clause | Trigger / limit | Required action | Phase 1 lab |
|------|------------|-----------------|-----------------|-------------|
| **110 % Vn limit** | O.9.2.1 | V ≈ 110 % Un at PdC; user-activated only | Autonomous P limit; **log** activation and intervention | **Not implemented** |
| **DSO command limit** | O.9.2.2 | Command from DSO (defence / grid) | Slaved function; hold P below received threshold; TsP ≤ **60 s** | **Not implemented** — local 900 kW only |
| **DSO modulation** | O.9.2.3 | DSO set-point | Same as O.10.3.1 active-power set-point | **Not implemented** |
| **Limitation mode (i)** | O.9.2 | — | Reduce generation at units | **DIO1 → K1** (open-loop bench) |
| **Limitation mode (ii)** | O.9.2 | Storage SOC OK | Absorb via storage | **Not implemented** |
| **Max step size** | O.9.2 | — | ≤ CEI-defined steps | Not enforced in lab FSM |
| **Comms loss** | O.9.2.2 + O.13.1.2 | DSO channel lost | Autonomous mode per Operating Rule; manual fallback | Lab: **10 s stale → safe OFF** |

## B.3 DSO interface — Annex T Table 85 (`DWMX1`, prefix `Wlim`)

| Parameter | Unit | Range | Default | Action when active |
|-----------|------|-------|---------|-------------------|
| Operating status | — | 1=Operating / 5=Not-Operating | 5 | Enable limit function |
| Active power limit (generation) | **% Smax** | 0..100 | 0 | Target export cap at PdC |
| Activation command | — | 5=Inactive / 1=Active | 5 | Apply limit |
| Setpoint function status | — | 0=N/A / 1=Autonomous / 2=Automatic | 1 | Mode |
| Limit function status (110 % V) | — | 0=N/A / 1=Azutonomous | 2 | O.9.2.1 status |

**Lab mapping gap:** Product must subscribe to **`Wlim`** on Eth_A (MMS), not `threshold_kw` in yaml.

## B.4 Observability — Annex T measurements (`MMXU1`, prefix `PdC`)

| Quantity | Unit | IEC 61850 DO | Period | Lab Phase 1 |
|----------|------|--------------|--------|-------------|
| Active power (signed) | kW | `MMXU1.PdC.TotW` | **4 s** | Sim 1 s poll; no MMS publish |
| Reactive power | kVAr | `MMXU1.PdC.TotVAr` | **4 s** | Q = 0.1×P (sim only) |
| Phase-phase voltage | kV | `MMXU1.PdC.PPV` | **4 s** | Not used |
| Phase current | A | `MMXU1.PdC.A` | 4 s (optional) | Not used |

## B.5 Control timing limits (O.7)

| Parameter | CEI limit | Lab equivalent | Gap |
|-----------|-----------|----------------|-----|
| **TsP** (active power settling) | ≤ **60 s** to ±5 % band | Not measured | debounce + min-on ≠ TsP proof |
| **TsQ** (reactive settling) | ≤ **10 s** | N/A | PF3/PF2 Q not in lab |
| **MC200** (fast ring) | **200 ms** blocks | N/A | Not implemented |
| **ΔT** (slow ring) | **10–600 s**, default **60 s** | N/A | Not implemented |
| External set-point min interval | **3 s** | N/A | No DSO path |
| PF1 TX alignment | **:00/:04/:08…** every **4 s** | 1000 ms poll, unaligned | **Gap** |
| DG/DDG status change notify | ≤ **4 s** | N/A | DIO3 not wired |

## B.6 Measurement accuracy (O.13.2.1)

| Element | CEI requirement | Lab Phase 1 |
|---------|-----------------|-------------|
| Measuring converter | Class ≤ **0.2** | Simulator — no class |
| CT / VT | Class ≤ **0.5** | N/A |
| Fiscal CT/VT reuse | **Excluded** | N/A |
| PV/wind 100–500 kW (V5 / A.72) | Error ≤ **5 %** allowed | N/A until real analyzer |

## B.7 Priority among functions (O.11)

| Priority | Function | Overrides lower? | Lab |
|----------|----------|------------------|-----|
| — (absolute) | Unit O/F regulation (§8.8.6.3.2/3) | Yes — at **unit**, not CCI | N/A |
| — (absolute) | Defence plan **teledistacco** (Annex M) | Yes — no conflicting CCI action | DIO3 spare — not wired |
| 1 | 110 % Vn P limit (O.9.2.1) | Yes vs 2–7 | Not impl. |
| 2 | DSO P limit (O.9.2.2) | Yes vs 3–7 | Not impl. |
| 3 | DSO P modulation (O.9.2.3) | Yes vs 4–7 | Not impl. |
| 4 | Active power set-point (O.10.3.1) | Yes vs 5–7 | Not impl. |
| 5–7 | Reactive / cosφ functions | Per table | Not impl. |

**O.11 note:** Teledistacco and DSO P limit share top priority below unit O/F.

## B.8 DIO / physical interfaces (O.9.3, O.12)

| CEI requirement | Target action | Lab |
|-----------------|---------------|-----|
| O.9.3 — Annex M device interfaced | CCI detects trip; **no conflicting** control | DIO3 proposed; **not in code** |
| O.12 fig. 128 note (2) — **teledistacco predisposition** | Physical wiring to CCI | Policy only |
| O.14 — log DG/DI status, control interventions | Data logger ≥ **2048** events | Partial — `EventRing` DO/PF2 only |
| XCBR1.IDG.Pos (Annex T) | Breaker open/closed to DSO | Not impl. |

**Permissive DIO2:** **Not** in Allegato O/T — lab engineering interlock only.

## B.9 CEI limits → actions (target product)

| Input / limit | Source | FSM / function | Actuation | Log (O.14 / T §8.7) |
|---------------|--------|----------------|-----------|---------------------|
| `Wlim` limit % + activate | DSO MMS | O.9.2.2 slaved limit | Modbus/unit cmd **or** DIO1 | DSO command + intervention |
| V ≈ 110 % Un | POC meter | O.9.2.1 autonomous | P reduction | Activation + intervention |
| P, Q, V at PdC | Analyzer class ≤ 0.2 | PF1 publish | MMS `MMXU1.PdC` every 4 s | Measurement quality |
| Annex M trip | DI to CCI | Block PF2 conflicts | Cease curtail commands | Teledistacco event |
| Eth_A loss | O.13.1.2 | Autonomous mode delay | Per Operating Rule | Comms loss |
| Stale POC data | Internal policy | Safe state | DIO de-energised | Same as lab intent |
| Time sync | O.13.5 / T §8.1 | UTC ± **100 ms** | Timestamp all TX | NTP/NTS |

---

# Part C — Traceability matrix (lab → CEI)

| Lab item | CEI target | Migration action |
|----------|------------|------------------|
| `threshold_kw` 900 | `Wlim` % of **Smax** | MMS server + map command to FSM |
| Modbus sim `p_kw` | `MMXU1.PdC.TotW` + analyzer | libmodbus + 4 s block aggregator |
| DIO1 curtail | O.9.2 mode (i) actuation | Keep; add unit/Modbus command path |
| DIO2 permissive | Not CEI | Optional; document as SL-2 interlock |
| DIO3 spare | Annex M / teledistacco feedback | Wire + FSM inhibit |
| 10 s stale → safe | O.13 comms fallback (longer) | Align timers to Operating Rule |
| `--lab-demo` triangle | Test vector only | Disable in production |
| Event ring strings | O.14 data logger categories | Expand to 2048 + syslog |

---

# Part D — Verification pointers

| Scope | Test | Pass criteria |
|-------|------|---------------|
| Lab | `ccli --config /etc/ccli/lab.yaml --lab-demo` | DO `curtail_on` after ~85 s; DIO1 ON with permissive |
| Lab | Open DIO2 during curtail | `curtail_blocked_permissive`; DIO1 OFF |
| Lab | Stop ccli / kill modbus | DIO1 OFF at shutdown |
| CEI | MMS read `TotW` | 4 s period, q + t present |
| CEI | DSO write `Wlim` | P within ±5 % in ≤ 60 s |
| CEI | Annex M trip on DIO3 | No conflicting curtail |

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-17 | Initial — lab FSM/DIO + Allegato O/T target tables |

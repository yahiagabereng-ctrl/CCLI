# PF2 regulation master map — Phase 1 code & equations

**Document ID:** CCLI-LAB-PF2-REG-P1-001  
**Revision:** 1.2  
**Date:** 2026-09-22  
**Code:** `apps/ccli/core/dso/` · `pf2_fsm.cpp` · `lab_tr400.yaml`  
**Structure:** `apps/ccli/core/dso/README.md`  
**Companion:** `PF2_Limits_and_Actions.md` · `PHASE1_TRACEABILITY.md` · **`CCI_Figura2_Parameters.md`**

Phase 1 adds an **equation layer** (O.8.2, O.9.2.x mock, O.11 min, O.7.3.1 band check) without MMS. Full Allegato T Eth_A remains **Phase 3**.

---

## Software structure (what we use in Phase 1)

```text
config/lab_tr400.yaml | lab_tr400_dso_tr57126.yaml
         │
         ▼
core/config/ccli_config.cpp  ──►  PlantConfig, DsoConfig, Pf2Config
         │
         ▼
core/dso/dso_phase1.cpp      ──►  apply_dso_to_pf2_config()  [if enabled]
         │                         uses plant_envelope + dso_active_power
         ▼
core/pf2/pf2_fsm.cpp         ──►  P_meas vs threshold → curtailment_active
         ▲
adapters/modbus_master       ──►  MeasurementStore.p_kw
         │
services/ccli_main.cpp       ──►  DIO1 via svc_io (permissive DIO2)
```

| Layer | Directory | Phase | Role |
|-------|-----------|-------|------|
| **Config** | `core/config/` | 1 | Yaml → structs |
| **DSO equations** | `core/dso/` | 1 mock / 3 MMS | % Smax → kW, O.11 min |
| **PF2 FSM** | `core/pf2/` | 1 | Debounce, stale-safe, hysteresis |
| **Plant I/O** | `adapters/modbus_*`, `platform/` | 1 | P kW in, DIO out |
| **MMS** | `adapters/iec61850_mms/` | 3 stub | Will feed `DsoActivePowerCommands` |

Module detail: **`apps/ccli/core/dso/README.md`**.

---

## Enable CEI mock on bench

| Profile | Config file | Gates | Effective PF2 enter (typ.) |
|---------|-------------|-------|----------------------------|
| **Legacy 900 kW** | `lab_tr400.yaml` | `dso.enabled: false` | **900 kW** |
| **TR Figura 2** | `lab_tr400_dso_tr57126.yaml` | both **true**, `profile: tr57126_table1` | **~42 kW** |
| **Phase 1 bench** | `lab_tr400_phase1_regulation.yaml` | both **true**, custom WSd 20 % | **~42 kW** |
| **O.11 all yaml params** | `lab_tr400_dso_custom_o11.yaml` | w110+wlim+wsd | **105 kW** |

```bash
ccli --config config/lab_tr400_dso_tr57126.yaml --lab-demo
ccli --regulation-check --config config/lab_tr400_phase1_regulation.yaml
```

Requires **`pf2.use_dso_mock: true`** and **`dso.enabled: true`** for equation layer.

---

## Master map

| ID | Clause | Annex T | Equation | Example values | Phase 1 code |
|----|--------|---------|----------|----------------|--------------|
| R01 | **O.8.2** Smax | `DPCC3.PdC_VA` | **(1)** | calc **206.15 kVA**; TR **210 kVA** | `plant_envelope.hpp` `calc_smax_kva()` |
| R02 | **O.9.2.2** Wlim | Tab. **84** `WMaxSptPct` | **(2)** | default **0 %** | `DsoActivePowerCommands` + yaml |
| R03 | **O.9.2.3** WSd | Tab. **85** `WSptPct` | **(2)(4)** | TR **+20 %** → **42 kW** | `derive_active_power_kw()` |
| R04 | **O.9.2.1** 110 % V | `FctOpStAuto` | policy | — | yaml `w110_*` (no V meter) |
| R05 | **O.11** priority | — | **(5)(6)** min() | 50/70 → **50**; 80/70 → **70** | `derive_active_power_kw()` |
| R06 | **O.7.3.1** TsP ±5 % | — | **(7)(8)** | TsP ≤ **60 s** | `settled_within_band_kw()` test only |
| R07 | **O.7.3.3** 3 s spacing | — | **(9)** | NOT IMPL | — |
| R08 | **O.13.1.2** DSO loss | Annex U | timers | NOT IMPL | lab **10 s** meter stale only |
| R09 | **O.8.3** 4 s TotW | `MMXU1.PdC.TotW` | 4 s blocks | NOT IMPL | Modbus **1 s** poll |
| R13 | **O.9.1.4 / O.10.3.2** VArSd/VArSa | Tab. **88/89** `VArSptPct` | **(3)** | direct Q % Smax | **Phase 5-R** (not Phase 1) |
| R10–R12 | O.9.1.x reactive curves | VArV/PFW/PFSP | **(10)–(12)** | Tab. defaults | **Phase 5-R** (not Phase 1) |
| R14 | **O.14** logger | — | ≥2048 | PART | `EventRing` |
| R15 | **Annex M** | DIO3 | inhibit | NOT IMPL | yaml spare not parsed |
| L01–L03 | Lab FSM | — | **(13)–(17)** | **900/850**, stale **10 s** | `pf2_fsm.cpp` |
| L04 | Lab ramp | — | **(18)** | **400–950 kW** | `modbus_rtu_slave.py` |

Italian consolidated **Tab. 84 = Wlim**, **Tab. 85 = WSd**. English extract §5 table numbers differ by +1.

---

## Equations

**(1)** \(S_{\max}=\sqrt{\max(P_{\mathrm{imm}}^2,P_{\mathrm{ass}}^2)+\max(Q_{\mathrm{ind}}^2,Q_{\mathrm{cap}}^2)}\)

**(2)** \(P_{\mathrm{kW}} \approx \dfrac{\mathrm{pct}}{100}\,S_{\max,\mathrm{kVA}}\)

**(3)** \(Q_{\mathrm{kVAr}} = \dfrac{\mathrm{VArSptPct}}{100}\,S_{\max,\mathrm{kVA}}\) — VArSd / VArSa direct Q (signed +cap / −ind)

**(4)** \(\mathrm{WSptPct}\in[-100,+100]\) (% Smax, + export)

**(5)(6)** \(P_{\mathrm{effective}}=\min(\text{active export caps/targets in kW})\) — order: 110 % → Wlim → WSd (see `dso_active_power.cpp`)

**(7)(8)** Settled: \(|P-P_{\mathrm{exp}}|/|P_{\mathrm{exp}}|\le 0.05\) within \(T_{sP}\le 60\,\mathrm{s}\)

**(13)–(17)** Lab: `P > threshold_kw`, debounce, release, stale — `pf2_fsm.cpp`

**(18)** \(P = 400 + 550|1 - |(\phi/60)-1||\), \(\phi=t\bmod 120\)

---

## Unit tests

| Test | File |
|------|------|
| R01, R03, R05, O.11, O.7.3.1, Phase1 apply | `tests/unit/dso_phase1_test.cpp` |
| L01–L03 FSM | `tests/unit/pf2_fsm_test.cpp` |

```bash
cmake -S apps/ccli -B apps/ccli/build-native -DCCLI_BUILD_TESTS=ON
cmake --build apps/ccli/build-native
ctest --test-dir apps/ccli/build-native -R 'pf2_fsm|dso_phase1'
```

**Regulation bench (standards vs config / live):**

```bash
ccli --regulation-check --config config/lab_tr400.yaml
ccli --regulation-check --live --json --config config/lab_tr400_dso_tr57126.yaml
# Web UI: lab/regulation_bench/index.html — see lab/REGULATION_BENCH_INTERFACE.md
```

---

## Traceability note

This layer is **Phase 1 engineering mock**, not accredited CEI sign-off. MMS **`Wlim`/`WSd`**, O.13 DSO fallback, and 4 s **`TotW`** remain **NOT IMPL** until Phases 3–5.

---

## Validation summary (Phase 1 complete — equation + lab layer)

| Check | Result |
|-------|--------|
| Master map **R01–L04** in `regulation_map.hpp` + `--regulation-check` | **PASS** |
| All yaml DSO mocks (`w110_*`, `wlim_*`, `wsd_*`, `plant:*`) | **PASS** `dso_phase1_test` |
| Eq (1) TR plant → **206.15 kVA** calc vs **210 kVA** authoritative | **PASS** |
| TR **WSptPct 20 %** → **42 kW** effective | **PASS** |
| O.11 **50/70** and **80/70** + custom **105 kW** | **PASS** |
| O.7.3.1 ±5 % band helper | **PASS** unit only |
| Legacy **900 kW** FSM when mock off | **PASS** `pf2_fsm_test` |
| MMS, O.13 Annex U, 4 s TotW, reactive R10–R13 | **NOT IMPL** (rows in report) |
| Accredited CEI 0-16 certification | **NOT CLAIMED** |

**Known Phase 1 limits (by design):**

- WSd maps to **export ceiling** for threshold FSM, not full O.9.2.3 tracking or TsP ≤ 60 s proof.
- Negative **WSptPct** (import) ignored.
- Reactive functions (VArV, PFW, PFSP) not in `core/dso/`.
- `dso.enabled` without `pf2.use_dso_mock` leaves yaml kW thresholds unchanged.

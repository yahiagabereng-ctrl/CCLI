# EMT432 — CCLI yaml commissioning vs Modbus map

**Document ID:** CCLI-LAB-EMT432-CCLI-COMM-001  
**Revision:** 5.0  
**Date:** 2026-10-22  
**Code:** `apps/ccli/adapters/modbus_master/modbus_emt432_commissioning_revision.hpp` (`kRevision` / `kRevisionStr`)  
**Parent:** [EMT432_CERT_REGISTER_PIN.md](EMT432_CERT_REGISTER_PIN.md)  
**Transducer MR:** [EMT432_TRANSUCER_HOLDING_SPEC.md](EMT432_TRANSUCER_HOLDING_SPEC.md)  
**Energy MR:** [EMT432_ENERGY_HOLDING_SPEC.md](EMT432_ENERGY_HOLDING_SPEC.md)  
**Device map:** `apps/ccli/config/modbus/chronos_emt432_map.yaml`  
**Lab yaml:** `apps/ccli/config/lab_tr400_emt432_lan3_tcp_102.yaml`

CCLI applies **`modbus.commissioning`** once after TCP connect (when `enabled: true`). Poll registers are separate under **`modbus.reg_*`**.

Yaml **`modbus.commissioning.revision`** must match compiled **`kRevision`** (currently **4**). Mismatch → commissioning **ABORT** (no meter writes).

All **offsets** below are **0-based from 40001** (same as `reg_active_power: 932`).

---

## Revision history (code + doc)

| Rev | Date | Code `kRevision` | Change |
|-----|------|------------------|--------|
| **5.0** | 2026-10-22 | **5** | **40245** min current ripple + **40247** min power ripple (bench **0**) |
| **4.0** | 2026-10-22 | **4** | Energy: **40256** kWh unit; **40232** bit **9**; read **KWh1/KVARh1/KVAh1** |
| **3.0** | 2026-10-22 | **3** | TA/TV block **40234–40243** verify/write bench defaults (see transducer spec) |
| **2.0** | 2026-10-22 | **2** | 40232 bit **15** filtered measurement (`write_filtered_measurement`) |
| **1.0** | 2026-10-22 | **1** | Initial: verify 40011; RMW 40232 1P+integrator; optional 40235; revision gate |

When adding registers or yaml keys: increment **`kRevision`** and **`kRevisionStr`**, bump this table, set lab yaml `commissioning.revision`, and update §1.

---

## 1. Commissioning (writes / verify) — yaml keys

| Step | PLC | Offset | MR name | Yaml flag / field | Action |
|------|-----|--------|---------|-------------------|--------|
| 0 | 40011 | **10** | mounted_version | `verify_mounted_version: true` | **Verify** u16 == **1** (Rogowski) |
| **1** | 40232 | **231** | Measurement_Config | `write_meas_1p_integrator: true` | **RMW** connection mode **0** + **bit 13** integrator ON |
| **1b** | 40232 | **231** | (same) | `write_filtered_measurement: true` | **RMW** **bit 15** filtered measurement **ON** (MR; not Rogowski integrator) |
| — | 40232 | **231** | (same) | (always when commissioning enabled) | **Verify** 1P+integrator; verify bit **15** if filter write enabled |
| **2** | 40235 | **234** | TA_Transducer_ratio | `write_rogowski_sensitivity: true` | **Write** float32 **[V/A]** |
| 2b | 40235 | **234** | (same) | `rogowski_sensitivity_v_per_a: 0.000085` | **85 mV/kA** → **0.000085** |
| 2c | 40235 | **234** | (same) | `rogowski_sensitivity_v_per_a: 0.0001` | **FRC-350 100 mV/kA** label |
| **3** | 40234 | **233** | TA_Chronos_register | `verify_ta_chronos_register` / `write_ta_chronos_register` | **0** = user TA |
| **4** | 40237 | **236** | TA_Transducer_delay | `verify_transducer_bench_defaults` / `write_transducer_bench_defaults` | **0** ° @ 50 Hz |
| **4** | 40239 | **238** | TV_Transducer_ratio | `tv_transducer_ratio: 1.0` | Direct voltage (no VT) |
| **4** | 40241 | **240** | TV_Transducer_delay | `tv_transducer_delay_deg: 0` | **0** ° |
| **4** | 40243 | **242** | minimum_voltage_ripple | `minimum_voltage_ripple: 0` | **0** — non-zero can force **0** V readings |
| **4b** | 40245 | **244** | minimum_current_ripple | `minimum_current_ripple: 0` | **0** — non-zero can force **0** I (Rogowski accuracy) |
| **4c** | 40247 | **246** | minimum_power_ripple | `minimum_power_ripple: 0` | **0** — non-zero can force **0** P/Q/S |
| **5** | 40256 | **255** | Energy_unit_factor | `verify_energy_unit_kwh` / `write_energy_unit_kwh` | **4** = kWh |
| **5b** | 40232 | **231** | bit **9** Energy saving | `write_energy_saving: true` | MR energy-saving mode ON |
| **6** | 40716–40857 | **715–856** | KWh/KVARh/KVAh* | `log_energy_readings: true` | **Read** int64 Wh/10 → kWh (not MMS poll yet) |

**Not in ccli:** 40227–40231 RTU comm; IP/gateway (portal). Full energy list: [EMT430_ENERGY_REGISTER_OFFSETS.txt](EMT430_ENERGY_REGISTER_OFFSETS.txt).

Full MR text: [EMT432_TRANSUCER_HOLDING_SPEC.md](EMT432_TRANSUCER_HOLDING_SPEC.md).

**Enable order on bench:** Step **1** first (`write_meas_1p_integrator`). Step **2** only when **LCD I1/P1 > 0** with load; then set `write_rogowski_sensitivity: true` and redeploy yaml.

---

## 2. Realtime reads (poll → MMS) — yaml keys

| PLC | Offset | Signal | Yaml key | Scale | MMS |
|-----|--------|--------|----------|-------|-----|
| 40933 | **932** | P1 [W] | `reg_active_power: 932` | `power_scale: 0.001` | TotW kW |
| 40941 | **940** | Q1 [VAR] | `reg_reactive_power: 940` | `reactive_scale: 0.001` | TotVAr |
| 40907 | **906** | V_L1_N [V] | `reg_line_voltage: 906` | `line_voltage_scale: 0.001` | PPV.phsAB kV |
| 40923 | **922** | I1 [A] | `reg_phase_a_current: 922` | `phase_a_current_scale: 1.0` | A.phsA A |

**Common decode:** `float_word_swap: true` (Chronos TCP float32).

**Optional diagnostics (not on MMS yet):**

| PLC | Offset | Signal |
|-----|--------|--------|
| 40973 | 972 | Frequency [Hz] |

Do **not** map PPV to **905** (40906 is u16 alarm, not voltage).

---

## 3. Example yaml blocks

**Phase A — 40232 only (recommended first deploy):**

```yaml
modbus:
  commissioning:
    revision: 5
    enabled: true
    apply_once: true
    verify_mounted_version: true
    write_meas_1p_integrator: true
    write_filtered_measurement: true
    verify_ta_chronos_register: true
    write_ta_chronos_register: true
    verify_transducer_bench_defaults: true
    write_transducer_bench_defaults: true
    verify_energy_unit_kwh: true
    write_energy_unit_kwh: true
    write_energy_saving: true
    log_energy_readings: true
    write_rogowski_sensitivity: false
    rogowski_sensitivity_v_per_a: 0.000085
```

**Phase B — add 40235 after LCD stable:**

```yaml
    write_rogowski_sensitivity: true
    rogowski_sensitivity_v_per_a: 0.000085   # or 0.0001 for 100 mV/kA coil
```

**Disable all meter writes (poll only):**

```yaml
  commissioning:
    enabled: false
```

---

## 4. Verification

1. Deploy yaml to DUT: `lab/tg544-openwrt/deploy-emt432-80mv.ps1` (yaml-only is fine).  
2. Restart **ccli**; on stderr look for `modbus: EMT432 commissioning PASS`.  
3. Cross-check: `python lab/emt432_rogowski_config.py --host 192.168.178.10 --verify-cert-pin`  
4. Confirm poll: DUT trace / MMS TotW vs meter LCD P1.

---

## 5. Adding a new commissioning register (developer)

1. Increment **`kRevision`** / **`kRevisionStr`** in `modbus_emt432_commissioning_revision.hpp`.  
2. Set lab yaml **`commissioning.revision`** to the same integer.  
3. Add field under `ModbusCommissioningConfig` in `core/config/ccli_config.hpp`.  
4. Parse under `modbus` → `commissioning` indent-4 in `ccli_config.cpp`.  
5. Implement read/write/verify in `modbus_emt432_commissioning.cpp`.  
6. Document row in **§1** and add a row to **Revision history**; bump cert pin if type-test scope changes.

# EMT432 — TA/TV transducer holding registers (EMT430_MR)

**Document ID:** CCLI-LAB-EMT432-TA-TV-001  
**Revision:** 1.0  
**Date:** 2026-10-22  
**Parent:** [EMT432_CERT_REGISTER_PIN.md](EMT432_CERT_REGISTER_PIN.md) · [EMT432_CCLI_COMMISSIONING_MAP.md](EMT432_CCLI_COMMISSIONING_MAP.md)  
**MR:** `knowledge-base/08-engineering/reference/vendor/chronos/EMT430_MR.pdf`

Offsets are **0-based from 40001** (ccli / pymodbus). All float fields are **float32**, 2 holding registers, **word-swap** on EMT432 TCP (`float_word_swap: true`).

---

## Register table

| PLC | Offset | Type | Name | Meaning | Factory / bench (1P Rogowski, direct V) |
|-----|--------|------|------|---------|----------------------------------------|
| **40234** | **233** | u16 | `TA_Chronos_register` | 0=user TA; 1=Chronos 100/5; 2=… | **0** (user); cert verify |
| **40235** | **234** | f32 | `TA_Transducer_ratio` | 5A: CT ratio M/N; **Rogowski: sensitivity [V/A]** | **0.000085** (85 mV/kA) or **0.0001** (100 mV/kA) |
| **40237** | **236** | f32 | `TA_Transducer_delay` | Current / TA phase delay [°] @ 50 Hz | **0** |
| **40239** | **238** | f32 | `TV_Transducer_ratio` | Voltage transformer ratio M/N | **1.0** (no external VT) |
| **40241** | **240** | f32 | `TV_Transducer_delay` | VT phase delay [°] @ 50 Hz | **0** |
| **40243** | **242** | f32 | `minimum_voltage_ripple` | Below threshold → instrument reads **0** | **0** (do not raise on bench) |

**40235 factory 1.0** is for **5 A CT**, not Rogowski — wrong value zeros or scales I/P incorrectly.

---

## CCLI commissioning (rev ≥ 3)

| Yaml key | Register |
|----------|----------|
| `verify_ta_chronos_register` / `write_ta_chronos_register` | 40234 → **0** |
| `verify_transducer_bench_defaults` / `write_transducer_bench_defaults` | 40237, 40239, 40241, 40243 |
| `tv_transducer_ratio`, `ta_transducer_delay_deg`, `tv_transducer_delay_deg`, `minimum_voltage_ripple` | Target values when writing defaults |
| `write_rogowski_sensitivity` + `rogowski_sensitivity_v_per_a` | 40235 |

---

## Verification

```powershell
python lab\emt432_rogowski_config.py --host 192.168.178.10 --read
```

DUT ccli stderr after deploy: transducer PASS lines when commissioning enabled.

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-22 | Initial MR extract 40234–40243 + bench defaults |

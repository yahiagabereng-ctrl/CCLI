# EMT432 — Energy holding registers (EMT430_MR Rev2)

**Document ID:** CCLI-LAB-EMT432-ENERGY-001  
**Revision:** 1.0  
**Date:** 2026-10-22  
**Parent:** [EMT432_CERT_REGISTER_PIN.md](EMT432_CERT_REGISTER_PIN.md) · [EMT432_CCLI_COMMISSIONING_MAP.md](EMT432_CCLI_COMMISSIONING_MAP.md)  
**MR:** `knowledge-base/08-engineering/reference/vendor/chronos/EMT430_MR.pdf`

Values are **`signed long long`**, **4 holding registers** each, scale **\[Wh tenth\]** → **kWh = raw / 10000** (0.1 Wh per LSB).

Offsets = **PLC address − 40001**.

---

## Configuration

| PLC | Offset | Type | Name | Bench |
|-----|--------|------|------|--------|
| **40256** | **255** | u16 | `Energy_unit_factor` | **4** = display unit kWh (0 Wh/10, 1 Wh, 4 kWh, …) |
| **40232** bit **9** | **231** | bit | Energy saving | Preserve or `write_energy_saving: true` → bit **1** |
| **40232** bit **6** | **231** | bit | DO pulsed kWh | Not changed by ccli unless extended |

---

## Active / reactive / apparent energy (instantaneous totals)

| PLC | Offset | Name | 1P bench |
|-----|--------|------|----------|
| 40716 | **715** | `KWh1` | **Primary** import active |
| 40729 | **728** | `KWh_SUM` | Sum (≈ KWh1 on 1P) |
| 40733–40761 | 732–760 | `KWh*_Plus/Neg` | Bidirectional splits |
| 40765 | **764** | `KVARh1` | Reactive L1 |
| 40777 | **776** | `KVARh_SUM` | Reactive sum |
| 40781–40841 | 780–840 | `KVARh*_Ind/Cap ±` | Quadrant splits |
| 40845 | **844** | `KVAh1` | Apparent L1 |
| 40857 | **856** | `KVAh_SUM` | Apparent sum |

Full list (36 registers): [EMT430_ENERGY_REGISTER_OFFSETS.txt](EMT430_ENERGY_REGISTER_OFFSETS.txt).

---

## CCLI (commissioning rev ≥ 4)

| Yaml | Register |
|------|----------|
| `verify_energy_unit_kwh` / `write_energy_unit_kwh` | 40256 = **4** |
| `write_energy_saving` | 40232 bit **9** ON |
| Poll (future / lab script) | 715, 764, 844, 728 |

Cert **TotW** path still uses instantaneous **932** P1 [W]; energy registers are for **UI parity** and export evidence.

---

## Lab read

```powershell
python lab\emt432_rogowski_config.py --host 192.168.178.10 --read
```

Shows **KWh1 / KVARh1 / KVAh1 / KWh_SUM** in kWh after config registers.

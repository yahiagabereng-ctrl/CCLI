# EMT432 — Modbus map validation vs EMT430_MR Rev2

**Document ID:** CCLI-LAB-EMT432-MR-VAL-001  
**Date:** 2026-10-22  
**Source:** `knowledge-base/08-engineering/reference/vendor/chronos/EMT430_MR.pdf`  
**Full RMS block:** [EMT432_RMS_REALTIME_HOLDING_SPEC.md](EMT432_RMS_REALTIME_HOLDING_SPEC.md) · [EMT430_RMS_REGISTER_OFFSETS.txt](EMT430_RMS_REGISTER_OFFSETS.txt)  
**Rule:** Holding offset = **PLC 4xxxx address − 40001** (pymodbus / ccli `reg_*`).

---

## Root cause (repo error)

**40906** in the MR is **not** `V_L1_N`. It is a **UINT16** configuration field (alarm / frequency-detection channel start).  
**`V_L1_N` RMS float starts at PLC 40907** → offset **906**.

Using offset **905** reads the u16 at 40906 plus the **first word** of the voltage float → garbage V on Modbus while the **LCD** still shows correct V/P/I.

---

## Realtime RMS block (validated)

| Signal | PLC addr | Offset @40001 | ccli yaml | MR name |
|--------|----------|---------------|-----------|---------|
| V_L1_N RMS | **40907** | **906** | `reg_line_voltage: 906` | `V_L1_N` |
| I_L1 RMS | 40923 | 922 | (not in ccli today) | `I_L1` |
| P1 | 40933 | 932 | `reg_active_power: 932` | `P1` |
| Q1 | 40941 | 940 | `reg_reactive_power: 940` | `Q1` |
| Frequency | 40973 | 972 | — | `Frequency` |

**Do not use for 1P P/Q/V:**

| PLC | Offset | MR name | Why |
|-----|--------|---------|-----|
| 40987 | 986 | `I_L1_peak` | Peak **current**, not power (was misread as ~541 “W”) |
| 40965 | 964 | `CF1` | Crest factor, not amps |
| 40981 | 980 | `V_L1_L2_peak` | 3P line-line peak |

---

## Verification

```powershell
python lab/modbus_emt432_tcp_probe.py --host 192.168.178.10
# V_L1_N RMS @40907/906 ≈ LCD L1-N; P1 @40933 ≈ LCD P1
```

After yaml change: `deploy-emt432-80mv.ps1` → DUT `logread -e modbus` P and decoded V sane.

## Modbus Schema Toolkit (Basjes)

| Artifact | Path |
|----------|------|
| Schema (SDM630-style) | `apps/ccli/config/modbus/chronos_emt432_basjes.yaml` |
| Offline cross-check | `python lab/emt432_validate_register_map.py` |
| Upstream docs | [modbus.basjes.nl](https://modbus.basjes.nl/) · [SDM630.yaml reference](https://github.com/nielsbasjes/modbus-devices/blob/main/modbus-device-sdm630/SDM630.yaml) |

Run Basjes JVM tests (optional, Java 17 + Maven): validate schema in `modbus-device-*` module pattern.

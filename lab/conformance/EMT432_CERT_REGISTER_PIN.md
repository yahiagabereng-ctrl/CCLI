# EMT432 certification register pin — fixed bench / type-test specimen

**Document ID:** CCLI-LAB-EMT432-CERT-PIN-001  
**Revision:** 1.1  
**Date:** 2026-10-22  
**RAG source_id:** `ccli-emt432-cert-register-pin`  
**Parent:** [METER_SPECIMEN_EMT432_V_FULL.md](METER_SPECIMEN_EMT432_V_FULL.md)  
**Deploy:** `lab/tg544-openwrt/deploy-emt432-80mv.ps1` · `lab/emt432_rogowski_config.py --apply-cert-pin`  
**DUT yaml:** `apps/ccli/config/lab_tr400_emt432_lan3_tcp_102.yaml` · **CCLI commissioning map:** [EMT432_CCLI_COMMISSIONING_MAP.md](EMT432_CCLI_COMMISSIONING_MAP.md) · **TA/TV holding:** [EMT432_TRANSUCER_HOLDING_SPEC.md](EMT432_TRANSUCER_HOLDING_SPEC.md)

This is the **only** Modbus + yaml map approved for **5-LN Milestone A** and **O.15 #2** meter evidence on specimen **EMT432.V.FULL** + **85 mV/kA** Rogowski. Do not change offsets or scales without bumping this revision.

---

## Requirements

| ID | Requirement |
|----|-------------|
| REQ-EMT-CERT-001 | Meter **40235** = **0.000085 V/A** (85 mV/kA), float32 word-swap |
| REQ-EMT-CERT-002 | **40232** connection **single phase** (bits 1–2 = 0) + **integrator ON** (bit 13 = 1) |
| REQ-EMT-CERT-003 | **40011** mounted_version = **1** (Rogowski / 333 mV path) |
| REQ-EMT-CERT-004 | Realtime reads: **P1 932 · Q1 940 · V_L1_N 906** (40907; offsets from 40001), scale **0.001** W→kW |
| REQ-EMT-CERT-005 | **No** extra filter/demand register writes for PMF path — integrator in **40232** is the Rogowski filter |
| REQ-EMT-CERT-006 | Apply + verify before cert sessions; evidence in deploy log or `P5_EMT432_LAN3_VERIFY_*` |

---

## Meter holding registers (write / verify)

| PLC addr | Offset | Name | Cert value | Verify |
|----------|--------|------|------------|--------|
| **40011** | 10 | mounted_version | **1** | RO == 1 |
| **40232** | 231 | Measurement_Config | bits: **mode 0**, **bit13=1** | decode, not fixed raw |
| **40235** | 234 | TA / Rogowski sensitivity | **0.000085** V/A | abs(Δ) < 1e-8 |
| **40234** | 233 | TA_Chronos_register | **0** (user) | RO == 0 typical |

**40232:** Do **not** pin a single decimal raw word across firmware revs; pin **connection_mode == 0** and **integrator_rogowski == true**. Script clears bits 1–2 and sets bit 13 (read-modify-write).

**Not written for cert:** cutoff / demand / PQ timer registers (meter UI only; see specimen § Impostazioni potenze).

---

## Realtime holding registers (read only — ccli map)

| PLC addr | Offset | Signal | ccli yaml key | Scale to MMS |
|----------|--------|--------|---------------|--------------|
| **40933** | **932** | P1 active [W] | `reg_active_power: 932` | × **0.001** → kW TotW |
| **40941** | **940** | Q1 reactive [VAR] | `reg_reactive_power: 940` | × **0.001** → kvar |
| **40907** | **906** | V_L1_N RMS [V] | `reg_line_voltage: 906` | × **0.001** → kV PPV |

**40906** is **UINT16** alarm/channel config (EMT430_MR) — **not** voltage. Do not map PPV to 905.

| Parameter | Value |
|-----------|--------|
| Modbus TCP | **192.168.178.10:502**, unit **1** |
| float_word_swap | **true** |
| poll_ms (DUT) | **4000** (independent of O.8 URCB **IntgPd=4000**) |

---

## Verification

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1
# or read-only:
python lab\emt432_rogowski_config.py --host 127.0.0.1 --port 15002 --verify-cert-pin
```

**Pass:** `CERT PIN: PASS` · **Fail:** non-zero exit; do not run **U-T1-04** until fixed.

---

## Maintenance

| Rev | Date | Change |
|-----|------|--------|
| 1.1 | 2026-10-22 | **V_L1_N → 906 (40907)** per EMT430_MR; 40906 is not V |
| 1.0 | 2026-10-22 | Initial pin: **85 mV/kA**, 932/940/905, 40232 1P+integrator |

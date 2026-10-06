# Chronos EMT432 — engineering extract

**Document ID:** CCLI-CHRONOS-EMT432-001  
**Date:** 2026-10-05  
**RAG source_id:** `ccli-chronos-emt432`  
**Vendor:** [Chronos srl](https://chronos-tech.it/) · Selvazzano Dentro (PD), Italy  
**Product:** EMT432 three-phase network analyzer (Base / Full)  
**CCLI track:** P5-06 meter map · P5-04 accuracy · POC Modbus on TG544 A1/B1

---

## Corpus files (HAVE)

| File | Role |
|------|------|
| `reference/vendor/chronos/EMT432_MI-ENG.pdf` | Installation manual EN rev 2 |
| `reference/vendor/chronos/EMT432_MI-ITA.pdf` | Installation manual IT rev 2 (wiring) |
| `reference/vendor/chronos/EMT432_FL-ITA.pdf` | Product flyer IT |
| `reference/vendor/chronos/EMT430_MR.pdf` | Modbus register map Rev2 (EMT430/432 family) |
| `apps/ccli/config/modbus/chronos_emt432_map.yaml` | CCLI register YAML (generated) |

**Download URLs (static CDN):**

- `EMT432_MI-ENG.pdf` → https://static.chronos-tech.it/EMT_432_MI_ENG_3d382e57d1.pdf
- `EMT432_MI-ITA.pdf` → https://static.chronos-tech.it/EMT_432_MI_ITA_9fcf8aaff7.pdf
- `EMT432_FL-ITA.pdf` → https://static.chronos-tech.it/EMT_432_FL_ITA_8f0283606e.pdf
- `EMT430_MR.pdf` → https://static.chronos-tech.it/EMT_430_MR_cb7f1d080e.pdf

---

## Product summary

| Item | Value |
|------|-------|
| Aux supply | 10–36 V DC or 13–26 V AC |
| VT range L-N | 85–265 V AC |
| VT range L-L | 150–450 V AC |
| Current inputs | **I** = TA 5 A · **V** = 333 mV / Rogowski (internal integrator) |
| Comms | RS485 Modbus RTU · Ethernet/WiFi Modbus TCP · NFC |
| Accuracy (vendor) | Class 0.2S EN 62053-22 · ±0.1% rdg P @ 25°C |
| Class II | **No PE bond** on instrument (EN 61140) |

### Order code

`EMT432` + `I` (5 A TA) or `V` (Rogowski/333 mV) + `BASE` or `FULL`.

---

## Modbus RTU defaults (EMT430_MR Rev2)

| Parameter | Register | Default | CCLI lab note |
|-----------|----------|---------|---------------|
| Slave address | 40227 (`modbus_RTU_address`) | **1** | Match `modbus.slave_id` |
| Baud code | 40229 | **3** → **9600** | Match `modbus.baud` |
| Parity code | 40230 | **0** → NONE | Match `modbus.parity: N` |
| Stop bits | 40231 | **0** → 1 bit | 8N1 |

Baud code map: 0=1200, 1=2400, 2=4800, 3=9600, 4=19200, 5=38400, 6=57600, 7=115200

### Measurement configuration (40232)

Bits 1–2 connection mode:

- `0` → **Single phase**
- `1` → 3P 3-wire 2 TA (Aron)
- `2` → 3P 3-wire 3 TA
- `3` → 3P 4-wire 3 TA + N

For **230 V 1P bench**: set single-phase + wire L1/N + one TA on I1.

---

## Key realtime registers (float32, 2 regs, RO)

| Register | Modbus addr | Offset @40001 | Description | CCLI map |
|----------|-------------|---------------|-------------|----------|
| `P_SUM` | 40939 | 938 | RMS sum active power [W] | PdCMMXU1.TotW (kW, scale 0.001) |
| `Q_SUM` | 40947 | 946 | RMS sum reactive power [VAR] | PdCMMXU1.TotVAr (kvar, scale 0.001) |
| `P1` | 40933 | 932 | RMS active power line 1 [W] | 1P bench TotW |
| `Q1` | 40941 | 940 | RMS reactive power line 1 [VAR] | 1P bench TotVAr |
| `Frequency` | 40973 | 972 | Frequency [Hz] | Grid frequency |
| `V_L1_L2_peak` | 40981 | 980 | Line voltage L1-L2 peak [V] | PPV proxy (verify vs RMS avg regs) |
| `V_L1_N_peak` | 40975 | 974 | Star voltage L1-N peak [V] | — |
| `modbus_RTU_address` | 40227 | 226 | Modbus RTU slave address | — |
| `modbus_RTU_baudrate` | 40229 | 228 | Baudrate code 0=1200..7=115200 | — |
| `modbus_RTU_parity` | 40230 | 229 | 0=NONE 1=EVEN | — |
| `modbus_RTU_Stop_bit` | 40231 | 230 | Stop bit code | — |
| `Measurement_Configuration_Register` | 40232 | 231 | Connection mode bits 1-2: 0=single phase | — |
| `feature_version` | 40015 | 14 | 0=Base 1=Full | — |
| `mounted_version` | 40011 | 10 | 0=5A 1=Rogowski/333mV | — |

> **Verify on bench:** float byte order and whether libmodbus uses address `offset` or `address-1`.

---

## Lab wiring — 230 V monofase (no Rogowski)

From MI-ITA § schemi di collegamento + bench photo (2026-10-05):

- **Monofase, 2 fili, connessione con 1 TA** — L1, N, I1 on one CT; L2/L3 open.
- **Pow+ / Pow-** = **18–36 V DC aux only** — **never** connect 230 V mains here.
- RS485 screw block: **G** (GND) · **B** (B−) · **A** (A+) → TG544 A1/B1.
- **Class II — do not bond instrument PE**; avoid N–PE current.

**Terminal diagram:** `lab/evidence/phase5/P5_EMT432_TERMINAL_WIRING.md`

### CCLI bench topology

```text
24 V DC → Pow+ / Pow-     (aux — energise before or with mains)
230 V L+N → L1 + N        (L2, L3 open)
CT 5A     → I1 S1 / S2    (optional; without CT: V/f OK, P/Q ≈ 0)
RS485 A/B/G → TG544 A1/B1 (/dev/ttyS1)
Optional: COM5 pymodbus slave remains plant Q command path (FC16 on A2/B2)
```

---

## Introduction excerpt (MI-ENG)

```text
Product description
Three-phase network analyzer with power supply 10 - 36 VDC or 13 - 26 VAC, 5 (6) A or 333mV/Rogowski.
320x240 pixel color graphic display, RS485 port, Ethernet, USB, SD card Wifi, NFC, pulse output, RTC.
Product features
●Equivalent to class 0.2S (kWh) of EN 62053-22
●Equivalent to class 0.2S (kvarh) of EN 62053-24
●Accuracy ±0.2% RDG
●Bidirectional energy meter
●TRMS measurement of distorted waveforms
(voltage/current)
●Neutral current calculation
●One opto-MOS output for alarms or pulses
●Wi-Fi Station and Access Point (Modbus TCP)
●Ethernet (Modbus TCP)
●RS485 serial output (Modbus RTU)
●NFC
●Logging via USB or SD card (or internal memory) with
timestamp
●2.2” color display, 16-bit, 320x240 pixels, capacitive
touchscreen
●Sampling frequency: 6400 samples @ 50 Hz
●Dimensions: 4 DIN modules
●Direct association of Chronos CTs via App to correct
phase shift and signal amplitude
●Complete harmonic analysis (amplitudes and phases)
●Two versions for CT input: 5 A current or voltage (333
mV/Rogowski)
●Internal integrator for Rogowski coils
●Available in two variants: Base, Full
SYMBOLS
DANGER:
Failure to follow the instructions
may cause damage to property,
people, or animals.
WARNING, DANGEROUS VOLTAGE:
Presence of electrical voltage.
Disconnect the power supply before
performing any work.
DOUBLE INSULATION:
Class 
II 
equipment. 
No
grounding 
connection 
is
required.
WEEE:
Do not dispose of with household waste.
Deliver to authorized collection centers.

ENG - Rev 2
2
```

---

## Verification (P5-06)

| Step | Pass |
|------|------|
| Read `machine_id_*` @ 40001 → spells EMT430/432 | Identity |
| `feature_version` @ 40015 = 0 Base / 1 Full | SKU |
| `P_SUM`/`Q_SUM` track load on 1P+CT | Metrology |
| ccli `modbus: FC3` log @ 4 s | P5-02/P5-M07 |

**Total registers parsed from MR PDF:** 251

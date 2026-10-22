# Chronos Rogowski (FRC) + EMT430/432 bench extract

**Document ID:** CCLI-CHRONOS-ROGOWSKI-001  
**Date:** 2026-10-21  
**RAG source_id:** `ccli-chronos-rogowski-bench`  
**Vendor:** [Chronos srl](https://chronos-tech.it/)  
**Parents:** `ccli-chronos-emt432` · `EMT430_MR.pdf` · `TA_FL_ITA.pdf`

---

## Corpus files (HAVE)

| File | Role |
|------|------|
| `reference/vendor/chronos/TA_FL_ITA.pdf` | CT / Rogowski catalog IT (FRC-350, KCT, CP, …) |
| `reference/vendor/chronos/EMT430_MR.pdf` | Modbus register map (same for TCP/RTU) |
| `apps/ccli/config/modbus/chronos_emt432_map.yaml` | CCLI register YAML |
| `apps/ccli/config/lab_tr400_emt432_lan3_tcp_102.yaml` | Bench overlay — meter **192.168.178.102** |
| `knowledge-base/08-engineering/CCI_Chronos_EMT432_Web_UI_Capabilities.md` | INSTALLER portal map · MQTT/logging · CCLI gap matrix |

**Download (TA catalog):** https://static.chronos-tech.it/TA_FL_ITA_cb6650f79f.pdf

---

## Product pairing (Rogowski)

| Item | Requirement |
|------|-------------|
| Meter order code | **V** suffix = **333 mV / Rogowski** + **internal integrator** |
| Coil | **FRC-350-100mV** = **100 mV/kA @ 50 Hz**. Catalog has **no 85 mV** SKU. |
| Wrong pairing | **FRC on I (5 A CT) meter** → no valid I/P |
| Verify on Modbus | `mounted_version` @ **40011** → **1** = Rogowski/333 mV |

EMT register and scaling details: [CCI_Chronos_EMT432_Extract.md](CCI_Chronos_EMT432_Extract.md).

---

## FRC series excerpt (from TA_FL_ITA)

```text
serie FRC
Caratteristiche prodotto Applicazioni:
● Compatibile con analizzatori
con integratore con ingresso
TA in tensione
● Elevata linearità
● Completamente schermato
● Elevata reiezione ai disturbi
● Misura, monitoraggio e protezione della corrente per cablaggi
elettrici e apparecchiature.
● Misura di corrente e potenza per motori elettrici, illuminazione,
compressori d’aria, sistemi di riscaldamento e ventilazione,
apparecchiature di condizionamento e sistemi di automazione
e controllo.
● Dispositivi per il monitoraggio di corrente, potenza ed energia.
● Misure di corrente molto elevate
● Misure in ambienti difficilmente raggiungibili
SONDA DI ROGOWSKI
Dimensioni (mm)
Modello FRC-350 FRC-420
A: Finestra 1 105 130
B: Finestra 2 100 125
C: Diametro esterno 121 142
D: Diametro spira 8
E: Lunghezza cavo 2000
F: Lunghezza Spira 350 420

ITA - Rev 1
SEDE LEGALE
Via Germania 7 - 35010 Vigonza (PD)
SEDE OPERATIVA
Via Ceresina 11/e - 35030 Selvazzano Dentro (PD)
T +39 049 227 4996
info@chronos-tech.it
www.chronos-tech.it
subject to change without notice
Sensibilità alla posizione
Posizione del conduttore Errore tipico
Adiacente al centro della spira < 0.2%
Adiacente al lato della spira < 1%
Specifiche tecniche
Modello FRC-350 FRC-420
Lunghezza della spira 350 mm 420 mm
Diametro finestra 100 mm 120 mm
Resistenza spira 210 Ω 250 Ω
Sensibilità 100 mV/kA @ 50 Hz
50 mV/kA @ 50 Hz
Linearità ±0.2%
Sezione spira 8 mm
Lunghezza cavo 2 metri
Temperatura di lavoro -30°C .. +80 °C
Materiale Gomma termoplastica UL94 V0
Certificazioni EN 61010-1:2010, EN 61326-1:2013
Isolamento Spira: 2000V
Cavo di segnale: 1000 V
Categoria installazione 1000V CAT III, 600V CAT IV
Modello Sensibilità Lunghezza spira Codice d’ordine
FRC-350 100 mV/kA 350 mm FRC-350-100mV
FRC-420 100 mV/kA 420 mm FRC-420-100mV
FRC-420 50 mV/kA 420 mm FRC-420-50mV
```

---

## Modbus registers (ccli — unchanged for Rogowski)

Offsets from **40001** (holding registers). Float32 × 2, **`float_word_swap: true`** on bench.

| Use | Register | Offset | Maps to ccli / MMS |
|-----|----------|--------|---------------------|
| P (1P) | P1 | 932 | TotW (×0.001 → kW) |
| Q (1P) | Q1 | 940 | TotVAr |
| V L1-N RMS | V_L1_N_rms | 906 | PPV (×0.001 → kV) |
| Connection mode | Measurement_Configuration_Register | 40232 (off 231) | bits 1–2: **0** = single phase |
| Input type | mounted_version | 40011 (off 10) | **1** = Rogowski |
| Sensitivity | TA_Transducer_ratio | 40235 (off 234) | **0.0001 V/A** (100 mV/kA). Not 85, not factory 1.0 |

TCP: **502**, unit **1**. Plant LAN3 example: meter **192.168.178.102/24**, GW **192.168.178.1** (DUT).

---

## Wiring checklist (1P + FRC-350)

1. **Pow+ / Pow−** = 18–36 V DC aux only (never mains on aux).
2. **L1 + N** for 1P; L2/L3 open.
3. **FRC-350** around conductor on L1 path; **blue/black** → **I1+ / I1−** (same as **I1 S1/S2** in MI-ITA). Swap if **P** is negative with import load.
4. **Ethernet** → LAN3; set IP/GW on meter (UI / app / NFC).
5. PC plant NIC: `lab/set_pc_lan3_plant_178.cmd` → **192.168.178.10/24**.

---

## Verification

| Step | Command / check | Pass |
|------|-----------------|------|
| TCP probe | `python lab/modbus_emt432_tcp_probe.py --host 192.168.178.102` | connect OK · **333mV/Rog** · P1 plausible |
| DUT deploy | `lab_tr400_emt432_lan3_tcp_102.yaml` → `/etc/ccli/lab.yaml` | `grep host` → .102 |
| Bench script | `validate-bench-hw-configuration.ps1 -MeterHost 192.168.178.102 -ConfigYaml lab_tr400_emt432_lan3_tcp_102.yaml -ProbeMeter` | PASS |

**Wiring doc:** [lab/evidence/phase5/P5_ROGOWSKI_FRC350_BENCH.md](../../lab/evidence/phase5/P5_ROGOWSKI_FRC350_BENCH.md)

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-21 | Initial FRC-350 + .102 bench overlay |

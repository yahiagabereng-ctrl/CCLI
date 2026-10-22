# Meter specimen pin — Chronos EMT432.V.FULL + FRC-350-100mV

**Document ID:** CCLI-PROD-SPEC-EMT432-001  
**Revision:** 1.1  
**Date:** 2026-10-21  
**RAG source_id:** `ccli-meter-specimen-emt432-v-full`  
**Parents:** `ccli-chronos-emt432` · `ccli-chronos-rogowski-bench` · `ccli-chronos-emt432-ui` · `ccli-61557-12-extract`  
**Normative object:** PMD front-end for O.15 #2 / O.13.2. TesPro TG544 stays **outside** IEC 61557-12:2018 §1.

**Datasheets HAVE (2026-10-21 CDN pull):**

| File | Bytes | Role |
|------|------:|------|
| `knowledge-base/08-engineering/reference/vendor/chronos/EMT432_FL-ITA.pdf` | 869418 | Flyer / class / 85–265 V L-N |
| `knowledge-base/08-engineering/reference/vendor/chronos/EMT432_MI-ENG.pdf` | 4353874 | Install EN |
| `knowledge-base/08-engineering/reference/vendor/chronos/EMT432_MI-ITA.pdf` | 4290048 | Install IT / wiring |
| `knowledge-base/08-engineering/reference/vendor/chronos/EMT430_MR.pdf` | 1277777 | Modbus map Rev2 |
| `knowledge-base/08-engineering/reference/vendor/chronos/TA_FL_ITA.pdf` | 1940085 | FRC / CT catalog |
| [chronos-tech.it/emt432](https://chronos-tech.it/prodotti/chronos/emt432/) | — | Web datasheet (same numbers) |

---

## Requirements

| ID | Requirement | Basis | Status |
|----|-------------|-------|--------|
| **REQ-MTR-001** | Frozen meter SKU = **EMT432.V.FULL** | Chronos order code · bench SN | **HAVE** |
| **REQ-MTR-002** | Frozen coil = **FRC-350-100mV** = **100 mV/kA @ 50 Hz** | `TA_FL_ITA` · no 85 mV SKU | **HAVE** |
| **REQ-MTR-003** | Do **not** treat **85** as Rogowski class | Flyer **85–265 V L-N** (volts) | **HAVE** — support FAQ |
| **REQ-MTR-004** | Meter **.V** input = **333 mV** nominal, Vmax 500 mV, internal integrator | Web + flyer | **HAVE** |
| **REQ-MTR-005** | Write **40235** = **V/A** for coil (not factory 1.0; not raw mV in UI) | EMT430_MR `TA_Transducer_ratio` | **SETUP** |
| **REQ-MTR-005a** | **Bench deploy (2026-10):** **80 mV/kA** → **40235 = 0.00008** · yaml `rogowski_sensitivity_mv_per_ka: 80` | Chronos support + coil label | **SETUP** |
| **REQ-MTR-005b** | **Catalog FRC-350-100mV:** **100 mV/kA** → **40235 = 0.0001** | `TA_FL_ITA` | **HAVE** |
| **REQ-MTR-006** | 1P bench: **40232** single-phase + integrator bit13 **ON** | MR bits 1–2 / 13 | **SETUP** |
| **REQ-MTR-007** | ccli 1P map: **P1 932 · Q1 940 · V_L1_N 905** · scale 0.001 · `float_word_swap: true` | MR + libmodbus 0-based from 40001 | **HAVE** (V was 906 — first-fail) |
| **REQ-MTR-008** | CCI publishes only P/Q/V totals. Full PQ/harmonics stay on the meter | 61557-12 §4.8.1.1 | **HAVE** |

### Support answer — 85 mV or 100 mV?

**100 mV/kA.** Not an accuracy class. Not 85.

| Phrase someone says | What the datasheet actually is | Write this |
|---------------------|--------------------------------|------------|
| “85 mV class” | **85–265 V AC L-N** voltage **range** (flyer) | Ignore for the coil |
| “100 mV class” | FRC-350 order code **FRC-350-100mV** = **100 mV per kA @ 50 Hz** | **40235 = 0.0001 V/A** |
| “85 mV Rogowski” | **No Chronos SKU.** Catalog is **100** or **50** mV/kA (FRC-420 only) | Do not use 85 |
| “333 mV” | Meter **.V** input nominal (mV-CT / integrated Rogowski) | Hardware pin, not 40235 |
| “Class 0,2 / 0,2S” | **EN 62053-22** energy (and ±0,1 % rdg P) | Accuracy — not mV |
| UI **Rapporto Trasf. TA** | Same register **40235**. On Rogowski it is **V/A**, not turns and not mV | **0.0001** |

Conversion: `V/A = (mV/kA) / 1e6`.  
**80 mV/kA (bench pin)** → **0.00008**. **100 mV/kA (FRC-350 SKU)** → **0.0001**. 85 mV/kA (no Chronos SKU) → **0.000085**.

One-shot deploy (PC on LAN3 **.10**, meter **.102**, DUT **.1**):

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1
```

Factory default **40235 = 1.0** is a **5 A CT ratio**. On a **.V** meter that makes I ≈ 10 000× too small (bench: I ≈ 0,001 A, P ≈ 40 mW).

---

## Architecture

### Functional

```text
FRC-350-100mV  --333 mV-->  EMT432.V.FULL integrator  --P/Q/U-->  Modbus TCP
                                | 40235 = 0.00008 V/A (bench 80 mV/kA)
                                v
                         ccli last-sample (not Annex D)
                                v
                         PdCMMXU1 TotW / TotVAr / PPV
```

Meter **measures**. CCLI **forwards**. TesPro **does not measure**.

### Hardware (frozen)

| Block | Pin | Notes |
|-------|-----|-------|
| Analyzer | **EMT432.V.FULL** SN **261001085352** · HW **04** · FW **008** | `feature_version` @40015 = **1** · `mounted_version` @40011 = **1** |
| Current sensor | **FRC-350-100mV** · 100 mV/kA @ 50 Hz · linearity ±0,2 % | 61010-1:2010 + 61326-1:2013 on coil |
| Voltage | Meter L1/N terminals · **85–265 V L-N** | Structure **PMD SD** (internal V + external I) |
| Gateway | TesPro TG544 | No V/I — 61557 **N/A ALIGNED** |

### Network

| Role | IP | Yaml |
|------|----|------|
| Meter (this freeze) | **192.168.178.102:502** unit **1** | `lab_tr400_emt432_lan3_tcp_102.yaml` |
| DUT LAN3 | **192.168.178.1** | UCI |
| PC-A plant NIC | **192.168.178.10** | `lab/set_pc_lan3_plant_178.cmd` |

Do **not** put meter Ethernet on **.10** (first-fail IP clash with PC).

### Power

Meter aux **10–36 V DC**. Coil is passive. Never put mains on Pow±.

---

## Why the first adapt failed

Seven independent mistakes. Any one zeros or garbage-fills TotW/PPV.

| # | What we did first | Why it fails | Freeze |
|---|-------------------|--------------|--------|
| **1** | Factory **40235 = 1.0** (or typed **85** / **100** into Rapporto TA) | Rogowski field is **V/A**. 1.0 → I/P ~0. 85 or 100 → worse | **0.0001** |
| **2** | **40232** left **3P 4W + integrator OFF** (raw 518) | 1P wiring + di/dt coil without integrator | bits 1–2 = 0, bit13 = 1 |
| **3** | Mapped **P_SUM 938** + **V_L1_L2_peak 980** (3P family defaults) | 1P: L2 open → L-L peak ~0; PPV dies | **P1 932 · V_L1_N 905** |
| **4** | Voltage offset **906** (`40906 − 40000`) | libmodbus is **40001-based**. P1 used **932 = 40933−40001**. V must be **905 = 40906−40001** | **905** |
| **5** | `float_word_swap: false` | EMT432 TCP is low-word-first | **true** |
| **6** | Missing `power_scale: 0.001` | Meter speaks **W**; MMS wants **kW** | **0.001** |
| **7** | Meter IP **.10** = PC **.10** | No TCP :502 | Meter **.102** |

Probe still prints peak @974/980 for diagnosis. Runtime yaml must use **RMS 905**.

UI **Impostazioni Potenze** (screenshot 2026-10-21): Rapporto TA/TV, cutoffs, RMS/demand seconds, Vnom **230**, sag **0,9**, swell **1,2**. Those demand/PQ timers are **meter Annex D / Full PQ**. They are **not** O.8 4 s blocks and **not** the 85/100 question.

---

## Interface matrix

| Interface | Source | Destination | Protocol / register |
|-----------|--------|-------------|---------------------|
| I | FRC-350-100mV | EMT432 I1+/I1− | 333 mV · integrator ON |
| U | L1-N | EMT432 V terminals | 85–265 V |
| Sensitivity | Support / label | **40235** | float **0.0001 V/A** |
| Connection | UI / script | **40232** | 1P + bit13 |
| P | EMT432 P1 | ccli TotW | **932** × 0.001 |
| Q | EMT432 Q1 | ccli TotVAr | **940** × 0.001 |
| V | EMT432 V_L1_N RMS | ccli PPV | **905** × 0.001 |
| POC | ccli | DSO | 61850 5-LN |

---

## BOM matrix

| Block | Function | PN | Status |
|-------|----------|----|--------|
| PMD | Measure P/Q/U | **EMT432.V.FULL** | **FROZEN** |
| CS | Rogowski 100 mV/kA | **FRC-350-100mV** | **FROZEN** — not 85 mV |
| Map | 1P registers | `chronos_emt432_map.yaml` | **HAVE** |
| Bench yaml | LAN3 .102 | `lab_tr400_emt432_lan3_tcp_102.yaml` | **HAVE** |
| 61557-12 cert | PMD type-test | Chronos | **MISS** (62053-22 HAVE) |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| 85 mV coil | Not in Chronos catalog | Confirm bench coil **label** is 100 mV/kA | Photo **OPEN** if support still asks |
| 61557-12 | 62053-22 0,2S only | ACCREDIA or Chronos 61557 report | **MISS** |
| Annex D in ccli | Last-sample | Fig D.2 200 ms / 4 s | **MISS** (meter demand ≠ O.8) |

Corpus: PDFs **HAVE** · SKU **HAVE** · 85 mV SKU **NONE**.

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-MTR-001 | Support writes 85 or 100 into Rapporto TA | P/I wrong by 10³–10⁶ | Script `--set-sensitivity-mvka 100` |
| R-MTR-002 | Keep yaml V=906 | Scrambled PPV float | Freeze **905** |
| R-MTR-003 | Call 62053-22 a 61557-12 cert | False O.15 #2 | Keep #2 open |

---

## Verification

| Requirement | Test | Pass |
|-------------|------|------|
| REQ-MTR-001 | `40015=1` · `40011=1` · nameplate V.FULL | Full + Rogowski HW |
| REQ-MTR-005a | `emt432_rogowski_config.py --read` | 40235 ≈ **0.00008** (80 mV/kA bench) |
| REQ-MTR-005b | Same, if coil is FRC-350-100mV | 40235 ≈ **0.0001** (not 1, not 85, not 100 in UI) |
| REQ-MTR-006 | 40232 | 1P · integrator ON |
| REQ-MTR-007 | Probe P1 vs display; V_L1_N @905 ≈ 230 | Scale and offset OK |
| Load | Known kW through FRC | TotW tracks (not 40 mW idle) |

```powershell
lab\set_pc_lan3_plant_178.cmd
python lab\emt432_rogowski_config.py --host 192.168.178.102 --read
powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1
python lab\modbus_emt432_tcp_probe.py --host 192.168.178.102
# Catalog 100 mV/kA only: --set-sensitivity-mvka 100
```

---

## Related

| Doc | Role |
|-----|------|
| [CHRONOS_EMT432_VENDOR_REQUIREMENTS.md](CHRONOS_EMT432_VENDOR_REQUIREMENTS.md) | **All Chronos cert + normative asks** (master vendor list) |
| [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md) | MMS 5-LN pin (unchanged) |
| [P5_ROGOWSKI_FRC350_BENCH.md](../evidence/phase5/P5_ROGOWSKI_FRC350_BENCH.md) | Wiring |
| [CCI_Chronos_EMT432_Extract.md](../../knowledge-base/08-engineering/CCI_Chronos_EMT432_Extract.md) | Register extract |
| [61557-12_CLAUSES_VS_TESPRO.md](61557-12_CLAUSES_VS_TESPRO.md) | TesPro still not PMD |

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-21 | Freeze V.FULL + 100 mV/kA; pull datasheets; first-adapt fail list; V offset 905 |
| 1.1 | 2026-10-21 | Bench deploy **80 mV/kA** (40235=0.00008); `deploy-emt432-80mv.ps1`; yaml metadata |

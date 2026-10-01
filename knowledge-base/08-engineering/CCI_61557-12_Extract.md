# IEC / CEI EN 61557-12 — Engineering Extract (CCLI Metrology)

**Document ID:** CCLI-NORM-61557-12-001  
**Revision:** 1.1  
**Date:** 2026-09-21  
**RAG source_id:** `ccli-61557-12-extract`  
**Normative source:** IEC 61557-12:2018+AMD1:2021 CSV (CEI EN 61557-12:2018 + AMD1:2021)  
**Licensed PDF:** `07-protocols/regulations/standards-drop-20260918/IEC 61557-12 2018.pdf` (~143 MB, verified 2026-09-21)  
**Capture method:** Screenshot extract (Aug 2026) — upgrade via `ccli-61557-12-capture-plan` against licensed PDF  
**Corpus status:** **HAVE (P1)** — Annex D/H + Table 3/8 in extract; licensed PDF on disk for cert page cites  
**Programme link:** HiTEKS CCLI / PF2 · CEI 0-16 Annex O measurement timing and accuracy

---

## Summary

IEC 61557-12 defines **Power Metering and monitoring Devices (PMD)** and embedded **Power Metering Functions (PMF/EPMF)**. For the **TG-524 gateway CCI**, the applicable model is **EPMF SS** (Annex H): main function = protocols/gateway; measurement at POC via **external analyzer + CT + VT**. Annex O cites “61557-12 Annex B” for fixed blocks — in this edition **fixed-block aggregation is normative in Annex D** (Fig D.2), not Annex B (electrical parameter definitions).

**CCLI firmware must implement:** gapless P/Q sampling → **fixed-block** averages at **200 ms (MC200)**, **4 s (PF1 TX)**, and **ΔT (MCΔT)**; boundary-aligned 4 s blocks at `:00/:04/:08…`; **no sliding blocks** for Annex O PF1.

---

## Requirements

| ID | Requirement | 61557-12 basis | CCLI status |
|----|-------------|----------------|-------------|
| REQ-MET-001 | Active power P: gapless measurement required | §4.8.2.1, §3.4.2 | PART — spec only |
| REQ-MET-002 | Reactive power Q: gapless measurement required | §4.8.3.1, §3.4.2 | PART |
| REQ-MET-003 | Voltage U: gapless **not** required | §4.8.7.1 | PART |
| REQ-MET-004 | P/Q/U reporting uses **fixed block** intervals (not sliding) | Annex D, Fig D.2 | PART — `core/measurement/` TBD |
| REQ-MET-005 | Measuring converter class **≤ 0,2** for active P at POC | Table 3, Table 8; Annex O O.13.2.1 | PART — analyzer selection |
| REQ-MET-006 | CT class **≤ 0,5**, VT class **≤ 0,5** | Annex O O.13.2.1; Annex F | PART |
| REQ-MET-007 | MC200 = 200 ms fixed blocks for fast control ring | Annex D + Annex O O.7.3.1 | PART |
| REQ-MET-008 | PF1 P/Q/V TX = 4 s fixed blocks, aligned :00/:04/:08… | Annex D + Annex O O.8 | PART |
| REQ-MET-009 | MCΔT = measurements aggregated over slow-ring ΔT (10–600 s, default 60 s) | Annex D + Annex O O.7.3.2 | PART |
| REQ-MET-010 | TG-524 classified as **EPMF**; metering chain **EPMF SS** | Annex H, Table H.3, Fig H.1 | PART |
| REQ-MET-011 | Rated voltage band **80% Un < U < 120% Un** for P and Q | §4.8.2.2, §4.8.3.2 | PART |
| REQ-MET-012 | Voltage measurement range **20% Un … 120% Un** (PMD xS) | Table 26 | PART |
| REQ-MET-013 | Reference conditions K55: **−5 … +55 °C** | Tables 5–6 | HAVE (captured) |
| REQ-MET-014 | Fiscal CT/VT reuse excluded for CCI measurements | Annex O O.13.2.1 | HAVE (Annex O extract) |

---

## Architecture

### Functional

- **TG-524 (OpenWrt gateway):** EPMF — primary role = IEC 61850 / 60870-5-104 / Modbus / plant orchestration (Annex H).
- **Field analyzer at POC:** PMD Sx or SS front-end — performs gapless P/Q sampling; may expose registers via Modbus to gateway.
- **Gateway PMF logic:** Poll or subscribe to instantaneous P/Q/U; apply **Annex D fixed-block** aggregation; timestamp and quality-index each block for DSO exchange (Annex O O.8.3).
- **Control rings (Annex O):** Fast ring uses **MC200** blocks; slow ring uses **MCΔT** blocks over configurable ΔT; external set-point commands min 3 s apart.

### Hardware (measurement chain)

| Block | Role | 61557-12 model |
|-------|------|----------------|
| TG-524 | Comms + aggregation + control FSM | **EPMF** (Annex H) |
| Energy analyzer | Gapless P/Q/U acquisition | **PMD Sx** or part of **EPMF SS** |
| CT (×3) | Phase current | External sensor per **Annex F** |
| VT (×3) | Phase voltage | External sensor per **Annex F** |

**Figure 2 (PMD configurations):** CCLI path = **SS** (separate voltage and current sensors) or **DD** (dedicated external sensors) — treat PMD + dedicated CT/VT as **PMD DD** per Table 3 NOTE (separate type-test report per sensor combo).

### Network / data exchange

- PF1 quantities (P, Q, V at POC) transmitted every **4 s** on boundaries `:00/:04/:08…` with timestamp + quality index.
- 4 s values = **fixed-block averages** over preceding 4 s (Annex D), not sliding windows.
- Instantaneous P/Q for MSD (PF3) = real-time value at boundary, not block average (Annex O O.8.5) — out of initial PF2 scope.

### Security

- Metrology extract does not define cyber controls; IEC 62351 / Annex T govern exchange security.
- Measurement integrity: quality flags on invalid samples (phase missing, out-of-range U) per PMD behaviour (Table 9 footnote f).

### Power

- Type tests at min/max auxiliary supply (Table 13 footnote f — relevant for 12 V CCI PSU).

---

## Scope and definitions (extract)

**§1 Scope:** Applies to PMD and equipment embedding PMF/EPMF. Excludes tariff/fiscal metering (IEC 62052/62053 series). PMF may be embedded in UPS, STS, breaker, PLC, inverter, or **industrial communications equipment** (Annex H Table H.1 context).

**§3.1.17 EPMF:** Equipment whose **main function is not metering** but contains a PMF.

**§3.4 Measurement techniques:**
- **§3.4.1 Measurement window** — time over which a quantity is determined.
- **§3.4.2 Gapless measurement** — continuous sampling without gaps; **required for P and Q**, not for U.

**§4.5 Table 3 — performance classes (allowed values):**

| Quantity | Allowed classes | Annex O target |
|----------|-----------------|----------------|
| Active power P | 0,1 · 0,2 · 0,5 · 1 · 2 · 2,5 | **≤ 0,2** |
| Reactive power Q | 1 · 2 · 3 | typically **2** (check Tables 12–15) |
| Voltage U | 0,05 · 0,1 · 0,2 · 0,5 · 1 · 2 | VT path **≤ 0,5** |
| Phase current I | 0,05 … 2 | CT **≤ 0,5** |

---

## Active power P (§4.8.2)

| Clause | Requirement |
|--------|-------------|
| 4.8.2.1 | Techniques per Annex B; **gapless required** |
| 4.8.2.2 | **80% Un < U < 120% Un** |
| 4.8.2.3 | Limits in **Table 8** |

**Table 8 — intrinsic uncertainty (class C = 0,2, normal load, PF = 1, I ≥ 10% Ib):** limit ≈ **±1,0 × C = ±0,2%** of measured P.

**Table 9 — influence quantities (P, class C = 0,2, PMD Sx column):**

| Influence | Limit at C = 0,2 |
|-----------|------------------|
| Ambient temperature | 0,05 × C per K |
| Voltage 80–120% Un | 0,3×C + 0,04 ≈ **±0,10%** |
| Frequency ±2% | 0,3×C + 0,04 ≈ **±0,10%** |
| 5th harmonic (10% V / 40% I) | ≈ **±0,38%** |
| Phase missing (3-phase 4-wire) | **±0,40%** |
| Odd / sub-harmonics | 3,0 × C = **±0,6%** |
| RF / EMC conducted & radiated | 3,4×C + 0,3 ≈ **±0,98%** |

**Table 11 — starting current (P, PMD Sx):** at C = 0,2 → **1×10⁻³ × In**.

**Table 10:** minimum type-test period Δt (certification only, not runtime ΔT).

---

## Reactive power Q (§4.8.3)

| Clause | Requirement |
|--------|-------------|
| 4.8.3.1 | Annex B; **gapless required** |
| 4.8.3.2 | **80% Un < U < 120% Un** |
| Table 12 | Reactive classes **1, 2, 3** |

**Table 12 — intrinsic (class C = 2, sin φ = 1):** **±1,0 × C = ±2%** Q.

**Table 13 — influence (C = 2, Sx):**

| Influence | Limit |
|-----------|-------|
| Temperature | 0,10 %/K |
| Voltage 80–120% Un | **±1,0%** |
| Frequency ±2% | **±2,5%** |
| RF / magnetic EMC | **±3,0%** |

**Table 15 — starting current (Sx, C = 2):** **(C+1)×10⁻³ × In = 3×10⁻³ × In**.

**PF2 link:** TsQ ≤ 10 s and Q = f(V) depend on Q accuracy under these influence quantities — stricter than P.

---

## Voltage U (§4.8.7)

| Clause | Requirement |
|--------|-------------|
| 4.8.7.1 | Annex B; **gapless NOT required** |
| Table 26 | PMD xS range **20% Un … 120% Un**; bandwidth 45 Hz – 15×fn |
| Table 27 | Uncertainty **±1,0 × C** → at **C = 0,5: ±0,5%** |

**Table 28 — influence (C = 0,5):**

| Influence | Limit |
|-----------|-------|
| Ambient temperature | 0,05 × C = **0,025 %/K** |
| Aux supply ±15% | 0,1 × C = **±0,05%** |

**PF2 link:** Q = f(V) uses **ΔT-averaged voltage** — analyzer must stay within Table 26 range.

---

## Annex D — fixed and sliding block intervals

**Correction vs CEI Annex O text:** Annex O references “IEC 61557-12 Annex B” for fixed blocks. In **61557-12:2018+AMD1:2021**, normative fixed-block rules are in **Annex D** (Fig D.2). Annex B = electrical parameter definitions (p. 79+).

### Fixed block interval (Fig D.2) — **use for all CCLI aggregation**

- Intervals are **consecutive and non-overlapping**.
- Reported value = **arithmetical average** over the interval (for power: energy ÷ time).
- Value updated **at the end** of each interval.
- Diagram shows 15 min as **example only** — any duration is valid if fixed-block rules are met.

### Sliding block interval (Fig D.3) — **do not use for Annex O PF1**

- Overlapping windows; not aligned to CEI 4 s boundary requirement.

### Map to Annex O symbols

| Annex O symbol | Interval | Implementation |
|----------------|----------|----------------|
| **MC200** | **200 ms** | Fixed block, 5 blocks/s — fast control ring input |
| **PF1 P/Q/V TX** | **4 s** | Fixed block, aligned **:00/:04/:08/:12…** |
| **MCΔT** | **ΔT** (10–600 s, default 60 s) | Fixed block average over ΔT — slow control ring |
| Sliding block | — | **Excluded** for PF1 |

### Gapless vs block (§3.4.2 vs §4.8.7)

- **P, Q:** gapless sampling internally → aggregate into fixed blocks for reporting.
- **U:** gapless not required → still use **fixed blocks** for 4 s DSO reporting.

---

## Annex F — external CT/VT sensors

**Table F.1 — PMD + external sensors → overall uncertainty (active P):**

| PMD (analyzer) class | Min sensor class | Typical overall P |
|----------------------|------------------|-------------------|
| **0,2** | **0,2 or better** | **0,5%** |
| 0,5 | 0,5 or better | 1% |

**Annex O chain:** analyzer **≤ 0,2**, CT/VT **≤ 0,5** → expected overall **≈ 0,5%** on P (Table F.1 row 1 with 0,5 sensors).

Footnote **b:** use **0,2S / 0,5S** sensor classes for energy measurements.  
Footnote **c:** CT/VT per **IEC 61869** / legacy **IEC 60044**.

**Table F.2 — sensor contribution:**

| Quantity | Current sensor | Voltage sensor |
|----------|----------------|----------------|
| **P, Q** | ✓ | ✓ |
| **U** | — | ✓ |
| **I** | ✓ | — |
| **f** | — | — |

Both CT and VT required for P/Q at POC — matches **EPMF SS**.

---

## Annex H — EPMF / PMF (gateway role)

| Item | Standard | CCLI mapping |
|------|----------|--------------|
| **EPMF definition** | Main function ≠ metering; embeds PMF | **TG-524** = protocol gateway + aggregation |
| **Fig H.1** | MAIN FUNCTION (comms/protocols) + PMF (measure chain) | `apps/ccli/`: adapters = main function; `core/measurement/` = PMF |
| **Table H.3** | **EPMF SS** = external V + external I sensors | Analyzer + CT/VT at POC; gateway polls/forwards |
| **Table H.5** | Declare PMF class, EPMF type (SD/DD/SS/DS), temp class K55 | Product datasheet / cert form |
| **Core clauses** | Read “PMD” as EPMF/PMF with Annex H modifications | Apply Tables 8–28 + Annex D to measurement path |

**Table H.1** lists equipment types that may embed EPMF (UPS, STS, PLC, inverter…). TG-524 is an **industrial comms gateway** — classify as EPMF per §3.1.17 + H.1 scope.

---

## Annex O traceability matrix

| Annex O requirement | 61557-12 basis | This extract |
|---------------------|----------------|--------------|
| MC200 — 200 ms blocks (fast ring) | Annex **D** Fig D.2 | ✓ p. 89–90 |
| 4 s P/Q/V TX, :00/:04/:08… | Annex **D** fixed block | ✓ |
| MCΔT slow-ring aggregation | Annex **D** fixed block over ΔT | ✓ |
| Converter class ≤ 0,2 | Table 3, Table 8 | ✓ p. 27, 31–32 |
| CT/VT class ≤ 0,5 | Annex F + O.13.2.1 | ✓ p. 93–94 |
| Fixed block method (Annex O cites “Annex B”) | **Annex D** in 2018+AMD1:2021 | **Correction documented** |
| Tables 5 & 6 (Grid Code ref) | K55 temp −5…+55 °C | ✓ p. 28–29 |

---

## Interface matrix

| Interface | Source | Destination | Protocol / method |
|-----------|--------|-------------|-------------------|
| POC P/Q/U samples | Analyzer (PMD Sx) | TG-524 PMF | Modbus RTU/TCP (TBD register map) |
| CT secondary | Field CT 0,5 | Analyzer current inputs | Hardwired |
| VT secondary | Field VT 0,5 | Analyzer voltage inputs | Hardwired |
| MC200 blocks | PMF aggregator | Fast control FSM | Internal API |
| 4 s P/Q/V + QI | PMF aggregator | IEC 61850 / 60870-5-104 | Annex T data model |
| MCΔT blocks | PMF aggregator | Slow control FSM (PF2) | Internal API |

---

## BOM matrix

| Block | Function | Candidate / class | Status |
|-------|----------|-------------------|--------|
| TG-524 gateway | EPMF — protocols + aggregation | TesPro TG-524 / MT798X | FROZEN |
| Energy analyzer | Gapless P/Q/U, class 0,2 P | **TBD model** — K4.5 register map missing | MISS |
| CT set | Phase current, class ≤ 0,5 | IEC 61869-2, 0,5S preferred | PART — architecture only |
| VT set | Phase voltage, class ≤ 0,5 | IEC 61869-3, 0,5 preferred | PART |
| RS485 link | Analyzer ↔ gateway | Existing TG-524 RS485-1 | HAVE (hardware) |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| Licensed PDF | Screenshot extract only | CEI EN 61557-12:2018 full PDF | **MISSING** `cei-en-61557-12-2018` |
| Analyzer model | None chosen | Register map + cert class 0,2 | K4.5 |
| Firmware aggregator | Not implemented | `core/measurement/` fixed-block pipeline | REQ-MET-004 |
| Gapless type test | Not captured (p. 75) | §6.2.18 test procedure for cert | P1 optional |
| Current I tables | Not captured (p. 43–46) | Full PMD cert if needed | P2 |
| Annex B math | Not captured (p. 79–85) | P/Q computation cross-check | P2 |
| Table 1 / H.2 | Empty cells in preview | Complete PMD-I/II/III matrix | Low — use §4.8 + Annex H |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-MET-001 | Annex O cites wrong annex letter (B vs D) | Mis-implemented aggregation timing | Use **Annex D**; document in ICD/verification plan |
| R-MET-002 | Gateway-only metering without analyzer | Cannot meet gapless P/Q + class 0,2 | **EPMF SS** — external analyzer mandatory |
| R-MET-003 | Sliding block or non-aligned 4 s TX | DSO rejection / conformance fail | Fixed block + NTP/GNSS boundary sync |
| R-MET-004 | CT/VT shared with protection without isolation | Accuracy / safety interference | Dedicated cores/windings per Annex O O.13.2.1 |
| R-MET-005 | Screenshot extract incomplete for audit | Cert evidence gap | Procure licensed PDF before type test |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-MET-004 | Inject synthetic P/Q samples; aggregate 4 s blocks | Block average matches manual calc; boundaries on :00/:04/:08 |
| REQ-MET-007 | 200 ms block output during fast-ring test | 5 blocks/s; non-overlapping |
| REQ-MET-008 | Timestamp PF1 TX records | All TX at 4 s boundaries ±10 ms (GNSS synced) |
| REQ-MET-005 | Analyzer datasheet / cert | Declared class **≤ 0,2** for active P |
| REQ-MET-006 | CT/VT datasheets | Class **≤ 0,5**; 0,2S/0,5S for energy path |
| REQ-MET-003 | U block from non-gapless samples | Valid 4 s U per Table 26 range |
| REQ-MET-010 | Architecture review | TG-524 documented as EPMF SS in system ICD |

---

## Captured pages (reference)

| Pages | Content |
|-------|---------|
| 10–13 | Foreword, scope, normative refs |
| 16–29 | Definitions, Fig 1–2, Tables 3–6 |
| 27 | Table 3 performance classes |
| 31–36 | Active P, Tables 8–10, Table 9 |
| 37–40 | Reactive Q, Tables 11–15 |
| 47–48 | Voltage U, Tables 26–28 |
| 89–91 | Annex D fixed/sliding blocks; Annex E (informative) |
| 93–94 | Annex F external CT/VT |
| 100–105 | Annex H EPMF |
| 41, 49 | Apparent S, PF — optional |

Screenshot assets: Cursor workspace `assets/` (Aug 2026 capture session). Optional archive path: `knowledge-base/08-engineering/reference/61557-12/` (not copied in rev 1.0).

---

## Related RAG sources

| source_id | Role |
|-----------|------|
| `cei-0-16-allegato-o` | MC200, 4 s TX, MCΔT, accuracy classes |
| `ccli-cci-module-regs-class` | METROLOGY routing |
| `ccli-app-structure` | Target `core/measurement/` module |
| `cei-en-61557-12-2018` | **Target** full norm PDF (not yet ingested) |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-08-11 | Initial extract from screenshot corpus; RAG ingest `ccli-61557-12-extract` |

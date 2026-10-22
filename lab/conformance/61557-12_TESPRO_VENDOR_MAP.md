# 61557-12 — TesPro vendor certificate map (only)

**Document ID:** CCLI-LAB-61557-12-TESPRO-MAP-001  
**Revision:** 1.4  
**Date:** 2026-10-21  
**RAG source_id:** `ccli-61557-12-tespro-vendor-map`  
**Scope:** TesPro TG544 / TG500 **only**. Chronos, CT/VT, `ccli` Annex D, ACCREDIA #2 report = **out**.  
**Parents:** `ccli-tespro-cert-reply-2026-10` · `ccli-61557-12-extract` · `ccli-61557-12-2018-scan` · `ccli-61557-12-2018-ed2-extract`  
**Ask:** `TESPRO_VENDOR_ASK_CERT(Reply).xlsx` sheet **Hiteks** · 2026-10-21  
**Edition pin:** Desktop PDF = **2018 Ed 2.0 only** (no EPMF). Programme / TesPro EPMF split needs **2018+AMD1:2021**. O.15 #2 also **61010-1 / 2-201** + **17025** + **K55**.

TesPro object = comms gateway. **2018-only:** they are **outside §1** unless they claim **PMD** (then **§4.10 → 61010-1 + 61010-2-030** is mandatory). **AMD1:** they can stay **EPMF SS** (Annex H). Do not add 61557-12 on Ed 2.0 alone.

---

## All we HAVE (TesPro pack, 2026-10-21)

| Artefact | Where | 61557-12 use |
|----------|-------|--------------|
| Ask reply sheet **Hiteks** (9 rows) | `TESPRO_VENDOR_ASK_CERT(Reply).xlsx` | Index of asks |
| Scope Allocation v1.0 (2 pp., unsigned) | `TesPro-Certification-Scope-Allocation-Statement-EN.pdf` | **REQ-MET-010 ALIGNED** — EPMF; 61557 N/A; 61010 N/A |
| CNAS **L5868** card | `01 CNAS.pdf` + OCR | 17025 China lab identity |
| CNAS unfolding **p.1/83** | `CCI_TesPro_CNAS_L5868_62368_Scope_Extract.md` | ICT **62368-1** methods **Effectivity** (IEC/EN/GB/UL…). **No 61557** |
| A2LA **4674.01** cover | `03 A2LA.pdf` | Same lab, Electrical, to 2027-12-31 |
| A2LA **4674.01** scope rev 2026-09-22 | `4674-01.pdf` (7 pp.) | **62368-1**, **61326-1**, **61000-6-x**, CISPR 32, RED. **No 61557 / 61010 / K55** |
| FIPS 140-2 **L2** policy Rel 1.6 | SLB9672/9673 | **#3 only** — out of 61557-12 |
| CC **BSI-DSZ-CC-1278-2026** EAL4+ | `CC certificate.pdf` | Chip CC — out of 61557-12 |
| Hall TI **DRV5032** | datasheet | O.13.7 — out |
| TG500 zip (11 DOCX v0.1) | image / 61850 / IDevID / Secure Boot | out |
| IEC TS 62351-100-4 | already in `iec62351/` | out |
| ISO 9001 PDF | claimed | **NOT in drop** |
| TG544 CE / 62368 / 61326 / K55 **reports** | — | **NOT in drop** |

---

## Requirements

### 1. Vendor certificates we asked TesPro for

| Ask # | Certificate / deliverable we required | 61557-12 align? | TesPro reply | Align verdict |
|-------|----------------------------------------|-----------------|--------------|---------------|
| **6** | Written scope: does TesPro certify the **platform** as measuring/CCI HW (61557-12 + 61010), or only HiTEKS final apparatus? | **YES — this is the only 61557-12 ask** | `TesPro-Certification-Scope-Allocation-Statement-EN.pdf` v1.0 2026-10-21 | **ALIGNED as not PMD only.** They wrote: comms-only, **no measuring inputs**, **61557 series does not apply**, **61010 series does not apply**. They never wrote **EPMF / Annex H / SS**. That label is **AMD1 HiTEKS mapping**, not TesPro evidence |
| **2** | CE + DoC + EMC/LVD + applied-norms list for **TG544 / TR500** | **Partial** — O.15 #2 recalls 61010 + EMC influence; TesPro remaps platform to **62368-1 + 61326-1 / 61000-6-x** | CNAS **L5868** card + unfolding **p.1/83** (ICT **62368-1** Effectivity) + A2LA **4674.01** | **NOT aligned to 61557-12.** Lab **can** run 62368-1 (CNAS p.1) and 61326/61000 (A2LA). **No 61557 / 61010 / K55**. Menu ≠ TG544 report |
| 1 | FIPS 140-2 **L3** module (SLB9673) | **No** — O.15 #3 | FIPS **L2** policy + CC **BSI-DSZ-CC-1278-2026** | Out of this map |
| 3 | ISO 9001 manufacturing | **No** — O.15 #7 (17025 is the #2 lab QMS) | “Attached” — **PDF not in drop** | Out of this map |
| 4 | Secure Boot proof | **No** | TG500 zip v0.1 | Out of this map |
| 5 | Anti-tamper Hall / GPIO | **No** — O.13.7 | TI **DRV5032** datasheet | Out of this map |
| 7 | OpenWrt 25.12 image identity | **No** | Example `tesproos-32-2.1.0-r530…` | Out of this map |
| 8 | IEC 61850 coexistence | **No** | :3782 yield policy v0.1 | Out of this map |
| 9 | IDevID in TPM | **No** | EK only; IDevID not factory | Out of this map |

### 2. 61557-12 clauses vs TesPro only

| Clause | Shall (61557-12 / O.15 #2) | TesPro must certify? | TesPro HAVE | Gap (TesPro) |
|--------|----------------------------|----------------------|-------------|--------------|
| **REQ-MET-010 / Annex H** | Declare TG544 = **EPMF SS** (main function ≠ metering; external V/I) | **YES — paper, AMD1 only** | Scope §3: **not PMD** (“61557 does not apply”). **0 hits** for EPMF / Annex H / SS in their PDF | **NOT aligned as EPMF.** HiTEKS may *map* comms-only → AMD1 EPMF SS. Table **H.5** is HiTEKS. Need **2018+AMD1:2021** before that word is normative |
| **REQ-MET-013 / Tables 5–6** | Climate **K55** (−5…+55 °C) on the **equipment** | **YES if** TesPro’s platform cert includes environmental performance (scope §2 lists “environmental performance” on the **platform**) | **NONE** — no K55 / IEC 60068 report. 4674.01 has **no** climate methods | **GAP** — TesPro TG544 K55 (or equivalent) **report ID** |
| **61010-1 / 2-201** | Insulation / safety of **measuring** equipment (recalled by 61557-12) | **NO** on current SKU | Scope §3: 61010 **only if** measuring inputs or DO driving loads. TG544 = no measuring inputs | **ALIGNED N/A.** Do not ask TesPro for 61010 unless HiTEKS adds V/I/CT on the box |
| **ISO/IEC 17025** | Accredited lab for the **#2 type-test** | TesPro’s **platform** EMC/safety lab ≠ O.15 #2 lab | CNAS L5868 · A2LA 4674.01 · ISO/IEC 17025:2017 · valid to 2027-12-31 · **Electrical** only | **NOT aligned** for 61557-12. Useful only if TesPro later books **62368/61326** tests of **TG544** at Microtest. #2 still needs a lab with **61557-12 on scope** (ACCREDIA) on the **assembled CCI** — HiTEKS |
| **REQ-MET-001 / §4.8.2.1** | P gapless | **NO** — PMD | — | **N/A TesPro** |
| **REQ-MET-002 / §4.8.3.1** | Q gapless | **NO** | — | **N/A TesPro** |
| **REQ-MET-003 / §4.8.7.1** | U gapless not required | **NO** | — | **N/A TesPro** |
| **REQ-MET-004 / Annex D** | Fixed blocks | **NO** — PMF in `ccli` | — | **N/A TesPro** |
| **REQ-MET-005 / Table 3+8** | P class ≤ 0,2 | **NO** — analyzer | — | **N/A TesPro** |
| **REQ-MET-006 / Annex F** | CT/VT ≤ 0,5 | **NO** | — | **N/A TesPro** |
| **REQ-MET-007** | MC200 200 ms | **NO** | — | **N/A TesPro** |
| **REQ-MET-008** | 4 s `:00/:04` | **NO** | — | **N/A TesPro** |
| **REQ-MET-009** | MCΔT | **NO** | — | **N/A TesPro** |
| **REQ-MET-011 / §4.8.2.2** | P/Q 80–120 % Un | **NO** | — | **N/A TesPro** |
| **REQ-MET-012 / Table 26** | U 20–120 % Un | **NO** | — | **N/A TesPro** |
| **REQ-MET-014** | No fiscal CT/VT reuse | **NO** — plant design | — | **N/A TesPro** |
| **§6 PMD type tests** | Tables 8–28 influence, starting current | **NO** | 4674.01 EMC is **ITE**, not PMD Table 9 | **N/A TesPro** |
| **CEI EN 61439-2** | Cabinet (optional) | **NO** unless TesPro sells the cabinet | — | **N/A** |

### 3. If TesPro *adds* 61557-12 — clauses they must cover

Desktop PDF is **2018 Ed 2.0**. **EPMF is not in that file.**

| Path | Edition | TesPro covers | Do not use |
|------|---------|---------------|------------|
| **B (correct)** | **2018+AMD1:2021** | §3.1.17 · Annex **H** H.3 **SS** · Table **H.5** · Tables **5–6 K55** | §4.8 / §6 / Annex F (HiTEKS PMD) |
| **A (this PDF)** | 2018 Ed 2.0 only | **Whole PMD:** §1 · §4.6 Tables 5–7 · §4.8 if they measure · **§4.9 IP** · **§4.10 61010-1 + 61010-2-030** · §5 · §6.2 incl. **6.2.18 gapless** + **6.2.19 safety** · Annex D if demand | Contradicts their “61010 N/A” split |

Tell TesPro: add **AMD1:2021** (path B). Do not certify the gateway against Ed 2.0 alone.

Full OCR: `CCI_61557-12_2018_Scan.md`. Clause extract: `CCI_61557-12_2018_Ed2_Extract.md`. Clause vs reply: `61557-12_CLAUSES_VS_TESPRO.md`.

### 4. Gaps — CEI Annex O + 61557-12 annexes vs TesPro

TesPro letter used on every row: Scope §3 row 1 — *comms-only, no measuring inputs, 61557 N/A, 61010 N/A*. Unsigned.

#### 4.1 CEI Annex O clauses that call 61557-12 (O.15 #2 chain)

| Annex O | Shall | Who | TesPro respond | Gap |
|---------|-------|-----|----------------|-----|
| **O.15 / §12 #2** | **CEI EN 61557-12** + **61010-1** and notably **61010-2-201**; temp class **≥ K55**; lab **ISO/IEC 17025** (Italy: **ACCREDIA**) | **Finished CCI** = HiTEKS. TesPro = platform baseline only | Refused 61557 and 61010 on the box. Sent **CNAS/A2LA menus** (not ACCREDIA, not 61557 methods). **No K55 report**. **No TG544 CE/DoC** | **VENDOR GAP:** K55/60068 report ID · TG544 **62368-1 + 61326-1** reports (their named path) · **signed** Scope. **NOT TesPro:** ACCREDIA 61557 type-test, 61010-2-201 on the **chain** |
| **O.7.4 / O.7.3.1** | **MC200** = 200 ms **fixed block** “per IEC 61557-12 **Annex B**” | `ccli` + analyzer | No measurement on TG544 | **N/A TesPro.** **HiTEKS GAP:** SW is last-sample, not certified D.2 blocks. O cites **Annex B**; 2018 Ed 2.0 puts blocks in **Annex D Fig D.2** |
| **O.7.3.2** | **MCΔT** over ΔT, same 61557-12 Annex B; V/I **synchronous** | `ccli` + analyzer | Refused | **N/A TesPro.** HiTEKS curves waived / PART |
| **O.8.3 / O.8.4** | P/Q/V TX every **4 s**, `:00/:04/…`, 61557-12 fixed block + timestamp + QI | `ccli` URCB | Refused | **N/A TesPro.** HiTEKS alignment **PART** |
| **O.13.2.1** | Converter class **≤ 0,2**; CT/VT **≤ 0,5**; **not** fiscal CT/VT; 61557-12 fixed-block method | Analyzer + sensors | No V/I/CT on SKU. §3 row 2: if those inputs are added → **61010-1 + 2-030**, 61557 **still N/A** | **N/A TesPro.** Meter freeze **EMT432.V.FULL** + **FRC-350-100mV** (not 85 mV). EMT432 is **62053-22**, not 61557-12; Annex F **PART** |
| **O.13.2 Grid Code T5/T6** | Accuracy vs TSO Tables 5 & 6 | Chain | — | **N/A TesPro** |
| **O.15 #2 EMC** | 61557-12 influence (PMD Tables 9/13/17) via #2 lab | PMD/EPMF object | They named **61326-1 Class A / 61000-6-x** as **ITE**, not PMD Table 9. A2LA 4674.01 **can** run those methods. **No TG544 report** | **VENDOR GAP:** ITE EMC report on **this SKU**. PMD Table 9 **N/A TesPro** |
| **O.15 #2 insulation** | 61010-1 + **2-201** (O text) | Measuring equipment / CCI | **61010 series does not apply** on comms-only. Their alt if I/O added = **61010-2-030** (not 2-201) | **N/A TesPro** on this SKU. **EDITION GAP:** O says **2-201**; 61557-12:2018 says **2-030**. HiTEKS #2 lab must state which object |
| **O.15 #8** | CE + DoC DPR 445/2000 | Finished product | Ask #2 → lab cards only | **VENDOR GAP:** TG544 CE/DoC. Final DoC still **HiTEKS** |

#### 4.2 IEC 61557-12:2018 annexes vs TesPro

| 61557-12 annex | Type / shall | TesPro respond | Gap |
|----------------|--------------|----------------|-----|
| **A** Applications | Informative | Silent | None |
| **B** Electrical parameter definitions | Informative. **Not** demand blocks. O.7.4 cites this wrongly | Silent | **Citation GAP (HiTEKS/O text):** use **Annex D**. Not a TesPro deliverable |
| **C** PF sign | Informative | Silent | None |
| **D** **Normative** demand · **Fig D.2 fixed** / **D.3 sliding** | O.7.4 / O.8 need **D.2** | Silent / not PMD | **N/A TesPro.** **HiTEKS GAP:** implement D.2 in `core/measurement/` |
| **E** Operating uncertainty formula | Informative | Silent | **HiTEKS** chain calc |
| **F** Sensor classes (61869 · 0,2S/0,5S) · Table F.1 overall P | Informative; O.13.2 CT/VT | No sensors | **N/A TesPro.** **HiTEKS GAP:** CT/VT class reports |
| **G** Type-A uncertainty (intended normative next ed.) | Informative | Silent | **HiTEKS** #2 lab |
| **H / §3.1.17 EPMF** | **Not in Ed 2.0.** AMD1 only: main function ≠ metering + PMF; Table **H.3 SS**; Table **H.5** | **0 hits.** They refused **entire 61557 series** | **VENDOR: not declared.** Cannot file Ask #6 as H.5. **HiTEKS GAP:** AMD1 PDF + datasheet H.5 (SS + K55 + PMF class) if we keep the EPMF story |

#### 4.3 Vendor open list (TesPro only — ask these)

| # | Ask TesPro | Annex hook | Status |
|---|------------|------------|--------|
| V1 | **Sign** Scope §4 (TesPro + HiTEKS) | O.15 #2 scope split | **OPEN** unsigned |
| V2 | **K55** / IEC **60068-2-1/2/30** report ID on TG544 (−5…+55 °C rated) | O.15 #2 ≥K55 · 61557 Table 6 / §6.2.15 | **MISS** |
| V3 | TG544 **62368-1** + **61326-1** (or 61000-6-x) **test report numbers** + norms list | O.15 #2 EMC/safety as **ITE** (their path) · O.15 #8 input | **MISS** (menu ≠ report) |
| V4 | Written: **will not** add **61557-12:2018 Ed 2.0** to the CB. If the family is added, cite **2018+AMD1:2021** and stay off PMD | Annex H exists only in AMD1. Ed 2.0 would force §4.10 **61010-2-030** and **CONFLICT** their letter | **MISS** |
| V5 | Confirm **no** V/I/CT/temperature measuring inputs and **no** DO driving loads on this SKU | O.13.2 · Scope §3 row 2 | **HAVE** in text; freeze to HW photo/BOM |

**Do not ask TesPro for:** Annex B/D blocks, Annex F sensors, Table 3/8/12/26, §4.8 gapless, §6.2.18, 61010-2-201, ACCREDIA 61557, Table H.5, EPMF name (unless they choose Path B).

---

## Architecture

**What TesPro actually aligned:** TG544 is a communication / data-forwarding gateway (SELV, Ethernet / cellular / DIO, **no measuring inputs**). Applicable TesPro standards = **IEC/EN 62368-1** + **EN 61326-1** Class A or **EN 61000-6-x**. IEC **61557** series and IEC **61010** series **do not apply** to TesPro. That is **not PMD**. It is **not** an EPMF / Annex H declaration.

**Not aligned (do not file as O.15 #2):** CNAS/A2LA cards, FIPS L2, CC 1278, Hall datasheet, TG500 zip, ISO 9001 claim.

### Hardware (TesPro box only)

| Block | 61557-12 model | TesPro cert object |
|-------|----------------|--------------------|
| TG544 / TG500 | **EPMF** Annex H | Platform baseline: structure, insulation **as ITE (62368)**, EMC, env |
| Measuring inputs | None on SKU | 61010 / 61557 **off** |
| Shenzhen Microtest | 17025 Electrical | Can run TesPro **62368/61326** — cannot run 61557-12 |

---

## Interface matrix

| Interface | Source | Destination | Protocol / artefact |
|-----------|--------|-------------|---------------------|
| Ask #6 | HiTEKS | TesPro | Scope Allocation PDF |
| Ask #2 | HiTEKS | TesPro | CE DoC + EMC/LVD — **not delivered** |
| Platform safety/EMC | TesPro | HiTEKS technical file | 62368-1 + 61326-1 **reports TBD** |
| 61557-12 #2 | HiTEKS | ACCREDIA | Assembled EPMF SS — **not TesPro** |

---

## BOM matrix

| Block | Function | TesPro PN / file | 61557-12 status |
|-------|----------|------------------|-----------------|
| Scope statement | Not-PMD split (not EPMF) | `TesPro-Certification-Scope-Allocation-Statement-EN.pdf` | **HAVE — not PMD; Annex H MISS** |
| A2LA 4674.01 | Lab scope | `4674-01.pdf` + `03 A2LA.pdf` | **HAVE — not 61557** |
| CNAS L5868 | Lab QMS | `01 CNAS.pdf` | **HAVE — not 61557** |
| CNAS unfolding p.1 | ICT 62368-1 methods | `CCI_TesPro_CNAS_L5868_62368_Scope_Extract.md` | **HAVE — 62368 only**; 82 pages not ingested |
| TG544 CE/DoC | Ask #2 | — | **GAP TesPro** (enclosure #8 / 62368/61326) |
| K55 report | REQ-MET-013 | — | **GAP TesPro** |
| 61557-12 certificate | PMD / EPMF SS type-test | TesPro will not issue | **N/A TesPro** |

---

## Knowledge gaps (TesPro ∩ 61557-12 only)

| Area | Current | Required from TesPro | Gap |
|------|---------|----------------------|-----|
| Annex H split | Unsigned PDF 2026-10-21 | Signed + sealed confirmation | **PART** |
| K55 | Claimed in scope §2 “environmental performance” | Type-test report ID, class **K55** | **MISS** |
| 62368-1 + 61326-1 | Lab **can** test (4674.01) | **TG544** CB / test reports + norms list | **MISS** (Ask #2) |
| 61010 | Written N/A | None unless SKU gains measuring I/O | Closed **N/A** |
| 61557-12 cert | Refused | None | Closed **N/A** |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-MET-010 | Treat TesPro pack as 61557-12 certificate | False #2 PASS | File only the **split** under REQ-MET-010 |
| R-MET-013 | Skip K55 because “61557 N/A” | O.15 #2 climate fail on the **box** | Still demand TesPro K55 / env report |
| R-NCV-08 | File 4674.01 as #2 17025 | Wrong lab / wrong methods | 4674.01 = TesPro EMC house only |

---

## Verification

| Requirement | TesPro evidence | Pass |
|-------------|-----------------|------|
| Ask #6 / REQ-MET-010 | Signed Scope Allocation | “61557 does not apply” + HiTEKS owns finished product |
| Ask #2 (enclosure only) | TG544 62368-1 + 61326-1 reports | Report numbers on **this SKU** |
| REQ-MET-013 | K55 report | −5…+55 °C, 17025 lab (Microtest OK **if** method added) |
| 61010 | Scope §3 | N/A while no measuring inputs |
| MET-001…012, 014, §6 | — | **Do not ask TesPro** |

**TesPro score vs annexes:** Ask #6 **ALIGNED as not PMD** (not as EPMF) · Annex **H EPMF MISS** · O.15 #2 K55 **VENDOR GAP** · 62368/61326 reports **VENDOR GAP** · Annex **D / F / O.7.4 / O.13.2** = **HiTEKS**, not TesPro.

---

## Related

| Doc | Role |
|-----|------|
| [61557-12_CLAUSES_VS_TESPRO.md](61557-12_CLAUSES_VS_TESPRO.md) | Every Ed 2.0 clause vs TesPro Scope §3 |
| [Full Validate.md](Full%20Validate.md) §I | All #2 clauses (HiTEKS + analyzer) |
| `CCI_TesPro_Cert_Reply_2026-10_Extract.md` | Full 9-row vendor pack |
| `CCI_61557-12_Extract.md` | REQ-MET-001…014 |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-21 | TesPro-only map: vendor asks vs 61557-12 alignment |
| 1.1 | 2026-10-21 | All-HAVE inventory: CNAS p.1 + 4674.01 + scope + what is still missing |
| 1.2 | 2026-10-21 | Full scan 2018 Ed 2.0: no Annex H; if TesPro adds 61557-12 use AMD1 or accept full PMD+61010 |
| 1.3 | 2026-10-21 | Ask #6 not EPMF; gaps vs CEI O.7.4/O.8/O.13.2/O.15 #2 and 61557 Annex A–H |
| 1.4 | 2026-10-21 | O.13.2 pin EMT432.V.FULL + FRC-350-100mV (not 85 mV) |

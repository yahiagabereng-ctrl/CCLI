# Chronos EMT432 — vendor requirements (certificates + integration)

**Document ID:** CCLI-VENDOR-CHRONOS-001  
**Revision:** 1.0  
**Date:** 2026-10-22  
**RAG source_id:** `ccli-chronos-emt432-vendor-requirements`  
**Audience:** Chronos srl · HiTEKS procurement · #2 type-test lab · bench integrators  
**Parents:** `ccli-meter-specimen-emt432-v-full` · `ccli-chronos-emt432` · `ccli-normative-cert-validation-matrix` · `cei-0-16-allegato-o-extract`  
**Bench pin (detail):** [METER_SPECIMEN_EMT432_V_FULL.md](METER_SPECIMEN_EMT432_V_FULL.md)  
**Register / UI:** [CCI_Chronos_EMT432_Extract.md](../../knowledge-base/08-engineering/CCI_Chronos_EMT432_Extract.md) · `apps/ccli/config/modbus/chronos_emt432_map.yaml`

**Role in programme:** **EMT432.V.FULL** + **FRC-350** Rogowski = **PMD front-end** on the **O.15 #2 / O.13.2** measurement chain. **TesPro TG544** is **not** the measuring object (comms-only; 61557 **N/A** on gateway). **HiTEKS `ccli`** polls Modbus and publishes **TotW / TotVAr / PPV** only — full PQ/harmonics remain on the meter.

---

## Requirements

### Normative (CEI 0-16 → meter chain)

| ID | Requirement | Normative basis | Status |
|----|-------------|-----------------|--------|
| **REQ-CHR-001** | External analyzer shall support **converter class ≤ 0,2** for active P at POC | Annex O **O.13.2.1** · 61557-12 Table 3 / 8 | **PART** — vendor claims **62053-22 0,2S**; **61557-12 cert MISS** |
| **REQ-CHR-002** | Current sensor **≤ 0,5** class; **not** fiscal CT/VT reuse | O.13.2.1 · 61557-12 Annex F | **PART** — FRC-350 catalog; formal class report **OPEN** |
| **REQ-CHR-003** | Type-test evidence for **CEI EN / IEC 61557-12** (PMD) on **EMT432** as used in CCI | Annex O **§12 #2** | **MISS** — **62053-22 ≠ 61557-12** |
| **REQ-CHR-004** | **61010-1** (+ applicable **61010-2**) safety on measuring equipment | O.15 #2 · O text **61010-2-201** vs 61557 **2-030** — lab must name object | **PART** — datasheet refs **HAVE**; Chronos **test report IDs OPEN** |
| **REQ-CHR-005** | EMC per **61557-12** PMD influence (Tables 9/13/17) or bundled in #3 | O.15 #2 EMC | **MISS** unless inside 61557 type-test |
| **REQ-CHR-006** | Climate **≥ K55** (−5…+55 °C) on analyzer | O.15 #2 · 61557 Table 6 | **OPEN** — ask Chronos report |
| **REQ-CHR-007** | Tests performed or reports issued under **ISO/IEC 17025**; Italy: **ACCREDIA**-recognised lab | O.15 #2 | **ACC** — HiTEKS **assembled CCI** type-test; Chronos supplies **OEM factory reports** |
| **REQ-CHR-008** | **CE Declaration of Conformity** + applied harmonised standards list for **EMT432** (and coil if separate SKU) | O.15 #8 technical file input | **OPEN** |
| **REQ-CHR-009** | Written statement: **EN 62053-22/24 class 0,2S is not IEC 61557-12 certification** | Programme traceability · R-MTR-003 | **OPEN** |

### Product pin (HiTEKS freeze)

| ID | Requirement | Basis | Status |
|----|-------------|-------|--------|
| **REQ-CHR-010** | SKU **EMT432.V.FULL** (Rogowski / 333 mV, FULL features) | Order code · bench SN **261001085352** | **HAVE** |
| **REQ-CHR-011** | Rogowski **FRC-350-100mV** = **100 mV/kA @ 50 Hz** (catalog); bench may use **80 mV/kA** with support/label | `TA_FL_ITA` · no **85 mV** SKU | **HAVE** / **SETUP** |
| **REQ-CHR-012** | Do **not** confuse flyer **85–265 V L-N** with coil “85 mV” | `EMT432_FL-ITA` | **HAVE** |
| **REQ-CHR-013** | **40235** = Rogowski sensitivity **[V/A]** = (mV/kA)/1e6 — not turns; not raw mV in UI | `EMT430_MR` | **SETUP** |
| **REQ-CHR-014** | **40232**: **1P** (bits 1–2 = 0) + **integrator bit 13 ON** for FRC | `EMT430_MR` | **SETUP** |
| **REQ-CHR-015** | Modbus TCP **502**, unit **1**, float32 **word-swap**; 1P map **P1 932 · Q1 940 · V_L1_N 905**; scales **0.001** W→kW | `chronos_emt432_map.yaml` · `lab_tr400_emt432_lan3_tcp_102.yaml` | **HAVE** |
| **REQ-CHR-016** | Meter **does not** implement CEI **O.8** 4 s fixed blocks for DSO — **PMF (`ccli`)** must aggregate per **61557-12 Annex D** (programme gap) | O.7.4 · O.8 · Annex D Fig D.2 | **HiTEKS SW GAP** — meter demand/PQ timers ≠ O.8 |

### Out of scope for Chronos

| ID | Not required from meter vendor |
|----|--------------------------------|
| **REQ-CHR-N01** | **FIPS 140-2 L3** (O.15 #3) — TPM on **TG544** |
| **REQ-CHR-N02** | **61850 UCA** (#5), **62351-100-3** (#6), **62443 EDSA** (#4) — **`ccli` / DUT** |
| **REQ-CHR-N03** | **MID / fiscal billing** meter cert — O.13.2 excludes **fiscal** CT/VT reuse |
| **REQ-CHR-N04** | **TesPro** 62368-1 / 61326 platform CE — separate vendor pack |

---

## Vendor deliverables — certificates and reports

Use as **Chronos ask list** (spreadsheet / letter). Status = corpus **2026-10-23** (STA pack — [CHRONOS_STA_REPORTS_ANNEX_MAP.md](../../lab/conformance/CHRONOS_STA_REPORTS_ANNEX_MAP.md)).

| NO. | Deliverable | Shall contain | O.15 / norm link | Status |
|-----|-------------|---------------|------------------|--------|
| **1** | **IEC/CEI EN 61557-12** type-test **certificate or full report** on **EMT432.V.FULL** (PMD) | Test lab name, report ID, edition (**2018+AMD1:2021** if EPMF split), scope (1P/3P, Rogowski) | **#2** | **MISS** |
| **2** | **EN 62053-22 / 62053-24** conformity evidence (class **0,2S**) | Cert or report ID — **explicitly not a substitute for row 1** | Supports **REQ-CHR-001** | **PART** (datasheet only) |
| **3** | **IEC 61010-1** test report + **CE DoC** for **EMT432** | Applied standards, model, SN range | **#2** safety | **PART** — **C25306_01** on **EMT130/430** only; **EMT432 + DoC OPEN** |
| **4** | **IEC 61010-2-*** part report** (as claimed on DoC) | Match O **2-201** vs 61557 **2-030** with HiTEKS #2 lab | **#2** | **PART** — **61010-2-030 HAVE** (STA); O text **2-201** gap remains |
| **5** | **EMC report** (61557 PMD tables or EN 61326 on analyzer) | Report ID, class, ports | **#2** EMC | **PART** — **C25305-ACR_01/02** · **61326-1** (not 61557 tables) |
| **6** | **K55 / climatic** test evidence | −5…+55 °C or 60068 series reference | **#2** | **PART** — declared **−25…+65 °C** on 61010 forms; **60068 report ID OPEN** |
| **7** | **FRC-350-100mV** (or supplied coil) test / class statement | **≤ 0,5** sensor class · **61010-1** / **61326-1** as applicable | **O.13.2** · Annex F | **PART** |
| **8** | **Rogowski coil label** photo + **mV/kA** written confirmation (bench **80** vs catalog **100**) | Traceability for **40235** | **REQ-CHR-011** | **OPEN** if disputed |
| **9** | **Modbus / firmware** stability statement for **FW 008** (or frozen FW for cert build) | Register map **EMT430_MR Rev2** alignment | Integration | **HAVE** map |
| **10** | **Factory calibration** / certificate of conformity per unit (if programme requires) | SN **261001085352** format | Bench trace | **OPEN** |

**Minimal vendor reply column (paste):**

| NO. | HiTEKS status (minimal) |
|-----|-------------------------|
| 1 | **OPEN** — need **61557-12** accredited report; **62053-22 ≠ 61557**. |
| 2 | **PART** — send **formal 62053-22/24** cert ID. |
| 3–4 | **OPEN** — **61010** reports + **CE DoC**. |
| 5–6 | **OPEN** — EMC + **K55** report IDs. |
| 7–8 | **PART** — FRC catalog **HAVE**; class report + coil label **OPEN**. |

---

## Architecture

### Functional

```text
FRC-350 Rogowski → EMT432.V.FULL (P/Q/U, Modbus TCP) → ccli (last-sample today) → PdCMMXU1
                         ↑ 40235 V/A, 40232 1P+integrator
TesPro TG544: no V/I — forwards only
```

### Interface matrix

| Interface | Source | Destination | Protocol / note |
|-----------|--------|-------------|-----------------|
| I | FRC-350 / Rogowski | EMT432 I1+/I1− | 333 mV · integrator ON |
| U | Grid L1-N | EMT432 V terminals | 85–265 V L-N |
| Fieldbus | EMT432 | TG544 LAN3 / PC | **Modbus TCP :502** unit **1** |
| P/Q/V | Reg **932 / 940 / 905** | `ccli` → MMS | float32, **word-swap**, ×**0.001** |
| DSO | `ccli` | Eth_A :3782 | **61850** — not on meter |

### BOM matrix

| Block | Function | Candidate / PN | Status |
|-------|----------|----------------|--------|
| Analyzer | PMD front-end | **EMT432.V.FULL** | **FROZEN** |
| Coil | External I | **FRC-350-100mV** (catalog) · bench **80 mV/kA** | **FROZEN / SETUP** |
| Map | Registers | `chronos_emt432_map.yaml` | **HAVE** |
| 61557-12 cert | PMD type-test | Chronos | **MISS** |

---

## Knowledge gaps

| Area | Current | Required from Chronos | Gap |
|------|---------|----------------------|-----|
| 61557-12 | 62053-22 marketing | Accredited **61557-12** report on **EMT432** | **MISS** |
| 61010 / CE | MI/FL PDFs | **DoC + test report numbers** | **OPEN** |
| FRC class | Flyer linearity ±0,2 % | **≤ 0,5** class test report | **OPEN** |
| 80 vs 100 mV/kA | Bench **80** deploy | Label + support letter | **OPEN** |
| FW lifecycle | FW **008** on bench | Frozen FW for production cert build | **OPEN** |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| **R-CHR-001** | File **62053-22** as **61557-12 #2** | DoC reject | **REQ-CHR-009** · keep #2 **OPEN** |
| **R-CHR-002** | Wrong **40235** (1.0, 85, 100 in UI) | P/I/P ~0 or 10³ error | `lab/emt432_rogowski_config.py` |
| **R-CHR-003** | No **61557** EMC/climate reports | #2 lab blocked | Rows **1, 5, 6** in deliverables |
| **R-CHR-004** | Fiscal CT shared with CCI chain | O.13.2 violation | Dedicated FRC only |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-CHR-010 | Nameplate + Modbus `40011=1`, `40015=1` | **V.FULL** Rogowski HW |
| REQ-CHR-013/014 | `python lab/emt432_rogowski_config.py --host <meter> --read` | 40235 **0.00008** or **0.0001**; 1P + integrator ON |
| REQ-CHR-015 | `python lab/modbus_emt432_tcp_probe.py --host <meter>` | P1/V @905 sane vs display |
| REQ-CHR-003 | Chronos letter + report ID | **61557-12** on scope |
| REQ-CHR-001/002 | Type-test file + Annex F chain calc | ≤ **0,2** / ≤ **0,5** demonstrated |

```powershell
lab\set_pc_lan3_plant_178.cmd
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1
python lab\modbus_emt432_tcp_probe.py --host 192.168.178.102
```

---

## Related documents

| Document | Role |
|----------|------|
| [METER_SPECIMEN_EMT432_V_FULL.md](METER_SPECIMEN_EMT432_V_FULL.md) | Bench specimen pin · first-adapt fail list |
| [Full Validate.md](Full%20Validate.md) § **I** · **A.8 #2** | Normative matrix · O.15 #2 |
| [61557-12_TESPRO_VENDOR_MAP.md](61557-12_TESPRO_VENDOR_MAP.md) §4.1 | O.13.2 / O.15 #2 vs TesPro (meter = HiTEKS) |
| [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md) | MMS model (downstream of meter) |
| [P5_ROGOWSKI_FRC350_BENCH.md](../evidence/phase5/P5_ROGOWSKI_FRC350_BENCH.md) | Wiring |
| [CCI_Chronos_Rogowski_Bench_Extract.md](../../knowledge-base/08-engineering/CCI_Chronos_Rogowski_Bench_Extract.md) | Rogowski extract |

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-22 | Master Chronos vendor list: normative REQ, cert deliverables 1–10, integration, gaps |

**Keywords:** `Chronos`, `EMT432`, `FRC-350`, `61557-12`, `O.15`, `#2`, `vendor`, `certificate`, `ccli-chronos-emt432-vendor-requirements`

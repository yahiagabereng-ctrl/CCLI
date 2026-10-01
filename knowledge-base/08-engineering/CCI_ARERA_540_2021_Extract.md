# ARERA Deliberation 540/2021/R/EEL — Engineering Extract (English)

**Document ID:** CCLI-REG-ARERA-540-001  
**Revision:** 1.0  
**Date:** 2026-09-16  
**RAG source_id:** `arera-540-2021`  
**Normative source:** ARERA Deliberation **540/2021/R/EEL** of **30 November 2021**  
**On-disk PDF:** [`../07-protocols/arera-540-2021.pdf`](../07-protocols/arera-540-2021.pdf) (24 pages)  
**Italian raw text:** `Architecture/_extracted_reg_analysis/arera_540-2021_it.txt`  
**Programme link:** K7.4 · REQ-REG-002 · `REGULATOR-IT` class

---

## Summary

ARERA **540/2021** regulates **data exchange** between **Terna** (TSO), **distribution system operators (DSOs)**, and **Significant Grid Users (SGUs)** for safe operation of the Italian national electrical system (SEN). It is the **market/regulatory driver** that mandated CEI development of **Annex O** (CCI functional spec) and **Annex T** (IEC 61850 interface).

**Scope of this deliberation:** production plants **≥ 1 MW** connected to **MV** networks (“**standard perimeter**”). Extended perimeter (&lt; 1 MW) is **deferred** to later ARERA decisions.

**Corpus status:** **HAVE (English engineering extract)**.

---

## Requirements (mapped to CCLI programme)

| ID | Requirement | Source | CCLI implication |
|----|-------------|--------|------------------|
| REQ-REG-540-001 | MV plants **≥ 1 MW** must use **CCI** per **CEI 0-16 Annex O + T** for data exchange | Art. 2.1, Considerando | Product must implement O/T; cert evidence |
| REQ-REG-540-002 | **DSO channel:** **IEC 61850** (plant → DSO) | Considerando p.12 | **Eth_A** / MMS server |
| REQ-REG-540-003 | DSO → Terna: **IEC 60870-5-104** on dedicated channels | Delib. 36/2020 cross-ref | DSO infrastructure; not CCI Eth_B unless OA |
| REQ-REG-540-004 | Real-time sampling **4 s** (MV production plants) | Delib. 36/2020 / SOGL | Aligns with Annex O PF1 blocks |
| REQ-REG-540-005 | **Producer** installs and maintains field CCI + plant comms | Art. 2.1 | Producer CAPEX; not gateway vendor alone |
| REQ-REG-540-006 | **DSO** collects, manages, and makes data available to Terna | Art. 2.2 | DSO ops; CCI must expose observability |
| REQ-REG-540-007 | **New plant:** CCI installed **before commissioning**; missing CCI → connection activation blocked | Art. 3.1–3.2 | Factory acceptance / TICA gate |
| REQ-REG-540-008 | **Existing plants:** upgrade by **31 January 2024** | Art. 4.1 | Historical deadline (programme context) |
| REQ-REG-540-009 | Per-unit **P** real-time measurement: **new plants only**; **not** required for existing-plant retrofits | Art. 2.1, Considerando p.16 | Reduces I/O/modbus burden on legacy retrofits |
| REQ-REG-540-010 | DSO comms infrastructure operational **before 1 December 2022** | Art. 5.2 | Lab timeline reference |
| REQ-REG-540-011 | CCI also required for **any MV plant** participating in **dispatch market (MSD)** regardless of size | Considerando p.12 | PF3 path optional in O but market-driven |

---

## Architecture

### CCI role (as defined by ARERA + CEI)

The **Central Plant Controller (CCI)** — including its **MCI** (Central Plant Monitor) observability part — enables the plant to appear as a **single equivalent generator** at the delivery point (PdC) while preserving visibility of constituent units.

| Function | Description | PF class (Annex O) |
|----------|-------------|-------------------|
| **Observability** | Collect plant data; convey to DSO (and via DSO to TSO) | **PF1 — mandatory** |
| **Regulation / control** | Coordinate plant elements for DSO grid-security needs at POC | **PF2 — optional** (see V5/385/2025 for ≥100 kW wind/PV) |
| **Dispatch / optimization** | MSD participation, BSP, plant management | **PF3 — discretionary** |

Annex O defines the CCI in **integrated form** (all function classes in one device); mandatory observability takes **priority** over optional/discretionary functions.

### Communication architecture

```mermaid
flowchart LR
  PLANT["Plant units / meters"]
  CCI["CCI / MCI"]
  DSO["DSO"]
  TERNA["Terna TSO"]

  PLANT -->|"internal plant bus"| CCI
  CCI -->|"IEC 61850 MMS"| DSO
  DSO -->|"IEC 60870-5-104"| TERNA
```

| Channel | Protocol | Owner |
|---------|----------|-------|
| Plant → CCI | Modbus / vendor / internal (Annex O plant side) | Producer |
| CCI → DSO | **IEC 61850** | CCI product (Annex T) |
| Second CCI port | Producer choice (104, OPC, etc.) | Optional — OA/BSP |
| DSO → Terna | **60870-5-104** | DSO |

**Note:** 540/2021 does **not** specify DI/DO counts — hardware I/O remains **CEI Annex O functional** + **product class** (AiLux 5 DI / 3 DO).

### Responsibilities

| Party | Responsibility |
|-------|----------------|
| **Producer** | Install/maintain CCI, field apparatus, plant internal comms for data acquisition per Annex O/T |
| **DSO** | Data acquisition (may use third DSO), management, forwarding to Terna; remote + sample on-site verification |
| **Terna** | Update Grid Code; receive DSO data; report compliance to ARERA |
| **CEI** | Annex O + T technical specifications (requested via Delib. 628/2018 procedure) |

If a DSO cannot perform data acquisition even via a third DSO, it must send **explicit motivated waiver** to Terna and ARERA; plant may then send data **directly to Terna** (104 MT / smart meter BT) as last resort per Delib. 36/2020.

---

## Applicability thresholds

| Category | Definition (540/2021) |
|----------|----------------------|
| **Standard perimeter** | MV-connected production **≥ 1 MW** |
| **Existing plant** | Entered service **on or before 30 Nov 2022** |
| **New plant** | Entered service **from 1 Dec 2022** |
| **MSD participation** | CCI required on MV regardless of plant size |
| **Extended perimeter (&lt; 1 MW)** | **Not regulated** in 540/2021 — deferred (Art. 7.2) |

Cross-reference: **ARERA 385/2025** (CEI V5) later extends PF2 mandates to **100–500 kW** wind/PV — see `CCI_Annex_O_Extract.md` §2.1.

---

## Standard perimeter — measurement scope (via Delib. 36/2020)

| Data | MV standard perimeter |
|------|----------------------|
| Plant-level **P and Q** | Required (real-time) |
| Per **generation-unit P** | **New plants:** required above CEI thresholds (170 kW inverter, 250 kW rotating, 50 kW storage); **Existing retrofits:** **not** required by 540/2021 |
| Sampling interval | **4 s** (MV) |
| Structural / forecast data | Per Grid Code All. A.6 / SOGL |

---

## Timelines (historical — programme context)

| Milestone | Date |
|-----------|------|
| DSO waiver deadline (if renouncing data role) | 31 Jan 2022 |
| DSO comms infrastructure ready | 30 Nov 2022 |
| New MV ≥1 MW plants must enter service with CCI | From **1 Dec 2022** |
| Existing-plant upgrade deadline | **31 Jan 2024** |
| Early-upgrade grant coefficients | 100% by 31 Mar 2023 → 25% by 31 Jan 2024 |
| Terna inadempienti report to ARERA | By 31 Mar 2024 |

---

## Costs and incentives (existing-plant retrofit)

| Item | Value (2021 deliberation) |
|------|---------------------------|
| Base forfait grant (field CCI + comms) | **€10,000** per plant |
| Politecnico di Milano cost range (2021 update) | €10,275 – €16,105 per plant |
| Per-unit P measurement add-on (new plants only) | €1,620 – €3,330 per generating unit |
| DSO sample site visit fee | **€200** per visit |

Grant funded from ARERA exceptional/resilience fund (Delib. 568/2019); DSO infrastructure from normal distribution tariffs.

---

## Verification / compliance (Art. 4.5)

DSO verifies existing-plant upgrades by:

1. Remote checks + **sample on-site visits**
2. Confirmation of device installation and **full data-exchange operability** including communication tests with DSO infrastructure

Producer submits ** sworn technician declaration** (D.P.R. 445/2000) that plant meets Grid Code and **CEI 0-16 Annex O/T observability** requirements.

**CCLI verification mapping:**

| 540/2021 test | CCLI bench equivalent |
|---------------|---------------------|
| Device installed | Physical install record |
| Data exchange operable | MMS client read on Eth_A; 4 s P/Q |
| DSO channel 61850 | libiec61850 + 62351-4 TLS interop |
| Annex O/T conformity | VAL L2/L3 + accredited lab path |

---

## Interface matrix (regulatory)

| Interface | Source | Destination | Protocol | Notes |
|-----------|--------|-------------|----------|-------|
| CCI → DSO | CCI Eth_A | DSO | **IEC 61850** | Mandated by 540/2021 |
| CCI → OA/BSP | CCI Eth_B | Operator | Not specified | Producer choice |
| DSO → Terna | DSO | TSO | **60870-5-104** | Outside CCI box |
| Plant → CCI | Inverters/meters | CCI | Modbus/etc. | Annex O plant scope |

---

## Knowledge gaps

| Area | Status | Gap |
|------|--------|-----|
| Extended perimeter (&lt; 1 MW) | Deferred in 540/2021 | Await ARERA + Terna criteria (Art. 7.2) |
| ARERA **385/2025** PF2 for 100–500 kW | Partial | Full delibera text **MISSING** — summary in Annex O §2.1 |
| DSO-specific ICD/SCL | PART | CEI TR 57-126 **HAVE**; per-DSO workbook still **MISSING** |
| Post-2024 inadempienza enforcement | Historical | Field policy — not in corpus |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-REG-540-001 | Implementing vendor box “61850” without Annex T | DSO rejection | Own libiec61850 + Allegato T ICD |
| R-REG-540-002 | Treating 540/2021 as hardware pin spec | Wrong BOM focus | O/T for function; product class for I/O |
| R-REG-540-003 | Ignoring producer install responsibility | Deployment model error | Package for producer/integrator handoff |

---

## Related corpus

| Document | Relationship |
|----------|--------------|
| `CCI_Annex_O_Extract.md` | Technical CCI requirements mandated by this delibera |
| `CCI_Annex_T_Extract.md` | IEC 61850 interface mandated for DSO path |
| `CCI_CEI_0-16_Consolidated.md` | CEI norm containing O/T |
| ARERA **36/2020/R/EEL** | Grid Code All. A.6 data-exchange scope (referenced, not ingested) |
| ARERA **628/2018/R/EEL** | Procedure that launched O/T CEI work (not ingested) |

---

## RAG routing

Add to **Pack D — Market + metrology:**

```
arera-540-2021, cei-0-16-allegato-o, cei-0-16-allegato-t, ccli-61557-12-extract
```

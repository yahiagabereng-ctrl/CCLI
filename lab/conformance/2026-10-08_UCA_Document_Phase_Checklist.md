# UCA lab document requirements — phase and evidence check

**Document ID:** CCLI-LAB-DOC-001  
**Date:** 2026-10-08  
**Trigger:** Lab email — return **filled PICS** for **price indication**; share templates with team.  
**RAG source_id:** `ccli-lab-uca-document-checklist`  
**Related:** `ccli-lab-conformance-questionnaire` · `ccli-61850-10-extract` · `ccli-phase-regulation-checklists` · [CCLI_PHASE_LAB_MASTER_LOG.md](../CCLI_PHASE_LAB_MASTER_LOG.md)

---

## 1. Lab deliverable set (IEC 61850-10 §5.4)

| # | Document | Template in repo | Lab priority | Purpose |
|---|----------|------------------|--------------|---------|
| D1 | **PICS** | `lab/TemplatePics_Ed1Ed2Ed2p1_Excel_rev3p0.xlsx` | **Immediate (quote)** | Declares ACSI blocks, services, editions → drives test days and fee |
| D2 | **PIXIT** | **MISSING** — add `TemplatePixit*` from email | Before test slot | Timeouts, max clients, IntgPd, dataset limits, ctlModel, segmentation |
| D3 | **MICS** | `lab/TemplateMICS_Ed2_FromTP2.0.5.docx` | With PICS / pre-test | Model vs standard logical nodes; maps to CID |
| D4 | **TICS** | `lab/TemplateTICS_Ed2_FromTP2.0.5.docx` | Pre-test | TISSUES resolution list |
| D5 | **ICD/CID/SCD** | Product CID (no UCA form) | Pre-test | Lab may build SCD from ICD (confirm in meeting §2.5) |
| D6 | **User / lab guide** | Internal draft | Pre-test | Configuration, ports, roles, firmware ID |
| D7 | **62351-3 PID** (PICS + PIXIT) | Separate from UCA xlsx | Quote / parallel track | Transport (TLS); see §4 |

**Corpus status:** Templates **HAVE** (3/4) · PIXIT template **MISSING** in git · Filled PICS/PIXIT/MICS/TICS **TODO**.

---

## 2. Cross-walk — document ↔ programme phases

Phases **P0–P6** are **CLOSED** (internal lab gates). **P7** is **OPEN** (evidence pack). Formal UCA testing is **after** P7 software exit and **does not** reopen P3 lab closure — except where noted for **TSP Connect** residual.

| Phase | Status (master log) | What it gives the UCA pack | Document impact |
|-------|---------------------|----------------------------|-----------------|
| **P0** Platform | CLOSED | TG544 deploy path, Eth_A bind | D6: DUT platform string; test setup only |
| **P1** Local PF2 | CLOSED (eng.) | Regulation mapping | Scope narrative for D1/D3 (PF2 / Annex O) |
| **P2** Isolation | CLOSED | Port / zone map | D2 PIXIT: which ports exposed at lab (102 bench, 3782 prod-like) |
| **P3** DSO MMS | CLOSED (lab) | Browse, URCB ~4 s, Wlim, TLS :3782, RBAC, CRL, chrony | **Core of D1** (ACSI services) · **D2** (IntgPd, ports) · **D5** `lab_tg544_eth_a.cid` |
| **P3 residual** | PART (§3.7) | TSP Connect fail after AARE (dual-cert) | **Risk for formal 62351-4 / TSP lab day** — not a blocker for **PICS quote** if scope is 61850-10 server + 62351-3 TLS only |
| **P4** IEC 104 | CLOSED | Out of 61850 PICS server scope | Exclude from D1 unless lab bundles |
| **P5** Observability | CLOSED | GOOSE publish exists | D1: **exclude GOOSE publisher** in first pass (questionnaire §1.4) unless lab mandates |
| **P6** Defence I/O | CLOSED | DIO / inhibit | MICS only if modeled as LN/DO; else out of 61850 scope |
| **P7** Evidence | OPEN | Event store, O.14 matrix, DSO demo script | D6 + O.15 narrative; **P7-07 OPEN** = 62351-100-3 cert **plan** (align with D7) |

**Phase 7 exit (internal)** before accredited **61850 certificate** run: P7-01, P7-04, P7-11 → PASS per [P7_README.md](../evidence/phase7/P7_README.md).

---

## 3. Per-document readiness

### D1 — PICS (`TemplatePics_Ed1Ed2Ed2p1_Excel_rev3p0.xlsx`)

| Sheet | Content | Evidence / source | Fill status |
|-------|---------|-------------------|---------------|
| Cover | Device ID, firmware, edition | Master log §0.5 build **r31+**; questionnaire §10 | **TODO** |
| General | Roles, editions, SCSM | Server only · Ed. 2 model · 8-1 MMS · questionnaire §0 | **TODO** |
| ACSI Basic | B11 server, B21 8-1, … | P3-01 listen, P3-03 browse | **TODO** (mostly **M** server, **N/A** client/subscriber) |
| ACSI Model | Logical devices, CDCs | ICD/CID · P3-02 PART (dynamic genconfig) | **PART** — static lab CID OK for cert specimen |
| ACSI Service | Association, datasets, URCB, control, time | P3-03, P3-04, P5 full-circle, `mms_adapter.cpp` | **TODO** — map each **M** to internal PASS id |
| 61869-9 / 9-3 | Process bus | Not in product claim | **N/A** — leave unsupported |

**Draft claim profile (for quote — confirm with lab):**

- **Include:** Server role; MMS (8-1); GetDirectory*; Get/SetDataValues; Data sets; **Unbuffered reporting**; **Control** (Operate / ctlModel per DO); **Time sync** (client); Association.
- **Exclude (first pass):** Buffered reporting; Logging; File transfer; Setting groups; Sampled Values; **GOOSE publisher**; Client role; 9-1 / 9-2 SCSMs.

**Competitor reference:** `knowledge-base/08-engineering/CCI_Certificate_Samples_Extract.md` (DNV UCA sample scope).

### D2 — PIXIT

| Item | HiTEKS draft value | Source |
|------|-------------------|--------|
| TCP ports | 3782 TLS (DSO); 102 plain lab only | `lab_tr400_phase1_regulation.yaml` |
| Max associations | e.g. 4 (confirm in implementation) | Questionnaire §10 |
| IntgPd | 1000–60000 ms; TotW default **4000 ms** | P3-03 ~3961 ms evidence |
| ctlModel | Direct-with-normal-security (per DO in CID) | ICD |
| TLS | TLS 1.2; mutual auth; cipher suites | P3-06 PASS |
| Cert sizes | RSA 2048 | Annex T / EJBCA lab PKI |

**Status:** Values **HAVE** in engineering docs; **TODO** copy into lab Word PIXIT once template is added to `lab/`.

### D3 — MICS

| Item | Source | Status |
|------|--------|--------|
| IED name, LD/LN inventory | `apps/ccli/config/icd/lab_tg544_eth_a.cid` | **HAVE** |
| URCB / RCB instances | CID + P3-03 traces | **HAVE** |
| Control objects (Wlim, WSd) | P3-04, P5 | **HAVE** |
| Formal MICS tables in Word | Template | **TODO** |

### D4 — TICS

| Item | Status |
|------|--------|
| TISSUES database date | **TODO** — ask lab (questionnaire §2.4) |
| Per-TISSUE implementation | **TODO** — minimal list; libiec61850 + our patches |

### D5 — SCL

| Artifact | Path | Status |
|----------|------|--------|
| Lab CID | `apps/ccli/config/icd/lab_tg544_eth_a.cid` | **HAVE** |
| Example / TR CID | `apps/ccli/config/icd/cei-tr-57-126-example.cid` | **HAVE** |
| ICD master | Under `apps/ccli/config/icd/` | **HAVE** |
| Lab SCL validator run | TSP offline SCL evidence | **PART** — `lab/evidence/testsuite-pro/offline-scl/` |

### D6 — Manual / lab configuration guide

| Content | Phase link | Status |
|---------|------------|--------|
| Eth_A addressing, yaml, roles | P2, P3 | **PART** — scattered in `lab/` scripts |
| Firmware ID procedure | P7, questionnaire §10 | **TODO** |
| Event log ≥2048 | P7-01 PART | **PART** |

### D7 — IEC 62351-3 PID (separate from UCA PICS xlsx)

| Item | Regulation | Phase | Status |
|------|------------|-------|--------|
| 62351-3 §8 PICS tables | O.15 / 62351-100-3 | P7-07 **OPEN** | **TODO** |
| TLS PIXIT | Lab 62351 template (if any) | P3-06 evidence | **TODO** |
| 62351-100-3 test cases | Extract in KB | — | **HAVE** (planning) |

Lab may quote **61850 only** from D1 first; confirm combined visit (questionnaire §1.6–1.7).

---

## 4. Internal evidence map (P3 → PICS services)

Use this when marking **M** / **O** in the Excel service sheet:

| Internal gate | Evidence pointer | PICS-relevant capability |
|---------------|------------------|---------------------------|
| P3-01 | `P3_01_MMS_LISTEN_ETH_A.txt` | Server listens MMS/TLS |
| P3-03 | `P3_03_MMS_CLIENT_STEP5.txt` | Browse, datasets, URCB IntgPd |
| P3-04 | Wlim / DIO evidence | Control Operate |
| P3-05 | Comms-loss fallback | Application behaviour (PIXIT timeout) |
| P3-06 | TLS :3782 | 62351-3 transport (D7 overlap) |
| P3-07 | ACSE mutual cert | 62351-4 (formal cert separate) |
| P3-08 | VIEWER DENY | Security / negative test readiness |
| P3-09 | CRL | PKI PIXIT |
| P3-11 | chrony ≤±100 ms | Time quality |
| §3.7 TSP Connect | `test4.pcapng`, master log | **Formal TSP / 62351-4 E2E** — fix before claiming full 62351-4 in D7 |

---

## 5. Gaps and actions

| ID | Gap | Owner | Priority |
|----|-----|-------|----------|
| G1 | **Filled PICS** not started | Engineering | **P0 — lab quote** |
| G2 | **PIXIT Word template** not in repo | Admin | Add from email attachment |
| G3 | Meeting answers empty (§11 questionnaire) | PM + lab | After Nicola thread |
| G4 | MICS/TICS Word not filled | Engineering | After PICS scope frozen |
| G5 | 62351-3 PID | Engineering | P7-07; may be second quote |
| G6 | TSP Connect / Cert B vs Cert A | Engineering | Before 62351-4 test day |
| G7 | P7-01/04/11 still PART | Lab TG544 | Before shipping DUT for cert |

---

## 6. Suggested submission order

1. **Filled PICS (D1)** → lab pricing (**this week**).  
2. Add **PIXIT template** to repo; fill **D2** using §3 table.  
3. **MICS (D3)** from `lab_tg544_eth_a.cid` + Services list in questionnaire §10.  
4. **TICS (D4)** after lab confirms TISSUES date.  
5. **CID + lab guide (D5–D6)** with firmware manifest when P7 gates green.  
6. **62351 PID (D7)** — same or second lab engagement.

---

## 7. Verification (this checklist)

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-LAB-DOC-001 | All four UCA templates present in `lab/` | 4/4 files tracked in git |
| REQ-LAB-DOC-002 | PICS draft aligns with closed P3 evidence | Every **M** row has internal PASS or explicit PART |
| REQ-LAB-DOC-003 | Scope excludes non-DSO features | GOOSE/SV/client marked **N/A** unless lab overrides |
| REQ-LAB-DOC-004 | Phase P7 exit tracked | P7-01/04/11 PASS before DUT ship |

**Current verdict:** REQ-LAB-DOC-001 **FAIL** (PIXIT template missing). REQ-LAB-DOC-002 **OPEN** (PICS not filled). REQ-LAB-DOC-003 **READY** (claim profile documented). REQ-LAB-DOC-004 **PART**.

---

**RAG tags:** `lab`, `UCA`, `PICS`, `PIXIT`, `MICS`, `TICS`, `61850-10`, `Phase7`, `ccli-lab-uca-document-checklist`

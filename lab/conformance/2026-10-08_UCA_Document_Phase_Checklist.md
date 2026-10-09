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
| D2 | **PIXIT** | **MISSING** Word — values in [PIXIT_DRAFT.md](PIXIT_DRAFT.md) | Before test slot | Timeouts, max clients, IntgPd, dataset limits, ctlModel, segmentation |
| D3 | **MICS** | `lab/TemplateMICS_Ed2_FromTP2.0.5.docx` | With PICS / pre-test | Model vs standard logical nodes; maps to CID |
| D4 | **TICS** | `lab/TemplateTICS_Ed2_FromTP2.0.5.docx` | Pre-test | TISSUES resolution list |
| D5 | **ICD/CID/SCD** | Product CID (no UCA form) | Pre-test | Lab may build SCD from ICD (confirm in meeting §2.5) |
| D6 | **User / lab guide** | [LAB_CONFIG_GUIDE.md](LAB_CONFIG_GUIDE.md) | Pre-test | Configuration, ports, roles, firmware ID |
| D7 | **62351-3 PID** (PICS + PIXIT) | [D7_62351-3_PID_DRAFT.md](D7_62351-3_PID_DRAFT.md) | Quote / parallel track | Transport (TLS); see §4 |

**Corpus status:** Templates **HAVE** (3/4) · PIXIT Word **MISSING** · markdown fills **HAVE** ([PICS_FILL.md](PICS_FILL.md) · [PIXIT_DRAFT.md](PIXIT_DRAFT.md) · [MICS_DRAFT.md](MICS_DRAFT.md) · [TICS_DRAFT.md](TICS_DRAFT.md)). Excel/Word paste **TODO**. CID Services vs PICS: [CID_PICS_ALIGNMENT.md](CID_PICS_ALIGNMENT.md).

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
| Cover | Device ID, firmware, edition | Master log §0.5 build **r31+**; questionnaire §10 · [PICS_FILL.md](PICS_FILL.md) | **READY to paste** |
| General | Roles, editions, SCSM | Server only · Ed. 2 model · 8-1 MMS · questionnaire §0 | **READY to paste** |
| ACSI Basic | B11 server, B21 8-1, … | P3-01 listen, P3-03 browse | **READY** — **M** server, **N/A** client/subscriber |
| ACSI Model | Logical devices, CDCs | ICD/CID · [MICS_DRAFT.md](MICS_DRAFT.md) | **READY** — static lab CID; trim Services for cert |
| ACSI Service | Association, datasets, URCB, control, time | P3-03, P3-04, P5 full-circle, `mms_adapter.cpp` | **READY** — each **M** mapped in PICS_FILL |
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
| ctlModel | **direct-with-enhanced-security** (Wlim/WSd) | CID · [PIXIT_DRAFT.md](PIXIT_DRAFT.md) |
| TLS | TLS 1.2; mutual auth; cipher suites | P3-06 PASS |
| Cert sizes | RSA 2048 | Annex T / EJBCA lab PKI |

**Status:** Values **HAVE** in [PIXIT_DRAFT.md](PIXIT_DRAFT.md). **TODO** paste into lab Word once `TemplatePixit*` is added to `lab/`.

### D3 — MICS

| Item | Source | Status |
|------|--------|--------|
| IED name, LD/LN inventory | `apps/ccli/config/icd/lab_tg544_eth_a.cid` | **HAVE** |
| URCB / RCB instances | CID + P3-03 traces | **HAVE** |
| Control objects (Wlim, WSd) | P3-04, P5 | **HAVE** |
| Formal MICS tables in Word | [MICS_DRAFT.md](MICS_DRAFT.md) | **READY to paste** |

### D4 — TICS

| Item | Status |
|------|--------|
| TISSUES database date | **HOLD** — ask lab (questionnaire §2.4) |
| Per-TISSUE implementation | [TICS_DRAFT.md](TICS_DRAFT.md) — empty until lab date |

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
| Eth_A addressing, yaml, roles | P2, P3 | **HAVE** — [LAB_CONFIG_GUIDE.md](LAB_CONFIG_GUIDE.md) |
| Firmware ID procedure | P7, questionnaire §10 | **HAVE** in D6 (`ccli --version` + SHA-256) |
| Event log ≥2048 | P7-01 PART | **PART** — code HAVE; signed wrap OPEN |

### D7 — IEC 62351-3 PID (separate from UCA PICS xlsx)

| Item | Regulation | Phase | Status |
|------|------------|-------|--------|
| 62351-3 §8 PICS tables | O.15 / 62351-100-3 | P7-07 **OPEN** | **READY to paste** — [D7_62351-3_PID_DRAFT.md](D7_62351-3_PID_DRAFT.md) |
| TLS PIXIT | Lab 62351 template (if any) | P3-06 evidence | **READY** in D7 draft |
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
| G1 | **PICS Excel** not pasted from [PICS_FILL.md](PICS_FILL.md) | Engineering | **P0 — lab quote** |
| G2 | **PIXIT Word template** not in repo | Admin | Add from email; values in PIXIT_DRAFT |
| G3 | Meeting answers empty (§11 questionnaire) | PM + lab | After Nicola thread |
| G4 | MICS/TICS **Word** not pasted | Engineering | Drafts HAVE |
| G5 | 62351-3 PID **lab form** | Engineering | Markdown HAVE; P7-07 formal |
| G6 | TSP Connect / Cert B vs Cert A | Engineering | Connect reported PASS; keep evidence |
| G7 | P7-01/04/11 still PART | Lab TG544 | Before shipping DUT for cert |
| G8 | CID `<Services>` over-claims vs PICS | Engineering | [CID_PICS_ALIGNMENT.md](CID_PICS_ALIGNMENT.md) — trim at cert freeze |

---

## 6. Suggested submission order

1. **Paste PICS (D1)** from [PICS_FILL.md](PICS_FILL.md) → Excel → lab pricing.  
2. Add **PIXIT Word** to repo; paste [PIXIT_DRAFT.md](PIXIT_DRAFT.md).  
3. **MICS (D3)** from [MICS_DRAFT.md](MICS_DRAFT.md).  
4. **TICS (D4)** after lab confirms TISSUES date ([TICS_DRAFT.md](TICS_DRAFT.md)).  
5. **CID + lab guide (D5–D6)** — [LAB_CONFIG_GUIDE.md](LAB_CONFIG_GUIDE.md); cert Services trim.  
6. **62351 PID (D7)** — [D7_62351-3_PID_DRAFT.md](D7_62351-3_PID_DRAFT.md).

---

## 7. Verification (this checklist)

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-LAB-DOC-001 | All four UCA templates present in `lab/` | 4/4 files tracked in git |
| REQ-LAB-DOC-002 | PICS draft aligns with closed P3 evidence | Every **M** row has internal PASS or explicit PART |
| REQ-LAB-DOC-003 | Scope excludes non-DSO features | GOOSE/SV/client marked **N/A** unless lab overrides |
| REQ-LAB-DOC-004 | Phase P7 exit tracked | P7-01/04/11 PASS before DUT ship |

**Current verdict:** REQ-LAB-DOC-001 **FAIL** (PIXIT Word missing; markdown PIXIT **HAVE**). REQ-LAB-DOC-002 **PART** (PICS values filled in markdown; Excel not yet sent). REQ-LAB-DOC-003 **READY**. REQ-LAB-DOC-004 **PART**.

---

**RAG tags:** `lab`, `UCA`, `PICS`, `PIXIT`, `MICS`, `TICS`, `61850-10`, `Phase7`, `ccli-lab-uca-document-checklist`

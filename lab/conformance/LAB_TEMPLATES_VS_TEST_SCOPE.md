# Lab templates vs test procedure (what the lab requires)

**Document ID:** CCLI-LAB-TPL-001  
**Date:** 2026-10-08  
**RAG source_id:** `ccli-lab-templates-test-scope`  
**Related:** `ccli-lab-uca-document-checklist` · `ccli-61850-10-extract` · `ccli-lab-requirement-test-matrix`

---

## Summary (important distinction)

| What lab sent | Type | Purpose |
|---------------|------|---------|
| **Templates in `lab/`** | **Declaration forms** (you fill and return) | Define **which tests** the accredited lab will run and **parameters** (PIXIT) |
| **UCA / IEC 61850-10 test procedure** | **Executable test cases** (lab-owned) | Lab runs after PICS/PIXIT/MICS/TICS are accepted — **not** a fill-in Excel of test steps |
| **Test Suite Pro** | **Internal pre-test** (HiTEKS) | Maps to same scope via [`LAB_REQUIREMENT_TEST_MATRIX.md`](LAB_REQUIREMENT_TEST_MATRIX.md) — **not** the lab’s certificate template |

**61850-10 §5.4:** filled **PICS + PIXIT** → lab **parameterizes** abstract test tables (§6.2.4, e.g. sAss, sRp Table 14, sCtl Table 26).  
**There is no separate “lab test steps template” in the repo** besides the **PID documents** below.

---

## Templates from lab email (repo status)

| # | Lab document | File in repo | Edition / lineage | Filled by | Drives which tests |
|---|--------------|--------------|-------------------|-----------|-------------------|
| **D1** | **PICS** (Protocol ICS) | `lab/TemplatePics_Ed1Ed2Ed2p1_Excel_rev3p0.xlsx` | UCAIug Excel **rev 3.0**; based on **61850-7-2 Annex A** (+ Ed.2 cols) | HiTEKS | **All** 61850-10 server groups you mark **M** / **O** |
| **D2** | **PIXIT** (extra test info) | Word **MISSING**; values [PIXIT_DRAFT.md](PIXIT_DRAFT.md) | IEC 61850-10 **Annex E** style; UCA server Word | HiTEKS | Timeouts, max clients, IntgPd range, segmentation, ctlModel → **how** cases run |
| **D3** | **MICS** (Model ICS) | `lab/TemplateMICS_Ed2_FromTP2.0.5.docx` | **Edition 2** · **UCA Test Procedure 2.0.5** | HiTEKS | Model / LN / DO claims → **Table 3** / model tests |
| **D4** | **TICS** (TISSUES ICS) | `lab/TemplateTICS_Ed2_FromTP2.0.5.docx` | **Edition 2** · **TP 2.0.5**; UCA IUG QAP | HiTEKS | TISSUES resolution → certificate reference |
| — | **Instructions** (not submitted) | PICS sheet **Instructions** | UCAIug help text | Remove before send | — |

**Naming clue:** `FromTP2.0.5` on MICS/TICS = lab procedure aligned with **UCA 61850 server test procedure v2.0.5** (confirm **v2.0.6** or **61850-10 Ed. 2.1** with lab — questionnaire **1.1**).

---

## PICS workbook — sheets = capability blocks (→ test groups)

| Sheet | Maps to 61850-10 | Server-only CCI (first pass) |
|-------|------------------|------------------------------|
| **Cover** | Table 1 documentation | Device id, firmware, date |
| **General** | Scope narrative | Server · 8-1 · not 9-2 SV |
| **ACSI Basic Conformance** | Roles, SCSM (B11, B21, …) | Server **M**, 8-1 **M**, client N/A |
| **ACSI Model Conformance** | Logical nodes / CDCs | From `lab_tg544_eth_a.cid` |
| **ACSI Service Conformance** | Per-service **M/O/—** | Association, Get*, Set*, DataSet, **URCB**, **Ctl**, Time |
| **61869-9 Conformance** | Process bus | **N/A** |
| **61850-9-3 Conformance** | 9-3 | **N/A** |
| **Instructions** | — | **Do not submit** |

Each **M** on **ACSI Service** sheet ≈ one or more **61850-10** abstract case groups (`sAss`, `sSrv`, `sDs`, `sRp`, `sCtl`, `sTm`, …) in `ccli-61850-10-extract` §4.4.

---

## PIXIT — parameters for tests (not optional for formal test)

Typical items lab expects (questionnaire §2.2, §10):

| PIXIT item | HiTEKS draft source |
|------------|---------------------|
| TCP port | **3782** (`lab_tr400_phase1_regulation.yaml`) |
| IntgPd min/max/default | **1000–60000**, default **4000** |
| Max associations | Declare (e.g. 4) |
| ctlModel | **direct-with-enhanced-security** (CID) |
| TLS / cert sizes | 62351-3 lab profile |
| Segmentation / max dataset | Static CID datasets |

**Until PIXIT Word template is in `lab/`,** copy [PIXIT_DRAFT.md](PIXIT_DRAFT.md) into the lab form. PICS cells: [PICS_FILL.md](PICS_FILL.md). Do not copy raw CID `<Services>` — [CID_PICS_ALIGNMENT.md](CID_PICS_ALIGNMENT.md).

---

## MICS / TICS — not the test script

- **MICS** = “how our IED models vs IEC 61850-7-4 / TR profile” — must match **same CID** as DUT and **PICS** model rows.  
- **TICS** = UCA **TISSUES** database resolutions — required for **certificate** (see TICS intro text in template).

Neither file lists step-by-step test clicks; they **scope** the lab’s automated/manual procedure.

---

## Separate from 61850 PICS (same lab visit or second quote)

| Track | Template source | Corpus |
|-------|-----------------|--------|
| **62351-3 transport** | **62351-3 §8 PICS** (+ TLS PIXIT) — questionnaire **2.6** | `ccli-62351-3-extract` |
| **62351-100-3** formal cases | Accredited test plan (not UCA xlsx) | `ccli-62351-100-3-extract` |
| **62351-4 MMS E2E** | 62351-4 PICS / lab PID | `ccli-62351-4-extract` |

D7 in [`2026-10-08_UCA_Document_Phase_Checklist.md`](2026-10-08_UCA_Document_Phase_Checklist.md).

---

## Internal “test template” (HiTEKS — Test Suite Pro)

Not from lab email; use to **preview** the same scope before accredited run:

| Artifact | Role |
|----------|------|
| [`LAB_REQUIREMENT_TEST_MATRIX.md`](LAB_REQUIREMENT_TEST_MATRIX.md) | Lab doc row ↔ 61850-10 ↔ TSP step ↔ evidence |
| [`TSP_TEST_IDENTIFICATION_POST_CONNECT.md`](../evidence/testsuite-pro/TSP_TEST_IDENTIFICATION_POST_CONNECT.md) | Tier 1–4 detail |
| [`CCI_TestSuitePro_Sequencer_Draft.md`](../CCI_TestSuitePro_Sequencer_Draft.md) | MMS object paths |
| `scripts/stage-tsp-field-pack.ps1` | Push checklist + CID + TLS to other PC |

---

## Action checklist

| □ | Action |
|---|--------|
| □ | Add missing **PIXIT** Word template to `lab/` |
| □ | Paste [PICS_FILL.md](PICS_FILL.md) → Excel → send for **quote** |
| □ | Confirm **TP version** (2.0.5 vs 2.0.6) and **61850-10 edition** with lab |
| □ | Paste **MICS/TICS** drafts into Word + TISSUES date |
| □ | Run **TSP matrix** on other PC; evidence supports PICS **M** rows |
| □ | Paste [D7_62351-3_PID_DRAFT.md](D7_62351-3_PID_DRAFT.md) if bundled with 61850 quote |

---

**RAG tags:** `lab`, `UCA`, `PICS`, `PIXIT`, `MICS`, `TICS`, `TP-2.0.5`, `61850-10`, `ccli-lab-templates-test-scope`

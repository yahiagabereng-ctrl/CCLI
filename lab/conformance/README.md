# Lab conformance documents (UCA / IEC 61850-10)

**RAG / product deploy:** [RAG_SOURCE_INDEX.md](RAG_SOURCE_INDEX.md) (`ccli-rag-source-index`) · [PRODUCT_DEPLOYMENT_INIT.md](PRODUCT_DEPLOYMENT_INIT.md) · [dnv/DNV_UCA_PRODUCT_READINESS.md](dnv/DNV_UCA_PRODUCT_READINESS.md)

Templates from the accredited lab (2026-10-08 email) live at repo root under `lab/`:

| File | Document | Normative basis |
|------|----------|-----------------|
| `lab/TemplatePics_Ed1Ed2Ed2p1_Excel_rev3p0.xlsx` | **PICS** (Protocol Implementation Conformance Statement) | IEC 61850-7-2 Annex A · UCAIug template rev 3.0 |
| `lab/TemplateMICS_Ed2_FromTP2.0.5.docx` | **MICS** (Model Implementation Conformance Statement) | ICD/CID + logical model |
| `lab/TemplateTICS_Ed2_FromTP2.0.5.docx` | **TICS** (TISSUES Implementation Conformance Statement) | UCA TISSUES database |
| *(not in repo yet)* | **PIXIT** Word template | IEC 61850-10 Annex E |
| [PIXIT_DRAFT.md](PIXIT_DRAFT.md) | **PIXIT values** (working copy) | Ports, IntgPd, ctlModel |
| [PICS_FILL.md](PICS_FILL.md) | **PICS M/N/A** (paste into Excel) | 7-2 **Annex A** first pass |
| [PICS_FILLED_WORKBOOK.md](PICS_FILLED_WORKBOOK.md) | **Filled D1 xlsx** + SHA + alignment review | `drafts/PICS_CCI016_01_filled_rev3p0_AutoRecovered.xlsx` |
| [CCI_61850-7-2_Extract.md](../../knowledge-base/08-engineering/CCI_61850-7-2_Extract.md) | **7-2 ACSI + Annex A (Batch F)** | OCR corpus `_extract_61850-7-2/` |
| [MICS_DRAFT.md](MICS_DRAFT.md) · [TICS_DRAFT.md](TICS_DRAFT.md) | Model + TISSUES drafts | CID / empty TICS |
| [D7_62351-3_PID_DRAFT.md](D7_62351-3_PID_DRAFT.md) | 62351-3 PID | §8 Tables 1–5 |
| [CID_PICS_ALIGNMENT.md](CID_PICS_ALIGNMENT.md) | CID Services vs PICS | Do not over-claim |
| [SCL_DATASET_PLACEMENT.md](SCL_DATASET_PLACEMENT.md) | DataSet under LLN0 + PdC FCDA map | IEC 61850-6 · lab CID |
| [LAB_CONFIG_GUIDE.md](LAB_CONFIG_GUIDE.md) | D6 lab guide | IPs, yaml, firmware ID |
| [LAB_SPECIMEN_VERSION.md](LAB_SPECIMEN_VERSION.md) | **CID/cfg pin** (History rev, configRev, SHA-256) | TSP Compare / MICS / deploy |
| [DUT_LAB_FINALIZE_AND_CERT_PATH.md](DUT_LAB_FINALIZE_AND_CERT_PATH.md) | **DUT bench close** vs **UCA certificate** path | After T1–T10 |
| [LAB_UNIFIED_CERT_TEST_MATRIX.md](LAB_UNIFIED_CERT_TEST_MATRIX.md) | **Single □** functional + cyber + O.15 (CSV for Excel) | `ccli-lab-unified-cert-test-matrix` |
| [LAB_UNIFIED_CERT_TEST_ROADMAP.md](LAB_UNIFIED_CERT_TEST_ROADMAP.md) | Two-PC bench phase order (TSP + meter + RS485) | `ccli-lab-unified-cert-test-roadmap` |
| [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md) | **5-LN** product specimen pin (first unified run) | `ccli-product-specimen-5ln` |
| [PRODUCT_SPECIMEN_5LN_VALIDATION.md](PRODUCT_SPECIMEN_5LN_VALIDATION.md) | Matrix vs 5-LN audit + validator | `ccli-product-specimen-5ln-validation` |
| [P5_FULLCIRCLE_5LN_SCOPE.md](P5_FULLCIRCLE_5LN_SCOPE.md) | Annex P5 steps **without** 31-LN / VArSd | `ccli-p5-fullcircle-5ln-scope` |
| [LAB_REQUIREMENT_TEST_MATRIX.md](LAB_REQUIREMENT_TEST_MATRIX.md) | Functional REQ ↔ TSP ↔ 61850-10 | `ccli-lab-requirement-test-matrix` |
| [LAB_REQUIREMENT_TEST_MATRIX_CYBER.md](LAB_REQUIREMENT_TEST_MATRIX_CYBER.md) | **62351 / D7 / Tier 3** cyber checklist | `ccli-lab-requirement-test-matrix-cyber` |
| [LAB_REQUIREMENT_TEST_MATRIX_2026-10-09_DUT.md](LAB_REQUIREMENT_TEST_MATRIX_2026-10-09_DUT.md) | Matrix snapshot ☑/◐/☐ | DUT sign-off |

**Working checklist (phases + evidence):** [2026-10-08_UCA_Document_Phase_Checklist.md](2026-10-08_UCA_Document_Phase_Checklist.md)

**Lab templates vs accredited test scope:** [LAB_TEMPLATES_VS_TEST_SCOPE.md](LAB_TEMPLATES_VS_TEST_SCOPE.md) (`ccli-lab-templates-test-scope`)

**Two-PC phased run (TSP + plant/logger):** [TSP_TWO_PC_BENCH_RUNBOOK.md](TSP_TWO_PC_BENCH_RUNBOOK.md) (`ccli-tsp-two-pc-bench-runbook`)

**Full bench HW (single PC — triple NIC + USB RS485):** [TSP_PC_FULL_BENCH_HW.md](TSP_PC_FULL_BENCH_HW.md) (`ccli-tsp-full-bench-hw`)

**Test Suite Pro — which tests to run after Connect:** [../evidence/testsuite-pro/TSP_TEST_IDENTIFICATION_POST_CONNECT.md](../evidence/testsuite-pro/TSP_TEST_IDENTIFICATION_POST_CONNECT.md)

**Lab requirement ↔ TSP test matrix (field checklist):** [LAB_REQUIREMENT_TEST_MATRIX.md](LAB_REQUIREMENT_TEST_MATRIX.md) · **Push to other PC:** [TSP_PC_FIELD_PACK.md](TSP_PC_FIELD_PACK.md) · `scripts/stage-tsp-field-pack.ps1`

**Meeting Q&A:** [../meetings/2026-10-08_Lab_Conformance_Questionnaire.md](../meetings/2026-10-08_Lab_Conformance_Questionnaire.md)

**Do not commit** filled documents that contain customer PKI paths, live credentials, or NDA firmware hashes until redacted; use `lab/conformance/drafts/` (gitignored when added) for work-in-progress exports.

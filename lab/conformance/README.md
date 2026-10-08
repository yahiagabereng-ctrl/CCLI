# Lab conformance documents (UCA / IEC 61850-10)

Templates from the accredited lab (2026-10-08 email) live at repo root under `lab/`:

| File | Document | Normative basis |
|------|----------|-----------------|
| `lab/TemplatePics_Ed1Ed2Ed2p1_Excel_rev3p0.xlsx` | **PICS** (Protocol Implementation Conformance Statement) | IEC 61850-7-2 Annex A · UCAIug template rev 3.0 |
| `lab/TemplateMICS_Ed2_FromTP2.0.5.docx` | **MICS** (Model Implementation Conformance Statement) | ICD/CID + logical model |
| `lab/TemplateTICS_Ed2_FromTP2.0.5.docx` | **TICS** (TISSUES Implementation Conformance Statement) | UCA TISSUES database |
| *(not in repo yet)* | **PIXIT** (Protocol Implementation eXtra Information for Testing) | IEC 61850-10 Annex E · lab server PIXIT Word template |

**Working checklist (phases + evidence):** [2026-10-08_UCA_Document_Phase_Checklist.md](2026-10-08_UCA_Document_Phase_Checklist.md)

**Lab templates vs accredited test scope:** [LAB_TEMPLATES_VS_TEST_SCOPE.md](LAB_TEMPLATES_VS_TEST_SCOPE.md) (`ccli-lab-templates-test-scope`)

**Full bench HW (LAN1+LAN2+LAN3 + USB RS485):** [TSP_PC_FULL_BENCH_HW.md](TSP_PC_FULL_BENCH_HW.md) (`ccli-tsp-full-bench-hw`)

**Test Suite Pro — which tests to run after Connect:** [../evidence/testsuite-pro/TSP_TEST_IDENTIFICATION_POST_CONNECT.md](../evidence/testsuite-pro/TSP_TEST_IDENTIFICATION_POST_CONNECT.md)

**Lab requirement ↔ TSP test matrix (field checklist):** [LAB_REQUIREMENT_TEST_MATRIX.md](LAB_REQUIREMENT_TEST_MATRIX.md) · **Push to other PC:** [TSP_PC_FIELD_PACK.md](TSP_PC_FIELD_PACK.md) · `scripts/stage-tsp-field-pack.ps1`

**Meeting Q&A:** [../meetings/2026-10-08_Lab_Conformance_Questionnaire.md](../meetings/2026-10-08_Lab_Conformance_Questionnaire.md)

**Do not commit** filled documents that contain customer PKI paths, live credentials, or NDA firmware hashes until redacted; use `lab/conformance/drafts/` (gitignored when added) for work-in-progress exports.

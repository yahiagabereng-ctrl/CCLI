# E2E input inventory after repository search

Date: 2026-10-21. Purpose: narrow the external-input request after checking both `CCLI Merged` and sibling `CCLI`, including ignored/hidden reference files and relevant ZIP member names. Private key contents were not read or extracted.

## Already available

| Input | Located evidence | Conclusion |
|---|---|---|
| Implementation standard edition | Sibling `CCLI/Normativa/IEC 62351-4 2020.pdf`; extract `knowledge-base/08-engineering/CCI_62351-4_Extract.md` | 2018+AMD1:2020 consolidated material is already local and was used in the detailed review. No need to resend this same export. Source issues still require resolution; another filename alone would not resolve them. |
| Machine-readable E2E ASN.1/XSD components | `apps/ccli/adapters/iec61850_mms/e2e/spec/iec62351-4-ed1.1/`; downloaded archive SHA-256 `4C0BC053CAFE7EE6B6CE13FEF3DD9BA02F400B452F162D92BA056B14BD241FCC` | The SEK Ed. 1.1 companion ZIP and extracted files are now in the repo. `IECManifest.xml` identifies the normative full component as publication IEC 62351-4:2018 (2018-04-19); confirm its applicability to the consolidated Ed. 1.1/amendment and independently decode vectors before freezing the wire map. |
| E2E test specification | Sibling `CCLI/Normativa/IEC TS 62351-100-4 2023.pdf` and the user's Desktop copy | Already reviewed. Missing/unloaded pages in the exports are documented in SOURCE_ISSUES.md. |
| Related standards and product references | Sibling `Normativa/`: 62351-100-3, 62351-8, CEI 0-16; repository engineering extracts for 62351-3/4/6/8/9 and IEC 61850-8-1; Annex T extracts | Enough reference context to proceed with engineering preparation; extracts are not substitutes for authoritative missing/corrected wire definitions. |
| TSP historical version | `knowledge-base/08-engineering/CCI_TestSuitePro_Extract.md` identifies v4.7.4; offline-help origin and `lab/evidence/testsuite-pro/offline-scl/TSP_OFF_SCLVERIFY_2026-09-26_r2_TRIAGE.md` identify **4.7.4.5037** | User does not need to supply the historical version. This is recorded bench evidence, not a live inventory of the currently installed application. |
| TSP manual | `knowledge-base/08-engineering/testsuite-pro/help-offline/`; connection-security topic describes MMS certificates and TLS/TLS RSA | Native E2E capability is not established. No E2E/SecPDU/HandshakeReq/ClearTransfer references were found in the searched help topics/raw text. Absence from these docs does not prove every TSP release lacks the feature. |
| TSP licensing history | `handoff/masomeh95/TSP_LICENSE_AND_CID.md` and evidence README | An expired trial is recorded for 2026-10-01. Later connection evidence exists. Neither establishes current license validity or native-E2E entitlement; do not present the historical expiry as a confirmed current blocker. |
| Intended laboratory/product scope | `lab/meetings/2026-10-08_Lab_Conformance_Questionnaire.md`, especially 7.1-7.7 | Our stated position already selects native E2E with Cert B. The lab-answer cells are blank; this is a questionnaire, not an approved lab response or agreed interpretation. |
| Existing transport PID | `lab/conformance/D7_62351-3_PID_DRAFT.md` | TLS 1.2 / 62351-3 draft exists and explicitly separates the 62351-4/100-4 PID. Its historical missing-PDF statement is stale and is not evidence that today's local test PDF is absent. |

## Not located in the searched trees/archives

1. **An additional complete/corrected authoritative source or corrigendum resolving S02 and the amendment applicability of the code component.** The consolidated export, companion module and extracts are now local; no separately named corrigendum/errata or alternative E2E PDF was found in the searched roots.
2. **A completed lab/DSO answer on the E2E test procedure and source conflicts.** The questionnaire records questions and our proposed position, with empty lab-answer fields. No completed response was located in the searched meeting/handoff material.
3. **Confirmed independent native-E2E test equipment/access.** No native-capable client evaluation package, E2E tool entitlement or demonstrated native E2E exchange was located. Existing TSP/ACSE tests cannot establish this capability.

These are search findings, not proof that the material does not exist outside the checked locations or under an unrecognizable filename.

## Revised request to the user

No additional upload of the ASN.1/XSD package is needed. Do not request the standards already present or the recorded TSP version again. Remaining useful external inputs are authoritative clarification of code-component applicability/amendment scope, a completed lab response if one exists elsewhere, and evidence/access for an independent native E2E peer. Missing Table 21 matters for conditional encryption testing; missing XMPP Table 77 is outside the current OSI server scope.

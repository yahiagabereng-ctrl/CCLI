# CCLI RAG source index — product deployment P0

**Document ID:** CCLI-RAG-INDEX-001  
**Revision:** 1.6  
**Date:** 2026-10-21  
**Matrix:** rev **1.4** — **U-CYB-1003/1004-*** vs `ccli-62351-100-3-extract` / `ccli-62351-100-4-extract` · Phase **0a** **U-P0-09…11**
**RAG source_id:** `ccli-rag-source-index`  
**Registry (machine):** `scripts/rag-product-deploy-sources.json`  
**Parents:** `ccli-knowledge-base-brief` · `ccli-product-sw-freeze` · `ccli-product-deployment-init`

Canonical map of **source_id → repo path → Telematry mirror** for final **product cyber** deployment and DNV/UCA prep.

---

## Initialize RAG (product deployment)

From repo root (`C:\Yahia\projects\CCLI`):

```powershell
# 1) Mirror repo → Telematry documents (no Docker)
powershell -File scripts\sync-ccli-corpus-to-telematry.ps1

# 2) Sync + enqueue product-deploy P0 jobs (RAG up on localhost:8001)
powershell -File scripts\ingest-product-deploy-rag.ps1

# 3) IEC TS 62351-100-4 OCR (priority cyber)
powershell -File scripts\ingest-62351-100-4.ps1

# 4) Full programme ingest (bulk + legacy priority + registry merge)
powershell -File scripts\ingest-next-phases.ps1 -SkipDocker

# UCA IUG tester-accreditation drop (need vs implement)
powershell -File scripts\ingest-uca-61850.ps1

# Finalize (product + accredited/TSP test requirements)
powershell -File scripts\ingest-product-deploy-rag.ps1
powershell -File scripts\ingest-next-phases.ps1 -SkipDocker
powershell -File scripts\ingest-62351-100-4.ps1 -SkipSync
```

**Probe examples (before matrix Phase 1):** `ccli-lab-unified-cert-test-matrix` · `ccli-lab-unified-session-pin-2026-10-14` · `ccli-pc1-log-capture-requirements` · `ccli-lab-unified-tsp-5ln-execution` · `ccli-product-specimen-5ln` · `ccli-rag-source-index`

---

## Product deployment cluster (query these first)

| source_id | Repo path | Role |
|-----------|-----------|------|
| `ccli-product-deployment-init` | `lab/conformance/PRODUCT_DEPLOYMENT_INIT.md` | Init checklist |
| `ccli-product-sw-freeze` | `knowledge-base/08-engineering/CCI_Product_SW_Freeze.md` | Lab-first → `releases/product-cyber/` |
| `ccli-product-cyber-release` | `releases/product-cyber/README.md` | Bundle layout |
| `ccli-app-structure` | `knowledge-base/06-application/CCI_Application_Structure.md` | Code + config layout |
| `ccli-openwrt-freeze` | `knowledge-base/08-engineering/CCI_OpenWrt_Freeze.md` | OS pin |
| `ccli-soc-freeze` | `knowledge-base/08-engineering/CCI_SOC_Freeze.md` | Silicon pin |
| `ccli-tg500-lab-platform` | `knowledge-base/08-engineering/CCI_TG500_Lab_Platform.md` | HW / ports |
| `ccli-lab-hw-configuration-index` | `lab/HW_CONFIGURATION_INDEX.md` | Lab HW docs index + conflict table |

---

## Test requirements cluster (`test-req` in registry)

| source_id | Repo path | Role |
|-----------|-----------|------|
| `ccli-lab-unified-cert-test-matrix` | `LAB_UNIFIED_CERT_TEST_MATRIX.md` + `.csv` | **Single □** functional + cyber + O.15 · **finalize clauses** |
| `ccli-lab-unified-session-pin-2026-10-14` | `LAB_UNIFIED_SESSION_PIN_2026-10-14.md` | r53 deploy pin · **U-P0-11** |
| `ccli-pc1-log-capture-requirements` | `handoff/masomeh95/PC1_LOG_CAPTURE_REQUIREMENTS.md` | PC-B logger · **U-P0-10** |
| `ccli-pc1-github-sync-network` | `handoff/masomeh95/PC1_GITHUB_SYNC_AND_NETWORK.md` | Owner `yahiagabereng-ctrl/CCLI` push · PC1 pull · hotspot |
| `ccli-pc1-other-device-setup` | `handoff/masomeh95/PC1_OTHER_DEVICE_SETUP.md` | Second PC: clone, auth, USB pack, no secrets in git |
| `ccli-share-via-github` | `handoff/masomeh95/00_SHARE_VIA_GITHUB.md` | **One-page** clone URL + pull commands for other PC |
| `ccli-pc1-product-5ln-cursor` | `handoff/masomeh95/PC1_PRODUCT_5LN_CURSOR_SETUP.md` | PC1 Cursor + `product/apps/ccli` 5-LN setup |
| `ccli-lab-unified-cert-test-roadmap` | `LAB_UNIFIED_CERT_TEST_ROADMAP.md` | Two-PC phase order |
| `ccli-lab-unified-tsp-5ln-execution` | `LAB_UNIFIED_TSP_5LN_EXECUTION.md` | TestPro logger map · 5-LN capture |
| `ccli-product-specimen-5ln` | `PRODUCT_SPECIMEN_5LN.md` | 5-LN product pin |
| `ccli-annex-o-software-matrix` | `ANNEX_O_SOFTWARE_MATRIX.md` | Short index → OT matrix |
| `ccli-annex-ot-sw-gap-matrix` | `ANNEX_OT_SW_GAP_MATRIX.md` | Annex O+T → SW → 5-LN → Phase (67×6) |
| `ccli-normative-cert-validation-matrix` | `Full Validate.md` | **All** O/M/T/U + O.15 #1–8 clause book |
| `ccli-product-specimen-5ln-validation` | `PRODUCT_SPECIMEN_5LN_VALIDATION.md` | Compare/Browse audit |
| `ccli-p5-fullcircle-5ln-scope` | `P5_FULLCIRCLE_5LN_SCOPE.md` | Annex T 5-LN only |
| `ccli-tsp-two-pc-bench-runbook` | `TSP_TWO_PC_BENCH_RUNBOOK.md` | Two-PC execution |
| `ccli-lab-testpro-pc1-config` | `lab/testpro/README.md` + `PC1_TSP_IMPORT_CHECKLIST.md` | PC1 TSP CID/TLS import |
| `ccli-tsp-evidence-per-test-protocol` | `lab/evidence/testsuite-pro/EVIDENCE_PER_TEST_PROTOCOL.md` | txt + pcap bundle |
| `ccli-lab-requirement-test-matrix` | `LAB_REQUIREMENT_TEST_MATRIX.md` | REQ ↔ TSP ↔ 61850-10 |
| `ccli-lab-pics-fill` | `PICS_FILL.md` | D1 PICS values |
| `ccli-lab-pics-filled-workbook` | `PICS_FILLED_WORKBOOK.md` | Filled xlsx SHA + alignment |
| `ccli-lab-pixit-draft` | `PIXIT_DRAFT.md` | D2 PIXIT |
| `ccli-lab-mics-draft` | `MICS_DRAFT.md` | D3 MICS |
| `ccli-lab-cid-pics-alignment` | `CID_PICS_ALIGNMENT.md` | Services trim |
| `ccli-lab-templates-test-scope` | `LAB_TEMPLATES_VS_TEST_SCOPE.md` | Template vs accredited scope |
| `ccli-lab-uca-document-checklist` | `2026-10-08_UCA_Document_Phase_Checklist.md` | D1–D7 |
| `ccli-61850-10-extract` | `CCI_61850-10_Extract.md` | Abstract test groups |
| `ccli-uca-61850-accreditation-extract` | `CCI_UCA_61850_Accreditation_Extract.md` | UCA tester-accred pack vs IED CBs we implement |
| `ccli-62351-100-3-extract` | `CCI_62351-100-3_Extract.md` | Accredited TLS cases |
| `ccli-tsp-test-identification` | `lab/evidence/.../TSP_TEST_IDENTIFICATION_POST_CONNECT.md` | Tier 1–4 steps |
| `ccli-lab-requirement-test-matrix-cyber` | `LAB_REQUIREMENT_TEST_MATRIX_CYBER.md` | 62351 / D7 / Tier 3 cyber checklist |
| `ccli-62351-8-extract` | `CCI_62351-8_Extract.md` | Viewer (**U-T3-02/03**) |
| `ccli-62351-9-extract` | `CCI_62351-9_Extract.md` | CRL (**U-T3-04**) |
| `ccli-phase-regulation-checklists` | `CCI_Phase_Regulation_Checklists.md` | IEC 104 (**U-P4-***) |
| `ccli-61557-12-extract` | `CCI_61557-12_Extract.md` | **U-ACC-04** programme |
| `ccli-tespro-cert-reply-2026-10` | `CCI_TesPro_Cert_Reply_2026-10_Extract.md` | TesPro 2026-10-21 vendor pack · O.15 #2/#3/#8 split |
| `ccli-61557-12-tespro-vendor-map` | `lab/conformance/61557-12_TESPRO_VENDOR_MAP.md` | TesPro asks vs 61557-12 align only |
| `ccli-61557-12-clauses-vs-tespro` | `lab/conformance/61557-12_CLAUSES_VS_TESPRO.md` | Ed 2.0 clauses vs TesPro Scope respond |
| `ccli-tespro-cnas-l5868-62368-scope` | `CCI_TesPro_CNAS_L5868_62368_Scope_Extract.md` | CNAS L5868 unfolding p.1 — ICT 62368-1 |
| `ccli-61557-12-2018-scan` | `CCI_61557-12_2018_Scan.md` | Full OCR 2018 Ed 2.0 — no EPMF |
| `ccli-61557-12-2018-ed2-extract` | `CCI_61557-12_2018_Ed2_Extract.md` | HQ clause extract of Desktop 2018 PDF |
| `ccli-62443-4-2-extract` | `CCI_62443-4-2_Extract.md` | **U-ACC-05** programme |
| `cei-0-16-consolidata-2025-12` | `CCI_CEI_0-16_Consolidated.md` | **U-ACC-06** programme |

---

## Lab specimen + TSP zero-error cluster

| source_id | Repo path | Role |
|-----------|-----------|------|
| `ccli-lab-specimen-version` | `lab/conformance/LAB_SPECIMEN_VERSION.md` | CID rev 9 · cfg SHA |
| `ccli-meter-specimen-emt432-v-full` | `lab/conformance/METER_SPECIMEN_EMT432_V_FULL.md` | EMT432.V.FULL + FRC-350-100mV · 85 vs 100 mV |
| `ccli-chronos-emt432-vendor-requirements` | `knowledge-base/08-engineering/CCI_Chronos_EMT432_Vendor_Requirements.md` (canonical copy also `lab/conformance/CHRONOS_EMT432_VENDOR_REQUIREMENTS.md`) | Chronos certificates + REQ-CHR · O.15 #2 vendor ask |
| `ccli-chronos-sta-test-reports` | `knowledge-base/08-engineering/CCI_Chronos_STA_Test_Reports_Extract.md` | S.T.A. C25305/C25306 ingest extract |
| `ccli-chronos-sta-annex-map` | `lab/conformance/CHRONOS_STA_REPORTS_ANNEX_MAP.md` | STA reports → Annex O / 61557 annex map |
| `ccli-lab-dut-finalize-cert-path` | `lab/conformance/DUT_LAB_FINALIZE_AND_CERT_PATH.md` | TSP vs UCA certificate |
| `ccli-lab-pics-fill` | `lab/conformance/PICS_FILL.md` | PICS M/N/A |
| `ccli-lab-cid-pics-alignment` | `lab/conformance/CID_PICS_ALIGNMENT.md` | Services trim |
| `ccli-lab-mics-draft` | `lab/conformance/MICS_DRAFT.md` | Model ICS |
| `ccli-lab-pixit-draft` | `lab/conformance/PIXIT_DRAFT.md` | PIXIT values |
| `ccli-lab-requirement-test-matrix` | `lab/conformance/LAB_REQUIREMENT_TEST_MATRIX.md` | REQ ↔ TSP |
| `ccli-testsuite-pro-procedure` | `lab/CCI_TestSuitePro_Verification_Layer.md` | TSP paths |
| `ccli-tsp-compare-waivers` | `lab/evidence/testsuite-pro/inbox/TSP_P5_COMPARE_WAIVERS.md` | Compare waivers |

---

## Cyber + DNV cluster

| source_id | Repo path | Role |
|-----------|-----------|------|
| `ccli-lab-requirement-test-matrix-cyber` | `lab/conformance/LAB_REQUIREMENT_TEST_MATRIX_CYBER.md` | **62351 test matrix** · P3-06…09 |
| `ccli-61850-10-extract` | `knowledge-base/08-engineering/CCI_61850-10_Extract.md` | Conformance · DNV Q§10 |
| `ccli-62351-3-extract` | `knowledge-base/08-engineering/CCI_62351-3_Extract.md` | TLS transport |
| `ccli-62351-4-extract` | `knowledge-base/08-engineering/CCI_62351-4_Extract.md` | MMS ACSE |
| `ccli-62351-100-3-extract` | `knowledge-base/08-engineering/CCI_62351-100-3_Extract.md` | Accredited TLS cases |
| `ccli-62351-100-4-extract` | `knowledge-base/08-engineering/CCI_62351-100-4_Extract.md` | MMS security planning + ingest status |
| `ccli-62351-100-4-ocr-corpus` | `reference/iec62351/IEC_TS_62351-100-4_2023_full.txt` | **115 pp. OCR** (2023) |
| `ccli-62351-100-4-ocr-index` | `CCI_62351-100-4_OCR_Corpus.md` | Corpus index |
| `ccli-lab-d7-62351-3-pid` | `lab/conformance/D7_62351-3_PID_DRAFT.md` | D7 PID |
| `ccli-certificate-samples-extract` | `knowledge-base/08-engineering/CCI_Certificate_Samples_Extract.md` | DNV UCA sample |
| `ccli-dnv-product-readiness` | `lab/conformance/dnv/DNV_UCA_PRODUCT_READINESS.md` | O.15 stack checklist |
| `ccli-lab-uca-document-checklist` | `lab/conformance/2026-10-08_UCA_Document_Phase_Checklist.md` | D1–D7 |

---

## Known RAG gaps (corpus MISSING — not ingest)

| Gap | Mitigation |
|-----|------------|
| **62351-100-4** table rows in extract | OCR **HAVE**; batch Table 2–4 into extract **PART** |
| Licensed IEC PDF (non-doc88) | Prefer IEC webshop copy for accredited submit |
| **Annex U** extract | Operating Rule timers — see `ccli-normative-cert-validation-matrix` §D |
| **PIXIT** Word template | Paste from `PIXIT_DRAFT.md` |
| **5-LN product CID** in `apps/ccli/config/icd/product/` | Block **U-T1-02** until committed; pin: `ccli-product-specimen-5ln` |
| TesPro vendor OpenWrt SDK | Upstream SDK in `lab/tg544-openwrt/` |
| TesPro ISO 9001 PDF | Claimed in reply; **not** in 2026-10 drop |
| TesPro TG544 EU CE / EMC / LVD | Drop has CNAS/A2LA **lab** cards only |

---

## Maintenance

When adding a doc with **RAG source_id** in front matter:

1. Add row to this index (human).  
2. Add entry to `scripts/rag-product-deploy-sources.json` if product/TSP/DNV relevant.  
3. Re-run `ingest-product-deploy-rag.ps1` or full `ingest-next-phases.ps1`.

**Keywords:** `RAG`, `product-deploy`, `ccli-rag-source-index`, `Telematry`, `ingest`

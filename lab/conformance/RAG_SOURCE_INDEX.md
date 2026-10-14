# CCLI RAG source index — product deployment P0

**Document ID:** CCLI-RAG-INDEX-001  
**Revision:** 1.2  
**Date:** 2026-10-14  
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

# Finalize (product + accredited/TSP test requirements)
powershell -File scripts\ingest-product-deploy-rag.ps1
powershell -File scripts\ingest-next-phases.ps1 -SkipDocker
powershell -File scripts\ingest-62351-100-4.ps1 -SkipSync
```

**Probe examples:** `ccli-product-sw-freeze` · `ccli-lab-requirement-test-matrix` · `ccli-lab-requirement-test-matrix-cyber` · `ccli-lab-pics-fill` · `ccli-lab-dut-finalize-cert-path` · `ccli-dnv-product-readiness`

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

---

## Test requirements cluster (`test-req` in registry)

| source_id | Repo path | Role |
|-----------|-----------|------|
| `ccli-lab-requirement-test-matrix` | `LAB_REQUIREMENT_TEST_MATRIX.md` | REQ ↔ TSP ↔ 61850-10 |
| `ccli-lab-pics-fill` | `PICS_FILL.md` | D1 PICS values |
| `ccli-lab-pixit-draft` | `PIXIT_DRAFT.md` | D2 PIXIT |
| `ccli-lab-mics-draft` | `MICS_DRAFT.md` | D3 MICS |
| `ccli-lab-cid-pics-alignment` | `CID_PICS_ALIGNMENT.md` | Services trim |
| `ccli-lab-templates-test-scope` | `LAB_TEMPLATES_VS_TEST_SCOPE.md` | Template vs accredited scope |
| `ccli-lab-uca-document-checklist` | `2026-10-08_UCA_Document_Phase_Checklist.md` | D1–D7 |
| `ccli-61850-10-extract` | `CCI_61850-10_Extract.md` | Abstract test groups |
| `ccli-62351-100-3-extract` | `CCI_62351-100-3_Extract.md` | Accredited TLS cases |
| `ccli-tsp-test-identification` | `lab/evidence/.../TSP_TEST_IDENTIFICATION_POST_CONNECT.md` | Tier 1–4 steps |
| `ccli-lab-requirement-test-matrix-cyber` | `LAB_REQUIREMENT_TEST_MATRIX_CYBER.md` | 62351 / D7 / Tier 3 cyber checklist |

---

## Lab specimen + TSP zero-error cluster

| source_id | Repo path | Role |
|-----------|-----------|------|
| `ccli-lab-specimen-version` | `lab/conformance/LAB_SPECIMEN_VERSION.md` | CID rev 9 · cfg SHA |
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
| **PIXIT** Word template | Paste from `PIXIT_DRAFT.md` |
| **5-LN product CID** in `apps/ccli/config/icd/product/` | Future trim; update specimen doc |
| TesPro vendor OpenWrt SDK | Upstream SDK in `lab/tg544-openwrt/` |

---

## Maintenance

When adding a doc with **RAG source_id** in front matter:

1. Add row to this index (human).  
2. Add entry to `scripts/rag-product-deploy-sources.json` if product/TSP/DNV relevant.  
3. Re-run `ingest-product-deploy-rag.ps1` or full `ingest-next-phases.ps1`.

**Keywords:** `RAG`, `product-deploy`, `ccli-rag-source-index`, `Telematry`, `ingest`

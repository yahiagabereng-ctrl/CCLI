# CCI Knowledge Base — Master Download Index

**Document ID:** CCLI-KB-DL-001  
**Revision:** 2.1  
**Date:** 2026-09-21  
**Root on disk:** `c:\Yahia\projects\CCLI\knowledge-base\`  
**Telematry mirror:** `C:\Yahia\projects\Telematry System\documents\projects\ccli\`

Lab platform: **OpenWrt on TesPro TG-524**.

---

## 1. Lab platform (`08-engineering/`)

| Local path | RAG `source_id` | Status |
|------------|-----------------|--------|
| `08-engineering/CCI_SOC_Freeze.md` | `ccli-soc-freeze` | **FROZEN** |
| `08-engineering/CCI_TG500_Lab_Platform.md` | `ccli-tg500-lab-platform` | **FROZEN** |
| `08-engineering/TG-500_Series_Datasheet.pdf` | `ccli-tespro-tg500-ds` | ON DISK |
| `08-engineering/TesPro_Gateway_Specifications_Response_Form.pdf` | `ccli-tespro-tg424-response` | ON DISK |
| `08-engineering/reference/vendor/tespro/IEC61850-User-Manual.docx` | `ccli-tespro-61850-manual` | **INGESTED** 2026-09-29 |
| `08-engineering/CCI_TesPro_61850_Manual_Extract.md` | `ccli-tespro-61850-manual-extract` | **HAVE** |
| `08-engineering/CCI_TesPro_61850_Capture_Plan.md` | `ccli-tespro-61850-capture-plan` | **HAVE** |

Ingest TesPro 61850 manual: `powershell -File scripts/ingest-tespro-61850-manual.ps1`
| `08-engineering/CCI_Validation_and_Mocking_Strategy.md` | `ccli-validation-strategy` | INGESTED |
| `08-engineering/CCI_Mocking_Bench_BOM.md` | `ccli-mocking-bench-bom` | INGESTED |
| `08-engineering/CCI_Architecture_Framework.md` | `ccli-architecture-framework` | INGESTED |
| `08-engineering/CCI_Competitors_Index.md` | `ccli-competitors-index` | **HAVE** |
| `08-engineering/CCI_Competitors_HW_Extract.md` | `ccli-competitors-hw-extract` | **HAVE** |
| `08-engineering/CCI_Competitors_HW_Validation.md` | `ccli-competitors-hw-validation` | **HAVE** |
| `../competitors/competitors/` | (raw PDFs) | ON DISK — AiLux, Higeco, MC, STCE, Tesmec |
| `../competitors/_extracts/` | (text harvest) | ON DISK — `scripts/extract-competitors.py` |

| `08-engineering/CCI_Phase_Regulation_Checklists.md` | `ccli-phase-regulation-checklists` | INGESTED |
| `08-engineering/CCI_TestSuitePro_Capture_Plan.md` | `ccli-testsuite-pro-capture-plan` | **HAVE** |
| `08-engineering/CCI_TestSuitePro_Extract.md` | `ccli-testsuite-pro-extract` | **HAVE** |
| `08-engineering/testsuite-pro/raw/` | `ccli-testsuite-pro-raw` | **HAVE** (web harvest 2026-09-26) |
| `08-engineering/testsuite-pro/help-offline/` | `ccli-testsuite-pro-help` | **HAVE** (mirrored from PC install v4.7.4.5037) |
| `../lab/CCI_TestSuitePro_Verification_Layer.md` | `ccli-testsuite-pro-procedure` | **HAVE** |
| `../lab/evidence/testsuite-pro/` | `ccli-testsuite-pro-lab-logs` | inbox for TSP exports |

Download / ingest Test Suite Pro:
`powershell -File scripts/download-testsuite-pro-docs.ps1` · `powershell -File scripts/ingest-testsuite-pro.ps1`  
Eval installer (manual, do not commit): https://trianglemicroworks.com/products/downloads

Ingest TG-500: `Telematry System/scripts/ingest-tg500-platform.ps1`

### OpenWrt procedural (K2.3–K2.7)

| Local path | Upstream | RAG `source_id` | Status |
|------------|----------|-------------------|--------|
| `08-engineering/CCI_OpenWrt_Capture_Plan.md` | authored | `ccli-openwrt-capture-plan` | **HAVE** |
| `08-engineering/CCI_OpenWrt_Procedural_Extract.md` | authored extract | `ccli-openwrt-procedural-extract` | **HAVE (P0)** |
| `08-engineering/CCI_OpenWrt_Buildsystem_Extract.md` | live wiki + archive | `ccli-openwrt-buildsystem-extract` | **HAVE (P0)** |
| `08-engineering/CCI_OpenWrt_Dependencies_Extract.md` | openwrt.org wiki | `ccli-openwrt-dependencies-extract` | **HAVE (P0)** |
| `08-engineering/openwrt-sources/raw_dependencies.txt` | oldwiki / openwrt.org | (harvest) | ON DISK |
| `08-engineering/openwrt-sources/raw_build-usage.txt` | oldwiki archive | (harvest) | ON DISK |
| `08-engineering/openwrt-sources/raw_raspberry_pi.txt` | oldwiki archive | (harvest) | ON DISK |
| `08-engineering/openwrt-sources/raw_procd-init-scripts.txt` | oldwiki archive | (harvest) | ON DISK |
| `08-engineering/openwrt-sources/raw_packages.txt` | oldwiki archive | (harvest) | ON DISK |
| `08-engineering/openwrt-sources/github_openwrt_procd.sh` | github.com/openwrt/openwrt | (harvest) | ON DISK |
| `08-engineering/openwrt-sources/github_openwrt-packages_libgpiod_Makefile` | github.com/openwrt/packages | (harvest) | ON DISK |

Refresh raw files: `bash scripts/download-openwrt-wiki.sh` (WSL/Linux).  
Note: `openwrt.org` direct curl returns Anubis bot challenge — use archive + GitHub only.

Re-ingest: `Telematry System/scripts/ingest-ccli.ps1 -ProjectOnly`

---

## 2. Protocol libraries (`07-protocols/`)

| Local path | Upstream | RAG `source_id` |
|------------|----------|-----------------|
| `07-protocols/0-16consolidata.pdf` | CEI 0-16 consolidated 2025-12 | `cei-0-16-consolidata-2025-12` |
| `08-engineering/CCI_CEI_0-16_Consolidated.md` | authored index (English) | `cei-0-16-consolidata-2025-12` |
| `08-engineering/CCI_Annex_O_Extract.md` | English extract + V5 deltas | `cei-0-16-allegato-o` |
| `08-engineering/CCI_Annex_T_Extract.md` | English extract + TR 57-126 note | `cei-0-16-allegato-t` |
| `07-protocols/cei-tr-57-126.pdf` | CEI TR 57-126 SCL/CID example (34 pp.) | `cei-tr-57-126` |
| `08-engineering/CCI_TR_57-126_Extract.md` | English engineering extract + CID map | `cei-tr-57-126` |
| `apps/ccli/config/icd/cei-tr-57-126-example.cid` | Annex A reference CID | `cei-tr-57-126` |
| `08-engineering/CCI_61850-6_Capture_Plan.md` | SCL ingest batches (61850-6) | `ccli-61850-6-capture-plan` |
| `08-engineering/CCI_61850-6_Extract.md` | English extract (pp. 33–45) | `ccli-61850-6-extract` |
| `07-protocols/arera-540-2021.pdf` | ARERA 540/2021/R/EEL | `arera-540-2021` |
| `08-engineering/CCI_ARERA_540_2021_Extract.md` | English engineering extract | `arera-540-2021` |
| `07-protocols/CCI_GitHub_Protocol_Libraries.md` | authored | `ccli-github-protocol-libs` |
| `07-protocols/github-refs/libiec61850.md` | github.com/mz-automation/libiec61850 | (harvest) |
| `07-protocols/github-refs/lib60870.md` | github.com/mz-automation/lib60870 | (harvest) |
| `07-protocols/CCI_Module_Regulations_Classification.md` | authored | `ccli-cci-module-regs-class` |
| `07-protocols/CCI_Standards_PDF_Inventory.md` | authored (2026-09-18 drop) | `ccli-standards-pdf-inventory` |
| `07-protocols/regulations/README.md` | junction pointer | — |
| `07-protocols/regulations/standards-drop-20260918/` | **35 licensed PDFs** (junction) | see inventory §1 |

### 2.1 Standards PDF drop (2026-09-18) — on disk, not in git

**Physical:** `C:\Yahia\projects\CCLI\Standards_pdf-20260918T124321Z-1-001\Standards_pdf`  
**Corpus junction:** `07-protocols/regulations/standards-drop-20260918/`  
**Index:** `07-protocols/CCI_Standards_PDF_Inventory.md`

| Priority | PDF in drop | Engineering extract | Capture plan |
|----------|-------------|---------------------|--------------|
| P0 | 61850-6, 61850-7-2, **61850-7-3**, **61850-7-4**, **61850-8-1**, 62351-3/4/6, 62443-3-2/3-3/4-1/4-2 | 61850-6/7-2/7-3/7-4 **HAVE**; **8-1 PART** (2004 OCR); 62351-3/4/6 **HAVE**; 62443 **HAVE** | 61850-6/7-2/7-3/7-4/8-1 capture **COMPLETE** |
| P0 gap | **61850-8-1:2011**, **62351-9/14** PDFs | 62351-9/14 extracts **HAVE** | — |
| P1 | 60870-5-104, 62351-5/7/8, 61850-5, 61557-12 | 61850-5/6/7-2, 104, 62351-5/7 **HAVE**; 62351-8 **PART**; 61557 **PART** | capture plans **COMPLETE** |
| Broken | 61131-3 only | 61557-12 PDF **OK** in drop | re-download **61131-3** |

**Ingest:** `scripts/sync-ccli-corpus-to-telematry.ps1` then Telematry `ingest-ccli.ps1 -ProjectOnly` (requires RAG on `:8001`).

**Still missing (P0 adjacent):** DSO-specific ICD workbook (lab `signal_map.yaml` MVP **HAVE** 2026-09-25); ARERA 385/2025 full text; **61850-8-1:2011 Ed.2** PDF; **62351-9/14** licensed PDFs (extracts **HAVE**).

---

## 3. Repo-root docs (Telematry `projects/ccli/`)

| Path | RAG `source_id` |
|------|-----------------|
| `knowledge-base/README.md` | `ccli-knowledge-base-brief` |
| `CCI_Project_Roadmap.md` | `ccli-project-roadmap` |
| `CCI_3Month_Italy_NoFab_Plan.md` | `ccli-3m-italy-nofab-plan` |

---

## 4. Re-ingest

```powershell
cd "C:\Yahia\projects\Telematry System"
.\scripts\ingest-tg500-platform.ps1
.\scripts\ingest-ccli.ps1 -ProjectOnly
```

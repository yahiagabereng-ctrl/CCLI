# Triangle MicroWorks 61850 Test Suite Pro — Capture Plan

**Document ID:** CCLI-LAB-TSP-CAP-001  
**Revision:** 1.0  
**Date:** 2026-09-26  
**RAG source_id:** `ccli-testsuite-pro-capture-plan` → `ccli-testsuite-pro-extract`  
**Vendor:** Triangle MicroWorks — product **61850 Test Suite Pro** (≠ TesPro OEM)  
**Lab procedure:** `lab/CCI_TestSuitePro_Verification_Layer.md`  
**Disk root:** `knowledge-base/08-engineering/testsuite-pro/`

---

## Paths (authoritative)

| Role | Absolute / repo path |
|------|----------------------|
| Raw web harvest | `knowledge-base/08-engineering/testsuite-pro/raw/` |
| Offline Online Help drop | `knowledge-base/08-engineering/testsuite-pro/help-offline/` |
| Engineering extract | `knowledge-base/08-engineering/CCI_TestSuitePro_Extract.md` |
| Evidence inbox (your logs) | `lab/evidence/testsuite-pro/inbox/` |
| Offline SCL exports | `lab/evidence/testsuite-pro/offline-scl/` |
| CCLI CID under test | `apps/ccli/config/icd/lab_tg544_eth_a.cid` |
| Object freeze | `apps/ccli/config/icd/signal_map.yaml` |
| TLS material | `apps/ccli/config/tls/` |
| Download script | `scripts/download-testsuite-pro-docs.ps1` |
| Ingest script | `scripts/ingest-testsuite-pro.ps1` |

---

## Upstream URLs (download / mirror)

| ID | URL | Local target | Status |
|----|-----|--------------|--------|
| U1 | https://www.trianglemicroworks.com/products/testing-and-configuration-tools/61850-test-suite-pro-pages/overview | `raw/overview_YYYY-MM-DD.txt` | **HAVE** 2026-09-26 |
| U2 | https://www.trianglemicroworks.com/products/testing-and-configuration-tools/61850-test-suite-pro-pages/system-requirements | `raw/system-requirements_*.txt` | **HAVE** |
| U3 | https://www.trianglemicroworks.com/products/testing-and-configuration-tools/61850-test-suite-pro-pages/whats-new | `raw/whats-new_*.txt` | **HAVE** |
| U4 | https://trianglemicroworks.com/help/6tsp + **local install Help** | `help-offline/` | **HAVE** 2026-09-26 — mirrored from `C:\Program Files\Triangle MicroWorks\TMW Test Suite Pro\Help\` |
| U5 | https://trianglemicroworks.com/products/downloads | Eval installer (do **not** commit) | Manual |
| U6 | https://www.trianglemicroworks.com/sales/evals | Eval registration | Manual |
| U7 | Product tool pages (Sequencer, Report Viewer, …) | `raw/tools_*.txt` | Refresh via script |

Refresh: `powershell -File scripts/download-testsuite-pro-docs.ps1`

---

## Capture batches

| Batch | Content | Owner | Status |
|-------|---------|-------|--------|
| **W** | Public web pages U1–U3 | Auto script | **COMPLETE** |
| **H** | Full Online Help HTML (U4) | Lab PC install mirror | **COMPLETE** 2026-09-26 |
| **S** | SCL Verify export vs `lab_tg544_eth_a.cid` | You → `offline-scl/` | **OPEN** |
| **L** | Live session logs (Connect/Operate/Report) | You → `inbox/` when DUT up | **WAIT** |

---

## RAG source_ids

| source_id | File | Ingest |
|-----------|------|--------|
| `ccli-testsuite-pro-capture-plan` | this file | yes |
| `ccli-testsuite-pro-extract` | `CCI_TestSuitePro_Extract.md` | yes |
| `ccli-testsuite-pro-raw` | `testsuite-pro/raw/*.txt` | yes |
| `ccli-testsuite-pro-help` | `help-offline/**` when present | yes |
| `ccli-testsuite-pro-lab-logs` | `lab/evidence/testsuite-pro/**/*.txt` | yes |
| `ccli-testsuite-pro-procedure` | `lab/CCI_TestSuitePro_Verification_Layer.md` | yes |

---

## Offline-first order (no DUT)

1. Run download script (batch **W**).  
2. Install TSP on PC → copy Online Help → batch **H**.  
3. Import CID → SCL Verify → save under `offline-scl/` (batch **S**).  
4. Paste/export logs to `inbox/` → tell Agent → triage + re-ingest.

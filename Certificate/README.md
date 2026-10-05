# CCLI — Certification documentation handoff

**Audience:** Colleague authoring **conformity / certificate documentation** (outside lab phases P0–P7)  
**Date:** 2026-10-02  
**Product:** HiTEKS CCLI · TesPro TG-524 / TG544 class · CEI 0-16 CCI

You **do not** run lab phases. You **write documents** and use this folder plus the RAG corpus below. Engineering owns firmware, lab evidence, and DUT.

---

## 1. Start here

| Step | Action |
|------|--------|
| 1 | Read this file |
| 2 | Read **[CCI_Certificate_Samples_Index.md](CCI_Certificate_Samples_Index.md)** — competitor cert breakdown |
| 3 | Query RAG: `source_id` **`ccli-certificate-samples-extract`** |
| 4 | Query RAG: **`cei-0-16-allegato-o`** §11–12 (O.14 logger + O.15 cert list) |
| 5 | Run ingest if RAG empty: `powershell -File scripts/ingest-certificates.ps1` |

---

## 2. Reference samples in this folder (Higeco — competitor)

These are **reference only**. Do not copy text into HiTEKS certificates.

| File | Certificate type | Lab / scheme |
|------|------------------|--------------|
| [IEC 61850 UCA Standard Group Certificate.pdf](IEC%2061850%20UCA%20Standard%20Group%20Certificate.pdf) | **IEC 61850 Ed.2** server conformance | DNV · **UCA IUG** Level A1 |
| [IEC 62443-4-2 - Isa Secure CSA-E01.pdf](IEC%2062443-4-2%20-%20Isa%20Secure%20CSA-E01.pdf) | **62443-4-1/4-2** component security | **ISASecure CSA** · BYHON (Italy) |
| [IEC-62351-3 Rev1.pdf](IEC-62351-3%20Rev1.pdf) | **62351-3** TLS transport | IMQ/Chrono Milano |
| [DI.CO. QUADRO_ACQUISIZIONE E MISURA.pdf](DI.CO.%20QUADRO_ACQUISIZIONE%20E%20MISURA.pdf) | **Dichiarazione** + rapporto di prova (61439-2 cabinet) | OCR ✓ `_extracts/DI_CO_*_ocr.txt` |
| [CEI 0-16.pdf](CEI%200-16.pdf) | **TÜV Rheinland** CEI 0-16 Annex O/T product cert (Higeco) | OCR ✓ `_extracts/CEI_0-16_ocr.txt` |

Text extracts: [`_extracts/`](_extracts/) · text layer: `python scripts/extract-certificates.py` · **OCR:** `python scripts/ocr-certificates.py`

---

## 3. Certification procedure docs (repo — your primary sources)

### 3.1 What CEI requires (mandatory certificate list)

| Document | Path | RAG `source_id` |
|----------|------|-----------------|
| Annex O extract (O.14 + **O.15.4 cert table**) | `knowledge-base/08-engineering/CCI_Annex_O_Extract.md` | `cei-0-16-allegato-o` |
| Annex T (61850, TLS, PKI) | `knowledge-base/08-engineering/CCI_Annex_T_Extract.md` | `cei-0-16-allegato-t` |
| Annex M (defence / teletrip) | `knowledge-base/08-engineering/CCI_Annex_M_Extract.md` | `cei-0-16-allegato-m` |
| CEI consolidated index | `knowledge-base/08-engineering/CCI_CEI_0-16_Consolidated.md` | `cei-0-16-consolidata-2025-12` |

### 3.2 PICS & transport (fill before any lab RFQ)

| Standard | Extract (PICS tables) | RAG `source_id` |
|----------|----------------------|-----------------|
| IEC 62351-3 TLS | `CCI_62351-3_Extract.md` §8 | `ccli-62351-3-extract` |
| IEC 62351-4 MMS E2E | `CCI_62351-4_Extract.md` §17 | `ccli-62351-4-extract` |
| IEC 62351-8 RBAC | `CCI_62351-8_Extract.md` | `ccli-62351-8-extract` |
| IEC 62351-9 PKI | `CCI_62351-9_Extract.md` §9 | `ccli-62351-9-extract` |
| IEC 62351-14 syslog | `CCI_62351-14_Extract.md` | `ccli-62351-14-extract` |
| IEC 61850-8-1 MMS | `CCI_61850-8-1_Extract.md` | `ccli-61850-8-1-extract` *(Ed.2 gap)* |
| MMS security test map | `CCI_62351_MMS_Security_Debug_Map.md` | `ccli-62351-mms-security-debug-map` |

### 3.3 Cyber SDLC & component (62443)

| Document | Path | RAG `source_id` |
|----------|------|-----------------|
| 62443-4-1 SDL requirements | `CCI_62443-4-1_Extract.md` | `ccli-62443-4-1-extract` |
| 62443-4-2 CR Annex B | `CCI_62443-4-2_Extract.md` | `ccli-62443-4-2-extract` |
| Zones / SL-T | `CCI_62443_Zones.md` | `ccli-62443-zones-extract` |
| Key ceremony SOP | `CCI_K6.3_Key_Ceremony_SOP.md` | `ccli-k63-ceremony-sop` |

### 3.4 Environmental / metrology

| Document | Path | RAG `source_id` |
|----------|------|-----------------|
| CEI EN 61557-12 | `CCI_61557-12_Extract.md` | `ccli-61557-12-extract` |
| Product spec (accuracy, I/O class) | `CCI_CCLI_Product_Spec_and_Roadmap.md` | `ccli-product-spec-roadmap` |

### 3.5 Legal / ARERA

| Document | Path | RAG `source_id` |
|----------|------|-----------------|
| ARERA 540/2021 extract | `CCI_ARERA_540_2021_Extract.md` | `arera-540-2021` |
| **385/2025 + 564/2025** | — | **NOT INGESTED** |

### 3.6 Competitor narrative (claims vs certificates)

| Document | Path | RAG `source_id` |
|----------|------|-----------------|
| Competitor HW + cert claims | `CCI_Competitors_HW_Extract.md` | `ccli-competitors-hw-extract` |
| Higeco compliance FAQ | `competitors/competitors/Higeco/CCI_Compliance_FAQ.html` | `ccli-higeco-compliance-faq` |
| Samples extract (this folder) | `CCI_Certificate_Samples_Extract.md` | `ccli-certificate-samples-extract` |

### 3.7 Lab evidence (read-only annex — do not re-test)

| Document | Path |
|----------|------|
| Evidence master index | `lab/evidence/README.md` |
| P5 closeout | `lab/evidence/phase5/P5_FINAL_CLOSEOUT.md` |
| TLS / Wireshark handoff | `lab/evidence/TLS_WIRESHARK_HANDOFF_ALIREZA.md` |

---

## 4. Documentation phases (CD — not lab P0–P7)

| Phase | Deliverable | Primary sources |
|-------|-------------|-----------------|
| **CD-1** | Product description + regulatory scope | §3.1 · `ccli-product-spec-roadmap` |
| **CD-2** | Traceability matrix + GAP register | §3.1 · lab evidence index |
| **CD-3** | PICS pack (61850 + 62351) | §3.2 · Higeco UCA sample (PICS/PIXIT list) |
| **CD-4** | SDLC + 62443 CR matrix + transport test plan | §3.3 · Higeco CSA sample |
| **CD-5** | O.14 logger spec + 61557-12 test plan | §3.4 · Annex O §11 |
| **CD-6** | Legal memo + CE file index + Dichiarazione draft | §3.5 · DI.CO sample (after OCR) |

Suggested output folder: `docs/conformity/` (create when starting CD-1).

---

## 5. RAG ingest — certificates first

```powershell
cd C:\Yahia\projects\CCLI
python scripts/extract-certificates.py
powershell -File scripts\ingest-certificates.ps1
```

If RAG is down, script still syncs to Telematry `documents/projects/ccli/certificate/`. Start Telematry stack and re-run.

**Priority `source_id` after ingest:**

1. `ccli-certificate-samples-extract`
2. `ccli-certificate-readme`
3. `ccli-cert-ref-uca-61850-higeco`
4. `ccli-cert-ref-62443-csa-higeco`
5. `ccli-cert-ref-62351-3-higeco`

---

## 6. Still missing from RAG (request from programme lead)

| Item | Needed for |
|------|------------|
| **IEC 62351-100-3** PDF | Transport test procedure (Annex O cites this, not 62351-3 alone) |
| **61850-8-1:2011 Ed.2** PDF | Authoritative UCA PICS |
| **ARERA 385/2025 + 564/2025** | Legal scope memo |
| **61010-1 / 61010-2-201** | Env test plan with 61557-12 |
| ~~OCR of DI.CO + CEI 0-16.pdf~~ | **DONE** — see `_extracts/*_ocr.txt` |

---

## 7. Boundaries

| You own | You do not own |
|---------|----------------|
| Conformity documents, PICS, matrices, plans, drafts | Lab phases P0–P7 |
| Citing `lab/evidence/` as annexes | Running TSP / fullcircle on DUT |
| Lab RFQ preparation (scope text) | Booking labs or signing certificates |
| GPL compliance register | libiec61850 commercial license |

---

## 8. Contacts / inputs from engineering

Request via ticket or `docs/conformity/CD0_INPUT_REQUESTS.md`:

- Product IED name + final CID path (`apps/ccli/config/icd/`)
- Build version (`apps/ccli/VERSION`)
- Product vs lab config diff
- Event log sample dump
- Open GAP list (I/O, Annex M, metrology, TPM)

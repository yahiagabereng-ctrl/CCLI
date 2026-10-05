# CCI Certificate Samples — Competitor Reference Extract (Higeco)

**Document ID:** CCLI-CERT-SAMPLES-001  
**Revision:** 1.0  
**Date:** 2026-10-02  
**RAG source_id:** `ccli-certificate-samples-extract`  
**Purpose:** English engineering extract from reference certificates in `Certificate/` — **not** HiTEKS product certs.  
**Vendor reference:** Higeco S.r.l. HSC-110-C V5.1 (AiLux-class competitor)

---

## 1. Regulatory target (CEI 0-16 Annex O §12)

HiTEKS must eventually match the **certificate types** listed in Annex O O.15.4:

| # | Certificate / deliverable | Accredited body type |
|---|---------------------------|----------------------|
| 1 | Functional tests (O.6, 61850, logger, self-diagnostics) | Manufacturer + evidence; 17065 for third party |
| 2 | **CEI EN 61557-12** (+ IEC 61010) environmental/EMC/insulation | **ISO/IEC 17025** (ACCREDIA in Italy) |
| 3 | **FIPS 140-2 L3** crypto module | Independent body (typically TPM) |
| 4 | **IEC 62443-4-1** SDLC + **62443-4-2** component | ISASecure EDSA/CSA or equivalent |
| 5 | **IEC 61850** conformance | **UCA User Group** accredited lab |
| 6 | **IEC 62351-3** transport (**62351-100-3** test cases) | Accredited cert body |
| 7 | **ISO 9001** manufacturing QMS | Manufacturer |
| 8 | **CE** + **Dichiarazione di conformità** (DPR 445/2000 Art. 47) | Manufacturer |

**Corpus:** `cei-0-16-allegato-o` §12 · `ccli-phase-regulation-checklists` (reference only for engineering phases)

---

## 2. Sample A — IEC 61850 UCA (DNV / UCA IUG)

**File:** `Certificate/IEC 61850 UCA Standard Group Certificate.pdf`  
**RAG source_id:** `ccli-cert-ref-uca-61850-higeco`

| Field | Value |
|-------|-------|
| Issued to | Higeco Srl, Belluno, Italy |
| Product | HSC RTU — HSC 110C V5.1 |
| Standard | IEC 61850 **Edition 2** Parts 6, 7-1, 7-2, 7-3, 7-4, 8-1 |
| Test basis | **IEC 61850-10 Ed.2** · **UCA IUG Server Test Procedures v2.0.6** |
| Lab | DNV Netherlands B.V. (Level **A1** — ISO 9001 lab) |
| PICS submitted | Protocol ICS, Model ICS, TISSUES ICS, **PIXIT** (dated Jan 2023) |
| Certificate No. | 10373575-DSO 23-2657 |
| Validity note | Single specimen; production process **not** assessed |

**Conformance blocks tested (passed/total):**

| Block | Result |
|-------|--------|
| Basic Exchange | 22/26 |
| Data Sets | 4/7 |
| Data Set Definition | 24/24 |
| Setting Group Selection | 4/4 |
| Unbuffered Reporting | 22/23 |
| Buffered Reporting | 32/33 |
| Direct / SBO / Enhanced Control | partial (12a–12d) |
| Time Synchronization | 4/7 |
| File Transfer | 8/8 |

**HiTEKS documentation implication:** prepare **PICS + MICS + TISSUES + PIXIT** before UCA lab booking; scope server blocks needed for CCI (control, reporting, time sync minimum).

---

## 3. Sample B — IEC 62443-4-2 ISASecure CSA (BYHON Italy)

**File:** `Certificate/IEC 62443-4-2 - Isa Secure CSA-E01.pdf`  
**RAG source_id:** `ccli-cert-ref-62443-csa-higeco`

| Field | Value |
|-------|-------|
| Product | HSC-110-C V5.1 |
| Standards | ANSI/ISA-62443-4-1:2018 · IEC 62443-4-2:2018 |
| Scheme | **ISASecure Component Security Assurance (CSA) 1.0.0** |
| SL-C | Capability Security Level **1** (embedded device) |
| SL vector | SL-C(HSC-110-C)={2 2 1* 1 1 1 3} (*non-field ports SL-C=2) |
| Certificate No. | HIGE-HSC110C-EMB-E01 Rev 01 |
| Issued | 2022-12-21 |
| Chartered lab | **BYHON** (HON CONSULTING), Prato, Italy — ISCI-CL0005 |
| Dependency | Valid while **SDLA** cert HIGE-SDLA remains valid |

**HiTEKS documentation implication:** pair **62443-4-1 SDLC pack** with **62443-4-2 CR matrix**; CSA cert depends on SDLA — document both in technical file.

---

## 4. Sample C — IEC 62351-3 transport (IMQ / Chrono Italy)

**File:** `Certificate/IEC-62351-3 Rev1.pdf`  
**RAG source_id:** `ccli-cert-ref-62351-3-higeco`

| Field | Value |
|-------|-------|
| Scheme | IND-F-006_CYB.62351 Rev 00 (Chrono / IMQ network) |
| Standard | **IEC 62351-3:2020** |
| Product | HSC-110C V5.1 — SN on certificate |
| Test report | CYB.62351.1 · IT FILE 22.IT.4467928.006 R1 |
| Validity | 2022-12-30 → **2025-12-30** (3 years) |
| Scope | TLS/TCP confidentiality, integrity, message auth for telecontrol |

**HiTEKS documentation implication:** transport cert cites **62351-3:2020**; Annex O requires **62351-100-3** test procedures — ingest 62351-100-3 PDF for HiTEKS test-plan doc.

---

## 5. Sample D — Dichiarazione + rapporto di prova (OCR ✓ 2026-10-02)

**File:** `Certificate/DI.CO. QUADRO_ACQUISIZIONE E MISURA.pdf`  
**OCR:** `Certificate/_extracts/DI_CO_QUADRO_ACQUISIZIONE_E_MISURA_ocr.txt`

| Field | Value |
|-------|-------|
| Issuer | Elettrotecnica Emme (Benaglia Mauro), Mesero |
| Standard | **CEI EN 61439-2** — LV switchboard (cabinet for CCI measure acquisition) |
| Client | ENERGY TEAM |
| Product | CCI - Quadro Gruppo Acquisizione misura · Matricola **CCI 059/2025** |
| Date | Rapporto di prova **17-02-2025** — esito **POSITIVO** |
| Scope | Not CEI 0-16 CCI product cert — **cabinet** declaration template |

---

## 6. Sample E — CEI 0-16 Annex O/T (TÜV — OCR ✓ 2026-10-02)

**File:** `Certificate/CEI 0-16.pdf` *(TÜV cert, not standard PDF)*  
**OCR:** `Certificate/_extracts/CEI_0-16_ocr.txt`

| Field | Value |
|-------|-------|
| Issuer | **TÜV Rheinland** |
| Holder | HIGECO MORE S.R.L., Belluno |
| Product | CCI **Q01-ESC-4T2F-10DI-PA** (Controllore Centrale di Impianto) |
| Tested acc. to | **Annex O and Annex T** — CEI 0-16:2022-03 · V1:2022-11 |
| Reg. No. | AK 60169282 0001 · Report IT234ETC 001 |

**HiTEKS target:** equivalent third-party or self-declaration path per Annex O O.15.4 after internal evidence pack.

---

## 7. Competitor FAQ (cert procedure summary)

**File:** `competitors/competitors/Higeco/CCI_Compliance_FAQ.html`  
**RAG source_id:** `ccli-higeco-compliance-faq`

| FAQ | Requirement |
|-----|-------------|
| Q13 | Self-declaration after testing **or** third-party accredited body |
| Q14 | **62443-4-1** (SDLA) + **62443-4-2** (CSA) + **FIPS 140-2 L3** |
| Q15 | Insulation/EMC/climate → **ISO 17025**; functional → **ISO 17065** |

**Brochure claims:** `ccli-competitors-hw-extract` — UCA 61850, 62351-100-3, 61557-12, CEI O/T 2022-03.

---

## 8. Documentation corpus map (procedure — not lab phases)

| Doc | Path | RAG `source_id` | Role |
|-----|------|-----------------|------|
| Annex O §11–12 | `CCI_Annex_O_Extract.md` | `cei-0-16-allegato-o` | **What** to certify |
| Annex T cyber | `CCI_Annex_T_Extract.md` | `cei-0-16-allegato-t` | 61850 / PKI / TLS |
| 62351-3 PICS | `CCI_62351-3_Extract.md` | `ccli-62351-3-extract` | TLS PICS tables |
| 62351-4 PICS | `CCI_62351-4_Extract.md` | `ccli-62351-4-extract` | MMS E2E PICS |
| 62351-9 PICS | `CCI_62351-9_Extract.md` | `ccli-62351-9-extract` | PKI PICS |
| 62443-4-1 SDL | `CCI_62443-4-1_Extract.md` | `ccli-62443-4-1-extract` | SDLC evidence |
| 62443-4-2 CR | `CCI_62443-4-2_Extract.md` | `ccli-62443-4-2-extract` | Component CR matrix |
| 61557-12 | `CCI_61557-12_Extract.md` | `ccli-61557-12-extract` | Env/metrology plan |
| ARERA | `CCI_ARERA_540_2021_Extract.md` | `arera-540-2021` | Legal scope (385/564 **MISSING**) |
| Key ceremony | `CCI_K6.3_Key_Ceremony_SOP.md` | `ccli-k63-ceremony-sop` | PKI operations |
| MMS debug map | `CCI_62351_MMS_Security_Debug_Map.md` | `ccli-62351-mms-security-debug-map` | Transport test narrative |
| Product spec | `CCI_CCLI_Product_Spec_and_Roadmap.md` | `ccli-product-spec-roadmap` | Product identity |
| Lab evidence index | `lab/evidence/README.md` | (filesystem) | Annex to conformity file — **read only** |

**Handoff index:** `Certificate/README.md`

---

## 9. Missing from RAG (ingest before finishing docs)

| Priority | Item | Blocks |
|----------|------|--------|
| P0 | **IEC 62351-100-3** PDF | Transport test procedure document |
| P0 | **61850-8-1:2011 Ed.2** PDF | UCA PICS authoritative baseline |
| P0 | **ARERA 385/2025 + 564/2025** | Legal scope memo |
| ~~P1~~ | ~~OCR DI.CO + CEI 0-16.pdf~~ | **DONE** — `Certificate/_extracts/*_ocr.txt` |
| P1 | **61010-1 / 61010-2-201** | 61557-12 env test plan |
| P1 | TPM **FIPS 140-2** module cert | Crypto claim memo |

**Ingest certificates:** `powershell -File scripts/ingest-certificates.ps1`

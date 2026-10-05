# Certificate samples index — Higeco reference (competitor)

**Folder:** `C:\Yahia\projects\CCLI\Certificate`  
**RAG extract:** `knowledge-base/08-engineering/CCI_Certificate_Samples_Extract.md`  
**source_id:** `ccli-certificate-samples-extract`

Use these samples to see **format, lab type, and attached evidence** (PICS, test reports, validity). HiTEKS documents must follow the same **certificate types** in Annex O O.15.4, not Higeco product data.

---

## Sample 1 — IEC 61850 UCA

**File:** `IEC 61850 UCA Standard Group Certificate.pdf`  
**RAG:** `ccli-cert-ref-uca-61850-higeco`

| Field | Detail |
|-------|--------|
| Issuer | DNV Netherlands B.V. |
| Scheme | **UCA International Users Group** — IEC 61850 Certificate **Level A1** |
| Product | Higeco HSC-110-C V5.1 (RTU server) |
| Standard tested | IEC 61850 **Ed.2** — parts 6, 7-1, 7-2, 7-3, 7-4, 8-1 |
| Test procedures | IEC 61850-10 Ed.2 + **UCA Server Test Procedures v2.0.6** |
| Submitted with test | **PICS**, **MICS**, **TISSUES ICS**, **PIXIT** (Jan 2023) |
| Cert number | 10373575-DSO 23-2657 |
| Report | 10373575-DSO 23-2656 rev 2 (archived at DNV) |

**Blocks to mirror for CCLI doc scope:** Basic exchange, datasets, reporting, control, time sync, file transfer — map to `signal_map.yaml` + CID.

---

## Sample 2 — IEC 62443 ISASecure CSA

**File:** `IEC 62443-4-2 - Isa Secure CSA-E01.pdf`  
**RAG:** `ccli-cert-ref-62443-csa-higeco`

| Field | Detail |
|-------|--------|
| Issuer | **BYHON** (Prato, Italy) — ISASecure chartered lab |
| Scheme | **ISASecure CSA 1.0.0** embedded device |
| Standards | IEC/ISA **62443-4-1** + **62443-4-2** |
| Product | HSC-110-C V5.1 |
| SL-C | Level 1 embedded; vector {2 2 1* 1 1 1 3} |
| Cert number | HIGE-HSC110C-EMB-E01 Rev 01 |
| Prerequisite | **SDLA** certificate HIGE-SDLA must stay valid |

**Docs to prepare for HiTEKS:** SDLC process doc (4-1) + CR matrix (4-2 Annex B) before CSA application.

---

## Sample 3 — IEC 62351-3 transport

**File:** `IEC-62351-3 Rev1.pdf`  
**RAG:** `ccli-cert-ref-62351-3-higeco`

| Field | Detail |
|-------|--------|
| Issuer | Chrono / IMQ network (Milano) |
| Standard | **IEC 62351-3:2020** |
| Product | HSC-110C V5.1 (serial number on cert) |
| Test report | CYB.62351.1 · IT FILE 22.IT.4467928.006 R1 |
| Validity | 3 years (2022-12-30 → 2025-12-30) |

**Note:** Annex O O.15.4 cites **62351-100-3** conformance tests. This sample is **62351-3** product cert — HiTEKS transport plan must reference **100-3 test cases** when PDF is ingested.

---

## Sample 4 — Dichiarazione di conformità (cabinet)

**File:** `DI.CO. QUADRO_ACQUISIZIONE E MISURA.pdf`  
**RAG:** `ccli-cert-ref-dico-quadro-misura` · OCR: `ccli-cert-ocr-di-co-quadro-acquisizione-e-misura-ocr`

| Field | Detail (OCR 2026-10-02) |
|-------|---------------------------|
| Issuer | Elettrotecnica Emme — Benaglia Mauro, Mesero (MI) |
| Standard | **CEI EN 61439-2** (low-voltage switchboard) — **not** CEI 0-16 product cert |
| Client / product | ENERGY TEAM · **CCI - Quadro Gruppo Acquisizione misura** |
| Matricola | CCI 059/2025 |
| Includes | Dichiarazione di conformità + **Rapporto di prova individuale** (17-02-2025) |
| OCR text | `_extracts/DI_CO_QUADRO_ACQUISIZIONE_E_MISURA_ocr.txt` |

Use as **layout reference** for Italian declaration + individual test report structure.

---

## Sample 5 — CEI 0-16 product conformity (TÜV)

**File:** `CEI 0-16.pdf` *(filename — actual doc is TÜV cert)*  
**RAG:** `ccli-cert-ref-cei-0-16` · OCR: `ccli-cert-ocr-cei-0-16-ocr`

| Field | Detail (OCR 2026-10-02) |
|-------|---------------------------|
| Issuer | **TÜV Rheinland** |
| Holder | HIGECO MORE S.R.L., Belluno |
| Product | Controllore Centrale di Impianto — **CCI Q01-ESC-4T2F-10DI-PA** |
| Tested acc. to | **Annex O and Annex T** — CEI 0-16:2022-03 · CEI 0-16 V1:2022-11 |
| Registration | AK 60169282 0001 · Report IT234ETC 001 |
| OCR text | `_extracts/CEI_0-16_ocr.txt` |

Primary **regulatory text** (not this cert): `knowledge-base/07-protocols/0-16consolidata.pdf` → `cei-0-16-consolidata-2025-12`.

---

## Certificate type checklist (HiTEKS target)

From Annex O + Higeco samples + FAQ:

| # | Certificate | Higeco sample | HiTEKS doc to write |
|---|-------------|---------------|---------------------|
| 1 | IEC 61850 UCA | Sample 1 ✓ | `05_61850_CONFORMANCE_SCOPE.md` + PICS |
| 2 | 62351-3 / 100-3 transport | Sample 3 ✓ (62351-3 only) | `06_CYBER/TRANSPORT_TEST_PLAN.md` |
| 3 | 62443-4-1 SDLA | *(dependency on Sample 2)* | `06_CYBER/SDLC_PROCESS.md` |
| 4 | 62443-4-2 CSA | Sample 2 ✓ | `06_CYBER/COMPONENT_CR_MATRIX.xlsx` |
| 5 | 61557-12 + 61010 | Brochure claim only | `07_ENV/61557-12_TEST_PLAN.md` |
| 6 | FIPS 140-2 L3 TPM | FAQ Q14 | `08_CRYPTO/TPM_FIPS_MEMO.md` |
| 7 | CE + Dichiarazione | Sample 4 (partial) | `09_LEGAL/DICHIARAZIONE_DRAFT.md` |
| 8 | ISO 9001 | Not in folder | QMS cross-ref from company |

---

## Regenerate extracts

```powershell
python C:\Yahia\projects\CCLI\scripts\extract-certificates.py
```

Output: `Certificate/_extracts/*.txt`

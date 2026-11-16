# IEC 62443 — CCLI package (this repo)

**Folder:** `knowledge-base/08-engineering/62443/`  
**Repo:** CCLI (lab + product workspace). All 3-2, 4-1 and 4-2 working files for this programme live **here**.  
**Scheme:** **IECEE** (IEC 62443-3-2, 4-1, 4-2). TesPro Excel is the platform slice only.  
**Parent CRS:** [`../CCI_62443_Zones.md`](../CCI_62443_Zones.md)  
**Applicant gap matrix:** [`../CCI_62443_TesPro_HiTEKS_Gap_Matrix.md`](../CCI_62443_TesPro_HiTEKS_Gap_Matrix.md)  
**Lab records:** [`../../../lab/evidence/62443/`](../../../lab/evidence/62443/README.md)  
**Toolchain:** [`../START_HERE_62443_TOOLCHAIN.md`](../START_HERE_62443_TOOLCHAIN.md)  
**Company-wide training (SM-4):** [`CCI_62443_ESSENTIAL_SECURITY_TRAINING.md`](CCI_62443_ESSENTIAL_SECURITY_TRAINING.md) · email text [`company-broadcast/MESSAGE_62443_ESSENTIAL_TRAINING.txt`](company-broadcast/MESSAGE_62443_ESSENTIAL_TRAINING.txt)

```text
Notion (same breadcrumb as the course)
  General
    IEC 62443-4-1 Secure product development lifecycle requirements
      1 Security Management
      0 Process Development · 2–8 practices · templates 1.1–8.1 · deliverables 9.1–10.5

git  62443/
  README.md                          ← this map
  SDL-4-1-Document-Library.md        ← Notion-shaped 4-1 table
  SDL-4-1-Process-Development.md     ← intro + eight practices + SM…SG shalls map
  SDL-SM-Lifecycle.md                ← waterfall + agile (SM-1)
  SDL-SM-Practice-1 … SDL-SG-Practice-8
  SDL-SM-Practice-1-Security-Management.md
  SDL-SM-1-Development-Process.md
  templates/                         ← lab 1.1–1.3 + SM-1…13 annexes
  ZCR-1 … ZCR-7 + risk matrix        ← IEC 62443-3-2
  trf/                               ← IEC 62443-4-2 TRF SL-C 2
  TesPro_IEC62443_Supplier_Clauses.xlsx
```

---

## IEC 62443-4-1 Practice 1 — security management

Generic lifecycle **first**, then embed the eight practices. CCLI **adopts the agile ring**.

| Document | Role | Status |
|----------|------|--------|
| [`SDL-4-1-Document-Library.md`](SDL-4-1-Document-Library.md) · [`CCI_62443_4-1_LIBRARY.csv`](CCI_62443_4-1_LIBRARY.csv) | Course-shaped Notion table (8 processes + templates) | **PART** |
| [`SDL-SM-Lifecycle.md`](SDL-SM-Lifecycle.md) | Waterfall example + agile cycle (adopted) | **HAVE** |
| [`SDL-SM-Practice-1-Security-Management.md`](SDL-SM-Practice-1-Security-Management.md) | SM-1…13 process (library **1**) | **PART** |
| [`SDL-SM-1-Development-Process.md`](SDL-SM-1-Development-Process.md) | SM-1 characteristics (a)–(f) | **PART** |
| [`templates/SM_Product_Security_Development_Plan.md`](templates/SM_Product_Security_Development_Plan.md) | Lab template **1.1** | **HAVE** blank |
| [`templates/SM_SDL_Requirement_Checklist.md`](templates/SM_SDL_Requirement_Checklist.md) | Lab template **1.2** | **HAVE** blank |
| [`templates/SM_Retrospective.md`](templates/SM_Retrospective.md) | Lab template **1.3** | **HAVE** blank |
| [`templates/`](templates/) | SM-1…13 clause annexes | **HAVE** templates; records **MISS** |

| Agile phase | 4-1 practice |
|-------------|--------------|
| Plan | SM |
| Design / Launch | SR |
| Design | SD |
| Develop | SI |
| Test | SVV |
| Deploy | DM |
| Deploy / Review | SG |
| Review | SUM |

---

## IEC 62443-3-2 — ZCR 1–7 (TR400 / TG-524)

**Standard:** IEC 62443-3-2:2020 Figure 1 workflow  
**Validation:** [`VALIDATION.md`](VALIDATION.md) · `scripts/validate-zcr-tr400.ps1`

| ZCR | Step | Document | Req ID | Engineering status |
|-----|------|----------|--------|-------------------|
| **1** | Identify SUC | [`ZCR-1-SUC-TR400.md`](ZCR-1-SUC-TR400.md) | REQ-SEC-ZC-001 | **PART** |
| **2** | Initial risk | [`ZCR-2-INITIAL-RISK-TR400.md`](ZCR-2-INITIAL-RISK-TR400.md) | REQ-SEC-ZC-002 | **PART** |
| **2+** | Matrix guide | [`ZCR-2-Risk-Matrix-Illustrated.md`](ZCR-2-Risk-Matrix-Illustrated.md) | — | **HAVE** |
| — | **5×5 combiner (adopted)** | [`Risk-Matrix-5x5-ADOPTED.md`](Risk-Matrix-5x5-ADOPTED.md) | — | **HAVE** |
| **3** | Zones / conduits | [`ZCR-3-PARTITION-TR400.md`](ZCR-3-PARTITION-TR400.md) | REQ-SEC-ZC-003 | **PART** |
| **4** | vs tolerable | [`ZCR-4-DECISION-TR400.md`](ZCR-4-DECISION-TR400.md) | REQ-SEC-ZC-007 | **PART** (YES → ZCR 5) |
| **5** | Detailed risk | [`ZCR-5-DETAILED-RISK-TR400.md`](ZCR-5-DETAILED-RISK-TR400.md) | REQ-SEC-ZC-007/008 | **PART** |
| **6** | CRS | [`ZCR-6-CRS-TR400.md`](ZCR-6-CRS-TR400.md) | REQ-SEC-ZC-009 | **PART** |
| **7** | Owner approval | [`ZCR-7-OWNER-APPROVAL-TR400.md`](ZCR-7-OWNER-APPROVAL-TR400.md) | REQ-SEC-ZC-010 | **MISS** signature |

---

## IEC 62443-4-2 TRF (SL-C 2)

**Folder:** [`trf/`](trf/README.md) · **Precise guide:** [`trf/READ.md`](trf/READ.md)  
**Export:** `python scripts/62443/export-trf-worksheet.py` · **Validate:** `scripts/validate-trf-62443.ps1`

| Document | Role | Status |
|----------|------|--------|
| [`trf/TRF-62443-4-2-CCLI.md`](trf/TRF-62443-4-2-CCLI.md) | Component SL-C 2 worksheet | **PART** (84 TBD, CR 1.8 **MISS**) |

---

## TesPro supplier slice

| File | Role |
|------|------|
| [`TesPro_IEC62443_Supplier_Clauses.xlsx`](TesPro_IEC62443_Supplier_Clauses.xlsx) | TesPro R / C / R clauses only — not the applicant dossier |

---

## Read order

1. [`README.md`](README.md) (this file)  
2. [`SDL-4-1-Document-Library.md`](SDL-4-1-Document-Library.md) · [`SDL-SM-Lifecycle.md`](SDL-SM-Lifecycle.md) · Practice 1 · SM-1 SOP · templates **1.1–1.3**  
3. ZCR **1 → 7**  
4. [`trf/READ.md`](trf/READ.md)  
5. [`VALIDATION.md`](VALIDATION.md)  
6. [`../CCI_62443_Zones.md`](../CCI_62443_Zones.md)

---

## One command — validate 3-2 package

```powershell
cd c:\Yahia\projects\CCLI
.\scripts\validate-zcr-tr400.ps1
```

Exit **0** = structural checks pass (content gaps may still show **PART** in `VALIDATION.md`).

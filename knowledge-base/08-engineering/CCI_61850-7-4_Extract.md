# IEC 61850-7-4 — CCLI Engineering Extract (LN classes / compatibility)

**Document ID:** CCLI-PROTO-61850-7-4-001  
**Revision:** 1.0  
**Date:** 2026-09-30  
**Edition:** IEC 61850-7-4 **Edition 2.0 2010-12**  
**RAG source_id:** `ccli-61850-7-4-extract`  
**Normative basis:** IEC 61850-7-4 — *Basic communication structure — Compatible logical node classes and data object classes*  
**Licensed PDF:** `07-protocols/regulations/standards-drop-20260918/IEC 61850-7-4-2010.pdf` (185 file pages OCR)  
**Full text:** `ccli-61850-7-4-ocr-corpus` → `CCI_61850-7-4_OCR_Corpus.md`  
**Capture status:** **HAVE (P0 corpus)** — PDF on disk + full OCR 2026-09-30  
**Parents:** `ccli-61850-7-3-extract` · `ccli-61850-6-extract` · `cei-tr-57-126` · `cei-0-16-allegato-t`  
**Programme link:** K4.3 ICD · TR 57-126 LN/DO names · **61850-7-420** DER profile

---

## Summary

**61850-7-4** defines **compatible logical node classes** (`MMXU`, `LLN0`, `LPHD`, …) and their **standardized data objects** (`TotW`, `TotVAr`, `Mod`, …). **61850-7-420** extends 7-4 for DER (`DPCC`, `DVAR`, `DPFW`, …) — declared via **`lnNs`** in TR 57-126 CID.

**CCLI:** We **conform** to 7-4/7-420 naming but **do not load** the CID at runtime; `mms_adapter.cpp` builds a **subset** matching `signal_map.yaml`. Base namespace **`IEC61850-7-4:2010`** on `LLN0.NamPlt.ldNs`.

---

## Requirements

| REQ-ID | Source | Requirement | CCLI |
|--------|--------|-------------|------|
| **REQ-874-LN-001** | 7-4 | LN **lnClass** + **prefix** + **inst** uniquely identify function | `PdCMMXU1`, `WlimDWMX1` |
| **REQ-874-MMXU-001** | **MMXU** | POC measurements: **TotW**, **TotVAr**, **PPV**, **A**, … | T.3.1.3 / O.8.3 |
| **REQ-874-DPCC-001** | 7-420 **DPCC** | Active power controller at POC | `PdC_Wi` / Wlim family |
| **REQ-874-DVAR-001** | 7-420 **DVAR** | Reactive power controller | P5-R01 `VArSdDVAR1` |
| **REQ-874-NS-001** | Namespace | `ldNs` = 7-4; DER LNs use **7-420** `lnNs` | Lab CID |
| **REQ-874-DS-001** | DataSets | FCDA reference **lnClass** + **doName** | `DS_R_PdC_Mis4sec` |

---

## Batch A — LN classes in TR 57-126 / CCLI MVP

| lnClass | Prefix (example) | Namespace | CCLI status |
|---------|------------------|-----------|-------------|
| **MMXU** | PdC | 7-4 | **IMPL** TotW, TotVAr |
| **MMXU** | GenPV, GenTer, … | 7-4 | STUB / future |
| **DPCC** / **DPCW** | Wlim, PdC_Wi | 7-420 | **PART** Wlim APC |
| **DAGC** | WSd | 7-420 | **PART** WSd APC |
| **DVAR** | VArSd | 7-420 | **OPEN** P5-R |
| **LLN0** | — | 7-4 | Reports, DataSets |
| **LPHD** | — | 7-4 | PhyNam / vendor |

---

## Batch B — PdC MMXU data objects (Annex T T.3.1.3)

| DO | CDC (7-3) | Mandatory | CCLI |
|----|-----------|-----------|------|
| **TotW** | MV | Yes | **IMPL** — 4 s URCB |
| **TotVAr** | MV | Yes | **IMPL** r21 |
| **PPV** | MV | Yes | **OPEN** P5-M08 |
| **A** | MV | Conditional | **OPEN** |

---

## Batch C — Namespace model (CID)

```
LLN0.NamPlt.ldNs  = IEC61850-7-4:2010     (base dictionary)
DER LN.NamPlt.lnNs = IEC61850-7-420:2019A (extension)
```

Runtime libiec61850 dynamic model uses **same object names** without parsing SCL.

---

## BOM matrix

| Block | Function | Candidate / PN | Status |
|-------|----------|----------------|--------|
| 7-4 dictionary | LN/DO names | IEC PDF + TR 57-126 | **INGESTED** |
| 7-420 DER LNs | PF2 control | CEI TR + Annex T | **PART** |
| libiec61850 | CDC runtime | third_party | **HAVE** |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-74-01 | Hand-built model ≠ full CID | DSO conformance fail | CID loader / genconfig |
| R-74-02 | 7-420 AMD drift | lnNs mismatch | Freeze TR 57-126 rev |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| PDF confirmed | Junction + SHA/size | Non-zero file |
| OCR corpus | Page count = 185 | Searchable MD |
| MMS browse | Client directory | `PdCMMXU1` present |
| P3-14 | Regulation checklist | **PASS** (corpus) |

---

## Keywords

`61850-7-4`, `MMXU`, `DPCC`, `DVAR`, `7-420`, `ccli-61850-7-4-extract`, `ccli-61850-7-4-ocr-corpus`

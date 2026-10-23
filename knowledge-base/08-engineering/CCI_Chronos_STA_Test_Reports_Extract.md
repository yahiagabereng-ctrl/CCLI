# Chronos S.T.A. test reports — engineering extract

**Document ID:** CCLI-KB-CHRONOS-STA-001  
**Revision:** 1.0  
**Date:** 2026-10-23  
**RAG source_id:** `ccli-chronos-sta-test-reports`  
**Corpus status:** **HAVE** (PDF + `_extracts/*.txt`)  
**Staging drop:** `CHRONOS/CHRONOS/` (Nicola intake)  
**Canonical PDFs:** `knowledge-base/08-engineering/reference/vendor/chronos/2026-sta-reports/`  
**Mapping:** [CHRONOS_STA_REPORTS_ANNEX_MAP.md](../../lab/conformance/CHRONOS_STA_REPORTS_ANNEX_MAP.md)  
**Parents:** `ccli-chronos-emt432-vendor-requirements` · `ccli-61557-12-extract` · `cei-0-16-allegato-o-extract`

**Lab:** S.T.A. S.r.l. (Grezzana / San Martino Buon Albergo VR) · stalab.it  
**Bench specimen (programme):** **EMT432.V.FULL** — **not present** in this pack (see mapping § model gap).

---

## Inventory (ingested 2026-10-23)

| File | Report ID | Model | Type | Issue / rev | Pages |
|------|-----------|-------|------|-------------|-------|
| `C25305-ACR_01_EMT130_EMC_61326-1.pdf` | **C25305-ACR_01** | **EMT130** | EMC | TRF M100 (see PDF rev table) | 50 |
| `C25305-ACR_02_EMT430_EMC_61326-1.pdf` | **C25305-ACR_02** | **EMT430** | EMC | First emission **05/03/2026** | 50 |
| `C25306_01_EMT130_61010-2-030.pdf` | **C25306_01** | **EMT130** | Safety | **26/06/2026** | 63 |
| `C25306_01_EMT430_61010-2-030.pdf` | **C25306_01** | **EMT430** | Safety | **01/07/2026** | 61 |

**Note:** Two safety reports share reference **C25306_01** but differ by **model**, **page count**, and **issue date** — treat as **separate EUT records**, not one report.

---

## Reference standards (from PDF headers)

| Report family | Normative base | Programme hook |
|---------------|----------------|----------------|
| EMC ACR_* | **EN IEC 61326-1:2021** + **EN 55011** (Class **B**, Group **1**) + **EN 61000-4-* / 61000-3-* | O.15 **#2 EMC** — **61326 path**, **not** 61557-12 PMD Tables 9/13/17 |
| Safety M213 | **EN IEC 61010-2-030:2022-02** + **EN IEC 61010-1:2010/A1:2019** | O.15 **#2** + **61557-12 §4.10** measuring-circuit safety (**2-030**, not Allegato O **2-201** text) |

---

## EMC — test accreditation flags (C25305-ACR_02 / EMT430)

Summary table marks several clauses with **\*** = **not part of ACCREDIA accreditation** of S.T.A. (bilingual disclaimer on cover pages). Example rows (EMT430 **ACR_02**):

- Radiated emission 30 MHz–1 GHz / 1–6 GHz — **Accredited: No** — **PASS**
- Conducted emission AC — (see full `_extracts` for each port)
- Immunity sections **I3**, **I6**, **E2**, **E4**, **E5** marked **\*** in TOC → verify line-by-line before filing as **17025 / ACCREDIA** evidence

**Corpus:** full text in `2026-sta-reports/_extracts/C25305-ACR_02_EMT430_EMC_61326-1.txt`.

---

## Safety — environmental declaration (customer data)

Both **61010-2-030** reports declare (EUT supplementary table):

- **Temperature:** **−25 °C to +65 °C**
- **Humidity:** 10–90 % non-condensing
- **IP20**, Class **II**, OVC **III**, PD **2**

**Programme read:** manufacturer rating **covers O.15 #2 ≥ K55 (−5…+55 °C)** as **declared spec** — **not** the same as an **IEC 60068 / 61557 Table 6** type-test report until Chronos cites one inside or outside these PDFs.

---

## Extraction

```powershell
# After copying PDFs into 2026-sta-reports/
python -c "..."  # see repo ingest note in reference/vendor/chronos/README.md
```

Re-run mapping doc after any new PDF drop; do **not** map to REQ-CHR until extract exists (**ingest → map**).

---

## Keywords

`Chronos`, `S.T.A.`, `C25305`, `C25306`, `61326-1`, `61010-2-030`, `EMT430`, `EMT130`, `EMT432`, `vendor`, `O.15`

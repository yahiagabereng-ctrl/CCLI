# Chronos S.T.A. reports → CEI Annex O / IEC 61557-12 annex map

**Document ID:** CCLI-LAB-CHRONOS-STA-ANNEX-MAP-001  
**Revision:** 1.0  
**Date:** 2026-10-23  
**RAG source_id:** `ccli-chronos-sta-annex-map`  
**Workflow:** **(1) ingest PDF → `_extracts` → extract MD** · **(2) this mapping** · **(3) update** [CHRONOS_EMT432_VENDOR_REQUIREMENTS.md](CHRONOS_EMT432_VENDOR_REQUIREMENTS.md) / Full Validate O.15 #2  
**Ingest parent:** [CCI_Chronos_STA_Test_Reports_Extract.md](../../knowledge-base/08-engineering/CCI_Chronos_STA_Test_Reports_Extract.md)  
**61557 annex baseline:** [61557-12_TESPRO_VENDOR_MAP.md](61557-12_TESPRO_VENDOR_MAP.md) §4.2 · [ANNEX_OT_SW_GAP_MATRIX.md](ANNEX_OT_SW_GAP_MATRIX.md)  
**Bench meter:** [METER_SPECIMEN_EMT432_V_FULL.md](METER_SPECIMEN_EMT432_V_FULL.md)

---

## Requirements

| ID | Requirement |
|----|-------------|
| **REQ-MAP-001** | Map each ingested report to **Annex O §12 #2** rows and **61557-12 annex** hooks without claiming **EMT432** coverage until model equivalence is documented. |
| **REQ-MAP-002** | **61326-1 EMC** shall **not** be filed as **61557-12 PMD EMC** (Tables 9/13/17) without lab object change. |
| **REQ-MAP-003** | **61010-2-030** satisfies **61557-12:2018 §4.10** safety part for **measuring circuits**; Allegato O **61010-2-201** text remains an **edition/wording gap** for HiTEKS #2 lab narrative. |
| **REQ-MAP-004** | **61557-12** type-test (**Annex D/F**, Table 3/8, class 0,2) stays **MISS** until a report names **61557-12** (prefer **2018+AMD1:2021** if EPMF). |

---

## Ingest → map pipeline

```text
CHRONOS/CHRONOS/*.pdf
    → knowledge-base/.../chronos/2026-sta-reports/*.pdf
    → _extracts/*.txt
    → CCI_Chronos_STA_Test_Reports_Extract.md
    → CHRONOS_STA_REPORTS_ANNEX_MAP.md (this file)
    → CHRONOS_EMT432_VENDOR_REQUIREMENTS deliverables table + Full Validate O.15 #2
    → RAG: ccli-chronos-sta-test-reports · ccli-chronos-sta-annex-map
```

**Do not** update vendor REQ-CHR **PASS/HAVE** from filename alone — only from extract fields (report ID, model, SN, norm, ACCREDIA flag, verdict).

---

## Report → Annex O (Allegato O)

| O clause | Shall (summary) | Ingested evidence | 5-LN / EMT432 bench | Verdict |
|----------|-----------------|-------------------|---------------------|---------|
| **O.15 §12 #2** (61557-12) | PMD / EPMF type-test on measurement chain | **None** in STA pack | EMT432 front-end | **GAP** — still **MISS** |
| **O.15 §12 #2** (61010) | Safety on measuring equipment | **C25306_01** ×2 · **61010-1 + 2-030** | Models **EMT130**, **EMT430** only | **PART** — **wrong SKU** for **EMT432.V.FULL**; **no CE DoC** in pack |
| **O.15 §12 #2** (EMC) | 61557 influence or bundled PMD EMC | **C25305-ACR_01/02** · **61326-1:2021** | EMT130 / EMT430 | **PART** — **61326**, not **61557** tables; **ACCREDIA *** on several lines |
| **O.15 §12 #2** (K55) | ≥ K55 climate | Declared **−25…+65 °C** on 61010 forms | Not 60068 report ID | **PART** (declared) / **GAP** (accredited climatic test) |
| **O.13.2.1** | Converter ≤ 0,2 · sensor ≤ 0,5 · 61557 fixed-block method | **Not in STA pack** (62053 / 61557) | 62053 marketing only | **GAP** |
| **O.7.4 / O.8.3** | Annex **D** fixed blocks · 4 s UTC | Not in STA pack | `ccli` last-sample | **HiTEKS SW GAP** (unchanged) |
| **O.13.5** | UTC ±100 ms | N/A on meter reports | GNSS on CCI | **N/A** here |

---

## Report → IEC 61557-12:2018 annexes (Ed 2.0 corpus)

Same annex table as [61557-12_TESPRO_VENDOR_MAP.md](61557-12_TESPRO_VENDOR_MAP.md) §4.2 — **meter / Chronos** column:

| 61557-12 annex | Shall / role | STA pack | EMT432 programme | Gap |
|----------------|--------------|----------|------------------|-----|
| **A–C** | Informative | — | — | None |
| **D** | **Normative** demand · Fig D.2 fixed block | Not tested | `ccli` | **GAP** |
| **E–G** | Uncertainty / sensors | Not tested | FRC + chain calc | **GAP** |
| **H (AMD1)** | EPMF SS | Not referenced | HiTEKS EPMF story | **MISS** |
| **§4.10 + 61010-2-030** | Measuring-circuit safety | **C25306_01** PASS path on **EMT130/430** | Need **EMT432** or **equivalence letter** | **PART** |
| **Tables 3/8/9/13/17/26** | PMD performance + EMC | **No 61557 report** | 61326 only | **GAP** |

---

## Vendor deliverables crosswalk (CHRONOS_EMT432 rows)

| NO. | Deliverable | STA ingest | Map verdict | Next ask Chronos |
|-----|-------------|------------|-------------|------------------|
| **1** | 61557-12 on **EMT432.V.FULL** | — | **MISS** | Accredited report or AMD1 EPMF path |
| **2** | 62053-22/24 | — | **PART** (unchanged) | Cert ID |
| **3** | 61010-1 + DoC | 61010-1 inside M213 | **PART** | **EMT432** report or **platform equivalence** + **DoC** |
| **4** | 61010-2-* | **2-030 HAVE** (EMT130/430) | **PART** | Same + #2 lab wording vs O **2-201** |
| **5** | EMC | **61326-1 HAVE** (EMT130/430) | **PART** | Confirm **Class B G1** applies to **EMT432**; optional **61557** EMC if PMD object |
| **6** | K55 / 60068 | Declared −25…+65 on 61010 | **PART** | **60068** or 61557 **Table 6** report ID |
| **7–8** | FRC / coil | — | **OPEN** | Unchanged |

---

## Architecture

**Split (unchanged):** TesPro = comms (**61557 N/A**). **Chronos analyzer** = measurement object for **#2**. **`ccli`** = PMF software, not a substitute for **61557-12** certificate.

**What STA pack proves:** Chronos **family** (EMT130/EMT430) meets **61010-2-030** and **61326-1** for **specific SNs** — **not** automatically **EMT432.V.FULL** + **FRC-350** Rogowski **DD** combo per 61557-12 Table 3 NOTE (separate type-test per sensor set).

---

## Interface matrix

| Interface | Source | Destination | Protocol / artefact |
|-----------|--------|-------------|---------------------|
| Vendor drop | Nicola / Chronos | `CHRONOS/CHRONOS/` | PDF |
| Corpus | HiTEKS | `2026-sta-reports/` | PDF + txt extract |
| Trace | Lab | O.15 #2 technical file | This map + vendor REQ |

---

## BOM matrix

| Block | Function | Candidate / PN | Status |
|-------|----------|----------------|--------|
| EMT432.V.FULL | Bench PMD front-end | Chronos | **FROZEN** — **no STA report** |
| EMT430 | Reported EUT | C25305-ACR_02 · C25306_01 | **HAVE** reports |
| EMT130 | Reported EUT | C25305-ACR_01 · C25306_01 | **HAVE** reports |
| FRC-350 | External I | Rogowski | **FROZEN** — not in STA pack |

---

## Knowledge gaps

| Area | Current (post-ingest) | Required | Gap |
|------|------------------------|----------|-----|
| Model coverage | EMT130/430 reports | **EMT432** or signed **equivalence** | **MISS** |
| 61557-12 | — | Accredited PMD/EPMF report | **MISS** |
| ACCREDIA | Mixed **No** on EMC * clauses | List which tests are accredited | **PART** |
| CE DoC | — | EU DoC for sold SKU | **MISS** |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| **R-STA-001** | File **61326** as **61557 #2 EMC** | #2 rejection | **REQ-MAP-002** · label as **61326 PART** |
| **R-STA-002** | Extrapolate **EMT430 → EMT432** without letter | Scope reject | Chronos **equivalence** or retest |
| **R-STA-003** | Duplicate **C25306_01** number across models | Audit confusion | Always cite **model + date + pages** |
| **R-STA-004** | Non-accredited (*) EMC rows as 17025 | ACCREDIA challenge | Extract per-row **Accredited** column |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-MAP-001 | PDF model field = **EMT432** or equivalence on file | Match [METER_SPECIMEN_EMT432_V_FULL.md](METER_SPECIMEN_EMT432_V_FULL.md) |
| REQ-MAP-002 | EMC report cites **61557** tables | Only if object is **PMD** |
| Ingest | `_extracts` printable chars > 5k per PDF | **HAVE** 2026-10-23 |
| RAG chunk score | `python lab/tmp/check_rag_chunk_scores.py ccli-chronos-sta-test-reports ccli-chronos-sta-annex-map` | **grade A/B**, mean ≥ **0.80**, not **MISSING** — after `scripts/ingest-chronos-sta-reports-rag.ps1` |

---

## Revision history

| Rev | Date | Note |
|-----|------|------|
| 1.0 | 2026-10-23 | First map after `CHRONOS/CHRONOS` ingest (4 PDFs) |

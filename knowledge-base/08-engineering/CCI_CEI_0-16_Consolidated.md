# CEI 0-16 Consolidated Working Edition — Corpus Index (English)

**Document ID:** CCLI-GRID-CEI-016-001  
**Revision:** 1.1  
**Date:** 2026-09-21  
**RAG source_id:** `cei-0-16-consolidata-2025-12`  
**On-disk source:** [`../07-protocols/0-16consolidata.pdf`](../07-protocols/0-16consolidata.pdf) (691 PDF pages, ~12 MB)  
**Edition:** CEI 0-16 **2025-12** consolidated working document (*versione consolidata di lavoro*)

---

## Summary

This PDF merges CEI 0-16:2022-03 with Variants **V1–V5 (2025)** and errata. It is a **working consolidation** — it does not replace the base norm + individual variants for legal certification, but is the **authoritative lab reference** for current CCI text including **ARERA 385/2025** changes.

**English engineering extracts (RAG-ready):**

| Annex | English extract | Italian raw extract |
|-------|-----------------|---------------------|
| **O** — CCI functional spec | `CCI_Annex_O_Extract.md` | `Architecture/_extracted_reg_analysis/cei_0-16_consolidated_allegato_o_it.txt` |
| **M** — Defence plan / telescatto | `CCI_Annex_M_Extract.md` | `Architecture/_extracted_reg_analysis/cei_0-16_consolidated_allegato_m_it_raw.txt` |
| **T** — IEC 61850 / cyber | `CCI_Annex_T_Extract.md` | `Architecture/_extracted_reg_analysis/cei_0-16_consolidated_allegato_t_it.txt` |
| **O/T/M verification** | `CCI_Annex_OTM_Verification.md` | PDF text extract 2026-09-21 |

Legacy CT 316 English summaries remain in `Architecture/_extracted_reg_analysis/annex_o.txt` and `annex_t.txt` — superseded for **V5** by the files above.

---

## Consolidation lineage (from PDF preface)

| Layer | Content |
|-------|---------|
| Base | CEI 0-16:2022-03 |
| V1:2022-11 | Intentional island; **Allegato T → TR 57-126 SCL reference**; Allegato U + Ubis |
| V2:2023-05 | **Allegato T** IEC 61850 exchange (consolidated) |
| V3:2024-01 | Annex Nbis storage simplification |
| V3/EC, EC2 | Material corrections |
| V4:2025 | Allegato M LTE; removed Allegati K, X |
| **V5:2025** | **Allegati O, T, U** — **ARERA 385/2025/R/EEL** (100–500 kW simplifications, teledistacco) |

Colour coding in PDF: Variants V1–V4 integrated text in **blue**; V5 in **green**.

---

## CCI-relevant annexes in this PDF

| Annex | Type | Doc. page (verified) | PDF page (verified) | English extract | Notes |
|-------|------|----------------------|---------------------|-----------------|-------|
| **M** | Normative | **310–314** | **317–321** | **HAVE** | *Partecipazione ai piani di difesa* — GSM/LTE telescatto, DI/DO → PI |
| **O** | Normative | **473–513** | **480–520** | **HAVE** | *Controllore Centrale di Impianto (CCI)* — ends O.15.5 |
| **P–S** | Informative | various | 521–540 (approx.) | — | Between O and T — not CCI core |
| **T** | Normative | **534–646** (approx.) | **541–613** (approx.) | **HAVE** | *Scambio informativo … IEC 61850* — 61850 mandatory to DSO |
| **U** | Normative | TBD | before 661 | — | Operating rules — **not extracted yet** |
| **U bis** | Normative | 654+ | **661+** | — | Intentional island generators |

---

## SCL example — **CEI TR 57-126** *(separate document, now ingested)*

Annex T §T.3 references:

**CEI TR 57-126** — *Example of SCL file for IEC 61850 communication of the CCI*  
(*Esempio di file SCL per la comunicazione IEC 61850 del CCI*)

| Item | Status |
|------|--------|
| Annex T data-model **tables** | **IN** `0-16consolidata.pdf` |
| **SCL/CID file content** | **INGESTED** — `knowledge-base/07-protocols/cei-tr-57-126.pdf` |
| `source_id` | `cei-tr-57-126` → `CCI_TR_57-126_Extract.md` |
| Example CID | `apps/ccli/config/icd/cei-tr-57-126-example.cid` · K4.3 · Phase 3 MMS |

---

## V5 (2025) highlights for CCLI programme

From Allegato O (V5 / ARERA 385/2025) — full table in `CCI_Annex_O_Extract.md` §2.1:

1. **PF2 DSO active-power limitation** is **mandatory** for wind/PV **≥ 100 kW** (no longer purely “optional class”).
2. **100 kW ≤ P < 500 kW** wind/PV with auxiliaries-only consumption: **aggregated P per source not mandatory** in O.8.
3. Same band: measurement error **≤ 5 %** permitted per Grid Code All. A.72 (O.13.2).
4. **Annex M** defence requirements remain for **≥ 100 kW** until a later regulatory variant.
5. **Teledistacco** (remote disconnect) physical predisposition shown in O.12 installation examples.

---

## Related documents still MISSING

| Document | Priority | `source_id` |
|----------|----------|-------------|
| **CEI TR 57-126** (SCL example) | — | `cei-tr-57-126` — **HAVE** (`CCI_TR_57-126_Extract.md`) |
| **ARERA 540/2021** | P0 | `arera-540-2021` — **HAVE** (`CCI_ARERA_540_2021_Extract.md`) |
| **ARERA 385/2025/R/EEL** (full text) | P1 | `arera-385-2025` |
| Allegato **M** extract (teletrip) | P1 | `cei-0-16-allegato-m` — **HAVE** (`CCI_Annex_M_Extract.md`) |
| Allegato **U** extract (operating rules) | P2 | `cei-0-16-allegato-u` |

---

## RAG routing

**Pack A — Italian CCI mandate (updated):**

```
cei-0-16-consolidata-2025-12, cei-0-16-allegato-o, cei-0-16-allegato-t, ccli-cci-module-regs-class
```

Add when available: `arera-385-2025`. Include `cei-tr-57-126` for ICD/MMS queries.

---

## Verification

| Check | Pass criteria |
|-------|---------------|
| PDF on disk | `0-16consolidata.pdf` opens; 691 pages |
| O/T/M titles | `CCI_Annex_OTM_Verification.md` — PDF pp. 317, 480, 541 |
| Annex O English | `CCI_Annex_O_Extract.md` includes V5 §2.1 |
| Annex M English | `CCI_Annex_M_Extract.md` — M.5.1 DI/DO → PI |
| Annex T English | `CCI_Annex_T_Extract.md` cites TR 57-126 |
| TR 57-126 CID | `cei-tr-57-126-example.cid` parses as well-formed XML |

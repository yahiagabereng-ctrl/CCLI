# CEI 0-16 Allegati O / T / M — PDF verification (2025-12 consolidated)

**Document ID:** CCLI-GRID-OTM-VERIFY-001  
**Revision:** 1.0  
**Date:** 2026-09-21  
**RAG source_id:** `cei-0-16-otm-verification`  
**Verified against:** [`../07-protocols/0-16consolidata.pdf`](../07-protocols/0-16consolidata.pdf) (691 PDF pages, text extract 2026-09-21)

---

## Summary

Machine verification confirms the **programme corpus** matches **CEI 0-16:2025-12** (*consolidata di lavoro*), not legacy **2012** annex titles sometimes seen in old scroll captures.

| Annex | Official title (consolidated PDF) | PDF pages | Doc. footer pages | English extract |
|-------|-----------------------------------|-----------|-------------------|-----------------|
| **M** | **Partecipazione ai piani di difesa** | **317–321** | **310–314** | **`CCI_Annex_M_Extract.md`** (new) |
| **O** | **Controllore Centrale di Impianto (CCI)** | **480–520** | **473–513** | **`CCI_Annex_O_Extract.md`** |
| **T** | **Scambio informativo basato su standard IEC 61850** | **541–613** (approx.) | **534+** | **`CCI_Annex_T_Extract.md`** |

**Offset:** PDF page ≈ doc. footer page **+ 7** (e.g. PDF 480 → footer **473**).

---

## What is **not** Allegato O / T in this edition

Some older CEI 0-16 editions or third-party summaries label annexes differently. **Do not use** these mappings for CCLI:

| Mislabel (legacy / wrong doc) | Actual in **2025-12 consolidated** |
|-------------------------------|-------------------------------------|
| Allegato O = Modbus register map to DSO | **O = CCI functional spec** (PF1/PF2/PF3, O.9 actuation, O.11 priority). DSO **setpoints** → **T (61850 MMS)**. |
| Allegato T = “type tests on monitoring only” | **T = mandatory IEC 61850 + cyber** toward DSO (`T.1`: 61850 **obbligatorio** verso DSO). |
| Allegato M = only “SPI principle diagram” (no telescatto) | **M = defence-plan participation** — GSM/LTE **telescatto** to **SPI/PI**, **DI/DO** to interface protection (**M.5.1**). |

Cover check on any photo/PDF strip: must show **CEI 0-16:2025-12** (or base **2022-03** + variants through **V5**), **not** “Terza edizione 2012-12” only.

---

## Programme split (DSO channel vs GPIO)

| Concern | Annex | CCLI lab/product |
|---------|-------|------------------|
| DSO **`Wlim`**, MMS, TLS, PKI | **T** (+ TR 57-126 CID) | Eth_A Phase 3 — **not** GD32 `dido_v2` |
| PF2 actuation, logger, O.11 priority | **O** | PF2 FSM → **DIO1** (`lab_tr400.yaml`) |
| Teledistacco / defence plan **inhibit** | **M** + **O.11** | Spare **IO3** DI (proposed) — **not** DSO Ethernet |
| Modem LTE/DI/DO class for **M** | **M.3–M.5** | **Separate** from TesPro GPIO sheet (G6K/TVS33); may need **expansion** for M voltage class |

---

## Verification method

```text
pypdf text extract → title pages PDF 317 (M), 480 (O), 541 (T)
TOC preface PDF 3–8 → "Allegato M … Partecipazione ai piani di difesa"
```

Raw Italian dump (M only): `Architecture/_extracted_reg_analysis/cei_0-16_consolidated_allegato_m_it_raw.txt`

---

## Official English Annex O (`EstrattoAllegatoO.pdf`)

| Check | Result |
|-------|--------|
| Title | **Annex O — DER Plant Controller** (*Controllore centrale di impianto*) — **correct** annex, not Modbus |
| Base norm | **CEI 0-16:2022-03** translation only (CT 316, pub. **2024-12**, 50 pages) |
| V5 / ARERA 385 / mandatory PF2 ≥100 kW | **Not in extract** (0 hits) — use `0-16consolidata.pdf` + `CCI_Annex_O_Extract.md` §2.1 |
| On disk | `Standards_pdf-20260918T124321Z-1-001/Standards_pdf/EstrattoAllegatoO.pdf` |

Public mirror (same document family): [CEI Estratto Allegato O](https://static.ceinorme.it/strumenti-online/doc/EstrattoAllegatoO.pdf)

---

## Related

- `CCI_CEI_0-16_Consolidated.md` — index (page ranges updated rev 1.1)  
- `CCI_TR400_GPIO_Schematic_Extract.md` — IO1–IO4 vs O/M  
- `lab/PHASE1.md` — no Annex T on Phase 1 bench  

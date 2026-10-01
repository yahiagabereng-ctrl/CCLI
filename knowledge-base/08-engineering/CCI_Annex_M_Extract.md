# CEI 0-16 Annex M — Defence plan / telescatto engineering extract (English)

**Document ID:** CCLI-GRID-ANNEX-M-001  
**Revision:** 1.0  
**Date:** 2026-09-21  
**RAG source_id:** `cei-0-16-allegato-m`  
**Normative source:** CEI 0-16 consolidated **2025-12** — **Allegato M** (normative), PDF pp. **317–321**, doc. pp. **310–314**  
**Companion:** `CCI_Annex_O_Extract.md` (O.11 priority) · `CCI_Annex_OTM_Verification.md` · `CCI_TR400_GPIO_Schematic_Extract.md`

---

## Summary

Annex **M** defines **participation in defence plans** (*piani di difesa*) for generation **≥ 100 kW**: remote **telescatto** via **GSM/LTE modem** to the **interface protection (SPI/PI)**, including **DI/DO** wiring to **DDI** and “external trip” inputs — **not** the IEC 61850 DSO path (that is **Annex T**).

**Corpus status:** **HAVE (English summary)** — full annex is short (~5 PDF pages); raw IT text in `Architecture/_extracted_reg_analysis/cei_0-16_consolidated_allegato_m_it_raw.txt`.

---

## 1. Scope (M.1)

- Applies to plants with total generator rated power **≥ 100 kW**.
- TSO/DSO must be able to **reduce production** for SEN security.
- DSO sends signals to an on-site **GSM/LTE receiver**; receiver commands **SPI** (**Telescatto** input) to **disconnect and inhibit** generation units.
- **One** GSM/LTE receiver per site even if **multiple SPI**; must telescatto **all** SPI.
- Alternative implementations may be agreed **DSO + Terna** at connection time (M.1 footnote).

---

## 2. Communication hardware (M.2–M.4)

| Item | Requirement |
|------|-------------|
| Modem | GSM/**LTE**, SIM slot, omni antenna |
| Interface | Optional **PI interface module** if modem has no DI/DO |
| SIM | Provided/configured by **DSO** (closed user group, CLI whitelist, etc.) |
| Aux power | Same aux supply that keeps **SPI** and **DDI hold-in** when main supply lost |
| Multiple SPI | Single modem **DO** opens **all** DDI; **DI** = **AND** of all DDI open states |

Programming (M.4): AT strings, CLI list, SMS templates for DO trip, DI confirm, diagnostics.

---

## 3. Modem I/O classes (M.3, M.3.1) — relevant to field wiring

**Modem with DI/DO or serial + interface board:**

| Signal | Class (normative text) |
|--------|-------------------------|
| **DI** | 24–48 Vdc/Vac; 115–230 Vac / 110–220 Vdc (interface board variants) |
| **DO** | Dry contact **24…265 V~**, **1 A** |

**V4 note (consolidated preface):** GPRS reference replaced by **LTE** in Annex M.

---

## 4. Installation — DI/DO to PI (M.5.1) — GPIO / CCI mapping

**DO** → PI input **“Scatto da segnale esterno”** (external signal trip / loss-of-mains path).

**DI** (feedback) — one of:

- Auxiliary contact of **DDI**;
- Current relay in series with loss-of-mains circuit opening DDI (existing plants);
- Auxiliary from **interface protection** (existing plants).

**CCI / TR400 programme:**

- Annex **O.11**: when defence-plan teletrip acts, **CCI must not take conflicting action** (see `CCI_Annex_O_Extract.md`).
- Proposed lab mapping: **IO3** as **teletrip/permissive inhibit DI** (`CCI_TR400_GPIO_Schematic_Extract.md`) — **engineering allocation**; **M modem DI/DO** is a **separate device class** from TesPro **G6K/SMBJ33** channels unless qualified.

---

## 5. Traceability hooks

| Requirement | Annex M | Programme |
|-------------|---------|-----------|
| Telescatto path | M.1, M.5 | LTE modem + PI — **product TBD** |
| DDI open feedback | M.2, M.5.1 | Optional **IO3** → PF2 inhibit |
| Priority vs PF2 | — | **O.11** overrides CCI curtail logic |

---

## 6. Not in Annex M

- IEC 61850 **`Wlim`** → **Annex T**
- CCI **PF2** actuation modes (i)–(iii) → **Annex O.9.2**
- Product **5 DI / 3 DO @ 10–120 V** → AiLux class, not CEI pin count

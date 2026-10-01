# CCI Competitors Corpus Index

**Document ID:** CCLI-COMP-001  
**Revision:** 1.0  
**Date:** 2026-09-24  
**RAG source_id:** `ccli-competitors-index`  
**Status:** **HAVE** — local PDFs under `competitors/competitors/`  
**Companions:** `CCI_Competitors_HW_Extract.md` · `CCI_Competitors_HW_Validation.md`

---

## Purpose

Ingest competitor CCI products as **behaviour / HW class references** for HiTEKS CCLI (TesPro TG544 platform). Not to clone firmware — to derive **HW validation requirements** vs AiLux-class and CEI 0-16 Annex O/T.

---

## Corpus inventory

| Vendor | Product / pack | Path | RAG `source_id` | Extract status |
|--------|----------------|------|-----------------|----------------|
| **AiLux** | CCI Tecnico + Manuale + Schema box | `competitors/competitors/AiLux/` | `ccli-comp-ailux` | **HAVE** — aligns with `ccli-cci-module-regs-class` |
| **Higeco More** | Brochure CCI v3 ENG + Compliance FAQ HTML | `competitors/competitors/Higeco/` | `ccli-comp-higeco` | **HAVE** |
| **meteocontrol (MC)** | Cabinets CCI + blue'Log XC datasheet IT | `competitors/competitors/MC(german)/` | `ccli-comp-meteocontrol` | **HAVE** |
| **Digital Platforms / STCE-SG** | STCE-SG CCI-DP leaflet | `competitors/competitors/STCE-SG CCI/` | `ccli-comp-stce` | **PART** — spaced OCR text |
| **Tesmec** | TC-CCI brochure IT | `competitors/competitors/tesmec/` | `ccli-comp-tesmec` | **HAVE** |

Raw text dumps (for re-RAG): `competitors/_extracts/` (from `scripts/extract-competitors.py`).

---

## Requirements (corpus)

| ID | Requirement | Status |
|----|-------------|--------|
| REQ-COMP-001 | All competitor PDFs indexed with `source_id` | **HAVE** |
| REQ-COMP-002 | HW fields comparable to AiLux / TR400 matrix | **HAVE** — see HW extract |
| REQ-COMP-003 | HW validation checklist for lab DUT | **HAVE** — see validation doc |

---

## Architecture note

Competitors span three patterns:

1. **Integrated module** (AiLux) — single box, 4× FE switch roles.  
2. **Cabinet solution** (Higeco, meteocontrol, Tesmec) — CPU + media converter + switch + analyzer + GNSS.  
3. **Modular RTU** (STCE-SG) — DIN modules, expandable I/O.

CCLI frozen path = **TesPro integrated gateway + SW isolation + optional FX media converter** (pattern 1 with cabinet accessories as needed).

---

## Knowledge gaps

| Area | Gap |
|------|-----|
| STCE pin-level Ethernet | Leaflet only — no datasheet with MAC/switch topology |
| AiLux internal switch vs O.13.1 | Marketing “4-port switch” — need their isolation evidence for comparison |
| Higeco “No switch/bridge” 2×Eth | Strong claim — use as validation bar for TesPro zones |

---

`competitors`, `AiLux`, `Higeco`, `Tesmec`, `meteocontrol`, `STCE`, `CCI`, `ccli-competitors-index`

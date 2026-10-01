# Standards PDF drop — inventory & gap analysis

**Document ID:** CCLI-KB-STD-INV-001  
**Revision:** 1.4  
**Date:** 2026-09-30  
**Verified:** **35** files via junction (+ **61850-8-1** 2026-09-21); **61557-12** non-zero; **61131-3** still 0 B  
**RAG source_id:** `ccli-standards-pdf-inventory`  
**Physical path:** `Standards_pdf-20260918T124321Z-1-001/Standards_pdf/` (repo sibling)  
**Corpus link:** `knowledge-base/07-protocols/regulations/standards-drop-20260918/` (junction)  
**Parent:** `ccli-cci-module-regs-class` · `ccli-knowledge-base-brief`

---

## Summary

**35 files** in the standards drop (2026-09-18 zip + **IEC 61850-8-1.pdf** added 2026-09-21). This closes many **IEC 61850 / 62351 / 62443 / 60870** PDF gaps. **Critical P0 PDFs still missing:** **62351-9**, **62351-14** (engineering extracts **HAVE**); **CEI / ARERA** Italian grid PDFs (held elsewhere in corpus). **61850-8-1** on disk is **2004 Ed.1** (doc88 OCR) — **procure 2011 Ed.2 + AMD1:2018** for conformance baseline.

**Corrupt / zero-byte in drop (re-download):** `IEC 61131-3-2025.pdf` only.  
**Fixed 2026-09-21:** `IEC 61557-12 2018.pdf` (~143 MB) — valid on disk.

**Oversize / verify integrity:** `IEC 62351-4 2020.pdf` (~485 MB — likely scan bundle; extract already in `CCI_62351-4_Extract.md`).

---

## Requirements

| ID | Requirement | Status |
|----|-------------|--------|
| REQ-STD-001 | P0 protocol PDFs indexed with `source_id` | **HAVE** (this doc) |
| REQ-STD-002 | Junction into `07-protocols/regulations/` | **HAVE** |
| REQ-STD-003 | Capture plan per new P0 PDF | **PART** — see table §3 |
| REQ-STD-004 | Engineering extract per P0 norm | **PART** — pre-existing extracts + new plans |
| REQ-STD-005 | **61850-8-1** PDF procured | **PART** — **2004 Ed.1** on disk; target **2011+AMD2018** |

---

## 1. File inventory (2026-09-18 drop)

| PDF filename | Size | Class | Priority | Extract | Capture plan | Notes |
|--------------|------|-------|----------|---------|--------------|-------|
| IEC 61850-6.pdf | 76 MB | PROTO | P0 | **HAVE** `ccli-61850-6-extract` | **HAVE** | 296 pp.; batches **A–K COMPLETE** (2026-09-21) |
| IEC 61850-7-2.pdf | 67 MB | PROTO | P0 | **HAVE** | **HAVE** `ccli-61850-7-2-extract` | MMS/ACSI |
| IEC 61850-7-3-2020.pdf | 45 MB | PROTO | P0 | **HAVE** `ccli-61850-7-3-extract` + OCR corpus | **HAVE** `ccli-61850-7-3-capture-plan` | 151 pp.; CDC — **confirmed on disk 2026-09-30** |
| IEC 61850-7-4-2010.pdf | 76 MB | PROTO | P0 | **HAVE** `ccli-61850-7-4-extract` + OCR corpus | **HAVE** `ccli-61850-7-4-capture-plan` | 185 pp.; LN/DO — **confirmed on disk 2026-09-30** |
| IEC 61850-5.pdf | 14 MB | PROTO | P1 | **PART** `ccli-61850-5-extract` | **HAVE** | Replace screenshot source |
| IEC 61850-9-2-2020.pdf | 9 MB | PROTO | P2 | — | TBD | SV — out of P0 CCI |
| IEC IEEE 61850-9-3.pdf | 3 MB | TIME | P2 | — | TBD | PTP power profile |
| IEC 61850-8-1.pdf | ~16 MB | PROTO | **P0** | **PART** `ccli-61850-8-1-extract` + `ccli-61850-8-1-ocr-corpus` | **HAVE** | **2004 Ed.1** (doc88); full OCR 2026-09-21 |
| **61850-8-2** | — | PROTO | **P2** | **PART** (TOC full; body **pp. 13–26, 30–56, 57–189, 190–222**; **W1d 27–29**, **W8 223+** open) | **HAVE** `ccli-61850-8-2-extract` rev **1.17** | **XMPP** — not CCLI P0; PDF optional |
| EN IEC 62351-3-2023.pdf | 51 MB | CYBER | P0 | **HAVE** | **HAVE** | TLS |
| IEC 62351-4 2020.pdf | 485 MB | CYBER | P0 | **HAVE** | **HAVE** | Verify file |
| IEC 62351-6-2020.pdf | 28 MB | CYBER | P0 | **HAVE** | **HAVE** | GOOSE/SV security |
| IEC TS 62351-5-2013.pdf | 44 MB | CYBER | P0 | **HAVE** | **HAVE** `ccli-62351-5-extract` | 104 security |
| IEC TS 62351-1 2007.pdf | 8 MB | CYBER | P1 | **HAVE** | **HAVE** | Overview |
| IEC 62351-7 2025.pdf | 63 MB | CYBER | P1 | **HAVE** | **HAVE** `ccli-62351-7-extract` | SNMP |
| IEC 62351-8-2020.pdf | 57 MB | CYBER | P1 | **PART** | **HAVE** | RBAC |
| **62351-9** | `08-engineering/reference/IEC_62351-9_2023.pdf` (~126 MB) | CYBER | P0 | **HAVE** extract | **HAVE** | doc88 scan; text via `CCI_62351-9_Extract.md` |
| **62351-14** | — | CYBER | P1 | **HAVE** extract | **HAVE** | **PDF not in drop** |
| IEC 60870-5-104-2006.pdf | 42 MB | PROTO | P1 | **HAVE** | **HAVE** `ccli-60870-5-104-extract` | Eth_B |
| IEC 60870-5-101-2003.pdf | 58 MB | PROTO | P2 | — | TBD | Serial |
| IEC 60870-5-103-1997.pdf | 77 MB | PROTO | P2 | — | TBD | Protection |
| IEC 61557-12 2018.pdf | ~143 MB | METRO | P1 | **PART** | **HAVE** `ccli-61557-12-capture-plan` | Upgrade extract from licensed PDF |
| IEC 61158-5-15-2010.pdf | 171 MB | PROTO | P1 | — | TBD | Modbus fieldbus ref |
| IEC 62443-3-2-2020.pdf | 26 MB | CYBER | P0 | **HAVE** zones | **HAVE** | PDF + extract |
| IEC 62443-3-3 2013.pdf | 37 MB | CYBER | P0 | **HAVE** | **HAVE** | PDF + extract |
| IEC 62443-4-1 2018.pdf | 52 MB | CYBER | P0 | **HAVE** | **HAVE** | PDF + extract |
| IEC 62443-4-2-2019.pdf | 180 MB | CYBER | P0 | **HAVE** | **HAVE** | PDF + extract |
| CRA Regulation 2024-2847-EU.pdf | 45 MB | CYBER-EU | P1 | ingested | — | Duplicate check vs corpus |
| FIPS 140-2 Security Policy.pdf | 5 MB | CYBER | P1 | — | TBD | **Policy doc** — not full FIPS 140-2 standard |
| IEEE Std 1815-2012.pdf | 259 MB | PROTO | P2 | — | — | DNP3 — out of CCLI scope |
| BS EN IEC 62541-100-2026.pdf | 52 MB | PROTO | P2 | — | — | OPC UA — out of scope |
| IEC62056-21.pdf | 44 MB | PROTO | P2 | — | — | DLMS — out of scope |
| IEEE 1588.pdf | 17 MB | TIME | P2 | — | — | PTP |
| IEEE Std C37.118-2005.pdf | 13 MB | PROTO | P2 | — | — | Out of scope |
| IEC 61131-3-2025.pdf | **0 B** | APP | P2 | — | — | **Re-download** |
| rfc8915.pdf | 11 MB | CYBER | P2 | — | — | NTS — time security |
| S7Comm Critical Infrastructure.pdf | 4 MB | PROTO | P3 | — | — | Whitepaper — not normative |
| A Guide to the EU Directive on Radio Equipment and.pdf | 18 MB | EMC | P2 | — | — | RED context |

---

## 2. Italian grid / CEI (not in this drop)

| Document | Location in CCLI corpus | Status |
|----------|-------------------------|--------|
| CEI 0-16 consolidata | `07-protocols/0-16consolidata.pdf` | **INGESTED** (691 pp., V5) |
| **EstrattoAllegatoO.pdf** | `Standards_pdf-…/Standards_pdf/EstrattoAllegatoO.pdf` (~1.4 MB, 50 pp.) | **HAVE** — CT 316 **English** Annex O, **2022-03** base, pub. 2024-12; **no V5** |
| Allegato O / T extracts | `CCI_Annex_O/T_Extract.md` | **HAVE** — O extract = CT316 + **§2.1 V5** |
| CEI TR 57-126 | extract + `.cid` | **HAVE** |
| ARERA 540/2021 | extract | **HAVE** |
| ARERA 385/2025 | — | **MISSING** |

---

## 3. P0 gap list (action now)

| # | Gap | Action |
|---|-----|--------|
| 1 | **IEC 61850-8-1** **2011 Ed.2** PDF | Upgrade from **2004 Ed.1** for lab conformance |
| 2 | **62351-9**, **62351-14** PDF | Add to library (extracts exist) |
| 3 | **61131-3** zero-byte | Re-download (**61557-12** ✓ on disk) |
| 4–7 | Extracts 7-2, 104, 62351-5/7, 61850-6 I/J/K | **DONE** 2026-09-21 |
| 8 | RAG ingest | Run `scripts/sync-ccli-corpus-to-telematry.ps1` then Telematry `ingest-ccli.ps1 -ProjectOnly` |

---

## 4. Interface matrix (PDF → CCLI function)

| CCLI function | Primary PDFs in drop |
|---------------|---------------------|
| MMS server (Eth_A) | 61850-7-2, **61850-7-3**, **61850-7-4**, 61850-6, **61850-8-1** (2004 Ed.1 + OCR corpus), 62351-3, 62351-4 |
| GOOSE (plant) | **61850-8-1** (2004), 62351-6 |
| SCL / CID | 61850-6 + TR 57-126 (corpus) |
| 104 (Eth_B) | 60870-5-104, 62351-3, 62351-5 |
| Modbus | 61158-5-15 (ref); Modbus.org optional |
| Metrology blocks | 61557-12 (licensed PDF **HAVE**; extract **PART**) |
| Cyber cert story | 62443-3-2/3-3, 62443-4-1/4-2, CRA |

---

## 5. Verification

| Check | Pass |
|-------|------|
| Junction `standards-drop-20260918` lists **35** files | `dir` junction |
| Inventory MD matches filename list | Diff script |
| Classification doc §3 updated | `ccli-cci-module-regs-class` rev **1.2** |
| P0 PDF gaps = 62351-9/14 + **61131-3** 0 B; **8-1 edition** upgrade documented | This §3 |

---

## Keywords

`standards`, `61850`, `62351`, `62443`, `60870`, `inventory`, `ccli-standards-pdf-inventory`

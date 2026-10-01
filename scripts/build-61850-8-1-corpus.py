#!/usr/bin/env python3
"""Build 61850-8-1 OCR corpus MD + engineering extract stub from per-page OCR."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OCR = ROOT / "Architecture" / "_extracted_reg_analysis" / "_pdf_ocr" / "61850-8-1"
KB = ROOT / "knowledge-base" / "08-engineering"
PDF_REF = (
    "07-protocols/regulations/standards-drop-20260918/IEC 61850-8-1.pdf"
)


def build_ocr_corpus() -> Path:
    out = KB / "CCI_61850-8-1_OCR_Corpus.md"
    parts = [
        "# IEC 61850-8-1 — Full OCR corpus (2004 Ed.1)\n",
        "**RAG source_id:** `ccli-61850-8-1-ocr-corpus`  \n",
        "**Purpose:** Verbatim OCR of all PDF file pages for search/RAG; verify critical tables against licensed PDF.  \n",
        f"**OCR path:** `Architecture/_extracted_reg_analysis/_pdf_ocr/61850-8-1/`  \n",
        f"**Licensed PDF:** `{PDF_REF}`  \n\n---\n\n",
    ]
    for p in sorted(OCR.glob("p*.txt")):
        text = p.read_text(encoding="utf-8", errors="replace")
        parts.append(f"\n\n## File page {p.stem[1:]}\n\n```\n{text}\n```\n")
    out.write_text("".join(parts), encoding="utf-8")
    return out


def build_extract() -> Path:
    out = KB / "CCI_61850-8-1_Extract.md"
    n = len(list(OCR.glob("p*.txt")))
    body = f"""# IEC 61850-8-1 — CCLI Engineering Extract (MMS + GOOSE)

**Document ID:** CCLI-PROTO-61850-8-1-001  
**Revision:** 1.0  
**Date:** 2026-09-21  
**Edition:** IEC 61850-8-1 **First edition 2004** (`© IEC:2004(E)`) — doc88/Print-to-PDF; **procure 2011 Ed.2 + AMD1:2018** for conformance baseline  
**RAG source_id:** `ccli-61850-8-1-extract`  
**Normative basis:** IEC 61850-8-1 — *SCSM — MMS (ISO/IEC 9506) and ISO/IEC 8802-3 (GOOSE)*  
**Licensed PDF:** `{PDF_REF}` ({n} file pages OCR)  
**Full text:** `ccli-61850-8-1-ocr-corpus` → `CCI_61850-8-1_OCR_Corpus.md`  
**Capture status:** **HAVE (P0 corpus)** — **full PDF OCR** 2026-09-21; engineering summary **PART** (verify tables at lab)  
**Parents:** `ccli-61850-8-1-capture-plan` · `ccli-61850-7-2-extract` · `ccli-61850-6-extract` · `cei-tr-57-126`  
**Programme link:** Eth_A **8-MMS** · TCP **102** · **62351-3/4** on C1

---

## Summary

**61850-8-1** maps **61850-7-2 ACSI** to **ISO/IEC 9506 MMS** (client/server over **TCP/IP** or OSI) and **802.3** **GOOSE/GSE**. **CCLI / TR 57-126** require this part — **not** 61850-8-2 (XMPP).

This extract is backed by **complete per-page OCR** of the user PDF (`61850-1.pdf` → standards drop). OCR quality varies (doc88 scan); use **`ccli-61850-8-1-ocr-corpus`** for full-text search.

---

## Requirements (CCLI — from 2004 Ed.1 OCR)

| ID | Requirement | Source | CCLI |
|----|-------------|--------|------|
| REQ-881-TCP-001 | Client/server **A-Profile** shall implement **TCP/IP T-Profile** as minimum | §6.2.2 p.23 OCR | Eth_A |
| REQ-881-TCP-002 | **Table 4** — TCP/IP stack: **TCP**, **IP**, **ICMP**, **802.3** CSMA/CD | §6.2.3 p.24 OCR | Plant + DSO ports |
| REQ-881-TCP-003 | **TCP_KEEPALIVE** per RFC 793; configurable; PIXIT range (recommended 1–20 s) | §6.2.3 OCR | Firewall/NAT behaviour |
| REQ-881-MMS-001 | **Table 1** — MMS objects: VMD, Named Variables (LN/data), NVL (DataSet), Journal, File | §5 / Table 1 p.21 OCR | `mms_adapter` |
| REQ-881-ASSOC-001 | **Two-party association** — Associate / Release / Abort; **Table 20–21** errors | §10 OCR | 62351-4 E2E wraps MMS |
| REQ-881-ADDR-001 | **Table 111** — allowed **P-Type** definitions for client/server **Address** in SCL | §25 p.109 IEC (TOC) | **61850-6** Batch G′ gap |
| REQ-881-GOOSE-001 | **GOOSE** mapping §18; **Annex A** APDU; **Annex C** 8802-3 frame / **Ethertype** | §18, Annex A/C OCR | K4.2 subscribe |
| REQ-881-CTL-001 | **§20** control — Select / Operate / Cancel / **AddCause** (**Table 77**) | §20 OCR | PF2 curtailment |
| REQ-881-RPT-001 | **§17** BRCB/URCB / Report — **Tables 37–40** | §17 OCR | TR **intgPd** reporting |
| REQ-881-PICS-001 | **§24** PICS **Tables 85–108** (+ GOOSE **109–110**) | §24 OCR | Phase 3 lab declaration |

---

## Document structure (2004 Ed.1 — TOC ingested)

| § | Topic | IEC pp. (TOC) |
|---|--------|---------------|
| **1–4** | Scope, refs, terms | 11–17 |
| **5–6** | Overview, **communication stack**, **Table 4 TCP/IP** | 18–29 |
| **7–8** | 61850 objects; **7-2/7-3** attribute mapping | 30–36 |
| **9–14** | Server, association, LD/LN, data, **DataSet** | 37–49 |
| **15–17** | Substitution, **SGCB**, **Report/Log** | 50–61 |
| **18** | **GOOSE/GSSE** | 62–80 |
| **19–20** | SV, **Control** | 81–89 |
| **21–23** | Time sync, naming, **files** | 89–94 |
| **24–25** | **PICS**, SCL / **Table 111** | 94–109 |
| **Annex A,C,E,G** | GOOSE APDU, 802.3, control CDCs, MMS types | 111–131 |

---

## §6 Communication stack (OCR highlights)

- **Figure 2** — OSI layers vs **A-Profile** / **T-Profile** (MMS, TCP/IP, GOOSE profiles).
- **Table 2** — services requiring client/server profile (directory, read/write, datasets, reports, control, files, …).
- **Table 3** — **A-Profile**: MMS + ACSE + presentation/session over TCP/IP or OSI.
- **Table 4** — **TCP/IP T-Profile** mandatory minimum: **TCP**, **IP**, **Ethernet 8802-3** (see OCR file page **027**).
- **Note:** Well-known MMS port **102** is industry practice; confirm in **2011 Ed.2** / PIXIT if not visible in this OCR slice.

---

## Annex T / TR 57-126 parity (MMS services)

| Annex T ACSI use | 8-1 § (2004 TOC) |
|------------------|------------------|
| Listobjects / directory | §9.3, §12.3, §13 |
| Readvalues | §13.2 |
| Dataset read | §14.2 |
| Reporting | §17 |
| CONTROL | §20, Annex E |
| GOOSE subscribe (Type 1) | §6.3, §18, Annex A |

---

## Knowledge gaps

| Gap | Mitigation |
|-----|------------|
| **2004 vs 2011 Ed.2** | Procure **2011+AMD2018** for product conformance |
| OCR table garble | Re-read **`ccli-61850-8-1-ocr-corpus`** or licensed PDF for **Table 111**, PICS |
| **62351-4** port **3782** | In **62351-4** extract, not 8-1 |

---

## Verification

| Check | Pass |
|-------|------|
| PDF in standards drop | `IEC 61850-8-1.pdf` |
| OCR pages | **{n}** files under `_pdf_ocr/61850-8-1/` |
| RAG extract + corpus | `ccli-61850-8-1-extract`, `ccli-61850-8-1-ocr-corpus` |

---

## Keywords

`61850-8-1`, `MMS`, `GOOSE`, `TCP`, `Table 4`, `Table 111`, `2004`, `ccli-61850-8-1-extract`, `ccli-61850-8-1-ocr-corpus`
"""
    out.write_text(body, encoding="utf-8")
    return out


if __name__ == "__main__":
    c = build_ocr_corpus()
    e = build_extract()
    print(c, e, sep="\n")

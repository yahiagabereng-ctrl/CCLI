# IEC 61850-8-1 — Capture Plan (MMS + GOOSE mapping)

**Document ID:** CCLI-PROTO-61850-8-1-001  
**Revision:** 1.2  
**Date:** 2026-09-21  
**RAG source_id:** `ccli-61850-8-1-capture-plan` → `ccli-61850-8-1-extract`  
**Normative basis:** IEC 61850-8-1 — *SCSM — Mappings to MMS (ISO/IEC 9506-1/9506-2) and ISO/IEC 8802-3*  
**Licensed PDF target:** `07-protocols/regulations/standards-drop-20260918/IEC 61850-8-1.pdf` (or `iec-61850/` folder)  
**Programme link:** K4.1 · K4.2 · Eth_A MMS **TCP 102** / **62351-3 TLS 3782** · plant GOOSE

---

## Summary

**61850-8-1** maps **61850-7-2 ACSI** to **MMS** (client/server) and **802.3** (GOOSE/GSE). **CCLI Eth_A / TR 57-126** require **`8-MMS`**, not 61850-8-2 (XMPP).

**Edition note (important):**

| Edition | Footer example | CCLI use |
|---------|----------------|----------|
| **Ed.1** | `61850-8-1 © IEC:2004(E)` | User PDF (~**140** pp.) — OK for **screenshot ingest**; tag extract **`2004-Ed1`** |
| **Ed.2** | `2011` (+ **AMD1:2018**) | **Preferred** product/conformance baseline — procure when possible |

Page numbers below match **2004 Ed.1** TOC from user capture (Contents through List of Tables). **Edition 2** section numbers are similar but **page numbers differ** — always shoot the **IEC footer** (`- n -`), not PDF viewer page.

**Do not confuse with** `ccli-61850-8-2-extract` (XMPP).

---

## Capture status

| Batch | Status |
|-------|--------|
| **T** | **TOC COMPLETE** (2026-09-21) — Contents §1–25 + Annexes A–G index; List of Figures; List of Tables **1–112** (+ annex tables) |
| **S0–S8** | **COMPLETE (OCR)** — all **142** file pages via `scripts/extract-61850-8-1-ocr.py` |

---

## Screenshot rules (same as 61850-8-2 batch W)

- Use **IEC footer page number** (`61850-8-1 © IEC:2004(E)` … `- n -`), not file page index.
- **One full page per photo**; footer visible.
- Wide **tables** (PICS **94–107**, **Tables 14–16**, **111**): add one **zoom crop** if needed.
- Optional save path: `Architecture/_extracted_reg_analysis/iec_61850-8-2/` → use **`iec_61850-8-1/`** instead:

```
Architecture/_extracted_reg_analysis/iec_61850-8-1/
  S1/p011.png … p029.png
  S5/p109_t111.png   # Table 111 P-Types
```

Drop batches in chat by label (**S1**, **S3**, …) or save under the path above.

---

## Batch **T** — already captured (do not re-shoot unless blurry)

| PDF pp. | Content |
|---------|---------|
| **1–7** | Cover, **Contents** §1–13, §14–25, Annexes A–G; **List of Figures**; **List of Tables** 1–112 (+ B.1, C.1–C.2, D.1–D.4, E.1–E.13, F.1–F.2) |

---

## Required screenshots — normative body

### Option A — minimum **P0** (Eth_A MMS + Annex T + Table 111)

Shoot these **IEC pages** first (~**85** pages). Closes `mms_adapter`, association, read/dataset/report/control, **P-Type** addressing, TCP profile.

| Order | Batch | IEC pages | Clauses / focus | CCLI |
|-------|-------|-----------|-----------------|------|
| 1 | **S1** | **11–29** | §1–§6 — scope, **Table 2–5** services, **TCP/IP T-Profile (Table 4)**, OSI profile, **time sync (Table 12–13)** | Port **102**, stack |
| 2 | **S2** | **30–45** | §7–§13 — objects, **§8** 7-2/7-3 mapping (**Tables 14–16**), server/LD/LN/**GetServerDirectory**, **Associate/Release (Tables 20–21)**, **Get/SetDataValues** | Annex T read path |
| 3 | **S3** | **46–61** | §14–§17 — **DataSet** CRUD/read (**Tables 28–35**), SG (**36**), **BRCB/URCB/Report (37–40, 52–53)**, logs (**41–49**) | TR **Reporting**, dataset read |
| 4 | **S4** | **62–89** | §18 **GOOSE** start (**50–56, 62–70**), §20 **control (67–77, 83–89)**, §21–23 start if on same pages | PF2 **Operate**; optional GOOSE subscribe |
| 5 | **S5a** | **89–94** | §21–§23 **file transfer** (**Figs 16–17**, **Tables 80–84**) | Signed CID / firmware policy |
| 6 | **S5b** | **94–109** | §24 **PICS** (**Tables 85–108** — at least **85–89, 95–97**), §25 SCL pointer, **Table 111 P-Type addressing** | Lab PICS + **61850-6 Address** gap |
| 7 | **S6** | **111–116** | **Annex A** GOOSE/GSE APDU (normative), **Annex C** **8802-3** frame + **Table C.2 Ethertype** | Plant GOOSE wire |

### Option B — **full standard** (recommended archive)

Photograph **every IEC page from 8 through last page** (Foreword through **Annex G**, ~**8–131+** in 2004 Ed.1), except duplicate TOC if batch **T** is sharp.

| Batch | IEC pages (2004 Ed.1 TOC) | Content |
|-------|---------------------------|---------|
| **S0** | **8–10** | Foreword, Introduction |
| **S1** | **11–29** | §1–§6 communication stack |
| **S2** | **30–45** | §7–§13 models + data services |
| **S3** | **46–61** | §14–§17 dataset, SG, report, log |
| **S4** | **62–80** | §18 GOOSE + **§18.2 GSSE** (legacy) |
| **S5** | **81–89** | §19 SV, §20 control, §21 time |
| **S6** | **89–94** | §22–§23 naming, files |
| **S7** | **94–109** | §24–§25 conformance, **Table 111** |
| **S8** | **111–131** | **Annex A, B, C, D, E, F, G** (normative **A, C, E, G**) |

**Overlap** at **89**, **94** — shoot each physical page once.

---

## Priority map vs CEI **Annex T** (P0 services)

| Annex T need | 8-1 sections (2004 TOC) | Batch |
|--------------|-------------------------|-------|
| MMS association + Type 2/3 performance | §5.3, §6.2, §10, §24 | S1, S2, S5b |
| **Listobjects** / directory | §9.3, §12.3, §13 | S2 |
| **Readvalues** | §13.2, Table 24 | S2 |
| **Dataset** (read; create/delete N/A on TR) | §14 | S3 |
| **Reporting** (URCB/BRCB) | §17, Tables 37–40 | S3 |
| **CONTROL** | §20, Tables 67–77, Annex E | S4, S8 |
| **Communication / P-Type** in CID | §25, **Table 111** | S5b |
| GOOSE subscribe (Type 1) | §6.3, §18, Annex A/C | S4, S6, S8 |

---

## Defer / lower priority (still in full Option B)

| Topic | Pages (2004) | Note |
|-------|--------------|------|
| **GSSE** | §6.4, §18.2, Tables 9–11, 57–65, 110 | Legacy; not TR core |
| **Substitution** | §15 p.50 | Rare on CCI |
| **Annex B** multicast example | 113 | Informative |
| **Annex D/F** SCL/time | 117+, 128+ | Mostly **61850-6** extract |
| **Sampled values §19** | 81 | P2 unless SV required |

---

## Planned extract batches (when screenshots ingested)

| Batch | Focus | CCLI use |
|-------|--------|----------|
| **A** | §6.2, Table 4 TCP, §10 Associate | `mms_adapter` Eth_A |
| **B** | §18, Annex A/C, Table C.2 | K4.2 GOOSE |
| **C** | **Table 111**, §25 | **61850-6** `tPTypeEnum` |
| **D** | §19 SV | Defer |

---

## Verification

| Check | Pass |
|-------|------|
| TOC batch **T** in chat or repo | **HAVE** (2026-09-21) |
| **S5b** includes **Table 111** (p. **109**) | P-Type list captured |
| Extract `ccli-61850-8-1-extract` | Published after **S1–S5b** minimum |
| Edition tagged in extract header | **2004-Ed1** vs **2011-Ed2** explicit |
| Distinction from **8-2** | Classification + this plan |

---

## Keywords

`61850-8-1`, `MMS`, `GOOSE`, `2004`, `2011`, `screenshot`, `ccli-61850-8-1-capture-plan`

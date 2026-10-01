# IEC 61850-8-2 — Capture Plan (XMPP SCSM)

**Document ID:** CCLI-PROTO-61850-8-2-001  
**Revision:** 1.18  
**Date:** 2026-09-21  
**Edition:** IEC 61850-8-2:2018 (per user TOC)  
**RAG source_id:** `ccli-61850-8-2-capture-plan` → `ccli-61850-8-2-extract`  
**Normative basis:** IEC 61850-8-2 — *Specific communication service mapping (SCSM) — Mapping to Extensible Messaging and Presence Protocol (XMPP)*  
**Licensed PDF target:** `07-protocols/regulations/standards-drop-20260918/IEC 61850-8-2.pdf` (place file in Standards_pdf folder + junction)  
**Programme link:** K4.1 optional · **CCLI Eth_A uses 8-MMS (8-1), not XMPP** per TR 57-126

---

## Summary

**61850-8-2** maps **61850-7-2 ACSI** to **XMPP** (TLS, SASL, XML payloads, XEP 0198/0199). **Not** the same as **61850-8-1** (MMS + GOOSE on 8802-3).

**CCLI priority:** **P2** — document for datasheet completeness; **Annex T / TR 57-126** specify **`SubNetwork type="8-MMS"`** only.

**TOC ingested from user capture 2026-09-21** (Contents through §20.1, doc page 4 of TOC).

---

## Capture batches

| Batch | § / pages (TOC) | Focus | Status |
|-------|-----------------|-------|--------|
| **A** | §1–4 pp. 16–21 | Scope, namespace, code distribution, refs, terms | **COMPLETE** → extract §1–4 |
| **B** | §5 pp. 22–26 | Overview; client/server mapping; XML; XMPP; time sync | **COMPLETE** → extract §5 |
| **C** | §6 pp. 26–28 | XMPP connection, TLS/SASL, ACSI mapping, presence, roster, XEP 0198/0199 | **COMPLETE** → extract §6 |
| **D** | §7 p. 29 | End-to-end security | **COMPLETE** → extract §7 |
| **E** | §8.1–8.3.3 pp. 29–43 | XSD; LN/DO/DA → VariableAccessSpecifications; BasicTypes | **COMPLETE** → extract §8 |
| **F** | §8.3.4–8.5 pp. 54–56 | Quality mapping; XML values; bandwidth optimization | **COMPLETE** → extract §8 cont. |
| **G** | §9–11 pp. 56–69 | Server directory; association E2E; logical device | **COMPLETE** → extract §9–11 |
| **H** | §12–14 pp. 70–98 | LN directory; GetAllDataValues; DO/DA; DataSet CRUD | **COMPLETE** → extract §12–14 |
| **I** | §14.6.1–§16 pp. 98–109 | DataSet response; ServiceTracking; Setting groups | **COMPLETE** → extract §14–16 |
| **J** | §17 pp. 110–130 | BRCB/URCB, Report, Get/Set RCB, Log/LCB | **COMPLETE** → extract §17 |
| **K** | §18–20.1 pp. 130–132 | GOOSE, SV, Control model start | **COMPLETE** → extract §18–20 |
| **L** | §20.2–20.10 pp. 134–152 | Select, Operate, Cancel, TimeActivated, CTS | **COMPLETE** → extract §20 |
| **M** | §21–23 pp. 153–164 | Time sync, naming, **file transfer** | **COMPLETE** → extract §21–23 |
| **N** | §24–25 pp. 166–189 | PICS, XML payload, SCL pointer | **COMPLETE** → extract §24–25 |
| **O** | Annex A pp. 190–192 | XMPP communication stack profiles | **COMPLETE** → extract Annex A |
| **P** | Annex B–G + Figures index (TOC p. 6) | B deploy, C DER security, **D normative stanzas**, F SCL, **G XSD** | **COMPLETE** → extract Annex B–G |
| **Q** | Figures 13–55 index (TOC p. 7) | XML Req/Resp through SelectActiveSG | **COMPLETE** → extract Figures § |
| **R** | Figures 56–97 (TOC p. 8) | SG, Report/BRCB, Log, Control, File XML | **COMPLETE** → extract |
| **S** | Figs 98–102 + Annex A/B/C figure index (TOC p. 9) | VariableSpec + DER/XMPP diagrams | **COMPLETE** → extract |
| **T** | Tables 1–51 (TOC pp. 9–10) | Encodings, service params, errors, ServiceType | **COMPLETE** → extract (gaps 22–25, 27 if any) |
| **U** | Tables 52–94 (TOC p. 11) | RCB, Log, GoCB, Control, File, PICS | **COMPLETE** → extract |
| **V** | Tables 95–123 + Annex A/D/F table index (TOC p. 12) | T-Profile PICS, MMS ref CS, P-Type, stanzas, SCL | **COMPLETE** → extract |
| **W** | Normative body + Annex G XSD OCR | Screenshots (2026-09-21) | **PARTIAL** — **W2–W7 + W3b COMPLETE**; **W1d** (27–29) + **W8** (223–end) pending |

### Batch **W** — screenshot sub-batches (normative PDF pages)

**Skip re-capture:** Contents + List of Figures/Tables **pp. 1–12** (batches **A–V** already in extract).

Use **printed page footer** (`© IEC 2018` page number), not camera roll order. One full page per image; include header/footer. For wide **tables**, one extra crop at readable zoom is fine.

| Sub-batch | PDF pp. | Priority | Ingest focus |
|-----------|---------|----------|----------------|
| **W1** | **16–29** | P2 | §1–7 — **PARTIAL** (**13–26 HAVE**; **27–29** pending) |
| **W2** | **30–56** | P2 | §8 — **COMPLETE** (screenshot 2026-09-21) |
| **W3** | **57–98** | P2+ | §9–§14.6.1 — **COMPLETE** (screenshot 2026-09-21) |
| **W3b** | **99–101** | P2+ | §14.6.2, §15 — **COMPLETE** (2026-09-21) |
| **W4** | **102–131** | P2+ | §15–§18.4 SG, reports, logs, GOOSE — **COMPLETE** (2026-09-21) |
| **W5** | **132–152** | P2 | §19 SV, §20 control (**Tables 66–79**) — **COMPLETE** (2026-09-21) |
| **W6** | **153–189** | P2 | §20.10–§25 CTS, files, **PICS**, **AltAccess**, **Table 123** — **COMPLETE** (2026-09-21) |
| **W7** | **190–222** | **P1 corpus** | **Annex A** profiles, **Annex D** normative **stanza** maps (**D.1–D.2**) — **COMPLETE** (2026-09-21) |
| **W8** | **223–end** | P2 | Annex **F** SCL, Annex **G** XSD listings |

**CCLI product P0** remains **61850-8-1**; **W3–W4** and **W7** are highest value for RAG cross-ref with **7-2** / **62351-4** / TR Annex T.

### File naming (screenshots)

```
Architecture/_extracted_reg_analysis/iec_61850-8-2/
  W1/p016.png … p029.png
  W7/p220_tD1.png    # page + primary table (Annex D.1)
```

Drop batches in chat (as TOC photos) or save under the path above; either works for ingest.

---

## Verification

| Check | Pass |
|-------|------|
| PDF `IEC 61850-8-2.pdf` in standards drop | User to add file |
| Extract `ccli-61850-8-2-extract` | **HAVE** rev **1.17** — screenshot body **pp. 13–26, 30–222** (gaps **27–29**, **223+**) |
| Distinction from **8-1** documented | Classification + Pack C note |

---

## Keywords

`61850-8-2`, `XMPP`, `SCSM`, `ccli-61850-8-2-capture-plan`

# Masomeh — Chronos S.T.A. reports · annex mapping · RAG corpus (2026-10-23)

**Date:** 2026-10-23  
**From:** Nicola / PC-A programme  
**Audience:** Masomeh (PC-B / TSP) · procurement awareness · **do not close O.15 #2 on meter**  
**Bench meter (PC-A):** **EMT432.V.FULL** + FRC-350 — see [00u_METER_SPECIMEN_EMT432.md](CCLI_TSP_FIELD_PACK/00u_METER_SPECIMEN_EMT432.md) after field pack stage  
**Canonical lab maps:** `lab/conformance/CHRONOS_STA_REPORTS_ANNEX_MAP.md` · `lab/evidence/CHRONOS_RAG_CHUNK_REPORT.json`

You **capture MMS / logger evidence** when PC-A runs EMT432 yaml. You **do not** sign Chronos type-test or annex #2 — use this pack so unified matrix / Full Validate rows stay **OPEN** where required.

---

## Read order (Chronos update)

| # | Document | Purpose |
|---|----------|---------|
| 1 | **[MASOMEH_EMT432_TESPRO_REQUIREMENTS.md](MASOMEH_EMT432_TESPRO_REQUIREMENTS.md)** | EMT432 + TesPro index |
| 2 | **This file** | STA reports vs **Annex O** / **61557-12** |
| 3 | **[CHRONOS_EMT432_VENDOR_REQUIREMENTS.md](CHRONOS_EMT432_VENDOR_REQUIREMENTS.md)** | Vendor ask list (rows 1–8) |
| 4 | Field pack **`00q2_CHRONOS_STA_ANNEX_MAP.md`** | Full mapping tables (copy of lab map) |
| 5 | **`00q3_CHRONOS_RAG_CHUNK_REPORT.json`** | Corpus ingest 22/22 PASS (not annex credit) |
| 6 | **[00v_61557-12_TESPRO_VENDOR_MAP.md](CCLI_TSP_FIELD_PACK/00v_61557-12_TESPRO_VENDOR_MAP.md)** | TesPro ≠ meter #2 |

---

## S.T.A. reports received (Chronos drop)

| Report | Model tested | Standard | File (repo) |
|--------|--------------|----------|-------------|
| **C25305-ACR_01** | **EMT130** | **EN IEC 61326-1:2021** EMC | `knowledge-base/.../chronos/2026-sta-reports/C25305-ACR_01_EMT130_EMC_61326-1.pdf` |
| **C25305-ACR_02** | **EMT430** | Same | `.../C25305-ACR_02_EMT430_EMC_61326-1.pdf` |
| **C25306_01** | **EMT130** | **61010-1 + 61010-2-030** (issue 26/06/2026) | `.../C25306_01_EMT130_61010-2-030.pdf` |
| **C25306_01** | **EMT430** | Same (issue 01/07/2026) | `.../C25306_01_EMT430_61010-2-030.pdf` |

**Not in pack:** any report naming **EMT432** or **IEC 61557-12**.

---

## Annex O — confirmation matrix (meter / O.15 #2)

| O hook | Shall (short) | STA evidence | vs **EMT432 bench** | TSP / matrix |
|--------|---------------|--------------|---------------------|--------------|
| **O.15 §12 #2 · 61557-12** | PMD/EPMF type-test | **None** | **GAP** | Keep **OPEN** — never PASS on 62053 or 61326 |
| **O.15 §12 #2 · 61010** | Measuring equipment safety | C25306_01 (130/430) | **PART** — wrong SKU | **OPEN** until EMT432 or equivalence letter |
| **O.15 §12 #2 · EMC** | 61557 influence or PMD bundle | C25305 (61326-1) | **PART** — not 61557 Tables 9/13/17 | **OPEN** for #2 EMC on 61557 object |
| **O.15 §12 #2 · K55** | Climate ≥ K55 | Declared −25…+65 °C on 61010 forms | **PART** | **OPEN** without 60068 / Table 6 report |
| **O.13.2.1** | ≤0,2 / ≤0,5 · 61557 method | Not in STA | **GAP** | Your MMS checks ≠ accredited class |
| **O.7.4 / O.8.3** | Annex **D** blocks · 4 s UTC | Not in STA | **HiTEKS SW GAP** | **U-T1-05/06** may stay **PART** |

**TesPro TG544:** comms-only — **61557 N/A** on gateway ([00v_61557-12_TESPRO_VENDOR_MAP.md](CCLI_TSP_FIELD_PACK/00v_61557-12_TESPRO_VENDOR_MAP.md)).

---

## IEC 61557-12 annexes (meter object — for #2 lab narrative)

| Annex / clause | Role | Chronos STA | Programme |
|----------------|------|-------------|-----------|
| **D** (Fig D.2 fixed block) | Normative demand | Not tested | **GAP** — O cites D; not Annex B |
| **H** (AMD1 EPMF) | SS / PMF | Not referenced | **MISS** |
| **§4.10 + 61010-2-030** | Safety | C25306 on 130/430 | **PART** for EMT432 |
| **Tables 3/8/9/13/17** | PMD class + EMC | No 61557 report | **GAP** |

---

## Vendor deliverables — sign-off (Chronos owes HiTEKS)

| # | Deliverable | Status after STA | Masomeh action |
|---|-------------|------------------|----------------|
| 1 | **61557-12** on **EMT432.V.FULL** | **MISS** | Do not mark #2 metrology PASS |
| 2 | 62053-22/24 | **PART** | Do not equate to row 1 |
| 3–4 | 61010 + DoC | **PART** (130/430 only) | **OPEN** |
| 5 | EMC | **PART** (61326) | Do not file as 61557 PMD EMC |
| 6 | K55 / 60068 | **PART** | **OPEN** |
| 7–8 | FRC class · mV/kA | **OPEN** | — |

---

## RAG corpus (Telematry — PC-A only)

All Chronos PDF + MD ingested **2026-10-23** via `scripts/ingest-chronos-all-rag.ps1` (requires RAG `:8001`).

| Kind | Pass rule | Result |
|------|-----------|--------|
| **13 × MD** | Chunks present | **PASS** |
| **9 × PDF** | `chunk_quality_score` mean ≥ 0.50 | **PASS** (MR **grade A**, STA EMC/61010 **grade B**) |

Machine report: **`lab/evidence/CHRONOS_RAG_CHUNK_REPORT.json`** (field pack copy **`00q3_...`**).

**Important:** RAG PASS = corpus quality only. It **does not** close annex **GAP** rows above.

---

## What you still do on bench (unchanged)

| When | Capture |
|------|---------|
| EMT432 yaml session | MMS **TotW / TotVAr / PPV** vs meter (P5-04 / Compare) |
| 4 s reporting | Note **PART** if not UTC block-aligned |
| Logger | Four artefacts per **00h_PC1_LOG_CAPTURE_REQUIREMENTS.md** |

PC-A deploy:

```powershell
powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1
python lab\modbus_emt432_tcp_probe.py --host 192.168.178.102
```

---

## Git / field pack

```powershell
cd C:\Yahia\projects\CCLI
git pull origin main
powershell -File scripts\stage-tsp-field-pack.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

New files: **`00q2_CHRONOS_STA_ANNEX_MAP.md`**, **`00q3_CHRONOS_RAG_CHUNK_REPORT.json`**, **`00q4_MASOMEH_CHRONOS_STA_ANNEX_HANDOFF.md`**.

Questions → Yahia / project channel.

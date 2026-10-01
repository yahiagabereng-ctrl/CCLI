# Engineering Knowledge Base

Cross-cutting project knowledge — architecture, lab platform, certification, validation.

**Master download paths:** [`../DOWNLOADS.md`](../DOWNLOADS.md)

## In this folder

| File | RAG `source_id` | Status |
|------|-----------------|--------|
| `CCI_SOC_Freeze.md` | `ccli-soc-freeze` | **FROZEN** — MediaTek MT798X final product SoC |
| `CCI_OpenWrt_Freeze.md` | `ccli-openwrt-freeze` | **FROZEN** — OpenWrt OS; Pi test path |
| `CCI_OpenWrt_Capture_Plan.md` | `ccli-openwrt-capture-plan` | **HAVE** — procedural doc capture batches |
| `CCI_OpenWrt_Procedural_Extract.md` | `ccli-openwrt-procedural-extract` | **HAVE (P0)** — UCI · procd · ipk · libgpiod |
| `CCI_OpenWrt_Buildsystem_Extract.md` | `ccli-openwrt-buildsystem-extract` | **HAVE (P0)** — build host · SDK · Pi3 paths |
| `CCI_OpenWrt_Dependencies_Extract.md` | `ccli-openwrt-dependencies-extract` | **HAVE (P0)** — DEPENDS / PKG_BUILD_DEPENDS |
| `openwrt-sources/` | (raw downloads) | Archive wiki + GitHub upstream |
| `CCI_TG500_Lab_Platform.md` | `ccli-tg500-lab-platform` | **FROZEN** — TG-524 / **TR400 on bench** (Rev 3.0) |
| `CCI_CCLI_Product_Spec_and_Roadmap.md` | `ccli-product-spec-roadmap` | Master SW/HW spec, reqs, roadmap |
| `CCI_Competitors_Index.md` | `ccli-competitors-index` | **HAVE** — AiLux · Higeco · meteocontrol · STCE · Tesmec |
| `CCI_Competitors_HW_Extract.md` | `ccli-competitors-hw-extract` | **HAVE** — competitor HW fields from `competitors/` PDFs |
| `CCI_Competitors_HW_Validation.md` | `ccli-competitors-hw-validation` | **HAVE** — V-NET/IO/TIME/SEC lab validation vs TG544 |
| `CCI_Phase_Regulation_Checklists.md` | `ccli-phase-regulation-checklists` | **HAVE** — P0–P7 shall/pass tables from O/T/M + 62351/62443 |
| `CCI_TestSuitePro_Capture_Plan.md` | `ccli-testsuite-pro-capture-plan` | **HAVE** — Triangle TSP download paths + batches |
| `CCI_TestSuitePro_Extract.md` | `ccli-testsuite-pro-extract` | **HAVE** — TSP↔CCLI gate map (v4.7.4) |
| `testsuite-pro/` | `ccli-testsuite-pro-raw` / `…-help` | raw web + offline help drop |
| `CCI_TR400_Capture_Plan.md` | `ccli-tr400-capture-plan` | **PLAN** — OEM user manual capture batches |
| `CCI_TR400_Extract.md` | `ccli-tr400-user-manual-extract` | **HAVE** — 49 pp. auto-extract (v1.0 EN) |
| `CCI_TR400_GPIO_Schematic_Extract.md` | `ccli-tr400-gpio-schematic-extract` | **HAVE** — GD32 DIO vs REQ-IO / Allegato O (start) |
| `reference/TR400_User_Manual_EN_v1.0.pdf` | `ccli-tr400-user-manual` | **INGESTED** — real HW user manual v1.0 EN |
| `reference/vendor/tespro/Schematic-GPIO.pdf` | `ccli-tr400-gpio-schematic` | **HAVE** — OEM GPIO sheet (2026-09-17) |
| `CCI_TG500_Knowledge_Tree.md` | `ccli-tg500-knowledge-tree` | LIVING — K0–K8 readiness tree |
| `CCI_TesPro_Supplier_Correspondence.md` | `ccli-tespro-supplier-correspondence` | Aug 2026 — OpenWrt 24.10.5, TG-424 Pro |
| `CCI_TesPro_61850_Capture_Plan.md` | `ccli-tespro-61850-capture-plan` | **HAVE** — vendor 61850 service manual batches |
| `CCI_TesPro_61850_Manual_Extract.md` | `ccli-tespro-61850-manual-extract` | **HAVE** — northbound MMS collector (not DSO server) |
| `CCI_MMS_Server_vs_TesPro_Collector.md` | `ccli-mms-server-vs-tespro-collector` | **HAVE** — `ccli` + CID = DSO server; TesPro = optional client poll |
| `reference/vendor/tespro/IEC61850-User-Manual.docx` | `ccli-tespro-61850-manual` | **INGESTED** 2026-09-29 |
| `../07-protocols/0-16consolidata.pdf` | `cei-0-16-consolidata-2025-12` | **INGESTED** — CEI 0-16 consolidated 2025-12 (691 pp.) |
| `CCI_CEI_0-16_Consolidated.md` | `cei-0-16-consolidata-2025-12` | **HAVE** — English index + V5 deltas + annex map |
| `CCI_Annex_O_Extract.md` | `cei-0-16-allegato-o` | **HAVE (English)** — V5/ARERA 385/2025 updates |
| `CCI_Annex_M_Extract.md` | `cei-0-16-allegato-m` | **HAVE (English)** — defence plan / telescatto (PDF pp. 317–321) |
| `CCI_Annex_T_Extract.md` | `cei-0-16-allegato-t` | **HAVE (English)** — IEC 61850 / cyber |
| `CCI_Annex_OTM_Verification.md` | `cei-0-16-otm-verification` | **HAVE** — PDF title/page check vs legacy mislabels |
| `../07-protocols/cei-tr-57-126.pdf` | `cei-tr-57-126` | **INGESTED** — SCL/CID example TR (34 pp.) |
| `CCI_TR_57-126_Extract.md` | `cei-tr-57-126` | **HAVE (English)** — DataSets, use case, CID map |
| `CCI_61850-6_Capture_Plan.md` | `ccli-61850-6-capture-plan` | **HAVE** — SCL batches **A–K COMPLETE** |
| `CCI_61850-6_Extract.md` | `ccli-61850-6-extract` | **HAVE (P0+P1 SCL)** — full CID grammar + I/J/K |
| `CCI_61850-7-2_Capture_Plan.md` | `ccli-61850-7-2-capture-plan` | **HAVE** |
| `CCI_61850-7-2_Extract.md` | `ccli-61850-7-2-extract` | **HAVE (P0)** — ACSI / MMS services |
| `CCI_61850-7-3_Capture_Plan.md` | `ccli-61850-7-3-capture-plan` | **HAVE** — CDC batches A–E |
| `CCI_61850-7-3_Extract.md` | `ccli-61850-7-3-extract` | **HAVE (P0)** — MV/APC/ENC · P3-14 |
| `CCI_61850-7-3_OCR_Corpus.md` | `ccli-61850-7-3-ocr-corpus` | **HAVE** — full OCR (2026-09-30) |
| `CCI_61850-7-4_Capture_Plan.md` | `ccli-61850-7-4-capture-plan` | **HAVE** — LN/DO batches A–E |
| `CCI_61850-7-4_Extract.md` | `ccli-61850-7-4-extract` | **HAVE (P0)** — MMXU / 7-420 cross-ref |
| `CCI_61850-7-4_OCR_Corpus.md` | `ccli-61850-7-4-ocr-corpus` | **HAVE** — full OCR (2026-09-30) |
| `CCI_61850-8-1_Capture_Plan.md` | `ccli-61850-8-1-capture-plan` | **HAVE** — **S0–S8 COMPLETE (OCR)** |
| `CCI_61850-8-1_Extract.md` | `ccli-61850-8-1-extract` | **PART** — 2004 Ed.1; target **2011 Ed.2** |
| `CCI_61850-8-1_OCR_Corpus.md` | `ccli-61850-8-1-ocr-corpus` | **HAVE** — 142 pp. full OCR |
| `CCI_61850-8-2_Capture_Plan.md` | `ccli-61850-8-2-capture-plan` | **PART** — W1d/W8 open (P2 XMPP) |
| `CCI_61850-8-2_Extract.md` | `ccli-61850-8-2-extract` | **PART** — screenshot body through p. 222 |
| `../07-protocols/arera-540-2021.pdf` | `arera-540-2021` | **INGESTED** — ARERA data-exchange delibera (24 pp.) |
| `CCI_ARERA_540_2021_Extract.md` | `arera-540-2021` | **HAVE (English)** — CCI mandate, 61850/104, timelines |
| `CCI_61557-12_Extract.md` | `ccli-61557-12-extract` | **PART** — upgrade from licensed PDF in drop |
| `reference/MT7986A_Datasheet_1.15.pdf` | `ccli-mt7986a-datasheet` | MT7986A v1.15 (BPI-R3 companion) |
| `CCI_MT7986A_Datasheet_Extract.md` | `ccli-mt7986a-datasheet-extract` | **PART** — Filogic pin mux; confirm TG-524 SoC with TesPro |
| `CCI_62443_Zones.md` | `ccli-62443-zones-extract` | **PART** — 62443-3-2 pp. 7, 11–27 + Annex A/B; TG-524 zones/conduits; SL-T 2 proposed |
| `CCI_62443-3-3_Capture_Plan.md` | `ccli-62443-3-3-capture-plan` | **PLAN** — 3-3 ToC + P0 screenshot batches |
| `CCI_62443-3-3_Extract.md` | `ccli-62443-3-3-extract` | **HAVE** — FR 1–7 + Annex A/B (pp. 11–78) |
| `CCI_62443-4-1_Capture_Plan.md` | `ccli-62443-4-1-capture-plan` | **PLAN** — 4-1 SDL batches |
| `CCI_62443-4-1_Extract.md` | `ccli-62443-4-1-extract` | **HAVE** — all 47 clauses + Annex B (pp. 6–51); SD body gap pp. 31–32 optional |
| `CCI_62443-4-2_Capture_Plan.md` | `ccli-62443-4-2-capture-plan` | **HAVE** — 4-2 ToC + batches A–K (Annex B) |
| `CCI_62443-4-2_Extract.md` | `ccli-62443-4-2-extract` | **HAVE** — FR 1–7 + EDR/NDR + Annex B Table B.1; gaps p.57/63 |
| `CCI_62351-1_Capture_Plan.md` | `ccli-62351-1-capture-plan` | **HAVE** — 62351-1 intro (pp. 4–34) |
| `CCI_62351-1_Extract.md` | `ccli-62351-1-extract` | **HAVE** — §1, §4–§6; bib p.35 optional |
| `CCI_62351-3_Capture_Plan.md` | `ccli-62351-3-capture-plan` | **HAVE (P0)** — Batch A pp. 6–19 |
| `CCI_62351-3_Extract.md` | `ccli-62351-3-extract` | **HAVE (P0)** — §5 TLS · §8 PICS Tables 1–5 |
| `CCI_62351-4_Capture_Plan.md` | `ccli-62351-4-capture-plan` | **HAVE (C1 P0)** — Batches A–G |
| `CCI_62351-4_Extract.md` | `ccli-62351-4-extract` | **HAVE (C1 P0)** — §4–§17 + Annex G + §13.2 |
| `CCI_K6.3_Key_Ceremony_SOP.md` | `ccli-k63-ceremony-sop` | **HAVE** — EST dual cert · CR 1.8 |
| `CCI_62351-14_Capture_Plan.md` | `ccli-62351-14-capture-plan` | **HAVE** — full capture |
| `CCI_62351-14_Extract.md` | `ccli-62351-14-extract` | **HAVE** — §4–§7 + Annex A/B/C/D |
| `CCI_62351-9_Capture_Plan.md` | `ccli-62351-9-capture-plan` | **HAVE** — P0 + §8 P2 |
| `CCI_62351-9_Extract.md` | `ccli-62351-9-extract` | **HAVE** — full normative body *(82–90 partial)* |
| `CCI_62351-6_Capture_Plan.md` | `ccli-62351-6-capture-plan` | **HAVE** — GOOSE/SV P1 core |
| `CCI_62351-6_Extract.md` | `ccli-62351-6-extract` | **HAVE (P1 core)** — §4/6/8/9/11 |
| `CCI_62351-8_Capture_Plan.md` | `ccli-62351-8-capture-plan` | **HAVE** — RBAC §5.2 + §12 *(front matter PART)* |
| `CCI_62351-8_Extract.md` | `ccli-62351-8-extract` | **HAVE (P1 core)** — roles/rights/Annex T crosswalk |
| `CCI_61850-5_Capture_Plan.md` | `ccli-61850-5-capture-plan` | **HAVE** — §11.2 + §13.7 Type 3 |
| `CCI_61850-5_Extract.md` | `ccli-61850-5-extract` | **HAVE (P1 P0)** — performance classes ↔ Annex T |
| `TG-500_Series_Datasheet.pdf` | `ccli-tespro-tg500-ds` | ON DISK |
| `TesPro_Gateway_Specifications_Response_Form.pdf` | `ccli-tespro-tg424-response` | ON DISK |
| `CCI_Prototype_BOM.md` / `.json` | `ccli-prototype-bom` | Lab BOM + missing docs |
| `CCI_Architecture_Framework.md` | `ccli-architecture-framework` | Requirements → verification methodology |
| `cci-knowledge-graph.json` | (seed graph) | MT798X / TG-524 / regs / C1–C10 |
| `CCI_Industrial_RAG_Architecture.md` | — | Three-index architecture note |
| `cci-hybrid-rag-eval.json` | — | Hybrid eval set |

Historical SoM vendor study: [`../_archive/som-study/`](../_archive/som-study/) — **not** in active ingest.

## Repo / Telematry companions

| Artifact | RAG `source_id` |
|----------|-----------------|
| `CCI_Project_Roadmap.md` | `ccli-project-roadmap` |
| `CCI_Clone_RE_Option.pdf` | `ccli-clone-re-option` |
| `CCI_3Month_Italy_NoFab_Plan.md` | `ccli-3m-italy-nofab-plan` |
| KB brief | `ccli-knowledge-base-brief` |

## Missing (lab P0)

- **TR400 DIO → Linux GPIO map** (`gpiodetect` / `gpioinfo` → `lab_tr400.yaml`)
- **Lab port map** — `ip link` ↔ silkscreen (WAN, LAN1–4)
- **TesPro OpenWrt 24.10.5 cross-SDK** (cross-compile on host — on-device build is interim)
- **TR400 / TG-424 Pro / TG-524** written SKU mapping
- Energy analyzer Modbus register map
- RED/EMC **test reports** (DoC offered; reports pending per Nicola follow-up)

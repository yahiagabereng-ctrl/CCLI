# CCI RAG Knowledge Base — Clustered Inventory

**Document ID:** CCLI-KB-RAG-001  
**Revision:** 1.0  
**Date:** 2026-07-25  
**RAG source_id:** `ccli-rag-knowledge-base`  
**Purpose:** Single clustered view of the document gate, KB branches, ingest status, and open items for Architecture Freeze and RAG completeness.  
**Companions:** `CCI_Prototype_BOM.md` (detail BOM) | `DOWNLOADS.md` | `README.md` (branch briefs)

---

## 1. Status rollup (document gate D0-D21)

| Cluster | Items | HAVE | INGESTED | PARTIAL | MISSING | Gate |
|---------|------:|-----:|---------:|--------:|--------:|------|
| A Programme & architecture | 3 | 3 | 0 | 0 | 0 | Ready |
| B Architecture freeze blockers | 2 | 0 | 0 | 0 | 2 | **Blocks schematic** |
| C SoM & silicon | 2 | 2 | 2 | 0 | 0 | **FROZEN** |
| D Carrier electronics (MPN TBD) | 5 | 0 | 0 | 1 | 4 | After pin budget |
| E Plant & protocols | 3 | 2 | 2 | 0 | 1 | Modbus map open |
| F Optional / rev A | 4 | 0 | 0 | 1 | 3 | Not Phase 1 block |
| G Fab & mechanical | 2 | 0 | 0 | 0 | 2 | After schematic |
| **Total** | **23** | **7** | **4** | **2** | **14** | |

**Architecture Freeze today:** Cluster **B** (OpenWrt SDK + lab port map) is the lab critical path. **SoC FROZEN:** MT798X on TG-524.

---

## 2. Cluster A — Programme & architecture (ready)

| # | Document | Status | `source_id` / path | Why |
|---|----------|--------|-------------------|-----|
| D0 | Architecture block diagram | **HAVE** | `CCI_Architecture.png` / mermaid | Functional interfaces |
| D1 | Project roadmap + BOM cost envelope | **HAVE** | `ccli-project-roadmap` | Budget / phases |
| D2 | SoC freeze + lab platform | **HAVE** | `ccli-soc-freeze`, `ccli-tg500-lab-platform` | MT798X locked |

---

## 3. Cluster B — Architecture freeze blockers (P0)

| # | Document | Status | `source_id` / path | Why |
|---|----------|--------|-------------------|-----|
| D3 | OpenWrt SDK (TesPro) | **MISSING** | vendor package | Cross-compile |
| D4 | Lab port map (Eth_A/B/plant) | **PARTIAL** | `CCI_TG500_Lab_Platform.md` | L2 HIL |

---

## 4. Cluster C — SoC (FROZEN)

| # | Document | Status | `source_id` / path | Why |
|---|----------|--------|-------------------|-----|
| D5 | SoC freeze — **MT798X** | **HAVE** | `CCI_SOC_Freeze.md` -> `ccli-soc-freeze` | Only active lab silicon |
| D5a | TG-500 datasheet | **HAVE / INGESTED** | `TG-500_Series_Datasheet.pdf` -> `ccli-tespro-tg500-ds` | Hardware spec |
| D6 | TesPro OpenWrt SDK | **MISSING** | vendor package | On-target build |

**Removed from active corpus:** NXP i.MX, Toradex Verdin, Variscite, Compulab — see `_archive/som-study/`.

---

## 5. Cluster D — Lab gaps (not custom carrier)

| # | Document | Status | `source_id` / path | Why |
|---|----------|--------|-------------------|-----|
| D8 | Energy analyzer Modbus map | **MISSING** | `07-protocols/...` -> `ccli-modbus-analyzer-map` | RS-485 plant serial |
| D9 | Pi validation runbook | **MISSING** | `CCI_Pi_Validation_Runbook.md` | Pre-hardware mock |

*Custom carrier electronics (MPN TBD) deferred to field product — not Phase 1 lab.*

---

## 6. Cluster E — Plant & protocols

| # | Document | Status | `source_id` / path | Why |
|---|----------|--------|-------------------|-----|
| D8 | Energy analyzer Modbus register map | **MISSING** | `07-protocols/...` -> `ccli-modbus-analyzer-map` | RS-485 plant serial |

---

## 7. Cluster F — Optional / rev A

| # | Document | Status | Notes |
|---|----------|--------|-------|
| D9 | GNSS module datasheet | **MISSING** | C5 — pick MPN after carrier |
| D10 | LTE module datasheet | **PARTIAL** | Optional rev A |
| D15 | TPM / secure element DS | **MISSING** | If not on SoM SKU |
| D16 | Anti-tamper switch + accelerometer | **MISSING** | Product / CRA |
| D17 | OLED / HMI DS | **MISSING** | Optional proto |

---

## 8. Cluster G — Fab & mechanical

| # | Document | Status | `source_id` / path | Why |
|---|----------|--------|-------------------|-----|
| D7 | ActiveBOM export (Altium) | **MISSING** | after schematic | Fab-ready MPN list |
| D18 | Enclosure / DIN-rail mechanical | **MISSING** | drawing + BOM | Not eval board form |

---

## 9. KB branch clusters (RAG tree)

| Branch | Folder | RAG readiness | Key `source_id`s |
|--------|--------|---------------|------------------|
| Application | `06-application/` | **Partial** | `ccli-app-iec61850`, `ccli-app-modbus`, `fw-sk0146-*` |
| Protocols | `07-protocols/` | **Strong** | CEI O/T, manual, regs class, EU CRA/RED, `ccli-github-protocol-libs` |
| Engineering | `08-engineering/` | **Strong** | TG-500 freeze, BOM, validation, bench BOM |

---

## 10. GitHub / protocol library clusters

| Cluster | Repos | Status |
|---------|-------|--------|
| **Protocols** | libiec61850, lib60870, pymodbus (bench) | 61850/60870 **ON DISK** — `ccli-github-protocol-libs` |
| **Lab platform** | TesPro TG-524 OpenWrt | **ON DISK** — `ccli-tg500-lab-platform` |

---

## 11. Ingest clusters (Telematry)

| Cluster | Count (approx.) | Examples |
|---------|----------------:|----------|
| Italian grid (CEI) | 3 | `cei-0-16-allegato-o`, `-t`, manual |
| Programme docs | 5+ | roadmap, vendor selection, prototype BOM, validation |
| TG-500 lab platform | 3 | datasheet, response form, freeze doc |
| NXP silicon (reference) | 3 | IMX8MPCEC, IMX8MDQLQCEC, precise extract |
| Protocol libraries | 1+ | `ccli-github-protocol-libs` |
| Reference patterns | 3 | Moxa, iGate, STM32 |
| EU regulations | 2 | RED, CRA |

**Rebuild ingest:** `Telematry System/scripts/ingest-ccli.ps1`

---

## 12. To do (by cluster priority)

### P0 — blocks lab bring-up

1. Request **TesPro OpenWrt SDK**  
2. Confirm **TG-424 vs TG-524** SKU  
3. Label **lab port map** on TG-524 receipt  
4. Obtain **Modbus analyzer map** (D8)

### P1 — blocks carrier schematic / L3 bench

5. Obtain **D8** energy analyzer Modbus map  
6. Pick MPNs + DS for **D11-D14** (C4, C1, C2, C10)  
7. Author PF2 spec + Modbus test vectors (`ccli-app-pf2`)

### P2 — product / optional

8. Licensed norm PDFs (ARERA, 61557-12, 62443, 62351)  
9. **D7** ActiveBOM after Altium  
10. **D18** enclosure drawing

## 13. Keywords

`rag knowledge base`, `document gate`, `cluster`, `D0`, `pin budget`, `carrier block`, `IMX8MPIEC`, `PCN`, `0063`, `ingest`, `Architecture Freeze`, `ccli-rag-knowledge-base`

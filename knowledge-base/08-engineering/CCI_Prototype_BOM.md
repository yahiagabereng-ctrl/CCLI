# CCI Prototype BOM & Missing Document Index

**Document ID:** CCLI-BOM-001  
**Revision:** 1.3  
**Date:** 2026-09-15  
**RAG source_id:** `ccli-prototype-bom`  
**Companion JSON:** `CCI_Prototype_BOM.json`  
**Budget target:** ~€1,000 single-unit Phase 1 (see `CCI_Project_Roadmap` §3)  
**Lead path:** TesPro **TG-524 / TR400** — **FROZEN FINAL PRODUCT PLATFORM** (OpenWrt / MT798X) · **TR400 ON BENCH** + Raspberry Pi L1 mock  
**Alternate SoM / custom carrier:** **REMOVED** — archived in `_archive/som-study/` only

**Purpose:** Single place to **store the prototype BOM**, list **every datasheet already downloaded**, and mark what is **still MISSING** before schematic / Architecture Freeze.

Status keys: **HAVE** (on disk) · **INGESTED** (on disk + Telematry RAG) · **PARTIAL** · **MISSING** · **TBD** (part not selected yet).

**Companions:** `CCI_SOC_Freeze.md` · `CCI_TG500_Lab_Platform.md` · `CCI_Mocking_Bench_BOM.md` · [`../DOWNLOADS.md`](../DOWNLOADS.md)

**SoC (FROZEN):** MediaTek **MT798X** — Cortex-A53 dual @ 1.6 GHz in TesPro **TG-524**

---

## 0. Snapshot — downloaded vs still missing

### 0.1 Datasheets & engineering PDFs **downloaded** (on disk)

| File | Role | `source_id` | RAG |
|------|------|-------------|-----|
| TG-500 Series datasheet | TesPro lab gateway | `ccli-tespro-tg500-ds` | **INGESTED** |
| `TesPro_Gateway_Specifications_Response_Form.pdf` | TG-424 / TG-524 spec response | `ccli-tespro-tg424-response` | **INGESTED** |
| `CCI_TG500_Lab_Platform.md` | Lab platform freeze (Rev 3.0) | `ccli-tg500-lab-platform` | **ON DISK** |
| `CCI_TR400_Extract.md` | TR400 user manual extract | `ccli-tr400-user-manual-extract` | **INGESTED** |
| `reference/TR400_User_Manual_EN_v1.0.pdf` | TR400 OEM manual | `ccli-tr400-user-manual` | **INGESTED** |
| `CCI_CCLI_Product_Spec_and_Roadmap.md` | Master SW/HW spec + roadmap | `ccli-product-spec-roadmap` | **ON DISK** |
| `CCI_SOC_Freeze.md` | SoC freeze MT798X | `ccli-soc-freeze` | **ON DISK** |
| `reference/moxa-mgate-5119-series-datasheet-v1.2.pdf` | Modbus gateway pattern ref | `ccli-moxa-mgate-5119` | **INGESTED** |
| `reference/iGate-850_V10_UM_EN.pdf` | Industrial gateway pattern ref | `ccli-igate-850-manual` | **INGESTED** |
| `reference/STM32L151RCTx.pdf` | Companion MCU pattern ref | `ccli-stm32l151rct` | **INGESTED** |

### 0.2 Authored engineering docs **HAVE**

| File | `source_id` | Notes |
|------|-------------|-------|
| `CCI_SOC_Freeze.md` | `ccli-soc-freeze` | MT798X frozen |
| `CCI_TG500_Lab_Platform.md` | `ccli-tg500-lab-platform` | TG-524 lab DUT |
| `CCI_Validation_and_Mocking_Strategy.md` | `ccli-validation-strategy` | L1–L4 PF2 bench ladder |
| `CCI_Mocking_Bench_BOM.md` | `ccli-mocking-bench-bom` | Lab CapEx Waves A–D |
| `cci-knowledge-graph.json` | — | C1–C10 vs TG-524 edges |

### 0.3 Still **MISSING** (must collect or author)

| Priority | Item | Target path / `source_id` | Blocks |
|----------|------|---------------------------|--------|
| P0 | **TesPro OpenWrt SDK** | vendor package | Cross-compile on TG-524 |
| P0 | **TG-424 vs TG-524 SKU confirm** | PO / email | Wrong device |
| P0 | **Lab port map** (Eth_A/B/plant labels) | `CCI_TG500_Lab_Platform.md` update | L2 HIL |
| P1 | Energy analyzer **Modbus register map** | `07-protocols/…` → `ccli-modbus-analyzer-map` | Plant serial software |
| P1 | **Pi validation runbook** | `CCI_Pi_Validation_Runbook.md` | Pre-hardware mock |
| P2 | ARERA 540/2021, IEC 62443, IEC 62351 | licensed | Product cert track |
| — | *Alternate SoM / custom carrier study* | `_archive/som-study/` | **ARCHIVED** — not active path |

---

## 1. Document gate (must exist before schematic freeze)

> **Clustered PDF summary:** see `CCI_RAG_Knowledge_Base.md` (Part VI). Full line-item detail below.

| # | Document | Status | Target path / `source_id` | Why |
|---|----------|--------|---------------------------|-----|
| D0 | Architecture block diagram | **HAVE** | `CCI_Architecture.png` / mermaid | Functional interfaces |
| D1 | Project roadmap + BOM cost envelope | **HAVE** | `ccli-project-roadmap` | Budget / phases |
| D2 | SoC freeze + lab platform | **HAVE** | `ccli-soc-freeze`, `ccli-tg500-lab-platform` | MT798X / TG-524 |
| D3 | OpenWrt SDK from TesPro | **MISSING** | vendor package | Cross-compile |
| D4 | Lab port map (Eth_A/B/plant) | **PARTIAL** | `CCI_TG500_Lab_Platform.md` | L2 HIL wiring |
| D5 | TG-500 datasheet + response form | **HAVE / INGESTED** | `ccli-tespro-tg500-ds`, `ccli-tespro-tg424-response` | Hardware spec |
| D7 | ActiveBOM export (Altium) | **MISSING** | after schematic | Fab-ready MPN list |
| D8 | Energy analyzer **Modbus register map** | **MISSING** | `07-protocols/…` → `ccli-modbus-analyzer-map` | RS-485 plant serial |

---

## 2. Architecture & system (non-PCB but BOM-driving)

| Item | Qty | Est. € | Doc status | Notes |
|------|-----|--------|------------|-------|
| Functional architecture (Eth_A/B, plant, GNSS, DI/DO) | — | — | **HAVE** | Diagram + CEI O/T |
| Regulations classification (feature → norm) | — | — | **HAVE** | `ccli-cci-module-regs-class` |
| Industrial RAG graph (C1–C10 vs TG-524) | — | — | **HAVE** | `cci-knowledge-graph.json` |
| Lab bring-up checklist | — | — | **PARTIAL** | SoC frozen; SDK open |

---

## 3. Compute — lab platform (Phase 1 buy)

| Ref | Description | Locked / suggested | Qty | Est. € | Datasheet / doc | Status |
|-----|-------------|-------------------|-----|--------|-----------------|--------|
| LAB | TesPro **TR400** (lab DUT) / **TG-524** (product SKU) | MT798X / TesproOS | 1 | TBD | TR400 manual + TG-500 DS | **FROZEN — TR400 RECEIVED** |
| MOCK | — | — | — | — | — | **REMOVED** |
| LAB-IO | USB-RS485 + relay module | Bench HIL on A1/B1, DIO1/2 | 1 | ~30 | — | **ACTIVE** |
| SDK | TesPro OpenWrt SDK | Cross-compile | — | — | Vendor package | **MISSING** |

**Product note:** TG-524 integrated gateway is the **final product compute** — not a disposable lab unit. Remaining TBD: enclosure (C9), full C4 I/O expansion, EMC evidence — **not** alternate silicon.

---

## 4. Ethernet & plant networking (C1) — lab

| Ref | Description | Qty | Doc needed | Status |
|-----|-------------|-----|------------|--------|
| — | WAN + **4× LAN** on TR400 | integrated | Port map in lab platform doc + `ip link` | **PARTIAL** |
| — | VLAN / firewall policy Eth_A ↔ Eth_B | SW | Architecture framework | **MISSING** ADR |

**Deferred:** discrete switch/PHY/magjack on custom carrier — **N/A** (integrated gateway is final platform).

---

## 5. Serial / Modbus plant (C2) — lab

| Ref | Description | Qty | Doc needed | Status |
|-----|-------------|-----|------------|--------|
| — | 2× RS485 on TG-524 | integrated | Confirm isolation with TesPro | **PARTIAL** |
| — | Energy analyzer Modbus map | — | Register map PDF | **MISSING** |

**N/A:** opto-isolated transceiver on custom carrier — use TG-524 integrated RS485.

---

## 6. Digital I/O (C4) — lab

| Ref | Description | Qty | Status |
|-----|-------------|-----|--------|
| — | TR400 **4× DIO** + AI/AO (lab: DIO1 DO, DIO2 DI) | integrated | **PARTIAL** vs AiLux 5 DI / 3 DO |
| — | Pi + Wave B DI fixture | bench | See mocking bench BOM |

**Deferred:** field DI 10–120 Vdc / relay DO on custom carrier.
| — | PF2 curtailment signal definition | — | — | App note / ICD | **MISSING** |

---

## 7. Time sync — GNSS (C5)

| Ref | Description | Qty | Est. € | Doc needed | Status |
|-----|-------------|-----|--------|------------|--------|
| U-GNSS | GPS/GLONASS module (u-blox-class) | 1 | 20–35 | Module DS | **MISSING** (MPN TBD) |
| ANT-G | Active GNSS antenna + SMA-F path | 1 | 5–15 | Antenna DS | **MISSING** |
| — | PPS / time sync note (IEEE 1588 optional) | — | — | App note | **MISSING** |

---

## 8. Cellular — LTE (optional rev A)

| Ref | Description | Qty | Est. € | Doc needed | Status |
|-----|-------------|-----|--------|------------|--------|
| U-LTE | LTE Cat.1 (Quectel-class) | 0–1 | 25–40 | Module HW design guide | **PARTIAL** (generic refs; CCI MPN **TBD**) |
| ANT-L | LTE antenna SMA | 0–1 | 5–10 | Antenna DS | **MISSING** |
| SIM | Nano-SIM holder | 0–1 | 1–2 | Holder DS | **MISSING** |

---

## 9. Security / trust (C6)

| Ref | Description | Qty | Est. € | Doc needed | Status |
|-----|-------------|-----|--------|------------|--------|
| — | Secure boot + TPM (TG-524) | — | — | TG-500 DS + SOC freeze doc | **PARTIAL** |
| U-TPM | Discrete TPM 2.0 (if not on SoM SKU) | 0–1 | 5–15 | TPM DS + FIPS cert note | **MISSING** until assembly option freeze |
| — | Secure element (optional) | 0–1 | TBD | SE DS | **TBD** |
| SW-AT | Anti-tamper cover switch | 0–1 | 1–3 | Switch DS | **MISSING** (product) |
| U-ACC | XYZ accelerometer anti-tamper | 0–1 | 2–5 | Accel DS | **MISSING** (product) |

---

## 10. Power (C10)

| Ref | Description | Qty | Est. € | Doc needed | Status |
|-----|-------------|-----|--------|------------|--------|
| U-PWR | 12–24 Vdc input → SoM + 3.3/1.8 rails | 1 | 10–25 | Buck/LDO/hot-swap DS | **MISSING** (MPN TBD) |
| F1 | Input fuse / eFuse / reverse polarity | 1 | 1–3 | Protection DS | **MISSING** |
| — | Field DI external supply isolation note | — | — | Design note | **MISSING** |

**Kit note:** Dev Board supply 7–24 V OK for bench — not DIN-rail CCI enclosure.

---

## 11. Mechanical / HMI / misc

| Ref | Description | Qty | Est. € | Doc needed | Status |
|-----|-------------|-----|--------|------------|--------|
| PCB | 4-layer carrier, proto qty 5–10 | 1 lot | 50–150 | Gerbers (later) | **MISSING** |
| ENC | DIN-rail enclosure | 1 | 15–40 | Mechanical drawing | **MISSING** |
| DISP | OLED 1″ (optional) | 0–1 | 5–15 | Display DS | **MISSING** |
| USD | MicroSD holder | 0–1 | 1–2 | Holder DS | **TBD** |
| — | Fab BOM (Altium ActiveBOM) | — | — | CSV/PDF export | **MISSING** |

---

## 12. Software / stack (not PCB BOM, but prototype “bill”)

| Item | Status | Doc / license note |
|------|--------|-------------------|
| OpenWrt on TG-524 | **ON DISK** | `ccli-tg500-lab-platform`, TesPro SDK TBD |
| libiec61850 | **HAVE** (refs) | GPLv3 — commercial to ship |
| lib60870 | **HAVE** (refs) | IEC 104 path |
| Modbus master + PF2 DO logic | **NOT STARTED** | needs analyzer map |

---

## 13. To do (lab)

1. Order **TG-524** and confirm SKU with TesPro  
2. Request **OpenWrt SDK** with PO  
3. Label **Eth_A / Eth_B / plant** ports on receipt  
4. Obtain energy analyzer **Modbus register map**  
5. Scaffold **Pi mock** + validation runbook  

*Field custom carrier / alternate SoM — deferred; see `_archive/som-study/`.*
6. **Energy analyzer Modbus map** (plant under test)  
7. Power tree IC datasheets (12–24 V in)  
8. Altium schematic → **ActiveBOM** export into this folder  
9. (Optional) Compulab CL-SOM eval PDFs if second-source SoM needed  

---

## 14. RAG / folder convention

```
knowledge-base/08-engineering/
  CCI_Prototype_BOM.md                 → ccli-prototype-bom
  CCI_Prototype_BOM.json
  CCI_Pin_Budget.md                    → ccli-pin-budget        (MISSING)
  CCI_Carrier_Block_Diagram.md         → ccli-carrier-block     (MISSING)
  Toradex_Verdin_*.pdf                 → ccli-toradex-*         (HAVE)
  reference/
    IMX8MPCEC.pdf                      → ccli-nxp-imx8m-plus-cec   (HAVE)
    IMX8MPIEC.pdf                      → ccli-nxp-imx8m-plus-iec   (MISSING)
    vendor-validate/                   → Wave 1 alternate/study PDFs (HAVE)
    <MPN>-datasheet.pdf                → ccli-bom-<mpn-slug>       (carrier TBD)
```

Telematry mirror: `documents/projects/ccli/engineering/` (+ `reference/`, `vendor-validate/`).

---

## 15. Keywords

`prototype bom`, `pin budget`, `carrier`, `relay`, `opto`, `RS-485`, `Eth_A`, `Eth_B`, `GNSS`, `LTE`, `Verdin`, `99991105`, `0063`, `Architecture Freeze`, `IMX8MPIEC`, `ccli-prototype-bom`, `Wave 1`

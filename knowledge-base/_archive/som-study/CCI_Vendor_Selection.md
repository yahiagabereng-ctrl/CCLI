# CCI Vendor Selection — Product Analysis & Dev-Kit Matrix

**Document ID:** CCLI-VENDOR-SEL-001  
**Revision:** 1.1  
**Date:** 2026-07-25  
**RAG source_id:** `ccli-vendor-selection`  
**Purpose:** Analyze each SoM vendor’s **product** (SoM + eval kit + optional industrial gateway) against the CEI 0-16 / AiLux-class CCI control set, then choose a **development kit** for Phase 1 firmware — without treating the kit as the field product.  
**Companions:** `CCI_SoM_Comparison` (silicon), `CCI_Project_Roadmap` (Option A), `CCI_Module_Regulations_Classification` (protocol/cyber).  
**Download index:** [`../DOWNLOADS.md`](../DOWNLOADS.md)

> **Lab platform override (2026-08-10):** Phase 1 lab DUT is **TesPro TG-524** (OpenWrt) — see `CCI_TG500_Lab_Platform.md`.  
> This document remains a **historical SoM vendor study** for future field hardware; it is **not** the active build path.

---

## 0. Decision rule (read first)

| Layer | Role | Deploy as CCI? |
|-------|------|----------------|
| **SoM** | Production compute on **custom carrier** | Yes (with carrier) |
| **Dev / evaluation board** | BSP, Linux, early 61850/Modbus, signed-boot experiments | **No** |
| **Industrial gateway** (vendor SKU) | Reference for I/O patterns / BSP maturity | Usually **No** — form factor & CEI Eth_A/B story rarely match |

**Phase 1 buy:** one vendor’s SoM + matching eval kit.  
**Phase 1 build:** custom carrier for missing controls (extra Ethernet, isolated RS-485, field DI/DO, GNSS/LTE).

---

## 1. Table A — CCI control checklist (master requirements)

| ID | Requirement | Prototype kit | Product (carrier + SoM) |
|----|-------------|-----------------|-------------------------|
| C1 | ≥4× 10/100 Ethernet: Eth_A (DSO), Eth_B (Operatore Abilitato / Qualified Operator), 2× plant | Partial OK (2 ports for SW demo) | **Required** |
| C2 | 2× opto-isolated RS-485 (Modbus RTU) on RJ45-class plant serial | 1× UART/RS232 OK for Modbus demo | **Required** |
| C3 | IEC 61850 Server/Client/GOOSE on Eth_A | **Required** (SW on kit) | **Required** |
| C4 | DI 10–120 Vdc ×5 (external field supply) / DO relay ×3 | Stub in SW OK | **Required** |
| C5 | GNSS (GPS/GLONASS) time sync + SMA path | Nice-to-have | **Required** |
| C6 | Secure boot + key storage (HAB + SE/TPM path) | BSP proof on kit | **Required** |
| C7 | Maintained OpenWrt/Linux BSP on lab gateway; industrial product TBD | **Required** (TG-524 lab) | **Required** (field) |
| C8 | Longevity / PCN (≈10-year industrial) | No | **Required** |
| C9 | Vendor signed-boot / security docs | Strongly preferred | **Required** |
| C10 | Power 12–24 Vdc class, DIN-rail product path | Eval may differ | **Required** |

**Scoring keys used below:** **Full** / **Partial** / **None** / **N/A**.

---

## 2. Table B — Vendor product shortlist (analyze each company)

| Vendor | SoM SKU (shortlist) | Dev / eval kit | Industrial reference | BSP / repos | Secure-boot path | Kit est. € |
|--------|---------------------|----------------|----------------------|-------------|------------------|------------|
| **TesPro** | **TG-524** (TG-500 series) | **Lab DUT — FROZEN** | TG-500 gateway | OpenWrt (vendor) | TPM 2.0 / Secure Boot (brochure) | TBD |
| **Toradex** | Verdin iMX8M **Plus** IT (historical study) | Verdin Dev Board **99991105** | — (custom carrier) | Historical vendor BSP | HAB reference | Board ~€300 + SoM |
| **Variscite** | DART-MX8M-PLUS IT (or MINI) | Vendor DART carrier / eval | VAR-SOM / SMARC MX8M-PLUS family | [variscite-bsp-platform](https://github.com/varigit/variscite-bsp-platform), [meta-variscite-imx](https://github.com/varigit/meta-variscite-imx) (Scarthgap 6.6.y) | Vendor HAB docs | TBD |
| **Compulab** | CL-SOM-iMX8 / MCM-iMX8M-Mini | Compulab eval carrier | **IOT-GATE-iMX8** industrial gateway | [meta-bsp-imx8mm](https://github.com/compulab-yokneam/meta-bsp-imx8mm) | TBD | TBD |
| **NXP** | i.MX 8M Plus EVK SoC path | EVK-MIMX8MP | Reference only | [imx-manifest](https://github.com/nxp-imx/imx-manifest), UG10164 | HAB / CAAM reference | Higher |

**Protocol libraries (active lab path):** see `07-protocols/CCI_GitHub_Protocol_Libraries.md` — libiec61850, lib60870, pymodbus (bench).

---

## 3. Toradex kit row — Verdin Development Board (PN 99991105) *(historical study — not lab DUT)*

### 3.1 Product card (from vendor listing)

| Field | Value |
|-------|--------|
| **Name** | Verdin Development Board with HDMI Adapter |
| **PN** | **99991105** |
| **Version** | V1.1F |
| **Status** | Sample Production |
| **Intended use** | Evaluation / Development (**not** field CCI) |
| **Unit price (listing)** | €300.00 |
| **Supply** | 7–24 V DC ±10% |
| **Dimensions** | 250 mm × 200 mm |
| **USB** | 2× USB-A 3.0 Host; 1× USB-C 2.0 DualRole |
| **Ethernet** | **2× Gigabit Ethernet** |
| **Serial / bus** | 1× RS232; 2× CAN; 4× I2C; 1× SPI |
| **I/O** | 3× PWM; 4× ADC; up to 98× GPIO |
| **Audio / video** | Stereo in/out; HDMI adapter |

**SoM to pair (recommended):** Verdin **iMX8M Plus** Industrial-temp SKU (dual GbE MAC on SoC — cleaner Eth_A / Eth_B story than Mini). Mini IT remains a cost-down option with more carrier Ethernet work.

### 3.2 Table — Toradex kit vs CCI controls (C1–C10)

| ID | CCI need | Kit / SoM score | Notes |
|----|----------|-----------------|-------|
| C1 | 4× Ethernet Eth_A/B + plant | **Partial** | Board has **2× GbE** only — enough for early Eth_A + Eth_B SW demo; **2 plant ports + switch/PHY on custom carrier** |
| C2 | 2× isolated RS-485 | **Partial** | Board has **1× RS485** (DB9 via UART1 transceiver) + 1× RS232 — **not** opto-isolated and **not** 2× RJ45 Modbus; product C2 still on custom carrier |
| C3 | IEC 61850 on Eth_A | **Full** (SW) | Run libiec61850 / commercial stack over GbE |
| C4 | DI 10–120 Vdc / DO relays | **None** | GPIO only — no field-voltage conditioning; stub DO in SW until carrier |
| C5 | GNSS | **None** on board | Add u-blox-class module on carrier (UART/I2C + SMA) |
| C6 | Secure boot / TPM path | **Partial → Full** | SoM HAB + vendor signed-boot docs; optional TPM on Plus SKUs — verify SKU |
| C7 | Linux BSP / industrial SKU | **Full** (historical) | Vendor BSP; IT SoM SKUs available |
| C8 | Longevity | **N/A** (kit) / **Full** (SoM IT) | Request 10-year / PCN letter for **chosen Verdin SKU** |
| C9 | Signed-boot docs | **Full** | meta-toradex-security + signed-boot GitHub project |
| C10 | DIN-rail product power/form | **Partial** | 7–24 V OK for bench; **not** DIN-rail CCI enclosure |

### 3.3 What this kit is for vs not for

| Use now (Phase 1) | Defer to custom carrier |
|-------------------|-------------------------|
| Boot Linux, pin BSP revision | Extra 2× Ethernet + managed switch / PHYs |
| Dual GbE networking, VLAN experiments | 2× opto-isolated RS-485 Modbus |
| libiec61850 MMS/GOOSE prototype on Eth_A | Field DI 10–120 Vdc + relay DO |
| Secure-boot / signed image chain trials | GNSS + LTE modules, anti-tamper, OLED |
| Local USB console / diagnostics | DIN-rail mechanical + product EMC |

---

## 4. Table C — SoM silicon shortlist (summary; detail in SoM comparison)

| Option | SoC | On-chip GbE | RT core | Ind. temp | CCI note |
|--------|-----|-------------|---------|-----------|----------|
| Toradex Verdin iMX8M Mini | 8M Mini | 1 (+TSN) | M4 | Yes (IT) | Lower cost; harder Eth_A/B isolation |
| **Toradex Verdin iMX8M Plus** | 8M Plus | **2** | M7 | Yes (IT) | **Preferred with kit 99991105** |
| Variscite DART-MX8M-MINI | 8M Mini | 1 | M4 | Yes | Pin2pin DART |
| Variscite DART-MX8M-PLUS | 8M Plus | **2** | M7 | Yes | Strong Toradex alternative |
| Compulab CL-SOM-iMX8 | 8M class | 1 typ. | M4 | Check | Confirm SKU + longevity |
| NXP EVK / bare SoC | 8M Dual/Quad/Plus | 1–2 | M4/M7 | Check | Reference, not DIN SoM |

See also `CCI_SoM_Comparison` certificate map (62443 / 61850 / 62351 vs silicon).

---

## 5. Table D — Phase 1 kit recommendation

| Goal | Recommended buy | Validates | Still on carrier |
|------|-----------------|-----------|------------------|
| **Lead path (recommended)** | Toradex **Verdin Dev Board 99991105** + **Verdin iMX8M Plus IT** SoM | C3, C6–C7, C9; Partial C1 (2× Eth) | C1 remainder, C2, C4, C5, C10 product form |
| Cost-down eval | Same board + Verdin **Mini** IT | Same SW path; weaker Ethernet | Same + harder Eth split |
| Parallel reference | Compulab IOT-GATE-iMX8 (study only) | Industrial I/O patterns | Not CEI CCI form factor |
| Silicon/HAB only | NXP EVK-MIMX8MP | HAB, DT, UG10164 | Everything else |

**Architecture Freeze gate:** (1) SoM SKU locked, (2) one eval kit ordered, (3) carrier block diagram frozen for C1/C2/C4/C5.

---

## 6. Scorecard template (fill when datasheets arrive)

Copy one block per vendor product (SoM / kit / gateway).

| Vendor / PN | C1 | C2 | C3 | C4 | C5 | C6 | C7 | C8 | C9 | C10 | Evidence (PDF/URL) |
|-------------|----|----|----|----|----|----|----|----|----|-----|---------------------|
| Toradex 99991105 (+ Plus SoM) | P | N | F* | N | N | P/F | F | SoM | F | P | This doc §3 |
| Variscite … | | | | | | | | | | | |
| Compulab … | | | | | | | | | | | |
| NXP EVK … | | | | | | | | | | | |

\*F for software capability on kit Ethernet, not full 4-port plant topology.

---

## 7. Toradex download paths (local + URL)

| Artifact | Local path (under `knowledge-base/`) | URL |
|----------|--------------------------------------|-----|
| Dev Board V1.1 datasheet | `08-engineering/Toradex_Verdin_Development_Board_V1.1_Datasheet.pdf` | https://docs.toradex.com/116796-verdin_development_board_datasheet.pdf |
| Carrier board design guide | `08-engineering/Toradex_Verdin_Carrier_Board_Design_Guide.pdf` | https://docs.toradex.com/108140-verdin-carrier-board-design-guide.pdf |
| Verdin family specification | `08-engineering/Toradex_Verdin_Family_Specification.pdf` | https://docs.toradex.com/109262-verdin-family-specification.pdf |
| Kit product page (99991105) | — | https://www.toradex.com/products/carrier-board/verdin-development-board-kit |
| Developer center | — | https://developer.toradex.com/hardware/verdin-som-family/carrier-boards/verdin-development-board |
| Signed bootloader (GitHub) | historical reference on disk | https://github.com/toradex/build-signed-imx8m-bootloader |

RAG: historical Toradex PDFs may remain on disk — **not** in active ingest pack.

---

## 8. To do *(historical SoM track — superseded by TG-524 lab)*

1. ~~Confirm Verdin SKU~~ — **deferred**; lab path is TG-524.  
2. ~~Request Toradex PCN~~ — **deferred**.  
3. Keep **libiec61850** as prototype stack (**GPLv3**; MZ Automation **commercial license to ship**) — see roadmap §5.  
4. **Active:** order TG-524, request OpenWrt SDK, scaffold Pi mock — see `CCI_TG500_Lab_Platform.md`.

---

## 9. Keywords (RAG)

`vendor selection`, `toradex`, `verdin`, `99991105`, `development board`, `dart`, `variscite`, `compulab`, `iot-gate`, `eval kit`, `eth_a`, `eth_b`, `carrier`, `som shortlist`, `ccli-vendor-selection`, `download path`, `116796`

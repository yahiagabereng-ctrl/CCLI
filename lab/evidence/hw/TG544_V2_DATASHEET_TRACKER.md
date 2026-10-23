# TG-544 V2 schematic — datasheet download tracker

**Document ID:** CCLI-LAB-HW-DS-TRACK-001  
**Date:** 2026-10-23  
**Purpose:** Every MPN that **blocks** PASS on isolation, voltage, relay, or Eth_A/Eth_B claims must have a datasheet (or TesPro signed spec) in repo.

**Store downloaded PDFs under:**  
`knowledge-base/08-engineering/reference/vendor/tespro/2026-10-22-tg544-v2-schematic/datasheets/`  
**Naming:** `<MPN>.pdf` · note in table when saved.

---

## Priority P0 — download before closing supplier red comments

| MPN | Function | Req IDs | In repo today | Action | Key parameters to extract |
|-----|----------|---------|---------------|--------|---------------------------|
| **MT7987** (or exact SoC marking) | Main CPU / switch | REQ-GW-01, 06, 15 | **PART** — `datasheets/MT7986A_Datasheet_1.15_interim_for_MT7987.pdf` | **GET** MT7987 from TesPro | Core, ETH MAC, SGMII |
| **SLB9673** | TPM 2.0 | REQ-GW-09, V-SEC-002 | **HAVE** — `datasheets/SLB9673_Infineon_DataSheet_Rev1.4.pdf` (+ policy `.txt`) | — | I2C, 3.3 V |
| **CA-IS3721HS** | Digital isolator (UART) | REQ-GW-08, V-IO-003 | **HAVE** — `datasheets/CA-IS3721HS_Chipanalog_CA-IS372x_V1.08_en.pdf` | — | **3750 Vrms**, CMTI, UL1577 |
| **BL3085(I47)** or **BL3085** | RS485 XCVR | REQ-GW-08 | **HAVE** — `datasheets/BL3085_I47_Belling_V2.3_cn.pdf` | — | ±15 kV ESD, 500 kbps |
| **HK3FF-DC5V-SHG** | Field relay DO | REQ-GW-16d, REQ-O-DIO | **HAVE** — `datasheets/HK3FF-DC5V-SHG_HUIKE.pdf` | — | **10 A @ 250 VAC** |
| **SFP module** (TBD PN) | 100BaseFX 1310 nm LC | REQ-GW-06b, REQ-O-ETH-A | **MISSING** | **FROM TESPRO** — not generic cage | IEEE 802.3 **100BASE-FX**, wavelength |
| **SFP module** (TBD PN) | 10/100 TX copper | REQ-GW-06c, REQ-O-ETH-B | **MISSING** | **FROM TESPRO** | 10/100, not 1000 unless allowed |
| **SMDJ36CA** | Input TVS | REQ-GW-10 | **MISSING** | **DOWNLOAD** SMDJ series | Standoff, clamp @ Ipp |
| **SMBJ33CA** | DIO field TVS | REQ-GW-16c | **PART** — `SMBJ_Series_*.pdf` | **GET** SMBJ33CA line item | Proves **not** 120 V class alone |
| **SMBJ18CA** | RS485 TVS | V-IO-003 | **PART** — `SMBJ_Series_*.pdf` | **GET** SMBJ18CA line item | Line protection |
| **TLP291(GB-TP,SE)** | DIO opto | REQ-GW-16 | **HAVE** — `datasheets/TLP291-4_Toshiba.pdf` | — | Isolation voltage |
| **GD32F330CBT6** | DIO MCU | REQ-GW-16 | **HAVE** — `datasheets/GD32F330xx_Datasheet_Rev_3.0.pdf` | — | GPIO abs max |
| **PCAQ7060MW-301** | CM choke (with **C89**) | REQ-GW-10 | **HAVE** — `datasheets/PCAQ7060MW-301_Prodtech_Spec.pdf` | — | CM impedance, safety |
| **SK510L-TP** / **SK56L-TP** | Input OR Schottky | REQ-GW-10 | **PART** — `SK56L_MCC_Schottky.pdf` | **GET** SK510L-TP | Vr, If, reverse protection |
| **DMP10H400SK3-13** | P-FET reverse protect | REQ-GW-10 | **HAVE** — `datasheets/DMP10H400SK3-13_DiodesInc.pdf` | — | Vds, Id |
| **K0805L050BR** | USB fuse **F1** | REQ-GW-10b | **MISSING** | **DOWNLOAD** | 500 mA hold — **not** main DC fuse |

---

## Priority P1 — complete BOM / EMC story

| MPN | Function | In repo | Action |
|-----|----------|---------|--------|
| **WM1302** | Radio module (schematic ref) | **PART** — `WM1302_Seeed_LoRaWAN_Module.pdf` | **VERIFY** MPN on sheet vs Seeed LoRaWAN DS; GET TesPro integration guide |
| **FT232RNL** | USB-UART | MISSING | GET — lab debug only |
| **SP3232** | RS232 | MISSING | GET |
| **B0303S-1WR3** | RS485 isolated DC-DC | **FAIL** — `_bad/B0303S-1WR3_Mornsun.pdf` (404 HTML) | Re-download valid Mornsun PDF; extract **isolation voltage** |
| **918-518K2023D50012** | USB-C connector | MISSING | GET |
| **PRTR5V0U2X** | USB ESD | MISSING | GET Nexperia |
| **B3D090L-C** | GDT surge | MISSING | GET |
| **TR600-150 / TR600-350** | PTC | MISSING | GET Bel / description on sheet |
| **MVR1812-470G** | Varistor RV1 | MISSING | GET |
| **Hall sensor** (read ref from PDF) | Tamper S13 | MISSING | GET — TI DRV5032 **HAVE** in cert drop for *different* use |
| **G6K-2F-Y3VDC** | Old 4-ch relay | N/A V2 | Keep for **Schematic-GPIO.pdf** diff only |
| Buck **U1** (read ref) | 12 V → rails | MISSING | GET from BOM on S01 |

---

## Priority P2 — already in programme corpus

| Item | Location | Notes |
|------|----------|-------|
| MT7986A SoC reference | `reference/MT7986A_Datasheet_1.15.pdf` | Use until MT7987 pinned |
| SLB9673 security policy | `2026-10-cert-reply/_extracts/FIPS_140_2_…` | Not pinout DS |
| TR400 user manual | `TR400_User_Manual_EN_v1.0.pdf` | Port names — cross-check S03 |
| TesPro gateway form | `TesPro_Gateway_Specifications_Response_Form.pdf` | Baseline “Complied” column |
| Hall **DRV5032** | cert reply extract | Compare vs S13 part marking |

---

## Supplier-only documents (request if not downloadable)

| Document | Why |
|----------|-----|
| **Approved SFP vendor list** + **FX 1310 nm** PN | Closes REQ-GW-06b / REQ-O-ETH-A |
| **Ethernet port role map** (WAN/LAN/SFP ↔ Eth_A/Eth_B) | Closes REQ-GW-06 |
| **Main DC input fuse** drawing note | Closes REQ-GW-10b |
| **DI/DO voltage range** app note | Closes REQ-GW-16c |
| **ubus channel ↔ J1/J2** table | Lab deploy |
| **RS485 isolation test report** or **hipot** class | Closes V-IO-003 without guesswork |

---

## Download workflow

```powershell
# After saving PDFs to datasheets/
python scripts/extract-tg544-v2-datasheets.py
powershell -File scripts/ingest-tespro-tg544-v2-schematic.ps1
# RAG (repo root, Telematry up):
powershell -File scripts/sync-ccli-corpus-to-telematry.ps1
```

**Last ingest check:** `datasheets/_extracts/INGEST_CHECK.txt` — **13 OK**, Mornsun B0303S quarantined under `datasheets/_bad/`.

Update **In repo** column to **HAVE** when file lands; link path in [TG544_V2_REQUIREMENT_SHEET_VERIFY_MATRIX.md](TG544_V2_REQUIREMENT_SHEET_VERIFY_MATRIX.md) verify log.

---

## Quick answer — **C89** (from S01 POWER)

| Ref | Value | DS needed? |
|-----|-------|------------|
| **C89** | **1 nF / 250 V** | **P1** — confirm **dielectric** (C0G/Y) from BOM line, not just value on sheet |

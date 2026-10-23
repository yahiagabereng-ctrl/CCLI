# TG-544 V2 — requirement × schematic sheet verification matrix

**Document ID:** CCLI-LAB-HW-SCH-MATRIX-001  
**Date:** 2026-10-23  
**Schematic:** `knowledge-base/08-engineering/reference/vendor/tespro/2026-10-22-tg544-v2-schematic/TG544_V2_Schematic.pdf`  
**Supplier ask:** HiTEKS gateway requirements table (PPT / response form rows 1–16)  
**Datasheet tracker:** [TG544_V2_DATASHEET_TRACKER.md](TG544_V2_DATASHEET_TRACKER.md) · **Ingest score:** [TG544_V2_DATASHEET_INGEST_SCORE_2026-10-23.md](TG544_V2_DATASHEET_INGEST_SCORE_2026-10-23.md)  
**RAG source_id:** `ccli-tg544-v2-requirement-sheet-matrix`

---

## How to use

1. Open the PDF and jump to the **sheet block** named in the matrix (labels on the composite drawing).  
2. For each row, verify **every ● cell** on that sheet; mark **Verdict** PASS / PART / GAP / N/A.  
3. Download datasheets flagged **GET** in the tracker before signing isolation / voltage / relay ratings.  
4. Record evidence in the **Verify log** column (designator + value or DUT command).

**Legend:** **●** = must verify on this sheet · **—** = not applicable · **○** = secondary / cross-check only

---

## Schematic sheet index (V2 PDF blocks)

| Sheet ID | Block label on PDF | Primary content |
|----------|-------------------|-----------------|
| **S01** | **POWER** | DC_IN **CON2**, **12V**, buck **U1**, **C89**, TVS **SMDJ36CA**, OR **SK510L/SK56L** |
| **S02** | **SIM Card** | Dual SIM, **WM1302** / 4G-5G rails |
| **S03** | **Gigabit Ethernet** | **MT7987**, **WAN/LAN0–LAN2**, **2P5GBT_***, magnetics |
| **S04** | **SFP Fiber to RJ45** | **U30** SFP cage, host **SGMII** |
| **S05** | **TPM 2.0** | **SLB9673** |
| **S06** | **Isolated RS-485** | **U8/U10 BL3085**, **CA-IS3721HS**, **J3** A1/B1 |
| **S07** | **Isolated RS-232** | **U9 SP3232**, **CA-IS3721HS**, **J3** 232 pins |
| **S08** | **Isolated DI×6 & DO×6** | **GD32F330**, **J1/J2**, **K1–K6**, **TLP291** |
| **S09** | **USB Type-A Port** | **918-518K2023D50012**, **F1** |
| **S10** | **USB to UART** | **U19 FT232RNL** |
| **S11** | **TF Card Reader** | SD/TF socket, **MSDC0_** nets |
| **S12** | **LED & Reset** | Status LEDs, **S1–S3** buttons |
| **S13** | **Hall** (tamper) | Hall sensor, **GPIO/HALL** |
| **S14** | **Connector** (×2) | Mechanical / panel (confirm in PDF) |

If TesPro also supplied **multi-page Altium exports**, map **S01–S14** to their sheet numbers in the **Verify log** column.

---

## Matrix A — PPT supplier table (rows 1–16)

| Req ID | PPT row | Requirement (short) | S01 | S02 | S03 | S04 | S05 | S06 | S07 | S08 | S09 | S10 | S11 | S12 | S13 | S14 | Initial | Datasheets (see tracker) |
|--------|---------|---------------------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|---------|---------------------------|
| **REQ-GW-01** | 1 | CPU dual ≥1 GHz | — | — | ● | — | — | — | — | ○ | — | — | — | — | — | — | PART | **MT7987** (use MT7986 DS) |
| **REQ-GW-02** | 2 | RAM 2 GB | — | — | ● | — | — | — | — | — | — | — | — | — | — | — | OPEN | DDR vendor BOM |
| **REQ-GW-03** | 3 | eMMC ≥32 GB | — | — | ● | — | — | — | — | — | — | — | ○ | — | — | — | OPEN | eMMC MPN from BOM |
| **REQ-GW-04** | 4 | GPS / GNSS | — | ● | — | — | — | — | — | — | — | — | — | — | — | — | PART | **WM1302** module DS |
| **REQ-GW-05** | 5 | 4G LTE | — | ● | — | — | — | — | — | — | ○ | — | — | ○ | — | — | PART | Modem module DS |
| **REQ-GW-06a** | 6 | 4× Eth isolated | — | — | ● | ○ | — | — | — | — | — | — | — | — | — | ○ | PART | PHY/magnetics per port |
| **REQ-GW-06b** | 6 | **100BaseFX** LC 1310 nm (Eth_A) | — | — | ○ | ● | — | — | — | — | — | — | — | — | — | — | GAP | **SFP module PN** (FX) |
| **REQ-GW-06c** | 6 | **10/100 TX SFP** (Eth_B) | — | — | ○ | ● | — | — | — | — | — | — | — | — | — | — | OPEN | **SFP copper** module PN |
| **REQ-GW-07** | 7 | USB >2× | — | — | ○ | — | — | — | — | — | ● | ○ | — | — | — | ○ | PART | USB-C **918-518K** |
| **REQ-GW-08** | 8 | 2× RS485 isolated | — | — | — | — | — | ● | ○ | — | — | — | — | — | — | ○ | PART | **BL3085**, **CA-IS3721**, **B0303S** DS HAVE |
| **REQ-GW-09** | 9 | TPM 2.0 | — | — | ○ | — | ● | — | — | — | — | — | — | — | — | — | PASS | **SLB9673** (HAVE policy) |
| **REQ-GW-10a** | 10 | 12 V input / 12–36 V | ● | — | — | — | — | — | — | — | — | — | — | — | — | ○ | PART | **SK510/SK56** DS HAVE; **SMDJ36CA** GAP; **MVR1812** wrong DS (0805 on disk) |
| **REQ-GW-10b** | 10 | **Input fuse / protection** | ● | — | — | — | — | — | — | — | ○ | — | — | — | — | — | PART | **F1 K0805L050BR** DS HAVE; main DC fuse OPEN |
| **REQ-GW-11** | 11 | OpenWrt / C++ | — | — | — | — | — | — | — | — | — | — | — | — | — | — | N/A | Software |
| **REQ-GW-12** | 12 | IEC 61850 SCD/CID | — | — | — | — | — | — | — | — | — | — | — | — | — | — | N/A | HiTEKS **`ccli` CID** |
| **REQ-GW-13** | 13 | IEC 62351 | — | — | ○ | — | ○ | — | — | — | — | — | — | — | — | — | N/A | FW / PKI docs |
| **REQ-GW-14** | 14 | IEC 62443-4-1 SDLA | — | — | — | — | — | — | — | — | — | — | — | — | — | — | N/A | Programme |
| **REQ-GW-15** | 15 | 62443-4-2 / Secure Boot | — | — | ● | — | ○ | — | — | — | — | — | — | — | — | — | PART | MT7987 boot + TesPro guide |
| **REQ-GW-16a** | 16 | **6× DI** schematic | — | — | — | — | — | — | — | ● | — | — | — | — | — | ○ | PASS | **GD32F330** |
| **REQ-GW-16b** | 16 | **6× DO** schematic | — | — | — | — | — | — | — | ● | — | — | — | — | — | ○ | PASS | **HK3FF** relay |
| **REQ-GW-16c** | 16 | **10–120 V DI** class | — | — | — | — | — | — | — | ● | — | — | — | — | — | — | GAP | **SMBJ33CA** (wrong class) |
| **REQ-GW-16d** | 16 | **250 V / 3 A DO** | — | — | — | — | — | — | — | ● | — | — | — | — | — | — | PART | **HK3FF** DS **10 A @ 277 VAC** — confirm vs **3 A** curtail |

---

## Matrix B — CEI Annex O / lab validation (hardware)

| Req ID | Requirement | S01 | S02 | S03 | S04 | S06 | S08 | S13 | Initial | Notes |
|--------|-------------|-----|-----|-----|-----|-----|-----|-----|---------|-------|
| **REQ-O-ETH-A** | O.13.1.1.1 Eth_A **100BaseFX** | — | — | ○ | ● | — | — | — | GAP | Module not fixed on PCB |
| **REQ-O-ETH-B** | O.13.1.1.1 Eth_B 10/100 | — | — | ● | ● | — | — | — | OPEN | Port + SFP choice |
| **REQ-O-NO-BR** | O.13.1 no bridge | — | — | ● | — | — | — | — | PART | SW proof separate |
| **REQ-O-PLANT** | O.13.1.4 plant RS485 | — | — | ○ | — | ● | — | — | PART | Not on Eth_A/B |
| **REQ-O-UPS** | O.13.3 backup ≥1 h | ● | — | — | — | — | — | — | GAP | External UPS (lab DRC) |
| **REQ-O-DIO** | O.9.2 curtail DO | — | — | — | — | — | ● | — | PART | Rating vs HK3FF |
| **REQ-IO-004** | 5 DI 10–120 V / 3 DO 250 V | — | — | — | — | — | ● | — | GAP | 6+6 but wrong class |
| **V-IO-003** | RS485 iso evidence | — | — | — | — | ● | — | — | PART | **CA-IS3721**, **B0303S**, **B3D090L-C**, SMBJ18 series — hipot narrative still open |
| **V-SEC-002** | TPM present | — | — | ○ | — | — | — | — | ● | PASS | SLB9673 |
| **V-SEC-004** | Hall tamper | — | — | — | — | — | — | ● | PART | Hall IC MPN on PDF |

---

## Per-sheet verification checklist (minimum reads)

| Sheet | Before PASS, confirm on drawing |
|-------|----------------------------------|
| **S01 POWER** | **DC_IN±** rating; **C89** 1nF/250V; **L9** CM choke; **TVS1**; reverse **D11/D1/Q3**; **U1** input cap **C59**; main **DC fuse** (if any) |
| **S02 SIM** | Module **WM1302**; dual SIM ESD; antenna nets |
| **S03 ETH** | **MT7987**; **WAN** vs **LAN0–2** silk; transformer/PHY per port; isolation claim |
| **S04 SFP** | **U30** cage pinout; host bus; approved module list |
| **S05 TPM** | **SLB9673** power, I2C, decoupling |
| **S06 RS485** | **2× BL3085**; **CA-IS3721** on each UART; **A1/A2** TVS **SMBJ18CA**; **J3** pinout |
| **S07 RS232** | **SP3232** (DS: **SP3222B/3232B** family); shared iso domain with 485 |
| **S08 DIO** | **J1/J2** pinout **DO1–6/DI1–6**; **K1–K6**; **TLP291**; **SMBJ33CA**; **TR600-350** |
| **S09 USB-A** | **F1 K0805L050BR**; ESD |
| **S10 UART** | **FT232** debug only — not CEI commissioning IF |
| **S11 TF** | Optional storage — not CCI critical path |
| **S12 LED/Reset** | Factory reset vs field |
| **S13 Hall** | Tamper net to SoC/GD32 |
| **S14 Connector** | Panel ↔ sheet pin match photo |

---

## Verify log (fill during review)

| Req ID | Verdict | Sheet | Designator / evidence | Reviewer | Date |
|--------|---------|-------|------------------------|----------|------|
| REQ-GW-10a | | S01 | C89 = 1nF/250V | | |
| REQ-GW-16a | | S08 | J2 DI1+…DI6- | | |
| | | | | | |

---

## Related

- [TG544_V2_SCHEMATIC_REVIEW_2026-10-23.md](TG544_V2_SCHEMATIC_REVIEW_2026-10-23.md)  
- [CCI_TG544_V2_Schematic_Extract.md](../../../knowledge-base/08-engineering/CCI_TG544_V2_Schematic_Extract.md)  
- [CCI_Competitors_HW_Validation.md](../../../knowledge-base/08-engineering/CCI_Competitors_HW_Validation.md) (V-NET / V-IO IDs)

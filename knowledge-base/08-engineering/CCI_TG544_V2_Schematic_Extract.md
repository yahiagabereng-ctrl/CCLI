# TesPro TG-544 V2 — full board schematic extract (vs HiTEKS gateway table)

**Document ID:** CCLI-HW-TG544-SCH-V2-001  
**Revision:** 1.0  
**Date:** 2026-10-23  
**RAG source_id:** `ccli-tg544-v2-schematic-extract`  
**Source:** `reference/vendor/tespro/2026-10-22-tg544-v2-schematic/TG544_V2_Schematic.pdf` (supplier drop 2026-10-22)  
**Extract:** `_extracts/TG544_V2_Schematic_full.txt` (single composite PDF page; block labels embedded)  
**Parents:** `ccli-tr400-gpio-schematic-extract` · `ccli-tespro-tg424-response` · `ccli-competitors-hw-validation` · `cei-0-16-allegato-o`

---

## Summary

TesPro delivered a **TG-544 V2** composite schematic that includes the **MT7987** main board, **SLB9673 TPM**, **SFP cage**, **isolated RS-485/RS-232**, and **6× DI + 6× DO** field I/O (GD32 **GD32F330CBT6**). Compared to the earlier **`Schematic-GPIO.pdf`** (4 channels, **G6K-2F** relays), V2 adds **DI5/DI6**, **DO5/DO6**, **HK3FF-DC5V-SHG** relays, and documents **CA-IS3721HS** digital isolators on UART paths to the transceivers.

**Versus your supplier requirements table (PPT):** CPU/RAM/eMMC/4G/TPM/OpenWrt path **aligns**. **Row 6 (Eth):** four **2.5G SGMII** legs + **WAN/LAN0–LAN2** naming and **one SFP** (“SFP Fiber to RJ45”) — **does not yet prove** Annex O **100BaseFX LC 1310 nm** on a dedicated DSO port without module PN. **Row 8 (RS485):** **PART → PASS pending rating** — **BL3085** + **CA-IS3721HS** + separate **`GND_RS232-485`** / **`GND_GE`**. **Row 10 (fuse):** **PART** — **USB F1 (K0805L050BR)**; DC input has **TVS (SMDJ36CA)**, **SK510L/SK56L** OR-ing, **DMP10H400SK3** — **no obvious main DC fuse** on `DC_IN+`. **Row 16 (6 DIO / 10–120 V / 250 V·3 A):** **6 channels PART**; field class still **SMBJ33CA + ~47k dividers** — **not 10–120 V product class**; relay **HK3FF-DC5V-SHG** contact rating **must be confirmed** (not 250 V/3 A from extract alone).

---

## Requirements (gateway table ↔ schematic)

| PPT row | Requirement | Schematic evidence | Status |
|---------|-------------|-------------------|--------|
| 1–3 | CPU / RAM / eMMC | **MT7987** (`SYSRSTB_MT7987`), **EMMC**, **2.5G Ethernet** MAC pins | **PASS** (vs TG-424 form) |
| 4–5 | GNSS / LTE | **WM1302**, dual **SIM**, **3.8V_4G_5G** | **PASS** |
| 6 | 4× isolated Eth; FX + SFP | **LAN0–LAN4** LED nets, **2P5GBT_** pairs, **U30 SFP**, label **WAN/LAN1/LAN2** | **PART** — SFP present; **1310 nm FX not specified** |
| 7 | USB | **USB Type-C**, **918-518K2023D50012**, **F1** on USB | **PART** (1× Type-C + internal paths; not “>2×”) |
| 8 | 2× RS485 isolated | **U8/U10 BL3085**, **U13–U16 CA-IS3721HS**, **A1/A2**, **J3** 7P | **PART** — isolation shown; **hipot/kV** from DS |
| 9 | TPM 2.0 | **SLB9673** I2C | **PASS** |
| 10 | 12 V; fuse | **DC_IN+/-**, **12V** rail, **SMDJ36CA**, Schottky OR **SK510L/SK56L** | **PART** — **no main input fuse** in extract |
| 11 | OpenWrt / C++ | N/A HW | **N/A** |
| 16 | 6 DIO; 10–120 V / 250 V DO | **J1/J2 3.5-12P**, **DI1–6**, **DO1–6**, **K1–K6 HK3FF**, **TLP291** | **PART** — **6ch PASS**; **voltage class GAP** |

---

## Architecture

### Block diagram (labels on PDF)

| Block | Content |
|-------|---------|
| POWER | 12 V in → DC-DC → **3V3_MB**, **5V**, **3V3_RS232-485** |
| Gigabit Ethernet | **MT7987** RGMII/SGMII to **4+WAN** ports |
| SFP Fiber to RJ45 | **U30 SFP** cage |
| TPM 2.0 | **SLB9673** |
| Isolated RS-485 | **2× BL3085**, **CA-IS3721HS** on UART |
| Isolated RS-232 | **SP3232**, shared **3V3_RS232-485** domain |
| Isolated DI×6 & DO×6 | **GD32F330** + **TLP291** + **HK3FF** relays |
| Hall | Tamper **Hall** + **GPIO/HALL** |

### Digital I/O (vs old GPIO sheet)

| Item | V2 (2026-10-22) | Old `Schematic-GPIO.pdf` |
|------|-----------------|---------------------------|
| Channels | **DI1–6**, **DO1–6** (J2 / J1) | IO1–4 |
| Relay | **HK3FF-DC5V-SHG** K1–K6 | **G6K-2F-Y3VDC** K1–K4 |
| Field TVS | **SMBJ33CA** (DIO), **TR600-350** PTC note | SMBJ33CA, TR600-350 |
| MCU | **GD32F330CBT6** | Same |

### RS-485

- Transceiver: **BL3085(I47)** ×2 (**U8**, **U10**).
- Field TVS: **SMBJ18CA** on A/B; **B3D090L-C** GDT; **TR600-150** on lines.
- Isolation: **CA-IS3721HS** (**U13–U16**) between SoC UART and **3V3_RS232-485** domain.
- Connectors: **J3** **3.5-7P-WJ** (A1/B1 + 232), second RS485 USB path **DM_RS485-2**.

### Ethernet / SFP

- SoC: **MediaTek MT7987** — **SGMII_P5_**, **2P5GBT_TXV*_* ** to four port groups.
- Silk/logical: **WAN**, **LAN0**, **LAN1**, **LAN2**, **LAN4LED0**…
- **U30 SFP** — label **“SFP Fiber to RJ45”** (media type depends on **plug-in module**, not fixed on PCB).

### Power input

- **CON2 DC_IN1**, **DC_IN+/-**, **12V** bus, **FB_12V**, **SMDJ36CA (TVS1)**, **MVR1812-470G (RV1)**.
- Reverse protection: **D11 SK510L-TP**, **D1 SK56L-TP**, **Q3 DMP10H400SK3-13**.
- **F1 K0805L050BR** on **USB 5V** branch only (not documented as main DC fuse).

---

## Interface matrix (field ↔ programme)

| Terminal | Schematic | HiTEKS role (target) | Notes |
|----------|-----------|----------------------|-------|
| **WAN** | 2.5G port A | Eth_A / DSO (with FX SFP) | Map in `CCI_62443_Zones.md` |
| **LAN0–2** | 2.5G B–D | Plant / OA / eng | Bench yaml LAN1–3 |
| **SFP U30** | Cage | DSO fibre or Eth_B copper module | **Module PN required** |
| **A1/B1** | J3 | Plant RS485 #1 | Lab **ttyS1** |
| **A2/B2** | (second BL3085) | Plant RS485 #2 | LuCI **rs485_2_uart** |
| **DI1–6** | J2 | DG/DD, Annex M, permissive | **ubus map TBD** |
| **DO1–6** | J1 | Curtail, trip, spares | **HK3FF** rating TBD |

---

## BOM matrix (validation-critical)

| Block | MPN (extract) | vs PPT ask | Status |
|-------|---------------|------------|--------|
| SoC | MT7987 | MT798X freeze | **PASS** |
| TPM | SLB9673 | TPM 2.0 | **PASS** |
| RS485 XCVR | BL3085 | 2× isolated | **PART** (iso **CA-IS3721**) |
| Digital iso | CA-IS3721HS | Row 8 comment | **HAVE** |
| DIO relay | HK3FF-DC5V-SHG | 250 V/3 A DO | **OPEN** — need DS |
| DIO TVS | SMBJ33CA | 10–120 V DI | **GAP** |
| SFP | U30 cage | 100BaseFX 1310 | **OPEN** — module not PCB |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| Eth_A **100BaseFX** module | SFP cage only | 1310 nm MM LC PN | **MISSING** |
| **Port role** table | WAN/LAN0–2 on sheet | Signed map ↔ CEI Eth_A/B | **MISSING** |
| **HK3FF** contact rating | Coil 5 V | 250 Vac / 3 A curtail | **MISSING** |
| **Main DC fuse** | TVS + OR-ing | Fuse rating | **MISSING** on extract |
| **ubus ch ↔ DI1–6** | Net names GPIO*_IF | TesPro FW table | **MISSING** |
| **6ch** vs product REQ-IO-004 | 6+6 terminals | 5 DI + 3 DO product spec | **PART** (count OK; class not) |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-SCH-V2-01 | DIO still **33 V TVS class** | REQ-IO-004 / Annex M field voltage | Expansion board or ECO |
| R-SCH-V2-02 | **SFP** without fixed FX module | O.13.1.1.1 Eth_A | Specify **100BaseFX** SFP + test |
| R-SCH-V2-03 | **HK3FF** vs **G6K** relay change | Safe-state / contact rating | Wiring review + load test |
| R-SCH-V2-04 | All ports **2.5G** — “isolated” not numeric | Zone narrative | Request isolation test / diagram |

---

## Verification (next steps)

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| Row 8 RS485 iso | TesPro written + optional hipot | **CA-IS3721** + **BL3085** confirmed |
| Row 16 6 DIO | Visual J1/J2 + smoke | 6 DI + 6 DO toggle |
| Row 6 SFP | Install FX SFP + link | Link on DSO path |
| V-IO-002 | Relay DS | **250 V / 3 A** or downgrade claim |
| Bench | `TR400_DIO_SMOKE_TEST.md` on ch 1–2 | Extend to ch 3–6 |

---

## References

- `lab/evidence/hw/TG544_V2_SCHEMATIC_REVIEW_2026-10-23.md` — line-by-line checklist  
- `knowledge-base/08-engineering/CCI_TR400_GPIO_Schematic_Extract.md` — prior 4-ch sheet  
- `knowledge-base/08-engineering/TesPro_Gateway_Specifications_Response_Form.pdf` — original table  

**Revision history**

| Rev | Date | Notes |
|-----|------|-------|
| 1.0 | 2026-10-23 | Initial extract from TG544 V2 PDF |

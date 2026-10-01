# BOM Component Review — PF2 CCI Carrier

**Document ID:** CCLI-BOM-REV-001  
**Revision:** 1.1  
**Date:** 2026-09-15  
**Roles:** Senior Hardware Architect · Component Engineer · Supply-Chain Reviewer  
**Scope:** Product BOM (`Architecture/BOM_Matrix.xlsx` / `ccli-prototype-bom`) — **not** lab mocking-bench BOM  
**Status:** **Toradex custom-carrier rows archived.** Active product path is **TesPro TR400 / TG-524** integrated gateway — see `CCI_TG500_Lab_Platform.md` Rev 3.0.

**Caveat:** Catalog stock links change. Use manufacturer pages + distributor **search by MPN**. Always verify lifecycle and IT grade on the quote date.

---

## Platform supersession (2026-09-15)

| Active path | Part | Role |
|-------------|------|------|
| **PROD-GW** | TesPro **TG-524** / **TR400** | FROZEN integrated gateway — MT798X, OpenWrt / TesproOS |
| **LAB-DUT** | **TesPro TR400** (received) | Active L2+ on-target validation |
| **LAB-MOCK** | Raspberry Pi 4/5 | L1 unit tests only |
| **ARCHIVED** | Toradex **0063** + **99991105** | Custom-carrier study — `_archive/som-study/` |

Integrated gateway covers C1 (5× GE), C2 (2× RS485), partial C4 (**4× DIO** on TR400 vs AiLux 5 DI / 3 DO), C6 (TPM), C10 (12–30 V). Full AiLux C4 may need Wave B expansion module.

---

## Executive summary

| Ref | Block | Proposed MPN | Status | Go / hold |
|-----|-------|--------------|--------|-----------|
| PROD-GW | Integrated gateway | TesPro **TR400 / TG-524** | **FROZEN — ACTIVE** | **GO** — TR400 on bench |
| U-SOM | SoM (archived) | Toradex **0063** | **SUPERSEDED** | Reference only |
| KIT | Lab (archived) | **99991105** | **SUPERSEDED** | Reference only |
| SW1 | C1 | Microchip **KSZ9893RNXI** (+ SoM dual GbE) | PROPOSED | **HOLD** until ADR-005 |
| J-ETH | C1 | Pulse **JK0-0136NL** class magjack | PROPOSED | After C1 |
| U-485 | C2 | ADI **ADM2795EARWZ** ×2 | PROPOSED | Strong industrial fit |
| U-DI | C4 | TI **ISO1212DBQ** ×3 (5 ch) | PROPOSED | Design for 10–120 V resistors |
| K-DO | C4 | Omron **G2RL-1A-E-DC12** ×3 | PROPOSED | Power relay — not signal G6K |
| U-GNSS | C5 | u-blox **NEO-M9N-00B** | PROPOSED | PPS available |
| ANT-G | C5 | Taoglas / Molex active SMA | PROPOSED | Match module |
| U-SE | C6 | Microchip **ATECC608B-TFLXTLSU** (TrustFLEX) | PROPOSED | Prefer Trust Platform SKU |
| HAB | C6 | i.MX8M Plus HAB (on SoM) | HAVE | Process missing |
| U-PWR | C10 | Traco **TEN 40-2412WIR** | PROPOSED | ≥40 W — not 6 W SIP |
| U-LTE | C7 | Quectel **EG25-G** (rev A) | OPTIONAL | Do not gate freeze |
| SW-61850 | C3 | libiec61850 + **MZ commercial quote** | LEGAL HOLD | Quote before ship |

---

## Component — U-SOM

### Function
Linux compute for PF2 CCI: IEC 61850 / 60870 / Modbus stacks, dual GbE MAC, HAB secure boot, industrial temperature.

### Selected Part
| Field | Value |
|-------|-------|
| Manufacturer | Toradex |
| Part Number | **0063** Verdin iMX8M Plus Quad 4GB IT (order latest rev e.g. **00631102** V1.1C) |
| Lifecycle | Active family; **PCN in progress** (V1.1B → V1.1C, LTB noted for older rev) |
| Temperature | Industrial (IT) |
| Package | Verdin MXM3 SoM |
| Lead time / availability | Webshop / FAE — often constrained; validate replacement rev |
| Typical unit price | ~EUR 150–350 |
| Second source | Variscite DART-MX8M-PLUS (architecture alternate only) |

### Evidence
- Datasheet: https://developer.toradex.com/hardware/verdin-som-family/modules/verdin-imx8m-plus/ · `ccli-toradex-verdin-plus-som`
- Application Note: Toradex Carrier Board Design Guide — https://docs.toradex.com/108140-verdin-carrier-board-design-guide.pdf
- Reference Design: Verdin Development Board, Dahlia, Ivy carriers (Toradex)
- Commercial Product Example: Multiple Toradex OEM carriers in industrial HMI/gateways

### Purchasing Sources
- Toradex webshop (primary): https://www.toradex.com/  
- DigiKey / Mouser: limited / often OEM — search “Verdin iMX8M Plus 0063”
- Farnell / Arrow / Avnet: typically via Toradex channel partners — request quote

### Lifecycle
Active IT SKU with published PCNs. **Request longevity/PCN letter (gate D6) before Architecture Freeze.**

### Risks
| Risk | Level | Note |
|------|-------|------|
| Supply-chain | High | Single-vendor SoM; RAM/eMMC PCNs |
| Obsolescence | Medium | Toradex longevity program — get letter |
| Single-source | High | No pin-compatible second source without redesign |
| Technical | Low–Med | Proven i.MX8M Plus path |
| Cybersecurity | Low (silicon) | HAB present; key ceremony still MISSING |
| Industrial | High fit | IT grade |

### Alternative Parts
- **0058** WB IT (Wi-Fi) — not needed for cabinet CCI  
- Variscite DART-MX8M-PLUS — dual GbE alternate path (HAVE docs) — different connector ecosystem  

### Recommendation
**KEEP 0063.** Order samples of current production rev; obtain PCN/longevity letter; do not dual-source SoM without full carrier redesign.

---

## Component — KIT (lab only)

### Function
Phase-1 lab DUT for software/HIL — **not** field product.

### Selected Part
Toradex Verdin Development Board Kit **99991105** (~EUR 300).

### Evidence
- Datasheet: https://docs.toradex.com/116796-verdin_development_board_datasheet.pdf  
- Product: https://www.toradex.com/products/carrier-board/verdin-development-board-kit  

### Purchasing Sources
Toradex webshop primary. DigiKey/Mouser search “99991105”.

### Lifecycle
Active eval product.

### Risks
**Process risk High** if treated as product (R-BOM-001). Kit RS485 DB9 ≠ opto RJ45 C2.

### Recommendation
**BUY for lab.** Label every report “LAB ONLY”.

---

## Component — SW1 (C1 Ethernet)

### Function
Network segregation for Eth_A (DSO), Eth_B (Operator), Plant-1/2. Must support port isolation / no L2 bridge Eth_A↔Eth_B.

### Selected Part (proposed topology)
**Hybrid (recommended pending ADR-005):**
1. SoM **EQOS/FEC** → Eth_A + Eth_B (dedicated magjacks)  
2. Microchip **KSZ9893RNXI** → Plant-1 + Plant-2 copper + RGMII/RMII to SoM **if** third MAC/RGMII available in pin budget  

If pin budget cannot spare RGMII: use **KSZ9567** / discrete PHY path — re-open ADR.

| Field | Value |
|-------|-------|
| Manufacturer | Microchip |
| Part Number | **KSZ9893RNXI** (industrial) |
| Lifecycle | Active |
| Temperature | −40…+85 °C (I grade) |
| Package | 64-VQFN 8×8 |
| Price class | ~EUR 8–20 (verify) |
| Second source | Marvell / Realtek industrial switches (different SW) |

### Evidence
- Datasheet / product: https://www.microchip.com/en-us/product/KSZ9893  
- Product brief: https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/00002318A.pdf  
- Evaluation board: **EVB-KSZ9893** (Microchip)  
- Features: VLAN, QoS, ACL, 802.1X — useful for plant isolation  

### Purchasing Sources
- DigiKey: search `KSZ9893RNXI`  
- Mouser: search `KSZ9893RNXI`  
- Farnell / Arrow / Avnet: same MPN  

### Lifecycle
Active industrial SKU.

### Risks
| Risk | Level | Note |
|------|-------|------|
| Technical | **High until ADR** | 3-port device ≠ 4 copper ports alone |
| Supply | Medium | Broadline stocked |
| Single-source | Medium | Other Microchip KSZ9xxx family |
| Cyber | Medium | Must disable unused bridging; ACL/VLAN config is FW duty |

### Alternative Parts
- **KSZ9477 / KSZ9567** — more ports if SoM dual GbE absorbed into switch  
- Discrete **RTL8211FI** PHYs ×2 for plant only  

### Recommendation
**PROPOSE KSZ9893RNXI for plant expansion only after ADR-005 + pin budget.** Do not buy for production until topology locked. Original BOM EUR 8–20 may be OK for this IC alone; full C1 BOM higher with magnetics.

---

## Component — J-ETH (magjacks ×4)

### Function
RJ45 + magnetics for Eth_A/B/Plant; EMC front-end.

### Selected Part
Pulse Electronics **JK0-0136NL** class (or Bel Fuse / TE MagJack GbE industrial) — **finalize after PHY/switch pinout**.

### Evidence
- Pulse MagJack catalog / DigiKey “RJ45 MagJack 1000Base-T”  
- Toradex carrier guide magnetics recommendations  

### Purchasing Sources
DigiKey / Mouser / Farnell / Arrow — search selected MagJack MPN.

### Risks
EOL of specific MagJack SKUs common → **qualify two footprints**. PoE vs non-PoE must match design.

### Recommendation
Pick MagJack **after** SW1/PHY freeze; dual-source same footprint.

---

## Component — U-485 (C2)

### Function
2× galvanically isolated RS-485 Modbus RTU to energy analyzer / spare (product C2).

### Selected Part
| Field | Value |
|-------|-------|
| Manufacturer | Analog Devices |
| Part Number | **ADM2795EARWZ** (×2) |
| Lifecycle | Active |
| Temperature | −40…+125 °C |
| Package | 16-lead wide-body SOIC |
| Isolation | 5 kV rms signal isolation + Level 4 EMC on bus |
| Price class | ~EUR 10–20 ea (verify) |
| Second source | TI ISO1410 / ISO1430; ADI ADM2587E (older) |

### Evidence
- Datasheet: https://www.analog.com/media/en/technical-documentation/data-sheets/adm2795e.pdf  
- Product: https://www.analog.com/en/products/adm2795e.html  
- Applications: industrial fieldbus, utility networks (ADI listed)  
- Eval: ADI EVAL-ADM2795E / related isolated RS-485 boards  
- MikroE / community boards exist around ADM2795E  

### Purchasing Sources
- DigiKey: search `ADM2795EARWZ`  
- Mouser: search `ADM2795EARWZ`  
- Farnell / Arrow / Avnet: same  

### Lifecycle
Active; industrial/utility positioning.

### Risks
| Risk | Level | Note |
|------|-------|------|
| Supply | Low–Med | ADI broadline |
| Obsolescence | Low | Strong industrial part |
| Single-source | Med | Cross to TI ISO14xx |
| Technical | Low | Excellent EMC integration |
| Cyber | Low | Physical layer only |
| Industrial | **Excellent** | Prefer over SK0146 ADM3483 (non-isolated) |

### Alternative Parts
- **ISO1410BDWR** (TI) — reinforced isolated RS-485  
- **ADM2582E** — if power-isolated bus preferred  

### Recommendation
**SELECT ADM2795EARWZ ×2** for product C2. Do **not** copy SK0146 ADM3483 into CCI.

---

## Component — U-DI (C4)

### Function
Sense field DI 10–120 Vdc ×5 with galvanic isolation into SoM GPIO (PF2 status).

### Selected Part
| Field | Value |
|-------|-------|
| Manufacturer | Texas Instruments |
| Part Number | **ISO1212DBQ** (dual channel) ×3 → 6 ch (use 5) |
| Lifecycle | Active |
| Temperature | Industrial grade variants — verify orderable |
| Package | 16-SSOP |
| Field range | Designed 24–60 V; **9–300 V** with external RTHR/RSENSE (TI docs) |
| Price class | ~EUR 3–6 / IC |
| Second source | ISO1211 (1 ch); ISO1228 (8 ch denser); discrete opto + clamp |

### Evidence
- Product: https://www.ti.com/product/ISO1212  
- Datasheet: https://www.ti.com/lit/ds/symlink/iso1212.pdf  
- App note: https://www.ti.com/lit/ab/slla370d/slla370d.pdf (PLC DI modules)  
- EVM: **ISO1212EVM**  
- Industrial use: PLC DI modules, grid/motor I/O (TI positioning)  

### Purchasing Sources
DigiKey / Mouser / Farnell / Arrow / Avnet — `ISO1212DBQ`

### Lifecycle
Active; PLC-oriented.

### Risks
| Risk | Level | Note |
|------|-------|------|
| Technical | Medium | Must **prove 10 V and 120 V** thresholds with resistor network |
| Supply | Low | TI broadline |
| Cyber | Low | Status inputs — debounce + authenticity in FW |
| Industrial | High | IEC 61131-2 Types 1/2/3 |

### Alternative Parts
- Discrete **TLP291** / **ACPL** optos + zener clamp (SK0146 craft) — more BOM lines  
- **ISO1228** if denser 8-ch preferred  

### Recommendation
**PROPOSE ISO1212** family; validate full 10–120 V on Wave B before schematic freeze. Do not reuse SK0146 3.3 V short-to-GND limits.

---

## Component — K-DO (C4)

### Function
Dry-contact outputs ×3 for PF2 curtailment (drive external contactors / indicators).

### Selected Part
| Field | Value |
|-------|-------|
| Manufacturer | Omron |
| Part Number | **G2RL-1A-E-DC12** (SPST-NO high capacity, 12 V coil) ×3 |
| Lifecycle | Active |
| Temperature | −40…+85 °C class (verify) |
| Package | PCB through-hole |
| Contacts | Up to **16 A @ 250 VAC** (resistive) — suitable for contactor coils with snubber |
| Price class | ~EUR 2–5 ea |
| Second source | Panasonic JW1FSN-DC12V; TE Potter & Brumfield T9A |

### Evidence
- Datasheet: https://omronfs.omron.com/en_US/ecb/products/pdf/en-g2rl.pdf (family) / DigiKey G2RL PDF  
- DigiKey category: Power Relays >2 A  
- Industrial use: widespread PCB power switching  

### Purchasing Sources
DigiKey / Mouser / Farnell / Arrow — `G2RL-1A-E-DC12`

### Lifecycle
Active long-running Omron family.

### Risks
| Risk | Level | Note |
|------|-------|------|
| Technical | Medium | Add TVS/snubber for inductive contactor coils |
| Cyber / safety | **High system** | Needs HW WDT / safe-state (DRB) — relay alone insufficient |
| Reject | — | **Do not use G6K signal relays** for curtailment |

### Alternative Parts
- **G5LE-1A4-DC12** — lower cost  
- External DIN relay module (Phoenix Contact) — if PCB space / certification easier  

### Recommendation
**SELECT G2RL-1A-E-DC12 ×3** (or equivalent 16 A class). Spec inductive load + failsafe path in architecture.

---

## Component — U-GNSS (C5)

### Function
UTC/GNSS time + **PPS** for event timestamping (REQ-TIME-001).

### Selected Part
| Field | Value |
|-------|-------|
| Manufacturer | u-blox |
| Part Number | **NEO-M9N-00B** |
| Lifecycle | Active (M9 platform) |
| Temperature | Module industrial options — confirm IT bin on PO |
| Package | 12.2 × 16.0 mm LCC |
| Interfaces | UART/I2C/SPI + **TIMEPULSE (PPS)** |
| Price class | ~EUR 20–40 |
| Second source | Quectel LC29H / L76; u-blox ZED-F9P (overkill RTK) |

### Evidence
- Datasheet: https://content.u-blox.com/sites/default/files/NEO-M9N-00B_DataSheet_UBX-19014285.pdf  
- Eval kit: **EVK-M91** — https://content.u-blox.com/sites/default/files/documents/EVK-M91_Userguide_UBX-19056858.pdf  
- Integration manual: UBX-19014286 (u-blox)  

### Purchasing Sources
DigiKey / Mouser / Farnell / Arrow — `NEO-M9N-00B`  
EVK: DigiKey/Mouser search `EVK-M91`

### Lifecycle
Active; watch u-blox PCN on M9.

### Risks
| Risk | Level | Note |
|------|-------|------|
| Supply | Medium | Module EOL cycles |
| Technical | Low | PPS pin must reach SoM GPIO in pin budget |
| PTP | Separate | IEEE1588 not required unless CEI demands — GNSS first |

### Recommendation
**PROPOSE NEO-M9N-00B** + buy EVK-M91 for bring-up. Antenna SMA active next.

---

## Component — ANT-G

### Function
Active GNSS antenna to U-GNSS.

### Selected Part
Taoglas **AA.171.301111** class or Molex / Pulse active GPS/GNSS SMA — match 3–5 V bias from module.

### Evidence
Module integration manuals; DigiKey “Active GPS Antenna SMA”.

### Purchasing Sources
DigiKey / Mouser / Farnell.

### Recommendation
Select after GNSS module bias voltage confirmed; keep cable length short in cabinet.

---

## Component — U-SE (C6)

### Function
Hardware root of trust / key storage for TLS, secure boot assist, device identity (REQ-SEC-002).

### Selected Part
| Field | Value |
|-------|-------|
| Manufacturer | Microchip |
| Part Number | **ATECC608B-TFLXTLSU** (TrustFLEX) or TrustCUSTOM for production |
| Lifecycle | Prefer **Trust Platform SKUs** (base ATECC608B marked NRND for new designs on some pages) |
| Temperature | −40…+85 °C |
| Package | 8-UDFN (or SOIC) |
| Price class | ~EUR 1–4 (+ provisioning) |
| Second source | STSAFE-A110; NXP SE050; on-SoM TPM if SKU proves it |

### Evidence
- Product: https://www.microchip.com/en-us/product/atecc608b  
- Trust Platform: https://ww1.microchip.com/downloads/en/DeviceDoc/00003278C.pdf  
- Eval: Trust Platform USB Kit **DM320118**, ATECC608B Trust Kit **DT100104**  
- App note: ATECC608A→B migration DS40002237  

### Purchasing Sources
DigiKey / Mouser / Farnell / Arrow / Avnet — TrustFLEX ordering codes.

### Lifecycle
Use Trust Platform production codes — not generic engineering samples alone.

### Risks
| Risk | Level | Note |
|------|-------|------|
| Cyber | **Critical if omitted** | Filesystem keys = R-SEC-001 |
| Supply | Low | Microchip broadline |
| Single-source | Med | ST / NXP SE alternatives |
| Process | High | Provisioning + key ceremony SOP still MISSING |

### Recommendation
**PROPOSE ATECC608B TrustFLEX** for prototype; plan TrustCUSTOM for production. Confirm I2C on Verdin pin budget. Alternative: prove Toradex SoM TPM option before committing discrete SE.

---

## Component — HAB (on SoM)

### Function
High Assurance Boot — reject unsigned images.

### Selected Part
NXP i.MX8M Plus HAB (included in **0063** silicon) + Toradex/meta-toradex-security / signed boot recipes.

### Evidence
- NXP UG10164 / HAB guides  
- https://github.com/toradex/build-signed-imx8m-bootloader  
- Toradex meta-toradex-security  

### Purchasing Sources
N/A (SoM feature).

### Risks
Without fuse policy + CI signing = **checkbox security**.

### Recommendation
**KEEP.** Author key hierarchy SOP before Hardware Freeze.

---

## Component — U-PWR (C10)

### Function
12–24 Vdc field input → isolated intermediate rail for SoM + carrier (PHY, RS485, relays).

### Selected Part
| Field | Value |
|-------|-------|
| Manufacturer | Traco Power |
| Part Number | **TEN 40-2412WIR** (9–36 Vin → 12 Vout, 40 W) |
| Lifecycle | Active; EN 50155 railway / industrial |
| Temperature | −40…+85 °C with derating |
| Package | 2"×1" brick |
| Isolation | 1600–3000 VDC class (see DS for Vin model) |
| Price class | ~EUR 40–70 (verify) — **above old BOM 10–25** |
| Second source | MEAN WELL DDR-30A-12 DIN; RECOM RPA40-2405S |

### Evidence
- Product: https://www.tracopower.com/model/ten-40-2412wir  
- Datasheet: https://www.tracopower.com/sites/default/files/products/datasheets/ten40wir_datasheet.pdf  

### Purchasing Sources
DigiKey / Mouser / Farnell / Arrow / Avnet — `TEN 40-2412WIR`

### Lifecycle
Active industrial/railway series.

### Risks
| Risk | Level | Note |
|------|-------|------|
| Technical | **Reject 6 W SIP** | Verdin + GbE + relays exceed TMR 6WIR |
| Supply | Low–Med | Traco widely stocked |
| Thermal | Medium | Derating in sealed DIN enclosure |

### Alternative Parts
- **TEN 40-2411WIR** (5 V) if feeding SoM 5 V input path  
- DIN-rail **MEAN WELL DDR-30/60** if mechanical prefers DIN module  

### Recommendation
**SELECT ≥40 W isolated converter (TEN 40-2412WIR class).** Update BOM cost envelope. Complete load budget before PCB.

---

## Component — U-LTE (C7 optional)

### Function
Cellular backup / remote monitoring (rev A).

### Selected Part
Quectel **EG25-G** (LTE Cat.4 global) or Cat.1 **EG91/EG95** if lower power preferred.

### Evidence
- Quectel product pages; UMTS&LTE EVB kit  
- DigiKey: `UMTSLTEEVB-KIT-B`  
- Mouser: Quectel UMTS LTE EVB  

### Purchasing Sources
DigiKey / Mouser / Arrow — module + EVB.

### Risks
Regional certification, antenna, single-source modem, **must not bridge to Eth_A**.

### Recommendation
**DEFER** — optional rev A. Do not block Architecture Freeze.

---

## Component — SW-61850 (software)

### Function
IEC 61850 MMS/GOOSE stack on Linux.

### Selected Part
**libiec61850** (MZ Automation) — lab GPLv3; **commercial license required for ship**.

### Evidence
- https://github.com/mz-automation/libiec61850  
- MZ Automation commercial licensing  

### Purchasing Sources
MZ quote (not DigiKey).

### Risks
**Legal High** (R-LIC-001) if shipped on GPLv3 only.

### Recommendation
**Get commercial quote now.** Keep GPLv3 for Wave A lab only.

---

## Component — PCB / ENC

### Function
4-layer carrier prototype; DIN-rail enclosure.

### Selected Part
- PCB: JLCPCB / PCBWay 4-layer impedance-controlled  
- Enclosure: Phoenix Contact / Hammond / Fibox DIN — after thermal envelope  

### Evidence
Standard fab houses; mechanical after C9 thermal budget.

### Recommendation
No MPN lock until outline freeze (D18).

---

## Supply-chain scorecard (proposed set)

| Ref | MPN | DigiKey/Mouser path | Obsolescence | Single-source | Industrial |
|-----|-----|---------------------|--------------|---------------|------------|
| U-SOM | 0063 | Toradex primary | Med (PCN) | High | Yes IT |
| U-485 | ADM2795EARWZ | Broadline | Low | Med | Excellent |
| U-DI | ISO1212DBQ | Broadline | Low | Low | Excellent |
| K-DO | G2RL-1A-E-DC12 | Broadline | Low | Low | Yes |
| U-GNSS | NEO-M9N-00B | Broadline | Med | Med | Yes |
| U-SE | ATECC608B TrustFLEX | Broadline | Med (use Trust SKU) | Med | Yes |
| U-PWR | TEN 40-2412WIR | Broadline | Low | Med | Railway/ind |
| SW1 | KSZ9893RNXI | Broadline | Low | Med | Yes I-temp |

---

## Architect holds (do not PO yet)

1. **SW1 / J-ETH** — wait for ADR-005 + pin budget  
2. **All carrier ICs** — wait for D3 pin budget  
3. **U-SE production provisioning** — wait for key ceremony SOP  
4. **SW-61850 commercial** — wait for MZ quote  

## Safe to buy now

1. Toradex **0063** + **99991105** (lab)  
2. Wave A mocking bench (separate BOM)  
3. Eval kits: EVB-KSZ9893, EVAL-ADM2795E / ISO1212EVM, EVK-M91, Trust Platform USB kit — for learning, not production freeze  

---

## Regeneration

Enrichment lives in this file. To push proposed MPNs into Excel/DB, run (after Agent updates corpus):

```powershell
python scripts/generate-architecture-from-rag.py
python scripts/init-architecture-db.py --no-rag-probe
```

**RAG source_id (when ingested):** `ccli-bom-component-review`

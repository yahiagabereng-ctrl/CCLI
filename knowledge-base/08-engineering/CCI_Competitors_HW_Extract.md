# CCI Competitors — Hardware Extract

**Document ID:** CCLI-COMP-002  
**Revision:** 1.0  
**Date:** 2026-09-24  
**RAG source_id:** `ccli-competitors-hw-extract`  
**Sources:** `competitors/competitors/{AiLux,Higeco,MC(german),STCE-SG CCI,tesmec}/`  
**Status:** **HAVE** (brochure-level; not full schematics)

---

## 1. AiLux CCI module (`ccli-comp-ailux`)

From `CCI_Tecnico.pdf` / already classified in `CCI_Module_Regulations_Classification.md`.

| Block | Spec |
|-------|------|
| CPU | i.MX6D Cortex-A9 dual-ish class, **800 MHz** |
| RAM / Flash | **1 GB** DDR3, **4 GB** eMMC |
| Ethernet | **4× 10/100** switch — Eth_A (DSO), Eth_B (OA), **2× plant** |
| Serial | **2× opto** RS-232/485, RJ45 Modbus pinout |
| USB | 1× local config + 2× plant |
| DI / DO | **5×** opto DI **10–120 Vdc**; **3×** relay DO **250 Vac / 3 A** |
| GNSS / LTE | GPS/GLONASS; LTE Cat.1 |
| Security | Secure Boot, TPM 2.0, FIPS 140-2 L3, 62443-4-1/2, anti-tamper |
| Power / temp | **12–24 Vdc ±20%**, 5 W; **−5…+45 °C** |
| Norms | CEI 0-16 O–T; 61557-12; 62351; ARERA |

---

## 2. Higeco More CCI (`ccli-comp-higeco`)

From `Brochure_CCI_v3_ENG.pdf` + `CCI_Compliance_FAQ.html`.

| Block | Spec |
|-------|------|
| CCI Ethernet | **2× Eth 1000Base-T** — brochure: **“(No switch/bridge)”** |
| Media | **Ethernet Medium Converter 100/1000Base-X SFP** |
| Extra switch | **2× 100/1000Base-T** (cabinet interconnect — separate from CCI claim) |
| Serial | **2× RS485**; 2× CAN optional |
| Digital | **13× DI** |
| Protocols | IEC 61850, 60870-101/104, Modbus; export DNP3 |
| UI | USB configuration |
| GNSS | GPS/GLONASS 32 dB |
| Analyzer | Contrel EMA-90-N Class **0.2S**; isolation **2.5 kV** on PA inputs |
| Cabinet PSU | AC **230 Vac** or DC **110 Vdc** SKUs |
| Temp | **0…+40 °C** (cabinet) |
| Conformity | CEI O/T 2022-03; **IEC 62443-4-1/4-2**; **UCA IEC 61850** cert; IEC 62351-3 |

FAQ HW-relevant (Higeco public FAQ):

- Time sync: **GPS or NTS** — plain NTP **not** enough (Q05).  
- Cyber: **62443-4-1 + 4-2** + **FIPS 140-2 L3** (Q14).  
- Lab: insulation/EMC → **ISO 17025**; functional → **ISO 17065** (Q15).  
- Measurement: CCI’s own MU; not PI/PG CTs (Q08–Q09).

---

## 3. meteocontrol Cabinet CCI (`ccli-comp-meteocontrol`)

From Italian datasheet (blue'Log XC + Cabinet CCI).

| Block | Spec |
|-------|------|
| Scope | MV plants **≥100 kW** bands to **>1 MW**; PF1/PF2/PF3 |
| Network | **2× Ethernet copper** + **1× Ethernet copper/fibre** |
| DSO protocol | **IEC 61850 secure** (IEC 62351) |
| Plant protocols | **Modbus TCP**; optional **IEC 60870-5-104** |
| DI / relays | **16× DI**; **6×** interface relays |
| Time | GPS antenna; **(S)NTP** |
| Power | **230 Vac / 16 A**; internal **24 Vdc**; IP55 |
| Processing | Full PF1/PF2/PF3 claimed |

---

## 4. STCE-SG / Digital Platforms (`ccli-comp-stce`)

From `STCE-SG_CCI-DP.pdf` (OCR spaced).

| Block | Spec |
|-------|------|
| Form | Modular **RTU** — DIN or wired cabinet |
| I/O | **16 DI**, **4 AI**, **8 DO**, power meter, GPS |
| Centre protocols | **IEC 61850**; **IEC 60870-5-104 / 62351** |
| Field | Modbus, wired I/O; IEC 61131 automation |
| Security | Security-by-design; 62351; 62443 claimed |
| Norms | CEI 0-16 O/T; Terna A.72; ARERA 385/2025, 564/2025 |

---

## 5. Tesmec TC-CCI (`ccli-comp-tesmec`)

From `CCI_IT-aggiornamento2 1-compressed.pdf`.

| Block | Spec |
|-------|------|
| Architecture | Modular PLC: CPU+I/O, **media converter Cu/fibre**, GNSS/NTP, **Ethernet switch** |
| Eth_A | **1× 100BaseFX** via media converter (CEI 0-16 style) |
| Eth_B | Dedicated Ethernet for other enabled actors |
| Plant Eth | **2× 100BaseTX** spare **dedicated to plant only** |
| Field bus | Modbus RTU (RS-485) or Modbus TCP — **separate from DSO channel** |
| DI | Expandable to **16**; **110 Vdc** nom. (≈80–165 V); field isolation relays |
| GNSS | GPS / GLONASS / GALILEO, SMA, 32 channels |
| Enclosure | **IP55**; supply **230 Vac** or **110 Vdc** |
| Measurement | Integrated V/I paths (wide range stated for CT/VT classes) |

---

## Cross-competitor HW comparison (summary)

| Feature | AiLux | Higeco | meteocontrol | STCE | Tesmec | CCLI TesPro TG544 |
|---------|-------|--------|--------------|------|--------|-------------------|
| Eth_A / Eth_B / plant split | 4× FE roles | **2× GE no bridge** + SFP FX | 2 Cu + 1 Cu/FX | Modular | **FX + Eth_B + 2 plant TX** | 1 WAN + 4 LAN; **SW zones** |
| Explicit “no bridge” | Implied by roles | **Stated** | Implied | — | Plant bus separated | **Must prove** |
| Eth_A optical | Not in module sheet | **SFP converter** | **1× Cu/fibre** | — | **100BaseFX** | **Need IMC extension** |
| DI class | **5× 10–120 V** | 13× DI | 16× DI | 16× DI | ≤16 @ **110 Vdc** | **4× DIO** OEM — **GAP** |
| DO / relays | **3× 250 Vac** | via cabinet | 6× iface relays | 8× DO | field relays | **GAP** vs AiLux |
| RS485 | 2× opto RJ45 | 2× RS485 | (via blue'Log) | Modbus | RTU/TCP | 2× ttyS — isol. **OPEN** |
| GNSS | Yes | Yes | Yes | Yes | Yes | Brochure — **verify** |
| 62443 / FIPS | Claimed | Claimed + FAQ | — | Claimed | — | Brochure — **verify** |
| 61850 UCA | — | **Cert claimed** | — | — | Stack claimed | **OPEN** |

---

## Knowledge gaps

| Vendor | Gap |
|--------|-----|
| All | Full Ethernet ASIC topology (shared switch vs multi-MAC) |
| STCE | Exact Eth port count / FX |
| AiLux | Schematic for Eth galvanic / bridge policy |
| Higeco | How “no switch/bridge” is implemented in silicon |

`AiLux`, `Higeco`, `Tesmec`, `meteocontrol`, `STCE`, `Eth_A`, `100BaseFX`, `no bridge`, `ccli-competitors-hw-extract`

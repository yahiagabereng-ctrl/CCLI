# NXP i.MX 8M Dual / QuadLite / Quad — Precision RAG Extract

**Source PDF:** `IMX8MDQLQCEC-1280316.pdf`  
**Document Number:** IMX8MDQLQCEC  
**Title:** i.MX 8M Dual / 8M QuadLite / 8M Quad Applications Processors Data Sheet for Consumer Products  
**Revision:** 3, 04/2021  
**RAG companion source_id:** `ccli-nxp-imx8m-dqlq-cec-precise`  
**Parent PDF source_id:** `ccli-nxp-imx8m-dqlq-cec`

> Purpose: diagram pages in the PDF have corrupted text-layer glyphs for signal names (e.g. NAND_CE_B rendered as garbled symbols). This extract restates **tables and section headings** in clean text so RAG answers on timing/electricals are precise. Always verify final numbers against the PDF page cited.

---

## 1. Product identity

- Family: **i.MX 8M Dual / 8M QuadLite / 8M Quad** (not i.MX 8M Plus, not i.MX 8M Mini)
- Package: Bare Die Package, FBGA 17 × 17 mm, 0.65 mm pitch
- Example orderables (from cover): MIMX8MQ6DVAJZAA/AB, MIMX8MQ5DVAJZAA/AB, MIMX8MQ6DVAJZIB, MIMX8MD6DVAJZAA/AB
- Grade in this PDF: **Consumer Products** electricals — Industrial SKUs require separate confirmation

---

## 2. Core features (Table 1 — cleaned)

### CPU
- Quad symmetric **Cortex-A53** @ up to **1.5 GHz**
  - 32 KB L1 I-cache, 32 KB L1 D-cache per core
  - L1 parity/ECC; **1 MB** unified L2 with ECC
  - NEON + FPU; Armv8-A 64-bit
- **Cortex-M4** co-processor: 16 KB I/D, **256 KB TCM**

### Connectivity
- **2× PCIe Gen2** (1-lane each)
- **2× USB 3.0/2.0** with integrated PHY (OTG)
- **2× uSDHC**
- **1× Gigabit Ethernet** with EEE, AVB, **IEEE 1588**
- **4× UART** (to 5 Mbps), **4× I2C**, **3× SPI**, **4× PWM**

### Memory interfaces
- DRAM: LPDDR4-3200 / DDR4-2400 / DDR3L-1600 (32/16-bit)
- 8-bit NAND Flash (GPMI)
- eMMC 5.0, SPI NOR, QuadSPI (XIP)

### Multimedia (variant-dependent)
- VPU: 4Kp60 HEVC/H.265 + VP9; 4Kp30 H.264; 1080p60 legacy codecs (Quad/Dual; QuadLite lacks HW video decode)
- GPU: 4 shaders; OpenGL ES 3.1, OpenCL 1.2, Vulkan
- HDMI 2.0a Tx (to 4Kp60), eDP, MIPI-DSI, 2× MIPI-CSI2

### Security
- Resource Domain Controller (RDC)
- Arm **TrustZone**
- **HAB** (High Assurance Boot)
- CAAM crypto acceleration
- SNVS secure RTC, Secure JTAG, eFuse key storage, TRNG, 32 KB Secure RAM

---

## 3. GPMI / NAND Flash timing overview (Section 3.9.4)

GPMI = General-Purpose Media Interface (NAND controller):
- 8-bit data width
- Up to **200 MB/s** I/O
- Modes:
  1. **Asynchronous** — ONFI 1.0 compatible (NF1–NF17) — §3.9.4.1
  2. **Source Synchronous** — ONFI 2.x compatible (NF18–NF31) — §3.9.4.2 ← matches user Figure 26
  3. **ONFI NV-DDR2** — ONFI 3.2 compatible — §3.9.4.3
  4. **Toggle** — §3.9.4.4

---

## 4. Source Synchronous mode (ONFI 2.x) — Figure 26 context

**Section:** 3.9.4.2 Source synchronous mode AC timing (ONFI 2.x compatible)  
**PDF pages:** 55–57  
**Figures:**
- Figure 26 — Source Synchronous mode **command and address** timing (signals: NAND_CE_B, NAND_CLE, NAND_ALE, NAND_WE/RE_B, NAND_CLK, NAND_DQS, NAND_DATA[7:0])
- Figure 27 — data write timing
- Figure 28 — data read timing
- Figure 29 — NAND_DQS / NAND_DQ read valid window

### Figure 26 parameter callouts (IDs on the diagram)

| ID | Role on Figure 26 (command/address) |
|---|---|
| NF18 | NAND_CE_B access / CE assertion window related |
| NF19 | NAND_CE_B hold (release) |
| NF20 | Command/address DATA setup |
| NF21 | Command/address DATA hold |
| NF22 | Clock period tCK |
| NF23 | Preamble delay |
| NF24 | Postamble delay |
| NF25 | NAND_CLE and NAND_ALE **setup** |
| NF26 | NAND_CLE and NAND_ALE **hold** |

Signal names (correct, not PDF glyph garbage):
- `NAND_CE_B` (chip enable, active low)
- `NAND_CLE` (command latch enable)
- `NAND_ALE` (address latch enable)
- `NAND_WE/RE_B` (write/read enable)
- `NAND_CLK`, `NAND_DQS`, `NAND_DATA[7:0]`

---

## 5. Table 50 — Source Synchronous mode timing parameters (precise)

**Source:** IMX8MDQLQCEC Rev. 3, page 57, Table 50  
**Notes from datasheet:**
1. Output timing controlled by registers `GPMI_TIMING2_CE_DELAY`, `GPMI_TIMING_PREAMBLE_DELAY`, `GPMI_TIMING2_POST_DELAY` (CE_DELAY / PRE_DELAY / POST_DELAY in formulas).
2. `T = tCK` (GPMI clock period) − 0.075 ns (half of maximum peak-to-peak jitter), where noted.

| ID | Parameter | Symbol | Timing formula | Unit |
|---|---|---|---|---|
| NF18 | NAND_CE0_B access time | tCE | CE_DELAY × T − 0.79 | ns |
| NF19 | NAND_CE0_B hold time | tCH | 0.5 × tCK − 0.63 | ns |
| NF20 | Command/address NAND_DATAxx setup | tCAS | 0.5 × tCK − 0.05 | ns |
| NF21 | Command/address NAND_DATAxx hold | tCAH | 0.5 × tCK − 1.23 | ns |
| NF22 | Clock period | tCK | — | ns |
| NF23 | Preamble delay | tPRE | PRE_DELAY × T − 0.29 | ns |
| NF24 | Postamble delay | tPOST | POST_DELAY × T − 0.78 | ns |
| NF25 | NAND_CLE and NAND_ALE setup time | tCALS | 0.5 × tCK − 0.86 | ns |
| NF26 | NAND_CLE and NAND_ALE hold time | tCALH | 0.5 × tCK − 0.37 | ns |
| NF27 | NAND_CLK to first NAND_DQS latching transition | tDQSS | T − 0.41 | ns |
| NF28 | Data write setup | — | 0.25 × tCK − 0.35 | — |
| NF29 | Data write hold | — | 0.25 × tCK − 0.85 | — |
| NF30 | NAND_DQS/NAND_DQ read setup skew | — | Max 2.06 | — |
| NF31 | NAND_DQS/NAND_DQ read hold skew | — | Max 1.95 | — |

### DDR Source Synchronous read window (Figure 29 text)

At **200 MB/s**:
- Typical **tDQSQ** = **0.85 ns (max)**
- Typical **tQHS** = **1 ns (max)**
- Sample `NAND_DATA[7:0]` on both edges of delayed `NAND_DQS` (internal DPLL)
- Delay register: `GPMI_READ_DDR_DLL_CTRL.SLV_DLY_TARGET` (see IMX8MDQLQRM)
- Typical delay value **0x7** ≈ 1/4 clock cycle; increase if board delay is significant

---

## 6. Table 49 — Asynchronous mode (ONFI 1.0) timing parameters

**Source:** page 53, Table 49. Max async I/O ≈ **50 MB/s**.  
Timing as multiples of GPMI clock cycle T with fixed delays. AS/DS/DH from GPMI timing registers.

| ID | Parameter | Symbol | Timing (T = GPMI clock cycle) | Unit |
|---|---|---|---|---|
| NF1 | NAND_CLE setup | tCLS | (AS + DS) × T − 0.12 | ns |
| NF2 | NAND_CLE hold | tCLH | DH × T − 0.72 | ns |
| NF3 | NAND_CE0_B setup | tCS | (AS + DS + 1) × T | ns |
| NF4 | NAND_CE0_B hold | tCH | (DH + 1) × T − 1 | ns |
| NF5 | NAND_WE_B pulse width | tWP | DS × T | ns |
| NF6 | NAND_ALE setup | tALS | (AS + DS) × T − 0.49 | ns |
| NF7 | NAND_ALE hold | tALH | DH × T − 0.42 | ns |
| NF8 | Data setup | tDS | DS × T − 0.26 | ns |
| NF9 | Data hold | tDH | DH × T − 1.37 | ns |
| NF10 | Write cycle time | tWC | (DS + DH) × T | ns |
| NF11 | NAND_WE_B hold | tWH | DH × T | ns |
| NF12 | Ready to NAND_RE_B low | tRR | (AS + 2) × T | ns |
| NF13 | NAND_RE_B pulse width | tRP | DS × T | ns |
| NF14 | READ cycle time | tRC | (DS + DH) × T | ns |
| NF15 | NAND_RE_B high hold | tREH | DH × T | ns |

(Figures 21–25: Command / Address / Write / Read latch diagrams.)

---

## 7. Toggle / ONFI 3.2 cross-references

- **ONFI 3.2 command/address timing** = same as Async ONFI 1.0 (§3.9.4.1)
- **ONFI 3.2 read/write timing** = same as Toggle mode (§3.9.4.4)
- Toggle command/address = same as Async ONFI 1.0
- Toggle DDR window @ **133 MB/s**: tDQSQ max **1.4 ns**, tQHS max **1.4 ns** (page 60 text)
- Toggle Table 51 continues NF18/NF22–NF24/NF28–NF31 with slightly different formulas (verify PDF pp. 59–60)

---

## 8. CCI project relevance (what matters / what does not)

**Useful from this SoC for CCI:**
- A53 Linux + M4 assist
- HAB / TrustZone / CAAM for secure boot path (IEC 62443 supporting tech)
- 1× GbE with IEEE 1588 (still need external switch/PHYs for 4 CCI ports)
- UART/SPI/I2C for RS-485 bridges, GNSS, modem control
- eMMC/SD for firmware storage

**Usually not needed on CCI carrier:**
- NAND GPMI timing (Figures 26–31) unless you deliberately attach raw NAND — most SoMs use eMMC
- HDMI / 4K VPU / GPU / CSI camera blocks

**Datasheet limitation:**
- This file is **Consumer** electrical characteristics — do not treat as Industrial −40…+85 °C authority without matching Industrial orderable + SoM datasheet

---

## 9. Retrieval keywords (for RAG routing)

`IMX8MDQLQCEC`, `i.MX 8M Dual`, `i.MX 8M Quad`, `QuadLite`, `GPMI`, `NAND`, `ONFI 2.x`, `ONFI 1.0`, `source synchronous`, `NF18`, `NF19`, `NF20`, `NF21`, `NF22`, `NF23`, `NF24`, `NF25`, `NF26`, `NF27`, `NF28`, `NF29`, `NF30`, `NF31`, `NAND_CE_B`, `NAND_CLE`, `NAND_ALE`, `NAND_DQS`, `tCE`, `tCALS`, `tCALH`, `tDQSQ`, `tQHS`, `HAB`, `TrustZone`, `CAAM`, `IEEE 1588`, `PCIe Gen2`, `USB 3.0`, `Cortex-A53`, `Cortex-M4`

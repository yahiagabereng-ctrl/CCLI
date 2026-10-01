# MediaTek MT7986A Datasheet — Engineering Extract (Filogic 830)

**Document ID:** CCLI-SOC-MT7986A-001  
**Revision:** 1.0  
**Date:** 2026-08-11  
**RAG source_id:** `ccli-mt7986a-datasheet-extract`  
**PDF source_id:** `ccli-mt7986a-datasheet`  
**Source:** *MT7986A Datasheet for BPI-R3*, MediaTek Inc., **v1.15**, 2022-05-29 (23 pages)  
**Corpus status:** **PART** — SoC family reference; **not** TG-524 board pin map; confirm exact die with TesPro  
**Programme link:** K1.7 · `ccli-soc-freeze` (MT798X Filogic)

---

## Summary

MT7986A is MediaTek **Filogic 830** — a highly integrated router SoC with **quad Cortex-A53 @ up to 2.0 GHz**, **dual 2.5 Gbps HSGMII**, **HW NAT/QoS**, **Secure Boot**, and rich pin-mux (GPIO0–GPIO100+). This document is the **Banana Pi BPI-R3 companion datasheet**, useful for **pin-sharing schemes** and peripheral capability — **not** a substitute for TesPro TG-524 board DTS (K2.2).

### TG-524 applicability warning

| Attribute | MT7986A (this datasheet) | TesPro TG-524 (frozen spec) |
|-----------|--------------------------|----------------------------|
| CPU | **Quad** Cortex-A53 @ **2.0 GHz** | **Dual** Cortex-A53 @ **1.6 GHz** |
| Role | Wi-Fi 6E AX6000 router SoC | Industrial gateway (61850/Modbus/PF2) |

**TG-524 may use MT7981 or another MT798X variant**, not necessarily MT7986A. Use this doc for **Filogic architecture and mux patterns**; verify SoC SKU and pin assignment with TesPro before schematic or GPIO work.

---

## Key features (from datasheet)

| Block | Specification |
|-------|---------------|
| CPU | Quad ARM Cortex-A53 @ 2.0 GHz; 32 KB L1 I/D; 512 KB L2; NEON/FPU |
| DRAM | Discrete 16-bit DDR3/DDR4 |
| Storage | NOR (SPI), NAND (SPI SLC), **eMMC 5.1** |
| USB | USB3.0/USB2.0 host ×1 + USB2.0 host ×1 |
| PCIe | PCIe 2.0 ×1, 2-lane RC |
| Ethernet | **Two HSGMII (2.5 Gbps)** interfaces |
| Serial I/O | **SPI ×1**, **I2C ×1**, **UART-Lite (2-pin) ×1**, **UART (4-pin) ×2** |
| Other | GPIO, PWM, JTAG, MDC/MDIO (SMI), PCM audio |
| Wi-Fi | 4×4 + 4×4 Wi-Fi 6E integration (with external RF) |
| NAT/QoS | HW NAT (IPv4/IPv6), 128 HW queues, SFQ 1k queues |
| Security | Secure boot, crypto suite, anti-clone |
| Package | **16.85 × 16.85 mm** MFC VFBGA-570B, 0.65 mm pitch |

---

## Block diagram highlights (§1)

- **Ethernet:** HSGMII ×2 — typically to external PHY or **MT7531** switch (`MT7531_INT` on GPIO66).
- **Flash paths:** SPI NOR/NAND (SNFI), eMMC on dedicated EMMC_* pins (GPIO50–61).
- **Debug/console:** UART0 (GPIO39/40), UART1 (GPIO42–45), UART2 (GPIO46–49).
- **Plant-style expansion candidates:** UART1/UART2, SPI, I2C (board-dependent mux).

---

## Pin sharing / GPIO mux (§2.1 — Table 2-1)

Pins are **register-configurable** among Aux Func 0–4. Default name = Aux Func.0 unless configured otherwise.

### System / control

| Pin name | GPIO # | Notable alternate functions |
|----------|--------|----------------------------|
| SYS_WATCHDOG | GPIO0 | Watchdog |
| I2C_SCL / I2C_SDA | GPIO3 / GPIO4 | SGMII1 PHY I2C, USB3 PHY I2C |
| GPIO_0 / GPIO_1 | GPIO5 / GPIO6 | PCIe PHY I2C, SGMII0 PHY I2C |
| MT7531_INT | GPIO66 | Ethernet switch interrupt |
| SMI_MDC / SMI_MDIO | GPIO67 / GPIO68 | Ethernet MDIO (PHY management) |

### UART (RS-485 transceiver candidates — board wiring TBD)

| Interface | Pins (default) | GPIO # |
|-----------|----------------|--------|
| UART0 | UART0_RXD / UART0_TXD | GPIO39 / GPIO40 |
| UART1 | UART1_RXD/TXD/CTS/RTS | GPIO42–45 |
| UART2 | UART2_RXD/TXD/CTS/RTS | GPIO46–49 |

Many pins **also** mux to SPI flash, eMMC, or Wi-Fi — final function is **board DT + strap** dependent.

### SPI / flash / eMMC (storage dominates many pins)

| Signal | GPIO # | Alternate mux |
|--------|--------|---------------|
| SPI0_CLK…WP | GPIO23–28 | SNFI, eMMC DATA, UART1 |
| SPI1_* | GPIO29–32 | SPIC, eMMC, UART1/2 |
| SPI2_* | GPIO33–38 | SPI0, UART1/2, SPIC |
| EMMC_DATA_0–7 | GPIO50–57 | eMMC bus |
| EMMC_CMD / CK | GPIO58 / GPIO59 | eMMC |

### PWM / misc

| Pin | GPIO # | Alt |
|-----|--------|-----|
| PWM0 / PWM1 | GPIO21 / GPIO22 | PWM, EMMC_RSTB, NET_WO UART |

Full table spans **GPIO0–GPIO100+** (Wi-Fi RF host-bus pins on upper GPIOs).

---

## Strapping (§2.2)

Boot mode and configuration sampled at reset — see datasheet Table 2-2. **TG-524 strap set unknown** until vendor BSP provided.

---

## Electrical interfaces (§3 — summary)

| Interface | Notes |
|-----------|-------|
| UART | UART-Lite 2-wire or full 4-wire; see Fig 3-1 timing |
| SPI master | Fig 3-2; Table 3-4 electrical specs |
| I2C | Fig 3-5; single controller |
| HSGMII | 2.5 Gbps Ethernet to PHY/switch |
| eMMC | 8-bit data + CMD/CK/RST |

---

## CCLI mapping (preliminary)

| CCLI need | MT7986A capability | TG-524 action |
|-----------|-------------------|---------------|
| 4× Ethernet | HSGMII + MT7531 DSA typical | Confirm switch/PHY in TesPro DTS |
| 2× RS485 | UART + external transceiver | Map UART instance from **board DTS** |
| 2× DI + 2× DO | GPIO (free mux pins) | Vendor schematic — **not** in this PDF |
| HW NAT | On-SoC | OpenWrt `hnat` / MediaTek path |
| Secure Boot + TPM | SoC secure boot; TPM likely I2C/SPI | TesPro TPM wiring in BSP |
| GNSS / LTE | USB or PCIe modem | Separate from SoC pin table |

---

## Knowledge gaps

| Area | This doc | Still required |
|------|----------|----------------|
| Exact TG-524 SoC | MT7986A reference | TesPro written SKU (7981 vs 7986 vs 7988) |
| Board pin map | Pin mux table only | TesPro device tree + schematic (K2.2) |
| TRM / register map | Not in datasheet | NDA or vendor BSP |
| DI/DO / RS485 pin # | Not BPI-R3 specific | Hardware guide from TesPro |

---

## Risks

| ID | Description | Mitigation |
|----|-------------|------------|
| R-SOC-006 | Assume TG-524 = MT7986A from this PDF | Confirm CPU core count/speed with TesPro |
| R-SOC-007 | Use BPI-R3 pin mux on TG-524 | Use only after diff against vendor DTS |
| R-SOC-001 | No SDK | Request OpenWrt SDK with board DTS (K2.2) |

---

## Related RAG sources

| source_id | Role |
|-----------|------|
| `ccli-mt7986a-datasheet` | Full PDF (23 pages) |
| `ccli-soc-freeze` | MT798X programme freeze |
| `ccli-tg500-lab-platform` | TG-524 product platform |
| `ccli-tespro-tg500-ds` | TesPro gateway datasheet |
| Kernel `mediatek,mt7986-pinctrl.yaml` | Open pinmux binding (complement) |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-08-11 | Initial extract + RAG ingest |

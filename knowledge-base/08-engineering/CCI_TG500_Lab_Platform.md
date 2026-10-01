# CCI Platform Freeze — TesPro TG-500 / TR400 Series (FINAL PRODUCT)

**Document ID:** CCLI-HW-TG500-001  
**Revision:** 3.0  
**Date:** 2026-09-15  
**RAG source_id:** `ccli-tg500-lab-platform`  
**PDF sources:** `ccli-tespro-tg500-ds` · `ccli-tespro-tg424-response` · `ccli-tr400-user-manual` · `ccli-tr400-user-manual-extract`  
**I/O map:** `ccli-tr500-io-peripheral` — bench TG544 DIO + serial  
**Status:** **FROZEN — FINAL PRODUCT PLATFORM** · **TR400 ON BENCH (active DUT)**

---

## Freeze decision

| Field | Frozen value |
|-------|----------------|
| **Vendor** | TesPro Electronics Co., Ltd. — https://www.tespro.com |
| **Product family** | **TG-500 / TR-400 series** edge gateway |
| **Default SKU** | **TG-524** (4G) — order **TG-525** only if 5G RedCap required |
| **Lab DUT (received)** | **TesPro TR400** — TesproOS 1.0.0; same MT798X / OpenWrt line |
| **SKU alias** | **TG-424 Pro** / **TR-424** — confirm equivalence with TesPro (K1.6 **OPEN**) |
| **SoC / module** | MediaTek **MT798X** — **FROZEN FINAL SOC** — see `CCI_SOC_Freeze.md` |
| **OS** | **OpenWrt / TesproOS** — **FROZEN** — customer **C/C++** application firmware |
| **Secondary mock** | **None** — Pi 4 path **removed from scope** (2026-09-17) |
| **Role** | **Product CCI compute + comm box** — PF2 / 61850 / Modbus in **new application FW** |
| **CEI qualification** | **Not** from datasheet alone — Allegato O/T is **software + evidence** |
| **Supersedes** | Toradex Verdin **99991105 + 0063** and all alternate SoM paths (**archived**) |

**Development path:** **TG544 / TR500** is the **only** on-target DUT (`platform_tg500`). Cross-compile via OpenWrt **25.12.5** filogic SDK (`lab/tg544-openwrt/`) or on-device bootstrap.

Detail: `CCI_OpenWrt_Freeze.md` · `CCI_CCLI_Product_Spec_and_Roadmap.md` · `CCI_TR400_Extract.md`

---

## TR400 bench unit (as-built — photo + manual)

### Front panel (confirmed on received hardware)

| Connector | Labels | Linux / CCI (provisional) |
|-----------|--------|---------------------------|
| Ethernet | **WAN**, **LAN3**, **LAN2** (+ **LAN1** on unit — confirm) | Map via `ip link` → Eth_A / Eth_B / plant |
| Serial (7-pin) | **RXD, TXD, GND**, **A1, B1**, **A2, B2** | **`/dev/ttyS0`**, **`/dev/ttyS1`**, **`/dev/ttyS2`** |
| CAN (4-pin) | **H1, L1, H2, L2** | Out of CCI Phase 1 scope |
| I/O (10-pin) | **AO1/2, AOG, AI1/2, GND, DIO1–4** | **GD32** → `ubus dido_v2`: **DIO1** ch1 DO curtail, **DIO2** ch2 DI permissive |
| Power | **12–30 V** | C10 **HAVE** |
| USB | USB 3.0 | C8 commissioning |
| TF | MicroSD | Optional logging |

### Lab defaults (TesproOS)

| Item | Value |
|------|--------|
| Default IP | **192.168.0.1** |
| Login | `root` / **`000000`** — change immediately |
| DHCP pool | 192.168.0.100–200 |
| RS485-1 | **`/dev/ttyS1`** (A1/B1) |
| RS485-2 | **`/dev/ttyS2`** (A2/B2) |
| Config file | `apps/ccli/config/lab_tr400.yaml` |

### TR400 vs TG-524 datasheet deltas

| Item | TG-524 datasheet | TR400 (bench) |
|------|------------------|---------------|
| DI/DO | 2× DI + 2× DO | **4× DIO** + 2× AI + 2× AO |
| Ethernet | 1 WAN + 3 LAN | **1 WAN + 4 LAN** (Gigabit) |
| Power | 12–36 V | **12–30 V** (panel label) |

Treat **TR400 front panel** as authoritative for wiring; datasheet as family summary.

---

## Requirements

| ID | Requirement | TR400 / TG-500 mapping |
|----|-------------|------------------------|
| REQ-HW-001 | Product compute + comm box ≤ budget caps | TG-524 / TR400 within budget (confirm quote) |
| REQ-HW-002 | C/C++ on Linux | OpenWrt / TesproOS native C/C++ — **FROZEN** |
| REQ-HW-003 | ≥2 Ethernet (target 4 isolated) | **1× WAN + 4× LAN** on TR400 |
| REQ-HW-004 | 2× RS-485 plant serial | **A1/B1**, **A2/B2**; simultaneous |
| REQ-HW-005 | TPM + secure boot path | TPM **2.0** + Secure Boot — **verify on TR400** |
| REQ-HW-006 | 12–36 V DC input | **12–30 V** on TR400 panel — within C10 |
| REQ-SW-001 | No vendor DLMS/OPC as CCI product | Disable Wi-Fi/OPC/DLMS in product profile |
| REQ-NET-001 | No L2 bridge Eth_A ↔ Eth_B/plant | **Firmware policy** on TesproOS |

---

## Architecture

### Functional (product)
- PF2 / observability FSM (new C/C++)
- IEC 61850 MMS on Eth_A — **libiec61850**
- Modbus RTU/TCP on RS485 / plant LAN — **libmodbus**
- IEC 60870-5-104 on Eth_B — **lib60870**
- Factory / commissioning — USB / SSH / Web (out of cert path)

### Hardware (TG-524 family datasheet + TR400 bench)

| Item | TG-524 / TR400 class |
|------|----------------------|
| CPU | Cortex-A53 dual 1.6 GHz + HW NAT |
| RAM | 2 GB DDR4 |
| eMMC | 32 GB std |
| Serial | 1× RS232 + **2× RS485** |
| Field I/O | **TR400:** 4× DIO + AI/AO; **datasheet:** 2 DI + 2 DO |
| USB | USB 3.0 |
| Wi-Fi | Present — **disable** in CCI profile |
| Temp | −40 … +75 °C |
| Power | 12–36 V (12–30 V on TR400 label) |

### Network (port map — **OPEN K3.1**)

| CCI role | Provisional binding | Protocol |
|----------|---------------------|----------|
| Eth_A (DSO) | **LAN1** (confirm `eth*`) | IEC 61850 MMS |
| Eth_B (operator) | **LAN2** or **LAN3** | IEC 60870-5-104 |
| Plant | Remaining LAN or RS485 | Modbus TCP / RTU |
| WAN / LTE | **WAN** | Backup only — no bridge to Eth_A |
| Engineering PC | **LAN2** (current bench wire) | SSH / Web |

### Security
- TPM 2.0 — verify with `tpm2_*` on TR400
- Secure Boot — verify on receipt
- Wi-Fi disabled in product profile

---

## Interface matrix (TR400 product)

| Interface | Source | Destination | Protocol |
|-----------|--------|-------------|----------|
| WAN/LAN | TR400 | DSO / operator / plant | TCP/IP — segmented by FW |
| RS485-1 (A1/B1) | TR400 `/dev/ttyS1` | Energy analyzer / PC sim | Modbus RTU |
| RS485-2 (A2/B2) | TR400 `/dev/ttyS2` | Plant spare | Modbus RTU |
| RS232 | TR400 `/dev/ttyS0` | Service / debug | — |
| DIO1 | PF2 FSM | Curtailment relay | Digital out |
| DIO2 | Field | Permissive DI | Digital in |
| DIO3–4 | Spare | Interlocks / feedback | — |
| USB | Engineer PC | Commissioning | Out of cert path |

---

## BOM matrix

| Block | Function | Part | Status |
|-------|----------|------|--------|
| PROD-GW | Product compute + comm | **TesPro TG-524 / TR400** | **FROZEN — FINAL** |
| LAB-DUT | Active on-target DUT | **TesPro TR400** (received) | **ACTIVE** |
| DEV-MOCK | — | Raspberry Pi | **REMOVED** from scope |
| LAB-IO | Relay + USB-RS485 | COTS bench kit | **ACTIVE** |
| PROD-SW | Protocol stacks | libiec61850, libmodbus, lib60870 | BUILD |
| EXP-IO | Full AiLux C4 (5 DI / 3 DO) | Wave B / external module | **PARTIAL** |

---

## CCI control checklist (C1–C10 vs TR400)

| ID | Need | TR400 score | Notes |
|----|------|-------------|-------|
| C1 | ≥4 Ethernet domains | **Partial → Full** | 5 GE ports; map + firewall |
| C2 | 2× isolated RS485 | **Partial/Full** | A1/B1, A2/B2 wired for L2 |
| C3 | IEC 61850 on Eth_A | **Full (SW)** | You implement Allegato T |
| C4 | 5 DI / 3 DO field class | **Partial** | **4 DIO** on TR400; AiLux 5/3 needs expansion |
| C5 | GNSS | **Partial** | Modem GNSS optional |
| C6 | Secure boot + TPM | **Partial** | Verify on TR400 |
| C7 | OpenWrt + C/C++ | **Full** | TesproOS 1.0.0 |
| C8 | Commissioning | **Full** | USB / SSH / Web |
| C9 | DIN product form | **Partial** | Gateway enclosure |
| C10 | 12–36 V power | **Full** | 12–30 V panel |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| SKU naming | TR400 / TG-424 / TG-524 | Written TesPro confirm + label photo | **OPEN** |
| TR-400 manual | **INGESTED** 49 pp. | — | **HAVE** |
| Port map K3.1 | LAN2 wired to PC | `ip link` ↔ silkscreen table | **OPEN** |
| DIO → API map | **HAVE** — ubus ch1–4 | `ccli-tr500-io-peripheral` | **CLOSED** |
| DIO voltage class | Logic-level open-drain DO | AiLux 10–120 V DI — expansion for product | **OPEN** |
| OpenWrt SDK | Not in repo | Cross-compile for MT798X | **MISSING** |
| CEI qualification | Not claimed | DSO demo + evidence | **NEW WORK** |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-TG-001 | Vendor 61850 ≠ Allegato T | False cert confidence | Own libiec61850 + ICD |
| R-TG-002 | 4 DIO ≠ AiLux 5 DI + 3 DO | Incomplete C4 demo | Wave B + document expansion |
| R-TG-003 | OpenWrt SDK gaps | Delayed cross-compile | On-device bootstrap; escalate SDK |
| R-TG-004 | SKU mismatch TR400 vs TG-524 | Wrong assumptions | Label photo + TesPro confirm |
| R-TG-005 | Wi-Fi / OPC / DLMS enabled | Scope creep | Product profile disables extras |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-HW-003 | `ip link` + silkscreen photo | Port map documented |
| REQ-HW-004 | Modbus RTU on A1/B1 (`ttyS1`) | pymodbus slave on PC responds |
| REQ-IO-001 | DIO1 → relay | Click on curtail command |
| C3 | libiec61850 on Eth_A LAN | Client reads demo ICD |
| REQ-NET-001 | Eth_A → Eth_B traffic | **Blocked** by firewall |
| REQ-HW-005 | TPM on TR400 | `tpm2_getcap` succeeds |

---

## Keywords

`TG-500`, `TG-524`, `TR400`, `TR-400`, `TesPro`, `MT798X`, `TesproOS`, `OpenWrt`, `AiLux`, `CCLI`, `ccli-tg500-lab-platform`

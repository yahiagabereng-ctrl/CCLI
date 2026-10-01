# CCI SoC Freeze — MediaTek MT798X (TesPro TG-524)

**Document ID:** CCLI-SOC-001  
**Revision:** 2.1  
**Date:** 2026-09-15  
**RAG source_id:** `ccli-soc-freeze`  
**Status:** **FROZEN — FINAL PRODUCT SOC**  
**Companion:** `CCI_TG500_Lab_Platform.md` (`ccli-tg500-lab-platform`) · `CCI_OpenWrt_Freeze.md` (`ccli-openwrt-freeze`)

---

## Freeze decision

| Field | Frozen value |
|-------|----------------|
| **SoC** | **MediaTek MT798X** (Wi-Fi 7 BE5000 class router platform) |
| **CPU** | **ARM Cortex-A53 dual @ 1.6 GHz** + hardware NAT offload |
| **Product platform** | TesPro **TG-524 / TR400** (TG-500 series gateway) |
| **Active lab DUT** | **TG544 / TR500** — TesproOS 32-2.1.0 / OpenWrt **25.12** |
| **OS** | **OpenWrt 25.12** — **FROZEN** — C/C++ application firmware |
| **Scope** | **Final CEI 0-16 CCI compute** — lab and product on **same TesPro silicon** |
| **Secondary mock** | **None** — Raspberry Pi removed from scope (2026-09-17) |
| **Removed from active path** | NXP i.MX 8M (Plus/Mini/DQLQ), Toradex Verdin, Variscite DART, Compulab SoM/gateway study |

**There is no planned second product SoC.** Alternate SoM/carrier studies live in `_archive/som-study/` for historical reference only.

**Still open (not silicon):** mechanical enclosure (C9), full 5 DI / 3 DO plant class on integrated 2+2 I/O (Wave B fixture + external I/O expansion if required), EMC/CE evidence pack, OpenWrt SDK from TesPro.

---

## Requirements

| ID | Requirement | MT798X / TG-524 |
|----|-------------|-----------------|
| REQ-SOC-001 | Product compute for 61850 + Modbus + PF2 | **FROZEN** — MT798X on TG-524 |
| REQ-SOC-002 | OpenWrt cross-compile path | Vendor SDK — **REQUEST from TesPro** |
| REQ-SOC-003 | Secure boot + TPM | TPM 2.0 + Secure Boot (brochure — verify on receipt) |
| REQ-SOC-004 | Industrial temp | −40 … +75 °C (datasheet) |
| REQ-SOC-005 | No alternate SoC in active corpus | **FROZEN** — archive only |
| REQ-SOC-006 | Lab and field share one silicon line | **FROZEN** — Pi L1; **TR400** is active product DUT |
| REQ-SOC-007 | OpenWrt is the only product OS | **FROZEN** — see `CCI_OpenWrt_Freeze.md` |

---

## Architecture

| Layer | Choice |
|-------|--------|
| **Product compute** | TG-524 integrated gateway (not discrete SoM + custom carrier) |
| **Ethernet** | 1× WAN + **4× LAN** on TR400 (map to Eth_A / Eth_B / plant) |
| **Serial** | 2× RS485 (`ttyS1`/`ttyS2`) + 1× RS232 (`ttyS0`) |
| **I/O** | **TR400:** 4× DIO + AI/AO; datasheet 2 DI + 2 DO; Wave B / expansion for AiLux C4 |
| **Security** | TPM 2.0, Secure Boot — **not** NXP HAB/CAAM path |

---

## Interface matrix

| Interface | TG-524 | Protocol |
|-----------|--------|----------|
| LAN1 | Eth_A (DSO) | IEC 61850 MMS |
| LAN2 | Eth_B (operator) | IEC 60870-5-104 / 61850 client |
| LAN3 / RS485 | Plant | Modbus TCP / RTU |
| WAN / LTE | Backup | No bridge to Eth_A |
| DI/DO | 2+2 | PF2 + Wave B / expansion as needed |

---

## BOM matrix

| Block | Function | Part | Status |
|-------|----------|------|--------|
| SOC-PROD | Product compute | MediaTek MT798X in **TesPro TG-524** | **FROZEN — FINAL** |
| LAB-DUT | Active on-target DUT | **TesPro TR400** | **ON BENCH** |
| MOCK | — | Raspberry Pi | **REMOVED** from scope |
| SDK | Cross-compile | TesPro OpenWrt SDK | **MISSING** — request with PO |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| OpenWrt SDK | None | Toolchain + sysroot + **board DTS** | **Request TesPro** |
| MT798X pinmux (K1.7) | MT7986A datasheet extract | TG-524 board pin map + SoC SKU confirm | **PART** — see `ccli-mt7986a-datasheet-extract` |
| SKU confirm | TG-424 vs TG-524 alias | PO line item | Confirm with vendor |
| Secure Boot detail | Brochure only | SDK + fuse policy doc | **PARTIAL** |
| Full C4 I/O count | 2 DI / 2 DO on box | 5 DI / 3 DO for PF2 demo | Wave B fixture / expansion |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-SOC-001 | SDK delay | No on-target binary | Pi mock + native build |
| R-SOC-002 | Vendor 61850 ≠ Allegato T | False cert confidence | Own libiec61850 + ICD |
| R-SOC-003 | Alternate SoC docs in RAG | Wrong assistant answers | Removed from ingest; archive only |
| R-SOC-004 | 2+2 I/O treated as full C4 | Incomplete PF2 I/O in cabinet | Document expansion path; Wave B on Pi |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-SOC-001 | `uname -m` + SoC id on TG-524 | Cortex-A53 / MT798X confirmed |
| REQ-SOC-002 | Cross-compile hello world | Runs on device |
| REQ-SOC-004 | Thermal soak at 50 °C cabinet | No throttling in product profile |
| REQ-SOC-006 | Same binary/test vectors Pi → TG-524 | Identical pass/fail on L1 vectors |

---

## Keywords

`MT798X`, `MediaTek`, `Cortex-A53`, `SoC freeze`, `TG-524`, `TG-500`, `OpenWrt`, `ccli-soc-freeze`, `final product soc`, `product compute`

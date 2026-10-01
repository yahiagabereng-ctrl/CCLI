# TesPro Supplier Correspondence — TG-424 Pro / OpenWrt (HiTEKS / Shitek)

**Document ID:** CCLI-VENDOR-TESPRO-001  
**Revision:** 1.0  
**Date:** 2026-08-10  
**RAG source_id:** `ccli-tespro-supplier-correspondence`  
**Source:** Email thread Aug 2026 — Ms. Hong Huang (TesPro) ↔ Nicola Canella / Marco Ragazzo (Shitek)  
**Programme link:** HiTEKS CCLI / PF2 gateway procurement (same hardware path as CCLI TG-524 freeze)

---

## Summary

TesPro confirmed **OpenWrt baseline 24.10.5** (supersedes earlier reply **23.05**), **TG-424 Pro** with **TPM 2.0**, and hardware matching the frozen TG-500 programme. **Cross-compile SDK** was **not** explicitly offered — only that OpenWrt supports installing development tools. **IEC 61850** is a **manufacturer declaration**, not independent lab / CEI qualification.

---

## Confirmed by TesPro

| Topic | Supplier statement | Date / note |
|-------|-------------------|-------------|
| **Product** | **TG-424 Pro** with TPM 2.0 | PI quoted Jul–Aug 2026 |
| **OpenWrt version** | Baseline **24.10.5** — R&D double-confirmed | 2026-08-03 (replaces 23.05 reply same day) |
| **OS** | **OpenWrt**; customers may install common dev tools via opkg | 2026-08-03 |
| **CPU / RAM / storage** | Dual-core ~1.6 GHz class, 2 GB RAM, ≥32 GB free eMMC for custom app | Aligned with TG-500 datasheet |
| **Ethernet** | **1× WAN + 3× LAN** (isolated) — 4 ports total; mainboard supports 4 LAN; enclosure may need redesign | Samples with 4 LAN targeted ship ~2026-09-01 |
| **Cellular** | 4G LTE (Cat1 option); modem + **antennas included** | Confirmed |
| **GNSS** | GPS/modem-integrated; **antenna included** | Confirmed |
| **Serial** | 2× RS485 | Confirmed |
| **DI/DO** | **2× DI + 2× DO** | Confirmed (Marco summary said "2 GPIO" — treat as DI/DO pair) |
| **USB** | 1× USB 3.0 (USB-A agreed) | Not 2× USB without OEM |
| **Power** | 12 V DC | Confirmed |
| **TPM** | **TPM 2.0** — mandatory for Shitek; TesPro agreed after review | Was initially software-only offer |
| **Modbus libs** | **libmodbus** LGPLv2.1+; **libubus/libubox** LGPLv2.1 — **commercial use OK** | 2026-08-03 |
| **IEC 61850** | Gateway series "compliant"; manufacturer doc + screenshot of built-in function | **Not** third-party lab test — Marco confirmed understanding |
| **CE / IEC** | Manufacturer **Declaration of Conformity** offered | Nicola requested **RED/EMC/IEC test reports** — **pending** |

---

## Sample order status (Aug 2026 thread)

| Item | Detail |
|------|--------|
| Qty | 2 samples, 4-LAN version (1 WAN + 3 LAN) |
| Price | €350/each + €58 shipping (PI in thread) |
| Ship | Factory ship ~**2026-09-01** if order confirmed; ~5–6 days to Italy |
| Enclosure | May ship open frame or without extra port holes if faster; final enclosure TBD |

---

## Open items — still request from TesPro

| ID | Item | Why still needed | Tree node |
|----|------|------------------|-----------|
| V-001 | **OpenWrt 24.10.5 cross-SDK** (toolchain + sysroot + sample app) | "Install dev tools on device" ≠ host cross-compile for `apps/ccli` | K2.2 |
| V-002 | Written **TG-424 Pro ≡ TG-524** SKU mapping | Corpus uses TG-524; PO may say TG-424 Pro | K1.6 |
| V-003 | **Port map** — Linux names for WAN, LAN1–3, RS485, DI/DO, TPM | K3.1 labelling — **GD32 DIO sheet HAVE** (`Schematic-GPIO.pdf`); ubus ch↔GPIO map still open | K3.1 |
| V-004 | **RED / EMC / IEC test reports** (not DoC alone) | Nicola follow-up | K7.6 |
| V-005 | Is vendor **IEC 61850** a separate daemon or hardware capability only? | **PARTIAL** — `iec61850-mmsd` + web UI = **MMS client/collector** (port 102 → user-uploaded ICD); **not** DSO MMS server / no SCD for TG544. **`ccli` + `lab_tg544_eth_a.cid`** owns Eth_A :3782. See `ccli-mms-server-vs-tespro-collector`. Coexistence lab **OPEN**. | K4.1, R-TG-001 |
| V-006 | **Secure Boot** + image update documentation | K2.7, K6.1 | K2.7 |
| V-007 | RS485 / LAN **galvanic isolation** proof | K3.2, K5.1 — **DIO: TLP291 + GND_GE PARTIAL** on GPIO schematic; RS485 still open | K3.2 |

---

## Requirements traceability

| ID | Requirement | Supplier evidence | Status |
|----|-------------|-------------------|--------|
| REQ-VEND-001 | OpenWrt OS on gateway | Email + datasheet | **CONFIRMED** |
| REQ-VEND-002 | OpenWrt **24.10.5** baseline | R&D email 2026-08-03 | **CONFIRMED** |
| REQ-VEND-003 | TPM 2.0 hardware | TG-424 Pro spec | **CONFIRMED** |
| REQ-VEND-004 | 4 Ethernet domains (1+3) | Email thread | **CONFIRMED** (samples) |
| REQ-VEND-005 | Cross-compile SDK for C/C++ app | Not in thread | **MISSING** |
| REQ-VEND-006 | CEI / IEC 61850 certification | Manufacturer claim only | **NOT MET** — own stack (`apps/ccli`) |
| REQ-VEND-008 | Vendor 61850 stack documented | `IEC61850-User-Manual.docx` ingested | **HAVE** — collector role only |
| REQ-VEND-007 | libmodbus commercial use | LGPLv2.1+ stated | **CONFIRMED** |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-VEND-001 | OpenWrt version corrected 23.05 → 24.10.5 | Toolchain mismatch if wrong baseline used | Freeze **24.10.5** in corpus; verify on receipt |
| R-VEND-002 | Dev tools on device ≠ cross-SDK | Blocked L3 until K2.2 | Explicit SDK request in next email |
| R-VEND-003 | Vendor IEC 61850 ≠ CEI Allegato T | False cert confidence | R-TG-001 — libiec61850 + own ICD |
| R-VEND-004 | DoC without test reports | EMC/RED gap at product gate | Nicola's test-report request |
| R-VEND-005 | Sample enclosure ≠ final mechanical | Port map ambiguity | Label ports on receipt; confirm final SKU |

---

## Verification

| Check | Pass criteria |
|-------|---------------|
| OpenWrt version on sample | `/etc/openwrt_release` shows **24.10.5** |
| 4 LAN present | `ip link` shows WAN + 3 LAN interfaces labelled |
| TPM | `tpm2_getcap properties-fixed` succeeds |
| libmodbus | `opkg list-installed \| grep modbus` or build against sysroot |

---

## Draft email — V-005 SCD/CID clarification

**To:** hong.huang@tespro-china.com  
**Subject:** TG544 IEC 61850 — MMS server SCD/CID vs client collector

> We are implementing the **CEI 0-16 DSO MMS server** in our application **`ccli`** on **Eth_A port 3782** with our own **CID** (`CCI016_01`).  
> Your **IEC61850-User-Manual** and APK packages (`iec61850-mmsd`, `iec61850-proto-tespro-combined`) describe **polling remote IEDs on port 102** with **user-uploaded ICD/CID**.
>
> Please confirm:
> 1. You do **not** provide an **SCD/CID** for TG544 as a **DSO-facing MMS server**.
> 2. Vendor 61850 is **client/collector only** + optional MQTT/TCP northbound.
> 3. **`ccli` and `iec61850-mmsd` may coexist** on TG544 (CPU/RAM, port binding, supported config).
>
> Reference: `knowledge-base/08-engineering/CCI_MMS_Server_vs_TesPro_Collector.md`

---

## Contacts

| Role | Name | Email |
|------|------|-------|
| TesPro sales | Ms. Hong Huang | hong.huang@tespro-china.com · hong.huang@tespro.com |
| Shitek CEO | Nicola Canella | nicola.canella@shitek.it |
| Shitek IT / CCLI PM | Marco Ragazzo | marco.ragazzo@shitek.it |

---

## Keywords

`TesPro`, `TG-424 Pro`, `TG-524`, `OpenWrt 24.10.5`, `Hong Huang`, `supplier`, `libmodbus`, `TPM`, `IEC 61850`, `manufacturer declaration`, `ccli-tespro-supplier-correspondence`

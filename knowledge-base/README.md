# CCI Knowledge Base Brief

**Document ID:** CCLI-KB-001  
**Revision:** 2.1  
**Date:** 2026-10-14  
**RAG source_id:** `ccli-knowledge-base-brief`  
**Purpose:** Knowledge tree for the Original CCI project — **TesPro TG-524 / MT798X final product platform**.

**On-disk root:** `c:\Yahia\projects\CCLI\knowledge-base\`  
**Telematry mirror:** `documents/projects/ccli/`  
**Master index:** [`DOWNLOADS.md`](DOWNLOADS.md)  
**Lab + product Cursor workspace:** [`../lab/WORKSPACE_LAB_AND_PRODUCT.md`](../lab/WORKSPACE_LAB_AND_PRODUCT.md) · `CCLI-Lab-And-Product.code-workspace`

---

## 1. Knowledge Base tree

```
Knowledge Base
├── 06-application/          Application design + SK0146 AT style
├── 07-protocols/            CEI, regs, GitHub protocol libraries
└── 08-engineering/          Architecture, BOM, validation, TG-500 freeze
```

---

## 2. Branch briefs

### 2.1 Application source (`06-application/`)

| Component | Language | Status |
|-----------|----------|--------|
| IEC 61850 MMS (62351 TLS/ACSE) | C/C++ | **PART** — TG544 lab |
| Modbus RTU/TCP master | C/C++ | **PART** |
| PF2 curtailment + DI/DO | C/C++ | **PART** |
| Commissioning / web | Python | Out of cert path |
| SK0146 AT syntax reference | docs | HAVE — `sk0146-fw-at/` |

**Platform:** `platform_tg500` only (TesPro TG544 / TG-524 OpenWrt 25.12)

**RAG ids:** `ccli-app-iec61850`, `ccli-app-modbus`, `ccli-app-pf2`, `fw-sk0146-*`

---

### 2.2 Protocol documentation (`07-protocols/`)

| Item | RAG status |
|------|------------|
| CEI 0-16 Allegato O/T | INGESTED |
| Module regs classification | INGESTED |
| AiLux manual (reference) | INGESTED |
| EU RED / CRA | INGESTED |
| libiec61850 / lib60870 harvest | ON DISK — `07-protocols/github-refs/` |
| ARERA 540/2021 | MISSING |
| IEC 62443 / 62351 PDFs | MISSING |

**RAG id:** `ccli-github-protocol-libs`

---

### 2.3 Engineering knowledge (`08-engineering/`)

| Topic | RAG status |
|-------|------------|
| **OpenWrt freeze (FROZEN)** | ON DISK — `ccli-openwrt-freeze` |
| **Product SW freeze (lab-first)** | ON DISK — `ccli-product-sw-freeze` |
| **TG-500 knowledge tree (K0–K8)** | ON DISK — `ccli-tg500-knowledge-tree` |
| **TG-500 platform (FROZEN — FINAL)** | ON DISK — `ccli-tg500-lab-platform` |
| **SoC freeze (MT798X — FINAL)** | ON DISK — `ccli-soc-freeze` |
| Architecture framework | INGESTED |
| Validation + mocking BOM | INGESTED |
| 3-month Italy plan | INGESTED |
| Vendor selection (historical SoM study) | ARCHIVED — `_archive/som-study/`, not ingested |
| OpenWrt SDK / cross-compile runbook | **PARTIAL** — `lab/tg544-openwrt/` |

---

## 3. Product hardware (frozen)

| Role | Device |
|------|--------|
| **SoC (FROZEN — FINAL)** | MediaTek **MT798X** in TesPro **TG-524** |
| **Product DUT** | TesPro **TG-524** (OpenWrt, MT798X) |
| **Lab DUT** | **TG544 / TR500** (`platform_tg500`) — **active** |
| **OS** | **OpenWrt 25.12 (FROZEN)** on TesPro MT798X |

Historical SoM vendor study: `_archive/som-study/` — **not** in active ingest.

---

## 4. Folder layout

```
knowledge-base/
├── README.md
├── DOWNLOADS.md
├── 06-application/
│   ├── README.md
│   └── sk0146-fw-at/
├── 07-protocols/
│   ├── CCI_GitHub_Protocol_Libraries.md
│   ├── github-refs/libiec61850.md
│   └── github-refs/lib60870.md
└── 08-engineering/
    ├── CCI_TG500_Lab_Platform.md
    ├── TG-500_Series_Datasheet.pdf
    └── …
```

---

## 5. To do

1. **Lab-first:** close remaining gates · deploy with TLS yaml · record under `lab/evidence/`  
2. **Product:** first promote to `releases/product-cyber/<semver>-rN>/` per `ccli-product-sw-freeze`  
3. RAG refresh (product deploy P0): `scripts/ingest-product-deploy-rag.ps1` · full: `scripts/ingest-next-phases.ps1`  
4. TesPro vendor SDK parity (optional vs upstream 25.12.5)  
5. Procure **62351-100-4** PDF for formal MMS cyber cases  

---

## 6. Keywords

`openwrt`, `tg-524`, `tespro`, `libiec61850`, `modbus`, `pf2`, `lab-first`, `product-cyber`, `ccli-kb`

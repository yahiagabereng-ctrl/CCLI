# CCI Knowledge Base Brief

**Document ID:** CCLI-KB-001  
**Revision:** 2.0  
**Date:** 2026-08-10  
**RAG source_id:** `ccli-knowledge-base-brief`  
**Purpose:** Knowledge tree for the Original CCI project — **OpenWrt / TG-524 lab platform**.

**On-disk root:** `c:\Yahia\projects\CCLI\knowledge-base\`  
**Telematry mirror:** `documents/projects/ccli/`  
**Master index:** [`DOWNLOADS.md`](DOWNLOADS.md)

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
| IEC 61850 MMS/GOOSE | C/C++ | NOT STARTED |
| Modbus RTU/TCP master | C/C++ | NOT STARTED |
| PF2 curtailment + DI/DO | C/C++ | NOT STARTED |
| Commissioning / web | Python | Out of cert path |
| SK0146 AT syntax reference | docs | HAVE — `sk0146-fw-at/` |

**Platforms:** `platform_pi` (mock) → `platform_tg500` (TesPro TG-524 OpenWrt)

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
| **TG-500 lab platform (FROZEN)** | ON DISK — `ccli-tg500-lab-platform` |
| Architecture framework | INGESTED |
| Validation + mocking BOM | INGESTED |
| 3-month Italy plan | INGESTED |
| Vendor selection (historical SoM study) | INGESTED — **not lab path** |
| Pin budget / carrier | MISSING |

---

## 3. Lab hardware (frozen)

| Role | Device |
|------|--------|
| **SoC (FROZEN)** | MediaTek **MT798X** in TesPro **TG-524** |
| **Lab DUT** | TesPro **TG-524** (OpenWrt, MT798X) |
| **Pre-hardware** | Raspberry Pi |
| **OS** | OpenWrt + C/C++ apps |

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

1. Ingest TG-500: `ingest-tg500-platform.ps1`  
2. OpenWrt SDK / cross-compile runbook for TG-524  
3. ARERA + cyber standards PDFs  
4. Application tree `apps/ccli/` with `platform_pi`  
5. MZ commercial quote for libiec61850  

---

## 6. Keywords

`openwrt`, `tg-524`, `tespro`, `libiec61850`, `modbus`, `pf2`, `raspberry pi`, `ccli-kb`

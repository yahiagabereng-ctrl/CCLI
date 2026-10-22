# CCI Application Software Structure

**Document ID:** CCLI-APP-STRUCT-001  
**Revision:** 1.3  
**Date:** 2026-10-14  
**RAG source_id:** `ccli-app-structure`  
**On-disk roots:**  
- **Lab / conformance:** `apps/ccli/` (`services/ccli_main.cpp`)  
- **Product (SK0153):** `product/apps/ccli/` (`program/main.c`, `ccli_program`) — see [`product/README.md`](../../product/README.md)  

**Platform:** `platform_tg500` only (**TG544 / TR500 / TG-524**). Raspberry Pi / `platform_pi` **ARCHIVED — out of scope** (2026-09-17).  
**Parents:** `ccli-openwrt-freeze` · `ccli-tg500-lab-platform` · `ccli-product-sw-freeze`

---

## Purpose

Canonical layout for CCI user-space firmware on **OpenWrt 25.12 / TesproOS (frozen)**. **TG544 / TR500** is the lab DUT; **TG-524 / TR400** is the field product line — same `platform_tg500` HAL.

**Lab-first:** develop and prove on TG544 (`lab/` configs, `lab/evidence/`) in **`apps/ccli/`**. **Product line:** SK0153-style modules under **`product/apps/ccli/program/`**; promote pinned `.apk` + cyber yaml + ICD under `releases/product-cyber/` per `ccli-product-sw-freeze`. Shared **`third_party`** — junction from product tree to lab (`scripts/link-product-third-party.ps1`).

**Not in scope:** SK0146 DLMS gas-meter firmware (reference AT style only in `tools/at_shell/`).

---

## Requirements mapping

| Req / tree node | Directory |
|-----------------|-----------|
| K2.1 On-target / SDK build | `platform/platform_tg500/` · `lab/tg544-openwrt/` |
| K3.1 port map | `config/lab_tr400*.yaml` (lab) · product yaml in release bundle |
| K3.3 no Eth_A↔Eth_B bridge | `core/policy/` |
| K4.1 MMS | `adapters/iec61850_mms/` |
| K4.3 Allegato T ICD | `config/icd/` |
| K4.4 Modbus | `adapters/modbus_master/` |
| K4.5 analyzer map | `config/modbus/` · `config/plant/` |
| K4.6 IEC 104 | `adapters/iec60870_104/` |
| K4.7 PF2 FSM | `core/pf2/` |
| K5 DI/DO | `adapters/io/` + HAL gpio |
| K8.4 test vectors | `test_vectors/` |
| Product promote | `releases/product-cyber/` |

---

## Directory tree

```text
apps/ccli/
├── CMakeLists.txt                 # top-level; BUILD_PLATFORM=tg500
├── VERSION                        # semver-rN — sync before lab/product deploy
├── cmake/
│   └── platform.cmake
├── config/
│   ├── lab_tr400.yaml             # TG544 default bindings
│   ├── lab_tr400_phase4_eth_b.yaml # Lab + product cyber template (TLS :3782)
│   ├── lab_tr400_cleartext_tsp.yaml  # LAB ONLY — TSP :102 (not product)
│   ├── icd/                       # Allegato T CID/CID/.cfg (K4.3)
│   ├── modbus/                    # register maps
│   └── plant/                     # field plant profiles (product-oriented)
├── core/                          # K4.7 — portable
│   ├── pf2/
│   ├── measurement/
│   ├── event/
│   └── policy/                    # REQ-NET-LAB-001
├── adapters/
│   ├── modbus_master/
│   ├── iec61850_mms/              # Eth_A MMS + 62351 TLS/ACSE
│   ├── iec60870_104/              # Eth_B
│   └── io/
├── platform/
│   ├── include/cci/hal/
│   └── platform_tg500/
├── services/
│   ├── ccli_main.cpp
│   ├── svc_pf2.cpp
│   ├── svc_modbus.cpp
│   └── svc_mms.cpp
├── test_vectors/                  # K8.4 — host + on-target TG544
├── tests/
│   ├── unit/
│   └── integration/               # against running TG544 (not Pi)
├── tools/                         # out of cert path
└── third_party/

releases/product-cyber/            # promoted product bundles (see README)
lab/tg544-openwrt/                 # SDK build · deploy-manifests/
```

---

## Runtime (target)

```text
ccli_main (supervisor)
├── svc_modbus   → MeasurementStore
├── svc_pf2      → Pf2State, DoIntent, EventRing
├── svc_mms      → Eth_A (libiec61850, :3782 product)
├── svc_io       → HAL gpio
└── svc_104      → Eth_B (when enabled in yaml)
```

Shared stores: `MeasurementStore`, `Pf2State`, `EventRing` — under `core/`.

---

## Build

| Target | Command | Role |
|--------|---------|------|
| **Product / lab DUT** | `lab/tg544-openwrt/build-ccli-ipk-sdk.sh` → `.apk` | **Primary** |
| Host unit tests | `cmake -B build -DBUILD_PLATFORM=tg500` (or host stub) | CI / FSM |
| `platform_pi` | — | **Removed** — do not use for programme gates |

Deploy runbook: `lab/tg544-openwrt/DEPLOY_CCLI.md` · manifests: `lab/tg544-openwrt/deploy-manifests/`.

---

## Config profiles

| Profile | Example yaml | MMS | Use |
|---------|--------------|-----|-----|
| Lab regulation + TLS | `lab_tr400_phase1_regulation.yaml` | :3782 | PF2 + cyber dev |
| Lab phase 4 + OA | `lab_tr400_phase4_eth_b.yaml` | :3782 | **Product cyber template** |
| Lab TSP cleartext | `lab_tr400_cleartext_tsp.yaml` | :102 | **Bench only** — exclude from product bundle |
| Product ship | `releases/.../config/product_tr400.yaml` | :3782 | Field TG-524 |

On DUT, active file is **`/etc/ccli/lab.yaml`** until K2.5 UCI/product path exists.

---

## Verification

| Check | Pass criteria |
|-------|---------------|
| TG544 `.apk` install | `ccli --version` matches `VERSION` |
| L1 vectors | `test_vectors/l1_pf2_sequence.yaml` → unit PF2 tests |
| Product gate | No cleartext MMS in promoted bundle · `ccli-product-sw-freeze` checklist |
| Portability | Same `core/` links for host tests and `tg500` target |

---

## Keywords

`apps/ccli`, `platform_tg500`, `lab-first`, `releases/product-cyber`, `PF2`, `libiec61850`, `62351`, `application structure`, `ccli-app-structure`

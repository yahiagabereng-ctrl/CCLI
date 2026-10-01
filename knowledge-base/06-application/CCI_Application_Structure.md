# CCI Application Software Structure

**Document ID:** CCLI-APP-STRUCT-001  
**Revision:** 1.2  
**Date:** 2026-09-15  
**RAG source_id:** `ccli-app-structure`  
**On-disk root:** `apps/ccli/`  
**Platform:** `platform_tg500` only (**TG544 / TR500 / TG-524**). Raspberry Pi / `platform_pi` **out of scope** (2026-09-17).

---

## Purpose

Canonical layout for CCI user-space firmware on **OpenWrt 25.12 / TesproOS (frozen)**. **TG544 / TR500** is the lab DUT via `platform_tg500`; all validation runs on TesPro hardware.

**Not in scope:** SK0146 DLMS gas-meter firmware (reference AT style only in `tools/at_shell/`).

---

## Requirements mapping

| Req / tree node | Directory |
|-----------------|-----------|
| K2.1 On-target / SDK build | `platform/platform_tg500/` · `lab/tg544-openwrt/` |
| K3.1 port map | `config/lab_tr400.yaml` |
| K3.3 no Eth_A↔Eth_B bridge | `core/policy/` |
| K4.1 MMS | `adapters/iec61850_mms/` |
| K4.3 Allegato T ICD | `config/icd/` |
| K4.4 Modbus | `adapters/modbus_master/` |
| K4.5 analyzer map | `config/modbus/` |
| K4.6 IEC 104 | `adapters/iec60870_104/` |
| K4.7 PF2 FSM | `core/pf2/` |
| K5 DI/DO | `adapters/io/` + HAL gpio |
| K8.4 test vectors | `test_vectors/` |

---

## Directory tree

```text
apps/ccli/
├── CMakeLists.txt                 # top-level; BUILD_PLATFORM=tg500
├── cmake/
│   └── platform.cmake
├── config/
│   ├── lab_tr400.yaml             # TG544 DIO / serial / PF2 bindings (active DUT)
│   ├── icd/                       # Allegato T data model (K4.3)
│   └── modbus/                    # register map abstraction (K4.5 placeholder)
├── core/                          # K4.7 — portable
│   ├── pf2/                       # curtailment FSM, hysteresis, safe state
│   ├── measurement/               # P, Q, PF, quality, scaling
│   ├── event/                     # timestamped log + alarm lifecycle
│   └── policy/                    # REQ-NET-LAB-001: no bridge rules (logic side)
├── adapters/                      # K4.1, K4.4, K4.6, K5
│   ├── modbus_master/             # libmodbus wrapper
│   ├── iec61850_mms/              # libiec61850 server (Eth_A)
│   ├── iec60870_104/              # lib60870 client/server stub (Eth_B)
│   └── io/                        # DI/DO intent → HAL
├── platform/
│   ├── include/cci/hal/           # serial, eth_bind, gpio, time, persist
│   └── platform_tg500/            # TesPro HAL — ubus DIO + ttyS1/S2
├── services/                      # one process per domain (start simple)
│   ├── ccli_main.cpp              # orchestrator / supervisor
│   ├── svc_pf2.cpp
│   ├── svc_modbus.cpp
│   └── svc_mms.cpp
├── test_vectors/                  # K8.4 — same files for Pi and TG-524
│   ├── l1_pf2_sequence.yaml       # L1-P-01..04, PF2 ramp from validation doc
│   ├── l1_comm_faults.yaml        # L1-C-01..03
│   └── README.md                  # maps vector ID → REQ-* / test ID
├── tests/
│   ├── unit/                      # core FSM without hardware
│   └── integration/               # pytest or CTest against running Pi
├── tools/                         # out of cert path
│   ├── commission/                # Python local web / SSH helpers (K4.8 style)
│   └── at_shell/                  # optional SK0146-syntax factory shell
└── third_party/                   # libiec61850, lib60870, libmodbus (submodules)
```

---

## Runtime (target)

```text
ccli_main (supervisor)
├── svc_modbus   → MeasurementStore
├── svc_pf2      → Pf2State, DoIntent, EventRing
├── svc_mms      → Eth_A (libiec61850)
├── svc_io       → HAL gpio (Wave B)
└── (later) svc_104 → Eth_B
```

Shared stores: `MeasurementStore`, `Pf2State`, `EventRing` — defined under `core/`.

---

## Build

| Platform | Command |
|----------|---------|
| Pi | `cmake -B build -DBUILD_PLATFORM=pi && cmake --build build` |
| TG-524 | `cmake -B build -DBUILD_PLATFORM=tg500` (requires OpenWrt SDK toolchain file) |

---

## Verification

| Check | Pass criteria |
|-------|---------------|
| Pi scaffold build | `ccli` binary links and `--version` runs |
| L1 vectors | `test_vectors/l1_pf2_sequence.yaml` drives unit PF2 tests |
| Portability | Same `core/` objects link for `pi` and `tg500` targets |

---

## Keywords

`apps/ccli`, `platform_pi`, `platform_tg500`, `PF2`, `libiec61850`, `libmodbus`, `application structure`, `ccli-app-structure`

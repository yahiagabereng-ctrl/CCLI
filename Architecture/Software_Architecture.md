# Software Architecture — CCLI

**Document ID:** CCLI-ARCH-SW-001  
**Revision:** 1.0  
**Date:** 2026-09-18  
**Diagram source:** [`Software_Architecture.drawio`](Software_Architecture.drawio)  
**Companions:** [`CCI_SW_Architecture_vs_Annex_Validation.md`](CCI_SW_Architecture_vs_Annex_Validation.md) · [`CCI_Runtime_Dataflow_Architecture.md`](CCI_Runtime_Dataflow_Architecture.md) · [`lab/PHASE1_TRACEABILITY.md`](../lab/PHASE1_TRACEABILITY.md) · [`lab/PHASE1.md`](../lab/PHASE1.md) · [`TR400_Lab_Platform.drawio`](TR400_Lab_Platform.drawio)

**Annex validation:** Use [`CCI_SW_Architecture_vs_Annex_Validation.md`](CCI_SW_Architecture_vs_Annex_Validation.md) as SoT for CEI O/T/M ↔ L0–L4. Draw.io colours on pages 1/4 (rev 1.0) may still show MMS/104 as stubs — Runtime DF + Phase checklist override for IMPL status.

Open the `.drawio` file in [diagrams.net](https://app.diagrams.net) or the Draw.io extension in VS Code/Cursor.

---

## What the diagram file contains

| Page | Name | Purpose |
|------|------|---------|
| **1** | `1-Layer-Model` | Vertical stack L0–L4: Platform HAL → Core → Adapters → Services → `ccli_main`; colour = Phase 1 active vs stub |
| **2** | `2-Phase1-Runtime` | End-to-end data flow: PC Modbus slave → `/dev/ttyS2` → PF2 FSM → permissive gate → DIO1/DIO2 via ubus |
| **3** | `3-CMake-Build` | Static libraries, link order, OpenWrt `.apk` pipeline, unit tests |
| **4** | `4-Roadmap-Stubs` | Component status table + phase 1→7 evolution; known gaps |
| **5** | `5-Shared-State` | `MeasurementStore`, `EventRing`, `CcliConfig` — producers/consumers |

---

## Layer summary (page 1)

| Layer | CMake target | Key paths | Role |
|-------|--------------|-----------|------|
| **L4 Orchestrator** | `ccli` executable | `services/ccli_main.cpp` | Config load, 100 ms loop, actuation policy, lab modes |
| **L3 Services** | `ccli_services` | `svc_modbus`, `svc_pf2`, `svc_io`, `svc_mms` | Thin `tick()` wrappers around adapters + core |
| **L2 Adapters** | `ccli_adapters` | `modbus_adapter`, `drv_gpio`, MMS/104 stubs | Wire protocols and I/O drivers |
| **L1 Core** | `ccli_core` | `pf2_fsm`, `measurement_store`, `event_ring`, `ccli_config` | Regulation-agnostic domain logic (PMF + PF2) |
| **L0 Platform** | `ccli_platform` | `platform_tg500/hal_gpio_ll.c`, `hal_time.cpp` | TG544 HAL — ubus `dido_v2`, timestamps |

**Dependency rule:** upper layers call lower; core never calls adapters directly (services wire them).

---

## Phase 1 runtime (page 2)

Single-threaded cooperative loop in `ccli_main.cpp`:

1. `ModbusService::tick()` — poll RTU or simulator → update `MeasurementStore`
2. `Pf2Service::tick()` — `Pf2Fsm::step()` on measurement snapshot
3. Read DIO2 permissive via `svc_io_read_permissive()`
4. Apply DIO1 only if `curtailment_active && permissive_ok`
5. Log status every ~5 s; append DO/PF2 events to `EventRing`

**Split of concerns:** PF2 decides *commanded* curtailment; main applies *permissive* interlock (lab policy, not CEI).

---

## Build & deploy (page 3)

```
apps/ccli/CMakeLists.txt
  ccli_core + ccli_platform + ccli_adapters + ccli_services → ccli
package/ccli/ → ccli-0.1.0-rN.apk
lab/tg544-openwrt/build-ccli-ipk-sdk.sh → deploy-phase1.ps1
```

Runtime config: **`/etc/ccli/lab.yaml`** (from `apps/ccli/config/lab_tr400.yaml`).

---

## Colour legend (all pages)

| Colour | Meaning |
|--------|---------|
| Green | Phase 1 active / verified path |
| Amber | Partial, scaffold, or lab-only |
| Red | Stub or not implemented (later phase) |
| Grey | TesproOS / external (procd, ubus, libmodbus) |

---

## Related architecture artefacts

| File | Relationship |
|------|--------------|
| `CCI_SW_Architecture_vs_Annex_Validation.md` | **Annex O/T/M ↔ layer validation matrix** (SoT for compliance reviews) |
| `CCI_Runtime_Dataflow_Architecture.md` | As-built LAN1/2 MMS · Modbus · PF2 · GPIO data path |
| `Functional_Architecture.drawio` | Product C1–C10 functional blocks (hardware-centric) |
| `TR400_Lab_Platform.drawio` p.4 | YAML + ubus DIO API (bench wiring) |
| `System_Context.drawio` | DSO / plant / CCI boundaries |
| `Network_Architecture.drawio` | Eth_A/B — Phase 2 target for `net_policy` |

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.1 | 2026-09-30 | Link Annex validation matrix + Runtime DF; note draw.io colour drift |
| 1.0 | 2026-09-18 | Initial SW architecture draw.io (5 pages) + this guide |

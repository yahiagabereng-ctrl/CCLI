# Application Source Code

CCI user-space on **OpenWrt 25.12 (TesPro TG544 / TG-524)**: IEC 61850, Modbus, PF2, commissioning tools.

**Software tree:** [`../../apps/ccli/`](../../apps/ccli/)  
**Structure doc:** [`CCI_Application_Structure.md`](CCI_Application_Structure.md) (`ccli-app-structure`)

## Status — scaffold created (2026-08-10)

| Component | Language | Notes |
|-----------|----------|-------|
| IEC 61850 / GOOSE / MMS | C/C++ | adapter stub; libiec61850 TBD |
| Modbus RTU/TCP master | C/C++ | adapter stub; libmodbus TBD |
| PF2 curtailment + DI/DO | C/C++ | `core/pf2/` minimal FSM |
| Local web / commissioning | Python | `apps/ccli/tools/commission/` |

## Platform

| Layer | Path |
|-------|------|
| TesPro MT798X | `apps/ccli/platform/platform_tg500/` |

**Build:** `lab/tg544-openwrt/build-ccli-ipk-sdk.sh` → deploy to TG544.

## House AT style

`sk0146-fw-at/` — SK0146 syntax rules (factory shell only)

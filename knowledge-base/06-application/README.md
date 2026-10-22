# Application Source Code

CCI user-space on **OpenWrt 25.12 (TesPro TG544 / TG-524)**: IEC 61850 MMS (62351), Modbus, PF2, IEC 104.

**Lab software tree:** [`../../apps/ccli/`](../../apps/ccli/) (`ccli_main`)  
**Product software tree:** [`../../product/apps/ccli/`](../../product/apps/ccli/) (`program/main.c` · SK0153) — [`../../product/README.md`](../../product/README.md)  
**Structure doc:** [`CCI_Application_Structure.md`](CCI_Application_Structure.md) (`ccli-app-structure`)  
**SK0153 module map:** [`SK0153_CCI_Module_Map.md`](SK0153_CCI_Module_Map.md)  
**Product promote:** [`../08-engineering/CCI_Product_SW_Freeze.md`](../08-engineering/CCI_Product_SW_Freeze.md) (`ccli-product-sw-freeze`) · [`../../releases/product-cyber/README.md`](../../releases/product-cyber/README.md)

## Status (2026-10-14)

| Component | Language | Notes |
|-----------|----------|-------|
| IEC 61850 MMS server | C/C++ | **PART** — TLS :3782 + ACSE; 31-LN cfg · lab P3/P5 |
| Modbus RTU/TCP master | C/C++ | **PART** — on TG544 RS485 paths |
| PF2 curtailment + DI/DO | C/C++ | **PART** — FSM + bench DIO |
| IEC 60870-5-104 | C/C++ | **PART** — phase 4 Eth_B |
| Local web / commissioning | Python | Out of cert path |

**Lab-first:** evidence under `lab/evidence/` on **TG544** before `releases/product-cyber/` promotion.

## Platform

| Layer | Path |
|-------|------|
| TesPro MT798X | `apps/ccli/platform/platform_tg500/` |

**Build:** `lab/tg544-openwrt/build-ccli-ipk-sdk.sh` → deploy to TG544 · manifests `lab/tg544-openwrt/deploy-manifests/`.

## House AT style

`sk0146-fw-at/` — SK0146 syntax rules (factory shell only)

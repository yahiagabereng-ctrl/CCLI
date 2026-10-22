# CCI Application — `apps/ccli/`

User-space firmware for **PF2 / CEI 0-16 CCI** on **TesPro only**.

**OS (frozen):** **OpenWrt 25.12 / TesproOS** on **TG544 / TR500 / TG-524** (MT798X).  
**Active lab DUT:** **TG544** — configs under `config/lab_tr400*.yaml`.  
**Platform:** `platform_tg500` only.

## Product software structure (SK0153 / HiTEKS)

The final product code lives in **`program/`** and follows the SK0153 – LINKY DIN 4G module structure (reference: `_sk0153_extract/components/ql-application/osi/` in the CCLI repo): `main.c` → `Boot_TaskBoot` → `Startup_TaskStartup` → `Main_TaskMain`, with one C module per function (`drvgpio`, `outputs`, `regulation`, `f_pf2`, `parser_modbus`, `transmission_mms`, `transmission_104`, `transmission_goose`, `synchronize`, `alarm`, `program_flash`, `queue_my`, `fw_config`, `commands`). `program/ccli_bridge.cpp` is the only boundary into the C++ `core/`, `adapters/` and `services/` libraries.

Module map and follow-ups: `knowledge-base/06-application/SK0153_CCI_Module_Map.md`. `-DCCLI_LEGACY_MAIN=ON` builds the old `services/ccli_main.cpp` entry for bisecting only.

| Platform | OS | Role | Build |
|----------|-----|------|-------|
| `platform_tg500` | OpenWrt / TesproOS | Lab-first, then product | `lab/tg544-openwrt/` → `.apk` |

**Corpus:** `knowledge-base/06-application/CCI_Application_Structure.md` (`ccli-app-structure`) · `CCI_OpenWrt_Freeze.md` · `CCI_Product_SW_Freeze.md` (`ccli-product-sw-freeze`)

**5-LN + cyber surface:** [`../../PRODUCT_5LN_CYBER_MINIMAL.md`](../../PRODUCT_5LN_CYBER_MINIMAL.md) · ship yaml **`config/product_tr400.yaml`**.

**Product drops:** promoted bundles in `releases/product-cyber/` after lab gates.

## Build for TG544 (cross-compile)

```bash
cd lab/tg544-openwrt
./download-sdk.sh
./build-ccli-ipk-sdk.sh    # → ccli-*.apk
```

Deploy: `lab/tg544-openwrt/DEPLOY_CCLI.md` · per-upload manifests: `lab/tg544-openwrt/deploy-manifests/`

## Config profiles

| Use | Yaml | MMS |
|-----|------|-----|
| **Product ship** | `config/product_tr400.yaml` | **:3782 TLS + ACSE + CRL · 5-LN** |
| Cert bench | `config/lab_tr400_phase4_eth_b_ttyS1_5ln.yaml` | same cyber · DSO mock on |
| Lab 31-LN | `config/lab_tr400_phase4_eth_b.yaml` | :3782 — **not** first cert CID |
| Lab TSP only | `config/lab_tr400_cleartext_tsp.yaml` | :102 — **startup REFUSE in this tree** |

## On-device bootstrap (interim)

If SDK parity issues appear, compile on TG544 with TesPro dev packages — prefer host SDK when stable.

Portable code: `program/`, `core/`, `adapters/`, `services/` — target-specific code only in `platform/platform_tg500/`.

`third_party/libiec61850` carries the CCLI 62351-4 ACSE patch (`third_party/patches/`); `ccli` does not link without it.

# CCI Application — `apps/ccli/`

User-space firmware for **PF2 / CEI 0-16 CCI** on **TesPro only**.

**OS (frozen):** **OpenWrt 25.12 / TesproOS** on **TG544 / TR500 / TG-524** (MT798X).  
**Active lab DUT:** **TG544** — configs under `config/lab_tr400*.yaml`.  
**Platform:** `platform_tg500` only (`platform_pi` **archived**).

| Platform | OS | Role | Build |
|----------|-----|------|-------|
| `platform_tg500` | OpenWrt / TesproOS | Lab-first, then product | `lab/tg544-openwrt/` → `.apk` |

**Corpus:** `knowledge-base/06-application/CCI_Application_Structure.md` (`ccli-app-structure`) · `CCI_OpenWrt_Freeze.md` · `CCI_Product_SW_Freeze.md` (`ccli-product-sw-freeze`)

**Product drops:** promoted bundles in `releases/product-cyber/` after lab gates — not a separate source tree.

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
| Lab / dev TLS | `config/lab_tr400_phase4_eth_b.yaml` | :3782 |
| Lab TSP only | `config/lab_tr400_cleartext_tsp.yaml` | :102 — **not product** |
| Product ship | `releases/product-cyber/.../config/product_tr400.yaml` | :3782 |

## On-device bootstrap (interim)

If SDK parity issues appear, compile on TG544 with TesPro dev packages — prefer host SDK when stable.

Portable code: `core/`, `adapters/`, `services/` — target-specific code only in `platform/platform_tg500/`.

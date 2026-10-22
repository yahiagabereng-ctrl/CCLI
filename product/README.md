# CCLI product firmware tree

**Document ID:** CCLI-PROD-TREE-001  
**Date:** 2026-10-15  

This folder is the **official product** source line inside the **single CCLI git repo**. It was imported from the former sibling folder `CCLI…Official Product` (2026-10-15).

---

## Layout

| Path | Role |
|------|------|
| **`product/apps/ccli/`** | Product **`apps/ccli`** — entry **`program/main.c`**, static lib **`ccli_program`** (SK0153-style modules) |
| **`apps/ccli/`** (repo root) | **Lab / conformance** line — entry **`services/ccli_main.cpp`**, TSP, deploy manifests, **P5-5LN-CERT-MODEL** |

Both trees share **`core/`**, **`adapters/`**, **`services/`**, **`config/`** by copy; **merge deliberately** when lab fixes ports (especially MMS / 62351).

---

## Build

OpenWrt package supports an alternate source dir:

```bash
# From OpenWrt SDK tree (see lab/tg544-openwrt/build-ccli-ipk-sdk.sh)
make package/feeds/ccli/ccli/compile V=s \
  CCLI_SRC_DIR=/mnt/c/Yahia/projects/CCLI/product/apps/ccli
```

Host CMake (WSL/Linux):

```bash
cmake -S product/apps/ccli -B product/apps/ccli/build-product \
  -DBUILD_PLATFORM=tg500 -DCCLI_LEGACY_MAIN=OFF
cmake --build product/apps/ccli/build-product
```

**`-DCCLI_LEGACY_MAIN=ON`** builds **`ccli_main`** inside the product tree (bisect only).

---

## Third-party libraries

`product/apps/ccli/third_party` is a **directory junction** to **`apps/ccli/third_party`** (one clone of libiec61850 / lib60870). After a fresh clone:

```powershell
powershell -File scripts\link-product-third-party.ps1
```

---

## 5-LN + full cyber (product surface)

**Map:** [`PRODUCT_5LN_CYBER_MINIMAL.md`](PRODUCT_5LN_CYBER_MINIMAL.md)

| Item | Path |
|------|------|
| Ship yaml | `product/apps/ccli/config/product_tr400.yaml` |
| Bench 5-LN yaml | `config/lab_tr400_phase4_eth_b_ttyS1_5ln.yaml` |
| CID / cfg | `config/icd/product/product_tr400_eth_a_5ln.*` |
| Specimen pin | `lab/conformance/PRODUCT_SPECIMEN_5LN.md` |
| Version | `product/apps/ccli/VERSION` → **0.1.0-p1** `PROD-5LN-CYBER` |

Startup **refuses** MMS without TLS or on port **102**, and IEC 104 without TLS. CMake **`CCLI_WITH_GOOSE=OFF`**.

---

## Workspace

Multi-root Cursor file: **`CCLI-Lab-And-Product.code-workspace`** at repo root — folders **CCLI Lab** (`.`) and **CCLI Product** (`product/apps/ccli`).

Guide: `lab/WORKSPACE_LAB_AND_PRODUCT.md`

---

## RAG / knowledge-base

Corpus sync runs from **repo root** only (`scripts/sync-ccli-corpus-to-telematry.ps1`).  
SK0153 module map: `knowledge-base/06-application/SK0153_CCI_Module_Map.md`

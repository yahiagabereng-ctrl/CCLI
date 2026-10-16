# Product cyber freeze — 0.1.0-r55

**Date:** 2026-10-15  
**Codename:** P5-5LN-CERT-MODEL  
**Parent:** `ccli-product-sw-freeze` · `ccli-lab-unified-session-pin-2026-10-14`

## Frozen cyber profile

| Item | Value |
|------|--------|
| MMS | **192.168.10.1:3782** · TLS 1.2 · mutual · ACSE G.2 |
| IEC 104 | **192.168.1.130:2404** · TLS |
| **Connect-PASS (superseded)** | r51 31-LN — see **`releases/product-cyber/0.1.0-r57/`** for live 5-LN Connect PASS |
| **Product source (this folder)** | r55 `program/main.c` · 5-LN · ship yaml **CRL on** |
| Bench yaml (keep TSP) | `apps/ccli/config/product_tr400_p3_07_connect.yaml` |
| Ship yaml (needs TSP PKI refresh) | `config/product_tr400.yaml` |
| GOOSE | **Off** (`CCLI_WITH_GOOSE=OFF`) |
| Cleartext :102 | **REFUSE** at startup (product + lab `program/main.c`) |

## Reference DUT — Connect PASS extract (2026-10-16)

| Field | Value |
|-------|--------|
| Version | **0.1.0-r51** `P3-07-AARE-SIGN-RAW-GT` |
| ELF SHA-256 | `7b05d7ccbbecd7c041d30cc6a362e1e8d1d12dea0e39792f37159d8cc5a6fabe` |
| Size | 2 022 099 B |
| Pin | `ccli-0.1.0-r51-dut-connect-pass` (this folder) · `lab/tg544-openwrt/ccli-bin.r51.dut-connect-pass` |
| Extract | `lab/tg544-openwrt/dut-extract-connect-pass-2026-10-16/MANIFEST.json` |
| DUT log | `iso session CONNECT` · `ACSE auth ACCEPT` · `AARE [10]EXTERNAL` |

Do **not** overwrite this ELF with workspace `ccli-bin` r55 (`e192d3cc…`) until Connect A/B on r55 passes.

## r55 source pin (lab, not live Connect)

| Field | Value |
|-------|--------|
| Binary SHA-256 | `a2c25122e777b4f62b5c856ee77cfb114de33fef9200b70c4ba206bd296be653` |
| Manifest | `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r55_2026-10-15_140421.json` |
| Preflight | `lab/evidence/testsuite-pro/inbox/PREFLIGHT_DUT_UNIFIED_2026-10-15.txt` |

## Trees

| Tree | Role |
|------|------|
| `apps/ccli/` | Lab build · deploy · **`program/main.c`** default |
| `product/apps/ccli/` | Product intent · sync via `scripts/sync-frozen-cyber-to-product.ps1` |

## Promote checklist

- [ ] Rebuild `ccli-bin` after merge (program entry)
- [ ] Deploy `PRODUCT-5LN-CERT` when DUT online
- [ ] Copy `.apk` + yaml + icd into this folder per `releases/product-cyber/README.md`
- [ ] Update `MANIFEST.json` SHA256 fields

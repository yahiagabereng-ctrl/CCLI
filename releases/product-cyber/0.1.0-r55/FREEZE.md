# Product cyber freeze — 0.1.0-r55

**Date:** 2026-10-15  
**Codename:** P5-5LN-CERT-MODEL  
**Parent:** `ccli-product-sw-freeze` · `ccli-lab-unified-session-pin-2026-10-14`

## Frozen cyber profile

| Item | Value |
|------|--------|
| MMS | **192.168.10.1:3782** · TLS 1.2 · mutual · ACSE G.2 · **CRL on** |
| IEC 104 | **192.168.1.130:2404** · TLS |
| Model | **5-LN** `product_tr400_eth_a_5ln.cfg` / `.cid` |
| Ship yaml | `config/product_tr400.yaml` |
| Bench yaml | `config/lab_tr400_phase4_eth_b_ttyS1_5ln.yaml` |
| GOOSE | **Off** (`CCLI_WITH_GOOSE=OFF`) |
| Cleartext :102 | **REFUSE** at startup (product + lab `program/main.c`) |

## Reference DUT (lab)

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

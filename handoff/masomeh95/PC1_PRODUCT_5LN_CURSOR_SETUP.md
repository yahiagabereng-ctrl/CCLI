# PC1 + Cursor — product tree and 5-LN cert setup

**Document ID:** CCLI-HANDOFF-PC1-PROD-5LN-001  
**Revision:** 1.0  
**Date:** 2026-10-22  
**Repo:** https://github.com/yahiagabereng-ctrl/CCLI.git  
**GitHub entry:** [00_SHARE_VIA_GITHUB.md](00_SHARE_VIA_GITHUB.md)

After **`git pull`**, PC1 has **both** code lines:

| Path | Role |
|------|------|
| **`apps/ccli/`** | Lab / TSP / deploy (`ccli_main`, bench yaml, `lab/tg544-openwrt/`) |
| **`product/apps/ccli/`** | **Product SK0153** entry **`program/main.c`**, ship yaml, **5-LN CID** |
| **`lab/conformance/`** | Specimen pin, Full Validate, **ANNEX_OT_SW_GAP_MATRIX** |

---

## One-time on PC1 (after clone)

```powershell
cd D:\CCLI\CCLI\CCLI
git pull origin main

# Shared libiec61850 / lib60870 (required for CMake)
cd apps\ccli\third_party
git clone --depth 1 --branch v1.6.0 https://github.com/mz-automation/libiec61850 libiec61850
git clone --depth 1 --branch v2.3.5 https://github.com/mz-automation/lib60870 lib60870
cd ..\..\..

# Product tree junction (Windows)
powershell -File scripts\link-product-third-party.ps1

powershell -File scripts\ensure-lab-env.ps1
powershell -File scripts\source-lab-env.ps1
powershell -File scripts\validate-5ln-stack.ps1
```

Open in **Cursor:** `D:\CCLI\CCLI\CCLI` or workspace **`CCLI-Lab-And-Product.code-workspace`**.

---

## 5-LN files (certificate model)

| Item | Path |
|------|------|
| CID | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid` |
| cfg | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cfg` |
| Product copy | same under `product/apps/ccli/config/icd/product/` |
| Bench yaml | `apps/ccli/config/lab_tr400_phase4_eth_b_ttyS1_5ln.yaml` |
| Ship yaml | `product/apps/ccli/config/product_tr400.yaml` |
| EMT432 bench | `product/apps/ccli/config/product_tr400_emt432_5ln.yaml` |
| Specimen doc | `lab/conformance/PRODUCT_SPECIMEN_5LN.md` |
| Product map | `product/PRODUCT_5LN_CYBER_MINIMAL.md` |

Validate before TSP Compare:

```powershell
powershell -File scripts\validate-5ln-cid.ps1
powershell -File scripts\pc1-p3-07-connect-preflight.ps1
```

Deploy cfg to DUT (logger PC or PC1 with LAN1 SSH):

```powershell
powershell -File scripts\push-5ln-product-cfg-to-dut.ps1
```

---

## Lab vs product sync (owner PC after edits)

```powershell
powershell -File scripts\sync-frozen-cyber-to-product.ps1 -IncludeProgram
git add apps/ccli product/apps/ccli scripts
git commit -m "…"
git push origin main
```

PC1 then **`git pull`** only.

---

## TSP session (PC1)

1. `lab\set_pc_lan1_dso.cmd` · `ping 192.168.10.1`  
2. [PC1_GITHUB_PULL_RUNBOOK.md](PC1_GITHUB_PULL_RUNBOOK.md) — TLS slots, **`product_tr400_eth_a_5ln.cid`** in TSP  
3. DUT: **`/etc/init.d/iec61850service stop`** · **`ccli`** owns **3782**

---

## Not in GitHub (use field pack or owner transfer)

- `lab/keys/` · `lab/tg544-openwrt/lab-env.ps1` (create via `ensure-lab-env.ps1`)  
- `handoff/.../CCLI_TSP_FIELD_PACK/tls/` PEM private keys → `scripts\stage-tsp-field-pack.ps1` on owner PC

---

## RAG

`ccli-product-specimen-5ln` · `ccli-share-via-github` · `ccli-prod-5ln-cyber-minimal`

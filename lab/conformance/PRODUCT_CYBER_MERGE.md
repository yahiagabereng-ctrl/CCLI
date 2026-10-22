# Lab ↔ product FW merge (frozen cyber)

**Document ID:** CCLI-LAB-PROD-CYBER-MERGE-001  
**Date:** 2026-10-15  
**RAG source_id:** `ccli-product-cyber-merge`

## Intent

One **certified cyber profile** (62351-3/4 on **3782/2404**, **5-LN**, **CRL on**) shared by:

- **Lab DUT** builds (`apps/ccli/` → `lab/tg544-openwrt/ccli-bin`)
- **Product** tree (`product/apps/ccli/`) and **`releases/product-cyber/0.1.0-r55/`**

## Entry point (merged)

| Build | Default entry | Cyber gate |
|-------|---------------|------------|
| **Cert / product** | `program/main.c` → `startup.c` REFUSE cleartext | `startup.c` |
| **Bisect only** | `-DCCLI_LEGACY_MAIN=ON` → `ccli_main.cpp` | `product_cyber_gate.hpp` (+ env `CCLI_ALLOW_CLEARTEXT_MMS` for old TSP yaml) |

## Sync

```powershell
powershell -File scripts\sync-frozen-cyber-to-product.ps1
powershell -File scripts\sync-frozen-cyber-to-product.ps1 -IncludeProgram   # refresh SK0153 sources
```

## Deploy profiles

| Profile | Yaml on DUT |
|---------|-------------|
| **PRODUCT-5LN-CERT** | `product_tr400.yaml` (ship) |
| **UNIFIED-5LN-CERT** | `lab_tr400_phase4_eth_b_ttyS1_5ln.yaml` (bench RTU) |

Both use the same **MMS/104/TLS/CRL/5-LN cfg**.

## After merge (DUT online)

1. Rebuild **r55** (program entry).
2. `deploy-from-revision-pin.ps1 -Profile PRODUCT-5LN-CERT`
3. `validate-dut-unified-matrix.ps1 -FixServices -WriteEvidence`

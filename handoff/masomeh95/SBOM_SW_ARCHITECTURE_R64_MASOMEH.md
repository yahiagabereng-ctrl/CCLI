# SBOM + software architecture — 0.1.0-r64 (Masomeh)

**Date:** 2026-11-13  
**Audience:** @masomeh95 (PC1 / TSP / Modbus plant)  
**Git commit:** **`a1956c4`** — https://github.com/yahiagabereng-ctrl/CCLI/commit/a1956c4

---

## Start here

| # | Document |
|---|----------|
| 1 | [`lab/evidence/EVD-SM-9-External-Components/CCLI_SW_ARCHITECTURE_AND_IMPLEMENTATION_0.1.0-r64.md`](../../lab/evidence/EVD-SM-9-External-Components/CCLI_SW_ARCHITECTURE_AND_IMPLEMENTATION_0.1.0-r64.md) |
| 2 | **App SBOM (CycloneDX):** [`CCLI_APP_SBOM_0.1.0-r64.cdx.json`](../../lab/evidence/EVD-SM-9-External-Components/CCLI_APP_SBOM_0.1.0-r64.cdx.json) |
| 3 | **Main entry + modules:** `apps/ccli/program/main.c` → SK0153 loop |
| 4 | **Your bench yaml:** `apps/ccli/config/product_tr400_p3_07_connect_5ln.yaml` |
| 5 | **EMT432 map:** `apps/ccli/config/modbus/chronos_emt432_map.yaml` |
| 6 | **Product mirror:** `product/apps/ccli/` (same layout; promote from lab) |

**Pull:** `git pull origin main` from repo root.

---

## What you implement / test against (r64)

| Feature | Port / path | Your role |
|---------|-------------|-----------|
| MMS 5-LN + TLS | **:3782** Eth_A | TSP Connect / Wireshark |
| Modbus → EMT432 | plant LAN **:502** | Chronos map, commissioning |
| 104 operator | **:2404** Eth_B | Logger if assigned |
| Version check | `ccli --version` | Must show **0.1.0-r64** |

**Legacy entry (do not use for cert):** `services/ccli_main.cpp` — only if `-DCCLI_LEGACY_MAIN=ON`.

# DUT ↔ repo ↔ PC1 TLS alignment — 2026-10-15

**DUT:** TG544 @ 192.168.1.130 · `ccli 0.1.0-r55` (P5-5LN-CERT-MODEL)  
**Action:** Pulled authoritative `/etc/ccli/tls/*` from DUT → synced `apps/ccli/config/tls/` → staged PC1 bundles.

## Verification

| Check | Result |
|-------|--------|
| `verify-dut-pc1-tls-alignment.ps1` | **PASS** (21 files MD5 match) |
| `verify-tsp-tls-key-match.ps1` on PC1 bundle | **PASS** |
| MD5 pin JSON | `DUT_PC1_TLS_PIN_2026-10-15.json` |

## Config snapshot (from DUT)

| File | Path |
|------|------|
| Lab yaml | `lab/evidence/dut-config-snapshots/lab_tr400_phase4_eth_b_ttyS1_5ln_from_dut_2026-10-15.yaml` |
| 5-LN cfg | `lab/evidence/dut-config-snapshots/product_tr400_eth_a_5ln_from_dut_2026-10-15.cfg` |

## PC1 handoff (same bytes as DUT)

| Deliverable | Location |
|-------------|----------|
| USB / one-file zip | **`C:\Yahia\CCLI_TSP_TLS_COMPLETE.zip`** (extract → use `tls\` in TSP) |
| Full field pack | **`C:\CCLI_TSP_FIELD_PACK\`** (`tls\` + `cid\product_tr400_eth_a_5ln.cid`) |
| Git path (after push) | `git pull` + `scripts/setup-pc1-testpro-from-git.ps1` |

**TSP Connect:** `root_CA.pem` · Transport `client_tls.pem` + `client_tsp.key` · MMS `client.pem` + **`client_acse.key`** · CRL **`lab_crl.pem`**.

Re-browse all cert paths in TSP after replacing files.

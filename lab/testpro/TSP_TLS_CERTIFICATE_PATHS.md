# Test Suite Pro — certificate paths (repo + PC staging)

**RAG:** `ccli-lab-testpro-pc1-config`  
**Machine JSON:** [`tsp_tls_and_cid.json`](tsp_tls_and_cid.json)

All lab PEMs and private keys live in the **private GitHub repo** under:

```text
apps/ccli/config/tls/              ← DUT deploy + stage-tsp-tls.ps1 default source
apps/ccli/config/tls/ejbca/        ← EJBCA export (sync → parent via sync-ejbca-tls-to-product.ps1)
```

Absolute example (clone on D:):

```text
D:\CCLI\CCLI\CCLI\apps\ccli\config\tls\
D:\CCLI\CCLI\CCLI\apps\ccli\config\tls\ejbca\
```

---

## 2026-10-08 documented PASS (historical wiring)

**Evidence:** [`TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08.md`](../evidence/testsuite-pro/inbox/TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08.md)

**TSP browse folder on PC at PASS:**

```text
C:\CCLI_product_tls\
```

**Files (repo-relative → same names on PC):**

| TSP slot | Repo path |
|----------|-----------|
| Trust | `apps/ccli/config/tls/root_CA.pem` |
| Transport TLS cert | `apps/ccli/config/tls/client_tls.pem` |
| Transport TLS key | `apps/ccli/config/tls/client_tsp.key` |
| MMS Security cert | `apps/ccli/config/tls/client.pem` |
| MMS Security key | `apps/ccli/config/tls/client_tsp.key` (both key slots) |

Stage from repo:

```powershell
powershell -File scripts\stage-tsp-tls.ps1
```

---

## Current lab PKI (EJBCA dual-key G.2 — use when DUT matches sync)

After `git pull`, run:

```powershell
powershell -File scripts\sync-ejbca-tls-to-product.ps1 -Verify
powershell -File scripts\setup-pc1-testpro-from-git.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

**TSP browse folder:**

```text
D:\CCLI_TSP_FIELD_PACK\tls\
```

(or `C:\CCLI_product_tls\` / `C:\CCLI_tls\` after `stage-tsp-tls.ps1`)

| TSP slot | File under `tls\` |
|----------|-------------------|
| Trust | `root_CA.pem` |
| Transport TLS | `client_tls.pem` + **`client_tsp.key`** |
| MMS Security | `client.pem` + **`client_acse.key`** (or `client_acse_tsp.key`) |
| CRL (when DUT `crl=on`) | `lab_crl.pem` |
| T3-04 revoked | `revoked_tls.pem` + `revoked.pem` + `revoked_tsp.key` + `revoked_acse.key` |

Preflight: `powershell -File scripts\verify-tsp-tls-key-match.ps1`

---

## Full file inventory in repo

See [`apps/ccli/config/tls/TESTPRO_TLS_FILE_MANIFEST.json`](../../apps/ccli/config/tls/TESTPRO_TLS_FILE_MANIFEST.json) and [`PKI_SYNC_MANIFEST.json`](../../apps/ccli/config/tls/PKI_SYNC_MANIFEST.json).

**Do not commit** ad-hoc `server.zip` exports unless listed in manifest; use tracked `.pem` / `.key` files.

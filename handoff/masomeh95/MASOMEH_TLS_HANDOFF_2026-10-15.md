# Masomeh — DUT-aligned TLS + 5-LN config (2026-10-15)

**DUT verified:** `ccli 0.1.0-r55` · repo TLS = DUT `/etc/ccli/tls/` (21 files MD5 match)  
**Evidence:** `lab/evidence/testsuite-pro/inbox/DUT_PC1_TLS_PIN_2026-10-15_live.json`

---

## Option A — Git (preferred)

```powershell
cd D:\CCLI\CCLI
git pull origin main
powershell -File scripts\setup-pc1-testpro-from-git.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

Read: **`00p_PC1_PULL_TLS_DUT_ALIGNED.md`** → **`00k_PC1_EJBCA_TLS_TESTPRO.md`**

---

## Option B — USB (full TLS zip from Yahia)

Yahia may send **`CCLI_TSP_TLS_COMPLETE.zip`** (every PEM/key from live DUT). Extract to:

```text
D:\CCLI_TSP_FIELD_PACK\tls\
```

Then still **`git pull`** for CID, scripts, and matrix docs — or copy **`cid\product_tr400_eth_a_5ln.cid`** from the full field pack zip if provided.

---

## Pass TLS in Test Suite Pro (Connect)

| TSP field | File under `tls\` |
|-----------|-------------------|
| Trust CA | `root_CA.pem` |
| Transport cert | `client_tls.pem` |
| Transport key | **`client_tsp.key`** |
| MMS / ACSE cert | `client.pem` |
| MMS / ACSE key | **`client_acse.key`** |
| CRL | **`lab_crl.pem`** |

| Connection | Value |
|------------|--------|
| Server | **`192.168.10.1:3782`** |
| TLS | **On** |
| IED | **`CCI016_01`** |
| CID | **`cid\product_tr400_eth_a_5ln.cid`** |

**Preflight (must PASS):**

```powershell
powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
openssl verify -CAfile D:\CCLI_TSP_FIELD_PACK\tls\root_CA.pem D:\CCLI_TSP_FIELD_PACK\tls\client.pem D:\CCLI_TSP_FIELD_PACK\tls\client_tls.pem
```

**LAN1:** `bench_scripts\set_pc_lan1_dso.cmd` → PC **`192.168.10.10/24`** → `ping 192.168.10.1`

**Success on DUT log (Yahia capture):** `ACSE auth ACCEPT` · `AARE` · TSP **Initiate Response Success**

**Do not use Oct 8 wiring** (`client_tsp.key` in **both** MMS and Transport slots) on this PKI.

Evidence: **`tsp-evidence-logger.ps1 -CcliPkg 0.1.0-r55`**

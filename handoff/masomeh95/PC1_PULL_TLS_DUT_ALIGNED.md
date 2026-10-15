# PC1 — pull certificates aligned with DUT (Masomeh)

**Date:** 2026-10-15  
**DUT pin:** `ccli 0.1.0-r55` · MMS `192.168.10.1:3782` · **CRL on** · model **`product_tr400_eth_a_5ln.cfg`**  
**Repo TLS:** `apps/ccli/config/tls/` (EJBCA sync — see `PKI_SYNC_MANIFEST.json`)

---

## One command (after every `git pull`)

```powershell
cd D:\CCLI\CCLI
git pull origin main
powershell -File scripts\setup-pc1-testpro-from-git.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

This stages:

| Output | Purpose |
|--------|---------|
| `D:\CCLI_TSP_FIELD_PACK\tls\` | Full lab PKI mirror (same file names as DUT `/etc/ccli/tls/`) |
| `tls\DUT_TLS_MANIFEST.json` | MD5 per file — compare with Yahia if Connect fails |
| `cid\product_tr400_eth_a_5ln.cid` | **Mandatory** Compare / browse (5-LN cert) |
| `00k_PC1_EJBCA_TLS_TESTPRO.md` | TSP TLS + CRL wiring for **current** DUT |
| `00_UNIFIED_CERT_TEST_MATRIX.md` | Cyber + functional checklist |

---

## Verify before Connect

```powershell
cd D:\CCLI_TSP_FIELD_PACK
powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir tls
openssl verify -CAfile tls\root_CA.pem tls\client.pem tls\client_tls.pem
```

**TSP (EJBCA / current DUT — not Oct 8 single-key profile):**

| TSP field | File |
|-----------|------|
| CA | `root_CA.pem` |
| Transport TLS | `client_tls.pem` + **`client_tsp.key`** |
| MMS Security | `client.pem` + **`client_acse.key`** |
| CRL | **`lab_crl.pem`** (DUT has `crl=on`) |

| Connection | Value |
|------------|--------|
| IED | **`CCI016_01`** |
| Server | **`192.168.10.1:3782`** · TLS **on** |
| CID import | **`cid\product_tr400_eth_a_5ln.cid`** |

Evidence logger: `-CcliPkg 0.1.0-r55`

Historical Oct 8 PASS (optional): `00m_PC1_P3_07_FIRST_PASS_TLS_2026-10-08.md` — use only if Yahia confirms old PKI.

---

## If repo TLS ≠ DUT

Yahia runs on logger PC:

```powershell
powershell -File scripts\pull-dut-tls-full-mirror.ps1
powershell -File scripts\publish-ejbca-lab-pki.ps1   # if re-sync from ejbca/
git add apps/ccli/config/tls; git commit; git push
```

Then you **`git pull`** and re-run **`setup-pc1-testpro-from-git.ps1`**.

**Keywords:** Masomeh, PC1, TLS, EJBCA, DUT-aligned, 5-LN, r55

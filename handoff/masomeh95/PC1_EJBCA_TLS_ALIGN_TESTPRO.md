# PC1 — align Test Suite Pro TLS with DUT (EJBCA lab PKI)

**For:** Masomeh · **PC-B** · Test Suite Pro only  
**Date:** 2026-10-15  
**Git pin:** `main` after **2026-10-15** TLS verify (repo = DUT MD5 pin `DUT_PC1_TLS_PIN_2026-10-15_live.json`)  
**DUT:** `ccli 0.1.0-r55` · MMS `192.168.10.1:3782` · **CRL on** (`crl=on`) · model **`/etc/ccli/icd/product_tr400_eth_a_5ln.cfg`**

Yahia re-issued lab certificates in **EJBCA** and deployed the same PEM set to the TG544. **You must replace any old TLS files in Test Suite Pro** with the new pack (do not keep a previous `C:\CCLI_tls` or old TSP browse paths from an earlier session).

---

## Step 1 — Get PEMs from GitHub

```powershell
cd D:\CCLI\CCLI
git pull origin main
powershell -File scripts\setup-pc1-testpro-from-git.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

Check these files exist:

```text
D:\CCLI_TSP_FIELD_PACK\tls\root_CA.pem
D:\CCLI_TSP_FIELD_PACK\tls\client.pem
D:\CCLI_TSP_FIELD_PACK\tls\client_tls.pem
D:\CCLI_TSP_FIELD_PACK\tls\client_tsp.key
D:\CCLI_TSP_FIELD_PACK\tls\client_acse.key
D:\CCLI_TSP_FIELD_PACK\tls\lab_crl.pem
D:\CCLI_TSP_FIELD_PACK\tls\revoked.pem
D:\CCLI_TSP_FIELD_PACK\tls\revoked_tls.pem
D:\CCLI_TSP_FIELD_PACK\tls\revoked_tsp.key
```

Optional: copy pack TLS into TSP browse folder:

```powershell
powershell -File scripts\stage-tsp-tls.ps1 -ProductDest "D:\CCLI_TSP_FIELD_PACK\tls"
```

---

## Step 2 — OpenSSL preflight (save output optional)

```powershell
cd D:\CCLI\CCLI
powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
cd D:\CCLI_TSP_FIELD_PACK\tls
openssl verify -CAfile root_CA.pem client.pem client_tls.pem
openssl x509 -in client_tls.pem -noout -subject -dates
openssl x509 -in client.pem -noout -subject -dates
```

All **client** checks must pass before Connect. **MMS key must be `client_acse.key`** (script PASS = correct pairing).

---

## Step 3 — Wire Test Suite Pro (normal Connect / T1-01)

Use folder: **`D:\CCLI_TSP_FIELD_PACK\tls\`**

| TSP field | File |
|-----------|------|
| Trusted CA / root | **`root_CA.pem`** |
| Transport TLS certificate | **`client_tls.pem`** |
| Transport TLS private key | **`client_tsp.key`** (Cert A — pairs with `client_tls.pem`) |
| MMS Security / ACSE certificate | **`client.pem`** |
| MMS Security private key | **`client_acse.key`** (Cert B — **different** key; do **not** use `client_tsp.key` here) |
| CRL (enable in connection if TSP has CRL browse) | **`lab_crl.pem`** |

| Connection | Value |
|------------|--------|
| Server | **`192.168.10.1:3782`** |
| TLS | **On** (not port 102 cleartext) |
| IED | **`CCI016_01`** |
| CID import | **`cid\product_tr400_eth_a_5ln.cid`** (5-LN cert — mandatory) |

LAN1 (Admin): `bench_scripts\set_pc_lan1_dso.cmd` → PC **`192.168.10.10`** → `ping 192.168.10.1`

Full UI notes: **`00i_PC1_TSP_IMPORT_CHECKLIST.md`** in the pack.

---

## Step 4 — T3-04 revoked test only (then restore DSO certs)

| TSP field | File |
|-----------|------|
| Transport TLS certificate | **`revoked_tls.pem`** |
| MMS Security certificate | **`revoked.pem`** |
| Transport private key | **`revoked_tsp.key`** |
| MMS private key | **`revoked_acse.key`** |
| CRL | **`lab_crl.pem`** |

Connect must **fail**. After evidence capture, **restore Step 3** (`client_tls.pem` + `client_tsp.key` + `client.pem` + `client_acse.key`).

---

## Step 5 — Evidence logger

```powershell
cd D:\CCLI_TSP_FIELD_PACK
powershell -File scripts\tsp-evidence-logger.ps1 -Action Init -SessionDate 2026-10-15 -Operator Masomeh -CcliPkg 0.1.0-r55
```

Per-test Start/Stop: **`00h_PC1_LOG_CAPTURE_REQUIREMENTS.md`**.

---

## If Connect fails after cert update

1. Confirm **`git pull`** and re-run **`setup-pc1-testpro-from-git.ps1`**.  
2. **`verify-tsp-tls-key-match.ps1`** must PASS (Transport=`client_tsp.key`, MMS=`client_acse.key`).  
3. In TSP UI, **re-browse every TLS path** after replacing files (do not rely on old `C:\CCLI_product_tls\`).  
4. CA = **`root_CA.pem`** from pack; CRL = **`lab_crl.pem`**.  
5. Ask Yahia: DUT **`ccli` on 3782** (not TesPro `ubus` mmsd) + 120s connect log capture.

**Keywords:** Masomeh, PC1, EJBCA, TLS, TestPro, 3782, revoked, CRL

# PC1 — TLS bundle (DUT deploy ∩ certification bench)

**Audience:** Masomeh (Test Suite Pro PC)  
**DUT pin:** `ccli 0.1.0-r55` · PKI under **`/etc/ccli/tls/`**  
**Scope:** Only what **(a)** is deployed on the DUT for MMS/TLS **and** **(b)** UCA / 61850-10 **Test Suite Pro** needs on the **client (DSO) PC**.

**Not pushed to PC1:** DUT **server** certs/keys (`server*.pem`, `server*.key`), **CA private key** (`root_CA.key`), or DUT dev junk (`gen_lab_pki.py`, etc.). Those stay on the TG544.

**Accredited lab (D1 PICS quote):** submit Excel/PIXIT — **not** this PEM zip.

---

## 1. One command (after `git pull`)

```powershell
cd D:\CCLI\CCLI
git pull origin main
powershell -File scripts\setup-pc1-testpro-from-git.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
```

(`setup-pc1-testpro-from-git.ps1` runs `stage-pc1-tls-dut-bundle.ps1` into `tls\`.)

Compare MD5 with DUT for **each file in the bundle**:

```bash
md5sum /etc/ccli/tls/root_CA.pem /etc/ccli/tls/client_tls.pem ...
```

Or open **`D:\CCLI_TSP_FIELD_PACK\tls\DUT_TLS_MANIFEST.json`**.

---

## 2. Files included on PC1

| File | On DUT | Cert / TSP |
|------|--------|------------|
| `root_CA.pem` | yes | T1-01, all TLS |
| `client_tls.pem` + `client_tsp.key` | yes | **T1-01** mutual TLS (transport) |
| `client.pem` + `client_acse.key` | yes | **T1-01** MMS Security (62351-4) |
| `client.key` | yes | Backup / OpenSSL |
| `viewer.pem` / `viewer_tls.pem` / `viewer.key` | yes | **T3-02 / T3-03** RBAC |
| `revoked.pem` / `revoked_tls.pem` / `revoked*.key` | yes | **T3-04** revoked + CRL |
| `lab_crl.pem` | yes | **T3-04** / P3-09 |

---

## 3. Files intentionally **not** on PC1

| Item | Reason |
|------|--------|
| `server.pem`, `server_tls.pem`, `server_combined.pem` | **IED server** role — only on DUT :3782 |
| `server.key`, `server_acse.key`, `server_combined.key` | DUT private keys — not used in TSP |
| `root_CA.key` | CA signing — not in deploy script, not for TSP |
| `gen_lab_pki.py`, `README.md`, … | Not part of cert bench |

---

## 4. MD5 spot-check (DUT r55, 2026-10-14)

| File | MD5 |
|------|-----|
| `root_CA.pem` | `f265ae5ca881a5097bc74a543784b443` |
| `client_tls.pem` | `13c231ee6a2e782b3eb464ac2dee4c21` |
| `client.pem` | `7bb6b77cde4614ae68ad387e580e2696` |
| `revoked_tls.pem` | `80f3df034b15c7439f1e8d6fe9459b24` |
| `revoked.pem` | `c92bf69d697711547647c6636496db5e` |
| `lab_crl.pem` | `0695da4dc4829236127e9e904b9ccdfc` |
| `viewer_tls.pem` | `d60f37d793dc03dd5fe0815c3d0a718e` |

---

## 5. TSP Connect (T1-01)

| TSP field | File |
|-----------|------|
| CA | `root_CA.pem` |
| Transport TLS | `client_tls.pem` + `client_tsp.key` |
| MMS Security | `client.pem` + `client_acse.key` |

See **`PC1_TSP_IMPORT_CHECKLIST.md`** · TLS errors: **`lab/evidence/testsuite-pro/inbox/TSP_TLS_ACCESS_DENIED_2026-10-14.md`**.

---

## 6. Delivery

| Method | Action |
|--------|--------|
| **Git** | `git pull` + `setup-pc1-testpro-from-git.ps1` |
| **USB** | Copy **`D:\CCLI_TSP_FIELD_PACK\tls\`** only (not whole repo) |
| **UCA lab** | PICS/PIXIT/MICS — separate from this folder |

# PC1 — full TLS bundle aligned with DUT release

**Audience:** Masomeh (Test Suite Pro PC)  
**DUT pin (2026-10-14):** `ccli 0.1.0-r55` · TLS under **`/etc/ccli/tls/`**  
**Repo source:** `apps/ccli/config/tls/` (after EJBCA sync — see `PKI_SYNC_MANIFEST.json`)

Give her **this bundle**, not an old `CCLI_product_tls` folder left on disk from a partial CA refresh.

---

## 1. One command (after `git pull`)

```powershell
cd D:\CCLI\CCLI
git pull origin main
powershell -File scripts\setup-pc1-testpro-from-git.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
powershell -File scripts\stage-pc1-tls-dut-bundle.ps1 -Dest D:\CCLI_TSP_FIELD_PACK\tls
powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
```

Check **`D:\CCLI_TSP_FIELD_PACK\tls\DUT_TLS_MANIFEST.json`**: each **`md5`** must match DUT:

```bash
md5sum /etc/ccli/tls/<same-filename>
```

---

## 2. Files to hand over (aligned with DUT)

| File | On DUT | Give PC1 | TSP use |
|------|--------|----------|---------|
| `root_CA.pem` | yes | **yes** | Trust anchor (all connects) |
| `server.pem` | yes | yes (reference) | DUT only — not loaded in TSP |
| `server_tls.pem` | yes | yes (reference) | DUT transport server cert |
| `server_combined.pem` | yes | yes (reference) | DUT optional combined |
| `client_tls.pem` | yes | **yes** | **T1 Connect** — Transport TLS |
| `client_tsp.key` | yes | **yes** | Transport private key |
| `client.pem` | yes | **yes** | **T1 Connect** — MMS Security |
| `client_acse.key` | yes | **yes** | MMS private key |
| `client.key` | yes | yes | Same as `client_tsp.key` (backup) |
| `viewer.pem` / `viewer_tls.pem` / `viewer.key` | yes | **yes** | **T3-02/03** viewer RBAC |
| `revoked.pem` / `revoked_tls.pem` | yes | **yes** | **T3-04** negative |
| `revoked.key` / `revoked_tsp.key` / `revoked_acse.key` | yes | **yes** | Revoked test keys |
| `lab_crl.pem` | yes | **yes** | **T3-04** CRL check |
| `root_CA.key` | yes | **yes** | Lab CA (offline / regen only — not loaded in TSP Connect) |
| `server.key` / `server_acse.key` / `server_combined.key` | yes | **yes** | DUT server keys — mirror for OpenSSL verify / emergency DUT restore |

**Full DUT PKI mirror on PC1** (same files as deploy to `/etc/ccli/tls/`). TSP Connect still uses **client** certs only; server keys are for parity with DUT r55, not for normal TSP UI.

**Handling:** USB or private git pull + script — **do not** email keys or commit `lab/testpro/staged/`.

---

## 3. MD5 spot-check (DUT r55, 2026-10-14)

| File | MD5 (must match PC1 bundle) |
|------|----------------------------|
| `root_CA.pem` | `f265ae5ca881a5097bc74a543784b443` |
| `client_tls.pem` | `13c231ee6a2e782b3eb464ac2dee4c21` |
| `client.pem` | `7bb6b77cde4614ae68ad387e580e2696` |
| `server_tls.pem` | `40af60a369dec05933bd603888877a50` |
| `revoked_tls.pem` | `80f3df034b15c7439f1e8d6fe9459b24` |
| `revoked.pem` | `c92bf69d697711547647c6636496db5e` |
| `lab_crl.pem` | `0695da4dc4829236127e9e904b9ccdfc` |
| `viewer_tls.pem` | `d60f37d793dc03dd5fe0815c3d0a718e` |

Full list: **`DUT_TLS_MANIFEST.json`** in the staged `tls\` folder.

---

## 4. TSP mapping (normal connect)

| TSP field | File |
|-----------|------|
| CA | `root_CA.pem` |
| Transport TLS | `client_tls.pem` + `client_tsp.key` |
| MMS Security | `client.pem` + `client_acse.key` |

Revoked / viewer: see **`PC1_TSP_IMPORT_CHECKLIST.md`** §3.  
TLS access denied after CA update: **`lab/evidence/testsuite-pro/inbox/TSP_TLS_ACCESS_DENIED_2026-10-14.md`**.

---

## 5. Delivery options

| Method | Action |
|--------|--------|
| **Git (preferred)** | Masomeh `git pull` + scripts above |
| **USB** | Copy entire **`D:\CCLI_TSP_FIELD_PACK\tls\`** including `DUT_TLS_MANIFEST.json` |
| **Do not** | Public email / Slack attachments of the `tls\` folder (contains all private keys) |

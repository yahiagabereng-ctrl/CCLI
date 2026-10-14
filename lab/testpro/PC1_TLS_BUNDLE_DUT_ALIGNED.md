# PC1 — full TLS mirror from **deployed DUT**

**Give Masomeh everything** under `/etc/ccli/tls/` that matters for deploy + TSP (including **server** and **CA private keys**).  
**Do not** git-commit or email this folder.

---

## Send (Yahia, logger PC)

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\export-pc1-tls-for-masomeh.ps1 -Dest D:\handoff\PC1_tls_full
```

This **pulls from the DUT** (not only repo) so bytes match **what is deployed**. USB **`D:\handoff\PC1_tls_full`** to Masomeh.

Check **`DUT_TLS_MANIFEST.json`** inside the folder for MD5 per file.

---

## Masomeh — use in TSP

Copy the whole folder to e.g. **`D:\CCLI_TSP_FIELD_PACK\tls\`**.

**Connect (T1-01):**

| TSP field | File |
|-----------|------|
| CA | `root_CA.pem` |
| Transport | `client_tls.pem` + `client_tsp.key` |
| MMS | `client.pem` + `client_acse.key` |

**Also in folder (Tier 3 / reference):** viewer*, revoked*, `lab_crl.pem`, `server*.pem`, `server*.key`, `root_CA.key`.

```powershell
powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
powershell -File scripts\pc1-test-mms-tls12.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
```

TSP: **TLS 1.2** · IED **`CCI016_01` @ `192.168.10.1:3782`**.

See **`READ_ME_CONNECT_FIX.md`** in the USB folder.

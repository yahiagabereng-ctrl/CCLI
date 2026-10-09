# TSP Connect — CA mismatch (2026-10-09)

**Symptom:** `AssociationTimeout` · `unable to get local issuer certificate` · server path HiTEKS / CCU_01 or CCI016_01.

**Root cause:** PC staging used **`apps/ccli/config/tls/ejbca`** (`CN=CCLI Lab CA`, MD5 `B28B0F41…`) while DUT **`/etc/ccli/tls/`** uses **`apps/ccli/config/tls`** (`CN=CCLI_Lab_Root_CA`, MD5 **`D0DC009C…`**). Deploy script already defaults to legacy; **`stage-tsp-tls.ps1`** incorrectly preferred ejbca.

**DUT verify (2026-10-09):**

| File | MD5 (match legacy repo) |
|------|-------------------------|
| `root_CA.pem` | `d0dc009c…` |
| `server_tls.pem` | `fd1e84b4…` |
| `client_tls.pem` | `f8a7dcb1…` |

**Fix applied on PC-B:**

```powershell
powershell -File scripts\stage-tsp-tls.ps1 -Src D:\CCLI\CCLI\CCLI\apps\ccli\config\tls
```

(`stage-tsp-tls.ps1` default updated to legacy to match deploy.)

**TSP:** Re-select (or confirm) CA **`C:\CCLI_product_tls\root_CA.pem`**, MMS **`client.pem`**, Transport **`client_tls.pem`**, key **`client_tsp.key`** → **Connect**.

**DUT:** `3782` listening · `server_tls.pem: OK` against DUT `root_CA.pem`.

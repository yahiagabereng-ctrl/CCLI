# Security PASS pack (binary + certificates)

**Repo pack:** [`lab/releases/P3-07-SECURITY-PASS/`](../../lab/releases/P3-07-SECURITY-PASS/)  
**Pins:** [`MANIFEST.json`](../../lab/releases/P3-07-SECURITY-PASS/MANIFEST.json)

## Build on PC1 / logger

```powershell
cd D:\CCLI\CCLI
git pull origin main
powershell -File scripts\build-security-pass-pack.ps1
powershell -File scripts\setup-pc1-p3-07-connect.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

## Accurate pins (must match)

| Role | Serial / hash |
|------|----------------|
| Transport `client_tls.pem` | serial **`DC9A9E26D547EF38`** · `CN=DSO_OPERATOR` |
| MMS `client.pem` | serial **`28E53F07CFE347B0`** |
| Key both slots | **`client_tsp.key`** |
| PASS apk | **`ccli-0.1.0-r51.apk`** (see MANIFEST SHA-256) |

TSP: **do not** use `client.pem` for Transport · **do not** use `client_acse.key` for this session · CID **`lab_tg544_eth_a.cid`**.

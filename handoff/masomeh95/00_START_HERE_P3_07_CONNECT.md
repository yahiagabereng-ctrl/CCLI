# START HERE — P3-07 Connect only (Oct-08 profile)

**Ignore** `00i` / `00p` / unified matrix docs for Connect — they use **`client_acse.key`** and **5-LN CID**.

## One command (PC1, Admin)

```powershell
cd D:\CCLI\CCLI\CCLI
git pull origin main
powershell -ExecutionPolicy Bypass -File scripts\pc1-p3-07-connect-preflight.ps1
```

## Test Suite Pro (required if Connect ever failed before)

1. **Close** TSP · **New project** (old `.tsp` caches wrong cert paths).
2. Import **`D:\CCLI_TSP_FIELD_PACK\cid\lab_tg544_eth_a.cid`** only.
3. IED **`CCI016_01`** · **`192.168.10.1:3782`** · TLS ON · local **`192.168.10.10`**
4. Browse **every** path from **`D:\CCLI_TSP_FIELD_PACK\tls\`**:

| Slot | File |
|------|------|
| Trust | `root_CA.pem` |
| Transport cert | `client_tls.pem` (serial **DC9A9E…**) |
| Transport key | `client_tsp.key` |
| MMS cert | `client.pem` (serial **28E53F07…**, OID **1.12**) |
| MMS key | **`client_tsp.key`** (same file — **not** `client_acse.key`) |
| CRL / revoked | **empty** |

5. AP Title **`1,1,1,999,1`** · chaining **1** · Sisco **OFF** · TLS **1.3 OFF**
6. **System Status → Connect**

## Pass

- TSP: **Initiate Response Success**
- DUT log: **`mms: ACSE auth ACCEPT`**

If DUT shows **`client CONNECT` only** → MMS key or CID still wrong in TSP UI.

Minimal USB pack (same bytes): **`handoff/masomeh95/CCLI_TSP_CONNECT_MIN/`**

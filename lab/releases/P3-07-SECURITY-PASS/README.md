# P3-07 SECURITY PASS pack — TesPro binary + certificates

**Purpose:** Pin the **exact** Test Suite Pro security profile that **PASS**ed (2026-10-08) and the PKI last deployed for Connect recovery (2026-10-15).

**Evidence:** [`TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08.md`](../../evidence/testsuite-pro/inbox/TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08.md)  
**Machine pins:** [`MANIFEST.json`](MANIFEST.json)

---

## What to deploy on TesPro (DUT)

| Item | Value |
|------|--------|
| **PASS-proven package** | **`bin/ccli-0.1.0-r51.apk`** · codename `P3-07-AARE-SIGN-RAW-GT` |
| **APK SHA-256** | see `MANIFEST.json` → `binary_pass_documented.apk_sha256_local` |
| **Lab yaml** | `config/lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml` (`crl=off`) |
| **Model** | `cid/lab_tg544_eth_a.cfg` (+ matching CID for TSP) |
| **TLS on DUT** | copy entire `tls/` → `/etc/ccli/tls/` |

Last lab deploy note: **r55** kept the r51 AARE stack and switched DUT to this yaml + `p3_07_connect` PKI. Prefer **r51** when you need the documented security PASS binary; use r55 only if you have the DUT SHA `a2c25122…` bytes.

---

## Certificates (TSP — must match MANIFEST serials)

| TSP slot | File | Serial (hex) |
|----------|------|--------------|
| Trust CA | `tls/root_CA.pem` | `4DAA40028901CFFB` |
| Transport TLS | `tls/client_tls.pem` | **`DC9A9E26D547EF38`** · `CN=DSO_OPERATOR` |
| Transport key | `tls/client_tsp.key` | |
| MMS Security | `tls/client.pem` | **`28E53F07CFE347B0`** · OID `1.1.1.999.1.12` |
| MMS key | **`tls/client_tsp.key`** (same file) | |

**CRL:** DUT yaml uses **`crl=off`**. On TSP, if the UI errors on an empty revocation list, use **`lab_crl.pem`** (from `apps/ccli/config/tls/lab_crl.pem`) on **both** Transport and MMS tabs — same file as staged in `C:\CCLI_tls\`.

| Connection | Value |
|------------|--------|
| IED | `CCI016_01` |
| Host | `192.168.10.1:3782` |
| Local IP | `192.168.10.10` |
| CID | `cid/lab_tg544_eth_a.cid` |

Verify:

```powershell
cd lab\releases\P3-07-SECURITY-PASS\tls
openssl x509 -in client_tls.pem -noout -serial -subject
openssl x509 -in client.pem -noout -serial -subject
powershell -File ..\..\..\..\scripts\verify-tsp-tls-key-match.ps1 -TlsDir .
```

---

## Rebuild this pack

```powershell
powershell -File scripts\build-security-pass-pack.ps1
```

---

## Do not mix

- EJBCA dual-key (`client_acse.key`) + 5-LN CID  
- Putting `client.pem` in the **Transport** slot  

See: `handoff/masomeh95/PC1_P3_07_CONNECT_PROFILE_ACTIVE.md`

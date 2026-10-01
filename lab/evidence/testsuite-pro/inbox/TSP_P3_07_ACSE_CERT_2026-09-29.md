# P3-07 — TSP ACSE certificate association (deploy 2026-09-29)

## Change

`ccli` now accepts **both** ACSE auth paths required by Triangle Test Suite Pro:

| Mechanism | Source | Handler |
|-----------|--------|---------|
| `ACSE_AUTH_TLS` (3) | TLS peer certificate | `mms_acse_auth` + RBAC |
| `ACSE_AUTH_CERTIFICATE` (2) | AARQ auth-value (62351-4 / TSP MMS Security) | same |

libiec61850 vendor patch (`acse.c`):

- Recognize certificate mechanism OID `{0x52,0x03,0x02}`
- Prefer AARQ auth-value over TLS peer cert when both present
- Extract embedded X.509 from MMS-Authentication-value wrapper

## DUT

- Binary: `lab/tg544-openwrt/ccli-bin` (r16 build)
- Listening: `192.168.10.1:3782` TLS, RBAC on
- Deploy: stop procd → upload `/usr/sbin/ccli` → restart

## TSP profile (exact)

| Setting | Value |
|---------|-------|
| IED IP | `192.168.10.1` |
| Local IP | `192.168.10.10` |
| Port | `3782` |
| GOOSE / SV | **OFF** |
| MMS Security | **Certificate** — `C:\CCLI_tls\` |
| CA | `root_CA.pem` |
| Client cert | `client.pem` |
| Client key | **`client_tsp.key`** |
| CRL | `lab_crl.pem` |
| Transport TLS | **ON** — same files + TLS RSA key/cert |
| Sisco Compatibility | **OFF** |
| TLS v1.3 | **OFF** |
| Directory to CA | **empty** |
| Connect from | **System Status** |

## Expected DUT log (success)

```text
mms: ACSE auth ACCEPT — mech=CERTIFICATE role=DSO_OPERATOR cert=… octets
```

## Evidence to capture

Save TSP log + DUT `tail /tmp/ccli.log` to this folder as `TSP_P3_07_CONNECT_*.txt`.

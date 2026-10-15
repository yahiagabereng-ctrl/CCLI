# Transport connection closed — root cause from DUT log (2026-10-15 ~22:03 CST)

**Symptom:** TestPro `Initiate Response Failed: Transport connection closed`  
**DUT:** `ccli` **15502** on **3782**, `crl=off`, P3-07 connect PKI (`CCLI_Lab_Root_CA`)

## What the DUT saw at TLS (22:03:31)

The **certificate presented on the wire at TLS** (client hello / client cert):

| Field | Value |
|--------|--------|
| Serial | **`28:E5:3F:07:CF:E3:47:B0`** |
| Subject | **`2.5.4.106=1.1.1.999.1.12`** (MMS / ACSE **client.pem**) |
| Issuer | `CN=CCLI_Lab_Root_CA` |

Allow-list check:

1. Compare to **transport** allow cert **`client_tls.pem`** serial **`DC:9A:9E:26:D5:47:EF:38`** (`CN=DSO_OPERATOR`) — **not the cert on the wire**
2. Compare to **`client.pem`** serial **`28:E5:3F:07:CF:E3:47:B0`** — **match** → TLS OK

Then:

```text
mms: client CONNECT count=1
mms: client DISCONNECT count=0
mms: no clients — Operating Rule fallback armed (15 s)
```

**No** `ACSE auth ACCEPT` · **No** `AARE responder auth [10]EXTERNAL`.

## Interpretation

TestPro (or PC1 TLS paths) is using **`client.pem` in the Transport TLS certificate slot** (or only one cert for both layers). Mutual TLS completes because `client.pem` is allowlisted, but the **Transport/MMS cert split expected by 62351-4 / Oct-8 profile is wrong** → session drops before ACSE completes → TSP reports **Transport connection closed**.

## PC1 fix (Oct-08 / P3-07 connect profile)

| TSP slot | File | Check on PC1 |
|----------|------|----------------|
| Transport TLS **certificate** | **`client_tls.pem`** | OpenSSL serial must be **`DC:9A:9E:26:D5:47:EF:38`** (or re-stage pack) |
| Transport TLS **key** | **`client_tsp.key`** | |
| MMS Security **certificate** | **`client.pem`** | Serial **`28:E5:3F:07:CF:E3:47:B0`** |
| MMS Security **key** | **`client_tsp.key`** (same file) | |
| Trust CA | **`root_CA.pem`** from **`p3_07_connect`** only | `CN=CCLI_Lab_Root_CA` |

```powershell
cd D:\CCLI_TSP_FIELD_PACK\tls
openssl x509 -in client_tls.pem -noout -serial -subject
openssl x509 -in client.pem -noout -serial -subject
```

**Re-browse all six paths in TSP** after fixing — do not leave Transport pointing at `client.pem`.

## DUT bench (Yahia)

Before Connect: `3782` must be **`ccli`**, not **`ubus`** (`dut-mms-tsp-preflight.ps1`).

Evidence: `TSP_CONNECT_DUT_LOG_2026-10-15_attempt.txt`, TLS detail pull 22:03:31.

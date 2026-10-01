# TSP P3-07 — G.6.2 cert decode fix (2026-09-29)

## TSP error

```
MMS Security error: Unable to decode peer certificate (+)
Openssl Error: error:0D0680A1:asn1 encoding routines: nested asn1 error
→ ACSE Abort → AssociationTimeout
```

## Root cause

AARE **responding-authentication-value** embedded `server.pem` with wrong RDN type **2.5.4.45** (`uniqueIdentifier`) instead of **2.5.4.106** (`id-at-objectIdentifier`). **OpenSSL/TSP could not parse** (`unsupported`).

Transport TLS already uses `server_tls.pem` (CN subject) per **62351-4 Annex G.2** dual-cert rule.

## Normative basis (RAG corpus)

From `knowledge-base/08-engineering/CCI_62351-4_Extract.md` (Batch F, pp. 103–106):

| Ref | Requirement |
|-----|-------------|
| **G.2** | Separate end-entity certs for **TLS** vs **E2E/application** when both are used |
| **G.6.2** | E2E/OSI cert subject: single RDN **objectIdentifier** (X.520) = AP-title + AE-qualifier (§10.5.3) |
| **§11.2.2 (A-profile)** | MMS auth value: DER X.509 cert + GeneralizedTime + sha256WithRSA signature — cert must be **decodable** |

**Lab resolution (2026-09-29 fix):** Annex **G.2 dual cert** — TLS `server_tls.pem`, AARE auth **`server.pem`** with subject **`2.5.4.106=1.1.1.999.1.12`** + G.5.2 keyUsage. AP/AE titles in AARE: `1.1.1.999.1` / `12`.

**2026-09-29 follow-up:** `gen_lab_pki.py` now patches subject value to ASN.1 **OBJECT IDENTIFIER** (`0x06`) via pyasn1 re-sign (cryptography alone emitted UTF8String `0x0c`). DUT: `cert=772 octets`.

**2026-09-29 regression:** mbedTLS `x509_crt_parse` on TG544 returns **-9186** for G.6.2 OID subject → AARE self-verify failed → **no auth on wire** (`TX_ACCEPT len=161`) → TSP *no ACSE authentication present*. Fix: self-verify via `mbedtls_pk_verify(&g_server_key, …)` only; embed raw DER for TSP/OpenSSL.

**2026-09-29 OpenSSL/TSP:** Strict OBJECT IDENTIFIER **value** under 2.5.4.106 makes OpenSSL 3 `x509 -subject` → **unsupported** (TSP OpenSSL decode error). **Lab fix:** keep 2.5.4.106 + **UTF8String** value (`subject=2.5.4.106=1.1.1.999.1.12`) — OpenSSL parses; signature path fixed separately.

## PDF OCR note

User PDF `IEC 62351-4 2020 - 道客巴巴.pdf` is a **502-page image scan** (no text layer). Tesseract not installed on build PC; corpus extract used. Rendered pages saved under `lab/evidence/testsuite-pro/inbox/62351-4_annexG_p*.png` for manual review.

## Deploy evidence

```
mms: AARE 62351-4 responder auth ready (cert=823 octets)   # was 771 (G.6.2 server.pem)
mms: AARE responding titles AP=1.1.1.999.1 AE=12
```

Config: `tls_own_cert: server_tls.pem`, `tls_acse_cert: server.pem` in `lab_tr400_phase1_regulation.yaml`. Regenerate: `python apps/ccli/config/tls/gen_lab_pki.py`.

## TLS unknown_ca (2026-09-29)

TSP: `ssl3_read_bytes: tlsv1 alert unknown ca` → transport closed before AARE.

**Cause:** TSP Transport TLS presents **`client.pem`** (G.6.2); DUT allow-list had only **`client_tls.pem`** (same key, different DER).

**Fix:** `tls_acse_client_cert: client.pem` in lab yaml + `mms_adapter.cpp` adds ACSE client cert to TLS allow-list when it differs from `tls_client_cert`.

## TSP retest

After deploy: reload **`C:\CCLI_tls\root_CA.pem`** in MMS Security **and** Transport TLS; **client.pem** + **client_tsp.key**; **lab_crl.pem**. **Disconnect → Connect** `CCI016_01` @ `192.168.10.1:3782`.

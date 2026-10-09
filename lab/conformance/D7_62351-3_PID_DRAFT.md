# D7 — IEC 62351-3 PID draft (PICS §8 + TLS PIXIT)

**Document ID:** CCLI-LAB-D7-PID-001  
**Revision:** 1.0  
**Date:** 2026-10-08  
**RAG source_id:** `ccli-lab-d7-62351-3-pid`  
**Basis:** `ccli-62351-3-extract` §8 Tables 1–5 · implementation `mms_adapter.cpp` TLS  
**Edition claimed:** IEC 62351-3:2014+AMD1+AMD2 (TLS 1.2). TLS 1.3 **not** claimed.

Formal **62351-100-3** cases remain a lab/P7-07 track. This sheet is the **PID fill** for quote.

Notation: **m** mandatory · **o** optional · **x** excluded · **c** CCI claim.

---

## Table 1 — Cipher suites (server)

| Suite | 62351-3 | CCI fill |
|-------|---------|----------|
| NULL / NULL_MD5 | **x** | **x** |
| NULL_SHA / NULL_SHA256 | **o** | **x** (not offered) |
| AES-GCM SHA-256 (ECDHE) | referencing std | **c** — product intent (OpenWrt TLS) |

List exact IANA names from the DUT `openssl ciphers` / wolfSSL build on the **submit** firmware.

---

## Table 2 — TLS versions (server)

| Version | 62351-3 | CCI fill |
|---------|---------|----------|
| 1.0 / 1.1 | o | **x** — `setMinTlsVersion(TLS_1_2)` |
| **1.2** | **m** | **m** |
| 1.3 | o | **o** / not claimed |

---

## Table 3 — Protocol features (server column)

| Feature | 62351-3 server | CCI fill | Notes |
|---------|----------------|----------|--------|
| Resumption ≥24 h | m | **PART** | Long-lived DSO; PIXIT may shorten for lab |
| HelloRequest resumption | m | **PART** | Stack-dependent |
| Session tickets RFC 5077 | o | **o** | |
| Renegotiation ≥24 h | m | **PART** | |
| RFC 5746 | m | **m** (stack) | |
| Trusted CA RFC 6066 | o | **o** | |

---

## Table 4 — Certificates

| Feature | 62351-3 | CCI fill |
|---------|---------|----------|
| Multiple CA roots | o | **o** (lab: one EJBCA root) |
| Max cert 8192 bytes | m | **m** |
| RFC 5280 validation | m | **m** (`chain_validation: true`) |
| CRL | m | **m** when `tls_crl` loaded |
| OCSP | o | **N/A** first pass |
| Cert white-list | o | **c** — allow-list PEMs |

---

## Table 5 — Algorithms

| Algorithm | 62351-3 | CCI fill |
|-----------|---------|----------|
| RSA 2048 | m | **m** |
| RSA 1024 | o deprecated | **x** |
| ECDSA secp256r1 | o | **o** (lab PEMs are RSA) |
| SHA-256 | m | **m** |
| SHA-1 / MD5 | o / x | **x** |

---

## TLS PIXIT (transport)

| Item | Value |
|------|--------|
| Secure port | **3782** |
| Cleartext port | **102** (bench only; not product DSO) |
| Mutual auth | Required |
| Dual cert G.2 | TLS Cert A + ACSE Cert B |
| CRL unreachable mid-session | Policy: do not drop session (62351-3 §5.6.4.4) — confirm on DUT |
| No client cert | Association **fails** |
| Revoked cert | Association **fails** if CRL loaded |

**62351-4 / 100-4** (ACSE E2E) is a **separate** PID — do not mark full 62351-4 **M** until P3-07 formal pack.

---

**RAG tags:** `lab`, `62351-3`, `PID`, `D7`, `ccli-lab-d7-62351-3-pid`

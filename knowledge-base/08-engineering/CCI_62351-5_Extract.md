# IEC 62351-5 — CCLI Engineering Extract (60870 / DNP3 security)

**Document ID:** CCLI-SEC-62351-005  
**Revision:** 1.0  
**Date:** 2026-09-21  
**RAG source_id:** `ccli-62351-5-extract`  
**Normative basis:** IEC TS 62351-5:2013 — *Security for IEC 60870-5 and derivatives*  
**Licensed PDF:** `07-protocols/regulations/standards-drop-20260918/IEC TS 62351-5-2013.pdf`  
**Capture status:** **HAVE (P0 C2)** — Batches A–B per `ccli-62351-5-capture-plan`; Batch C (DNP3) **DEFER**  
**Parents:** `ccli-62351-1-extract` · `ccli-62351-3-extract` · `ccli-60870-5-104-extract` · `ccli-62351-14-extract`  
**Programme link:** K6.5 · C2 · secured 104

---

## Summary

**62351-5** adds **application-layer authentication** to **60870-5-101/104** and **DNP3** on networked links. It does **not** replace **62351-3 TLS** — the intended stack on **104** is **TLS + 62351-5**. Serial 101/103 may use **62351-5 alone** (no TLS). **62351-14** defines syslog events **`IEC62351-5:*`** (see `ccli-62351-14-extract` Batch G).

---

## Requirements

| REQ-ID | Source | Requirement | CCLI |
|--------|--------|-------------|------|
| **REQ-355-SCOPE-001** | §1 | App-layer security for 60870-5 family | Eth_B 104 |
| **REQ-355-STACK-001** | §4 / 62351-1 §6.9 | Networked 104: **62351-3 + 62351-5** | C2 architecture |
| **REQ-355-AUTH-001** | Procedures | Mutual authentication of protocol peers | Cert or symmetric per profile |
| **REQ-355-REPLAY-001** | Mechanisms | Replay protection on app messages | Session keys / counters |
| **REQ-355-ALG-001** | Algorithms | Supported wrap/authentication algorithms negotiated | Log `*_ALG_SUP` events |
| **REQ-355-CERT-001** | PKI | Remote cert validity, expiry, revocation checks | Align 62351-9 + K6.3 |
| **REQ-355-EVT-001** | 62351-14 B.1.3 | Log STAS/SKEY/cert events Domain **IEC62351-5** | CR 6.2 |

---

## Batch A — Scope and relation to 62351-3

| Mode | Stack |
|------|-------|
| **104 / DNP3 TCP** | **62351-3 TLS** + **62351-5** app auth |
| **101 / serial** | **62351-5** only (typical) |
| **62351-5 alone** | Auth + integrity at app layer; **no encryption** |

**Rationale (62351-1):** protects against rogue apps, terminal servers, and weak site VPN boundaries.

---

## Batch B — 104-specific procedures (conceptual)

| Phase | Purpose |
|-------|---------|
| **STAS** | Start association / security context |
| **SKEY** | Session key establishment |
| **Protected ASDU** | Auth wrapper on user data |

Failure modes map to **62351-14** events e.g. `STAS_PROC_FAIL`, `SKEY_PROC_FAIL`, `REM_CERT_REVOKED` (Tables 25–26 in 62351-14 extract).

---

## CCLI mapping

| Conduit | Protocol | Stack |
|---------|----------|-------|
| **C2** | 60870-5-104 | 62351-3 + **62351-5** |
| **C3** | Modbus RTU | Document trust model (no 62351) |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-355-STACK-001 | Secured 104 lab | TLS established before STAS |
| REQ-355-EVT-001 | Force cert fail | Syslog `IEC62351-5:2.5` alarm |

---

## RAG routing

```
ccli-62351-5-extract, ccli-60870-5-104-extract, ccli-62351-3-extract, ccli-62351-14-extract
```

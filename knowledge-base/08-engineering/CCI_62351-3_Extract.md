# IEC 62351-3 — CCLI Engineering Extract

**Document ID:** CCLI-SEC-62351-005  
**Revision:** 1.0  
**Date:** 2026-08-12  
**RAG source_id:** `ccli-62351-3-extract`  
**Normative basis:** IEC 62351-3:2014+AMD1:2018+AMD2:2020 CSV — *Communication network and system security — Profiles including TCP/IP*  
**Capture status:** **HAVE (P0)** — Batch A pp. **6–19**; pp. 3–5, 20 optional  
**Parents:** `ccli-62351-3-capture-plan` · `ccli-62351-1-extract` · `ccli-62351-4-extract`  
**Programme link:** K6.5 · **C1** (:3782) · **C2** (104 TLS)

---

## Summary

**62351-3** constrains **TLS (RFC 5246)** for **SCADA/telecontrol** over **TCP/IP**: cipher suites, **TLS 1.2** default, **session resumption** and **renegotiation** for long-lived connections, **mutual certificate authentication**, CRL/OCSP handling, and **PICS** tables.

**TG-524:** Transport layer for **62351-4** on **C1 MMS**; primary TLS profile for **60870-5-104** on **C2**.

**Key refs:** **62351-9** key management · **62351-7/62351-14** security events · **RFC 5280** · **RFC 5746** renegotiation · **RFC 6066** Trusted CA.

---

## Requirements

| REQ-ID | Source | Requirement | CCLI |
|--------|--------|-------------|------|
| **REQ-3513-SCOPE-001** | §1.1 | TLS confidentiality, integrity, message auth on TCP/IP for TC 57 protocols | C1/C2 |
| **REQ-3513-SCOPE-002** | §1.1 | End-to-end between TCP peers; external bump-in-wire **out of scope** | Architecture |
| **REQ-3513-TLS-001** | §5.2 | **TLS 1.2 shall** be supported (RFC 5246) | OpenWrt policy |
| **REQ-3513-TLS-002** | §5.1 | **SHA-256 shall**; SHA-1 deprecated; NULL/weak suites disallowed | Cipher policy |
| **REQ-3513-TLS-003** | §5.3 | Session resumption **≥ every 24 h** (active) or within 24 h of session end | Permanent links |
| **REQ-3513-TLS-004** | §5.4 | Renegotiation **≥ every 24 h**; align with CRL/OCSP refresh | CR 1.8 |
| **REQ-3513-TLS-005** | §5.4 | **RFC 5746** renegotiation extension **shall** | OpenWrt/wolfssl |
| **REQ-3513-TLS-006** | §5.5 | MAC **mandatory** (not optional as in base TLS) | Integrity |
| **REQ-3513-PKI-001** | §5.6.3 | **Mutual authentication** — both certs required or terminate | Cert A (TLS) |
| **REQ-3513-PKI-002** | §5.6.2 | Max cert **≤8192 octets** | TPM profile |
| **REQ-3513-PKI-003** | §5.6.4.6–7 | **RSA ≥2048** mandatory; **1024 deprecated**; ECDSA **secp256r1** optional | CR 1.8 |
| **REQ-3513-PKI-004** | §5.6.4.4 | **CRL inaccessible shall NOT** terminate established session | Matches 62351-4 §6.2.8 |
| **REQ-3513-PKI-005** | §5.6.4.4–5 | Revoked/expired cert → refuse/terminate; **alarms** raised | CR 1.8 |
| **REQ-3513-PORT-001** | §5.7 | Referencing standard **shall** specify separate secure vs non-secure **TCP port** | C1 **3782/102** |
| **REQ-3513-EVT-001** | §4.4 | Security events → **62351-14** or **62351-7**; warning vs alarm | CR 6.2 |
| **REQ-3513-CONF-001** | §8 / Tables 1–5 | PICS declaration for lab | Lab quote |

---

## Architecture — TLS under 62351-4 (C1)

```
Z1 (CCI)                              Z0 (DSO)
     │  Eth_A — C1                          │
     │  ┌──────────────────────────────┐     │
     │  │ 62351-4 §13 E2E (native)     │     │  ← application
     │  │ 62351-3 §5 TLS 1.2  :3782    │     │  ← this document
     │  │ TCP/IP · RFC 1006 (61850)    │     │
     │  └──────────────────────────────┘     │
```

**C2 104:** **62351-3 TLS** only (+ **62351-5** app auth when required).

---

## §1 Scope (p. 6)

| Item | Content |
|------|---------|
| **§1.1** | Confidentiality, integrity, message-level authentication for SCADA/telecontrol over TCP/IP via **TLS** constraints (RFC 5246) |
| **Boundary** | Security between communicating entities at TCP connection ends |
| **Excluded** | Intervening external security devices (bump-in-the-wire) |
| **Layer** | Referenced as normative part of IEC protocols needing TCP/IP security; reflects TC 57 requirements |
| **§1.2** | Audience: protocol experts, product developers, managers |

---

## §2 Normative references (pp. 6–7)

| Reference | Use |
|-----------|-----|
| **62351-1**, **62351-2** | Intro, glossary |
| **62351-7**, **62351-9**, **62351-14** | NSM, key mgmt, cyber events |
| **ISO/IEC 9594-8 / X.509** | Certificates |
| **RFC 5246** | **TLS 1.2** |
| **RFC 5280** | PKI / CRL profile |
| **RFC 5746** | Renegotiation indication |
| **RFC 6066** | TLS extensions (Trusted CA) |
| **RFC 6176** | Prohibit SSL 2.0 |
| **RFC 4492** | ECC cipher suites |

---

## §3 Terms (p. 7)

Terms from **62351-2** apply. Additional abbreviations: **CRL**, **DER**, **ECDSA**, **ECGDSA**, **OCSP**, **PIXIT/PICS**.

---

## §4 Security issues (pp. 7–8)

### §4.1 Operational requirements

Telecontrol connections are often **permanent/long-lived** (unlike short IT sessions). Requires **session resumption** and **renegotiation** for symmetric key update. Referencing standards must specify at least one common cipher suite + TLS parameters.

Certificates = **public-key certificates** (X.509); management per **62351-9**.

### §4.2 Threats countered

TLS protects **OSI L5+ transparently**. Transport-layer countermeasures:

- Unauthorized modification/insertion → message auth + integrity
- Unauthorized access/theft → encryption (when required)

Does **not** cover application-layer (OSI L5+) security → **62351-4/5**.

### §4.3 Attack methods

| Attack | Countermeasure |
|--------|----------------|
| **MITM** | MAC / digital signatures |
| **Replay** | Processing state machines |
| **Eavesdropping** | Encryption |

### §4.4 Handling of security events *(AMD2)*

Implementations **should** announce security events. Recommended via **62351-14** cyber events or **62351-7** monitoring objects.

| Type | Meaning |
|------|---------|
| **Warning** | Raise awareness; may be safe to proceed |
| **Alarm** | Do **not** proceed |

Final handling per operator security policy.

---

## §5 Mandatory requirements (pp. 9–16)

### §5.1 Deprecation of cipher suites (p. 9)

| Rule | Detail |
|------|--------|
| **NULL encryption** | **Shall not** be used outside admin domain unless other encryption (e.g. VPN) guaranteed |
| **Integrity-only NULL suites** | `TLS_RSA_WITH_NULL_SHA` / `_SHA256` — only if encrypted by other means; **explicitly enabled** |
| **SHA-1** | Deprecated; backward compat only; disallowed next edition |
| **SHA-256** | **Shall** be supported; preferred |
| **Disallowed** | `TLS_NULL_WITH_NULL_NULL`, `TLS_RSA_WITH_NULL_MD5`, etc. |
| **No match** | **alarm: no matching TLS cipher suites** |

### §5.2 Negotiation of versions (p. 9)

| Rule | Detail |
|------|--------|
| **Default** | **TLS v1.2 shall** be supported (RFC 5246) |
| **Higher** | May be supported (TLS 1.3 — not all features in this doc) |
| **TLS 1.0/1.1** | May optionally support → **warning: insecure TLS version** |
| **Pre-TLS 1.0 / SSL** | **alarm: unsecure communication** |
| **Version change** | On renegotiation/resumption → **alarm: TLS Version change detected** |

### §5.3 Session resumption (pp. 10–11)

| Rule | Detail |
|------|--------|
| **Interval** | **At least every 24 h** for active sessions; **not later than 24 h** after session ends |
| **Relation** | `0 < resumption interval < renegotiation interval ≤ 24 h` |
| **Methods** | Session ID (RFC 5246) or **session tickets** (RFC 5077) |
| **Initiation** | Client: **ClientHello**; Server: **HelloRequest** (if policy permits both sides) |
| **CRL align** | Max resumption period aligned with CRL refresh |
| **PICS** | Parameters in referencing standard **PICS** (not PIXIT) |

**Informative example (p. 12):** CRL 24 h → renegotiation ≥12 h → resumption every **2 h**.

### §5.4 Session renegotiation (pp. 11–12)

| Rule | Detail |
|------|--------|
| **Purpose** | Full handshake; new master secret; cert validity + **revocation** re-checked |
| **Interval** | Configurable; **≥ every 24 h** for long connections; align with **CRL** or **OCSP cache** (½ refresh recommended) |
| **RFC 5746** | Renegotiation extension **shall** be used |
| **Initiation** | Client: **ClientHello**; Server: **HelloRequest** |
| **Missed renegotiation** | Client initiates if no **HelloRequest**; server terminates if no **ClientHello** response → **alarm: session renegotiation interval expired** |
| **Cipher-change timeout** | Configurable; timeout → terminate connection |

### §5.5 Message Authentication Code (p. 12)

MAC **shall** be used (mandatory here; optional in base TLS). Algorithm from negotiated cipher suite.

### §5.6 Certificate support (pp. 12–16)

#### §5.6.1 Multiple CAs

Support **>1 trust anchor**; count in PICS. Optional **RFC 6066 Trusted CA Indication** in ClientHello. No matching CA cert → **alarm: CA certificate not found**.

#### §5.6.2 Certificate size

Referencing standard specifies max size; recommendation **≤8192 octets**. Exceed → **alarm: TLS certificate size exceeded**.

#### §5.6.3 Certificate exchange

**Mutual authentication** — bi-directional cert exchange and validation. Missing cert → terminate → **alarm: certificate unavailable**.

#### §5.6.4 Public-key certificate validation

| § | Rule |
|---|------|
| **5.6.4.1** | Both nodes validate; CA-trust or individual-cert modes (configurable) |
| **5.6.4.2** | CA mode: accept from authorized CAs without per-cert config |
| **5.6.4.3** | Individual cert mode: whitelist specific certs |
| **5.6.4.4** | Revocation per X.509; **CRL check shall not terminate** established session; **CRL inaccessible shall not terminate**; revoked → refuse establishment / terminate renegotiation → **alarm: revoked certificate**; **warning: CRL not accessible/expired**; OCSP per **62351-9** |
| **5.6.4.5** | Expired → refuse/terminate → **alarm: expired certificate** |
| **5.6.4.6** | **RSA ≥2048 m**; **1024 deprecated**; **ECDSA secp256r1** optional; **brainpoolP256r1** optional; **SHA-256 m**; signature_algorithm extension shall include `{RSA,SHA-1; RSA,SHA-256; ECDSA,SHA256}` |
| **5.6.4.7** | DH/ECDHE supported; **RSA ≥2048** for key exchange; 1024-bit warnings/alarms |

### §5.7 Co-existence with non-secure traffic (p. 16)

Referencing standard **shall** provide **separate TCP port** for TLS vs cleartext.

**62351-4 mapping:** secure **3782** · non-secure **102**.

### §6 Optional security (p. 16)

Certificate **pinning / white-listing** per **62351-9** (optional).

---

## §7 Referencing standard requirements (p. 17)

Referencing standards (61850, 104, …) **shall** specify:

- Mandatory TLS cipher suites and TLS version (if not 1.2)
- Session resumption method + parameters (≤24 h)
- Session renegotiation parameters (CRL-aligned)
- Individual cert / CA trust policy
- TCP ports (secure vs non-secure)
- Max certificate size
- CRL/OCSP evaluation period and failure handling
- Security event handling
- Required conformance to this part

---

## §8 Conformance / PICS (pp. 17–19)

**Notation:** **m** mandatory · **o** optional · **x** excluded

### Table 1 — TLS cipher suites (p. 18)

| Suite | Client/Server |
|-------|---------------|
| `TLS_NULL_WITH_NULL_NULL` | **x** |
| `TLS_RSA_WITH_NULL_MD5` | **x** |
| `TLS_RSA_WITH_NULL_SHA` / `_SHA256` | **o** (integrity-only; SHA-1 deprecated) |

Referencing standard specifies additional mandatory/optional suites.

### Table 2 — TLS versions (p. 18)

| Version | Status |
|---------|--------|
| **1.0 / 1.1** | **o** (backward compat) |
| **1.2** | **m** |
| **1.3** | **o** (not all features herein) |

### Table 3 — TLS protocol features (p. 18)

| Feature | Client | Server |
|---------|--------|--------|
| Resumption ≥24 h | **m** | **m** |
| Resumption via ClientHello | **m** | **x** |
| Resumption via HelloRequest | **x** | **m** |
| Session tickets (RFC 5077) | **o** | **o** |
| Renegotiation ≥24 h | **m** | **m** |
| Renegotiation ClientHello / HelloRequest | **m** / **x** | **x** / **m** |
| RFC 5746 extension | **m** | **m** |
| Trusted CA extension (RFC 6066) | **o** | **o** |

### Table 4 — Certificate support (p. 19)

| Feature | Status |
|---------|--------|
| Multiple CA roots | **o** |
| Max cert **8192 bytes** | **m** |
| RFC 5280 validation | **m** |
| CRL revocation check | **m** |
| OCSP | **o** |
| Cert white-list (62351-9) | **o** |

### Table 5 — Cryptographic algorithms (p. 19)

| Algorithm | Status |
|-----------|--------|
| **RSA 2048** | **m** |
| **RSA 1024** | **o** (deprecated) |
| **ECDSA secp256r1** | **o** |
| **ECGDSA brainpoolP256r1** | **o** |
| **SHA-256** | **m** |
| **SHA-1** | **o** (deprecated) |
| **MD5** | **x** |

---

## CCLI conduit mapping

| Conduit | 62351-3 role | Stack |
|---------|--------------|-------|
| **C1** Eth_A | TLS **:3782** under 62351-4 E2E | 62351-3 + 62351-4 native |
| **C2** 104 | TLS transport | 62351-3 (+ 62351-5 app) |
| **C5** LTE | TLS minimum | 62351-3 |

---

## Interface matrix

| Interface | Source | Destination | Protocol |
|-----------|--------|-------------|----------|
| **C1** | TG-524 | DSO SCADA | TCP **:3782** TLS 1.2 + 61850 MMS |
| **C2** | TG-524 | DSO | TCP TLS + IEC 60870-5-104 |

---

## Security events (selected alarms)

| Event | Trigger |
|-------|---------|
| **alarm: no matching TLS cipher suites** | Handshake cipher mismatch |
| **alarm: unsecure communication** | SSL / pre-TLS 1.0 |
| **warning: insecure TLS version** | TLS 1.0/1.1 proposed |
| **alarm: TLS Version change detected** | Version change on resume/reneg |
| **alarm: CA certificate not found** | Trusted CA Indication fail |
| **alarm: TLS certificate size exceeded** | Cert > max |
| **alarm: certificate unavailable** | Missing peer cert |
| **alarm: revoked certificate** | Revoked cert on establish/reneg |
| **alarm: expired certificate** | Expired cert |
| **warning: CRL not accessible / expired** | CRL fetch issues |
| **alarm: session renegotiation interval expired** | Missed renegotiation |

---

## Knowledge gaps

| Area | Status | Gap |
|------|--------|-----|
| §1–§8 core | **HAVE** | — |
| Foreword / AMD2 intro | Not captured | pp. **3–5** optional |
| Bibliography | Not captured | p. **20** optional |
| 62351-5 | **HAVE** `ccli-62351-5-extract` | C2 app-layer auth |
| K6.3 ceremony | **HAVE** | `ccli-k63-ceremony-sop` | CR 1.8 implement on TG-524 |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-353-001 | TLS 1.0/1.1 enabled | Weak transport | Policy: **1.2 only** |
| R-353-002 | No resumption/renegotiation | Stale keys on permanent links | §5.3–5.4 timers (2 h / 12 h) |
| R-353-003 | RSA 1024 in legacy interop | Non-conformant | Table 5 — **2048 m** |
| R-353-004 | CRL unreachable terminates session | SCADA outage | §5.6.4.4 — shall **not** terminate |
| R-353-005 | Single cert for TLS + E2E | Wrong profile | 62351-4 Annex G.2 dual cert |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| Batch A §5 | TLS mandatory captured | Mapped to OpenWrt | **PASS** |
| Batch A §8 | PICS Tables 1–5 | Lab template | **PASS** |
| C1 interop | MMS-TLS :3782 | Mutual TLS + 62351-4 E2E | Lab |
| 62351-4 cross-ref | §6.2.1–7 gap closed | Extract cites §5 | **PASS** |

---

## Source pages — Batch A (pp. 6–19)

| Page | Content | Status |
|------|---------|--------|
| **6** | §1 Scope · §2 refs start | PASS |
| **7** | §2 refs · §3 terms · §4.1 start | PASS |
| **8** | §4.2–4.4 threats · events | PASS |
| **9** | §5.1 ciphers · §5.2 TLS versions | PASS |
| **10** | §5.3 session resumption | PASS |
| **11** | §5.3 tail · §5.4 renegotiation | PASS |
| **12** | §5.4 example · §5.5 MAC · §5.6.1 start | PASS |
| **13** | §5.6.3–5.6.4.4 revocation | PASS |
| **14** | §5.6.4.4–5 expired · §5.6.4.6 signing start | PASS |
| **15** | §5.6.4.6–7 RSA/ECDSA/SHA | PASS |
| **16** | §5.6.4.7 key exchange · §5.7 ports · §6 optional | PASS |
| **17** | §7 referencing · §8.1–8.2 | PASS |
| **18** | Tables 1–3 | PASS |
| **19** | Tables 4–5 | PASS |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-08-12 | **Batch A** pp. 6–19: §1–§8 + Tables 1–5 |

---

## RAG cross-links

| source_id | Document |
|-----------|----------|
| `ccli-62351-3-extract` | This document |
| `ccli-62351-3-capture-plan` | Capture batches |
| `ccli-62351-1-extract` | Series intro |
| `ccli-62351-4-extract` | MMS/TLS ports + E2E |
| `ccli-62351-9-extract` | PKI ceremony · CRL/OCSP |
| `ccli-62443-4-2-extract` | CR 1.8 PKI |
| `ccli-62443-zones-extract` | C1/C2 conduits |

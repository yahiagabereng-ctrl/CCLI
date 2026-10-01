# IEC 62351-9 — CCLI Engineering Extract

**Document ID:** CCLI-SEC-62351-009  
**Revision:** 1.3  
**Date:** 2026-08-12  
**RAG source_id:** `ccli-62351-9-extract`  
**Normative basis:** IEC 62351-9:2023 — *Power systems management and associated information exchange — Data and communications security — Part 9: Cyber security key management for power system equipment*  
**Capture status:** **HAVE** — P0 complete · **§8 GDOI (P2)** pp. **75–102** *(pp. **82–90** partial)*  
**Parents:** `ccli-62351-9-capture-plan` · `ccli-62351-3-extract` · `ccli-62351-4-extract`  
**Programme link:** K6.3 · **CR 1.8 PKI** · K6.5 dual cert

---

## Summary

**62351-9:2023** specifies **cryptographic key management** for IEC TC 57 protocols, focused on **long-term asymmetric keys** (public-key certificates + private keys). Pairwise symmetric session keys are managed in **62351-3/4/5**; **group keys** (GOOSE/SV/PTP) via **GDOI** (**§8**, captured pp. **75–102** — **P2** for C1 MMS).

**TG-524:** **K6.3 key ceremony** source — RA/CA, **IDevID/LDevID**, **SCEP/EST** enrolment, **CRL/OCSP/SCVP** revocation, **Annex C** enrolment/renewal flows, **Annex D → 62351-14** event IDs. Supports **62351-3 §5.6** and **62351-4 Annex G** dual certs.

**Key refs:** **X.509:2020** · **RFC 6960** OCSP · **RFC 7030** EST · **RFC 8894** SCEP · **RFC 4210** CMP · **IEEE 802.1AR** IDevID/LDevID · **RFC 8366** vouchers.

---

## Requirements

| REQ-ID | Source | Requirement | CCLI |
|--------|--------|-------------|------|
| **REQ-3519-SCOPE-001** | §1 | Long-term **asymmetric** key mgmt (certs + private keys); foundation for 62351 services | CR 1.8 |
| **REQ-3519-SCOPE-002** | §1 | Pairwise symmetric keys → **62351-3/4/5**; group keys → **62351-6/9 §8** | Stack split |
| **REQ-3519-PKI-001** | §5.8.1 | Cert mgmt requires **proof of identity** + **proof of possession** | K6.3 |
| **REQ-3519-PKI-002** | §5.8.2–3 | **IDevID** (manufacturer) → **LDevID** (operational domain) per **802.1AR** | TPM provisioning |
| **REQ-3519-PKI-003** | §5.8.5 | Enrolment via **PKCS#10 CSR**; entity generates key pair | On-device key gen |
| **REQ-3519-PKI-004** | §5.8.6 | Enrolment protocols: **SCEP**, **EST**, **CMP**, **CMC**; trust anchors via **TAMP** | OpenWrt policy |
| **REQ-3519-PKI-005** | §5.7.4.4 | External key gen allowed; distribute OOB; **PEM/PKCS#8/PKCS#12** | Factory ceremony |
| **REQ-3519-REV-001** | §5.9.1 | **CRL** — revoked certs invalid; timely update; **NTP/PTP** for timestamps | CR 1.8 |
| **REQ-3519-REV-002** | §5.9.2 | **OCSP** (RFC 6960); caching + **stapling** (62351-3 uses stapling) | C1 TLS |
| **REQ-3519-REV-003** | §5.9.2 / Fig 16 | **Proxy OCSP** at station controller; CRL fetch from backend | TG-524 as proxy |
| **REQ-3519-REV-004** | §5.9.2 | OCSP timeout ~**15 s**; failure → alarm; may assume **not revoked** (availability) | Matches 62351-3 |
| **REQ-3519-REV-005** | §5.9.3 | **SCVP** (RFC 5055) — delegate path validation to master node | Optional |
| **REQ-3519-REV-006** | §5.9.4 | Post-compromise: local technician procedure; remote re-enrol via LDevID/IDevID | Incident response |
| **REQ-3519-ATTR-001** | §5.7.5–6 | **Attribute certificates** for RBAC; **62351-8** `IECUserRoles` extension | RBAC layer |
| **REQ-3519-NORM-001** | §7.2 / Table 1 | Public-key cert components per **X.509:2020**; extensions per Table 1 | Lab PICS A-1 |
| **REQ-3519-NORM-002** | §7.3.2 | **PKCS#12 shall** be supported; private key protected in TPM/secure store | CR 1.8 · TPM |
| **REQ-3519-NORM-003** | §7.3.6 | **≥5 CA trust anchors** shall be supported | OpenWrt trust store |
| **REQ-3519-NORM-004** | §7.3.7 | **EST preferred**; **SCEP** backward compat; enrolment over **62351-3 TLS** recommended | K6.3 SOP |
| **REQ-3519-NORM-005** | §7.4.4.4 | **RSA ≥2048 m**; **ECDSA secp256r1 m**; 1024 deprecated | Matches 62351-3 |
| **REQ-3519-NORM-006** | §7.4.4.10.7 | TLS certs: **serverAuth** / **clientAuth** EKU mandatory | Cert A/B profiles |
| **REQ-3519-NORM-007** | §7.4.6 / Table 6 A-12 | End entity **shall** support **CRL or OCSP** (≥1) | C1 revocation |
| **REQ-3519-NORM-008** | §7.7 / Table 6 A-14 | **Clock sync m** for cert validity; **PTP 1588v2.1** or **NTS** recommended | GNSS on TG-524 |
| **REQ-3519-NORM-009** | §7.6 / Table 6 A-13 | Renew before expiry; configurable auto-renewal policy | K6.3 |
| **REQ-3519-CONF-001** | §9 / Tables 5–6 | PICS declaration — general + asymmetric | Lab quote |
| **REQ-3519-EVT-001** | §6.2 | Security events → **62351-14** and/or **62351-7**; severity notice/warning/error/alarm | CR 6.2 |
| **REQ-3519-EVT-002** | Annex D | Event IDs **IEC 62351-9:1.x–5.x** with mnemonics for logging | Lab SIEM |
| **REQ-3519-RNG-001** | §6.4 | RNG per **ISO/IEC 18031**; OOB seed if low entropy | TPM/TRNG |
| **REQ-3519-K63-001** | Annex C | Enrolment flow: register → configure → CSR → verify → store | K6.3 SOP |
| **REQ-3519-K63-002** | Annex C.2 | Renewal at **70%** of cert lifetime (informative state machine) | OpenWrt timer |
| **REQ-3519-PKI-006** | §5.8.4 | Enrolment scenarios: OTP · IDevID · self-signed · expired/revoked entity or CA | K6.3 |
| **REQ-3519-ZTP-001** | §5.8.3/p.35 | **RFC 8572 SZTP** · **RFC 8995 BRSKI** · **RFC 8520 MUD** for zero-touch onboarding | Factory |
| **REQ-3519-GDOI-001** | §8.1 | **GDOI** (**RFC 6407** + **RFC 8052**); **GROUPKEY-PULL m** · **GROUPKEY-PUSH o** | P2 GOOSE/SV |
| **REQ-3519-GDOI-002** | §8.2 Table 3 | KDC IKEv1: AES-CBC · SHA2-256 · RSA-2048 · MODP ≥2048; UDP **848** | P2 |
| **REQ-3519-GDOI-003** | §8.3 | IKEv1 **main mode m**; **aggressive mode prohibited**; GM=initiator, KDC=responder | P2 |
| **REQ-3519-GDOI-004** | §8.5.7 | **61850 SA TEK** payload; **HMAC-SHA256-128 m** auth | 62351-6 |
| **REQ-3519-GDOI-005** | §8.5.8 | **61850-9-3 PTP** integrity via GDOI + AuthenticationTLV | GNSS profile |
| **REQ-3519-GDOI-006** | §8.7.2 | KDC unreachable → prolong TEK + warning event; missing TEK → GROUPKEY-PULL | Ops policy |

---

## Architecture — PKI in C1 stack

```
Manufacturer                Field (TG-524 CCI)
     │                              │
     │  IDevID (802.1AR)            │  LDevID (operational CA)
     │  CSR → SCEP/EST              │  TLS cert (62351-3) + E2E cert (62351-4 G)
     ▼                              ▼
  Mfg CA ──trust anchor──►  DSO CA ──► CRL/OCSP ◄── substation proxy (Fig 16)
```

**Dual cert model (62351-4 Annex G.2):** separate end-entity certs for **TLS :3782** and **E2E §13** — both issued under **62351-9** ceremony.

---

## §1 Scope (p. 10)

| Item | Content |
|------|---------|
| **Focus** | **Long-term asymmetric** keys (public-key certs + private keys); foundation for IEC 62351 services (Annex A) |
| **Symmetric** | Session keys for **group communication** only (62351-6); pairwise session keys in **62351-3/4/5** |
| **62351-3** | TLS — profiles TLS options |
| **62351-4** | Application-layer E2E security |
| **62351-5** | 60870-5-101/104, DNP3 app security |
| **62351-6/9 §8** | Group keys via **GDOI** — GOOSE, SV, PTP parameters |
| **Policy** | Org selects key types + algorithms; this part specifies **management techniques** only |
| **Events** | Defines security events for error conditions; org response beyond scope → security policy |
| **PQC** | Future post-quantum consideration; no specific measures yet |

---

## §2 Normative references (p. 11)

| Reference | Use |
|-----------|-----|
| **62351-2** | Glossary |
| **62351-3** | TCP/IP TLS profiles |
| **62351-4** | MMS security |
| **62351-5** | 60870-5 / derivatives |
| **62351-6** | 61850 group security |
| **62351-14** | Cyber security event logging |
| **ISO/IEC 9594-8 / X.509:2020** | Public-key + attribute cert frameworks |
| **ISO/IEC 9594-11 / X.510** | Secure directory operations |
| **ISO/IEC 9834-1 / X.660** | Object identifiers |
| **RFC 5272** | CMC |
| **RFC 5755** | Attribute certificate profile |
| **RFC 5934** | **TAMP** — trust anchor management |
| **RFC 6407** | GDOI |
| **RFC 6960** | **OCSP** |
| **RFC 7030** | **EST** |
| **RFC 8052** | GDOI support for IEC 62351 |

*Footnotes:* 62351-3 and 62351-14 were under preparation at publication (RFDIS/ACDV 2023).

---

## §5.7 PKI and PMI (pp. 30–33)

### §5.7.1 General — Figure 6

PKI manages certificates over entity lifecycle. Three phases:

| Phase | Activities | Automation examples |
|-------|------------|---------------------|
| **Registration & enrolment** | Human (ID card) or device (vendor) → LRA → RA | SCEP, EST, CMP, CMC |
| **Certification & revocation** | RA ↔ CA; central key gen if low entropy; CRL + repository | CRL, OCSP, SCVP |
| **Distribution & fetching** | Key distribution; RA/LRA | LDAP, HTTP |

### §5.7.2 Registration authority (RA)

Entities prove identity to RA:

- **Humans:** official ID documents
- **Devices:** manufacturer-issued digital certs or **OTP**
- RA may be separate or co-located with CA

### §5.7.3 Certification authority (CA)

- Entity or manufacturer generates **CSR** with public key
- CA verifies CSR, issues signed certificate → trusted public/private key binding
- Trust in entity key = trust in CA key (§5.7.4)

### §5.7.4 Public-key certificates

| § | Rule |
|---|------|
| **5.7.4.1** | Binds identity to public key; CA or end-entity cert; extensions via OIDs; install only trusted CA roots |
| **5.7.4.2** | Short-lived certs may skip revocation; limited utility in power industry |
| **5.7.4.3** | **Renewal** before expiry — simplified re-enrolment using existing cert |
| **5.7.4.4** | External key gen if insufficient RNG/compute; **OOB** distribution; **PEM/PKCS#8/PKCS#12**; **Figure 7** central generation |

**Figure 7 — Central certificate generation:**

1. Offline: generate key pairs per device
2. Offline: generate certificate (CA co-located with engineering tool)
3. Online: distribute key material to field devices

Manufacturer credential (if known at CA) can secure distribution.

### §5.7.5 Attribute certificates — Figure 8

- **PKI** = identity (public-key cert); **PMI** = authorization (attribute cert)
- Attribute cert temporarily enhances permissions (RBAC) — used in **62351-8**
- Linked to public-key cert via serial number / holder reference
- Short-lived attribute certs: include **no revocation information** extension (X.509)

### §5.7.6 Extensions

- Extensions: OID + critical/non-critical flag
- Critical unsupported → cert **invalid**
- **62351-8:** `IECUserRoles` extension for power-system RBAC

---

## §5.8 Certificate management (pp. 33–42)

### §5.8.1 Certificate management process

Repeated on: creation, ownership/use change, approaching expiry.

**Mandatory properties:**

| Property | Mechanism |
|----------|-----------|
| **Proof of identity** | Device info + OTP; or existing cert + private key |
| **Proof of possession** | Sign CSR (**PKCS#10**); RA verifies signature |

Both required for authorized issuance.

### §5.8.2 Initial certificate creation — IDevID

- At manufacturer: register with RA → cert from manufacturer CA
- Verified against manufacturer **trust anchor**
- **IEEE 802.1AR IDevID triplet:**
  1. Initial device certificate
  2. Corresponding private key
  3. Chain to manufacturer trust anchor

### §5.8.3 Onboarding — LDevID

Introduce device to operational domain:

- Device gets domain trust anchor + operational cert/key
- **LDevID triplet:** local cert + private key + chain to local issuer root
- Multiple LDevIDs possible

| Method | Description |
|--------|-------------|
| **Manual** | Vendor delivery info → domain configures trust into device |
| **Automated (zero-touch)** | IETF autonomous networking protocols |
| **RFC 8366** voucher | Signed container with new owner domain cert; device validates with manufacturer root + IDevID |

**Zero-touch / segmentation (p. 35, continuation of §5.8.3):**

| RFC | Role |
|-----|------|
| **8572 SZTP** | Secure Zero Touch Provisioning — factory-default devices; builds on vouchers (**8366**) for domain trust + operational cert enrolment |
| **8995 BRSKI** | Bootstrapping Remote Secure Key Infrastructure — manufacturer voucher → domain cert → **LDevID** triplet (private key + local cert + chain to domain trust anchor) |
| **8520 MUD** | Manufacturer Usage Description — expected communication behaviour for ACL refinement and network segmentation |

### §5.8.4 Enrolment of an entity (p. 35)

**Enrolment** = entity receives a **signed certificate from the CA** in the operational environment.

| Starting credential | Scenario |
|---------------------|----------|
| **OTP** (no cert) | One-time password to RA |
| **CA-issued cert** | e.g. manufacturer **IDevID** |
| **Authorized self-signed** | Pre-registered fingerprint |
| **Expired/revoked entity cert** | **IDevID** or **LDevID** re-enrol |
| **Expired/revoked CA cert** | Trust anchor rotation + re-enrol |

**Protocol selection (§5.8.6):**

| Protocol | Power-system profile |
|----------|----------------------|
| **SCEP** | OTP as activation code |
| **EST** | Username/password **or** existing key pair (manufacturer-imprinted **IDevID**) |
| **CMP** | Feature-rich PKI operations |
| **CMC** | With CMP — better offline + revocation support |

**CCLI K6.3:** TG-524 field enrolment = **EST** over **62351-3 TLS** with **IDevID**-backed CSR; dual cert = two enrolment cycles (Annex G).

### §5.8.5 CSR processing — Figures 11–13

**Enrolment process (§5.8.5.1):**

1. Entity generates key pair
2. Entity generates **CertificationRequestInfo** (**PKCS#10**): subject DN, public key, optional attributes; signed with private key
3. Send to RA/CA via enrolment protocol (§5.8.6)
4. RA validates signature
5. CA issues cert (DN, public key, issuer, serial, validity, extensions, signature)
6. Entity verifies CA signature; stores **.der/.pem/.cer/.crt**

Manual admin approval may be required. Auth via **IDevID** or pre-shared OTP to RA.

**PKCS#10 structure (§5.8.5.2, Figure 12):**

Version · Subject · SubjectPublicKeyInfo · Attributes · Signature (proves private key possession)

Key attributes (RFC 2985): `challengePassword`, `extensionRequest`

**CRMF (§5.8.5.3, Figure 13, RFC 4211):**

Cert request ID · Certificate template · Controls · Proof of possession · Request information

### §5.8.6 Enrolment protocols

| Protocol | RFC | Key points |
|----------|-----|------------|
| **SCEP** | **8894** | CMS + PKCS#10 over HTTP; client-side key gen only; auth via self-signed cert + challenge password or CA-issued cert; RSA encrypt to recipient pubkey; ECDSA encrypt via challenge password |
| **EST** | **7030** | TLS channel; client **and** server key gen; `/.well-known/est/cacerts`, `csrattrs`, `simpleenroll`; CSR bound to TLS session (`tls-unique`); PoP in CMC portion; terminate at RA or CA; RFC 8951 clarifications |
| **CMP** | **4210** | CRL, cert request, ID/auth, PoP, cross-cert, revocation; client/server key gen; CRMF + PKCS#10; HTTP transport RFC 6712; lightweight industrial profile planned |
| **CMC** | **5272** | CMS + PKCS#10 + CRMF; simple + full PKI handshake; client/server key gen |

**Figure 9 — SCEP flow:** Entity → PKCSReq → RA (validate, unwrap, verify manifest/password) → CA → CertRep → Entity

**Figure 10 — EST flow:** TLS to RA → optional `cacerts` → TLS + imprinted client cert → optional `csrattrs` → PKCS#10 with `tls-unique` → `simpleenroll` → store cert

**Pre-config required:** device IP, RA/CA addresses, trust anchors (manual or §5.8.3 onboarding).

### §5.8.7 Trust Anchor Management Protocol (TAMP)

- **RFC 5934** — manage root/trust anchors in device trust store
- CMS-based; transport-independent

---

## §5.9 Revocation (pp. 42–47)

### §5.9.1 Certificate revocation lists (CRL)

- List of revoked serial numbers + revocation timestamp + CA signature; optional reason code
- Revoked certs **shall not** be relied upon
- Update whenever cert revoked; distribute timely
- **Time sync required:** **NTPv4** (RFC 5905) or **PTP** IEEE 1588v2.1 (§7.7)
- **Figure 14 — CRL structure:** issuer, thisUpdate, nextUpdate, revokedCertificates[], signature
- CRL growth problematic for embedded systems → partition or use OCSP

**Revocation reasons (X.509 §9.5.3.1):**

1. Private key compromised  
2. CA private key compromised  
3. Affiliation changed  
4. Certificate superseded  
5. Cessation of operation  
6. Certificate hold  
7. Privilege withdrawn  
8. Attribute authority private key compromised  

### §5.9.2 Online Certificate Status Protocol (OCSP)

- **RFC 6960** — alternative to CRL fetch
- Request: version, service, cert ID, extensions, **nonce** (anti-replay)
- Response: signed **good** / **revoked** / **unknown**
- **Figure 15:** Verifier → OCSP responder → signed status
- Field connectivity challenges → **OCSP caching** + **stapling**
- **62351-3 utilizes OCSP stapling**
- **Hybrid CRL + OCSP (Figure 16):** station controller fetches CRL periodically; acts as **proxy OCSP responder** for local IEDs (61850, 104, TASE.2)

**Operational rules (p. 45):**

| Rule | Detail |
|------|--------|
| **Clock** | Trusted real-time clock for OCSP timestamps |
| **Timeout** | Configurable (e.g. **15 s**); no response → **OCSP failure alarm**; may assume **not revoked** if availability priority |
| **Proxy** | Must have cert + private key; CA-signed; configured as trusted responder |
| **Chaining** | OCSP requests chain between peer responders to locate CA |

**Figure 17 — OCSP call flows:**

| Mode | Actor | Flow |
|------|-------|------|
| **A** Proactive client | Client | OCSP pre-fetch → connection incl. cert + OCSP response |
| **B** Proactive server | Server | OCSP pre-fetch → response incl. cert + OCSP |
| **C** Reactive client | Client | Connection → then OCSP check |
| **D** Reactive server | Server | Connection with cert → server OCSP check |

- **TLS 1.2 RFC 6066:** server stapling in `serverHello`
- **TLS 1.3 RFC 8446:** client-side stapling also
- Validation failure: generally continue after OCSP alarm (DoS mitigation)

### §5.9.3 Server-based certificate validation protocol (SCVP)

- **RFC 5055** — delegate cert path construction/validation to master node
- Compatible with CRL or OCSP backend
- **Figure 18:** Verifier → SCVP responder → OCSP backend → signed validation response
- Keep CA hierarchies **flat and compact** for performance

### §5.9.4 Recovering from certificate revocation

Depends on revocation reason + org security policy + issuer CPS:

| Scenario | Action |
|----------|--------|
| **Physical compromise** | Local technician procedure; change cert + key; verify device integrity |
| **Remote renewal** | Re-enrol using **LDevID** or **IDevID** (if not revoked); auto-check revocation + trigger enrolment; secure element storage |
| **Admin channels** | HTTPS, SNMP, SSH, OOB credentials |
| **Physical** | Dedicated button → re-bootstrap |

### §5.10 Trust via self-signed certificates (p. 47 start)

Self-signed **root/trust anchors** distribute trust in PKI hierarchies (§5.7.4).

---

## §7 Asymmetric key management (normative) — Batch B (pp. 51–72)

### §7.1 General (p. 51)

Asymmetric key material = public key + private key + issuing CA info (trust anchors / chain). Covers key-pair generation on end entities, PKI certification, and lifecycle management. References **IEEE 802.1AR DevID**. Certificates per **ISO/IEC 9594-8 / X.509:2020**.

### §7.2 Certificate components (pp. 51–53)

#### Table 1 — Public-key certificate components (pp. 51–52)

| Component | Support | Notes |
|-----------|---------|-------|
| Version | **M** | X.509v3 (§7.4.4.2) |
| Serial Number | **M** | Unique per CA |
| Signature | **M** | OID per §7.4.4.4 |
| Issuer | **M** | Issuing CA DN |
| Validity | **M** | notBefore/notAfter |
| Subject | **M** | Entity identity |
| Subject Public Key Info | **M** | Algorithm + public key |
| Authority Key Identifier | **M** | §7.4.4.10.2 |
| Subject Key Identifier | **C1** | **Shall** in CA certs |
| Key Usage | **M(c)** | Critical; §7.4.4.10.6 |
| Extended Key Usage | **C** | TLS/OCSP/AVL per use case |
| Certificate Policy | **O** | OID |
| Subject Alternative Name | **C** | TLS server: dNSName/IP required |
| Basic Constraints | **C(c)** | CA certs; pathLenConstraint |
| CRL Distribution Point | **C2** | HTTP/LDAP; CA responsibility |
| Authority Information Access | **C2** | OCSP `id-ad-ocsp` |
| Role-based Access control | **C3** | **62351-8 Profile A** end entity |
| Authorization Validation | **O(c)** | AVL check; critical if used |
| SOA identifier | **C4** | AA certificates |

**Notation:** **M** mandatory · **O** optional · **C** conditional · **(c)** critical — reject if unsupported

**C2 note:** CA may issue short-term certs without CRLDP/OCSP; infrastructure shall support both CRL and OCSP; both extensions may be provided.

#### Table 2 — Attribute certificate components (p. 53)

| Component | Support |
|-----------|---------|
| Version, Holder, Issuer, Signature, Serial Number, attributes, attrCertValidityPeriod | **M** |
| Authority Key Identifier | **O** |
| CRL Distribution Point / AIA / noRevAvail | **C1** mutually exclusive |

Attribute certs in IEC 62351 scope: **RBAC only** (62351-8).

### §7.3 Certificate generation and installation (pp. 53–58)

#### §7.3.1 Key generation (pp. 53–54)

Entity shall possess asymmetric key pair. Generate locally **or** receive externally generated pair + cert from CA.

**New key pair required when:**

- No key at startup / cert expired
- Change in controllership (ownership, control, reconfiguration)
- Command from authorized entity (renewal)
- Private key compromised
- IP address or FQDN changed

**Strongly recommended:** client-side (on-device) key generation.

#### §7.3.2 Cryptographic key protection (p. 54)

- Private keys + long-term symmetric keys protected against unauthorized modification, retrieval, cloning
- Transport: encrypt with transport key — **PEM (RFC 7468)**, **PKCS#8**, **PKCS#12 (RFC 7292)**
- **PKCS#12 shall** be supported; include issuing (sub-)CA certs in container
- Events: `warning:PKCS #12 format mismatch` · `warning:PKCS#8 format mismatch`
- Compromise attempts: log + alarm
- Guidance: NIST SP 800-57, FIPS 140-2, IEEE 1686, IEEE C37.240, **62443-4-2**

#### §7.3.3 Existing PKI (p. 54)

Existing PKI allowed if complete chain to root + required enrolment protocols (§7.3.7). Install only necessary root CAs per security policy.

#### §7.3.4 Certificate policy (p. 54)

Strongly recommended: certificate policy per **RFC 3647**.

#### §7.3.5 Entity registration (p. 55)

All entities registered with ≥1 RA (may co-locate with org-approved CA). Manual or automatic (scripts, engineering positive lists).

**Registration data (≥1):**

- Entity subject / unique ID (`serialNumber` / `X520SerialNumber`)
- **OTP** for SCEP authentication
- Manufacturer cert serial + issuer
- Certificate fingerprint
- Trusted issuing CA for enrollment

#### §7.3.6 Entity configuration (p. 55)

- CA trust anchor certificate(s)
- **Shall support minimum 5 CA trust anchors** (aligns with 62351-3)
- RA IP or FQDN (e.g. `IEC62351.LocalCA`)
- Subject with device unique ID (CN, etc.)
- CSR timeout: local issue; re-enroll if no cert received

#### §7.3.7 Entity enrolment (pp. 56–57)

**General (§7.3.7.1):**

- Auth: OTP or manufacturer cert
- Entity generates key pair + CSR per org policy
- Online RA/CA connection unless OOB
- Only registered entities (§7.3.5)
- Manual: PKCS#10 to RA; RA verifies PoP signature
- Automated: infrastructure **shall** support **SCEP (RFC 8894)** + **EST (RFC 7030, preferred)**
- Client may support ≥1 protocol
- Enrolment TLS: **62351-3 profile strongly recommended**

**SCEP (§7.3.7.2, p. 57):** RFC 8894 mandatory — `GetCACaps`, `GetCACert`, `PKCSReq`, `RenewalReq`

- Success: `notice:enrolment using SCEP successfully performed`
- Failure: `error:enrolment using SCEP cancelled with error.`

**EST (§7.3.7.3, p. 57):** RFC 7030 + mandatory **csrattrs**

- `/cacerts` — trust anchor build/update
- `/simplerenroll` — simple enrol + re-enrol/renewal
- Optional: `/fullcmc`
- Optional: `csrattrs` for RA/CA algorithm requirements
- RFC 8951 transfer encodings
- Success: `notice:enrolment using EST successfully performed`
- Failure: `error:enrolment using EST cancelled with error.`

#### §7.3.8 Trust anchor update (p. 58)

Automated update: **EST `/cacerts`** from existing trust anchor. Optional: **TAMP (RFC 5934)** — Status Query/Response, Trust Anchor Update/Confirm, Apex Trust Anchor Update/Confirm, TAMP Error.

### §7.4 Certificate verification (pp. 58–69)

#### §7.4.1–7.4.2 General / format (p. 58)

All certs verified by relying party per **X.509:2020**. **ASN.1 DER** structure validated.

- Decode error: `warning: certificate format mismatch. Failed verification.`
- Self-signed: recommend AVL validation (§7.8)

#### §7.4.3 Signature verification (p. 59)

- Pub-key sig fail: `alarm:public-key certificate signature could not be verified`
- Attribute cert sig fail: `alarm:attribute certificate signature could not be verified`

#### §7.4.4 Public-key certificate components (pp. 59–66)

| § | Rule / event |
|---|--------------|
| **7.4.4.2 Version** | Shall be **2** (v3); else `error:wrong certificate version error.` |
| **7.4.4.3 Issuer** | Untrusted issuer → `error:public-key certificate issuer not trusted.` |
| **7.4.4.4 Signature** | **RSA m** (≥2048); **ECDSA m** (`ecdsa-with-SHA256`, **secp256r1**); **EdDSA o** (agility); 1024 deprecated; 3072+ recommended; unsupported → `error:unsupported signature algorithm.` |
| **7.4.4.5 Validity** | Expired → `error:public-key certificate expired.`; not yet valid → `error:certificate not yet valid.`; UTCTime until 2049; GeneralizedTime from 2050; no expiry = `99991231235959Z`; invalid → refuse (62351-3 TLS) |
| **7.4.4.6 Subject** | Missing/unknown → `warning: subject not included…`; validate CN, O, OU, C, **serialNumber** (802.1AR); RBAC exception in 62351-8 Profile A |
| **7.4.4.7 SubjectPublicKeyInfo** | Unsupported alg → `error: native public key algorithm…not supported` |
| **7.4.4.8 Unique IDs** | issuerUniqueID / subjectUniqueID **shall be absent** (deprecated) |
| **7.4.4.9 Path validation** | Trust anchor to end entity; missing anchor → `error:trust anchor not supported.`; path fail → `error:certification path could not be verified.` |
| **7.4.4.10 Extensions** | Critical unsupported → reject + events; see sub-clauses below |

**Extension requirements (selected):**

| Extension | TLS / PKI rule |
|-----------|----------------|
| **authorityKeyIdentifier** | Required; missing → `error: authority key identifier not included…` |
| **subjectKeyIdentifier** | Required in CA cert |
| **subjectAltName** | TLS server: ≥1 dNSName or IPAddress; missing → `error:subject alternative name not included.` |
| **basicConstraints** | CA: critical, cA=true, pathLenConstraint; min support **pathLenConstraint ≥2** |
| **keyUsage** | TLS client: **digitalSignature**; server: digitalSignature and/or keyEncipherment; CA: **keyCertSign**; CRL: **cRLSign** |
| **extendedKeyUsage** | TLS server: **serverAuth**; client: **clientAuth**; OCSP: **OCSPSigning** (1.3.6.1.5.5.7.3.9); AVL: **id-av1Sign** (2.5.38.2) |
| **CRL Distribution Point** | HTTP GET **m** default; LDAP **o**; missing when CRL required → `warning: CRL distribution point not contained…` |
| **authorityInfoAccess** | OCSP `id-ad-ocsp`; missing → `warning: OCSP responder information not contained…` |
| **IECUserRoles** | 62351-8 RBAC |
| **sOAIdentifier** | AA certs from trusted CA |

#### §7.4.5 Attribute certificate verification (pp. 66–69)

Holder via `baseCertificateID` or `entityName`; issuer AA trusted by config or CA-issued with SOA + digitalSignature.

Events: holder verify fail → warnings; issuer untrusted → errors; expired/not valid; critical extension errors.

**noRevAvail:** `notice:no revocation information intended in attribute certificate`

#### §7.4.6 Certificate revocation status (pp. 69–70)

Positive revocation: `alarm:certificate has been revoked.`

**CRL events:** `warning:CRL distribution point not accessible` · `warning:CRL expired` · `warning:CRL signature could not be verified`

**OCSP events:** `warning:OCSP responder not accessible` · `warning:OCSP responder connection timeout` · `warning:Certificate not known to OCSP responder` · `warning:OCSP response expired` · `warning:OCSP response signature could not be verified`

Revoked certs shall not be accepted (62351-3 TLS establishment).

### §7.5 Certificate revocation (p. 70)

Revoke when: private key compromised · CA compromised · affiliation changed · end of life (+ org policy reasons).

PKI shall support **CRL** and **OCSP**; propagate ≥ every **24 h**. Entities support CRL, OCSP, or both.

### §7.6 Expiration and renewal (p. 71)

No fixed min/max lifespan — per cert type + local policy. Optional private key usage period extension.

- Generate new key pair + CSR before expiry
- Log successful and failed renewals
- Configurable: auto-renewal on/off; lead time before expiry

### §7.7 Clock synchronization and accuracy (p. 72)

Required for reliable cert validity verification.

| Protocol | Recommendation |
|----------|----------------|
| **IEEE 1588v2.1 PTP** | Use integrated security; 61850-9-3 / C37.248 profile |
| **NTP** | Consider **NTS (RFC 8915)** |
| Other | May secure time sync by other means |

**CCLI:** GNSS → PTP/NTP on TG-524; aligns with 62351-4 **10 min** skew tolerance.

### §7.8 Authorization and validation lists (p. 72 start)

Optional AVL support per **X.509:2020** + **X.510:2020**. Protocols: **AVMP** (cl. 13), **CASP** (cl. 14). `TBSCertAVL`: serialNumber, constrained=FALSE, entries per cert/group.

### §7.8.6–7.8.8 AVL operations (p. 75)

| § | Content |
|---|---------|
| **7.8.6** | Pub-key cert extensions for AVL use: authorization/validation ext (7.4.4.10.8), EKU for issuing AVLs (7.4.4.10.7). **2017 AVL distribution point ext deprecated** (7.4.4.10.12) — distribution point now operator-managed |
| **7.8.7** | Operator issues AVL from engineering data; may align with cert enrolment; update on component add/remove/exchange. Procedures: **ISO/IEC 9594-8:2020 cl. 21** |
| **7.8.8** | Endpoint verify cert + local AVL at connection establishment — **9594-8:2020** |

**IP address encoding (AVL entries, p. 75 tail):** `iPAddress` octet string network byte order — **4 octets** (IPv4) · **16 octets** (IPv6).

---

## §8 Group-based key management — GDOI (pp. 75–102, **P2**)

*Normative for **GOOSE/SV/PTP** group keys via **62351-6**; **not C1 MMS P0**. TG-524 relevance: **PTP/GNSS** profile cross-ref §8.5.8.*

### §8.1 GDOI requirements (p. 75)

| Requirement | Detail |
|-------------|--------|
| Method | **GDOI** (**RFC 6407**) + **RFC 8052** (IEC 62351 GDOI support) |
| **GROUPKEY-PULL** | **Mandatory (m)** |
| **GROUPKEY-PUSH** | **Optional (o)** — rekey + membership control; ACK per **RFC 8263** |
| Auth | **X.509 pub-key cert credentials** at connection establishment **mandatory** |
| KDC | Secure key repository; session-key renewal policy; "public key" = cert public key + private signing key |

### §8.2 IKEv1 — Table 3 KDC requirements (p. 76)

| Attribute | Values |
|-----------|--------|
| Exchanges | **2-Main Mode**, **5-Informational** |
| GM ID type | **9 — ID_DER_ASN1_DN** (Subject of identity cert) |
| Port | UDP **848** (configurable; IANA GDOI) |
| Encryption | **7-AES-CBC** 128/256 |
| Hash | **4-SHA2-256**, 5-SHA2-384, 6-SHA2-512 |
| Auth method | **3-RSA Signatures (m)**; 2-DSA; 9-ECDSA P-256 (**RFC 4754**) |
| DH groups | **14–18 MODP** (2048–8192); **MODP-1024/1536 deprecated** |
| Mandatory set | AES-CBC-128 · SHA2-256 · RSA-2048 · DH ≥2048 |

**DH policy:** KDC **shall** support groups **14–18** (14 backward-compat only per BSI/NIST). GM **shall** 16–18; may optionally 14–15.

### §8.3 Phase 1 IKEv1 main mode (pp. 77–80)

| Rule | Detail |
|------|--------|
| **Main mode** | **Mandatory** |
| **Aggressive mode** | **Prohibited** |
| Roles | **GM = initiator** · **KDC = responder** |
| Auth | **RSA-2048 digital signatures mandatory**; DSA/ECDSA optional |
| Recommendation | One signature scheme per group/KDC domain (unicast + multicast PUSH) |
| Cert request payload | **Shall not** be used — cert always in message 3 (**§8.3.5.3**) |
| SA payload DOI | **GDOI (2)**; Situation = **0** |
| SPI size in proposal | Expected **0** (ignored if non-zero) |
| Events | Unknown IKEv1 algorithm → `"error: unspecified cryptographic algorithm proposed in IKEv1 phase 1."` |
| Padding error | → `"error: padding error during decryption of in IKEv1 handshake."` + abort |

**Figure 19/20/21/22 exchange:**

```
(1) HDR, SA  ↔  HDR, SA
(2) HDR, KE, Ni  ↔  HDR, KE, Nr
(3) HDR*, IDii, CERT, SIG_I  ↔  HDR*, IDir, CERT, SIG_R
```

**§8.3.5 ID authentication (pp. 80–81):**

| Payload | Rule |
|---------|------|
| **IDii/IDir** | Type **ID_DER_ASN1_DN (9)** only — DER Subject from X.509 cert |
| **CERT** | DER X.509; encoding type **4** only; KDC sends **exactly one** cert |
| **SIG_I/R** | `HASH_I = prf(SKEYID, g^xi|g^xr|CKY-I|CKY-R|SAi_b|IDii_b)` · RSA = PKCS#1 **private-key encryption** (not signature OID form) |
| Verify fail | KDC → Phase 2 Informational + **Delete** payload (encrypted Phase 1 SA) |

### §8.4 ISAKMP informational exchange type 5 (p. 81)

| Phase | Behaviour |
|-------|-----------|
| Purpose | KDC notifies peers of **errors** only — **not** status SAs (RFC 2409 §5.7) |
| Phase 1 info | **Not encrypted**; Message ID = **0** |
| Phase 2 info | **Encrypted**; unique Message ID for IV |

*§8.4.2–8.5.6 (pp. 82–90): GROUPKEY-PULL phase 1/2 detail · Table 4 OID registry · GOOSE/SV selector ASN.1 — **PARTIAL capture** (see gaps).*

**Captured ASN.1 fragment (OID-specific payload, corrupted page):**

```
IecUdpAddrPayload ::= SEQUENCE {
  version    ...,
  ipAddress  IPADDRESS,
  dsRef      ...
}
IPADDRESS ::= SEQUENCE {
  typeOfAddress ...,
  address CHOICE { ip ..., dns ... }
}
```

### §8.5 SA TEK payloads (pp. 91–96)

#### §8.5.7 IEC 61850 SA TEK — Figure 38 (p. 91)

| Field | Size | Content |
|-------|------|---------|
| OID Length | 1 | OID field length |
| OID | var | DER ASN.1 — 61850 message type (**Table 4**) |
| OID-Specific Payload Length | 2 | 0 if none |
| OID-Specific Payload | var | Traffic selector (e.g. multicast); DER |
| SPI | 4 | Current key ID |
| Auth Alg | 2 | **HMAC-SHA256-128 m**; also HMAC-SHA256, AES-GMAC-128/256 |
| Enc Alg | 2 | **AES-CBC-128 m** (RFC 8052 §2.2.3) |
| Remaining Lifetime | 4 | Seconds; 0 = no expiry; correlate with CRL refresh |
| SA Data Attributes | var | RFC 8052 §2.2.4 |

**Algorithm combination rules (p. 92):**

| Mode | Auth Alg | Enc Alg |
|------|----------|---------|
| **AEAD** (AES-GCM) | **NONE (1)** | AES-GCM-128/256 |
| **Non-AEAD** (AES-CBC) | HMAC-SHA256-128 **m** or AES-GMAC | AES-CBC-128/256 |
| Confidentiality off | HMAC/AES-GMAC required | **NONE (1)** |

#### §8.5.8 IEC 61850-9-3 PTP (pp. 92–93)

- Secures **IEEE 1588:2019** via **AuthenticationTLV** per-message integrity
- **GDOI** selected key mgmt (Annex P ref); immediate security processing
- **SPP** = low 8 bits of **SA TEK SPI**; **Enc Alg = 0** (no PTP confidentiality)
- **Auth Alg** = `IntegrityAlgTyp`; **icvLength** from Auth Alg

#### §8.5.9 SPI discussion — Figure 39 (p. 94)

| SPI | Size | Role |
|-----|------|------|
| **SA TEK SPI** | 4 | KeyID — KDC-assigned, unique per group |
| **SA KEK SPI** | 16 | Populates Initiator/Responder Cookie in GROUPKEY-PUSH; correlates PULL↔PUSH |

KDC **shall** provide globally unique SA KEK SPI per key group.

#### §8.5.10 SA data attributes (p. 95)

| Attribute | Value | Meaning |
|-----------|-------|---------|
| **SA_ATD** | 1 | Activation time delay (seconds before install); KDC includes even if 0 |
| **SA_KDA** | 2 | Key delivery assurance 0–100%; default 100 if unsupported |

#### §8.5.11 GROUPKEY-PULL key download — Figures 40–41 (p. 95)

```
(1) HDR*, HASH(1), Ni, ID
(2) HDR*, HASH(2), Nr, SA
(3) HDR*, HASH(3)     — HASH(3) = prf(SKEYID_a, M-ID|Ni_b|Nr_b)
(4) HDR*, HASH(4), KD — RFC 8052 §2.3 format; KDC shall not alter KD format
```

Invalid HASH(3) → Phase 2 Informational **`INVALID_HASH_INFORMATION (23)`**.

#### §8.5.12 TEK download + renewal — Figures 42–43 (pp. 96–97)

- TEK KD type = **1** · KEK KD type = **2** (RFC 6407 / IANA)
- KDC distributes **K0** (SA_ATD=0, use now) + **K1** (SA_ATD>0, future); GM switches at ATD expiry
- Renewal before **Remaining Lifetime** expires; GM requests **K2** when **K0** lifetime ends
- KDC may hold 2–3 SAs depending on trigger (expiry vs ATD)
- Event: DH negotiation fail → **`GROUP-PULL-PHASE1-KEY-NEGOTIATION`**
- Expired SA → **`GROUP_PULL_EXPIRED_SA`** · decrypt fail → **`GROUP_PULL_PHASE2_DECRYPTION`** · unknown group → **`GROUP-PULL-PHASE2-GROUP-NONEXISTENT`**

### §8.6 GROUPKEY-PUSH (pp. 98–99)

**Figure 43:** `HDR*, SEQ, [D,] SA, KD, SIG` — protected by rekey SA KEK

**Figure 44 ACK:** `HDR, HASH, SEQ, ID`

| Rule | Detail |
|------|--------|
| SIG | Over full message incl. HDR (**RFC 6407**) |
| KEK_ACK_REQUESTED | In SA KEK for PUSH only (**RFC 8263**) |
| HASH | `prf(ack_key, SEQ|ID)` |
| ack_key | `prf(base_key, "GROUPKEY-PUSH ACK"|SPI|L)` — SPI = I-Cookie+R-Cookie from PULL |
| SIG_HASH | **SHA256 (3)** + KEK_ACK_REQUESTED=1, L=512 **or** SHA512 (5) + KEK_ACK=3, L=1024 |
| Port | GM ACK to KDC source port; recommend KDC PUSH from **848** |
| Unreachable | **`GROUP-PUSH-UNREACHABLE-DESTINATION`** |

### §8.7 Operational considerations (pp. 100–102)

#### §8.7.2 Group security policy

| Condition | Action | Event |
|-----------|--------|-------|
| KDC unreachable at rotation | Continue current TEK | `"warning:connection to KDC not available,prolonged usage of TEK"` |
| Expired TEK grace | Accept expired TEK **< 1 h** initially | Local policy |
| Missing TEK for KeyID | Immediate **GROUPKEY-PULL** | `"warning:current TEK not available,GROUPKEY-PULL initiated"` |
| KDA failure | GKCS raises **`KDA-FAILURE`** | Publisher policy |

#### §8.7.3 Group dynamicity

| Operation | Effect |
|-----------|--------|
| **Add** | PULL: GM queries KEK+TEK · PUSH: TEK pushed but GM must PULL for KEK |
| **Delete** | Effective next key update; PULL omits removed GM; PUSH unicast delete payload |
| **Revoke** | **Immediate** TEK delete + KEK change; PUSH new TEK/KEK → all GMs PULL; revoked GM rejected |

**Preconditions:** X.509 certs + trust anchors on KDC and GDOI client; GM configured with KDC address.

#### §8.7.4 Key Delivery Assurance (informative)

- GM→KDC: PULL msg 3 ACK · PUSH Key ACK if requested
- KDC→Publisher: PUSH informational payload with reception %; indeterminate in PULL-only groups

**§9 PICS** starts p. **102** (Tables 7–8 GDOI on p. **104** — already in Batch B extract).

---

## §9 Conformance / PICS (pp. 103–104)

**Notation:** **m** mandatory · **o** optional · **c** conditional · **x** excluded

### Table 5 — General key management (p. 103)

| Item | Description | C/S | Ref |
|------|-------------|-----|-----|
| **G-1** | Required cryptographic materials | **m** | 6.3 |
| **G-2** | Random Number Generation | **ml** | 6.4 |
| **G-3** | AVL OIDs/extensions (conditional) | **c** | 6.5.2, 7.8 |
| **G-4** | Security events per **62351-14** | **o** | 6.2 |

**ml:** RNG mandatory; low-entropy entities need OOB seed.

### Table 6 — Asymmetric key management (pp. 103–104)

| Item | Description | End entity | PKI | Ref |
|------|-------------|------------|-----|-----|
| **A-1** | Table 1 pub-key components | **m** | **m** | 7.2.1 |
| **A-2** | Table 2 attribute cert components | **o** | **m** | 7.2.2 |
| **A-3** | Key generation + installation | **m** | **m** | 7.3.1 |
| **A-4** | Cryptographic key protection | **m** | **m** | 7.3.2 |
| **A-5** | Existing SKMI | **o** | **m** | 7.3.3 |
| **A-6** | Entity registration | — | **m** | 7.3.5 |
| **A-7** | Entity configuration | **m** | — | 7.3.6 |
| **A-8** | Entity enrolment | **ol** | **ml** | 7.3.7 |
| **A-9** | Trust anchor update | **o** | **o** | 7.3.8 |
| **A-10** | Pub-key cert verification | **m** | **m** | 7.4.3–4 |
| **A-11** | Attribute cert verification | **o** | **m** | 7.4.3, 7.4.5 |
| **A-12** | Certificate revocation | **c** | **m** | 7.5 |
| **A-13** | Expiration + renewal | **m** | **m** | 7.6 |
| **A-14** | Clock sync + accuracy | **m** | **m** | 7.7 |
| **A-15** | Secure time sync | **o** | **o** | 7.7 |
| **A-16** | AVL support | **o** | **o** | 7.8 |

**Notes:** **ol** — manual/EST/SCEP; **ml** — PKI shall automate SCEP+EST; **c** — end entity ≥1 of CRL or OCSP.

*Table 7–8 (GDOI/GOOSE) on p. 104 — P2, not C1 MMS.*

---

## §6 Key management — general (Batch C, pp. 49–50)

### §6.1 General (p. 49)

Applies to asymmetric + symmetric key material. **§7** = asymmetric lifecycle; **§8** = group symmetric keys.

### §6.2 Handling of security events (pp. 49–50)

Implementations **should** announce security events from platform/components.

**Strongly recommended:** expose events via **62351-14** cyber event logging and/or **62351-7** monitoring objects. **Annex D** maps to 62351-14.

| Severity | Meaning |
|----------|---------|
| **Notice** | Routine activity/maintenance; not breach or attack |
| **Warning** | Deviation from normal; not necessarily attack |
| **Error** | Unforeseen condition; may indicate unauthorized activity; may not need immediate action |
| **Alarm** | Serious problem / unauthorized activity requiring **immediate action** |

Org security policy determines final handling; multiple alarms may escalate to **incident**.

**Table 6 G-4:** Security events per 62351-14 = **optional (o)** in PICS.

### §6.3 Required cryptographic material (p. 50)

Referencing standards define materials in **Clause 9 PICS**.

### §6.4 Random Number Generation (p. 50)

- Random values per **ISO/IEC 18031**
- Key generators: statistically adequate RNG
- Guidance: **NIST SP 800-90A**, **RFC 4086**
- Low-capability entities: external CA generation + secure install (Annex B.12)

### §6.5 Object identifiers (p. 50)

**IEC 62351 OID namespace:** `1.0.62351` (62351-9 and future parts); `1.2.840.10070` used by 62351-8 access tokens.

| OID | Definition |
|-----|------------|
| `id-IEC62351-9` | `{ 1 0 62351 9 }` |
| `avl62351Extion` | `{ id-IEC62351-9 1 }` — AVL extensions |
| `avl62351EntryExt` | `{ id-IEC62351-9 2 }` — AVL entry extensions |
| `id-62351prot` | `{ id-IEC62351-9 3 }` — protocol identifiers |

---

## Annex C — Certificate enrolment and renewal (informative, pp. 129–130)

### C.1 Certificate Enrolment — Figure C.1

**K6.3 ceremony flow (TG-524):**

1. Register device in CA
2. Configure device with registration data
3. **Trigger:** key absent/expired · config change · external command
4. Generate key pair (on device)
5. Generate CSR (PKCS#10)
6. Send CSR via enrolment protocol (EST/SCEP) to CA
7. CA validates request
   - Invalid → error response to device
   - Valid → generate certificate
8. Send certificate to device via enrolment protocol
9. Device validates received certificate
   - Invalid → do not store
   - Valid → store in TPM/secure store

### C.2 Certificate Renewal — Figure C.2 (state machine)

```
START → cert present? ─NO→ ENROLL
              │
             YES → expired? ─YES→ ENROLL
                      │
                     NO → (monitor)
STORE cert → START TIMER (time-to-expiry)
              │
              └── at 70% of max lifetime → ENROLL (re-enrol/renew)
```

**CCLI policy anchor:** proactive renewal at **70%** cert lifetime (informative; align with 62351-3 renegotiation/CRL 24 h cadence).

**Dual cert (Annex G):** repeat flow separately for **Cert A (TLS)** and **Cert B (E2E)** — distinct CSR/keyUsage/EKU.

---

## Annex D — Security event mapping to IEC 62351-14 (pp. 131–137)

**IEC version:** all events = **"1"**

| Group | Value | Domain |
|-------|-------|--------|
| Credential transport / enrolment | **1** | PKCS#12/8, SCEP, EST |
| Public-key cert verification | **2** | §7.4.4 |
| Attribute cert verification | **3** | §7.4.5 |
| Revocation status | **4** | §7.4.6 |
| GDOI | **5** | §8 *(P2)* |

### Table D.1 — Credential transport and enrolment (Group 1)

| MNEMONIC | Severity | Event ID | Clause |
|----------|----------|----------|--------|
| `CRED_PKCS12_FORMAT` | Warning | IEC 62351-9:**1.1** | 7.3.2 |
| `CRED_PKCS8_FORMAT` | Warning | IEC 62351-9:**1.2** | 7.3.2 |
| `CRED_ENR_SCEP_SUCC` | Notice | IEC 62351-9:**1.3** | 7.3.7.2 |
| `CRED_ENR_SCEP_FAIL` | Error | IEC 62351-9:**1.4** | 7.3.7.2 |
| `CRED_ENR_EST_SUCC` | Notice | IEC 62351-9:**1.5** | 7.3.7.3 |
| `CRED_ENR_EST_FAIL` | Error | IEC 62351-9:**1.6** | 7.3.7.3 |

### Table D.2 — Public-key certificate verification (Group 2, pp. 132–134)

Selected **C1-critical** events:

| MNEMONIC | Severity | Event ID | Text (abbrev) |
|----------|----------|----------|---------------|
| `CERT_V_FORMAT` | Warning | **2.1** | Format mismatch |
| `CERT_V_PK_SIG_WRONG` | **Alarm** | **2.2** | Signature could not be verified |
| `CERT_V_PK_E` | Error | **2.6** | Certificate expired |
| `CERT_V_PK_TA` | Error | **2.10** | Trust anchor not supported |
| `CERT_V_PK_PATH` | Error | **2.11** | Path could not be verified |
| `CERT_KU_DIGSIG` | Error | **2.19** | digitalSignature missing (TLS) |
| `CERT_PKCE_EKU_TLS_SA` | Error | **2.23** | serverAuth missing |
| `CERT_PKCE_EKU_TLS_CA` | Error | **2.24** | clientAuth missing |
| `CERT_V_PKCE_NOCSP` | Warning | **2.27** | OCSP info not in cert |

Full table: **2.1–2.27** (pp. 132–134).

### Table D.3 — Attribute certificate verification (Group 3, pp. 134–135)

**3.1–3.16** — holder verify, issuer trust, expiry, extensions, revocation info. RBAC via 62351-8.

### Table D.4 — Certificate revocation status (Group 4, p. 136)

| MNEMONIC | Severity | Event ID |
|----------|----------|----------|
| `CERT_V_REVOKED` | **Alarm** | **4.1** |
| `CERT_V_R_NR_CRL` | Warning | **4.2** |
| `CERT_V_CRL_EXP` | Warning | **4.3** |
| `CERT_V_CRL_SIG_FAIL` | Warning | **4.4** |
| `CERT_V_R_NR_OCSP` | Warning | **4.5** |
| `CERT_V_R_TO_OCSP` | Warning | **4.6** |
| `CERT_V_R_Cu_OCSP` | Warning | **4.7** |
| `CERT_V_R_OCSP_EXP` | Warning | **4.8** |
| `CERT_V_OCSP_SIG_FAIL` | Warning | **4.9** |

ExtraInfo: revocation reason (4.1), CRLDP URL (4.2), OCSP URL (4.5).

### Table D.5 — GDOI events (Group 5, p. 137) — P2

**5.1–5.8** — IKEv1/GDOI algorithm mismatch, padding, GROUPKEY-PULL/PUSH errors. Not C1 MMS P0.

---

## K6.3 ceremony SOP (derived from Annex C + §7.3)

| Phase | Action | Evidence |
|-------|--------|----------|
| **Factory** | IDevID provision (802.1AR) | Manufacturer CA chain in TPM |
| **Commission** | Register with DSO RA; configure trust anchors (≥5), RA FQDN, subject CN/serial | Engineering record |
| **Enrol TLS cert** | EST `/simplerenroll`; EKU **clientAuth+serverAuth**; SAN = Eth_A IP/FQDN | `CRED_ENR_EST_SUCC` (1.5) |
| **Enrol E2E cert** | Second CSR; Annex G extensions; separate key pair | Second 1.5 event |
| **Operate** | CRL/OCSP check; clock sync (§7.7); renewal timer **70%** | Annex D group 4 events |
| **Renew** | Re-enrol before expiry; log success/failure | Figure C.2 state machine |
| **Compromise** | §5.9.4 local technician or re-enrol via surviving LDevID | Incident log |

---

## Security events catalogue (normative + Annex D IDs)

| Event | Type | Annex D ID |
|-------|------|------------|
| `alarm:public-key certificate signature could not be verified` | alarm | **2.2** |
| `error:public-key certificate expired.` | error | **2.6** |
| `error:unsupported signature algorithm.` | error | **2.5** |
| `warning: CRL distribution point not accessible` | warning | **4.2** |
| `warning:OCSP responder not accessible` | warning | **4.5** |
| `alarm:certificate has been revoked.` | alarm | **4.1** |
| `notice:enrolment using EST successfully performed` | notice | **1.5** |
| `error:enrolment using EST cancelled with error.` | error | **1.6** |
| `warning:PKCS #12 format mismatch` | warning | **1.1** |

---

## CCLI conduit mapping

| Conduit | 62351-9 role | Certs |
|---------|--------------|-------|
| **C1** Eth_A | DSO PKI; OCSP proxy at CCI/substation | **Cert A** TLS + **Cert B** E2E (Annex G) |
| **C2** 104 | Same CA policy as C1 or DSO-mandated | TLS cert |
| **Factory** | IDevID provisioning | Manufacturer CA |

---

## Interface matrix

| Interface | Source | Destination | Protocol |
|-----------|--------|-------------|----------|
| Enrolment | TG-524 | DSO RA/CA | **EST** or **SCEP** over TLS |
| Revocation check | TG-524 / peer | OCSP responder / proxy | **OCSP** (RFC 6960) |
| CRL fetch | Substation proxy | CRL distribution point | HTTP/LDAP |
| Trust anchor update | DSO CA | TG-524 | **TAMP** (optional) |

---

## BOM matrix

| Block | Function | Candidate / PN | Status |
|-------|----------|----------------|--------|
| TPM 2.0 | Private key + cert storage | On-board (datasheet) | HAVE |
| Secure element | IDevID / LDevID | TBD vs TPM | GAP |
| RA/CA | DSO infrastructure | Customer-provided | EXTERNAL |
| OCSP proxy | Local revocation | TG-524 CCI role (Fig 16) | DESIGN |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| §5.8.4 enrolment detail | **HAVE** | p. **35** | Batch A supplement **PASS** |
| §6.2 + Annex D | **HAVE** | pp. 49–50, 131–137 | Batch C **PASS** |
| Annex C SOP | **HAVE** | pp. 129–130 | Batch C **PASS** |
| §7 normative cert mgmt | **HAVE** | pp. 51–72 | Batch B **PASS** |
| §7.8 AVL tail | **HAVE** | p. **75** | §7.8.6–8 **PASS** |
| §9 PICS Tables 5–6 | **HAVE** | pp. 103–104 | Batch B **PASS** |
| §8.4–8.5.6 GDOI detail | **PARTIAL** | pp. **82–90** | Table 4 · phase-2 detail gap |
| §8 GDOI/GOOSE | **HAVE (P2)** | pp. **75–81, 91–102** | Batch E **PASS** |
| K6.3 written SOP | **HAVE** | `ccli-k63-ceremony-sop` | Lab execution |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-359-001 | Single cert for TLS + E2E | Wrong key usage / profile | 62351-4 Annex G.2 dual cert |
| R-359-002 | Central key gen without OOB protection | Key compromise | §5.7.4.4 trusted distribution only |
| R-359-003 | OCSP unreachable in field | False alarms / blocked connect | Proxy pattern Fig 16; 62351-3 CRL≠terminate |
| R-359-004 | No IDevID → hard field commissioning | Manual trust injection | 802.1AR + RFC 8366 voucher path |
| R-359-005 | Flat PKI not enforced | Slow path validation | §5.9.3 — compact CA hierarchy |
| R-359-006 | Edition drift 2017 vs 2023 | Wrong RFC refs | Corpus locked to **2023** |
| R-359-007 | GDOI on C1 MMS scope creep | Wrong test matrix | §8 = **P2**; C1 = 62351-3/4 only |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| Batch A §5.7–5.9 | PKI + revocation captured | Extract complete | **PASS** |
| Batch A p. 35 | §5.8.4 enrolment scenarios | K6.3 | **PASS** |
| Batch B §7 | Normative verification + lifecycle | Mapped to TPM/OpenWrt | **PASS** |
| Batch B §9 | PICS Tables 5–6 | Lab template | **PASS** |
| Batch C §6.2 + Annex D | 62351-14 event IDs | CR 6.2 mapping | **PASS** |
| Batch C Annex C | K6.3 ceremony flow | SOP derivation | **PASS** |
| Batch E §8 GDOI | pp. 75–102 (82–90 partial) | P2 corpus | **PASS** |
| Dual cert ceremony | Issue TLS + E2E from same CA | Annex G fields | Lab |
| EST enrolment | TG-524 obtains LDevID | §7.3.7.3 flow | Lab |
| Clock sync | GNSS + PTP/NTP | §7.7 + 62351-4 skew | Lab |

---

## Source pages — Batch A

| Page | Content | Status |
|------|---------|--------|
| **10** | §1 Scope | PASS |
| **11** | §2 Normative references | PASS |
| **30** | §5.7.1–5.7.3 PKI · Fig 6 | PASS |
| **31** | §5.7.4 Public-key certs · renewal | PASS |
| **32** | §5.7.4.4 Fig 7 · §5.7.5 attribute certs | PASS |
| **33** | Fig 8 · §5.7.6 · §5.8.1 | PASS |
| **34** | §5.8.2 IDevID · §5.8.3 LDevID onboarding | PASS |
| **35** | §5.8.3 ZTP refs · **§5.8.4** enrolment scenarios | PASS |
| **36** | Fig 9 SCEP | PASS |
| **37** | Fig 10 EST | PASS |
| **38** | §5.8.5.1 CSR · Fig 11 | PASS |
| **39** | §5.8.5.2 PKCS#10 · Fig 12 | PASS |
| **40** | §5.8.5.3 CRMF · Fig 13 | PASS |
| **41** | §5.8.6 SCEP · EST | PASS |
| **42** | §5.8.6.3–7 CMP · CMC · TAMP · §5.9.1 CRL | PASS |
| **43** | CRL structure Fig 14 · revocation reasons · §5.9.2 OCSP intro | PASS |
| **44** | OCSP · Fig 15 · stapling · hybrid | PASS |
| **45** | Fig 16 CRL+OCSP substation | PASS |
| **46** | Fig 17 OCSP flows · §5.9.3 SCVP | PASS |
| **47** | Fig 18 SCVP · §5.9.4 recovery · §5.10 start | PASS |

---

## Source pages — Batch B (pp. 51–72, 103–104)

| Page | Content | Status |
|------|---------|--------|
| **51** | §7.1 · §7.2.1 · **Table 1** start | PASS |
| **52** | Table 1 cont · §7.2.2 intro | PASS |
| **53** | **Table 2** · §7.3.1 key gen | PASS |
| **54** | §7.3.1 conditions · §7.3.2–7.3.4 protection/PKI/policy | PASS |
| **55** | §7.3.5 registration · §7.3.6 config (≥5 trust anchors) | PASS |
| **56** | §7.3.7.1 enrolment general | PASS |
| **57** | §7.3.7.2 SCEP · §7.3.7.3 EST | PASS |
| **58** | §7.3.8 TAMP · §7.4.1–7.4.2 format | PASS |
| **59** | §7.4.3 sig verify · §7.4.4.1–4.4.4 RSA/ECDSA | PASS |
| **60** | §7.4.4.4 ECDSA curves · §7.4.4.5 validity | PASS |
| **61** | §7.4.4.6 subject · §7.4.4.7–9 path | PASS |
| **62** | §7.4.4.10 extensions general · AKI/SKI/SAN | PASS |
| **63** | basicConstraints · keyUsage | PASS |
| **64** | extendedKeyUsage · AVL ext · CRLDP | PASS |
| **65** | CRLDP/AIA detail · RBAC · SOA ext | PASS |
| **66** | §7.4.5 attribute cert components | PASS |
| **67** | §7.4.5.4–5 issuer/signature | PASS |
| **68** | §7.4.5.6–8 attributes/validity/extensions | PASS |
| **69** | §7.4.5.8.3 attr revocation · §7.4.6 status | PASS |
| **70** | §7.4.6 CRL/OCSP events · §7.5 revocation | PASS |
| **71** | §7.5 notes · §7.6 expiration/renewal | PASS |
| **72** | §7.7 clock sync · §7.8.1–7.8.2 AVL start | PASS |
| **103** | §9.2 notation · **Tables 5–6** start | PASS |
| **104** | Table 6 cont (A-9–A-16) · Table 7–8 GDOI *(P2)* | PASS |

---

## Source pages — Batch C (pp. 49–50, 129–137)

| Page | Content | Status |
|------|---------|--------|
| **49** | §5.11.3 AVL constrained · §6.1–6.2 security events | PASS |
| **50** | §6.2 alarms · §6.3–6.5 RNG · OIDs | PASS |
| **129** | Annex C.1 · **Figure C.1** enrolment flow | PASS |
| **130** | Annex C.2 · **Figure C.2** renewal state machine (70%) | PASS |
| **131** | Annex D.1–D.2 · **Table D.1** enrolment events | PASS |
| **132** | **Table D.2** pub-key verification (2.1–2.13) | PASS |
| **133** | Table D.2 cont (2.14–2.24) extensions/keyUsage/EKU | PASS |
| **134** | Table D.2 tail (2.25–2.27) · **Table D.3** start | PASS |
| **135** | Table D.3 cont (3.6–3.16) attribute cert events | PASS |
| **136** | **Table D.4** revocation status (4.1–4.9) | PASS |
| **137** | **Table D.5** GDOI events (5.1–5.8) *(P2)* | PASS |

---

## Source pages — Batch E + supplements (pp. 35, 75–102)

| Page | Content | Status |
|------|---------|--------|
| **35** | RFC 8572/8995/8520 · **§5.8.4** enrolment | PASS |
| **75** | §7.8.6–8 AVL · **§8.1** GDOI requirements | PASS |
| **76** | **Table 3** KDC IKEv1 requirements | PASS |
| **77** | §8.3.1 main mode · DH groups · events | PASS |
| **78** | **Fig 19–20** IKEv1 main mode · §8.3.2 cert request prohibited | PASS |
| **79** | §8.3.3–4 SA/proposal/transform · **Fig 21** KE | PASS |
| **80** | §8.3.5.1–2 ID auth · **Fig 22** | PASS |
| **81** | §8.3.5.3–4 CERT/SIG · **§8.4.1** informational | PASS |
| **82–90** | §8.4.2–8.5.6 · **Table 4** · GOOSE/SV selectors | **PARTIAL** |
| **91** | **§8.5.7** · **Fig 38** 61850 SA TEK | PASS |
| **92** | TEK alg rules · **§8.5.8** 61850-9-3 PTP start | PASS |
| **93** | PTP TEK field mapping (a–i) | PASS |
| **94** | **§8.5.9** SPI · **Fig 39** PULL/PUSH correlation | PASS |
| **95** | §8.5.10 SA_ATD/KDA · **§8.5.11** · Figs 40–41 | PASS |
| **96** | Key renewal sequence · TEK/KEK KD types | PASS |
| **97** | **Fig 42** entity-triggered renewal | PASS |
| **98** | §8.5.12 events · **§8.6.1** · Figs 43–44 | PASS |
| **99** | §8.6.2–3 PUSH/ACK · Figs 45–46 | PASS |
| **100** | ACK params · port 848 · **§8.7.1–8.7.3.1** | PASS |
| **101** | §8.7.3.2–3 add/delete members | PASS |
| **102** | §8.7.3.4 revoke · §8.7.4 KDA · **§9** start | PASS |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-08-12 | **Batch A** pp. 10–11, 30–47: §1, §2, §5.7–5.9 PKI/enrolment/revocation |
| **1.1** | 2026-08-12 | **Batch B** pp. 51–72, 103–104: §7 normative + §9 PICS Tables 5–6 |
| **1.2** | 2026-08-12 | **Batch C** pp. 49–50, 129–137: §6.2 events · Annex C K6.3 · Annex D → 62351-14 |
| **1.3** | 2026-08-12 | **p. 35** §5.8.4 · **Batch E** pp. 75–102 §8 GDOI *(82–90 partial)* |
| **1.4** | 2026-08-12 | Cross-link **K6.3 SOP** `ccli-k63-ceremony-sop` |

---

## RAG cross-links

| source_id | Document |
|-----------|----------|
| `ccli-k63-ceremony-sop` | K6.3 key ceremony SOP (formal) |
| `ccli-62351-9-extract` | This document |
| `ccli-62351-9-capture-plan` | Capture batches |
| `ccli-62351-3-extract` | TLS — OCSP stapling · §5.6.4.4 |
| `ccli-62351-4-extract` | Annex G dual cert · E2E key ceremony |
| `ccli-62443-4-2-extract` | CR 1.8 PKI |
| `ccli-62443-zones-extract` | C1/C2 conduits |

# IEC 62351-4 — CCLI Engineering Extract

**Document ID:** CCLI-SEC-62351-004  
**Revision:** 1.8  
**Date:** 2026-08-12  
**RAG source_id:** `ccli-62351-4-extract`  
**Normative basis:** IEC 62351-4:2018+AMD1:2020 — *Profiles including MMS and derivatives*  
**Capture status:** **HAVE (C1 P0)** — Batches A–G; optional gaps §3 · §6.2.1–7 · §10  
**Parents:** `ccli-62351-4-capture-plan` · `ccli-62351-1-extract` · `ccli-62443-zones-extract`  
**Programme link:** K6.5 · KR-001 · **Conduit C1**

---

## Summary

**62351-4:2018** extends **IEC TS 62351-4:2007** with **compatibility mode** (2007 interop) and **native mode** (E2E app security). Specifies security at **transport** (TLS / **62351-3**) and **application** (A-profile §11 or **E2E §13**).

**TG-524 C1 target:** **native mode** — `e2eMmsAC` application context (§5.2) + **TLS 1.2** (RFC 5246) + **62351-3**.

**Key normative refs (§2):** **62351-3:2014+AMD1:2018**, **62351-8** RBAC, **62351-9** key management, **ISO 9506-2** MMS, **X.509:2020** (AMD1), **RFC 5246** TLS 1.2.

---

## Requirements

| REQ-ID | Source | Requirement | CCLI |
|--------|--------|-------------|------|
| **REQ-3514-SCOPE-001** | §1.1 | Extends 2007 TS; compatibility + native modes | Product policy |
| **REQ-3514-MODE-001** | §4.3 | **Native mode** for new implementations; compatibility not recommended | Default native on C1 |
| **REQ-3514-STACK-001** | §4.1–4.2 | A-profile (L5–7) + T-profile (L1–4); T-security = **TLS** | C1 MMS/TCP |
| **REQ-3514-COMBO-001** | Table 1 | Full security = E2E + TLS (Note 4) or A-profile + TLS (Note 2) | Lab PICS |
| **REQ-3514-61850-001** | §5.2 | Compatibility → `basic-AC`; native → **`e2eMmsAC`** | **C1** |
| **REQ-3514-TLS-001** | §6.3 | Secure MMS → **TCP port 3782**; non-secure → **102** | C1 firewall |
| **REQ-3514-TLS-002** | §6.3.4.3 / Table 3 | **Native mode** mandatory cipher suites (4× GCM/CBC-SHA256) | OpenWrt policy |
| **REQ-3514-TLS-003** | §6.2.8 | Revoked/expired cert → terminate (policy); revocation fetch fail → no auto-terminate | CR 1.8 PKI |
| **REQ-3514-TLS-004** | §6.2.9 | Security events → **62351-7** / **62351-14** | CR 6.2 |
| **REQ-3514-ICCP-001** | §5.1 | App security: `NON_SECURE` \| `SECURE` \| `END-TO-END SECURE` | N/A unless ICCP |
| **REQ-3514-CRYPTO-001** | §8.3–8.5 | **RSA ≥2048**; ECDSA **secp256r1**/brainpoolP256r1; **SHA-256** signatures | CR 1.8 |
| **REQ-3514-CRYPTO-002** | §8.6–8.7 | E2E encryption: **AES-CBC** / **AES-GCM**; unique IV/nonce per message | §13 |
| **REQ-3514-CRYPTO-003** | §8.8 | Integrity: **HMAC-SHA256** or AES-GCM ICV | §13 |
| **REQ-3514-APROF-001** | §11 | A-profile: **`basic-AC`** + mechanism **`{1 0 840 0 1 0 1 1}`** | Legacy interop |
| **REQ-3514-APROF-002** | §11.2.2 | MMS auth: X.509 cert + GeneralizedTime + **sha256WithRSA** sig | CR 1.8 PKI |
| **REQ-3514-APROF-003** | §11.2.3–4 | **10 min** UTC skew; duplicate time → **P-ABORT** | GNSS/NTP |
| **REQ-3514-E2E-001** | §13.1 | Handshake **HandshakeReq/Acc** with signed **ClearToken1** | CR 1.8/1.9 |
| **REQ-3514-E2E-002** | §13.1.2 | UTC time **`YYYYMMDDhhmmss,sssZ`** — no **`T`** separator | GNSS/NTP |
| **REQ-3514-E2E-003** | §13.1.3 | Protected protocol **`pp`** = **`IEC 61850-8-1`** on C1 | C1 MMS |
| **REQ-3514-E2E-004** | §13.3.1 | **DH group 14** (2048 MODP) or **ECDH 23** (secp256r1); **HKDF** key derive | CR 1.8 |
| **REQ-3514-E2E-005** | §13.3.2 | Data transfer **ClearToken2** + **Authenticator** ICV; seq anti-replay | CR 3.1 |
| **REQ-3514-E2E-006** | §13.3.1.4 | **`applProtected`** bit set for E2E app encryption on C1 | Allegato O E2E |
| **REQ-3514-LOG-001** | §4.6 | Security violations → tamper-resistant log | CR 6.2 / 62351-7 |
| **REQ-3514-E2E-007** | §14.4 | Data transfer: **10 min** time skew; **seq** anti-replay; ICV verify | CR 3.1 / 2.11 |
| **REQ-3514-E2E-008** | §13.2 | **ClearTransfer** / **EncrTransfer**; ICV over **tbp**; **Encrypted-ApplData** = encrypted PrPDU | CR 3.1 data path |
| **REQ-3514-OSI-001** | §15.2–15.4 | **`e2eMmsAC`**; SecPDUs in ACSE **`user-information`**; **Table 4** mapping | C1 MMS stack |
| **REQ-3514-OSI-002** | §15.2.2 | **No** `calling/called-authentication-value` or `mechanism-name` in AARQ/AARE (native) | vs A-profile §11 |
| **REQ-3514-CONF-001** | §17 / Tables 6–11 | PICS: **OSI** + **native mode** + Table 10/11 crypto | Lab quote |
| **REQ-3514-PKI-001** | Annex G.2 | **Separate** end-entity certs for **TLS** vs **E2E application** when both supported | CR 1.8 dual cert |
| **REQ-3514-PKI-002** | Annex G.3–G.4 | Cert size minimized; **≤8192** octets in MMS auth; fit **10 240** octet Session CONNECT budget | TG-524 cert profile |
| **REQ-3514-PKI-003** | Annex G.4 | **X.509 v3**; serial = **128-bit UUID** (non-negative integer); sig alg **SHA-256+**; **RSA ≥2048** / **EC ≥256** | CR 1.8 |
| **REQ-3514-PKI-004** | Annex G.5.2 | **keyUsage:** **digitalSignature** + **keyAgreement** | E2E handshake |
| **REQ-3514-PKI-005** | Annex G.5.3 | Revocation via **CRL** *or* **OCSP** (62351-9) — one method per cert | CR 1.8 |
| **REQ-3514-PKI-006** | Annex G.6.2 | OSI **subject** DN: **objectIdentifier** = AP-title + AE-qualifier from AARQ/AARE (§10.5.3) | address-mismatch check |

---

## Architecture — C1 security stack

```
Z1 (CCI)                         Z0 (DSO/SCADA)
     │  Eth_A — Conduit C1              │
     │  IEC 61850 MMS (61850-8-1)       │
     │  ┌────────────────────────────┐  │
     │  │ 62351-4 §13 E2E (native)   │  │  ← target
     │  │ 62351-4 §6 / 62351-3 TLS   │  │
     │  │ TCP/IP (RFC 793/791/2460)  │  │
     │  └────────────────────────────┘  │
     └──────────────────────────────────┘
```

### Fig 1 — Application and transport profiles (p. 18)

| Stack | A-profile (L5–7) | T-profile (L1–4) |
|-------|------------------|------------------|
| **OSI on TCP/IP** | MMS, ACSE, X.226, X.225 | X.224, RFC 1006, TCP, IPv4/v6, Ethernet |
| **Internet suite** | 61850-8-2, XMPP, MMS | TCP, IPv4/v6, Ethernet |

Hybrid stack: OSI apps (MMS) over **RFC 1006** on TCP/IP.

---

## §1 Scope (pp. 10–11)

### §1.1 General (p. 10)

- Extends **IEC TS 62351-4:2007** — **compatibility mode** + **native mode**.
- Security at **transport** and **application** layers.
- MMS handshake authentication; extended integrity/auth; shared key mgmt; encryption for handshake **and** data-transfer.
- **E2E security** with zero or more intermediate entities.
- Supports **OSI** (MMS) and **Internet** (TCP/IP) stacks; **XML** encoding; mapping to **OSI** and **XMPP**.
- **ICCP** implementations may depend on 2007 **T-profile** + **A-security-profile** (retained).
- Term **A-security-profile** kept for historical reasons.
- Audience: protocol WG, product developers, end users.

### §1.2 Code components (p. 11)

- License for code components: [iec.ch/CCv1](https://www.iec.ch/CCv1).
- Machine-readable files: `www.iec.ch/public/tc57/supportdocuments/IEC_62351-4.ASN.1_XSD.full.zip`.
- Code in **Annexes A–E**.
- ASN.1 / W3C XSD in **bold Courier New**; marked `<CODE BEGINS>` … `<CODE ENDS>`.

---

## §2 Normative references (pp. 11–12) — **partial**

| Standard | Role |
|----------|------|
| **IEC TS 62351-1** | Introduction |
| **IEC TS 62351-2** | Glossary |
| **IEC 62351-3:2014+AMD1:2018** | **TCP/IP TLS** — pairs with this part |
| **IEC TS 62351-8:2011** | **RBAC** |
| **IEC 62351-9:2023** | **Cyber security key management** |
| **ISO 9506-2:2003** | **MMS protocol** |
| **ISO/IEC 9594-8:2020 / X.509:2019** | **PKI certs** (AMD1) |
| **Rec. ITU-T X.227** | **ACSE** |
| **IETF RFC 5246:2008** | **TLS 1.2** |
| **IETF RFC 1006:1987** | ISO transport on TCP |
| **IETF RFC 2104:1997** | HMAC |
| **IETF RFC 5869:2010** | HKDF |
| ISO 8073, 8823-1, 8824-1, 8825-1/4 | OSI transport/presentation/ASN.1 |

---

## §4 Security issues (pp. 18–21)

### §4.2 Application and transport profiles (p. 18)

- **T-profile:** layers 1–4 (OSI) or TCP/IP transport.
- **A-profile:** layers 5–7 (application protocols).
- **T-security:** typically **TLS** per **IETF RFC 5246** (62351-3 for TCP/IP env).

### Table 1 — Security measure combinations (p. 19)

| App security | Transport | CCLI note |
|--------------|-----------|-----------|
| A-security profile | none | **Non-secure** (Note 1) |
| none | TLS | Transport-only |
| A-security profile | TLS | **Compatibility stack** (Note 2) |
| E2E without encryption | none / TLS | Auth + integrity E2E (Note 3) |
| **E2E with encryption** | none / TLS | **Full E2E** (Note 4) — **C1 target** |

**Notes:** TLS in OSI env per §6.3; secure cipher suites required. VPN suggested for ICCP if confidentiality needed without 62351-4 encryption.

### §4.3 Compatibility and native modes (p. 19)

| Mode | Spec | CCLI |
|------|------|------|
| **Compatibility** | 2007 interop; TLS ciphers §6.3.4.2; A-profile **§11**; MMS OSI (ISO 9506-2, X.227) | Legacy interop only — **not recommended for new** |
| **Native** | Extended TLS §6.3.4.3; **E2E §12–16**; zero+ intermediate entities | **TG-524 default** |

### §4.4 Threats countered (pp. 19–20)

**Compatibility + A-profile only:** masquerade at association.

**Compatibility + 62351-3 TLS:** + tampering (signatures/ICV/MAC), theft (encryption).

**Native + TLS (per hop):** masquerade, tampering, theft.

**Native E2E without encryption:** E2E masquerade, E2E tampering (handshake + data), replay (time, association ID, sequence).

**Native E2E with encryption:** + E2E confidentiality.

### §4.5 Attack methods (pp. 20–21)

**Compatibility (excl. 62351-3):** MITM, tamper, masquerade — TLS handshake auth + ICV on data.

**Native E2E:** MITM, tamper, masquerade, **replay** (time + sequence); + theft if encrypted.

### §4.6 Logging (p. 21)

Security violations → **separate tamper-resistant log** for audit/prosecution. Implementation local. See **62351-7** (monitoring), **62351-14** (security logging).

---

## §5 Specific requirements — **partial** (p. 21)

### §5.1 ICCP / IEC 60870-6 (p. 21) — *out of CCLI scope unless DSO requires*

Normative for **TASE.2** (60870-6-503/702/802). MMS security config via bilateral tables:

**T-security choices:** `DONT_CARE` | `NON_SECURE` | `SECURE`

**App security choices:** `NON_SECURE` | `SECURE` (A-profile §11 / 2007) | **`END-TO-END SECURE`**

Association parameters: address, profile indication (e2e/secure/non-secure), handshake auth — compared to configured expected values (acceptance = local policy).

### §5.2 Specific requirements for IEC 61850 (p. 22)

| Mode | Application context in AARQ |
|------|----------------------------|
| **Compatibility** | **`basic-AC`** (see 11.1.4.2) |
| **Native** | **`e2eMmsAC`** (see 15.2.2) |

**CCLI:** TG-524 C1 ships **native** — `e2eMmsAC` + TLS port **3782** + Table 3 ciphers.

---

## §6 Transport security (pp. 22–27) — **partial**

### §6.2.8 Public-key certificate validation (p. 24)

Per **62351-3:2018:** inability to retrieve **revocation info shall NOT** cause session termination.

Conformance implementations **shall** terminate connections (per local policy) if cert **revoked** or **expired**.

**Security events:**

| Condition | Event |
|-----------|-------|
| Revoked cert | **incident:** revoked public-key certificate |
| Expired cert | **warning:** expired public-key certificate |

### §6.2.9 Security events handling (p. 24)

Events/monitoring via **62351-7:2017** and/or **62351-14** (warnings + incidents).

### §6.3 T-security in OSI operational environment

#### §6.3.1 General (p. 24)

TLS per **62351-3:2018** with §6.2 parameters.

#### §6.3.2 TCP ports — **Fig 2** (p. 24)

| Profile | Port | Stack |
|---------|------|-------|
| **Non-secure TCP T-profile** | **102** | X.224 → RFC 1006 → TCP → IPv4/v6 → Ethernet |
| **Secure TCP T-profile** | **3782** | X.224 → RFC 1006 → **TLS** → TCP → IPv4/v6 → Ethernet |

**Conformance shall use port 3782** for transport security (IANA secure ISO TP0). Port 102 = RFC 1006 without T-security.

RFC 1006: transport class 0 of ISO/IEC 8073 (X.224) only.

#### §6.3.3 Disabling of TLS (p. 25)

Implementations **shall allow TLS disable** → use **port 102**. Permitted when lower-layer security (VPN/IPsec) or application profile warrants. **CCLI:** disable only on isolated lab; production C1/C5 **TLS mandatory**.

#### §6.3.4 TLS cipher suites (pp. 25–27)

**§6.3.4.1:** §6.3.4.2 = compatibility; §6.3.4.3 = **native**. ClientHello order per local security policy; mandatory suites in proposal list.

**§6.3.4.2 Compatibility mode (p. 25):** Minimum **`TLS_DH_DSS_WITH_AES_256_CBC_SHA`**. Table 2 (2007 legacy) — many now **disallowed** (RC4, anonymous DH).

#### Table 2 — Legacy cipher suites (2007 reference) (p. 26)

| Suite | Support | CCLI |
|-------|---------|------|
| TLS_RSA_WITH_RC4_128_SHA | **Disallowed** (weak) | Block |
| TLS_DH_*_AES_* anonymous | **Disallowed** | Block |
| TLS_DH_DSS_WITH_AES_256_CBC_SHA | **m** (compat min) | Legacy interop only |

#### §6.3.4.3 Native mode — **Table 3** (p. 27)

**Mandatory (m)** — conformance required:

| Cipher suite | IANA | Source |
|--------------|------|--------|
| **TLS_RSA_WITH_AES_128_CBC_SHA256** | 0x003C | RFC 5246 |
| **TLS_DH_RSA_WITH_AES_128_GCM_SHA256** | 0x00A0 | RFC 5288 |
| **TLS_DHE_RSA_WITH_AES_128_GCM_SHA256** | 0xC09E | RFC 5288 |
| **TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256** | 0xC023 | RFC 5289 |

**Optional (o):** DH_RSA AES_256_GCM, ECDHE_RSA GCM variants, etc.

**CCLI OpenWrt policy:** Prefer **ECDHE/DHE + GCM**; include all four mandatory; disable 3DES/RC4/SHA1.

#### §6.4 XMPP (p. 27)

Out of CCLI scope unless DSO requires XMPP profile.

---

## §7 Application layer overview (informative) — start (p. 27)

| Profile | Scope | CCLI |
|---------|-------|------|
| **A-security profile** (§11) | Peer auth at association only; **no** data-transfer binding/integrity/confidentiality; use with T-security | Compatibility / legacy |
| **E2E security** (§12–16) | App-layer security across 0+ intermediates | **Native C1 target** |

E2E XML namespace (§7.2.4): `http://www.iec.ch/62351/2018/E2E_4` (XSD in Annex C).

---

## §8 Cryptographic algorithms (pp. 29–32)

### §8.1 General (p. 29)

Algorithms for **A-security profile (§11)** and **E2E security (§13)**. Based on **X.509 ASN.1** constructs; §8.3–8.8 list IETF-defined algorithms. Open for future algorithms via ASN.1 extension mark.

### §8.3 Public-key algorithms (p. 30)

| Algorithm | Reference | CCLI |
|-----------|-----------|------|
| **rsaEncryption** | RFC 4055 | PKI / MMS auth |
| **ecPublicKey** | RFC 5480 | Optional ECDSA path |

**Curves:** **secp256r1**, **brainpoolP256r1** (RFC 5480 / RFC 5639).

### §8.4–8.5 Hash and signature (pp. 30–31)

| Algorithm | Use |
|-----------|-----|
| **sha256** | Hash (RFC 5754) |
| **sha256WithRSAEncryption** | RSA signatures (RFC 7427, RSASSA-PKCS1-v1_5) |
| **ecdsa-with-SHA256** | ECDSA signatures (RFC 7427) |

### §8.6 Symmetric encryption — AES-CBC (p. 31)

When E2E encryption selected, content encrypted with **aes128-CBC** or **aes256-CBC**.

- **IV:** 16 octets, **CSPRNG**, unique per message, transmitted with ciphertext (RFC 3602).

### §8.7 Authenticated encryption — AES-GCM (pp. 31–32)

**aes128-GCM**, **aes256-GCM** — encryption + integrity + auth (or GMAC-only).

| Parameter | Requirement |
|-----------|-------------|
| **aes-nonce** | Recommended **12 octets** (RFC 5084); unique per key use |
| **aes-ICVlen** | **12–16** octets, default **12** |

### §8.8 Integrity check value (p. 32)

**hmacWithSHA256** for ICV (integrity + sender auth; not legal non-repudiation). AES-GCM without encryption usable as MAC.

### §9 Object identifier allocation (p. 32)

```
e2e-security ::= { iso(1) standard(0) iec62351(62351) part4(4) }
secModule    ::= { e2e-security module(0) }
```

---

## §11 A-security profile (compatibility) (pp. 37–41)

**When used:** `SECURE` in bilateral agreement (§5.1) or application context **`basic-AC`** (§5.2). Equivalent to **62351-4:2007** A-profile. **Not recommended for new CCLI deployments** — use native E2E.

### §11.1 OSI upper-layer requirements

| Clause | Requirement |
|--------|-------------|
| **11.1.2** Session | Short or no session selectors (X.638 C.5.1.3) |
| **11.1.3** Presentation | Include **mmsAuthenticationAS** + **MMS** abstract syntaxes in context list |
| **11.1.4.1** ACSE | **Authentication** functional unit bit set in AARQ/AARE |
| **11.1.4.2** App context | **`basic-AC`** OID `{ iso standard iso9506 part2 mms-application-context-version1(3) }` |
| **11.1.4.3** Mechanism | **`mechanism-name`** = **`{ 1 0 840 0 1 0 1 1 }`** |

**mmsAuthenticationAS OID:** `{ 1 0 840 0 1 1 4 1 }` — kept for backward compat (2007 had typo). Partners may omit syntax; must accept valid AARQ without it.

### §11.2 MMS Authentication value (pp. 39–41)

#### §11.2.2 Data type (p. 40)

```
MMS-Authentication-value ::= CHOICE {
  certificate-based [0] SEQUENCE {
    authentication-Certificate [0] OCTET STRING,  -- DER X.509, max 8192 octets
    time [1] GeneralizedTime,
    signature [2] OCTET STRING } }  -- over DER-encoded time field
```

| Field | Rule |
|-------|------|
| **authentication-Certificate** | DER X.509; max **8 192 octets**; Annex G encoding |
| **time** | GeneralizedTime; accuracy as good as practical |
| **signature** | **sha256WithRSAEncryption**; RSA key **≥ 2048 bits (256 octets)**; algorithm not embedded in octet string |

#### §11.2.3–11.2.4 Association handling (pp. 40–41)

**Client (AARQ):** generate `MMS-Authentication-value`, include in `calling-authentication-value`.

**Server/client validation — failure → P-ABORT or reject:**

| Check | Failure action |
|-------|----------------|
| `mechanism-name` absent | `rejected-permanent` / `authentication-mechanism-name-required` |
| Cert invalid (expired/revoked) | **P-ABORT** |
| `time` **> 10 min** from local UTC | **P-ABORT** |
| Same `time` received twice within **10 min** | **P-ABORT** (replay) |
| Invalid signature | **P-ABORT** |

**Server (AARE):** on accept, generate matching `MMS-Authentication-value` in `responding-authentication-value`.

**CCLI / CR 1.8:** A-profile cert model aligns with **X.509** + **TPM** store; **10 min** skew bound → requires **GNSS/NTP** (62443 CR 2.11).

---

## §12 E2E application security model (pp. 41–43)

### §12.1 Architecture — Figs 5–8

| Figure | Concept |
|--------|---------|
| **Fig 5** | **Operational environment(s)** → **E2E-security** → **Protected protocol(s)** (modular building block) |
| **Fig 6** | Client ↔ Intermediary ↔ Server: **Env** wraps **E2E** wraps **Protected protocol** |
| **Fig 7** | **PrPDU** ⊂ **SecPDU** ⊂ **EnvPDU** |
| **Fig 8** | **62351-4** fixes E2E + Env mapping; **Virtual-API** maps PrPDU↔SecPDU; protocol in **61850-8-1/8-2** etc. |

**Environments:** **OSI** (Clause 15) and **XMPP** (Clause 16) defined; others may be added.

**Syntax:** ASN.1 (**BER/EXTENDED-XER**) and/or **W3C XSD** (Annex F virtual-API template).

---

## §13 End-to-end application security — normative (pp. 44–57)

### §13.1 Association management

**Lifecycle:** association establishment (handshake) → **data transfer** → termination.

#### §13.1.2 UTC time (p. 44)

| Rule | Value |
|------|-------|
| Date | `YYYYMMDD` |
| Time | `hhmmss,sss` (24h; comma or dot for fraction) |
| Timezone | **`Z` suffix only** — pure UTC |
| **Constraint** | **No `T`** between date and time |

#### §13.1.3–13.1.4 Handshake (pp. 44–46)

**HandshakeReq** (client):

```
HandshakeReq ::= SEQUENCE {
  pp    NMTOKEN,           -- protected protocol identity
  tbs   SEQUENCE { token1 ClearToken1, applData OPTIONAL },  -- Initiate-RequestPDU for MMS
  sign  Signature }        -- over tbs (exclude BER tag/len or XML tbs tags)
```

**Protected protocol `pp` values (C1):** **`IEC 61850-8-1`**

**HandshakeAcc** (server): same structure; **`applData`** = **Initiate-ResponsePDU** for MMS.

#### §13.1.5–13.1.11 Reject / abort / release (pp. 46–48)

| SecPDU | Use |
|--------|-----|
| **ApplicationReject** | Protected protocol rejects association |
| **HandshakeSecReject** | E2E security reject (+ optional **HandshakeDiagnostics**) |
| **HandshakeSecAbort** | Client rejects **HandshakeAcc** security info |
| **DtSecAbort** | Security fault during **data transfer** (signed, not ICV-only) |
| **ApplAbort** | Protected protocol abort |
| **ReleaseReq / ReleaseRsp** | Association teardown (**ClearToken3** + signature) |

All carry **`tbs`** (signed) + **`sign`** per §13.4.1 encoding rules.

### §13.2 Data transfer phase (p. 49)

#### §13.2.1 General

After successful association until termination. Data may be sent **in clear** or **encrypted**.

#### §13.2.2 Clear data transfer — **ClearTransfer**

```
ClearTransfer ::= SEQUENCE {
  tbp SEQUENCE {
    token2   ClearToken2,
    applData ApplData,          -- PrPDU (e.g. MMS confirmed service)
    ... },
  auth Authenticator }          -- ICV per §13.4.2
```

| Component | Rule |
|-----------|------|
| **tbp** | Integrity-protected area |
| **token2** | **ClearToken2** (§13.3.2) — assoID, time, seq, optional iv/rekey |
| **applData** | Protected protocol **PrPDU** |
| **auth** | **Authenticator** — ICV over **tbp** content |

**ICV encoding rules:**

| Encoding | Exclude from ICV input |
|----------|------------------------|
| **BER** | **tbp** start tag + length field (include trailing zeros if indefinite length) |
| **XML** | **tbp** start and end tags |

#### §13.2.3 Encrypted data transfer — **EncrTransfer**

```
EncrTransfer ::= SEQUENCE {
  tbp SEQUENCE {
    token2   ClearToken2,
    applData Encrypted-ApplData,  -- OCTET STRING: encrypted encoded ApplData
    ... },
  auth Authenticator OPTIONAL }   -- absent when authenticated encryption (AE) used

Encrypted-ApplData ::= OCTET STRING
```

| Mode | **auth** component |
|------|-------------------|
| **Non-AE** (separate enc + ICV) | **Present** — ICV over **tbp** (same rules as ClearTransfer) |
| **Authenticated encryption (AE/GCM)** | **Absent** — **ClearToken2** is **additional authenticated data (AAD)** per **RFC 5084** |

**AE / ICV encoding (pp. 49–50):**

| Item | Rule |
|------|------|
| **ClearToken2 as AAD** | In AE mode, encoded **ClearToken2** is AAD input to AES-GCM (RFC 5084) |
| **ClearToken2 BER** | Exclude start tag + length field (same convention as **tbs**/**tbp**) |
| **ClearToken2 XML** | Exclude start and end tags |
| **auth (non-AE)** | ICV per §13.4.2 over **tbp**; BER/XML exclude **tbp** tags as in ClearTransfer |
| **auth (AE)** | Component **absent** — integrity in ciphertext + AAD |

**Encryption scope:**

| Encoding | Encrypted octets |
|----------|------------------|
| **BER** | From top ASN.1 tag of PrPDU through length-defined end |
| **XER** | From element after **ApplData** start tag through matching end tag |

**CCLI C1:** target **EncrTransfer** with **AES-GCM AE** when **applProtected=1**; fallback **ClearTransfer** + separate ICV for lab interop.

**OSI wrap:** §15.4.2 **OSI-ClearTransfer** / §15.4.3 **OSI-EncrTransfer** in presentation TD-PPDU.

### §13.3 ClearToken data types (pp. 50–56)

#### ClearToken1 — handshake (§13.3.1, p. 50)

**ASN.1:**

```
ClearToken1 ::= SEQUENCE {
  sigAlg     AlgorithmIdentifier {{SupportedSignatureAlgorithms}},
  pkCert     PKCert,
  certPath   CertPath OPTIONAL,
  version    Version {v1},
  assoID     AssoID,
  dhKey      DiffieHellmanSet,
  hmac       AlgorithmIdentifier {{SupportedHmacAlgorithms}},
  time       TimeStamp,
  encr-mode  CHOICE {
    aea        SEQUENCE SIZE (1..MAX) OF AlgorithmIdentifier {{Supported-AEA-algorithms}},
    non-aea [0] SEQUENCE {
      encr     [0] SEQUENCE SIZE (1..MAX) OF AlgorithmIdentifier {{Supported-encrypt-algorithms}} OPTIONAL,
      icvAlgID [1] SET SIZE (1..MAX) OF AlgorithmIdentifier {{Supported-ICV-Algorithms}} },
    ... },
  confParams ConfidentialityParms,
  attCert    ACert OPTIONAL,
  ... }

Version   ::= BIT STRING { v1 (0) }
AssoID    ::= NMTOKEN (PATTERN "\d{1,3}:\d{1,3}|\d{1,3}")
TimeStamp ::= GeneralizedTime
PKCert    ::= OCTET STRING  -- DER X.509 Certificate
ACert     ::= OCTET STRING  -- DER AttributeCertificate
```

| Field | Purpose |
|-------|---------|
| **sigAlg** | Signature algorithm (in integrity-protected area) |
| **pkCert** | Sender end-entity cert (DER) |
| **certPath** | Optional CA chain (short path preferred) |
| **version** | E2E version bit-string (**v1** only today) |
| **assoID** | Client/server pair ID (`nnn:nnn` pattern) |
| **time** | Handshake creation UTC |
| **dhKey** | Diffie-Hellman public key |
| **hmac** | HMAC algorithm for **HKDF** |
| **encr-mode** | **aea** (AES-GCM combined) or **non-aea** (separate encr + ICV algs) |
| **confParams** | Bit flags: **tlsProtected**, **applProtected**, **vpn** |
| **attCert** | Optional attribute cert (62351-8 RBAC) |

**Diffie-Hellman (§13.3.1.2):**

| Algorithm | Group ID | Reference |
|-----------|----------|-----------|
| MODP RSA/DH | **14** (2048-bit) | RFC 3526 — **shall support** |
| ECDH | **23** (secp256r1) | RFC 5114 |
| ECDH optional | **28** (brainpoolP256r1) | §8.3 |

**Key derivation (§13.3.1.3–4) — HKDF RFC 5869:**

- **IKM** = DH shared secret **Z**
- **Salt** = chained hash of handshake `tbs` contents: `salt₁ = hash(data₂ || hash(data₁ || 0))`
- **OKM** = HKDF-expand(PRK, info absent, L) → **2 keys** (client/server) or **4 keys** (ICV + encrypt each direction)
- Key refresh: **15 min – 24 h** configurable; **ClearToken2.rekey** / **changedKey** handshake

**ConfidentialityParms bits:** set **`applProtected=1`** for C1 native E2E encryption target.

#### ClearToken2 — data transfer (§13.3.2, p. 55)

| Field | Purpose |
|-------|---------|
| **assoID** | From handshake |
| **time** | ICV creation UTC |
| **seq** | Sequence number (0 at new time; increment per message) |
| **iv** | Present if AES-CBC encryption |
| **rekey / reqRekey / changedKey** | DH key refresh coordination |

#### ClearToken3 — reject/abort (§13.3.3, p. 56)

**sigAlg**, **version**, **assoID** (if known), **time**, **pkCert**, optional **certPath** — authenticates reject/abort issuer.

### §13.4 Authentication and integrity (p. 57)

**Signature:** `algo` + `sign` OCTET STRING.

**Authenticator:** `nonce` (**mandatory for AES-GCM**), `algorithm` (ICV alg from HandshakeAcc), `icv`.

## §14 E2E security error handling (pp. 57–63)

### §14.2 Diagnostics

#### HandshakeDiagnostics (pp. 57–59)

| Code | Diagnostic |
|------|------------|
| 0 | no-reason-given |
| 1 | protocol-error |
| 2 | protected-protocol-not-supported |
| 3 | invalid-signatureAlgorithm |
| 4 | unexpected-version |
| 5 | hmac-algorithm-not-supported |
| 6 | encryption-not-required |
| 7 | encryption-required |
| 8 | invalid-public-key-certificate |
| 9 | invalid-certification-path |
| 10 | invalid-time-value |
| 11 | dhGroup-not-supported |
| 12 | illegal-dhGroup-selected |
| 13 | icv-algorithms-not-supported |
| 14 | encrypt-algorithms-not-supported |
| 15 | aea-algorithms-not-supported |
| 16 | invalid-attribute-certificate |
| 17 | encr-mode-aea-not-supported *(A1)* |
| 18 | ae-is-required |
| 19 | aea-select-but-encryp-not-supp |
| 20 | invalid-ae-algorithm |
| 21 | single-ae-algorithm-required |
| 22 | ae-not-used |
| 23 | invalid-encryption-algorithm |
| 24 | single-encrypt-algo-required |
| 25 | single-icv-algo-required *(A1)* |

#### DtDiagnostics (pp. 59–60)

| Code | Diagnostic | Trigger |
|------|------------|---------|
| 0 | no-reason-given | Default |
| 1 | protocol-error | Malformed SecPDU |
| 2 | unexpected-version | Version ≠ handshake |
| 3 | encryption-not-selected | Encrypted payload but no E2E enc |
| 4 | encryption-required | Cleartext but enc agreed |
| 5 | gmac-nonce-required | AES-GCM/GMAC without **nonce** |
| 6 | invalid-sequence-number | **seq** not +1 or not 0 after time change |
| 7 | aes-init-vector-required | AES-CBC without **iv** |
| 8 | unexpected-dhPublicKey | Server sees duplicate **rekey** without **changedKey** |
| 9 | unexpected-changedKeys | Client sees unexpected **changedKey** |

*(Page 60 also lists **gmac-init-vector-required** and **invalid-time-value** for ClearToken2 — 10 min skew / monotonic time.)*

### §14.3 Handshake checking (pp. 60–62)

**General:** Server exception in **HandshakeReq** → **HandshakeSecReject** (+ **diag** unless alarm raised). Client exception in **HandshakeAcc** → **HandshakeSecAbort**.

**§14.3.2 Signature:** Verify signature **before** validating signed data.

**ClearToken1 checks (§14.3.4) — selected failures:**

| Check | Outcome |
|-------|---------|
| Cert invalid (Annex G) | **invalid-public-key-certificate** or alarm |
| Subject ≠ sender address (G.6) | **address-mismatch** alarm |
| Cert path fail | **invalid-certification-path** |
| Time **> 10 min** UTC | **invalid-time-value** |
| Duplicate **time** + **assoID** | **replay detected** alarm |
| DH group unsupported / mismatch | **dhGroup-not-supported** / **illegal-dhGroup-selected** |
| HMAC alg unsupported / changed | **hmac-algorithm-not-supported** |
| **encr-mode** / **confParams** mismatch | AE/AEA/encrypt/ICV diagnostics (items l–aa) |
| **attCert** invalid | **invalid-attribute-certificate** |

### §14.4 Data transfer checking (p. 63)

Exception in **ClearTransfer** / **EncrTransfer** → **DtSecAbort** (+ **diag** unless alarm).

| Check | Outcome |
|-------|---------|
| Enc mismatch vs handshake | **encryption-not-selected** / **encryption-required** |
| ICV alg ≠ agreed | **invalid-icv-algorithm** alarm |
| ICV verify fail | **invalid ICV** alarm |
| **time** > 10 min or earlier than prior | **invalid-time-value** |
| Duplicate **time**+**seq** | **replay detected** alarm |
| **seq** not incremented | **invalid-sequence-number** |
| AES-CBC without **iv** | **aes-init-vector-required** |
| Client gets **dhPublicKey** | **protocol-error** |
| Server gets **changedKeys** | **protocol-error** |

---

## §15 E2E in OSI operational environment (pp. 64–68)

### §15.1–15.2 Upper layers (p. 64)

| Item | Requirement |
|------|-------------|
| **Presentation** | Include **`e2eSecurityAS`** abstract syntax + protected protocol syntax (§10.4.1) |
| **e2eSecurityAS OID** | `{ iso standard iec62351 part4 abstract-syntax e2eSecurity version1 }` |
| **ACSE AARQ/AARE** | **No** `calling/called-authentication-value`; **no** `mechanism-name`; **no** sender/responder-acse-requirements (kernel ACSE only) |
| **Security control** | In **`user-information`**, not auth-value (unlike A-profile §11) |
| **61850 context** | **`e2eMmsAC`** OID `{ appl-context e2eMMS(1) }` |

### Table 4 — SecPDU ↔ ACSE EnvPDU (p. 65)

| SecPDU | ACSE EnvPDU |
|--------|-------------|
| **HandshakeReq** | **AARQ-apdu** |
| **HandshakeAcc** / **ApplicationReject** / **HandshakeSecReject** | **AARE-apdu** |
| **HandshakeSecAbort** / **DtSecAbort** / **ApplAbort** / **OsiDiagnostics** | **ABRT-apdu** |
| **ReleaseReq** | **RLRQ-apdu** |
| **ReleaseRsp** | **RLRE-apdu** |

### §15.3 Association management mapping (pp. 65–67)

**Association-information:** `SEQUENCE SIZE (1..MAX) OF EXTERNAL` in ACSE **user-information**.

**AARQ (§15.3.2):**

```
OSI-AssoReq ::= SEQUENCE SIZE(1) OF [UNIVERSAL 8] SEQUENCE {
  indirect-reference INTEGER,          -- presentation-context-id (§10.4.1)
  single-ASN1-type [0] EXPLICIT HandshakeReq }
```

**AARE (§15.3.3):** `OSI-AssoRsp` with **HandshakeRsp** CHOICE:

| Choice | AARE result |
|--------|-------------|
| **assoAccept** → HandshakeAcc | **accepted**; diagnostic **null** |
| **applReject** | **rejected-permanent/transient**; **no-reason-given** |
| **secReject** | **rejected-permanent**; **no-reason-given** |

**ABRT (§15.3.4):** `OSI-AssoAbr` wraps **AssoAbort** CHOICE (HandshakeSecAbort / DtSecAbort / ApplAbort); **abort-source** = **acse-service-user**; **abort-diagnostic** absent.

**Release (§15.3.5–6):** `OSI-AssoRelReq/Rsp` with **ReleaseReq/Rsp** in **RLRQ/RLRE user-information**.

### §15.4 Data transfer mapping (pp. 67–68)

**General:** SecPDUs in presentation **User-data** (X.226 TD-PPDU).

**ClearTransfer (§13.2.2 / §15.4.2):**

```
OSI-ClearTransfer ::= SEQUENCE SIZE(1) OF SEQUENCE {
  presentation-context-identifier INTEGER,
  single-ASN1-type [0] EXPLICIT ClearTransfer }
```

**EncrTransfer (§13.2.3 / §15.4.3):** same structure with **EncrTransfer**.

**§15.5 Routing:** Intermediaries route **EnvPDUs** only — must not alter E2E content.

**CCLI C1 evidence path:** **`e2eMmsAC`** AARQ/AARE → **HandshakeReq/Acc** (`pp=IEC 61850-8-1`) → **ClearToken1** + **62351-9** key ceremony → **ClearTransfer/EncrTransfer** on MMS TD-PPDU → TLS **:3782**.

---

## §17 Conformance / PICS (pp. 75–78)

### §17.1–17.2 General

Compliance determined by: **§6** T-security · **§11** A-profile · **§13–14** E2E.

| Notation | Meaning |
|----------|---------|
| **m** | Mandatory |
| **o** | Optional |
| **c[n]** | Conditional (see footnote) |
| **x** | Excluded |
| **i** | Out of scope |

### Table 6 — Operational environment (p. 76)

| Environment | Client/Server |
|-------------|---------------|
| **OSI** | **c[2]** — at least one row **m** both sides per referencing spec |
| **XMPP** | **c[2]** |

**CCLI:** declare **OSI** (**61850-8-1**).

### Table 7 — Mode of operation (p. 76)

| Mode | CCLI |
|------|------|
| **Compatibility** (2007 interop) | **c[3]** — optional on OSI |
| **Native** (extended E2E) | **c[4]** — **m** on OSI for C1 |

### Table 8 — Compatibility TLS ciphers (p. 76)

Legacy **3DES/AES-SHA** suites — **o** except **TLS_DH_DSS_WITH_AES_256_CBC_SHA** (**m**). **CCLI:** do not declare compatibility mode.

### Table 9 — Encryption mode (p. 77)

| Mode | Requirement |
|------|-------------|
| **Authenticated encryption (AE)** | **c[2]** — at least one side **m** |
| **Non-use AE** (separate enc+ICV or no enc) | **c[2]** |

**CCLI lab PICS:** declare **AE (AES-GCM)** preferred; non-AE path as fallback.

### Table 10 — Native TLS cipher suites (p. 77)

**Mandatory (m)** — matches §6 Table 3:

| Suite | IANA |
|-------|------|
| TLS_RSA_WITH_AES_128_CBC_SHA256 | 0x003C |
| TLS_DH_RSA_WITH_AES_128_GCM_SHA256 | 0x00A0 |
| TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 | 0xC09E |
| TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 | 0xC023 |

### Table 11 — E2E cryptographic algorithms (p. 78)

| Category | Algorithm | Status |
|----------|-----------|--------|
| Public-key | **rsaEncryption** | **m** |
| | **ecPublicKey** secp256r1, brainpoolP256r1 | **m** |
| Signature | **sha256WithRSA**, **ecdsa-with-SHA256** | **m** |
| Symmetric (non-AE path) | **aes128/256-CBC** | **c[5]** |
| Authenticated enc | **aes128/256-GCM** | **c[6]** |
| ICV | **hmacWithSHA256**, **aes128/256-GCM** | **c[7]** |

**CCLI native PICS declaration:** OSI · native mode · Table 10 mandatory suites · **AES-GCM AE** + Table 11 **m** rows.

---

## Annex G — End-entity public-key certificate (normative) (pp. 103–106)

**Scope:** Structure/content of **E2E application-layer** end-entity certs (`ClearToken1.pkCert`). Does **not** define CA/trust-anchor certs.

### G.2–G.3 General

| Rule | Requirement |
|------|-------------|
| **Dual certs** | If TLS **and** app-layer security → **separate** end-entity certs (**shall**) |
| **Size budget** | OSI Session CONNECT extended user data **≤ 10 240 octets** (CP-type + AARQ/AARE + **user-information**) |
| **Recommendation** | Minimize cert size — affects AARQ/AARE fit and §11 max **8192** octets |

### G.4 Basic structure

| Component | Rule |
|-----------|------|
| **version** | **v3** (**shall**) |
| **serialNumber** | Non-sequential; **128-bit UUID** (ITU-T X.667); **shall not** be negative (MSB ≠ 1) — regenerate if negative |
| **issuer sig alg** | Must match CA signature alg on cert — mismatch → **invalid** |
| **Hash / keys** | Hash **SHA-256 or stronger**; **RSA ≥2048 bits**; **EC ≥256 bits** |
| **issuer / subject DN** | Globally unique; keep minimal; FQDN allowed in **dnsName** |
| **validity** | Typical 2–3 yr (person); up to 10 yr in stable env — balance vs CRL size + crypto lifetime |
| **subjectPublicKeyInfo** | **rsaEncryption** or **ecPublicKey** with **secp256r1** or **brainpoolP256r1** |
| **issuerUniqueID / subjectUniqueID** | **Absent** (**shall**) |

### G.5 Extensions

| Extension | Rule |
|-----------|------|
| **General** | Minimize extension count (X.509); per **ISO/IEC 9594-8** unless noted |
| **keyUsage** | **Present** — **digitalSignature** + **keyAgreement** bits set |
| **CRL distribution points** | If CRL revocation — **distributionPoint** present when CA = CRL issuer; **reasons** absent; **cRLIssuer** if CRL issuer ≠ CA |
| **authorityInfoAccess** | If **OCSP** revocation (RFC 5280) — details in **62351-9** |
| **IEC user role** (62351-8) | Only when privilege info included (62351-8 §11.5.1.2) |

**CCLI:** pick **OCSP** or **CRL** consistently with **62351-9** ceremony; TPM stores **E2E app cert** separately from **TLS :3782** cert.

### G.6 Operational environment binding (OSI — C1)

**Purpose:** Enable §14.3.4 **address-mismatch** alarm — subject must match sending entity address.

**G.6.2 OSI (61850 MMS on C1):**

| Item | Value |
|------|-------|
| **Subject DN** | Single RDN: **objectIdentifier** (X.520) |
| **Attribute value** | **`calling-AP-title` + `calling-AE-qualifier`** (client AARQ) or **`responding-AP-title` + `responding-AE-qualifier`** (server AARE) — see **§10.5.3** |

**TG-524 PKI profile (CR 1.8):**

```
Cert A (TLS)     → port 3782 server/client auth (62351-3)
Cert B (E2E)     → ClearToken1.pkCert; subject OID binds MMS AP/AE identity
CA               → DSO or HiTEKS intermediate per 62351-9
Revocation       → OCSP or CRL (one per cert)
Store            → TPM NV / OpenWrt cert bundle (separate paths)
```

**62443 trace:** CR **1.8** PKI · CR **1.9** auth · §14.3.4 cert validation against Annex G.

---

## CCLI conduit mapping

| Conduit | Mode | Stack | 62443 |
|---------|------|-------|-------|
| **C1** Eth_A | **Native** | `e2eMmsAC` + E2E §13 + TLS **:3782** + Table 3 | CR 1.8, 1.9, 3.1, 4.1 |
| C1 fallback | Compatibility | A-profile §11 + TLS | Policy-gated; not default |
| **C5** LTE | Native/compatibility | TLS minimum | CR 1.13 |

---

## Interface matrix

| Interface | Source | Destination | Protocol / profile |
|-----------|--------|-------------|-------------------|
| **C1** | TG-524 Eth_A | DSO SCADA | 61850 MMS + **62351-4 native E2E** + **62351-3 TLS** |
| C1 legacy | TG-524 | Legacy IED | Compatibility A-profile (phased) |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| §3 terms | Not captured | Definitions | pp. **12–16** |
| §5.2 61850 | **HAVE** | `basic-AC` / `e2eMmsAC` | — |
| §6.2.1–6.2.7 | Not captured | TLS general, renegotiation, cert size | pp. **22–23** |
| §6.3–6.4 / Tables 2–3 | **HAVE** | Ports, ciphers, cert events | — |
| §8 crypto | **HAVE** | §8.3–8.8 algorithms + §9 OID | pp. **28** gap |
| §10 OSI upper layer | Not captured | Session/presentation/ACSE base | pp. **33–36** |
| §11 A-profile | **HAVE** | MMS auth value + AARQ/AARE rules | — |
| §12–13 E2E | **HAVE** | Handshake + ClearToken1/2/3 + **ClearTransfer/EncrTransfer** | — |
| §14–15 OSI map | **HAVE** | Diagnostics + Table 4 + ACSE/TD mapping | — |
| §17 PICS | **HAVE** | Tables 6–11 native declaration | Lab form TBD |
| Annex G | **HAVE** | E2E end-entity cert spec + OSI subject binding | — |
| 62351-4:2007 TS | Separate doc | Legacy A+T only | Optional parallel |
| K6.3 ceremony | **HAVE** | `ccli-k63-ceremony-sop` | Lab EST + dual cert |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-354-001 | libiec61850 lacks native E2E | C1 cannot ship Allegato O E2E | Evaluate §17 + stack matrix |
| R-354-002 | Compatibility mode default | Weak A-profile-only path | §4.3 — default **native** |
| R-354-003 | A-profile + TLS without E2E | No data-transfer E2E auth | Target Table 1 row E2E+TLS |
| R-354-005 | TLS disabled on C1 in production | Cleartext MMS | §6.3.3 — lab only |
| R-354-006 | Compatibility ciphers (Table 2) | Weak crypto | Native mode + Table 3 only |
| R-354-007 | RSA < 2048 in A-profile | Non-conformant MMS auth | §11.2.2 enforce 2048+ |
| R-354-008 | Clock skew > 10 min | P-ABORT / handshake fail | GNSS sync (CR 2.11) |
| R-354-009 | libiec61850 E2E ClearToken/HKDF | Native mode blocked | Stack eval before lab quote |
| R-354-011 | Dual cert provisioning error | TLS cert used for E2E | Annex G.2 — separate certs |
| R-354-012 | PICS not filed with lab | Certification delay | Declare Tables 6–11 before quote |
| R-354-013 | E2E cert exceeds CONNECT budget | AARQ reject / oversized PDU | Minimize extensions; monitor DER size |
| R-354-014 | Subject OID ≠ AP/AE title | **address-mismatch** alarm | G.6.2 + provisioning SOP (K6.3) |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| Batch A §4 | Table 1 + modes captured | Native vs compatibility documented | **PASS** |
| Batch B §5.2+§6 | 61850 contexts + TLS ports/ciphers | Mapped to C1 OpenWrt | **PASS** *(§6.2.1–7 gap)* |
| Batch C §8+§11 | Crypto + A-profile captured | PKI + replay rules documented | **PASS** |
| Batch D §12–13 | E2E handshake + ClearTokens + §13.2 | CR 1.8/3.1 trace | **PASS** |
| Batch E §14–15+§17 | OSI mapping + PICS Tables 6–11 | Lab declaration template | **PASS** |
| Batch F Annex G | E2E cert profile + G.6.2 OSI binding | CR 1.8 PKI design input | **PASS** |
| C1 lab | MMS-TLS + E2E interop | `e2eMmsAC` + ClearTransfer | Before DSO demo |

---

## Source pages

### Batch A — Foundation (pp. 8–11, 18–21)

| Page | Content | Status |
|------|---------|--------|
| **8** | Foreword (TC 57; FDIS 57/2032) | PASS |
| **9** | Foreword tail; code typography; CCv1 | PASS |
| **10** | **§1.1** scope — 2007 extension, E2E, OSI+Internet | PASS |
| **11** | **§1.2** code zip; **§2** normative refs start | PASS |
| **12** | **§2** refs tail — X.509, RFC 5246 TLS 1.2, MMS | PASS *(partial)* |
| **18** | **§4.1 Fig 1** profiles; **§4.2** A/T-profile, TLS | PASS |
| **19** | **Table 1** combinations; **§4.3** modes | PASS |
| **20** | **§4.4.2–4.4.3** threats; **§4.5.1–4.5.2** attacks | PASS |
| **21** | **§4.5.3** E2E attacks; **§4.6** logging; **§5.1** ICCP start | PASS |

### Batch B — 61850 + TLS (pp. 22–27)

| Page | Content | Status |
|------|---------|--------|
| **22** | §5.1 app-security choices; **§5.2** `basic-AC` / **`e2eMmsAC`** | PASS |
| **22–23** | §6.2.1–6.2.7 TLS general *(not in batch)* | GAP |
| **24** | **§6.2.8–6.2.9** cert validation + events; **§6.3.1–6.3.2 Fig 2** ports 102/3782 | PASS |
| **25** | **§6.3.3** TLS disable; **§6.3.4.1–6.3.4.2** compat ciphers | PASS |
| **26** | **Table 2** legacy 2007 ciphers | PASS |
| **27** | **Table 3** native mandatory ciphers; **§6.4** XMPP; **§7.1** A vs E2E | PASS |

### Batch C — Crypto + A-profile (pp. 29–32, 37–41)

| Page | Content | Status |
|------|---------|--------|
| **29** | **§7.2.4** E2E XML namespace; **§8.1–8.2** ALGORITHM / AlgorithmIdentifier | PASS |
| **30** | **§8.3–8.5** RSA/EC, SHA-256, signature algorithms | PASS |
| **31** | **§8.6–8.7** AES-CBC IV rules; AES-GCM start | PASS |
| **32** | **§8.7–8.8** GCM params; **hmacWithSHA256**; **§9** OID root | PASS |
| **37** | **§10.5.4** EXTERNAL encoding; **§11** A-profile start | PASS *(§10 partial)* |
| **38** | **§11.1.2–11.1.4** session/presentation/ACSE; **`basic-AC`** | PASS |
| **39** | **§11.1.4.3–11.2.1** mechanism-name; Authentication-value | PASS |
| **40** | **§11.2.2–11.2.3** MMS-Authentication-value; AARQ client/server | PASS |
| **41** | **§11.2.4** AARE; **§12.1** Figs 5–6 E2E architecture | PASS |
| **42–43** | **§12.1–12.2** Figs 7–8 PrPDU/SecPDU/EnvPDU; virtual-API | PASS |
| **44** | **§13.1.1–13.1.3** association; UTC time; **HandshakeReq** | PASS |
| **45** | **§13.1.4** **HandshakeAcc**; MMS Initiate-Request/Response | PASS |
| **46–47** | **§13.1.5–13.1.9** reject/abort SecPDUs | PASS |
| **48** | **§13.1.10–11** ReleaseReq/Rsp | PASS |
| **49** | **§13.2** ClearTransfer · EncrTransfer · ICV/encryption rules | PASS |
| **50–54** | **§13.3.1** **ClearToken1**; DH; **HKDF**; confParams; certPath | PASS |
| **55** | **§13.3.2** **ClearToken2**; key refresh | PASS |
| **56** | **§13.3.3** **ClearToken3** | PASS |
| **57** | **§13.4** Signature + Authenticator; **§14.2.1** diagnostics start | PASS |
| **58** | **§14.2.1** HandshakeDiagnostics enum 3–13 + descriptions | PASS |
| **59** | HandshakeDiagnostics 14–25; **§14.2.2** DtDiagnostics start | PASS |
| **60** | DtDiagnostics values; **§14.3** handshake checking | PASS |
| **61** | **§14.3.2–14.3.4** signature + ClearToken1 checks | PASS |
| **62** | ClearToken1 encr-mode checks (l–y) | PASS |
| **63** | **§14.4** data transfer checking | PASS |
| **64** | **§15.1–15.3.1** OSI E2E; **`e2eMmsAC`**; ACSE rules | PASS |
| **65** | **Table 4** SecPDU↔ACSE; **§15.3.2** AARQ mapping | PASS |
| **66** | **§15.3.3–15.3.4** AARE + ABRT mapping | PASS |
| **67** | **§15.3.5–6** release; **§15.4.1–15.4.2** ClearTransfer | PASS |
| **68** | **§15.4.3–15.5** EncrTransfer; OSI routing | PASS |
| **75** | **§17.1–17.3** conformance general + notation | PASS |
| **76** | **Tables 6–8** environment · mode · compat ciphers | PASS |
| **77** | **Tables 9–10** encryption mode · native TLS | PASS |
| **78** | **Table 11** E2E crypto algorithms | PASS |

### Batch F — Annex G PKI (pp. 103–106)

| Page | Content | Status |
|------|---------|--------|
| **103** | **Annex G.1–G.4.3** scope; dual certs; 10 240 oct limit; v3; UUID serial; sig alg | PASS |
| **104** | **G.4.4–G.4.7** issuer/validity/subject; **E2EPublicKeyAlgorithms** ASN.1 | PASS |
| **105** | **G.4.8–G.5.3** unique IDs absent; **keyUsage**; CRL vs OCSP | PASS |
| **106** | **G.5.4–G.6.3** 62351-8 role ext; **G.6.2 OSI objectIdentifier** subject binding | PASS |

### Batch G — §13.2 data transfer (p. 49)

| Page | Content | Status |
|------|---------|--------|
| **49** | **§13.2.1–13.2.3** **ClearTransfer** / **EncrTransfer** ASN.1; **tbp** + **auth** ICV rules | PASS |

### Optional capture (non-P0)

```
12–16, 22–23, 33–36   ← §3 terms · §6.2 TLS general · §10 OSI upper layer base
```

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-08-11 | Skeleton + capture plan |
| **1.1** | 2026-08-12 | **Batch A** pp. 8–11, 18–21: §1 · §2 partial · §4 · §5.1 start |
| **1.2** | 2026-08-12 | **Batch B** pp. 22–27: §5.2 · §6 TLS · Tables 2–3 · §7.1 start |
| **1.3** | 2026-08-12 | **Batch C** pp. 29–32, 37–41: §8 crypto · §11 A-profile MMS auth |
| **1.4** | 2026-08-12 | **Batch D** pp. 41–57: §12 E2E model · §13 handshake · ClearToken1/2/3 |
| **1.5** | 2026-08-12 | **Batch E** pp. 58–68, 75–78: §14 diagnostics · §15 OSI mapping · §17 PICS |
| **1.6** | 2026-08-12 | **Batch F** pp. 103–106: **Annex G** E2E end-entity cert + OSI subject binding |
| **1.7** | 2026-08-12 | **Batch G** p. **49**: **§13.2** ClearTransfer/EncrTransfer — **C1 P0 complete** |

---

## RAG cross-links

| source_id | Document |
|-----------|----------|
| `ccli-62351-4-extract` | This document |
| `ccli-62351-4-capture-plan` | ToC + batch order |
| `ccli-62351-1-extract` | Series intro · Fig 9 |
| `ccli-62351-3-extract` | TCP/TLS transport |
| `ccli-62351-9-extract` | PKI ceremony · CRL/OCSP |
| `ccli-62443-zones-extract` | C1 conduit |
| `ccli-62443-4-2-extract` | CR 1.8 PKI |

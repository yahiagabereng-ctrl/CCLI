# K6.3 — Key Hierarchy & Provisioning Ceremony SOP

**Document ID:** CCLI-SEC-K63-001  
**Revision:** 1.2  
**Date:** 2026-08-12  
**RAG source_id:** `ccli-k63-ceremony-sop`  
**Platform:** TesPro **TG-524** (OpenWrt / MT798X) · Pi mock (`platform_pi`) until hardware receipt  
**Normative basis:** IEC **62351-9:2023** · **62351-3** (TLS) · **62351-4 Annex G** (E2E cert) · **62351-14** (Syslog) · IEC **62443-4-2 CR 1.8**  
**Parent extracts:** `ccli-62351-9-extract` · `ccli-62351-3-extract` · `ccli-62351-4-extract` · `ccli-62443-4-2-extract`  
**Knowledge node:** **K6.3** · Risk **KR-006**

---

## Summary

This SOP defines **who does what, when, and with what evidence** to provision, operate, renew, and revoke **public-key credentials** on the CCLI. It implements:

- **IDevID** (factory, IEEE 802.1AR) → **LDevID** (operational domain)
- **Dual end-entity certificates** on C1 MMS: **Cert A (TLS :3782)** + **Cert B (E2E Annex G)**
- **EST** enrolment (preferred) over **62351-3 TLS**; **SCEP** as backward-compatible alternative
- **CRL and/or OCSP** revocation per cert (one method per certificate)
- **62351-9 Annex D** security events for lab and field logging (target: **62351-14** when available)

**Out of scope:** GDOI group keys (62351-9 §8 / 62351-6) — P2, not C1 MMS P0.

---

## Requirements

| REQ-ID | Source | Requirement | Verification |
|--------|--------|-------------|--------------|
| **REQ-K63-001** | 62351-9 §5.8.1 | Proof of **identity** + proof of **possession** (CSR signature) before issuance | RA audit log |
| **REQ-K63-002** | 62351-9 §5.8.2–3 | **IDevID** triplet at factory; **LDevID** triplet after domain onboarding | TPM inventory |
| **REQ-K63-003** | 62351-9 §7.3.6 | **≥5** trust anchors configurable on device | `uci`/config inspect |
| **REQ-K63-004** | 62351-9 §7.3.7.3 | **EST preferred**; enrolment over **62351-3 TLS** | Lab EST trace |
| **REQ-K63-005** | 62351-4 Annex G.2 | **Separate** certs for TLS and E2E when both enabled | Two distinct serial numbers |
| **REQ-K63-006** | 62351-4 Annex G.6.2 | E2E cert **subject OID** = MMS AP-title + AE-qualifier | address-mismatch test |
| **REQ-K63-007** | 62351-9 Annex C.2 | Renewal trigger at **70%** of cert max lifetime | Timer / cron evidence |
| **REQ-K63-008** | 62351-9 §7.7 | Clock sync (**PTP/GNSS** or **NTP/NTS**) before validity checks | §7.7 skew test |
| **REQ-K63-009** | 62443-4-2 CR 1.8 | Documented PKI ceremony when public-key auth used | This SOP + records |
| **REQ-K63-010** | 62351-9 Annex D | Log enrolment/revocation events with **IEC 62351-9:1.x / 4.x** IDs | SIEM / syslog sample |

---

## Architecture

### Key hierarchy

```
Manufacturer CA ──► IDevID (802.1AR) ──► factory trust anchor in TPM
                           │
                           ▼ field commissioning (EST/SCEP)
DSO / operator CA ──► LDevID + Cert A (TLS) + Cert B (E2E)
                           │
                           ▼
                    CRL / OCSP responder (substation proxy optional — 62351-9 Fig 16)
```

### Functional roles

| Role | Responsibility |
|------|----------------|
| **Manufacturer** | Issue **IDevID**; imprint manufacturer trust anchor; optional **RFC 8366** voucher |
| **DSO PKI admin (RA/CA)** | Register device; approve CSRs; issue **Cert A/B**; publish CRL/OCSP |
| **Commissioning engineer** | Configure trust store, EST URL, subject templates; run ceremony checklist |
| **TG-524 CCI (device)** | Generate keys in **TPM**; CSR; validate issued certs; renewal timer; emit events |
| **Substation proxy** *(optional)* | OCSP stapling / CRL fetch for constrained peers (62351-9 §5.9.2 Fig 16) |

### Security

| Control | Implementation |
|---------|----------------|
| Private key non-export | TPM 2.0 or OpenWrt secure keystore — **62351-9 §7.3.2** |
| Enrolment channel | **TLS 1.2** per **62351-3** to RA/CA |
| Trust anchor update | **TAMP** (RFC 5934) optional; change-control record required |
| Compromise | **62351-9 §5.9.4** — local technician wipe + re-enrol via surviving **IDevID/LDevID** |
| Decommission | **62443 CR 4.2** — factory reset clears operational keys; retain audit trail |

### Network (enrolment path)

| Leg | Protocol | Port / path |
|-----|----------|-------------|
| Device → RA/CA | **62351-3 TLS** | EST: `https://<ra>/.well-known/est/simpleenroll` |
| Device → OCSP/CRL | HTTP/LDAP per cert AIA/CRLDP | Via Eth_A or proxy |
| C1 MMS peer | TLS **:3782** + E2E §13 | Post-ceremony only |

---

## Certificate profiles (C1)

### Cert A — TLS transport (62351-3)

| Field | Value |
|-------|-------|
| **Purpose** | MMS transport security on **TCP :3782** |
| **keyUsage** | **digitalSignature** (mandatory for TLS client/server) |
| **extendedKeyUsage** | **clientAuth** + **serverAuth** (both mandatory per 62351-9 §7.4.4.10.7) |
| **subjectAltName** | Eth_A **IP** and/or **FQDN** of CCI |
| **subject DN** | Minimal: **CN**, **O**, **serialNumber** (device ID) |
| **Algorithms** | RSA ≥2048 or ECDSA **secp256r1**; sig **SHA-256+** |
| **Revocation** | **OCSP** *or* **CRL** (pick one; document in RA policy) |
| **Store path** | e.g. `/etc/ssl/ccli/tls-cert.pem` + TPM-wrapped key |

### Cert B — E2E application (62351-4 Annex G)

| Field | Value |
|-------|-------|
| **Purpose** | `ClearToken1.pkCert` in MMS E2E handshake |
| **keyUsage** | **digitalSignature** + **keyAgreement** |
| **subject** | Single RDN: **objectIdentifier** = **calling-AP-title + calling-AE-qualifier** (client) or **responding-*** (server) per **62351-4 §10.5.3 / G.6.2** |
| **serialNumber** | **128-bit UUID** (non-negative integer) |
| **Size** | Target **≤8192 octets** DER (Session CONNECT budget **10240 octets**) |
| **Revocation** | Same method family as Cert A; **independent** CRL/OCSP config allowed |
| **Store path** | Separate from Cert A — e.g. `/etc/ssl/ccli/e2e-cert.pem` |

**Never** reuse Cert A private key or certificate for E2E (**Annex G.2**).

---

## Ceremony procedures

### Phase 0 — Factory (Manufacturer)

**Trigger:** First power-on at manufacturer or HiTEKS pre-provision bench.

| Step | Action | Pass criteria |
|------|--------|---------------|
| F1 | Generate device key pair in **TPM** | Key handle recorded |
| F2 | Register with manufacturer RA; obtain **IDevID** cert + chain | Chain validates to mfg trust anchor |
| F3 | Store **IDevID triplet**: cert + private key + trust anchor | TPM inventory |
| F4 | Record **serialNumber** / asset tag ↔ device UID | ERP ↔ engineering DB |
| F5 | *(Optional)* Issue **RFC 8366** voucher for zero-touch domain join | Voucher signed by mfg root |

**Evidence:** Factory provision record **FR-K63-001** (template below).

---

### Phase 1 — Domain onboarding (Commissioning)

**Trigger:** CCI arrives on site; engineer has DSO PKI credentials.

| Step | Action | Pass criteria |
|------|--------|---------------|
| O1 | Verify **IDevID** present and valid | `CRED_*` no errors |
| O2 | Load **≥5** DSO trust anchors (**62351-9 §7.3.6**) | Config audit |
| O3 | Configure EST endpoint, RA auth (IDevID re-use or OTP per **§5.8.4**) | UCI committed |
| O4 | Sync time (**GNSS/PTP** or **NTP/NTS**) — **§7.7** | Skew within 62351-4 **10 min** for E2E |
| O5 | Register device CN/serial with DSO RA | RA shows **pending** → **approved** |

**Evidence:** Commissioning record **CR-K63-001**.

---

### Phase 2 — Enrolment (Annex C Figure C.1)

Repeat **once for Cert A** and **once for Cert B** (distinct key pairs and CSRs).

| Step | Action | Event on success / failure |
|------|--------|----------------------------|
| E1 | **Trigger:** no cert / expired / config change / operator command | — |
| E2 | Generate **new key pair on device** (never accept CA-generated private key without OOB policy) | — |
| E3 | Build **PKCS#10 CSR** with profile-specific extensions (`extensionRequest`) | — |
| E4 | **EST:** TLS to RA → optional `cacerts` → `simpleenroll` with `tls-unique` binding | **1.5** / **1.6** |
| E5 | RA validates identity + CSR signature | RA log |
| E6 | Device validates issued cert: path, validity, EKU/KU, SAN/OID | **2.x** on failure |
| E7 | Store cert + chain in dedicated slot; **do not overwrite** other cert | — |

**SCEP fallback:** Use OTP activation (**§5.8.6.1**); events **1.3** / **1.4**.

**Pi mock:** Same procedure; keys in software store labelled `platform_pi` — **not** production evidence.

---

### Phase 3 — Operate

| Activity | Cadence | Action |
|----------|---------|--------|
| **Revocation check** | Each TLS connect + periodic | OCSP or CRL per cert; do **not** tear down session solely on CRL fetch failure (**62351-3 §5.6.4.4**) |
| **Renewal timer** | Continuous | At **70%** of cert **notAfter − notBefore**, start re-enrol (**Annex C.2**) |
| **CRL refresh** | ≤ **24 h** (**62351-9 §7.5**) | Proxy or direct fetch |
| **Event logging** | Real-time | Annex D → **62351-14** RFC 5424 · **TCP/6514 TLS** |

---

### Phase 4 — Renewal (Figure C.2)

```
Monitor → at 70% lifetime → new key pair + CSR → EST re-enrol
         → validate → atomic switch → log CRED_ENR_EST_SUCC (1.5)
Expired? → immediate ENROLL (no grace for new connections)
```

| Step | Action |
|------|--------|
| R1 | Generate **new** key pair (recommended) or reuse per local policy |
| R2 | Re-enrol before **notAfter**; overlap window ≥ 1 CRL period |
| R3 | Retain old cert until all peers refreshed or policy timeout |
| R4 | Log success/failure (**62351-9 §7.6**) |

---

### Phase 5 — Compromise / revocation

| Scenario | Procedure |
|----------|-----------|
| **Cert compromised** | DSO revokes cert; device gets **4.1** alarm; block new sessions; re-enrol Phase 2 |
| **TPM/key compromise** | **§5.9.4** local technician: wipe operational keys; re-onboard from **IDevID** if intact |
| **CA compromise** | **TAMP** or manual trust anchor rotation; re-enrol all entities |
| **Decommission** | **CR 4.2** factory reset; wipe **LDevID**, Cert A/B; retain **FR/CR** records offline |

---

## Interface matrix

| Interface | Source | Destination | Protocol / artefact |
|-----------|--------|-------------|---------------------|
| Factory provision | Manufacturer RA | TG-524 TPM | PKCS#10 / offline |
| Domain onboard | Commissioning tool | TG-524 UCI | SSH / local console |
| Enrolment | TG-524 | DSO RA/CA | **EST** over **62351-3 TLS** |
| Trust update | DSO CA | TG-524 | **TAMP** (optional) |
| Revocation status | TG-524 | OCSP/CRL DP | RFC 6960 / RFC 5280 |
| C1 MMS | TG-524 Eth_A | DSO peer | TLS **:3782** + E2E §13 |
| Security events | TG-524 Eth_mgmt | SIEM | **62351-14** Syslog · SD `62351-14@41912` |

---

## BOM matrix

| Block | Function | Candidate / PN | Ceremony role |
|-------|----------|----------------|---------------|
| TPM 2.0 | Key storage | On-board TG-524 | IDevID, LDevID, Cert A/B keys |
| DSO CA/RA | Issuance | Customer PKI | Cert A + Cert B |
| EST server | Enrolment | e.g. EJBCA, step-ca, Microsoft AD CS + EST | `/simpleenroll` |
| OCSP responder | Revocation | With CA | Optional per cert |
| GNSS module | Time | TG-524 modem option | §7.7 validity |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| DSO CA product | Unknown | RA/EST profile for lab | **EXTERNAL** |
| 62351-14 event schema | **HAVE (Batch A–C)** | 62351-3/4 Annex B events | Capture **Batch D** |
| 62351-8 RBAC attr certs | Not ingested | Optional Profile A roles | P1 |
| TG-524 TPM API | PART | Confirm PKCS#11/ESAPI on OpenWrt | TesPro receipt test |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-K63-001 | Single cert for TLS + E2E | Protocol reject / wrong KU | Dual ceremony Phase 2 ×2 |
| R-K63-002 | E2E subject OID wrong | **address-mismatch** alarm | G.6.2 template in CSR |
| R-K63-003 | Renewal missed | Outage on expiry | 70% timer + monitoring **2.6** |
| R-K63-004 | EST without TLS hardening | MITM enrolment | 62351-3 cipher suites on enrolment leg |
| R-K63-005 | Pi mock keys in production | False certification | Separate `platform_pi` policy banner |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-K63-001 | CSR signature verify at RA | RA accepts; PoP proven | Lab |
| REQ-K63-005 | Inspect two certs | Distinct SKI/serial; different EKU profile | Lab |
| REQ-K63-006 | MMS connect wrong AP/AE | **address-mismatch** diagnostic | Lab |
| REQ-K63-004 | Wireshark EST over TLS | `CRED_ENR_EST_SUCC` logged | Lab |
| REQ-K63-007 | Force 70% timer | Re-enrol before expiry | Lab |
| REQ-K63-009 | Audit this SOP + FR/CR records | CR 1.8 evidence pack | Review |

---

## Evidence record templates

### FR-K63-001 — Factory provision

| Field | Value |
|-------|-------|
| Device UID | |
| IDevID serial | |
| Manufacturer CA fingerprint | |
| Date / operator | |

### CR-K63-001 — Commissioning

| Field | Value |
|-------|-------|
| Site / DSO | |
| Eth_A IP / FQDN | |
| MMS AP-title / AE-qualifier (for Cert B OID) | |
| Trust anchors installed (count ≥5) | |
| Cert A serial / notAfter | |
| Cert B serial / notAfter | |
| Revocation method (OCSP/CRL) | |
| Engineer sign-off | |

---

## 62351-14 Syslog sample (CRED_ENR_EST_SUCC)

Per **`ccli-62351-14-extract`** §6 — EST enrolment success (**62351-9:1.5**):

```
<109>1 2026-08-12T08:00:00.123Z cci-lab-01 ccli-pki - IEC62351-14:1 [62351-14@41912 ID="IEC62351-9:1.5" MNEMONIC="CRED_ENR_EST_SUCC" Text="Enrolment using EST successfully performed" SqNum="42"]
```

| Item | Value |
|------|-------|
| PRI | `<109>` = facility **13** + severity **5** (notice) |
| MSGID | `IEC62351-14:1` |
| Transport | **TCP/6514** · RFC 5425 TLS · cipher profile from **62351-3** |
| PROCID | `-` (ignore on receive) |
| Catalogue | **`ccli-62351-14-extract`** Annex B.1.6 Tables **33–36** (groups 1–4) |

### K6.3 event quick reference (62351-14 Annex B.1.6)

| Phase | MNEMONIC | ID | Severity | PRI |
|-------|----------|-----|----------|-----|
| EST success | `CRED_ENR_EST_SUCC` | IEC62351-9:1.5 | notice | `<109>` |
| EST failure | `CRED_ENR_EST_FAIL` | IEC62351-9:1.6 | error | `<107>` |
| Cert revoked | `CERT_V_REVOKED` | IEC62351-9:4.1 | alarm | `<105>` |
| Path fail | `CERT_V_PK_PATH` | IEC62351-9:2.11 | error | `<107>` |
| OCSP unreachable | `CERT_V_R_NR_OCSP` | IEC62351-9:4.5 | warning | `<108>` |
| TLS EKU missing | `CERT_PKCE_EKU_TLS_SA` | IEC62351-9:2.23 | error | `<107>` |

Full catalogue: **58 C1 events** (groups 1–4) + **8 GDOI** (group 5, P2) in `ccli-62351-14-extract`.

---

## OpenWrt / TG-524 configuration sketch

*Implementation-specific — adjust paths after TesPro SDK review.*

```uci
# Illustrative — not committed product config
ccli.pki.est_url='https://ra.example.dso/.well-known/est/simpleenroll'
ccli.pki.trust_anchor_count='5'
ccli.pki.renewal_pct='70'
ccli.pki.tls_cert='/etc/ssl/ccli/tls-cert.pem'
ccli.pki.e2e_cert='/etc/ssl/ccli/e2e-cert.pem'
ccli.pki.revocation='ocsp'   # or 'crl' — one per cert policy
```

---

## RAG cross-links

| source_id | Document |
|-----------|----------|
| `ccli-k63-ceremony-sop` | **This document** |
| `ccli-62351-9-extract` | PKI normative + Annex C/D |
| `ccli-62351-3-extract` | TLS :3782 + cert validation |
| `ccli-62351-4-extract` | Annex G E2E cert + OSI binding |
| `ccli-62443-4-2-extract` | CR 1.8 PKI |
| `ccli-62351-14-extract` | Syslog wire + **Annex B.1.6** PKI catalogue (Tables 33–37) |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-08-12 | Initial SOP derived from 62351-9 Annex C + §7.3 + 62351-4 Annex G |
| **1.1** | 2026-08-12 | Added **62351-14** RFC 5424 wire example for `CRED_ENR_EST_SUCC` |
| **1.2** | 2026-08-12 | K6.3 event quick reference from **62351-14** Annex B.1.6 |

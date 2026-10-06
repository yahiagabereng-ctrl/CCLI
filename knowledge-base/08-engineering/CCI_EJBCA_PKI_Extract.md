# EJBCA PKI — CCLI profile mapping extract

**Document ID:** CCLI-PKI-EJBCA-002  
**Revision:** 1.0  
**Date:** 2026-10-06  
**RAG source_id:** `ccli-ejbca-pki-extract`  
**Normative:** 62351-3 · 62351-4 Annex G · 62351-9 · CEI Annex T T.3.3.4.9  
**Ceremony SOP:** `CCI_K6.3_Key_Ceremony_SOP.md` (`ccli-k63-ceremony-sop`)

---

## Requirements

| REQ-ID | Source | Requirement | EJBCA implementation |
|--------|--------|-------------|----------------------|
| REQ-EJBCA-001 | T.3.3.4.9 | EST or SCEP enrolment | EST alias + `simpleenroll` |
| REQ-EJBCA-002 | G.2 | Separate TLS vs E2E certs | Two certificate profiles + two end entities / enrolments |
| REQ-EJBCA-003 | G.6.2 | E2E subject = AP+AE OID | End entity DN or custom extension on Cert B profile |
| REQ-EJBCA-004 | 62351-9 §7.4.4.10 | TLS EKU serverAuth + clientAuth | Cert A profile EKU |
| REQ-EJBCA-005 | G.5.2 | E2E keyUsage signature + keyAgreement | Cert B profile keyUsage |
| REQ-EJBCA-006 | K6.3 | Keys generated on device (TPM) | CSR from TG544; **disable** EST `serverkeygen` for production |
| REQ-EJBCA-007 | Lab TSP | Client PEM + TSP OpenSSL key | Export operator cert; convert key to PKCS#1 if needed |

---

## Architecture

```text
EJBCA CE (lab CA)
  ├── Certificate Profile: CCLI-Cert-A-TLS
  ├── Certificate Profile: CCLI-Cert-B-E2E-MMS
  ├── End Entity: TG544-<serial>  → 2× EST enroll → DUT /etc/ccli/tls/
  └── End Entity: TSP-DSO-OPERATOR → PEM → C:\CCLI_product_tls\ → Test Suite Pro
```

### Functional roles (unchanged from K6.3)

| Role | EJBCA object |
|------|--------------|
| CA | Management CA / dedicated `CCLI-Lab-CA` |
| RA | EJBCA admin + EST RA mode (pilot) or Client mode (pre-register) |
| Device | End entity + EST client (future firmware) |
| TSP PC | End entity `TSP-DSO-OPERATOR` — manual PEM export today |

---

## Certificate profile mapping

### Cert A — TLS (62351-3) → EJBCA `CCLI-Cert-A-TLS`

| Field | Value |
|-------|-------|
| Type | X.509 v3 end entity |
| Key | RSA 2048, SHA-256 |
| **keyUsage** | digitalSignature, keyEncipherment (server) |
| **extendedKeyUsage** | serverAuth, clientAuth |
| **subject DN** | `CN=CCI016_01`, `O=HiTEKS`, `OU=CCLI`, `serialNumber=<device-id>` |
| **subjectAltName** | `IP:192.168.10.1` (lab Eth_A) |
| **Validity** | ≤825 days (align lab) |
| **DUT files** | `server_tls.pem`, `server.key` |
| **TSP files** | Operator TLS client cert (if separate from E2E) |

### Cert B — E2E MMS (62351-4 Annex G) → EJBCA `CCLI-Cert-B-E2E-MMS`

| Field | Value |
|-------|-------|
| Key | **Separate** RSA 2048 key pair (G.2) |
| **keyUsage** | digitalSignature, keyAgreement |
| **extendedKeyUsage** | (omit TLS EKU) |
| **subject** | `2.5.4.106` = **`1.1.1.999.1.12`** (responding AP `1.1.1.999.1` + AE `12`) |
| **Encoding note** | Match lab `gen_lab_pki.py` / TSP — UTF8String OID text if OBJECT IDENTIFIER breaks OpenSSL 3 |
| **DUT files** | `server.pem`, `server_acse.key` |
| **TSP client** | Calling-side OID for DSO operator (same AP/AE in current lab CID) |

**Never** reuse Cert A private key for Cert B.

---

## Interface matrix

| Interface | Source | Destination | Protocol |
|-----------|--------|-------------|----------|
| Enrolment (target) | TG544 TPM | EJBCA | EST `simpleenroll` over TLS |
| Enrolment (lab interim) | Engineer | EJBCA Admin / OpenSSL CSR paste | Manual issue → PEM copy |
| Enrolment (lab REST) | Engineer PC | EJBCA CE | REST `pkcs10enroll` + admin mTLS P12 (`scripts/issue_ejbca_rest_pki.py`) |
| MMS C1 | TG544 Eth_A | Test Suite Pro | TLS :3782 + 62351-4 E2E |
| Trust | DSO CA cert | TG544 + TSP | PEM trust anchor |
| Revocation | EJBCA CRL | TG544 + TSP | `dso_crl.pem` |

---

## Test Suite Pro wiring (product PEMs)

| TSP field | File |
|-----------|------|
| Transport TLS CA | `dso_root_ca.pem` |
| Transport TLS client cert/key | Cert A operator PEM + `*_tsp.key` |
| MMS Security CA | same root |
| MMS Security client cert/key | Cert B operator PEM + key |
| CRL | `dso_crl.pem` |
| Port | **3782** |
| Directory to CA | **empty** |

Evidence: `lab/evidence/testsuite-pro/inbox/TSP_P3_07_ACSE_CERT_2026-09-29.md`

---

## EJBCA setup checklist (lab)

1. Start container: see `reference/vendor/ejbca/README.md`
2. Complete EJBCA setup wizard — create **Management CA** or dedicated lab CA
3. Create certificate profiles **CCLI-Cert-A-TLS** and **CCLI-Cert-B-E2E-MMS**
4. Create end entity profiles with allowed DN fields
5. Configure **EST alias** (Client mode for pre-registered TG544)
6. Issue **two** certs per device; export PEM + chain
7. Deploy to DUT yaml `lab_tr400_phase4_eth_b.yaml` paths under `/etc/ccli/tls/`
8. Stage operator certs to `C:\CCLI_product_tls\` (mirror `stage-tsp-tls.ps1` paths)
9. Verify with `openssl x509` and `lab/verify_62351_auth.py` on capture

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| EJBCA G.6.2 DN template | PART | Issued cert matches TSP | First lab issuance + openssl dump |
| EST in ccli firmware | MISSING | Annex T product | Software sprint |
| Issued product PEMs in repo | MISSING | Evidence pack | After EJBCA lab ceremony |
| IEEE 802.1AR IDevID | MISSING | Factory Phase 0 | External standard PDF |

---

## Risks

| ID | Description | Mitigation |
|----|-------------|------------|
| R-EJBCA-001 | Default EJBCA profile ≠ 62351 | Use this extract + K6.3 tables |
| R-EJBCA-002 | Single cert for TLS+E2E | Two profiles, two enrolments |
| R-EJBCA-003 | CE used in production | CE lab only; DSO buys Enterprise |
| R-EJBCA-004 | Wrong G.6.2 encoding | Compare DER to `gen_lab_pki.py` output |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-EJBCA-002 | Two serial numbers on TG544 | Distinct Cert A / B |
| REQ-EJBCA-003 | `openssl x509 -text` on Cert B | 2.5.4.106 / `1.1.1.999.1.12` |
| REQ-EJBCA-004 | EKU on Cert A | serverAuth + clientAuth present |
| REQ-EJBCA-001 | EST `cacerts` + enroll (pilot) | PEM received over HTTPS |
| TSP Connect | System Status :3782 | Assoc + `ACSE auth ACCEPT` in DUT log |

---

## Corpus references

| Doc | Path |
|-----|------|
| RFC 7030 | `reference/pki/RFC7030_EST.txt` |
| EJBCA EST | `reference/vendor/ejbca/docs/EST_Overview.md` |
| Lab PKI generator | `apps/ccli/config/tls/gen_lab_pki.py` |
| Capture plan | `CCI_EJBCA_PKI_Capture_Plan.md` |

# CCLI — EJBCA Community lab PKI setup

**Document ID:** CCLI-LAB-EJBCA-001  
**Date:** 2026-10-06  
**Software:** EJBCA CE (Docker) — **lab only, not production**  
**Mapping:** `knowledge-base/08-engineering/CCI_EJBCA_PKI_Extract.md`

---

## 1. Start EJBCA

```powershell
cd c:\Yahia\projects\CCLI
powershell -File scripts\start-ejbca-lab.ps1
```

| URL | Purpose |
|-----|---------|
| https://localhost:8443/ejbca/adminweb/ | Admin (setup + profiles) |
| https://localhost:8443/ejbca/ | Public / enrolment pages |

First boot: accept browser TLS warning (self-signed Management CA).

**Lab login:** this container runs with `TLS_SETUP_ENABLED=simple` — open Admin Web in the browser **without** importing a SuperAdmin P12. (Production uses client-certificate admin.)

Stop / logs:

```powershell
powershell -File scripts\start-ejbca-lab.ps1 -Stop
powershell -File scripts\start-ejbca-lab.ps1 -Logs
```

Health check:

```powershell
curl.exe -k https://localhost:8443/ejbca/publicweb/healthcheck/ejbcahealth
```

---

## 2. First login (Management CA already created)

On first Docker start, EJBCA creates an internal **Management CA** automatically.

1. Open **https://localhost:8443/ejbca/adminweb/** in your browser.
2. Accept the self-signed TLS warning.
3. You should land in the Admin UI (no password in lab simple mode).

### Create CCLI issuing CA (recommended next step)

1. **CA Functions → Certificate Authorities → Create CA**
2. Suggested name: **`CCLI-Lab-CA`**
3. Crypto token: **soft** PKCS#12 (lab only)
4. Key: **RSA 2048**, signature **SHA256WithRSA**
5. DN: `CN=CCLI Lab CA,O=HiTEKS,OU=CCLI-Lab,C=IT`
6. Export CA certificate (PEM) → `root_CA.pem` / `dso_root_ca.pem` for TG544 + TSP

---

## 3. Certificate profile — **CCLI-Cert-A-TLS** (62351-3)

**Certificate Profiles → Add**

| Setting | Value |
|---------|-------|
| Profile name | `CCLI-Cert-A-TLS` |
| Available bit lengths | 2048 |
| Signature algorithm | SHA256WithRSA |
| **Key usage** | Digital Signature, Key Encipherment |
| **Extended key usage** | TLS Web Server Authentication, TLS Web Client Authentication |
| Subject DN | Use End Entity Profile (below) |
| Validity | 825 days (match lab) |

Used for: `server_tls.pem`, operator TLS client, `client_tls.pem` on DUT allow-list.

---

## 4. Certificate profile — **CCLI-Cert-B-E2E-MMS** (62351-4 Annex G)

**Certificate Profiles → Add**

| Setting | Value |
|---------|-------|
| Profile name | `CCLI-Cert-B-E2E-MMS` |
| Available bit lengths | 2048 |
| **Key usage** | Digital Signature, Key Agreement |
| **Extended key usage** | *(leave empty — not TLS profile)* |
| Subject DN | **`2.5.4.106`** = `1.1.1.999.1.12` (see note below) |
| Validity | 825 days |

**G.6.2 subject note:** Lab `gen_lab_pki.py` encodes OID as UTF8String for TSP/OpenSSL. After first issuance, verify:

```powershell
openssl x509 -in issued-server-b.pem -noout -subject -text
```

Compare to `apps/ccli/config/tls/server.pem`. Adjust End Entity DN template if TSP rejects.

Used for: `server.pem`, `server_acse.key`, TSP MMS Security `client.pem`.

---

## 5. End entity profiles

### Server — `CCLI-Server-Profile`

| DN field | Modifiable | Example |
|----------|------------|---------|
| CN | Required | `CCI016_01` |
| O | Fixed | `HiTEKS` |
| OU | Fixed | `CCLI-Lab` |
| serialNumber | Required | `TG544-LAB-001` |

Default certificate profile: **CCLI-Cert-A-TLS** (override to Cert B for second issuance).

### Operator — `CCLI-Operator-Profile`

| DN field | Example |
|----------|---------|
| CN | `DSO_OPERATOR` |
| O | `HiTEKS` |

Profiles: issue **Cert A** + **Cert B** for TSP (two end entities or two cert requests).

---

## 6. Create end entities (manual lab path)

Until EST is in ccli firmware, use **Admin Web → Register Entity**:

| Username | Profile | Cert profile | Role |
|----------|---------|--------------|------|
| `tg544-lab-001` | CCLI-Server-Profile | CCLI-Cert-A-TLS | TG544 TLS |
| `tg544-lab-001-e2e` | CCLI-Server-Profile | CCLI-Cert-B-E2E-MMS | TG544 MMS |
| `tsp-dso-operator` | CCLI-Operator-Profile | CCLI-Cert-A-TLS | TSP TLS client |
| `tsp-dso-operator-e2e` | CCLI-Operator-Profile | CCLI-Cert-B-E2E-MMS | TSP MMS client |

For each entity: **Create certificate** (or PKCS#12 download) → export **PEM**.

**G.2 rule:** Generate **separate keys** for Cert A and Cert B on the server (two CSRs / two enrollments).

---

## 7. Export PEMs → CCLI layout

Copy issued files to match deploy script expectations:

| EJBCA export | CCLI path (DUT) | CCLI path (TSP PC) |
|--------------|-----------------|---------------------|
| CA cert | `/etc/ccli/tls/root_CA.pem` | `C:\CCLI_product_tls\dso_root_ca.pem` |
| Server Cert A + key | `server_tls.pem`, `server.key` | — |
| Server Cert B + key | `server.pem`, `server_acse.key` | — |
| Operator Cert A + key | `client_tls.pem` (allow-list) | TSP Transport TLS |
| Operator Cert B + key | `client.pem` (allow-list) | TSP MMS Security |
| CRL | `lab_crl.pem` | same |

Stage TSP folder (adapt `stage-tsp-tls.ps1` or copy manually):

```powershell
New-Item -ItemType Directory -Force C:\CCLI_product_tls
# copy PEMs + openssl rsa for TSP key if needed
```

Deploy DUT: existing `lab/tg544-openwrt/deploy-ccli-session-fix.ps1` with yaml `lab_tr400_phase4_eth_b.yaml`.

---

## 8. EST (optional next step)

**System Configuration → EST Configuration → Add alias**

| Setting | Lab value |
|---------|-----------|
| Alias | `ccli-lab` |
| Mode | Client (pre-registered entities) |
| CA | CCLI-Lab-CA |
| URL | `https://localhost:8443/.well-known/est/ccli-lab/simpleenroll` |

Full EST testing waits on **EST client in ccli** (deferred). Manual PEM export is enough for first TSP Connect.

---

## 9. Verification checklist

| Step | Command / action | Pass |
|------|------------------|------|
| EJBCA up | `scripts/start-ejbca-lab.ps1 -Status` | HTTPS 8443 |
| Cert A EKU | `openssl x509 -in server_tls.pem -text` | serverAuth + clientAuth |
| Cert B KU | `openssl x509 -in server.pem -text` | digitalSignature + keyAgreement |
| G.6.2 subject | `openssl x509 -in server.pem -subject` | `1.1.1.999.1.12` |
| DUT deploy | deploy script + `:3782` listening | `ss -tlnp \| grep 3782` |
| TSP Connect | System Status, port 3782 | `ACSE auth ACCEPT` in DUT log |

---

## 10. Annex / 62351 — precise release matrix (MUST before TSP Connect)

Creating **`CCLI-Lab-CA`** (§2) is only the **trust anchor**. Annex compliance is proven on **each issued end-entity certificate**, not on the CA name alone.

### 10.1 What §2 (issuing CA) satisfies

| Clause | Requirement | EJBCA `CCLI-Lab-CA` setting | Lab | Product |
|--------|-------------|----------------------------|-----|---------|
| **T.3.3.4.1** | RSA ≥ 2048 | RSA 2048 | ✓ | ✓ |
| **T.3.3.4.1** | Signature SHA-256 | SHA256WithRSA | ✓ | ✓ |
| **62351-4 G.3** | X.509 v3 CA | X.509 CA in EJBCA | ✓ | ✓ |
| **T.3.3.4.9** | DSO trust anchor | Export PEM → `root_CA.pem` | ✓ (1 of 3) | DSO CA required |
| **T.3.3.4.9** | Admin + Manufacturer CAs | — | **GAP** (lab simulates DSO only) | **MUST** add 2 more anchors on device |
| **62351-9 §7.3.6** | ≥ 5 trust anchors configurable | ccli config slots | PART | Product config |

**Do not** issue MMS/TLS certs from **Management CA** — only from **`CCLI-Lab-CA`**.

### 10.2 Per-device release — two certificates (MANDATORY)

| Clause | Requirement | EJBCA artefact | Exact lab value | Verify |
|--------|-------------|----------------|-----------------|--------|
| **62351-4 G.2** | Separate TLS vs E2E cert + key | Cert A + Cert B profiles; **two keys** on TG544 | `server_tls` ≠ `server.pem` keys | Two serial numbers |
| **T.3.3.4 / 62351-3** | MMS TLS port **3782** | Cert A used on ccli `:3782` | yaml `tcp_port: 3782` | `ss -tlnp \| grep 3782` |
| **62351-9 §7.4.4.10** | TLS EKU **serverAuth + clientAuth** | **CCLI-Cert-A-TLS** | both EKU bits set | `openssl x509 -ext extendedKeyUsage` |
| **62351-3 / 62351-9** | TLS keyUsage **digitalSignature** (+ encipherment RSA) | Cert A profile | Digital Signature, Key Encipherment | `openssl x509 -ext keyUsage` |
| **62351-4 G.5.2** | E2E keyUsage **digitalSignature + keyAgreement** | **CCLI-Cert-B-E2E-MMS** | both bits, **no TLS EKU** | `openssl x509 -ext keyUsage` |
| **62351-4 G.6.2** | E2E subject **objectIdentifier** = AP+AE | Cert B DN **2.5.4.106** | **`1.1.1.999.1.12`** (responding) | match `lab_tg544_eth_a.cid` / `mms_aare_auth.hpp` |
| **62351-4 §10.5.3** | AP-title `1.1.1.999.1`, AE **12** | OID final arc = AE | `.12` suffix | AARE wire vs subject |
| **62351-4 G.3/G.4** | Serial non-sequential; 128-bit UUID style | EJBCA random serial | auto | not sequential |
| **T.3.3.4.1 / G.4** | RSA 2048, SHA-256 | both profiles | 2048 / SHA256WithRSA | `openssl x509 -text` |
| **T.3.3.4.2** | Max cert **8192 octets** | keep extensions minimal | DER < 8192 | `openssl x509 -outform DER \| wc -c` |
| **62351-4 G.2** | Cert A subject CN + SAN | End entity profile | CN=`CCI016_01`, SAN IP=`192.168.10.1` | SAN present on TLS cert |

**TSP operator (client) certs** — same split:

| Role | Cert A (TLS client) | Cert B (MMS client) |
|------|---------------------|---------------------|
| CN | `DSO_OPERATOR` | *(G.6.2 OID subject)* |
| G.6.2 OID | N/A | **`1.1.1.999.1.12`** (calling, lab CID) |
| RBAC | maps to **DSO_OPERATOR** (−1) on DUT | same cert in MMS Security |

Golden reference: `apps/ccli/config/tls/` from `gen_lab_pki.py` — **compare every extension** after first EJBCA issuance.

### 10.3 Revocation & enrolment (product; partial lab)

| Clause | Requirement | EJBCA / ccli | Lab today |
|--------|-------------|--------------|-----------|
| **T.3.3.4.9** | SCEP **or** EST | EST alias §8 | Manual PEM (**EST client deferred**) |
| **T.3.3.4.9** | CRL **and** OCSP supported | Enable CRL on `CCLI-Lab-CA`; OCSP optional | CRL → `lab_crl.pem`; OCSP **GAP** |
| **T.3.3.4.2** | CRL update **≤ 24 h** | CA → CRL Publish → periodic | configure in EJBCA |
| **62351-9 §7.5** | OCSP nonce mandatory | OCSP responder config | product |
| **T.3.3.4.5** | UTC ±100 ms before validity checks | chrony/GNSS on TG544 | bench check |

### 10.4 Release gate — do not deploy until ALL pass

```powershell
# After EJBCA export to staging folder:
openssl x509 -in server_tls.pem -noout -subject -ext keyUsage,extendedKeyUsage
openssl x509 -in server.pem      -noout -subject -ext keyUsage
openssl verify -CAfile root_CA.pem server_tls.pem server.pem client.pem
```

| # | Gate | Pass criteria |
|---|------|---------------|
| 1 | Issuer | All EE certs signed by **`CCLI-Lab-CA`**, not Management CA |
| 2 | Dual cert | **Cert A ≠ Cert B** serial; **server.key ≠ server_acse.key** |
| 3 | Cert A EKU | **serverAuth** + **clientAuth** |
| 4 | Cert B KU | **digitalSignature** + **keyAgreement** only |
| 5 | Cert B subject | **2.5.4.106** = `1.1.1.999.1.12` |
| 6 | Compare lab gold | `diff` extensions vs `gen_lab_pki.py` output |
| 7 | TSP | Connect `:3782` → DUT log `ACSE auth ACCEPT role=DSO_OPERATOR` |
| 8 | Wire (optional) | `lab/verify_62351_auth.py` on pcap |

### 10.5 Known lab vs product gaps (document, do not hide)

| Annex item | Lab EJBCA | Product target |
|------------|-----------|----------------|
| 3 trust anchors | 1 CA | DSO + Admin + Manufacturer |
| EST on device | manual export | firmware EST client |
| TPM key storage | PEM on disk | TPM (62351-9 §7.3.2) |
| IDevID factory cert | none | IEEE 802.1AR Phase 0 |

---

## 11. Related docs

| Doc | Path |
|-----|------|
| Profile mapping | `knowledge-base/08-engineering/CCI_EJBCA_PKI_Extract.md` |
| Ceremony SOP | `knowledge-base/08-engineering/CCI_K6.3_Key_Ceremony_SOP.md` |
| Annex T cyber | `knowledge-base/08-engineering/CCI_Annex_T_Extract.md` §T.3.3.4 |
| 62351-4 Annex G | `knowledge-base/08-engineering/CCI_62351-4_Extract.md` |
| TSP TLS settings | `lab/evidence/testsuite-pro/inbox/TSP_P3_07_ACSE_CERT_2026-09-29.md` |
| Lab PKI golden | `apps/ccli/config/tls/gen_lab_pki.py` |

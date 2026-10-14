# D7 — IEC 62351-3 PID draft (PICS §8 + TLS PIXIT)

**Document ID:** CCLI-LAB-D7-PID-001  
**Revision:** 1.1  
**Date:** 2026-10-13  
**RAG source_id:** `ccli-lab-d7-62351-3-pid`  
**Corpus status:** **HAVE** (draft fill) — paste into lab 62351-3 PID template when provided  

**Edition claimed:** IEC 62351-3:2014+AMD1:2018+AMD2:2020 (**TLS 1.2**). TLS 1.3 **not** claimed.  

**Last ingestion / parents (2026-10-13):**

| Source | RAG / path | Role for this PID |
|--------|------------|-------------------|
| 62351-3 requirements | `ccli-62351-3-extract` §8 Tables 1–5 | Normative PICS columns |
| 62351-100-3 tests | `ccli-62351-100-3-extract` rev **1.1** | Abstract cases this PID parameterizes |
| 62351-100-4 planning | `ccli-62351-100-4-extract` | **Separate** MMS PID — PDF **MISSING** |
| Allegato O | `cei-0-16-allegato-o` §12 | Mandates accredited **62351-100-3** |
| Allegato T | `cei-0-16-allegato-t-extract` T.3.3.4 | TLS profile (:3782, mutual auth, CRL, crypto) |
| 61850 PIXIT (app) | `PIXIT_DRAFT.md` | Port / cert overlap; not a substitute for D7 |
| Implementation | `mms_adapter.cpp` · lab yaml `tcp_port: 3782` | DUT behaviour |

**DUT evidence:** TSP Connect **PASS** on **:3782** (2026-10-08/09) · firmware class **r52** · PEMs EJBCA / `C:\CCLI_product_tls\`. Formal **62351-100-3** Tables 4–5 = gate **P7-07 OPEN**.

Notation: **m** mandatory · **o** optional · **x** excluded · **c** CCI claim · **PART** claimed with lab PIXIT waiver.

---

## Requirements

| REQ-ID | Source | Requirement | D7 claim |
|--------|--------|-------------|----------|
| **REQ-D7-ED-001** | 62351-3 §5.2 | TLS **1.2** minimum | **m** — `TLSConfiguration_setMinTlsVersion(TLS_VERSION_TLS_1_2)` |
| **REQ-D7-PORT-001** | 62351-3 §5.7 · Allegato **T** · 62351-4 §6.3 | Distinct secure TCP port | **3782** secure · **102** cleartext lab-only |
| **REQ-D7-MUT-001** | 62351-3 §5.6 · Allegato **T** | Mutual certificate authentication | **m** — no client cert → fail |
| **REQ-D7-CRYPTO-001** | 62351-3 §5.6.4 · Allegato **T.3.3.4.1** | RSA ≥2048 · SHA-256 · cert ≤8192 octets | **m** |
| **REQ-D7-CRL-001** | 62351-3 §5.6.4.4 · Allegato **T** | CRL check; unreachable CRL shall not drop session | **m** when `tls_crl` loaded |
| **REQ-D7-OCSP-001** | 62351-3 · Allegato **T** | OCSP support | **x** first pass (not claimed) |
| **REQ-D7-REN-001** | 62351-3 §5.3–5.4 | Resumption / renegotiation for long-lived links | **PART** — PIXIT intervals for lab |
| **REQ-D7-1003-001** | Allegato **O §12** | Accredited **62351-100-3** report | PID input only · cert = **P7-07** |
| **REQ-D7-SCOPE-001** | Programme | This PID = **transport (62351-3)** only | **62351-4 / 100-4** = separate PID |

---

## Architecture

### Functional
- Eth_A DSO path: **IEC 61850-8-1 MMS** as application context for **62351-100-3** (§4.3.1).  
- Transport security: **62351-3** TLS. Application / ACSE security: **62351-4** (dual cert G.2) — **out of this PID**.

### Network / security
```text
 TSP / DSO client ──TLS mutual──► CCI TG544 :3782
   Cert A (TLS EE)                  Cert A (TLS EE)
   CA = lab EJBCA root              CA = same root
```

### Power / hardware
- Platform: **TesPro TG544** (`platform_tg500`) · OpenWrt frozen — see `CCI_SOC_Freeze.md`. Pi out of scope.

---

## Interface matrix

| Interface | Source | Destination | Protocol |
|-----------|--------|-------------|----------|
| Eth_A | Lab / DSO client | CCI MMS server | TCP **TLS 1.2** **:3782** |
| Eth_A (bench only) | TSP cleartext profile | CCI | TCP **:102** — **not** product DSO claim |
| PKI | EJBCA lab CA | Both peers | X.509 · optional CRL file |
| App (context only) | Same TLS session | MMS ACSE | 61850-8-1 (62351-4 PID separate) |

---

## BOM matrix

| Block | Function | Candidate / PN | Status |
|-------|----------|----------------|--------|
| Gateway | MMS + TLS | TesPro TG544 / TR500 | Lab DUT |
| TLS stack | wolfSSL / libIEC61850 TLS | OpenWrt package | HAVE |
| Cert A | TLS end-entity | EJBCA · `server_tls.pem` / client_tls | HAVE |
| Cert B | ACSE / MMS (not D7) | EJBCA · `server.pem` / client.pem | HAVE (62351-4 PID) |
| CRL | Revocation | `lab_crl.pem` via `tls_crl:` | PART (on/off per run) |

---

## PICS §8 — Table 1 Cipher suites (server)

| Suite | 62351-3 | CCI fill |
|-------|---------|----------|
| NULL / NULL_MD5 | **x** | **x** |
| NULL_SHA / NULL_SHA256 | **o** | **x** (not offered; Allegato T null needs explicit enable) |
| AES-GCM / CBC SHA-256 (ECDHE / DHE / RSA) | per 62351-3 + Allegato **T** list | **c** — product intent: ECDHE + AES-GCM + SHA-256 |

**Submit action:** Attach DUT `openssl ciphers` / wolfSSL enabled list from **frozen** submit firmware (exact IANA names). Allegato T reference suites (extract):  
`TLS_RSA_WITH_AES_128_CBC_SHA256` · `TLS_DHE_RSA_WITH_AES_128_GCM_SHA256` · `TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256` — lab PEMs are **RSA**; ECDSA suite **o** until EC PEMs claimed.

---

## PICS §8 — Table 2 TLS versions (server)

| Version | 62351-3 | CCI fill |
|---------|---------|----------|
| 1.0 / 1.1 | o | **x** |
| **1.2** | **m** | **m** |
| 1.3 | o | **x** / not claimed |

---

## PICS §8 — Table 3 Protocol features (server)

| Feature | 62351-3 server | CCI fill | Notes |
|---------|----------------|----------|--------|
| Resumption ≥24 h policy | m | **PART** | Allegato T: resume shorter than reneg; example ≤2 h — set PIXIT for lab |
| HelloRequest resumption | m | **PART** | Stack-dependent |
| Session tickets RFC 5077 | o | **o** | |
| Renegotiation ≥24 h policy | m | **PART** | Align to CRL period (Allegato T / 62351-3 informative) |
| RFC 5746 | m | **m** (stack) | |
| Trusted CA RFC 6066 | o | **o** | Lab: single EJBCA root |

---

## PICS §8 — Table 4 Certificates

| Feature | 62351-3 | CCI fill |
|---------|---------|----------|
| Multiple CA roots | o | **o** (lab: one EJBCA root) |
| Max cert 8192 bytes | m | **m** |
| RFC 5280 validation | m | **m** (`chain_validation: true`) |
| CRL | m | **m** when `tls_crl` loaded · **PART** if DUT `crl=off` for TSP |
| OCSP | o | **x** first pass |
| Cert white-list | o | **c** — allow-list PEMs |

---

## PICS §8 — Table 5 Algorithms

| Algorithm | 62351-3 | CCI fill |
|-----------|---------|----------|
| RSA 2048 | m | **m** |
| RSA 1024 | o deprecated | **x** |
| ECDSA secp256r1 | o | **o** (lab PEMs RSA) |
| SHA-256 | m | **m** |
| SHA-1 / MD5 | o / x | **x** |

---

## TLS PIXIT (transport)

| Item | Value |
|------|--------|
| Secure port | **3782** |
| Cleartext port | **102** (bench only; not product DSO) |
| Mutual auth | **Required** |
| TLS EE (Cert A) | `server_tls.pem` / `client_tls.pem` (+ keys) |
| Dual cert G.2 | Declared for app stack — **ACSE Cert B** belongs in **62351-4 PID**, not this sheet |
| CA | Lab EJBCA `root_CA.pem` |
| CRL path | `/etc/ccli/tls/lab_crl.pem` when enabled |
| CRL unreachable mid-session | Do **not** drop session (62351-3 §5.6.4.4) — confirm on DUT |
| No client cert | Handshake / association **fails** (TSP T3-01) |
| Revoked cert | Fail if CRL loaded (TSP T3-04) |
| Application context | IEC 61850-8-1 MMS (for 100-3 §4.3.1) |
| Station role | **Server** (client station tests N/A unless claimed) |

**62351-4 / 100-4:** Separate PID — see `knowledge-base/08-engineering/CCI_62351-100-4_Extract.md`. Do **not** mark full native E2E **M** until implemented. Allegato **O §12** still requires **100-3** even if 100-4 is skipped.

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| Exact cipher IANA list | Intent **c** | DUT dump on submit build | Capture before quote lock |
| Resumption / reneg timers | **PART** | PIXIT numbers agreed with lab | Confirm vs Allegato T / CRL period |
| OCSP | **x** | Allegato T also cites OCSP | Defer or add later PID rev |
| Lab PID Word/Excel template | Markdown HAVE | Lab form paste | **TODO** |
| 62351-100-3 Tables 4–5 | Pre-lab TSP PASS | Accredited report | **P7-07 OPEN** |
| 62351-100-4 PDF | Planning extract | Case IDs | **MISSING** |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-D7-01 | Lab confuses D7 with 61850 PICS (D1) | Wrong test day | Label **62351-3 transport only** |
| R-D7-02 | Claim 100-3 certified from TSP Connect alone | False O §12 claim | P7-07 accredited run |
| R-D7-03 | Mark OCSP / TLS 1.3 **m** accidentally | Lab fail | Keep **x** / not claimed |
| R-D7-04 | Put ACSE Cert B rows in D7 | Scope creep | Point to 62351-4 PID / 100-4 extract |
| R-D7-05 | Cipher list drifts from OpenWrt freeze | Interop fail | Freeze + `openssl ciphers` evidence |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-D7-ED-001 / PORT / MUT | TSP **T1-01** Connect `:3782` | TLS 1.2 mutual · Initiate Success |
| REQ-D7-MUT-001 | **T3-01** no client cert | Fail |
| REQ-D7-CRL-001 | **T3-04** revoked (CRL on) | Fail establish |
| REQ-D7-CRYPTO-001 | `verify_annex_pki.py` / openssl | RSA-2048 · SHA-256 · ≤8192 |
| REQ-D7-1003-001 | Accredited 100-3 | Tables 4–5 complete · report ID |

Evidence pointers: `lab/evidence/testsuite-pro/inbox/TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08.md` · `TSP_PHASE1_SESSION_INDEX_2026-10-09.md` · cyber matrix [LAB_REQUIREMENT_TEST_MATRIX_CYBER.md](LAB_REQUIREMENT_TEST_MATRIX_CYBER.md) (`ccli-lab-requirement-test-matrix-cyber`).

---

**RAG tags:** `lab`, `62351-3`, `PID`, `D7`, `62351-100-3`, `Allegato-O`, `Allegato-T`, `P7-07`, `ccli-lab-d7-62351-3-pid`

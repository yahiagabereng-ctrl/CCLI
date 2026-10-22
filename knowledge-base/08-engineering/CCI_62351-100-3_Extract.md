# IEC TS 62351-100-3 — Transport conformance test extract

**Document ID:** CCLI-SEC-62351-100-3-001  
**Revision:** 1.1  
**Date:** 2026-10-13  
**Capture status:** **HAVE** — OCR **37** pages (2026-10-08); PDF local + gitignored  
**RAG source_id:** `ccli-62351-100-3-extract`  
**OCR RAG id:** `ccli-62351-100-3-ocr-corpus`  
**Corpus (OCR, CCLI repo):** `knowledge-base/08-engineering/reference/iec62351/_extract_62351-100-3/full.txt` (+ `p001.txt`…`p037.txt`)  
**Telematry sync copy:** `documents/projects/ccli/reference/iec62351/IEC_TS_62351-100-3_2020_full.txt` (same bytes as `full.txt`; **not** under CCLI `reference/iec62351/` root)  
**Source PDF:** `knowledge-base/08-engineering/reference/iec62351/IEC_TS_62351-100-3-2020.pdf` (local; covered by `knowledge-base/**/*.pdf` gitignore)  
**Standard identity:** **IEC TS 62351-100-3:2020** (Edition **1.0**, 2020-01) — *Conformance test cases for **IEC 62351-3** (TCP/IP TLS profile)*  

**Normative profile (separate RAG):** `ccli-62351-3-extract` — **62351-3:2014+AMD1:2018+AMD2:2020**  
**CEI grid parents:** `cei-0-16-allegato-o` · `cei-0-16-allegato-t` / `cei-0-16-allegato-t-extract`  
**Other parents:** `ccli-62351-9-extract` · `ccli-61850-10-extract`  
**Programme:** CEI **Allegato O §12** mandates accredited **62351-100-3** · Allegato **T.3.3.4** TLS profile on **:3782** · CCLI gate **P7-07** · PID fill **D7**

---

## 1. What this document is (vs 62351-3 and CEI Allegato O/T)

| Document | Role |
|----------|------|
| **CEI 0-16 Allegato O** | **Market / product obligation** — §12 *Secure transport*: conformance to **62351-3** verified **per IEC 62351-100-3**, certified by an **accredited body** |
| **CEI 0-16 Allegato T** | **DSO interface cyber profile** — **T.3.3.4** (esp. T.3.3.4.1 algorithms, T.3.3 transport **TLS / 62351-3**, port **3782**, CRL, PKI, logging) — *what* the CCI must implement |
| **IEC 62351-3** | **Security requirements** — TLS 1.2, ciphers, mutual auth, resumption/renegotiation, ports, security events |
| **IEC TS 62351-100-3** | **Abstract test cases** — how a lab verifies 62351-3 on a DUT (client **and** server); Tables **1–5** |
| **IEC 61850-10** | **ACSI/MMS server** conformance — **does not** include TLS cases; §6.3 points to **62351-100-3/4/6** |
| **RFC TLS tests** | **Out of scope** of 100-3 body — lab may add separate RFC conformance |
| **62351-8 / 62351-9** | Extra cases for RBAC certs / PKI profiles when applicable |

**Scope note (§1):** Tests **62351-3 integration** over TCP/IP, **not** raw TLS RFC conformance. **61850-10 §6.3** points here for **cyber** device tests alongside **62351-100-4/6**.

**HiTEKS CCI:** DUT is **MMS server** on Eth_A (Annex T DSO path); may also be tested as **TLS client** toward DSO if PICS claims client.

### 1.1 CEI Allegato O — why 100-3 is mandatory

From `CCI_Annex_O_Extract.md` §12 *Testing, Accreditation & Certification*:

| Test category | Requirement |
|---------------|-------------|
| IEC 61850 conformance | Certified by a lab accredited by the **UCA User Group** → **61850-10** / UCA procedure |
| **Secure transport (IEC 62351-3)** | **Conformance per IEC 62351-100-3**, certified by an **accredited body** |

So for Italian PF2 / CEI 0-16 CCI:

- **UCA / 61850-10** ≠ substitute for **62351-100-3**.  
- Allegato O **names 100-3 explicitly** as the transport conformance path.  
- Programme gate **P7-07** = plan + book that accredited run; **D7** = 62351-3 PID (PICS+PIXIT) input to 100-3.

### 1.2 CEI Allegato T — profile that 100-3 exercises

Allegato **T.3.3.4** (cybersecurity) / transport profile maps to **62351-3** behaviour that **100-3 Tables 1–3** verify. HiTEKS claims (lab + D7):

| Allegato T topic | Clause (extract) | Product claim | 100-3 relevance |
|------------------|------------------|---------------|-----------------|
| Secure MMS port | T.3.3 / T-profile TLS | TCP **3782** | PIXIT port · Table 1 config |
| Min TLS version | T.3.3.4 transport | **TLS 1.2** | Table 2 normal / Table 3 resiliency |
| Cipher suites | T.3.3.4 | AES-128 CBC/GCM + ECDHE-ECDSA GCM (per extract) | Table 1 + 2 |
| Mutual authentication | T.3.3.4 / 62351-3 | Client + server certs | Table 2 · negative no-cert |
| Cert / key lengths | T.3.3.4.1 | RSA ≥ **2048**, SHA-256, cert ≤ **8192** octets | Table 1 / 3 |
| CRL update / revocation | T.3.3.4 | CRL (OCSP also required by T) | Table 3 resiliency |
| Renegotiation / session resume | T.3.3.4 | Align to CRL period; resume ≤ 2 h (T extract) | Table 2 soak / PIXIT |
| Security event logging | T.3.3.4.6 | TLS + cert validation events | §4.3.4 logging |
| Application under TLS | T.3.3 MMS | **61850-8-1 MMS** (Annex T DSO) | §4.3.1 application context |
| E2E / dual cert / ACSE | T + 62351-4 G.2 | Dual Cert A/B · A-profile | **Not 100-3** → planning extract `ccli-62351-100-4-extract` (PDF **MISSING**) |

**Allegato O functional clauses** (O.8 measures, O.9 controls, O.14 logger, etc.) are **not** 100-3 cases — they drive **61850-10** / TSP functional scope. O only **forces** the **transport cert** via §12.

---

## 2. Documents to submit (PID)

**Protocol Implementation Document (PID) = PICS + PIXIT** (§3.1, §4.3.2):

| Item | Content | CCLI status (2026-10-13) |
|------|---------|--------------------------|
| **PICS** | Capabilities per **62351-3** (incl. §8 tables in base standard) | Draft **HAVE** — `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **PIXIT** | Ports, cipher order, cert paths, renegotiation interval, OCSP/CRL URLs | **PART** — D7 + `PIXIT_DRAFT.md` (`:3782`); lab Word template may still be separate |
| **DUT ready** | Configured per PID; representative data points; human-readable verification | TG544 + CID · TLS Connect **PASS** 2026-10-08/09 |
| **Manuals** | Install/operate or on-site support | Deploy docs + lab yaml |

**Application context (§4.3.1):** All cases run **in context of a real application protocol** that **requires 62351-3** — for CCI: **IEC 61850-8-1 MMS** over TLS (**Allegato T** DSO Eth_A; Allegato **O** secure-transport certification path).

---

## 3. Test architecture (§4.2)

```text
  [Test simulator / client+server] <--TLS/TCP--> [DUT]
         |                                           |
    PID-tailored plan                          PICS/PIXIT values
    Clause 5 then 6, order preserved           Security event log
```

| Clause | Purpose |
|--------|---------|
| **5** | **Configuration parameters** (62351-3 AMD1 **Clause 7** + referencing standards) — **Table 1** |
| **6** | **62351-3 requirements** — **Table 2** normal procedures · **Table 3** resiliency (fault/negative) |
| **7** | **Test results charts** — **Table 4** (config) · **Table 5** (requirements); Pass / Fail / NA / Empty |

**Station type:** Clause 6 cases addressed **per client and per server** unless a case says otherwise (§4.2.2).

**Mandatory flags (§4.2.1):**

- **M** — mandatory in 62351-3  
- **PICS** / **PIXIT** — mandatory if capability enabled in PID  

**Test ID syntax:** `{subclause}.{case}` (table subclause + case number).

---

## 4. Test facility rules (§4.3.3–4.3.4)

1. Verify DUT **HW/SW version** vs PID.  
2. Publish **tailored test plan** from PID before execution.  
3. Clauses **5 and 6**: no errors; **follow step order** in Clause 6.  
4. Mark **Table 4/5** for every case (no empty cells when complete).  
5. Issue **conformance test report** to initiator.  
6. Tests **reproducible** by lab engineers (automated or manual + logs).

**Logging (DUT or equivalent visibility):**

- Handshake, **renegotiation**, **session resumption**  
- Certificate checks: valid, expired, revoked, bad key length, bad signature  
- Cipher change failures  
- **62351-3 security events** on resiliency failures  

If DUT cannot log, it must expose another verifiable means.

**Out of scope for pass/fail here:** application logic (only PID-described protocol behaviour).

---

## 5. Requirement themes → expected test areas

Map to **62351-3** (see `ccli-62351-3-extract`). Table rows in PDF are **OCR-garbled** — use licensed PDF + this map for lab booking.

### 5.1 Clause 5 — Configuration parameters (Table 1)

| Theme | 62351-3 ref | CCI lab hook |
|-------|-------------|--------------|
| TLS version support | §5.2 | TLS 1.2 only policy on product |
| Cipher suites | §5.1, §5.8 | AES-GCM / SHA-256; no NULL/weak |
| Mutual authentication | §5.6.3 | Cert A at TLS; trust store |
| Session resumption interval | §5.3 | Long-lived DSO link |
| Renegotiation interval | §5.4, RFC 5746 | 24 h class behaviour |
| CRL/OCSP policy | §5.6.4 | EJBCA CRL; no drop on CRL unreachable mid-session |
| Secure vs non-secure **TCP port** | §5.7 | **3782** secure · **102** plain (lab) |
| Security events | §4.4 | 62351-14 / 62351-7 mapping |

### 5.2 Clause 6 — Normal procedures (Table 2)

Typical **positive** flows (both station types where applicable):

- Successful TLS handshake with **valid peer cert**  
- Application data over established TLS (MMS associate after TLS)  
- **Session resumption** within policy  
- **Renegotiation** within policy  
- Cipher suite negotiation matching PICS  

### 5.3 Clause 6 — Resiliency (Table 3)

Typical **negative** flows:

- **Expired / revoked / wrong CA / wrong key length** peer cert → refuse + **alarm**  
- **Unsupported cipher** → failure + event  
- **TLS version downgrade** attempt → alarm  
- **MITM** or altered messages → integrity failure  
- Renegotiation/resumption **policy violations**  

**HiTEKS lab:** Align with Allegato **T** + OpenSSL gates; TSP Connect on **:3782** **PASS** (2026-10-08/09). Formal Table 3 negatives (expired/revoked) still for accredited / P7-07.

### 5.4 Clause 7 — Results charts (Tables 4–5)

Matrix: each **configuration value** × each **test case** → P / F / NA.  
Accredited cert (e.g. IMQ sample) cites **62351-3:2020** with report ID; procedure aligns with **100-3** tables.

---

## 6. Requirements (HiTEKS trace)

| REQ-ID | Source | Requirement | Verification |
|--------|--------|-------------|--------------|
| **REQ-1003-DOC-001** | §4.3.2 | Submit **PID** (PICS+PIXIT) with DUT | Document review pre-test |
| **REQ-1003-APP-001** | §4.3.1 · Allegato **T** | Tests under **61850 MMS** (DSO Eth_A) | Lab script / TSP over `:3782` |
| **REQ-1003-ROLE-001** | §4.2.2 | **Server** station tests mandatory; **client** if claimed | TG544 server P0 |
| **REQ-1003-TLS-001** | Table 2 / §6 · Allegato **T.3.3.4** | Normal TLS + app procedures pass | Automated + pcap |
| **REQ-1003-TLS-002** | Table 3 / §6 · Allegato **T** CRL/cert | Resiliency cases pass | Negative cert injection |
| **REQ-1003-LOG-001** | §4.3.4 · Allegato **T.3.3.4.6** | Handshake/reneg/resumption + cert results logged | DUT syslog / tester |
| **REQ-1003-RPT-001** | §4.3.3 · Allegato **O §12** | Completed Tables 4–5 + accredited test report | Accredited body output |
| **REQ-1003-PORT-001** | Clause 5 / 62351-3 · Allegato **T** | Distinct secure port documented in PIXIT | **3782** in PIXIT / D7 |
| **REQ-1003-CEI-001** | Allegato **O §12** | Product claims **62351-100-3** cert by accredited body | P7-07 plan + lab quote |

---

## 7. Interface matrix (lab)

| Interface | Source | Destination | Protocol |
|-----------|--------|-------------|----------|
| Eth_A DSO | Test simulator (client) | CCI TG544 (server) | TCP **TLS 62351-3** → MMS **62351-4** |
| PKI | EJBCA lab CA | Both peers | X.509 / CRL (62351-9) |
| Evidence | DUT logs | Lab report | §4.3.4 events |

---

## 8. Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| **PICS/PIXIT (62351-3)** | D7 markdown draft **HAVE** | Lab-accepted PID | Paste / agree template with lab |
| **Table 2/3 case list** | OCR unreadable | Licensed PDF row IDs | Verify at lab |
| **Accredited lab (O §12)** | Questionnaire drafted | 62351-100-3 quote + report | **P7-07 OPEN** |
| **DUT TLS on :3782** | TSP Connect **PASS** (2026-10-08/09) | Full Tables 4–5 at lab | Pre-lab negatives (CRL, expired) PART |
| **Allegato T vs 100-3** | Profile mapped in §1.2 | Formal PIXIT alignment (reneg/CRL/OCSP) | Confirm with TUV/lab |
| **62351-100-4** (MMS) | Dual-cert Connect evidenced · planning extract **HAVE** | Formal cases if Allegato T E2E claimed | PDF / OCR **MISSING** — see `CCI_62351-100-4_Extract.md` |

---

## 9. Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-1003-01 | Confuse **62351-3 cert** with **100-3 test plan** only | Wrong lab scope | Cite both; Allegato **O §12** names **100-3** |
| R-1003-02 | Test MMS without TLS | Invalid 100-3 + Allegato T fail | Always bind to **:3782** |
| R-1003-03 | OCR table IDs wrong | Wrong internal checklist | Re-type Tables 1–3 from licensed PDF |
| R-1003-04 | Treat Allegato **O/T** as the test procedure | Missing Tables 1–5 | O/T = requirement; **100-3** = tests |
| R-1003-05 | Bundle 100-3 into 61850-10 quote only | No transport cert for O §12 | Separate cyber quote / same visit Q17 |

---

## 10. Verification (minimum internal pre-lab)

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-1003-PORT-001 | OpenSSL s_client `:3782` | TLS 1.2 + client auth |
| REQ-1003-TLS-001 | TSP Connect / MMS browse | App data after TLS |
| REQ-1003-TLS-002 | Expired cert peer | Handshake fail + log |
| REQ-1003-APP-001 | PICS states 61850-8-1 | Lab application declared |

---

## 11. DNV / accredited lab — questions

Full questionnaire: **`lab/meetings/2026-10-08_Lab_Conformance_Questionnaire.md`** (`ccli-lab-conformance-questionnaire`), sections 6–7. Transport-specific items:

### 11.1 Scheme and edition

1. Do you execute **IEC TS 62351-100-3:2020 Tables 1–5 verbatim** or a national scheme (e.g. IMQ IND-F-006_CYB.62351)?  
2. Reference edition: **62351-3:2014+AMD1+AMD2** (TLS 1.2) or **62351-3:2023 Ed. 2** (TLS 1.3)?  
3. Is the application context required to be **61850 MMS** (8-1) or any TCP application?  
4. Are **client-station** tests waived for a **server-only** DUT?  
5. How are **RFC TLS** base-protocol tests combined with 100-3 — same report or add-on?

### 11.2 Configuration parameters (Clause 5 / Table 1)

6. Mandatory and forbidden **cipher suites**; is **TLS 1.3** required or optional?  
7. **Renegotiation / resumption** intervals — simulated 24 h via shortened PIXIT values?  
8. **Revocation**: CRL, OCSP, or both; OCSP nonce required? How is "CRL unreachable must not drop session" tested?  
9. Certificates: lab PKI or HiTEKS EJBCA CA? Key sizes / curves; max certificate size (≤ 8192 octets)?  
10. Secure vs non-secure **port** declaration (3782 / 102) — tested together?

### 11.3 Logging and evidence (§4.3.4)

11. Which **security events** must be visible — syslog RFC 5424 enough, or 62351-14 / 62351-7 objects?  
12. Is **remote read-only access** to DUT logs allowed during tests?

### 11.4 Relation to 62351-4 / PKI

13. Must we provide a **separate PID** for 62351-4 E2E? Which mode is tested — compatibility (A-security) or **native E2E**?  
14. Is **62351-100-4** published and executed by you, or still draft?  
15. Do you check **certificate profiles** (62351-9 / Annex T) and **enrolment** (EST / SCEP), or only pre-installed certificates?  
16. Which **trust anchors** are simulated (DSO CA, admin-domain CA, manufacturer CA)?

### 11.5 Process / CEI

17. Can 100-3 run in the **same visit** as 61850-10 on the same firmware?  
18. Pricing, duration, re-test rules, certificate wording (62351-3 edition + 100-3 reference) acceptable to Italian DSO under **Allegato O §12**?  
19. Do you accept HiTEKS **Allegato T** PIXIT values (port **3782**, IntgPd **4000** for app context, CRL policy) as written in D7 / `PIXIT_DRAFT.md`?

---

## 12. Related corpus (Allegato O/T + lab)

| Doc | RAG / path | Role |
|-----|------------|------|
| Allegato O extract | `cei-0-16-allegato-o` · `CCI_Annex_O_Extract.md` | §12 mandates **62351-100-3** accredited cert |
| Allegato T extract | `cei-0-16-allegato-t-extract` · `CCI_Annex_T_Extract.md` | T.3.3.4 TLS / PKI / logging profile |
| 62351-3 requirements | `ccli-62351-3-extract` | Normative profile under test |
| D7 PID draft | `lab/conformance/D7_62351-3_PID_DRAFT.md` | PICS+PIXIT for 100-3 quote |
| Lab requirement matrix | `ccli-lab-requirement-test-matrix` | Pre-lab TSP rows (P3-06 / T3-*) |
| 61850-10 extract | `ccli-61850-10-extract` | Functional UCA — **not** transport cases |
| 62351-100-4 planning | `ccli-62351-100-4-extract` · `CCI_62351-100-4_Extract.md` | MMS/ACSE cases · PDF **MISSING** |

---

## 13. Regenerate / ingest

```powershell
python scripts/extract-62351-100-3.py
powershell -File scripts/ingest-62351-100-3.ps1
```

**OCR quality:** doc88 scan; prose clauses **5–14** readable; **Tables 1–5** need licensed PDF for exact case IDs.

---

**RAG tags:** `62351-100-3`, `62351-3`, `TLS`, `transport`, `conformance`, `PICS`, `PIXIT`, `Table 2`, `Table 3`, `3782`, `Allegato-O`, `Allegato-T`, `CEI-0-16`, `P7-07`, `ccli-62351-100-3-extract`, `ccli-62351-100-3-ocr-corpus`

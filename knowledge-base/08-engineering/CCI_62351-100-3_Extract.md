# IEC TS 62351-100-3 — Transport conformance test extract

**Document ID:** CCLI-SEC-62351-100-3-001  
**Revision:** 1.0  
**Date:** 2026-10-08  
**RAG source_id:** `ccli-62351-100-3-extract`  
**Full text:** `ccli-62351-100-3-ocr-corpus` → `reference/iec62351/IEC_TS_62351-100-3_2020_full.txt`  
**Corpus (OCR):** `knowledge-base/08-engineering/reference/iec62351/_extract_62351-100-3/full.txt`  
**Source PDF:** `knowledge-base/08-engineering/reference/iec62351/IEC_TS_62351-100-3-2020.pdf` (local; gitignored)  
**Standard identity:** **IEC TS 62351-100-3:2020** (Edition **1.0**, 2020-01) — *Conformance test cases for **IEC 62351-3** (TCP/IP TLS profile)*  

**Normative profile (separate RAG):** `ccli-62351-3-extract` — **62351-3:2014+AMD1:2018+AMD2:2020**  
**Parents:** `ccli-62351-3-extract` · `ccli-62351-9-extract` · `ccli-61850-10-extract` · `cei-0-16-allegato-o`  
**Programme:** Annex O **62351-100-3** accredited transport cert · **C1** MMS `:3782` · P7-07

---

## 1. What this document is (vs 62351-3)

| Document | Role |
|----------|------|
| **IEC 62351-3** | **Requirements** — TLS 1.2, ciphers, mutual auth, resumption/renegotiation, ports, security events |
| **IEC TS 62351-100-3** | **Abstract test cases** — how a lab verifies 62351-3 on a DUT (client **and** server) |
| **RFC TLS tests** | **Out of scope** of 100-3 body — lab may add separate RFC conformance |
| **62351-8 / 62351-9** | Extra cases for RBAC certs / PKI profiles when applicable |

**Scope note (§1):** Tests **62351-3 integration** over TCP/IP, **not** raw TLS RFC conformance. **61850-10 §6.3** points here for **cyber** device tests alongside **62351-100-4/6**.

**HiTEKS CCI:** DUT is **MMS server** on Eth_A; may also be tested as **TLS client** toward DSO if PICS claims client.

---

## 2. Documents to submit (PID)

**Protocol Implementation Document (PID) = PICS + PIXIT** (§3.1, §4.3.2):

| Item | Content | CCLI status |
|------|---------|-------------|
| **PICS** | Capabilities per **62351-3** (incl. §8 tables in base standard) | **TODO** — derive from OpenWrt/wolfssl + `ccli` policy |
| **PIXIT** | Ports, cipher order, cert paths, renegotiation interval, OCSP/CRL URLs | **PART** — lab yaml, `:3782`, EJBCA PEM pack |
| **DUT ready** | Configured per PID; representative data points; human-readable verification | TG544 + ICD subset |
| **Manuals** | Install/operate or on-site support | Deploy docs |

**Application context (§4.3.1):** All cases run **in context of a real application protocol** that **requires 62351-3** — for CCI: **IEC 61850-8-1 MMS** over TLS (Annex T / CEI O).

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

**HiTEKS lab:** Align with Annex §10.4 OpenSSL gates + TSP Connect once **ISO session** patch is on DUT.

### 5.4 Clause 7 — Results charts (Tables 4–5)

Matrix: each **configuration value** × each **test case** → P / F / NA.  
Accredited cert (e.g. IMQ sample) cites **62351-3:2020** with report ID; procedure aligns with **100-3** tables.

---

## 6. Requirements (HiTEKS trace)

| REQ-ID | Source | Requirement | Verification |
|--------|--------|-------------|--------------|
| **REQ-1003-DOC-001** | §4.3.2 | Submit **PID** (PICS+PIXIT) with DUT | Document review pre-test |
| **REQ-1003-APP-001** | §4.3.1 | Tests under **61850 MMS** (or other 62351-3-requiring app) | Lab script over `:3782` |
| **REQ-1003-ROLE-001** | §4.2.2 | **Server** station tests mandatory; **client** if claimed | TG544 server P0 |
| **REQ-1003-TLS-001** | Table 2 / §6 | Normal TLS + app procedures pass | Automated + pcap |
| **REQ-1003-TLS-002** | Table 3 / §6 | Resiliency cases pass | Negative cert injection |
| **REQ-1003-LOG-001** | §4.3.4 | Handshake/reneg/resumption + cert results logged | DUT syslog / tester |
| **REQ-1003-RPT-001** | §4.3.3 | Completed Tables 4–5 + test report | Accredited body output |
| **REQ-1003-PORT-001** | Clause 5 / 62351-3 §5.7 | Distinct secure port documented in PIXIT | **3782** in PIXIT |

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
| **PICS/PIXIT (62351-3)** | Policy in extract | Formal PID document | **TODO** |
| **Table 2/3 case list** | OCR unreadable | Licensed PDF row IDs | Verify at lab |
| **Accredited lab** | DNV 61850 sample | 62351-100-3 quote | **OPEN** |
| **DUT TLS on :3782** | PEMs ready | Session layer on firmware | iso_session fix |

---

## 9. Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-1003-01 | Confuse **62351-3 cert** with **100-3 test plan** only | Wrong lab scope | Cite both standards in RFQ |
| R-1003-02 | Test MMS without TLS | Invalid 100-3 session | Always bind to :3782 |
| R-1003-03 | OCR table IDs wrong | Wrong internal checklist | Re-type Tables 1–3 from licensed PDF |

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

### 11.5 Process

17. Can 100-3 run in the **same visit** as 61850-10 on the same firmware?  
18. Pricing, duration, re-test rules, certificate wording (62351-3 edition + 100-3 reference) acceptable to Italian DSO.  

---

## 12. Regenerate / ingest

```powershell
python scripts/extract-62351-100-3.py
powershell -File scripts/ingest-62351-100-3.ps1
```

**OCR quality:** doc88 scan; prose clauses **5–14** readable; **Tables 1–5** need licensed PDF for exact case IDs.

---

**RAG tags:** `62351-100-3`, `62351-3`, `TLS`, `transport`, `conformance`, `PICS`, `PIXIT`, `Table 2`, `Table 3`, `3782`, `ccli-62351-100-3-extract`, `ccli-62351-100-3-ocr-corpus`

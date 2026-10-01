# IEC 62351-3 — Capture Plan (CCLI / TG-524)

**Document ID:** CCLI-SEC-62351-002  
**Revision:** 1.0  
**Date:** 2026-08-12  
**RAG source_id:** `ccli-62351-3-capture-plan` → `ccli-62351-3-extract`  
**Normative basis:** IEC 62351-3:2014+AMD1:2018+AMD2:2020 CSV — *Profiles including TCP/IP*  
**Capture status:** **HAVE (P0)** — Batch A pp. **6–19**  
**Parent:** `ccli-62351-1-extract` · `ccli-62351-4-extract` · `ccli-62443-zones-extract`  
**Programme link:** K6.5 · **C1** (:3782) · **C2** (60870-5-104 TLS)

---

## Summary

**62351-3** is the **normative TLS transport profile** for IEC TC 57 protocols over **TCP/IP** (RFC 5246 TLS 1.2). It specifies cipher suites, session resumption/renegotiation, certificate handling, and PICS tables for referencing standards (**61850**, **60870-5-104**, ICCP, DNP3/TCP).

**TG-524:** **62351-3 TLS** under **62351-4 E2E** on **C1**; standalone **62351-3** on **C2 104**.

**Document length:** ~20 pp. (consolidated CSV including AMD2:2020).

---

## ToC (confirmed from PDF)

| § | Title | Page |
|---|-------|------|
| — | Foreword | 3 |
| — | Introduction to Amendment 2 | 5 |
| **1** | Scope | 6 |
| **2** | Normative references | 6–7 |
| **3** | Terms, definitions, abbreviations | 7 |
| **4** | Security issues addressed | 7–8 |
| **5** | **Mandatory requirements** | 9–16 |
| **6** | Optional security measure support | 16 |
| **7** | Referencing standard requirements | 17 |
| **8** | **Conformance** (Tables 1–5) | 17–19 |
| — | Bibliography | 20 |

---

## Capture batches

### Batch A — Full normative core — **COMPLETE**

```
6–19   ✓ §1 Scope · §2 refs · §4 threats/events · §5 TLS mandatory · §7–§8 PICS Tables 1–5
```

Optional: **pp. 3–5, 20** (foreword, AMD2 intro, bibliography).

---

## CCLI mapping template

| 62351-3 concept | Conduit | 62443 / sibling |
|-----------------|---------|-----------------|
| §5.2 TLS 1.2 **m** | C1 **:3782**, C2 TLS | CR 3.1, 4.1 |
| §5.3–5.4 resumption/renegotiation ≤24 h | Permanent SCADA links | CR 1.8 CRL align |
| §5.6 mutual TLS + RSA ≥2048 | TPM Cert A (TLS) | CR 1.8 PKI |
| §5.6.4.4 CRL unreachable ≠ terminate | OpenWrt policy | Matches 62351-4 §6.2.8 |
| §4.4 / §5 security events | **62351-7/14** | CR 6.2 |
| §5.7 separate TCP port | **3782 / 102** (via 62351-4 §6.3) | C1 firewall |
| §8 Tables 1–5 PICS | Lab declaration | K6.5 |

---

## Verification

| Item | Pass criteria |
|------|---------------|
| K6.5 transport | **62351-3 HAVE** + 62351-4 HAVE → full C1 stack documented |
| 62351-4 §6.2.1–7 gap | Filled by **62351-3 §5** cross-ref |
| OpenWrt cipher policy | Align Table 1 + 62351-4 Table 3 |
| Lab | Mutual TLS + resumption/renegotiation test plan |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-08-12 | **Batch A** pp. 6–19 captured |

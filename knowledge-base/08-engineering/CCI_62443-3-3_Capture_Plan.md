# IEC 62443-3-3 — Capture Plan (CCLI / TG-524)

**Document ID:** CCLI-SEC-62443-002  
**Revision:** 2.0  
**Date:** 2026-08-11  
**RAG source_id:** `ccli-62443-3-3-capture-plan` · Batch A → `ccli-62443-3-3-extract`  
**Normative basis:** IEC 62443-3-3:2013 — *System security requirements and security levels*  
**Capture status:** **HAVE** — normative body pp. 24–78 (FR 1–7 + Annex A/B)  
**Parent:** `ccli-62443-zones-extract` (SL-T 2 proposed for Z0, C1, C2)  
**Programme link:** K6.4 · K6.5 · KR-012

---

## Summary

Your ToC screenshots confirm **62443-3-3:2013**. This part defines **Foundational Requirements (FR 1–7)** and **System Requirements (SR x.y)** with **SL-C** capability levels 1–4.

**62443-3-2** sets **SL-T** (done — `CCI_62443_Zones.md`). **62443-3-3** selects **countermeasures** and assigns **SL-C** to each SR (ZCR 5.8 / 5.12). For the **TG-524 as a certified component**, **62443-4-2** is the lab test basis — but 3-3 vocabulary and FR/SR structure are the same family; ingest 3-3 first if 4-2 PDF is not yet available.

**Target for CCLI:** map **SL-T 2** zones to minimum **SL-C 2** on applicable SRs for Z0 (CCI core).

---

## ToC captured (from screenshots)

| § | Title | Page |
|---|-------|------|
| 0 | Introduction (0.1–0.3 usage in 62443 series) | 11–12 |
| 1 | Scope | 14 |
| 2 | Normative references | 14 |
| 3 | Terms, definitions, acronyms (3.1–3.3) | 14–22 |
| 4 | Common control system security constraints | 22–24 |
| 4.4 | Least privilege | 24 |
| **5** | **FR 1 — Identification and authentication control (IAC)** | 24+ |
| 5.3 | SR 1.1 — Human user identification and authentication | 24–25 |
| 5.4 | SR 1.2 — Software process and device identification and authentication | 26–27 |
| 5.5 | SR 1.3 — Account management | 27 |
| 5.6 | SR 1.4 — Identifier management | 28 |
| 5.7 | SR 1.5 — Authenticator management | 28–29 |
| 5.8 | SR 1.6 — Wireless access management | 30 |
| 5.9 | SR 1.7 — Strength of password-based authentication | 30–31 |
| 5.10 | SR 1.8 — Public key infrastructure (PKI) certificates | 31–32 |
| 5.11 | SR 1.9 — Strength of public key authentication | 32–33 |
| 5.12 | SR 1.10 — Authenticator feedback | 33 |
| 5.13 | SR 1.11 — Unsuccessful login attempts | 34 |
| 5.14 | SR 1.12 — System use notification | 34–35 |
| 5.15 | SR 1.13 — Access via untrusted network | 35 |
| **6** | **FR 2 — Use control (UC)** | 36+ |
| 6.3 | SR 2.1 — Authorization enforcement | 36–37 |
| 6.4 | SR 2.2 — Wireless use control | 37–38 |
| *(continues)* | FR 2 SR 2.3+ · FR 3–7 | *not in screenshot* |

Each SR follows: **Requirement → Rationale → Enhancements → Security levels (SL 1–4)**.

---

## Capture priority — SL-T 2 CCI (P0)

Capture **`§x.y.4 Security levels`** for every SR below **plus** intro pages and Annex A (SL vectors).

### Batch A — Foundation (capture first)

```
11–15, 17, 20–24   ✓ captured
```

### Batch B — FR 1 IAC — **COMPLETE**

```
24–35   ✓ SR 1.1–1.13 + all SL tables
```

### Batch C — FR 2 Use control — **COMPLETE**

```
36–44   ✓ SR 2.1–2.12 + all SL tables
```

### Patch 3 — FR 6–7 + Annex A/B — **COMPLETE**

```
61–78   ✓ FR 6 complete (SR 6.2 SL) · FR 7 complete (SR 7.1–7.8) · Annex A/B
```

### Optional remaining

```
16      ← §3.1 terms continuation (non-blocking)
```

| SR | CCLI relevance |
|----|----------------|
| **1.1** | Local console / maintainer login (Z4) |
| **1.2** | Process/device auth — MMS client, Modbus identity |
| **1.3–1.5** | Account / credential lifecycle (K6.3 ceremony) |
| **1.6** | Wi‑Fi disabled; LTE isolated (Z5) |
| **1.7–1.9** | Password policy; **PKI for 62351 TLS** (K6.5) |
| **1.11–1.13** | Lockout; banner; **untrusted network = WAN/LTE** |

### Batch C — FR 2 Use control (RBAC, authorization)

```
36, 37, 38, …
```
Through FR 2 completion (SR 2.x SL tables). **SR 2.1** authorization maps to `apps/ccli/` RBAC and firewall rules.

### Batch D — FR 3–7 (P0 for SL 2 system)

| FR | Topic | CCLI hook |
|----|-------|-----------|
| **FR 3** | System integrity | Secure Boot, signed images (K6.1) |
| **FR 4** | Data confidentiality | 62351 TLS on C1/C2 |
| **FR 5** | Restricted data flow | No L2 bridge (K3.3), zone firewall |
| **FR 6** | Timely response to events | Audit log, alerts |
| **FR 7** | Resource availability | DoS / rate limits on services |

*Page numbers for FR 3–7: capture from full ToC when available — typically pp. ~40–90.*

### Batch E — Annexes (P0)

```
Annex A — Security level vectors (SL-C composition)
Annex B — FR/SR overview matrix (if present in 2013 edition)
```

---

## CCLI mapping template (fill during ingest)

| FR | SR | SL-T | SL-C target | CCLI control | Status |
|----|-----|------|-------------|--------------|--------|
| 1 | 1.1 | 2 | 2 | Console RBAC + local auth | PART |
| 1 | 1.8 | 2 | 2 | TPM-stored certs; 62351 MMS | MISS |
| 1 | 1.13 | 2 | 2 | LTE/VPN; deny bridge to Eth_A | PART |
| 2 | 2.1 | 2 | 2 | Role-based API / UCI policy | PART |
| 3 | 3.x | 2 | 2 | Secure Boot + signed `.ipk` | PART |
| 5 | 5.x | 2 | 2 | `net_policy` / OpenWrt firewall | PART |

---

## Relationship to other parts

| Part | Role | Corpus |
|------|------|--------|
| **62443-3-2** | Zones, SL-T, CRS | `ccli-62443-zones-extract` PART |
| **62443-3-3** | FR/SR, SL-C (system) | `ccli-62443-3-3-extract` **HAVE** |
| **62443-4-2** | FR/SR, SL-C (component lab test) | MISSING — P0 cert |
| **62443-4-1** | SDLC process audit | MISSING — P0 cert |

Lab scope for TG-524 will cite **4-2**; design team uses **3-3** SR language in CRS and traceability matrix.

---

## Suggested screenshot order (same workflow as 3-2)

1. **Batch A** (6 pages) — scope + constraints  
2. **Batch B** — FR 1 SL tables (13 pages)  
3. **Batch C** — FR 2 start (3+ pages)  
4. Send for ingest → `CCI_62443-3-3_Extract.md`  
5. Continue FR 3–7 + Annex A as ToC pages become available  

---

## Verification

| Item | Pass criteria |
|------|---------------|
| SL-T → SL-C trace | Every Z0 SR has SL-C ≥ SL-T 2 or compensating control documented |
| 4-2 alignment | 3-3 extract SR IDs match 4-2 component checklist (lab quote) |
| K6.4 | Moves toward HAVE when FR 1–7 SL-2 rows complete + workshop sign-off |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-08-11 | ToC from user screenshots; P0 capture batches for SL-T 2 CCI |
| 1.1 | 2026-08-11 | Patch 2 status: FR 3–5 complete; FR 6 partial pp. 51–60 |
| **2.0** | 2026-08-11 | Patch 3 pp. 61–78: FR 6–7 + Annex A/B; corpus **HAVE** |

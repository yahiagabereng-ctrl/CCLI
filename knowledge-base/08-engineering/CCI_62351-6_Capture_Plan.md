# IEC 62351-6 — Capture Plan (CCLI / TG-524)

**Document ID:** CCLI-SEC-62351-006  
**Revision:** 1.0  
**Date:** 2026-09-16  
**RAG source_id:** `ccli-62351-6-capture-plan` → `ccli-62351-6-extract`  
**Normative basis:** IEC 62351-6:2020 — *Security for IEC 61850 profiles including GOOSE, SV, and SNTP*  
**Source PDF:** `knowledge-base/07-protocols/62351-6.pdf` (doc88 image scan, 53 images ≈ 38 pp.)  
**Parent:** `ccli-62351-1-extract` · `ccli-62351-9-extract` · `ccli-61850-6-extract`  
**Programme link:** K6.5 Phase 3 · plant GOOSE/SV · `McSecurity` in SCL

---

## Summary

**62351-6:2020** specifies **message-level authentication** (HMAC) and optional **encryption** (AES-GCM) for **61850 GOOSE/SV/SNTP**, **GDOI group keys** via **62351-9**, and **SCL extensions** (`McSecurity`, `kdaParticipant`). **MMS/TCP** remains **62351-3 + 62351-4**.

**TG-524 CCLI:** **P1** for plant multicast; **not C1 MMS P0**. Cross-ref **61850-6 §9** `GOOSESecurity`/`SMVSecurity` + `Authentication certificate="true"`.

**PDF note:** doc88 capture is **image-only** and **page order scrambled** (Scope at PDF p.53). OCR ingested 2026-09-16.

---

## Capture status roll-up

| Batch | Focus | Status |
|-------|-------|--------|
| **A** | §1 Scope · Table 1 protocols | **COMPLETE** (PDF p.53 OCR) |
| **B** | §4 Threats · §4.1 L2 3 ms / no encryption default | **COMPLETE** |
| **C** | §6.2 GOOSE replay state machine · security checks | **COMPLETE** |
| **D** | §8.2 Extension PDU · AuthenticationValue · Key ID · GDOI | **COMPLETE** |
| **E** | §9 SCL — `McSecurity` on GSE/SMV/ClientServices | **COMPLETE** |
| **F** | §11 PICS — Tables 8–13 (L2/Routable GOOSE/SMV) | **COMPLETE** |
| **SKIP** | §5 full · §6.3 VLAN · §10 SNTP detail · Annex | P2 unless DSO mandates |

---

## ToC (from OCR — confirm printed page on re-scan)

| § | Title | OCR ref |
|---|-------|---------|
| **1** | Scope · Table 1 (61850-8-1/8-2/9-2/6) | PDF p.53 |
| **4** | Security issues · L2 performance note | PDF p.5–8 |
| **6.2** | Replay protection · GOOSE state machine | PDF p.14–16 |
| **8.2** | Extension octets · MAC · Key ID · publishers/subscribers | PDF p.28–35 |
| **9** | SCL service capability extensions | PDF p.36–38 |
| **11** | Conformance · PICS Tables 8–13 | PDF p.42–50 |

---

## CCLI mapping

| 62351-6 concept | CCLI artefact |
|-----------------|---------------|
| L2 GOOSE auth only (default) | Plant bus Phase 3; aligns with 62351-1 Table 2 |
| `McSecurity signature/encryption` | Extend CID/SCL tooling; `ccli-61850-6-extract` §9 |
| GDOI / Key ID | `ccli-62351-9-extract` §8 |
| PICS L2G3 HMAC-SHA256-128 | Lab declaration if GOOSE enabled |
| Routable GOOSE encryption | **Not** C1 Eth_A default |

---

## Verification

| Item | Pass criteria |
|------|---------------|
| Batch A | Extract cites Table 1 protocols |
| Batch E | Extract §9 ↔ 61850-6 `McSecurity` gate |
| Batch F | Extract Tables 9–12 ↔ PICS `m` markers |
| Corpus | `ccli-62351-6-extract` **HAVE (P1 P0 core)** |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-09-16 | Initial capture from `62351-6.pdf` OCR |

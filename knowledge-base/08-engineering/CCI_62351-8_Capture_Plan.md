# IEC TS 62351-8 — Capture Plan (CCLI / TG-524)

**Document ID:** CCLI-SEC-62351-008  
**Revision:** 1.0  
**Date:** 2026-09-16  
**RAG source_id:** `ccli-62351-8-capture-plan` → `ccli-62351-8-extract`  
**Normative basis:** IEC TS 62351-8:2011(E) — *Role-based access control*  
**Source PDF:** `knowledge-base/07-protocols/62351-8.pdf` (doc88 image scan, 53 images ≈ 48 pp.)  
**Parent:** `ccli-62351-4-extract` · `ccli-62351-9-extract` · `cei-0-16-allegato-t`  
**Programme link:** K6.5 · Annex T §T.3.3.4 RBAC · Phase 3 MMS

---

## Summary

**62351-8** defines **RBAC** for TC 57 protocols: **7 mandatory roles**, **11 mandatory rights**, **ACSI service mapping**, **SECAUD** logging, **SCL-based role-to-right assignment**, and **access token profiles A–D** (X.509 cert, attribute cert, software token, RADIUS).

**CEI Annex T** adds **two private roles** (`DSO_OPERATOR` **-1**, `AGGREGATOR_OPERATOR` **-2**) on top of the seven standard roles.

**PDF note:** doc88 capture **starts mid-document** (≈ p.20+); §1 Scope **MISSING** in PDF. Core §5.2 + §12 captured via OCR 2026-09-16.

---

## Capture status roll-up

| Batch | Focus | Status |
|-------|-------|--------|
| **A** | §5.2.1 Tables 1–3 — roles · rights · pre-defined roles | **COMPLETE** (PDF p.48–51) |
| **B** | §5.2.2 IEC 61850 — session · ACSI access model | **COMPLETE** (PDF p.52) |
| **C** | §5.2.2.1 Tables 5–7 — ALLOW/DENY · VIEW services | **COMPLETE** (PDF p.1–2, 53) |
| **D** | §5.2.2.3–4 SECAUD log · SCL configuration | **COMPLETE** (PDF p.2–3) |
| **E** | §12 Profiles A–C · access tokens · AoR extension | **COMPLETE** (PDF p.34–38) |
| **F** | §6–7 LDAP repository · token verify (partial) | **PART** |
| **MISSING** | §1 Scope · §9.5 cert profiles detail · Annex | Re-scan front matter |

---

## CCLI mapping

| 62351-8 concept | CCLI artefact |
|-----------------|---------------|
| 7 standard roles | Baseline RBAC engine |
| Private roles -1 / -2 | `CCI_Annex_T_Extract.md` Tables 97–99 |
| Profile A (X.509 + `IECUserRoles`) | Dual cert stack with 62351-4/9 |
| Profile B attribute cert | Annex T PMI recommendation < 24 h |
| SCL role-to-right + revision | Future CID extension / commissioning |
| SECAUD log | 62351-14 / CR 6.2 audit |

---

## Verification

| Item | Pass criteria |
|------|---------------|
| Batch A | Extract Table 3 seven roles documented |
| Annex T crosswalk | DSO/AGG roles map to private range -32768..-1 |
| 62351-4 ref | §2 cites 62351-8 RBAC — consistent |
| Corpus | `ccli-62351-8-extract` **HAVE (P1 core)** · front matter **PART** |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-09-16 | Initial capture from `62351-8.pdf` OCR |

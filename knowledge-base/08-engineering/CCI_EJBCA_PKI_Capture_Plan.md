# EJBCA / Product PKI — Capture Plan

**Document ID:** CCLI-PKI-EJBCA-001  
**Revision:** 1.0  
**Date:** 2026-10-06  
**RAG source_id:** `ccli-ejbca-pki-capture-plan` → `ccli-ejbca-pki-extract`  
**Parent:** `ccli-k63-ceremony-sop` · `ccli-62351-9-extract` · `ccli-62351-4-extract`  
**Software:** EJBCA Community Edition (LGPL) — lab only

---

## Summary

Capture corpus for **product-like PKI** using **EJBCA CE** to replace lab `gen_lab_pki.py` over time. Normative requirements remain in 62351/Annex T extracts; this plan covers **tool docs**, **RFCs**, and **TSP wire evidence**.

---

## Capture status

| Batch | Content | Status |
|-------|---------|--------|
| **A** | RFC 7030/8894/5280/6960 text | **INGESTED** — `reference/pki/` |
| **B** | EJBCA doc snapshots (EST, profiles, extensions) | **INGESTED** — `reference/vendor/ejbca/docs/` |
| **C** | EJBCA CE Docker image | **HAVE** — `keyfactor/ejbca-ce:latest` local |
| **D** | `CCI_EJBCA_PKI_Extract.md` (K6.3 ↔ EJBCA mapping) | **INGESTED** |
| **E** | TSP P3-07 evidence (cert shapes) | **INGESTED** — sync via `ingest-ejbca-pki.ps1` |
| **F** | EJBCA live profile export (Cert A/B issued PEMs) | **MISSING** — after first lab enrolment |
| **G** | IEEE 802.1AR IDevID | **MISSING** — factory Phase 0 |
| **H** | 62351-100-3 transport test procedure | **MISSING** — P7 |

---

## Software acquisition

| Item | Method | Path / command |
|------|--------|----------------|
| EJBCA CE container | `docker pull keyfactor/ejbca-ce:latest` | `reference/vendor/ejbca/docker/image-info.txt` |
| RFC sources | `scripts/sync-pki-rfc-reference.ps1` | `reference/pki/*.txt` |
| EJBCA source (optional) | GitHub `Keyfactor/ejbca-ce` | Not cloned (Docker sufficient for lab) |

---

## Ingest commands

```powershell
cd c:\Yahia\projects\CCLI

# Sync RFCs (if missing)
powershell -File scripts\sync-pki-rfc-reference.ps1

# PKI + EJBCA + TSP evidence → Telematry
powershell -File scripts\ingest-ejbca-pki.ps1

# Parent 62351 security corpus
powershell -File scripts\ingest-62351-security-debug.ps1

# Re-run with RAG up:
# powershell -File scripts\ingest-ejbca-pki.ps1
```

---

## Verification

| Check | Pass |
|-------|------|
| RFC7030 on disk | `reference/pki/RFC7030_EST.txt` > 100 KB |
| Docker image | `docker images keyfactor/ejbca-ce` |
| Extract readable | `CCI_EJBCA_PKI_Extract.md` lists Cert A/B |
| RAG query | `source_id:ccli-ejbca-pki-extract` returns profile table |

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-06 | Initial capture — RFCs, EJBCA docs, Docker CE, extract |

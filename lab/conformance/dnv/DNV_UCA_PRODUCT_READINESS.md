# DNV / UCA product readiness — O.15 vs product deploy

**Document ID:** CCLI-DNV-PROD-001  
**Revision:** 1.0  
**Date:** 2026-10-14  
**RAG source_id:** `ccli-dnv-product-readiness`  
**Parents:** `ccli-certificate-samples-extract` · `ccli-61850-10-extract` · `ccli-lab-uca-document-checklist` · `ccli-product-sw-freeze`

Separates **field product deployment** (HiTEKS release bundle) from **accredited certificate** (e.g. DNV Netherlands UCA IUG).

---

## Requirements

| ID | Requirement | Product deploy | DNV 61850 cert |
|----|-------------|----------------|----------------|
| REQ-DNV-P-001 | Frozen firmware semver-rN + manifest | **M** | **M** on certificate specimen |
| REQ-DNV-P-002 | MMS TLS :3782 + PKI layout | **M** | **M** |
| REQ-DNV-P-003 | PICS/PIXIT/MICS submitted match DUT model | **O** (pre-quote) | **M** |
| REQ-DNV-P-004 | TSP pre-test evidence | **M** (internal) | Supporting only |
| REQ-DNV-P-005 | 62351-100-3 accredited transport | **OPEN** P7-07 | **M** if O §12 requires |

---

## Annex O §12 deliverables (corpus map)

| # | Deliverable | RAG / path | Status |
|---|-------------|------------|--------|
| 5 | IEC 61850-10 UCA | `ccli-61850-10-extract` · D1–D5 lab docs | Pre-test **PART** |
| 6 | 62351-3 / 100-3 | `ccli-62351-100-3-extract` · D7 | Plan **OPEN** |
| 2 | 61557-12 lab tests | `ccli-61557-12-extract` | **PART** |
| 4 | 62443-4-1/4-2 | extracts **HAVE** | Cert **OPEN** |

Sample UCA scope: `ccli-certificate-samples-extract` (Higeco / DNV TP v2.0.6).

**Clause-by-clause book (O/M/T/U + #1–#8):** [Full Validate.md](../Full%20Validate.md) (`ccli-normative-cert-validation-matrix`).

---

## Test Suite Pro vs DNV

| Gate | Tool | Zero-error target |
|------|------|-------------------|
| Internal | TSP SCL Verify | **0 Error** on ship CID |
| Internal | TSP Compare | 0 LN/DO **MISSING**; Value waivers documented |
| Accredited | Lab 61850-10 programme | All PICS **M** rows pass |

Flow: `ccli-lab-dut-finalize-cert-path`.

---

## Product 5-LN model (planned)

Certificate model may be **5 LN** product CID (`LLN0`, `LPHD1`, `PdCMMXU1`, `WlimDWMX1`, `WSdDAGC1`) while lab evidence may reference **31 LN** rev 9 — declare both in PICS Cover and MICS.

---

## Verification

| Check | Pass |
|-------|------|
| RAG probes § RAG_SOURCE_INDEX product + DNV clusters | All P0 **HAVE** on disk |
| PICS Excel pasted | Lab quote unblocked |
| Specimen cfg SHA on DUT = pin doc | Compare rev 9 (or product pin) |

---

**Keywords:** `DNV`, `UCA`, `O.15`, `product`, `ccli-dnv-product-readiness`

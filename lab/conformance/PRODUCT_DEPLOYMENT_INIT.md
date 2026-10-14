# Product deployment initialization

**Document ID:** CCLI-PROD-DEPLOY-INIT-001  
**Revision:** 1.1  
**Date:** 2026-10-14  
**RAG source_id:** `ccli-product-deployment-init`  
**Parents:** `ccli-product-sw-freeze` · `ccli-rag-source-index` · `ccli-lab-dut-finalize-cert-path`

One-page gate before first **`releases/product-cyber/<semver>-rN>/`** promotion.

---

## 1. RAG (do first)

| Step | Action | Pass |
|------|--------|------|
| R1 | `scripts\sync-ccli-corpus-to-telematry.ps1` | Telematry `documents/projects/ccli/` updated |
| R2 | `scripts\ingest-product-deploy-rag.ps1` | No **Skip** warnings for P0 ids (product + **test-req** cluster) |
| R3 | `scripts\ingest-next-phases.ps1 -SkipDocker` | Bulk + priority jobs; registry merge |
| R4 | Query RAG: `ccli-product-sw-freeze`, `ccli-lab-requirement-test-matrix`, `ccli-62351-100-4-ocr-corpus` | Hits return current rev |
| R5 | `scripts\ingest-62351-100-4.ps1 -SkipSync` after PDF/OCR only | 3 cyber jobs enqueued |

**One-shot finalize (RAG up on :8001):**

```powershell
powershell -File scripts\ingest-product-deploy-rag.ps1
powershell -File scripts\ingest-next-phases.ps1 -SkipDocker
powershell -File scripts\ingest-62351-100-4.ps1 -SkipSync
```

Index: [RAG_SOURCE_INDEX.md](RAG_SOURCE_INDEX.md)

---

## 2. Frozen platform (already HAVE)

| Item | source_id |
|------|-----------|
| SoC / TG-524 line | `ccli-soc-freeze` |
| OpenWrt 25.12 | `ccli-openwrt-freeze` |
| Application layout | `ccli-app-structure` |

---

## 3. Build + version

| Step | Action |
|------|--------|
| B1 | Bump `apps/ccli/VERSION` · `scripts/sync-ccli-version.ps1` |
| B2 | `lab/tg544-openwrt/build-ccli-ipk-sdk.sh` → `.apk` |
| B3 | Deploy with TLS yaml (`lab_tr400_phase4_eth_b.yaml` or future product yaml) |

---

## 4. MMS specimen (lab today · product 5-LN next)

| Mode | CID / cfg | RAG |
|------|-----------|-----|
| **Lab (31 LN)** | `lab_tg544_eth_a.cid` rev **9** | `ccli-lab-specimen-version` |
| **Product (5 LN)** | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid` (**TODO**) | Pin: [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md) · matrix: [LAB_UNIFIED_CERT_TEST_MATRIX.md](LAB_UNIFIED_CERT_TEST_MATRIX.md) |

TSP target: SCL Verify **0 Error** on the CID you ship; Compare structural match — see `ccli-tsp-compare-waivers`.

---

## 5. Cyber profile (product)

- Eth_A **:3782** TLS + 62351-4 ACSE — **no** cleartext **:102** in release bundle  
- Zones/conduits: `ccli-62443-zones-extract`  
- D7 transport PID: `ccli-lab-d7-62351-3-pid`

---

## 6. DNV / UCA path (parallel)

Not blocking `.apk` ship; blocks **O.15 certificate**:

- D1 PICS Excel from `ccli-lab-pics-fill`  
- D2 PIXIT · D3 MICS · D5 CID  
- Checklist: `ccli-dnv-product-readiness` · `ccli-lab-uca-document-checklist`

Internal TSP T1–T10 ≠ accredited 61850-10 run (`ccli-lab-dut-finalize-cert-path`).

---

## 7. Promote release folder

When §1–5 pass on reference DUT:

```text
releases/product-cyber/<semver>-rN>/
├── FREEZE.md · MANIFEST.json · ccli-*.apk
├── config/product_tr400.yaml
└── icd/   (5-LN or lab-aligned cert CID)
```

See `ccli-product-cyber-release`.

---

**Keywords:** `product deployment`, `RAG init`, `ccli-product-deployment-init`

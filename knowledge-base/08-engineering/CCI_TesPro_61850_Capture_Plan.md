# TesPro IEC 61850 Protocol Service — Capture Plan

**Document ID:** CCLI-VENDOR-TESPRO-61850-CP-001  
**Revision:** 1.0  
**Date:** 2026-09-29  
**RAG source_id:** `ccli-tespro-61850-capture-plan` → `ccli-tespro-61850-manual-extract`  
**DOCX source_id:** `ccli-tespro-61850-manual`  
**Source file:** `knowledge-base/08-engineering/reference/vendor/tespro/IEC61850-User-Manual.docx`  
**Parent:** `ccli-tespro-supplier-correspondence` · K4.1 · V-005  
**Programme link:** Vendor IEC 61850 capability vs CCLI DSO MMS path

---

## Summary

TesPro supplied **IEC61850-User-Manual.docx** (WeChat, Sep 2026). Ingest closes supplier open item **V-005** at documentation level.

**Key finding:** Manual documents a **northbound MMS data collector** (poll IEDs on port **102**, upload MQTT/TCP). It does **not** document a **62351-4 secure DSO MMS server** on Eth_A — that remains **`apps/ccli`**.

---

## Ingest pipeline

| Step | Command / path |
|------|----------------|
| 1. Copy DOCX | `powershell -File scripts/ingest-tespro-61850-manual.ps1` |
| 2. Auto-extract | `python scripts/extract-tespro-61850-manual.py` |
| 3. RAG query | `source_id=ccli-tespro-61850-manual-extract` |

**Status:** **HAVE** — ingested 2026-09-29 from OneDrive WeChat export.

---

## Capture batches

### Batch A — Service identity — **COMPLETE**

| Topic | Manual section | Finding |
|-------|----------------|---------|
| Package list | §1.0 | libiec61850, libopen62541, iec61850-mmsd, iec61850-proto-tespro-combined |
| Web UI path | §1.1 | Services → IEC 61850 Protocol |
| Role | Architecture | **MMS client/collector**, not DSO server |

### Batch B — Device / point config — **COMPLETE**

| Topic | Manual section | CCLI relevance |
|-------|----------------|----------------|
| IED Host / MMS Port | §2.x | Default **102** — remote IED target |
| ICD/CID upload | §2.x | Model for **polled device**, not Allegato T server CID |
| RCB / points | §2.x | Northbound reporting — out of CCI DSO scope |

### Batch C — Security — **COMPLETE (negative)**

| Topic | Expected for P3-07 | Manual |
|-------|-------------------|--------|
| TLS port 3782 | REQ-3514-TPROF | **Not mentioned** |
| ACSE AARQ/AARE auth | REQ-3514-APROF | **Not mentioned** |
| Northbound MQTT TLS | Optional upload | **HAVE** §3.x |

### Batch D — Coexistence — **OPEN**

| Topic | Status | Action |
|-------|--------|--------|
| Run mmsd + `ccli` on same TG544 | Not documented | Ask TesPro if both can run; port bind conflict |
| Disable vendor service for lab | Not documented | Bench test: stop iec61850 service, run ccli only |

---

## Verification

| Check | Pass criteria |
|-------|---------------|
| DOCX on disk | `reference/vendor/tespro/IEC61850-User-Manual.docx` |
| Extract | `CCI_TesPro_61850_Manual_Extract.md` present |
| V-005 | Stack = libiec61850 + mmsd; role = collector |

---

## Keywords

`TesPro`, `IEC61850-User-Manual`, `iec61850-mmsd`, `V-005`, `ccli-tespro-61850-capture-plan`

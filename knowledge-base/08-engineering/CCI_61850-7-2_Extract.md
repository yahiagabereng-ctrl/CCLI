# IEC 61850-7-2 — CCLI Engineering Extract (ACSI / MMS)

**Document ID:** CCLI-PROTO-61850-7-2-001  
**Revision:** 1.0  
**Date:** 2026-09-21  
**RAG source_id:** `ccli-61850-7-2-extract`  
**Normative basis:** IEC 61850-7-2 — *Basic information and communication structure — Abstract communication service interface (ACSI)*  
**Licensed PDF:** `07-protocols/regulations/standards-drop-20260918/IEC 61850-7-2.pdf` (231 pp.)  
**Capture status:** **HAVE (P0)** — Batches A–E per `ccli-61850-7-2-capture-plan` (engineering capture; doc88 PDF lacks text layer — verify page cites at conformance lab)  
**Parents:** `ccli-61850-6-extract` · `cei-0-16-allegato-t` · `ccli-62351-8-extract` · `ccli-62351-4-extract`  
**Programme link:** K4.1 · REQ-61850-001 · `mms_adapter.cpp` · libiec61850

---

## Summary

**61850-7-2** defines **ACSI** — the abstract application services for substation automation: **associations**, **directory** browsing, **read/write** of data, **DataSets**, **reporting** (BRCB/URCB), **control** (SBO/direct), **GOOSE/SV** control blocks, and **time** services. **61850-8-1** maps ACSI to **MMS** on TCP (RFC 1006 profile).

**CCLI:** Eth_A DSO path implements a **Server** role with **TR 57-126** data model; minimum ACSI surface is fixed by **Annex T §7** (privilege mapping). Transport and PDU mapping remain **8-1** + **62351-3/4**.

---

## Requirements

| REQ-ID | Source | Requirement | CCLI |
|--------|--------|-------------|------|
| **REQ-872-SCOPE-001** | §1 | ACSI models client/server interactions for 61850 IEDs | MMS server only (Phase 3+) |
| **REQ-872-ASSOC-001** | Association | Client establishes **application association** before services | libiec61850 association handler |
| **REQ-872-ASSOC-002** | Association | **Release** / **Abort** tear down association cleanly | Session lifecycle |
| **REQ-872-DIR-001** | Server | **GetServerDirectory**, **GetLogicalDeviceDirectory**, **GetLogicalNodeDirectory** | Annex T Listobjects privilege |
| **REQ-872-DATA-001** | Data | **GetDataValues**, **GetDataDirectory**, **GetDataDefinition** | Readvalues + Listobjects |
| **REQ-872-DATA-002** | Data | **SetDataValues** only where Annex T grants Control/config | PF2 / setpoints via role-gated DOs |
| **REQ-872-DS-001** | DataSet | **GetDataSetValues**, **GetDataSetDirectory**; no Create/Delete on TR CID | Dataset privilege read-only |
| **REQ-872-RPT-001** | Reporting | **Report** notification + **GetBRCBValues** / **GetURCBValues** | Reporting privilege; **intgPd** 4000 ms |
| **REQ-872-RPT-002** | Reporting | One RCB instance **≤ one client** (reservation rules) | Match TR `RptEnabled max=2` |
| **REQ-872-CTL-001** | Control | **Select / SelectWithValue / Operate / Cancel** per **ctlModel** | ctlModel from 61850-6 templates |
| **REQ-872-TIME-001** | Time | Time quality and sync visible to association peers | NTP/NTS Annex T; ≤ ±100 ms |
| **REQ-872-SEC-001** | Cross-ref | Privileges enforced per **62351-8** roles + Annex T Tables 97–102 | DSO_OPERATOR / AGGREGATOR_OPERATOR |
| **REQ-872-MAP-001** | 61850-8-1 | MMS mapping of each implemented ACSI service | **BLOCKED** until 8-1 PDF in corpus |

---

## Architecture — ACSI under C1

```
DSO client (Z0)                    CCI IED (Z1)
     │  TLS 62351-3 :3782              │
     │  MMS 61850-8-1                  │
     │  ┌──────────────────────────┐   │
     └──│ ACSI (this document)      │───┘
        │  Data model = TR 57-126   │
        │  + 62351-4 E2E (Annex G)  │
        └──────────────────────────┘
```

---

## Batch A — Scope and ACSI model

| Topic | Normative intent | CCLI note |
|-------|------------------|-----------|
| **Server vs Client** | IED may expose **Server** AP; client associations for HMI/DSO | TR CID: single Server AP `accessPoint1` |
| **Logical model** | LD → LN → DO → DA tree | `LD_Plant`, LN0 reports, prefixed LNs |
| **Functional constraints** | FC (ST, MX, CF, DC, …) gate access | Map to 62351-8 rights |
| **Service tracking** | Services may be **tracked** (time-stamped) | Aligned with 61850-5 Type 3 ~500 ms class |

---

## Batch B — Directory and read services

**Minimum Annex T (T.3.3.2) service classes → ACSI:**

| ACSI class | Services | Annex T privilege |
|------------|----------|-------------------|
| Server / Association / LD | GetServerDirectory, Release, Abort, GetLogicalDeviceDirectory | Listobjects |
| Logical Node | GetLogicalNodeDirectory, GetAllDataValues | Listobjects, Readvalues |
| Data Object | GetDataValues, SetDataValues, GetDataDirectory, GetDataDefinition | Readvalues, Control/config, Listobjects |

**CCLI:** libiec61850 exposes these via ICD; restrict **Set** paths in server callback for non-DSO roles.

---

## Batch C — Reporting (BRCB / URCB)

| ACSI concept | 61850-6 SCL | TR 57-126 |
|--------------|-------------|-----------|
| **Integrity period** | `intgPd` ms on ReportControl | **4000** on `urcb_*_Mis4sec` |
| **TrgOps** | `period`, `dchg`, `qchg`, `gi` | period+gi on 4 s URCB |
| **Buffering** | `buffered=true` → BRCB | `brcb_Stato_Allarmi_Segnali` |
| **Reservation** | ClientLN + ResvTms | DSO client slots `max=2` |

**Services:** **Report** (unsolicited), **GetBRCBValues**, **GetURCBValues**; Annex T: **Set NOT applicable** on RCB for DSO read-only reporting profile.

---

## Batch D — Control model

| ctlModel (Enum) | ACSI behaviour | CCLI |
|-----------------|----------------|------|
| status-only | No Oper | Status LNs |
| direct-with-normal-security | Operate | Phase 2 lab |
| sbo-with-normal-security | Select → Operate | Curtailment / setpoints if mapped |
| *-enhanced-security | + 62351-4 auth | Phase 3 production |

**Annex T:** CONTROL privilege on DSO-reserved DOs only (Table 98).

---

## Batch E — GOOSE / SV (ACSI level)

| Item | ACSI | CCLI priority |
|------|------|---------------|
| **GSE control blocks** | GOOSE enable/publish at ACSI | P1 plant bus |
| **SV** | Sampled values streams | Out of P0 CCI |
| **Security** | 62351-6 message auth on wire | After 61850-6 Batch J + 8-1 |

---

## CCLI mapping

| ACSI area | Code / config |
|-----------|---------------|
| Server | `adapters/iec61850_mms/mms_adapter.cpp` → libiec61850 |
| Model | `apps/ccli/config/icd/cei-tr-57-126-example.cid` |
| Privileges | 62351-8 roles + Annex T Tables 97–102 |
| TLS | 62351-3 `:3782` |
| Application security | 62351-4 Annex G Cert B |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-872-DIR-001 | MMS client browse | LD/LN list matches CID |
| REQ-872-RPT-001 | Subscribe URCB | Periodic reports ≤ 4 s + integrity |
| REQ-872-SEC-001 | Role without CONTROL | SetDataValues rejected on protected DOs |
| REQ-872-MAP-001 | Wire capture | MMS PDUs match 8-1 mapping table |

---

## RAG routing

```
ccli-61850-7-2-extract, ccli-61850-6-extract, cei-0-16-allegato-t, cei-tr-57-126, ccli-62351-8-extract
```

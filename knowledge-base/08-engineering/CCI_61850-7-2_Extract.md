# IEC 61850-7-2 — CCLI Engineering Extract (ACSI / MMS)

**Document ID:** CCLI-PROTO-61850-7-2-001  
**Revision:** 1.1  
**Date:** 2026-10-12  
**RAG source_id:** `ccli-61850-7-2-extract` · `ccli-61850-7-2-ocr-corpus`  
**Normative basis:** IEC / BS EN **61850-7-2:2010** — *Abstract communication service interface (ACSI)*  
**PDF:** Programme `IEC 61850-7-2.pdf` · Ingest **BS EN 61850-7-2-2010** (Desktop, 222 PDF pages, OCR)  
**OCR corpus:** `reference/iec61850/_extract_61850-7-2/` — regen: `python scripts/extract-61850-7-2.py <path-to-pdf>`  
**Capture status:** Batches **A–F** (incl. **Annex A PICS**) per `ccli-61850-7-2-capture-plan`  
**Lab cross-refs:** `ccli-lab-pics-fill` · `ccli-lab-cid-pics-alignment` · `ccli-61850-10-extract` (Table 1 sDoc5)  
**Parents:** `ccli-61850-6-extract` · `cei-0-16-allegato-t` · `ccli-62351-8-extract` · `ccli-62351-4-extract`  
**Programme link:** K4.1 · REQ-61850-001 · `mms_adapter.cpp` · `lab_tg544_eth_a.cid`

---



## Summary

**61850-7-2** defines **ACSI** — the abstract application services for substation automation: **associations**, **directory** browsing, **read/write** of data, **DataSets**, **reporting** (BRCB/URCB), **control** (SBO/direct), **GOOSE/SV** control blocks, and **time** services. **61850-8-1** maps ACSI to **MMS** on TCP (RFC 1006 profile).

**CCLI:** Eth_A DSO path implements a **Server** role with **TR 57-126** data model; minimum ACSI surface is fixed by **Annex T §7** (privilege mapping). Transport and PDU mapping remain **8-1** + **62351-3/4**.

**Lab UCA:** The UCA Excel PICS (`lab/TemplatePics_…xlsx`) is a filled copy of **7-2 Annex A** (Tables A.1–A.3). **61850-10** tests what you mark **M** there; **61850-6** `<Services>` in CID must **correspond** (Table 1 **sDoc5**).

---



## Clause index (7-2:2010 — corpus PDF pages)

IEC **sheet** numbers appear in OCR footers (`— 188 —`). **PDF page** ≈ corpus `pNNN.txt`. Use purchased PDF for authoritative cites.


| Clause      | Topic                              | Corpus start (approx.)                             |
| ----------- | ---------------------------------- | -------------------------------------------------- |
| **5**       | ACSI overview, meta-model          | p009 (contents), p015+ body                        |
| **7**       | GenServerClass, directory services | contents p009                                      |
| **8**       | Application association (TPAA)     | contents p009 · **Associate / Release / Abort**    |
| **9–10**    | Logical device / logical node      | GetLogicalDeviceDirectory, GetLogicalNodeDirectory |
| **11–13**   | Data, DataSet                      | GetDataValues, SetDataValues, CreateDataSet        |
| **17**      | Reporting (URCB/BRCB)              | IntgPd, TrgOps, GI, Report                         |
| **18**      | GOOSE (GSE)                        | GSE control block                                  |
| **20**      | Control                            | Select, Operate, Cancel, ctlModel                  |
| **Annex A** | **PICS proforma**                  | **p193** (IEC sheet ~188)                          |


---



## Requirements


| REQ-ID                | Source      | Requirement                                                                        | CCLI                                    |
| --------------------- | ----------- | ---------------------------------------------------------------------------------- | --------------------------------------- |
| **REQ-872-SCOPE-001** | §1          | ACSI models client/server interactions for 61850 IEDs                              | MMS server only (Phase 3+)              |
| **REQ-872-ASSOC-001** | Association | Client establishes **application association** before services                     | libiec61850 association handler         |
| **REQ-872-ASSOC-002** | Association | **Release** / **Abort** tear down association cleanly                              | Session lifecycle                       |
| **REQ-872-DIR-001**   | Server      | **GetServerDirectory**, **GetLogicalDeviceDirectory**, **GetLogicalNodeDirectory** | Annex T Listobjects privilege           |
| **REQ-872-DATA-001**  | Data        | **GetDataValues**, **GetDataDirectory**, **GetDataDefinition**                     | Readvalues + Listobjects                |
| **REQ-872-DATA-002**  | Data        | **SetDataValues** only where Annex T grants Control/config                         | PF2 / setpoints via role-gated DOs      |
| **REQ-872-DS-001**    | DataSet     | **GetDataSetValues**, **GetDataSetDirectory**; no Create/Delete on TR CID          | Dataset privilege read-only             |
| **REQ-872-RPT-001**   | Reporting   | **Report** notification + **GetBRCBValues** / **GetURCBValues**                    | Reporting privilege; **intgPd** 4000 ms |
| **REQ-872-RPT-002**   | Reporting   | One RCB instance **≤ one client** (reservation rules)                              | Match TR `RptEnabled max=2`             |
| **REQ-872-CTL-001**   | Control     | **Select / SelectWithValue / Operate / Cancel** per **ctlModel**                   | ctlModel from 61850-6 templates         |
| **REQ-872-TIME-001**  | Time        | Time quality and sync visible to association peers                                 | NTP/NTS Annex T; ≤ ±100 ms              |
| **REQ-872-SEC-001**   | Cross-ref   | Privileges enforced per **62351-8** roles + Annex T Tables 97–102                  | DSO_OPERATOR / AGGREGATOR_OPERATOR      |
| **REQ-872-MAP-001**   | 61850-8-1   | MMS mapping of each implemented ACSI service                                       | **BLOCKED** until 8-1 PDF in corpus     |


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


| Topic                      | Normative intent                                              | CCLI note                                 |
| -------------------------- | ------------------------------------------------------------- | ----------------------------------------- |
| **Server vs Client**       | IED may expose **Server** AP; client associations for HMI/DSO | TR CID: single Server AP `accessPoint1`   |
| **Logical model**          | LD → LN → DO → DA tree                                        | `LD_Plant`, LN0 reports, prefixed LNs     |
| **Functional constraints** | FC (ST, MX, CF, DC, …) gate access                            | Map to 62351-8 rights                     |
| **Service tracking**       | Services may be **tracked** (time-stamped)                    | Aligned with 61850-5 Type 3 ~500 ms class |


---



## Batch B — Directory and read services

**Minimum Annex T (T.3.3.2) service classes → ACSI:**


| ACSI class                | Services                                                          | Annex T privilege                       |
| ------------------------- | ----------------------------------------------------------------- | --------------------------------------- |
| Server / Association / LD | GetServerDirectory, Release, Abort, GetLogicalDeviceDirectory     | Listobjects                             |
| Logical Node              | GetLogicalNodeDirectory, GetAllDataValues                         | Listobjects, Readvalues                 |
| Data Object               | GetDataValues, SetDataValues, GetDataDirectory, GetDataDefinition | Readvalues, Control/config, Listobjects |


**CCLI:** libiec61850 exposes these via ICD; restrict **Set** paths in server callback for non-DSO roles.

---



## Batch C — Reporting (BRCB / URCB)


| ACSI concept         | 61850-6 SCL                    | TR 57-126                    |
| -------------------- | ------------------------------ | ---------------------------- |
| **Integrity period** | `intgPd` ms on ReportControl   | **4000** on `urcb_*_Mis4sec` |
| **TrgOps**           | `period`, `dchg`, `qchg`, `gi` | period+gi on 4 s URCB        |
| **Buffering**        | `buffered=true` → BRCB         | `brcb_Stato_Allarmi_Segnali` |
| **Reservation**      | ClientLN + ResvTms             | DSO client slots `max=2`     |


**Services:** **Report** (unsolicited), **GetBRCBValues**, **GetURCBValues**; Annex T: **Set NOT applicable** on RCB for DSO read-only reporting profile.

---



## Batch D — Control model


| ctlModel (Enum)             | ACSI behaviour   | CCLI                              |
| --------------------------- | ---------------- | --------------------------------- |
| status-only                 | No Oper          | Status LNs                        |
| direct-with-normal-security | Operate          | Phase 2 lab                       |
| sbo-with-normal-security    | Select → Operate | Curtailment / setpoints if mapped |
| *-enhanced-security         | + 62351-4 auth   | Phase 3 production                |


**Annex T:** CONTROL privilege on DSO-reserved DOs only (Table 98).

---



## Batch E — GOOSE / SV (ACSI level)


| Item                   | ACSI                         | CCLI priority               |
| ---------------------- | ---------------------------- | --------------------------- |
| **GSE control blocks** | GOOSE enable/publish at ACSI | P1 plant bus                |
| **SV**                 | Sampled values streams       | Out of P0 CCI               |
| **Security**           | 62351-6 message auth on wire | After 61850-6 Batch J + 8-1 |


---



## Batch F — Annex A PICS (normative proforma)

**Corpus:** `_extract_61850-7-2/p193.txt` onward · **A.1 General** through **A.4 Table A.3**.

### F.1 Three conformance statements (A.1)


| Statement         | Table   | UCA Excel sheet (typical)                                                    |
| ----------------- | ------- | ---------------------------------------------------------------------------- |
| ACSI **basic**    | **A.1** | **ACSI Basic Conformance** (B11, B21, …)                                     |
| ACSI **models**   | **A.2** | **ACSI Model Conformance** (logical device/node, reporting options M7/M8, …) |
| ACSI **services** | **A.3** | **ACSI Service Conformance** (Associate, GetDataValues, URCB, Operate, …)    |


**Notation (Table A.1 footer, A.2/A.3):**


| Code          | Meaning                                                               |
| ------------- | --------------------------------------------------------------------- |
| **M**         | **Mandatory** — you claim support; **61850-10** runs applicable tests |
| **O**         | **Optional** — claimed optionally                                     |
| *(blank / —)* | Not offered — **no** tests for that row                               |


**Important:** The standard does **not** fix one global mandatory set for all IEDs. **You** choose **M/O/N/A** per device role. CCLI first-pass quote: [lab/conformance/PICS_FILL.md](../../../lab/conformance/PICS_FILL.md).

**NOTE 1 (Annex A):** PICS is **abstract**; details map via **SCSM** (for Eth_A: **61850-8-1** MMS + **62351-3/4** TLS).

**NOTE 2:** Model features (e.g. TrgOp **qchg**) can **imply** conformance rows when 7-3/7-4 CDCs require them — reconcile with CID `TrgOps` vs PICS (CCLI: **period+GI only** on `urcb_PdC_Mis4sec`).

### F.2 Table A.1 — Basic (→ CCLI first pass)


| ID          | Topic                   | CCLI PICS_FILL                            |
| ----------- | ----------------------- | ----------------------------------------- |
| **B11**     | Server side (TPAA)      | **M**                                     |
| **B12**     | Client side (TPAA)      | **N/A**                                   |
| **B21**     | SCSM 61850-8-1          | **M**                                     |
| **B22–B23** | 9-1 / 9-2               | **N/A**                                   |
| **B24**     | Other SCSM / subscriber | **N/A**                                   |
| **B31–B32** | GOOSE / GSSE publisher  | **N/A** (CID may still advertise — sDoc5) |
| **B41**     | TCP/IP profile          | **M**                                     |




### F.3 Table A.2 — Models (selected rows)

Declares **which models** exist (LD, LN, Data, DataSet, Report, Log, GOOSE, SV, …) and **optional report fields** (M7-x buffered, M8-x unbuffered: seqNum, timeStamp, reasonCode, datSet, dataRef, bufOvfl, entryID, confRev).


| CCLI                             | Fill guidance                                    |
| -------------------------------- | ------------------------------------------------ |
| **M8** unbuffered report control | **M** if URCB claimed                            |
| **M7** buffered                  | **N/A** first pass (even if `brcb_`* in lab CID) |
| **M11/M12** GOOSE / SV models    | **N/A** first pass                               |


Cross-check **OptFields** in CID (`entryID=false` on URCB) against chosen M8-x **O** rows.

### F.4 Table A.3 — Services (→ 61850-10 groups)

Structure (corpus p196+): grouped by **Server (Clause 7)**, **Application association (Clause 8)**, LD/LN, **DataSet (13)**, **Buffered / Unbuffered report (17)**, **Log**, **GSE**, **Control (20)**, **File**, **Time**, etc. Each row: service name + **M/O** columns per profile.


| ACSI area (7-2 clause)                 | CCLI PICS_FILL (first pass)      | 61850-10 (server)  |
| -------------------------------------- | -------------------------------- | ------------------ |
| Associate / Abort / Release            | **M**                            | sAss, Tables 4–5   |
| GetServerDirectory … GetDataDefinition | **M**                            | sSrv, Tables 6–7   |
| GetDataValues                          | **M**                            | sSrv               |
| SetDataValues                          | **O**                            | sSrv               |
| Get/Set DataSet (static)               | Get **M**, Create/Delete **N/A** | sDs, Tables 8–9    |
| Unbuffered reporting                   | **M**                            | sRp, **Table 14**  |
| Buffered reporting                     | **N/A**                          | sBr                |
| GOOSE                                  | **N/A**                          | sGop               |
| Control (Operate, …)                   | **M** (Wlim/WSd)                 | sCtl, **Table 26** |
| Time sync                              | **M** or **O**                   | sTm                |
| File transfer                          | **N/A**                          | sFt                |




### F.5 Annex A ↔ 61850-6 `<Services>` ↔ lab CID


| Annex A / PICS claim     | Typical CID `<Services>` (`lab_tg544_eth_a.cid`)                                |
| ------------------------ | ------------------------------------------------------------------------------- |
| Association **M**        | `<DynAssociation />`                                                            |
| Directory / read **M**   | `<GetDirectory />`, `<GetDataObjectDefinition />`, `<DataObjectDirectory />`    |
| DataSet read **M**       | `<GetDataSetValue />`, `<DataSetDirectory />`, `<ConfDataSet modify="false" />` |
| SetDataValues **O/M**    | `<ReadWrite />`                                                                 |
| URCB **M**, BRCB **N/A** | `<ConfReportControl bufMode="both" />` ⚠ sDoc5 — cert trim to `unbuffered`      |
| GOOSE **N/A**            | `<GOOSE max="5" />`, `GSEDir`, `GSESettings` ⚠ sDoc5                            |
| Logging **N/A**          | `<ConfLogControl max="1" />` ⚠                                                  |
| Files **N/A**            | `<FileHandling />` ⚠                                                            |
| Client **N/A**           | `<ClientServices …>` ⚠                                                          |
| Time **M/O**             | `<TimeSyncProt sntp="true" />`                                                  |


Detail matrix: [lab/conformance/CID_PICS_ALIGNMENT.md](../../../lab/conformance/CID_PICS_ALIGNMENT.md).

### F.6 Study order with PDF + repo

1. Read **Annex A** (corpus **p193–p210**, verify in licensed PDF).
2. For each **M** in `PICS_FILL.md`, read normative clause from **Table A.3** header (e.g. §8 association, §17 reporting, §20 control).
3. Map to CID **lines 55–77** and LLN0 RCBs.
4. Map tests via `ccli-61850-10-extract` §4.4.

---



## CCLI mapping


| ACSI area            | Code / config                                                |
| -------------------- | ------------------------------------------------------------ |
| Server               | `adapters/iec61850_mms/mms_adapter.cpp` → libiec61850        |
| Model                | `apps/ccli/config/icd/lab_tg544_eth_a.cid` (+ `.cfg` on DUT) |
| Privileges           | 62351-8 roles + Annex T Tables 97–102                        |
| TLS                  | 62351-3 `:3782`                                              |
| Application security | 62351-4 Annex G Cert B                                       |


---



## Verification


| Requirement     | Test                 | Pass criteria                           |
| --------------- | -------------------- | --------------------------------------- |
| REQ-872-DIR-001 | MMS client browse    | LD/LN list matches CID                  |
| REQ-872-RPT-001 | Subscribe URCB       | Periodic reports ≤ 4 s + integrity      |
| REQ-872-SEC-001 | Role without CONTROL | SetDataValues rejected on protected DOs |
| REQ-872-MAP-001 | Wire capture         | MMS PDUs match 8-1 mapping table        |


---



## RAG routing

```
ccli-61850-7-2-extract, ccli-61850-7-2-ocr-corpus, ccli-61850-6-extract, ccli-lab-pics-fill,
ccli-lab-cid-pics-alignment, ccli-61850-10-extract, cei-0-16-allegato-t, cei-tr-57-126, ccli-62351-8-extract
```


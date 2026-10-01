# IEC 61850-6 — SCL Engineering Extract (English)

**Document ID:** CCLI-PROTO-61850-6-001  
**Revision:** 2.0  
**Date:** 2026-09-16  
**RAG source_id:** `ccli-61850-6-extract`  
**Normative basis:** IEC 61850-6:2009+AMD1:2018+AMD2:2024 CSV  
**Capture plan:** `CCI_61850-6_Capture_Plan.md`  
**Companion:** `cei-tr-57-126` · `cei-tr-57-126-example.cid` · `CCI_Annex_T_Extract.md`  
**Programme link:** K4.1 · K4.3 · REQ-61850-001

---

## Summary

This extract covers **full CID grammar** — file types through Header, IED/Services, LD/LN, **DOI/DAI/SDI**, DataSet/ReportControl, Communication, and DataTypeTemplates (P0 batches **A–H + E COMPLETE**). It explains how the **CEI TR 57-126 CID** fits the IEC 61850-6 model and what to preserve when editing or loading with libiec61850.

**Corpus status:** **HAVE (P0+P1 SCL)** — capture batches **A–K COMPLETE**; **61850-8-1** Table 111 / P-types in **`ccli-61850-8-1-extract`** + **`ccli-61850-8-1-ocr-corpus`** (2004 Ed.1 OCR — verify at lab; target **2011 Ed.2**).

---

## Requirements (CCLI mapping)

| ID | Requirement | Source | CCLI implication |
|----|-------------|--------|----------------|
| REQ-61850-6-001 | CCI ships a **CID** (Configured IED Description), not a generic ICD template | §7 (p.34) | Runtime file = `cei-tr-57-126-example.cid` (project-specific names, IP, DataSets) |
| REQ-61850-6-002 | DataSets referenced by control blocks must reside in **same LN** as the control block | §6.6 (p.33) | All `DS_R_*` and `brcb_*`/`urcb_*` under **LLN0** in TR 57-126 |
| REQ-61850-6-003 | SCL root declares **version / revision / release** | §8.2.2 (p.40) | TR CID: `version="2007" revision="B" release="4"` |
| REQ-61850-6-004 | UTF-8 encoding in XML prolog | §8.4 (p.45) | First line: `<?xml version="1.0" encoding="UTF-8"?>` |
| REQ-61850-6-005 | Unknown elements without `mustUnderstand="true"` may be ignored | §8.2.1 (p.39) | Forward-compatible parsing in tooling |
| REQ-61850-6-006 | IED references use **IED name + LD inst**, not comm-level aliases | §8.4 (p.45) | FCDA `ldInst="LD_Plant"` consistent everywhere |
| REQ-61850-6-007 | Manufacturer extensions in **`<Private type="…">`** preserved on import/export | §8.3.6–8.3.8 (pp.43–44) | Optional CCLI `signal_map` metadata |
| REQ-61850-6-008 | Header **`uuid`** generated once and never changed across revisions | Table 3 (p.54) | TR 57-126 CID **lacks uuid** — add at commissioning if tool requires |
| REQ-61850-6-009 | History **`Hitem`** records version/revision/when/who/what/why | Table 4 (p.58) | TR 57-126 has one Hitem (RMS, 2022-08-05) |
| REQ-61850-6-010 | IED **name** unique, 1–64 chars, alphanumeric+`_`, not `None` | §9.3.2 (p.94) | `CCI016_01` valid |
| REQ-61850-6-011 | **`engRight="full"`** or subset on configured IED | Table 10 (p.93) | TR: `engRight="full"` |
| REQ-61850-6-012 | **`ClientServices`** declares buf/unbuf report + GOOSE caps | Table 11 (p.96) | TR: bufReport, unbufReport, goose, maxReports=10 |
| REQ-61850-6-013 | **`Authentication`** mandatory under Server; default `none` | Table 14 (p.108) | TR: empty `<Authentication />` → none |
| REQ-61850-6-014 | **`originalSclVersion/Revision`** track source ICD lineage | Table 10 (p.93) | TR: `2007` / `B` |
| REQ-61850-6-015 | **`ConfDataSet.modify="false"`** — preconfigured DataSets only | Table 11 (p.97) | TR: `max="10" maxAttributes="200" modify="false"` |
| REQ-61850-6-016 | **`ReportSettings` all Fix** — SCT cannot change RCB attrs online | Table 11 (p.98) | TR: cbName/datSet/rptID/optFields/bufTime/trgOps/intgPd all Fix |
| REQ-61850-6-017 | **`ConfReportControl`** caps static RCB count and buffer mode | Table 11 (p.97) | TR: `max="10" bufMode="both" bufConf="false"` |
| REQ-61850-6-018 | **`TimeSyncProt sntp="true"`** declares SNTP support | Table 11 (p.101) | TR: `sntp="true" other="true"` |
| REQ-61850-6-019 | Service **`max`** values are **guaranteed minimums**; 0 = disabled | Table 11 note (p.101) | TR GOOSE max=5, ConfReportControl max=10 |
| REQ-61850-6-020 | **IED-level Services** limited to `nameLength`, `ConfLNs`, `ConfLdName`, `ValueHandling`, `ClientServices/noIctBinding`, `MultiAPPerSubNet` | Table 50 (pp.104–106) | TR consolidates all `<Services>` on IED — common CID pattern; strict 2024 placement is under Server |
| REQ-61850-6-021 | **FCDA** requires `ldInst`, `lnClass`, `lnInst`, `doName`, `fc` (non-GSSE) | Table 22 (p.119) | All TR FCDAs include full path e.g. `ldInst=LD_Plant` |
| REQ-61850-6-022 | **Periodic DSO reports:** `TrgOps period=true` + **`intgPd` in ms** | Table 23 (p.121) | `urcb_*_Mis4sec`: `intgPd="4000"`, `period="true"` |
| REQ-61850-6-023 | **Buffered status report:** `buffered=true`, `dchg/qchg` triggers | Table 23 + TR CID | `brcb_Stato_Allarmi_Segnali`: `intgPd=0`, `bufTime=250` |
| REQ-61850-6-024 | **DataSet name** ≤32 chars (practical ≤20 with Services) | Restrictions (p.119) | `DS_R_PdC_Mis4sec` (17 chars) ✓ |
| REQ-61850-6-025 | **DataSet** members only from **same Server** | Rules (p.120) | All FCDA `ldInst=LD_Plant` on one Server |
| REQ-61850-6-026 | **`RptEnabled max`** = pre-instantiated RCB instances for DSO clients | Table 24 (p.123) | TR: `max="2"` on all four ReportControls |
| REQ-61850-6-027 | **`LDevice inst`** unique within IED; LD comm name ≤64 chars | Table 15 + restrictions (pp.109–110) | TR: single `LD_Plant` |
| REQ-61850-6-028 | **`LN0`:** `lnClass="LLN0"`, `inst=""` fixed; holds GSE/SV/SettingControl | Table 16 (p.111) | TR: `lnType="LLN01"` |
| REQ-61850-6-029 | **`LN`:** `prefix`+`lnClass`+`inst` unique within LD; `lnType` → LNodeType | Table 17 (p.112) | TR: e.g. `prefix="PdC" lnClass="MMXU" inst="1"` |
| REQ-61850-6-030 | **`SubNetwork/@type`** mandatory in SCD/CID for IP uniqueness check | Table 35 (p.145) | TR: `type="8-MMS"` |
| REQ-61850-6-031 | **`ConnectedAP`** unique by `iedName`+`apName` per SubNetwork | Tables 36, pp.144–146 | TR: `CCI016_01` / `accessPoint1` |
| REQ-61850-6-032 | Server **`Address`** P-types per **61850-8-1** (`IP`, `IP-SUBNET`, OSI-*) | §9.4.3 (pp.146–147) | TR: static Eth_A plan in `<Communication>` |
| REQ-61850-6-033 | **`LNodeType/@id`** referenced by `LN/@lnType`; `lnClass` consistent | Tables 40–41 (pp.156–157) | TR: `LLN01`, `MMXU1`, `DPCC1`, … |
| REQ-61850-6-034 | **`DA bType="Enum"`** references **`EnumType/@id`** via `type` attr | Tables 47, 49 (pp.164, 170) | TR: `ctlModel type="CtlModelKind"` |
| REQ-61850-6-035 | **`EnumVal/@ord`** required; label max 127 chars (Basic Latin) | Table 49 (p.170) | TR: validate `CtlModelKind` ord 0–4 at K4.1 |
| REQ-61850-6-036 | **One DOI per DO name** per LN; **`name`+`ix` unique** at each level | Tables 18–20 (pp.114–117) | TR: single `DOI name="Mod"` etc. |
| REQ-61850-6-037 | **DAI** holds instance **`<Val>`**; subset of template **DA** attrs | Table 19 (p.115) | TR: `stVal`, `ctlModel`, `lnNs`, `f` values |
| REQ-61850-6-038 | **SDI** nests structure; leaf values in **DAI** only | Table 20 (p.117) | TR: `WRtg/setMag/f` pattern |

---

## 1. Data flow modelling (§6.6, p.33)

- **Sources** = server/publisher LNs; **sinks** = client/subscriber LNs.
- Associations are at the **communication profile** (e.g. MMS/TCP), assignable to access point, LD, or LN.
- **GOOSE/SMV subscribers** modelled as whole IED; **report clients** as LN instances.
- Shared client channels: recommend **LLN0** as client LN (represents whole LD).
- Data flow at DO level via SCL wiring or **CDC ORG** in the IED model (61850-7-4).
- **Constraint:** DataSet definitions referenced by a control block must be in the **same logical node** as that control block → TR 57-126 places all report DataSets in **LN0**.

---

## 2. SCL file types (§7, pp.33–35)

| Extension | Name | Role | CCLI |
|-----------|------|------|------|
| **`.icd`** | IED Capability Description | IED type capabilities → system configurator; **one IED named TEMPLATE** | Product capability export (future) |
| **`.iid`** | Instantiated IED Description | Project-preconfigured single IED | Alternative to CID for some tools |
| **`.ssd`** | System Specification Description | Single-line diagram + required LNs; `iedName=None` if unallocated | Not CCI server scope |
| **`.scd`** | System Configuration Description | Full system: all IEDs, data flow, comm, substation | DSO-side aggregate |
| **`.cid`** | Configured IED Description | **Communication-related instantiated IED** for one project; includes **address** | **TR 57-126 Annex A** — what we deploy |
| **`.sed`** | System Exchange Description | Inter-project interface subset of SCD | DSO integration |

**Implementation rule (p.35):** Any IED implementing 61850 server/publisher or client/subscriber must provide **ICD or IID**; configurator generates/consumes **SCD** and produces **CID** for the device.

**Fig 8 (p.35):** One **IED class** may have several **ICD implementable subsets** (different function combinations that can run simultaneously on hardware).

---

## 3. SCL specification method (§8.1, p.36)

- SCL is **XML** with W3C schema types.
- **Naming:** schema types `t*`; attribute groups `ag*`; attributes lowercase; elements capitalized.
- Inheritance from **`tBaseElement`** → allows `Private` and `Text`.
- **`tUnNaming`:** optional `desc`.
- **`tNaming`:** mandatory **`name`** + optional `desc`.
- **`tIDNaming`:** mandatory **`id`** (no whitespace, max 255 chars) + optional `desc`.

---

## 4. Schema structure (Table 1, Fig 9, p.37)

| XSD file | Content |
|----------|---------|
| `SCL.xsd` | Root `<SCL>` element |
| `SCL_IED.xsd` | IED syntax |
| `SCL_Communication.xsd` | Communication |
| `SCL_DataTypeTemplates.xsd` | LNodeType, DOType, DAType, EnumType |
| `SCL_Substation.xsd` | Process/substation (skip for CCI CID) |
| `SCL_BaseTypes.xsd`, `SCL_Enums.xsd`, … | Shared types |

**Fig 9:** Root `SCL` contains exactly one `Header`, zero or more `IED`, optional `Communication`, optional `DataTypeTemplates`, optional substation/process sections.

---

## 5. Language versions and compatibility (§8.2, pp.38–41)

### 5.1 Evolution rules (§8.2, p.38)

- New features → new **version** (2007 edition).
- Error fixes → **revision** letter (A, B, C…).
- Interop fixes between revisions → **release** number.
- **Forbidden:** removing features or changing semantics of existing features (deprecate + add new instead).

### 5.2 MustUnderstand (§8.2.1, pp.39–40)

- Interoperability-critical elements: **`mustUnderstand="true"`**.
- Unknown element **without** mustUnderstand → tool may ignore element and children.
- Unknown element **with** mustUnderstand inside known parent → ignore entire parent.
- Unknown **attributes** always ignored (use optional attrs with defaults for compatibility).
- Example: `<Protocol mustUnderstand="true">R-GOOSE</Protocol>` inside `GSEControl` — tool that does not understand `Protocol` must not use that GOOSE control block.

### 5.2 Namespace and version attributes (§8.2.2, p.40)

- Namespace URI: `http://www.iec.ch/61850/2003/SCL` (see §8.3.5).
- **`version`** on `<SCL>`: **2007** for this edition (default 2003 for backward compat declaration).
- **`revision`**: e.g. **B** (corrigendum index).
- **`release`**: e.g. **3** or **4** per amendment level.
- Tools must accept older/newer instances; stop with error if unknown **`mustUnderstand="true"`** element encountered.
- Upgrade/downgrade rules: **Annex I** (not yet captured).

### 5.3 Incompatibilities 2003 → 2007 (§8.2.3, p.41) — TR 57-126 relevance

| Change | Impact on CID editing |
|--------|----------------------|
| `authentication` `'week'` → **`weak`** | Check Server auth if used |
| **`Private/@type` required** | Any Private sections need `type` |
| **`nameStructure=FuncName` deprecated** | TR uses `nameStructure="IEDName"` — correct |
| **AccessPoint name** alphanumeric + `_`, max 32 | `accessPoint1` OK |
| **`ldName` on LDevice** | Ed1 tools may ignore — risk if DSO uses Ed1 |
| **`ReportControl.rptID` non-empty** | TR sets full rptID strings — OK |
| **`ConfDataSet.maxAttributes` = FCDA count** | DataSet sizing |
| **`SubNetwork/@type` mandatory** | TR: `type="8-MMS"` |
| **`RptEnabled.max` > 0 if present** | TR: `max="2"` |

---

## 6. Extensions and Private data (§8.3, pp.42–44)

- **§8.3.5:** Default namespace must be SCL; private namespaces should prefix with **`e`**; unknown namespaces ignored on import.
- **§8.3.6 Private:** `<Private type="required" source="optional URL">` — tool-specific data; **non-owner tools must preserve** on import/export.
- **Table 2:** `type` = manufacturer/tool unique string; `source` = external file URL (preserve URL only).
- **Rule of thumb:** external `source` if private payload **> 1–2 kB**.
- **§8.3.8 conformance:** Tools must preserve Private sections and Text; ignore unknown SCL elements per mustUnderstand rules.

**CCLI use:** optional embedding of `signal_map.yaml` digest or HW binding under `<Private type="ccli:signal-map">`.

---

## 7. General document structure (§8.4, p.45)

Required prolog:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<SCL xmlns="http://www.iec.ch/61850/2003/SCL"
     xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
     xsi:schemaLocation="http://www.iec.ch/61850/2003/SCL SCL.xsd"
     version="2007" revision="…" release="…">
```

**Mandatory:** exactly one **`Header`**.  
**At least one of:** Process/Substation, Communication, IED, DataTypeTemplates.

**TR 57-126 CID contains:** Header, Communication, IED (with Server/LDevice/LN0/LN), DataTypeTemplates — **no Substation section** (correct for single-IED CID).

**Referencing rule:** All references to IED objects use **IED name + LD instance** (e.g. `ldInst="LD_Plant"`), including FCDA members of DataSets.

---

## 8. Header and history (§9, pp.54–58, Batch B)

### 8.1 Table 3 — Header attributes (p.54)

| Attribute | Description | TR 57-126 CID |
|-----------|-------------|---------------|
| **id** | File identifier string (mandatory, may be empty) | `CCI_Oss_Contr` |
| **version** | Project version of this SCL file | `1` |
| **revision** | Project revision (default empty) | `1` |
| **toolID** | Tool that created the file | `""` (empty) |
| **nameStructure** | Back-compat; if used, only **`IEDName`** allowed | `IEDName` |
| **fileType** | Optional: ICD, IID, CID, SSD, SCD, SED | *(not set — infer CID from content)* |
| **uuid** | Mandatory unique ID at first creation; **unchanged** across version/revision updates | **MISSING in TR example** |
| **baseUuid** | UUID of source file when derived (e.g. SED from SCD) | — |

**UUID rules:** One `uuid` per file lineage; never reuse across different files; version bumps keep same `uuid`.

### 8.2 Source file references (pp.55–56)

**Table 53 — `tSclFileUUIDReference` attributes:**

| Attribute | Notes |
|-----------|-------|
| fileUuid | Referenced file UUID (optional if fileName present) |
| fileName | Import filename (optional if fileUuid present) |
| **fileType** | **Mandatory** — ICD/IID/CID/SSD/SCD/SED |
| version, revision | Referenced file version/revision |
| when | Import date (optional) |

At least one of **fileUuid** or **fileName** required. SSD/SCD Header may list **SourceFiles** / **SclFileReference** chain — order = project history.

**Fig 25–26 (p.56):** Header-level and IED-level reference graph (IID→SCD/ICD/SSD; SCD→SSD/SED). **Proxy gateway note:** IID may hold multiple SCD references (e.g. LAN client + WAN server) — relevant if CCI later splits Eth_A/Eth_B roles in separate SCL imports.

### 8.3 History — Table 4, Hitem (pp.57–58)

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **version** | History entry version | `1` |
| **revision** | History entry revision | `1` |
| **when** | Release date/time | `05/08/2022 12:00:00` |
| **who** | Author/approver | `RMS` |
| **what** | Change summary | `Esempio CID CCI Osservab. + Controllab.` |
| **why** | Reason | `Per TR 57-126 relativo a Allegato T CEI 0-16` |

SCD history is **independent** of IID/ICD history it was built from. Optional `<SourceFiles>` under each Hitem.

**Example Header with SourceFiles (p.58 — SSD/SCD pattern, not in TR CID):**

```xml
<Header id="SCL Example T1-1" toolID="MySystemTool"
        uuid="6dacf413-cb9e-4ce8-8a45-7c6325b1afaa" version="0" revision="1">
  <SourceFiles>
    <SclFileReference fileType="SSD" fileUuid="cef3a885-67d8-44d4-a739-71ebb6f62ad7"
                      fileName="SCLExample.ssd" version="1" revision="0"/>
  </SourceFiles>
</Header>
```

**§9.2 Process description (p.58+):** Substation/primary-equipment modelling — **SKIP for CCI CID** (TR 57-126 has no Process section).

---

## 9. IED shell (§9.3, pp.87–88, 93–104, 108 — Batch C partial)

### 9.0 Fig 18 — IED structure UML (p.87–88)

**Hierarchy:** `tIED` → `Services`(0..1) · `KDC`(0..*) · `IEDSourceFiles`(0..1) · `Labels`(0..1) · **`AccessPoint`(1..*)**

| Level | Type | Key content |
|-------|------|-------------|
| **IED** | `tIED` | attrs: `name`, `type`, `manufacturer`, `configVersion`, `originalScl*`, `engRight` (default full), `owner`, uuid |
| **AccessPoint** | `tAccessPoint` | **exactly one of:** `Server`, `LN` list (pure client), `ServerAt`, or nothing; optional `Services`, `GOOSESecurity`/`SMVSecurity` (0..7 certs each) |
| **Server** | `tServer` | `timeout` (default 30 s); **`Authentication`(1)** mandatory; `LDevice`(1..*); `Association`(0..*); `AccessControl`(0..1) |
| **LDevice** | `tLDevice` | `inst`, `ldName`; **`LN0`(1)** + `LN`(0..*) |
| **Security** | `Authentication` | `none` (default true), `password`, `weak`, `strong`, `certificate` |
| **Refs** | `tKDC`, `tIEDSclRef` | KDC: `iedName`, `apName`, `apUuid`; source file chain |

**TR 57-126 path:** `IED` → `AccessPoint accessPoint1` → `Server` → `Authentication` + `LDevice inst=LD_Plant` → `LN0` + LNs.

**Fig 19:** not captured separately — content likely merged with Fig 18 in this edition; **SKIP** unless found on review.

### 9.1 Table 10 — IED attributes (p.93, complete)

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **name** | IED id; unique in SCL; ICD template must be **`TEMPLATE`** | `CCI016_01` (CID) |
| **desc** | Description text | *(not set)* |
| **type** | Manufacturer product type | *(not set)* |
| **manufacturer** | Manufacturer name | *(not set)* |
| **configVersion** | **Type** capability version (not project config) | via **LLN0.NamPlt.configRev** |
| **originalSclVersion** | ICD SCL version | `2007` |
| **originalSclRevision** | ICD SCL revision | `B` |
| **originalSclRelease** | ICD SCL release (optional, default 1) | *(not set)* |
| **engRight** | Engineering rights: `full`, `dataflow`, `fix` | **`full`** |
| **owner** | Owning SCD Header id | default = `CCI_Oss_Contr` |
| **uuid** / **templateUuid** | IED instance / template IDs | optional — not in TR |

**Config tracking:** `configVersion` (Table 10) ≠ project config — runtime config revision is **`LLN0.NamPlt.configRev`** in a DAI/Val. Missing `originalScl*` on legacy files → upgrade per **Annex I §I.4.3.3**.

**IEDSourceFiles:** Chronological list of ICD/IID/SSD/SCD files used to engineer this IED. **MinRequestedSCDFiles:** latest SCD required for this project version (multi-SCD for gateway IEDs).

**KDC:** Optional Key Distribution Center ref (`iedName`, `apName`, `apUuid`) per **IEC 62351-9** — Phase 3 TLS/PKI.

### 9.2 IED naming and structure rules (p.94)

- **IED name:** unique in SCL; **1–64** chars; starts with alpha; alphanumeric + `_` only; **`None` and empty forbidden**.
- **ICD template name:** must be **`TEMPLATE`** (TR uses real name `CCI016_01` — correct for CID).
- **AccessPoint:** no duplicate `name` within IED.
- **LDevice:** no duplicate **`inst`** within IED (key for all FCDA `ldInst` refs).

**`<Services>` element (tServices):** Declares MMS/report/GOOSE/log capabilities — see Table 11. TR enables ConfReportControl, GOOSE, ClientServices, TimeSyncProt, etc.

### 9.3 Table 11 — Services (pp.96–101, complete)

**Setting semantics (p.101):** attributes marked **Dyn** = online via 61850 services; **Conf** = SCL engineering; **Fix** = single fixed value in SCL. If `datSet` is **Fix**, DataSet structure is also fixed — `ConfReportControl`/`ConfLogControl`/`GOOSE`/`SMVsc` `max` ignored for backward compat.

**Max rule (p.101):** all `max` values are **guaranteed minimums** the IED must support; **0** = element cannot be used/created.

| Service | Key attributes / meaning | TR 57-126 |
|---------|--------------------------|-----------|
| **ClientServices** | Client/subscriber: `goose`, `gsse`, `sv`, `unbufReport`, `bufReport`, `readLog`; `supportsLdName`, `maxAttributes`, `maxReports`, `maxGOOSE`, `maxSMV`, `rGOOSE`, `rSV`, `noIctBinding`, `acceptServerInitiatedAssociation`; sub: `TimeSyncProt`, `GOOSEMcSecurity`, `SVMcSecurity`, `Security` | `bufReport unbufReport goose readLog supportsLdName maxAttributes=200 maxReports=10 maxGOOSE=5` |
| **DynAssociation** | Dynamic associations; `max` = guaranteed simultaneous count | present (empty) |
| **SettingGroups** / **SGEdit** / **ConfSG** | SGCB; `SelectActiveSG`; online edit (`SelectEditSG`, `ConfirmEditSGValues`, `SetSGValues`); SCD group count edit | *(not in TR)* |
| **GetDirectory** | LD/LN/DATA directory (GetServerDirectory, GetLogicalDeviceDirectory, GetLogicalNodeDirectory) | present |
| **GetDataObjectDefinition** | Full DA definition list for referenced LN data | present |
| **DataObjectDirectory** | DATA defined in LN (GetDataDirectory) | present |
| **GetDataSetValue** / **SetDataSetValue** | Read/write all DataSet member values | GetDataSetValue only |
| **DataSetDirectory** | FCD/FCDA of DataSet members | present |
| **ConfDataSet** | Static DataSet creation via SCL; `max`, `maxAttributes`, `modify` (default true) | `max=10 maxAttributes=200 modify=false` |
| **DynDataSet** | CreateDataSet/DeleteDataSet; `max`, `maxAttributes` | *(not in TR)* |
| **ReadWrite** | GetData, SetData, Operate | present |
| **TimerActivatedControl** | Timer-activated control services | present |
| **ConfReportControl** | Static RCB creation; `max`, `bufMode` (unbuffered/buffered/both), `bufConf`, `maxBuf` | `max=10 bufMode=both bufConf=false` |
| **ConfLogControl** | Static LCB creation; `max` | `max=1` |
| **ReportSettings** | RCB attrs changeable at engineering/online: `cbName`, `datSet`, `rptID`, `optFields`, `bufTime`, `trgOps`, `intgPd`, `resvTms`, `owner` — each Fix/Conf/Dyn | **all Fix**; `resvTms=false owner=false` |
| **LogSettings** | LCB attrs: `cbName`, `datSet`, `logEna`, `trgOps`, `intgPd` | *(not in TR)* |
| **GSESettings** | GOOSE CB attrs: `cbName`, `datSet`, `appID`, `dataLabel`, `kdaParticipant`; optional `McSecurity` | **all Fix** |
| **SMVSettings** | SV CB attrs: `cbName`, `datSet`, `svID`, `optFields`, `smpRate`, `samplesPerSec`, `synchSrcId`, `nofASDU`, `pdcTimeStamp`, `kdaParticipant` | *(not in TR — no SV publisher)* |
| **ConfLNs** | LN prefix/instance fixability: `fixPrefix`, `fixLnInst` (default false) | `fixPrefix=true fixLnInst=true` |
| **ConfLdName** | SCT may set functional `LDevice ldName` | present (empty) |
| **GSEDir** | GSE directory services (61850-7-2) | present |
| **GOOSE** | GOOSE publisher; `max`, `fixedOffs`, `goose` (L2, default true), `rGOOSE` (L3) | `max=5` |
| **GSSE** | GSSE publisher/subscriber; `max` (0 = client only) — **deprecated Ed2** | *(not in TR)* |
| **SMVsc** | SV publisher; `max`, `delivery`, `deliveryConf`, `sv`, `rSV` | *(not in TR)* |
| **FileHandling** | File services beyond Get*; `mms` (default true), `ftp`, `ftps` | present (empty → mms default) |
| **SupSubscription** | GOOSE/SV subscription supervision LNs; `maxGo` (LGOS), `maxSv` (LSVS) | `maxGo=10 maxSv=0` |
| **ConfSigRef** | Input refs (InRef/BlkRef ORG); `max` — **deprecated Ed2.2** | *(not in TR)* |
| **SCSM** | SCSM support: `iec61850_8_1` (MMS, default true), `iec61850_8_2` (XMPP), `serverAssociationInitiation` | implicit MMS |
| **CommProt** | Lower-layer: `ipv6` (IPv4 mandatory if IP used) | *(not in TR)* |
| **TimeSyncProt** | `sntp` (default true), `c37_238` (deprecated), `iec61850_9_3`, `other` | `sntp=true other=true` |
| **RedProt** | `hsr`, `prp`, `rstp` | *(not in TR)* |
| **ValueHandling** | `setToRo` — allow CF/DC/SP Set→RO | *(not in TR)* |
| **McSecurity** | Multicast security at server AP: `signature`, `encryption` | Phase 3 + 62351-6 |
| **Security** | TPAA security (62351-4): `ACSEAuthentication`, `E2ESecurity` | Phase 3 TLS |
| **MultiAPPerSubNet** | Multiple APs on same SubNetwork (mandatory for clients Ed2.2+) | *(not in TR)* |

**CCLI note:** TR `ReportSettings` all **Fix** means DSO cannot change `intgPd`, `trgOps`, etc. via SCT — 4 s periodic report (`intgPd="4000"`) is baked into CID (Batch F, Table 23).

### 9.3.1 tAccessPoint schema (p.102)

Access point defines communication endpoints. Choice of **Server**, **LN** list (client-only AP), or **ServerAt**. Optional: **Services** (adds to IED caps), **GOOSESecurity** / **SMVSecurity** (max 7 certs each, only if `Authentication certificate=true`), **Labels**. Attributes: **`name`** (required), `router`, `clock`, `kdc`, uuid group. **uniqueAssociationInServer** on Server: `@associationID` unique.

### 9.4 Table 12 — AccessPoint (p.103)

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **name** | AP id within IED | `accessPoint1` |
| **desc** | Description | optional |
| **router** | Router function (default false) | — |
| **clock** | Master clock on bus (default false) | — |
| **uuid** / **templateUuid** | AP instance IDs | optional |

**SA reference** = IED name + access point name. Services at AP level add to IED-level caps; **`max` on GOOSE/SMV/ConfReportControl at AP** means **capable of sending** (0 = disabled on this AP). See **Table 50** (§9.6).

### 9.6 Table 50 — Service placement IED vs Server (pp.104–106, complete)

Columns: **IED** (y/n) · **Server + ServerAt** (identical / may be different / identical or 0).

**IED-level allowed (`y`):** only these may appear on `<IED><Services>`:

| Service / attribute | IED | Server + ServerAt |
|---------------------|-----|-------------------|
| **nameLength** | y | — |
| **ConfLNs** / fixPrefix, fixLnInst | y | — |
| **ConfLdName** | y | — |
| **ValueHandling** / setToRO | y | — |
| **ClientServices** / noIctBinding | y | — |
| **MultiAPPerSubNet** | y | — |

**All other Table 11 elements:** **IED = n** — normatively under **Server** or **ServerAt**.

**Server vs ServerAt rules (summary):**

| Category | Server + ServerAt |
|----------|-------------------|
| **ConfDataSet**, **ReportSettings**, **LogSettings**, **GSESettings** (cbName/datSet/appID/…), **GetDirectory**, **GSEDir**, **SupSubscription**, **ConfSigRef** | **identical** |
| **ConfReportControl/max**, **GOOSE/max**, **SMVsc/max** | **identical or 0** |
| **DynAssociation**, **DynDataSet**, **ReadWrite**, **FileHandling**, **ClientServices** (most attrs), **TimeSyncProt**, **Security**, **McSecurity**, **RedProt**, **SCSM**, **CommProt** | **may differ** |
| **GSESettings/SMVSettings** kdaParticipant, McSecurity | **may differ** |
| **GOOSE** fixedOffs, goose, rGOOSE; **SMVsc** delivery/sv/rSV | **may differ** |

**p.106 note:** project-specific AP addressing (IP, OSI selectors) belongs in **`<Communication>`**, not in IED Services — maps to TR `ConnectedAP` / Batch G.

**TR 57-126 pattern:** entire `<Services>` block under `<IED>` (no Server-level duplicate). Widely used for single-AP server CIDs; strict Table 50 would relocate capability attrs under `<Server>`. CCLI preserves TR layout for DSO tool compatibility.

### 9.5 Tables 13–14 — Server and Authentication (p.108)

| Element | Attribute | Description | TR 57-126 |
|---------|-----------|-------------|-----------|
| **Server** | **timeout** | Incomplete transaction timeout (s) | default |
| **Server** | desc | Text | `CCI CEI 0-16 Oss. e Contr.` |
| **Authentication** | none | Default **true** if all omitted | `<Authentication />` → **none** |
| **Authentication** | password, weak, strong, certificate | Defined in SCSM (62351-4 for MMS) | all default false |

**Security note:** `GOOSESecurity` / `SMVSecurity` on access point **only allowed if** `certificate="true"` on Authentication — relevant for Phase 3 GOOSE + 62351-6.

**Server children:** `Authentication`, `LDevice`(s), optional `Association`.

---

## 10. LN structure (Fig 21, p.90 — Batch D)

**Fig 21 — class LN and LN0:**

| Type | Role |
|------|------|
| **tAnyLN** | Base: `lnType`, `templateUuid`, `uuid` |
| **tLN0** | `lnClass="LLN0"`, `inst=""` (fixed) — exclusive: **SettingControl**, **GSEControl**, **SampledValueControl** |
| **tLN** | `inst`, `lnClass`, `prefix` |
| **Data / control (any LN)** | `DataSet`(0..*)→`FCDA`(1..*); `ReportControl`(0..*); `LogControl`(0..*); `Log`(0..*) |
| **Wiring** | `Inputs`(0..1)→`ExtRef`(1..*); `Outputs`(0..1)→`ExtCtrl`(1..*) |
| **Instance values** | `DOI`(0..*)→`SDI`/`DAI`(0..*)→`Val`; attrs: `sAddr`, `valKind`, `valImport` |
| **Labels** | `Labels`(0..1)→`Label`(`id`, `lang`) |

**tFCDA attrs:** `ldInst`, `prefix`, `lnClass`, `lnInst`, `doName`, `daName`, `fc`, `ix`.

**tExtRef attrs (subset):** `iedName`, `ldInst`, `lnClass`, `lnInst`, `prefix`, `doName`, `daName`, `serviceType`, `srcCBName`, `srcLDInst`, `intAddr`, …

**CCLI:** all TR 57-126 DataSets and report control blocks live under **LN0** per §6.6 same-LN rule. Element attribute tables: **§13**; instance values: **§11**.

---

## 10.1 Control blocks (Fig 20, p.89 — Batch F)

**Fig 20 — class Control Blocks** inheritance:

```
tUnNaming
├── tSettingControl (actSG, numOfSGs, resvTms) — LN0 only
├── tRptEnabled (max, default 1)
└── tControl (name, datSet, uuid)
    ├── tControlWithTriggerOpt (+ intgPd, TrgOps)
    │   ├── tReportControl (+ buffered, bufTime, confRev, indexed, rptID, OptFields, ClientLN*)
    │   └── tLogControl (+ logEna, logName, ldInst, lnClass, reasonCode, …)
    └── tControlWithIEDName (+ confRev, IEDName*)
        ├── tGSEControl (+ appID, type GOOSE/GSSE, fixedOffs, securityEnable, Protocol)
        └── tSampledValueControl (+ smvID, smpRate, smpMod, nofASDU, multicast, SmvOpts, Protocol)
```

**tTrgOps:** `dchg`, `qchg`, `dupd`, `period`, `gi`.  
**OptFields:** `seqNum`, `timeStamp`, `dataSet`, `reasonCode`, `dataRef`, `bufOvfl`, `entryID`, `configRef`.  
**tProtocol:** `mustUnderstand="true"` (readOnly default) — unknown protocol → tool must not use CB.

**TR 57-126 maps:** see **§12** Tables 21–24.

---

## 11. Instance values — DOI / DAI / SDI (§9.3.6, pp.114–117 — Batch E)

**Purpose:** override or set **project-specific instance values** in the IED section; type definitions remain in **DataTypeTemplates**. Only attrs needing values or overrides appear in **DAI**/**SDI**.

### 11.1 Table 18 — DOI (p.114)

**Structure:** `DOI` contains **`SDI`** and/or **`DAI`** children (not both as sole content at same level — tree of SDI down to DAI leaves).

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **name** | DO root name from **LNodeType**; **unique** per LN (max one DOI per DO) | `Mod`, `Beh`, `WRtg`, … |
| **desc** | Description | optional |
| **ix** | Array index if DO is array type | rare in TR |
| **accessControl** | SCSM-specific; empty = inherit higher level | optional |

### 11.2 Table 19 — DAI (p.115)

**Schema `tDAI`:** extends `tUnNaming`; **`Val`** (0..*) + optional **`Labels`**.

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **name** | Terminal attribute name (leaf of structure path) | `stVal`, `ctlModel`, `f`, `lnNs` |
| **desc** | Description | e.g. WRtg setpoint text |
| **sAddr** | Short address at instance level | optional |
| **valKind** | Overrides type-definition valKind (Table 46) | usually inherited |
| **ix** | Array index for array DAI | with SDI `ix` for curves |
| **valImport** | Allow SCD import even if RO/Conf | default false |

**Usage:** engineering-time values for IED startup, simulation, or cross-IED visibility; **`Val`** content follows Table 45 encoding (Enum = enum **name** string).

**Uniqueness:** **`name` + `ix`** unique at each hierarchy level.

### 11.3 Table 20 — SDI (pp.116–117)

**Schema `tSDI`:** recursive — contains **`SDI`** and/or **`DAI`**.

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **name** | Sub-structure name (SDO in LNodeType or nested DA path) | `setMag` |
| **desc** | Description | optional |
| **ix** | Array element index | curve points `crvPts ix="0..2"` in example |
| **sAddr** | Whole-structure short address — only if no lower **sAddr** | optional |

**Restrictions (p.117):**

- Names start with **lowercase** (except `SIUnit`, 61850-8-1 exceptions).
- **No dots** in names; alphanumeric only.
- **`name` + `ix`** unique per level.

**Examples (standard):**

- Structured DO: `DOI Volts` → `SDI sVC` → `DAI offset`, `DAI scaleFactor`.
- Array DO: `DOI TmAst` → multiple `SDI crvPts ix="n"` → `DAI xVal`, `DAI yVal`.

### 11.4 TR 57-126 instance patterns

| Pattern | Example in CID | CCLI use |
|---------|----------------|----------|
| **LN0 Mod/Beh/Health** | `<DAI name="stVal"><Val>on</Val>` | default operational state |
| **ctlModel instance** | `<Val>status-only</Val>` under Mod | matches EnumType ord 0 |
| **Namespace** | `ldNs`, `lnNs` → 61850-7-4 / 7-420 URIs | profile declaration |
| **LPHD PhyNam** | vendor, swRev, location | commissioning identity |
| **Setpoint via SDI** | `WRtg/setMag/f` = 200 | plant characteristic kW |

**CCLI mapping:** runtime signal updates write to **61850 server model** (libiec61850); SCL **DAI/Val** = engineered defaults at CID load — align with `signal_map.yaml` for DI/DO→DOI paths (future).

---

## 12. DataSet and ReportControl (§9.3.7–9.3.8, pp.118–123 — Batch F)

### 12.1 Table 21 — DataSet attributes (p.118)

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **name** | DataSet id **within hosting LN** | `DS_R_PdC_Mis4sec`, `DS_R_Stato_Allarmi_Segnali`, … |
| **desc** | Description | present on all four |
| **uuid** / **templateUuid** | Instance / template IDs | optional — not in TR |

**Schema:** `tDataSet` extends `tUnNaming`; contains unbounded **`FCDA`** elements.

### 12.2 Table 22 — FCDA attributes (p.119)

| Attribute | Rule | TR 57-126 |
|-----------|------|-----------|
| **ldInst** | Mandatory (except GSSE) | always `LD_Plant` |
| **prefix** | Optional; default `""` | e.g. `prefix="PdC"`, `DisFR` |
| **lnClass** | Mandatory (except GSSE) | `MMXU`, `DECP`, `XCBR`, … |
| **lnInst** | Mandatory except **LLN0** | `1`, etc. |
| **doName** | Mandatory; structured DO with `.`; arrays as `(n)` | `TotW`, `Pos`, `Beh` |
| **daName** | If omitted → all DAs matching **fc** | often omitted |
| **fc** | Must match CDC / LNodeType definition | `MX`, `ST`, … |
| **ix** | Array index; must match `(n)` in doName/daName | e.g. `ix="3"` |
| **lnUuid** | LN reference UUID | optional |

**Restrictions (p.119–120):**

- Invalid **fc** for named **daName** → SCL processing **must error**.
- Message column order = **FCDA order** in DataSet; multi-DA FCDA follows DOType/DAType attribute order.
- FCDA shall not reference data not visible online (`valKind` Conf/Spec only).
- DataSet shall contain data **only from its own Server** (not another Server on same IED).
- DataSet name syntactic max **32** chars; practical max often **20** (interaction with 64-char Services `nameLength`).

### 12.3 Table 23 — ReportControl attributes (pp.121–122)

RCB contains **`TrgOps`**, **`OptFields`**, **`RptEnabled`**. Extends `tControlWithTriggerOpt` → `tControl` (`name`, `datSet`, uuid).

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **name** | RCB name; unique within LN | `urcb_PdC_Mis4sec`, `brcb_Stato_Allarmi_Segnali` |
| **desc** | Description | present |
| **datSet** | DataSet in **same LN**; may be absent in ICD/unused CB | matches `DS_R_*` |
| **intgPd** | Integrity period **ms** (61850-7-2); used when **period** trigger true | **`4000`** on `urcb_*_Mis4sec`; **`0`** on brcb |
| **rptID** | Report ID; NULL if unused (7-2) | full path strings |
| **confRev** | Config revision; **0** only without datSet; increment ~10000 on SCL change | **`1`** on all |
| **buffered** | Buffered (BRCB) vs unbuffered (URCB); default false | `true` on brcb; omitted/false on urcb |
| **bufTime** | Buffer time ms; default 0 | **`250`** on brcb; **`0`** on urcb |
| **indexed** | Append 2-digit index to instance names; default **true** | default (no ClientLN list) |

**TrgOps (p.122):** `dchg`, `qchg`, `dupd`, `period`, `gi`.

| Report | TrgOps | Purpose |
|--------|--------|---------|
| `brcb_Stato_Allarmi_Segnali` | `dchg qchg gi` | Event-driven status/alarms |
| `urcb_*_Mis4sec` | **`period gi`** | **4 s** periodic measurements + GI |

**OptFields (p.123):** `seqNum`, `timeStamp`, `dataSet`, `reasonCode`, `dataRef`, `entryID`, `configRef`, `bufOvfl` (default **true**). `segmentation` **deprecated** — accept on import, ignore.

### 12.4 Table 24 — RptEnabled (pp.122–123)

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **desc** | Description | `Client DSO` |
| **max** | Max RCB **instances** of this type pre-instantiated at config time; default **1**; must be **>0** | **`2`** (two DSO client slots) |

**RCB instance rules (p.122):**

- One RCB dedicated to **at most one client** (61850-7-2).
- If **`max > 1`** → multiple RCB instances created in IED.
- **`indexed=true`:** instance names = `RCName` + 2-digit index (`01`…`max`); index matches **ClientLN** position when preconfigured.
- **`indexed=false`:** single instance name = `RCName`; buffered allows only **`max=1`**.
- Buffered permanent reservation: preconfigure **ClientLN**, set **ResvTms=-1**; else **ResvTms=0**. URCB with preconfigured ClientLN → **Resv=true**.

**tClientLN (p.122):** `apRef` (required) + `agLNRef` (`iedName`, `ldInst`, `prefix`, `lnClass`, `lnInst`, `lnUuid`). Table 25 — Batch I (P1).

### 12.5 TR 57-126 report inventory

| DataSet | ReportControl | Type | intgPd | TrgOps | Notes |
|---------|---------------|------|--------|--------|-------|
| `DS_R_Stato_Allarmi_Segnali` | `brcb_Stato_Allarmi_Segnali` | BRCB | 0 | dchg, qchg, gi | `bufTime=250`, bufOvfl in OptFields |
| `DS_R_PdC_Mis4sec` | `urcb_PdC_Mis4sec` | URCB | **4000** | period, gi | PdC 4 s measures |
| `DS_R_GenAcc_Mis4sec` | `urcb_GenAcc_Mis4sec` | URCB | **4000** | period, gi | Gen+storage type |
| `DS_R_SingGen_Mis4sec` | `urcb_SingGen_Mis4sec` | URCB | **4000** | period, gi | Single generator |

**CCLI implementation:** libiec61850 must honour `intgPd=4000` + `period` for DSO periodic subscription; map live signals into FCDA members before enabling RCB.

---

## 13. Logical Device and LN (§9.3.4–9.3.5, pp.109–112 — Batch D)

### 13.1 Table 15 — LDevice (p.109)

**Definition:** logical device reachable via access point; **must contain `LN0`**; may contain preconfigured report/GSE/SMV definitions (in LN0/LN).

**Schema `tLDevice`:** `LN0`(1) + `LN`(0..*) + optional `AccessControl`, `Labels`.

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **inst** | LD id within IED; key for SCL references; non-empty | **`LD_Plant`** |
| **desc** | Description | present (empty) |
| **ldName** | Explicit LD name on wire (61850-7-1/7-2); default = **IEDname + inst** | omitted → `CCI016_01LD_Plant` style |
| **uuid** / **templateUuid** | Instance / template IDs | optional |

**Restrictions (p.110):**

- **`inst` unique** within IED.
- **LD name** (ldName or IED+inst) unique per SCL file; **ldName** unique per **SubNetwork** across SCL files.
- LD name max **64** chars; alphanumeric + `_`.
- **uuid:** SCT creates on new LD; preserve from IID/ISD import; new uuid when templateUuid from ICD.

### 13.2 Table 16 — LN0 (pp.110–111)

**Schema `tLN0`:** extends `tAnyLN`; adds **GSEControl**, **SampledValueControl**, **SettingControl**; fixed **`lnClass="LLN0"`**, **`inst=""`**.

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **lnType** | Reference to **LNodeType** in DataTypeTemplates | **`LLN01`** |
| **lnClass** | Fixed **LLN0** | `LLN0` |
| **inst** | Fixed empty string | `""` |
| **desc**, **prefix** | Description; prefix N/A for LLN0 | — |
| **uuid** / **templateUuid** | Optional instance IDs | not in TR |

**Inherited from `tAnyLN` (p.111):** `DataSet`, `ReportControl`, `LogControl`, `DOI`, `Inputs`, `Outputs`, `Log`, `Labels`.

**LLN0 reference rule:** when referencing LN0 in links — **`lnInst` omitted or empty**; **`lnClass=LLN0`**.

**Log element:** indicates LD contains a log; default log name = **LD inst** if `Log/@name` omitted.

### 13.3 Table 17 — LN (p.112)

**Schema `tLN`:** extends `tAnyLN`; attrs **`lnClass`**, **`inst`**, **`prefix`** (default `""`).

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **lnType** | LNodeType reference | `MMXU1`, `DPCC1`, `LPHD1`, … |
| **lnClass** | IEC 61850-7-4 class | `MMXU`, `DPCC`, `LPHD`, … |
| **inst** | Instance number (unsigned); no leading zeros recommended | **`1`** typical |
| **prefix** | LN prefix | `PdC`, `DisFR`, `PdC_Wi`, … |
| **desc** | Description | per LN |
| **uuid** / **templateUuid** | Optional | not in TR |

**LN contents:** `DataSet`, `ReportControl`, `LogControl`, `DOI`, `Inputs`; optional `Log`(s) with **unique `name`** within LN (default = LD inst).

**Uniqueness:** combination **`prefix` + `lnClass` + `inst`** unique within LDevice (matches **uniqueLNInAccessPoint** at AP level for client LNs).

**TR 57-126 inventory:** one `LD_Plant`; **LN0** holds all 4 DataSets + 4 ReportControls; **~30+ LNs** (LPHD, DPCC, MMXU, DECP, DGEN, control/measurement classes per Allegato T).

---

## 15. Communication (§9.4, pp.143–150 — Batch G)

### 15.1 Fig 22 and structure (pp.143–144)

**Hierarchy:** `<Communication>` → **`SubNetwork`(1..*)** → **`ConnectedAP`(1..*)** → optional **`Address`**, **`GSE`**, **`SMV`**, **`PhysConn`**.

**Uniqueness constraints:**

| Constraint | Rule |
|------------|------|
| **uniqueSubNetwork** | `@name` unique under `Communication` |
| **uniqueConnectedAP** | `@iedName` + `@apName` unique per SubNetwork |
| **uniqueGSEinConnectedAP** | `@cbName` + `@ldInst` unique per ConnectedAP |
| **uniqueSMVinConnectedAP** | same for SMV |

**Principle:** IED section = which LD/LN reachable via AP; Communication section = which APs share a logical bus (**SubNetwork**) and their **project-specific addresses**.

### 15.2 Table 35 — SubNetwork (p.145)

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **name** | Bus id; unique in SCL | `subnetwork1` |
| **desc** | Description | optional |
| **type** | Protocol: `8-MMS`, `8-XMPP`, `IP`, `PHYSICAL` — **mandatory in SCD** | **`8-MMS`** |
| **uuid** / **templateUuid** | Optional instance IDs | not in TR |

**Children:** optional **`BitRate`**; one or more **`ConnectedAP`**. May contain `Text`, `Private`.

**Note:** logical SubNetwork ≠ physical cable — multiple protocol SubNetworks may share one physical network.

### 15.3 Table 36 — ConnectedAP (p.146)

| Attribute | Description | TR 57-126 |
|-----------|-------------|-----------|
| **iedName** | IED reference | `CCI016_01` |
| **apName** | AP within IED | `accessPoint1` |
| **desc** | AP description on this bus | optional |
| **redProt** | `hsr`, `prp`, `rstp`, `none` — per IED caps | not set |
| **apUuid** | AP UUID reference | optional |

**Complete SCD/CID rule:** must specify **server Address** and/or at least one **GSE/SMV** control-block address per ConnectedAP.

**TR:** MMS server only — single `<Address>` block; no GSE/SMV children (GOOSE caps declared in `<Services>` but no GOOSE comm section in TR CID).

### 15.4 Address and P-types (pp.146–147)

**`tAddress`:** unbounded **`<P type="…">value</P>`** elements.

**`tP`:** `type` from **`tPTypeEnum`** (normative values in **IEC 61850-8-1**); value = `tPAddr` (no LF/CR/Tab).

**Validation:** use **`xsi:type="tP_IP"`** etc. for pattern validation (e.g. dotted-quad IP).

**Extension rule:** custom P types start with **capital letter**, alphanumeric + dash only.

**TR 57-126 Address P-types (61850-8-1 MMS):**

| P type | TR value | CCLI |
|--------|----------|------|
| **IP** | `192.168.8.167` | Eth_A static — DSO plan |
| **IP-SUBNET** | `255.255.255.0` | /24 lab default |
| **IP-GATEWAY** | `192.168.8.1` | gateway |
| **OSI-TSEL/PSEL/SSEL/AP-Title/AE-Qualifier** | present | MMS stack selectors |

**Gap:** full **`tPTypeEnum`** list not on pp. 143–150 — cite **61850-8-1** for normative MMS address types.

### 15.5 GSE / SMV comm (pp.147–149) — Batch J partial

| Element | Key attrs | Notes |
|---------|-----------|-------|
| **GSE** (Table 37) | `ldInst`, `cbName`, `cbUuid`; **Address**; **MinTime**, **MaxTime** | GOOSE CB in **LLN0** only |
| **SMV** (Table 38) | `ldInst`, `cbName`, `cbUuid`; **Address** | SV publisher address |

**MinTime/MaxTime:** GOOSE retransmit delay (ms) and supervision heartbeat (ms).

### 15.6 PhysConn — Table 39 (p.150)

**Not MMS IP types** — physical layer description per ConnectedAP.

| PhysConn type | P type | Examples |
|---------------|--------|----------|
| **Connection**, **RedConn** | **Type** | `10BaseT`, `100BaseT`, `FOC`, `Radio` |
| | **Plug** | `RJ45`, `ST` |
| | **Cable**, **Port** | cable/port id strings |

**Restrictions:** one **RedConn** max per AP; if RedConn exists, exactly one matching **Connection**.

---

## 16. DataTypeTemplates (§9.5, pp.153 + 156–170 — Batch H)

### 16.0 Fig 23 — UML overview (p.153)

**Figure 23** shows the **type-definition hierarchy** referenced by the IED instance section (§11 DOI/DAI/SDI). Instance elements mirror this tree: **DOI** ↔ **DO**, **SDI** ↔ **SDO**/DA substructure, **DAI** ↔ leaf **DA**/**BDA**.

```
DataTypeTemplates
└── tDataTypeTemplates
    ├── LNodeType (1..*)     ← LN/@lnType
    ├── DOType    (0..*)
    ├── DAType    (0..*)
    ├── EnumType  (0..*)
    └── ProtNs    (0..*)
```

**Inheritance (base types):**

| Type | Extends | Key attrs |
|------|---------|-----------|
| **tIDNaming** | tBaseElement | **id** (required), desc |
| **tUnNaming** | tBaseElement | desc only |
| **tLNodeType** | tIDNaming | **lnClass**, iedType (deprecated) |
| **tDOType** | tIDNaming | **cdc**, iedType |
| **tDAType** | tIDNaming | iedType |
| **tEnumType** | tIDNaming | — |
| **tDO** | tUnNaming | **name**, **type**→DOType, accessControl, transient |
| **tSDO** | tUnNaming | **name**, **type**→DOType, count |
| **tAbstractDataAttribute** | tUnNaming | **name**, **bType**, fc (DA only), type, valKind, count, sAddr, valImport; **Val** (0..*) |
| **tDA** | tAbstractDataAttribute | **fc**, dchg, qchg, dupd |
| **tBDA** | tAbstractDataAttribute | (nested in DAType; no fc) |

**Composition / references:**

| From | To | Cardinality | Mechanism |
|------|-----|-------------|-----------|
| tDataTypeTemplates | tLNodeType | 1..* | container |
| tLNodeType | tDO | 1..* | DO list defines LN data model |
| tDO | tDOType | 1 | **`@type`** id reference |
| tDOType | tSDO, tDA | 0..* each | structure + leaf attrs |
| tSDO | tDOType | 1 | **`@type`** (may nest; no recursive loops) |
| tDA / tBDA | tDAType or tEnumType | 0..1 | **`@type`** when bType=Struct or Enum |
| tDAType | tBDA | 1..* | structured attribute breakdown |
| tEnumType | tEnumVal | 1..* | **ord** + string value |

**Labels:** tLNodeType, tDOType, tDAType may contain **Labels** → **Label** (`@id`, `@lang`); uniqueness per IED (`uniqueLabelInIED`).

**TR 57-126 read path:** `LN lnType="MMXU1"` → LNodeType **MMXU1** → DO **TotW** type=**MV_*** → DOType **MV_*** → DA **mag** bType=Struct type=**AnalogueValue_*** → DAType → BDA **f** bType=FLOAT32; control DOs use **DA ctlModel** bType=Enum type=**CtlModelKind**.

### 16.1 Table 40 — Template elements (p.156)

| Element | Role | TR 57-126 |
|---------|------|-----------|
| **LNodeType** | LN instantiable type; referenced by `LN/@lnType` | `LLN01`, `MMXU1`, `DPCC1`, … |
| **DOType** | DO type; CDC-based (61850-7-3); referenced by `DO/@type` or SDO | `ENC_1_Mod`, `MV_2_*`, … |
| **DAType** | Structured attribute type; referenced when `bType=Struct` | e.g. analogue structures |
| **EnumType** | Enumeration; referenced when `bType=Enum` | `CtlModelKind`, `Beh`, `Mod`, … |

**Location:** `<DataTypeTemplates>` at end of `<SCL>` (with Header, IED, Communication).

### 16.2 Tables 41–42 — LNodeType and DO (pp.157–158)

**Table 41 — LNodeType:**

| Attribute | Description | TR |
|-----------|-------------|-----|
| **id** | Global template id | `MMXU1` |
| **lnClass** | 61850-7-4 class | `MMXU`, `LLN0`, … |
| **desc** | Description | optional |
| **iedType** | **Deprecated Ed2.2** — do not constrain at LNodeType | not used |

**Table 42 — DO (in LNodeType):**

| Attribute | Description |
|-----------|-------------|
| **name** | DO name per 61850-7-4 |
| **type** | References **DOType/@id** |
| **accessControl**, **transient**, **desc** | optional |

### 16.3 Tables 43–44 — DOType and SDO (pp.158–160)

**DOType** must contain ≥1 **DA** or **SDO** in SCD/ICD. **`cdc`** required (61850-7-3). **No recursive SDO→DOType loops.**

| DOType attr | Meaning |
|-------------|---------|
| **id** | Global DOType id |
| **cdc** | Common Data Class |
| **iedType** | Empty = all IED types |

**SDO (Table 44):** sub-structure reference; **`type`** → DOType id; **`count`** for arrays (0 = no array).

### 16.4 Table 45 — Basic type mapping (pp.160–161)

Maps **61850-7-2** types → XML Schema → SCL **`bType`**. Key entries:

| bType | Use in SCL Val |
|-------|----------------|
| INT*, FLOAT32/64, BOOLEAN | numeric / true/false |
| **Enum** | EnumType member **name** string |
| **VisString***, **Unicode255** | normalizedString, no tabs/LF |
| **Timestamp** | `dateTime` e.g. `2007-12-31T21:01:12.345` |
| **ObjRef** | `@ldInst/lnName[.doName[.daName]]` or `#UID` |
| **Quality**, **EntryID**, **Dbpos**, **Tcmd**, **TrgOps**, **OptFlds** | not specified in SCL (opaque / in control blocks) |

### 16.5 Table 46 — valKind (p.162)

| valKind | Meaning |
|---------|---------|
| **Spec** | Specification-phase wanted value (SSD) |
| **Conf** | Engineered into IED; not visible online |
| **RO** | Read-only at IED; set at configuration |
| **Set** | Default/setting value in IED |

**Rules:** ignored for `bType=Struct`; **`valImport=true`** allows SCD import even if RO/Conf.

### 16.6 Table 47 — DA (pp.163–164)

| Attribute | Description | TR example |
|-----------|-------------|------------|
| **name** | From 61850-7-3 enum + lowercase extensions | `ctlModel`, `stVal` |
| **fc** | Functional constraint; SE implies SG | `CF`, `ST`, `MX`, … |
| **bType** | From `tBasicTypeEnum` | `Enum`, `Struct`, … |
| **type** | Required if `bType=Enum` or `Struct` | `CtlModelKind` |
| **dchg/qchg/dupd** | Supported trigger options | on oper/status attrs |
| **valKind** | Default **Set** | template defaults |
| **count** | Array size (0 = scalar) | |
| **sAddr** | Short address (SCSM/IED-specific) | optional |

**Mandatory on every instantiable DA:** **name**, **fc**, **bType**.

### 16.7 tBasicTypeEnum (pp.165–166)

Predefined types include BOOLEAN, INT8–INT128, INT8U–INT32U, FLOAT32/64, Enum, Dbpos, Tcmd, Quality, Timestamp, VisString*, Octet64, Struct, ObjRef, TrgOps, OptFlds, EntryID, Octet6/16, …

**Notes:** INT128/INT24U **deprecated**; extensions via `tBasicTypeEnum` with mustUnderstand/mayIgnore rules.

### 16.8 Tables 48–49 — BDA and EnumType (pp.168–170)

**BDA:** same attrs as DA (Table 48); nested in **DAType**; **`ProtNs`** follows for stack-specific structures (default type `8-MMS`).

**EnumType (Table 49):**

| Attribute | Description |
|-----------|-------------|
| **id** | Referenced by DA/BDA `type` when `bType=Enum` |
| **desc** | Optional |

**EnumVal:** required **`ord`** (int); value = **`tEnumStringValue`** (max 127 chars, Basic Latin + Latin-1 Supplement).

**TR 57-126 — `CtlModelKind`:**

| ord | Intended value | CCLI note |
|-----|----------------|-----------|
| 0 | status-only | verify TR CID strings at K4.1 — PDF may have corrupted spacing |
| 1 | direct-with-normal-security | |
| 2 | sbo-with-normal-security | |
| 3 | direct-with-enhanced-security | Phase 3 + 62351-4 |
| 4 | sbo-with-enhanced-security | |


---

## 14. Crosswalk — TR 57-126 example CID

| TR 57-126 CID | 61850-6 rule (this extract) |
|---------------|----------------------------|
| `<Header id="CCI_Oss_Contr">` + `<History><Hitem>` | §8.1–8.3, Tables 3–4 |
| No `uuid` on Header | Gap vs Table 3 (2024) — add at deploy |
| `<IED engRight="full" originalSclVersion="2007" originalSclRevision="B">` | §9.1–9.2 |
| `<Services>` full block (ConfDataSet modify=false, ReportSettings Fix, TimeSyncProt) | §9.3 Table 11 |
| `<AccessPoint name="accessPoint1">` + `<Server>` | §9.4–9.5 |
| `<Authentication />` (none until 62351-4 TLS) | §9.5 — Phase 3 enable certificate |
| File is `.cid` / configured IED | §7 CID definition (p.34) |
| `version="2007" revision="B" release="4"` | §8.2.2 |
| `encoding="UTF-8"` | §8.4 |
| `<LDevice inst="LD_Plant">` + `<LN0 lnType="LLN01">` | §13 Tables 15–16 |
| `<LN prefix="PdC" lnClass="MMXU" lnType="MMXU1" inst="1">` | §13 Table 17 |
| Single `LD_Plant`, all reports in LN0 | §6.6 + §13 |
| No `<Substation>` | §8.4 — CID need not include substation |
| `nameStructure="IEDName"` | §8.2.3 (FuncName deprecated) |
| `DS_R_PdC_Mis4sec` + `urcb_PdC_Mis4sec intgPd=4000` | §12 Tables 21–23 |
| `brcb_Stato_Allarmi_Segnali` buffered status | §12.3–12.5 |
| `RptEnabled max=2` | §12.4 Table 24 |
| `<SubNetwork type="8-MMS">` + `ConnectedAP` + IP/OSI P-types | §15 Tables 35–36, Address |
| `<DataTypeTemplates>` LNodeType/DOType/EnumType | §16 Fig 23 + Tables 40–49 |
| `LN/@lnType` → LNodeType → DO → DOType chain | §16.0 Fig 23 |
| `CtlModelKind` + `DA ctlModel bType=Enum` | §16.6–16.8 |
| `<DOI><DAI><Val>` instance defaults (Mod, WRtg/setMag) | §11 Tables 18–20 |

---

## 17. Batch I — ExtRef, Log, ClientLN (pp. 125–142)

### 17.1 Table 25 — ClientLN (preconfigured report client)

| Attribute | Description | TR / CCLI |
|-----------|-------------|-----------|
| **apRef** | Client access point reference | DSO AP name |
| **iedName**, **ldInst**, **prefix**, **lnClass**, **lnInst** | Client LN reference (`agLNRef`) | Match subscribing HMI |
| **lnUuid** | Optional UUID | Future tooling |

Links **ReportControl** `indexed=true` instance index to **ClientLN** list order.

### 17.2 Table 26 — LogControl

| Attribute | Description |
|-----------|-------------|
| **name**, **datSet** | Log binds to DataSet in same LN |
| **logName** | Target log |
| **logEna** | Enable logging |

**CCLI:** optional audit log for control actions; primary cyber log remains **62351-14** Syslog.

### 17.3 Table 33 — ExtRef (external reference)

Maps internal **DO/DA** to external IED signal (`iedName`, `ldInst`, `lnClass`, `lnInst`, `doName`, `daName`).

**Use:** `signal_map.yaml` → `<ExtRef>` for plant devices without duplicating full remote IED in CID.

### 17.4 Tables 51–52 — ExtRef use cases

| Table | Content |
|-------|---------|
| **51** | Engineering use cases for external references |
| **52** | External control (`ExtCtrl`) patterns |

**Not** Table 52 @ p.174 (SCT engineering actions) — see SKIP list in capture plan.

---

## 18. Batch J — GOOSE control comm (pp. 129–130) — complete

### 18.1 Tables 27–28 — GSEControl in IED section

| Element | Key attributes |
|---------|----------------|
| **GSEControl** (Table 27) | `name`, `datSet`, `appID`, `confRev`, `type` GOOSE/GSE |
| **GSE address** (Table 28) | MAC/VLAN/AppID comm parameters at IED level |

**ConnectedAP GSE** (Tables 37–38, §15.5) carries wire-level Address on SubNetwork — already captured.

**CCLI P1:** plant GOOSE requires **61850-8-1** + **62351-6** auth extension; TR 57-126 CID is MMS-centric (no GOOSE comm block in example).

---

## 19. Batch K — Annex G / I (pp. 255, 267–285)

### 19.1 Annex G — Table G.1 (IED configurator SICS)

**System Integration Conformance Statement** checklist for tools loading CID/ICD — evidence for DSO acceptance tests (Phase 3).

| Area | SICS topic |
|------|------------|
| Data model | LN/DO/CDC support |
| Services | ACSI services implemented |
| Reports | BRCB/URCB behaviour |
| Security | 62351-4/6 capabilities |

### 19.2 Annex I — Mixed SCL versions (Fig I.1–I.7)

Rules for **downgrade/upgrade** when `version` / `revision` / `release` differ between tool chain and deployed CID.

**TR 57-126:** `version="2007" revision="B" release="4"` — validate imports with Annex I when DSO tools emit newer `release`.

---

## Knowledge gaps (remaining)

| Batch | Content |
|-------|---------|
| **G′** | MMS **`tPTypeEnum`** full list — normatively **61850-8-1** → **`ccli-61850-8-1-extract`** / OCR corpus (2004 Ed.1) |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-61850-6-001 | Inspect TR CID | No `TEMPLATE` IED name — real `CCI016_01` |
| REQ-61850-6-003 | Parse `<SCL>` attrs | version/revision/release present |
| REQ-61850-6-002 | Grep CID | All `datSet=` on ReportControl match DataSet in same LN0 |
| REQ-61850-6-022 | Grep `urcb_*` | `intgPd="4000"` and `<TrgOps period="true"/>` |
| REQ-61850-6-004 | Read line 1 | UTF-8 prolog |
| REQ-61850-6-009 | Inspect `<History><Hitem>` | version, revision, when populated |
| REQ-61850-6-030 | Grep `<Communication>` | `8-MMS`, IP 192.168.8.167, ConnectedAP match IED |
| REQ-61850-6-034 | Grep `CtlModelKind` | EnumType + DA references present |
| K4.1 | libiec61850 load CID | No schema errors; RCB + templates resolve |

---

## RAG routing

**Pack C — Protocols** (P0 extract sufficient for CID engineering):

```
ccli-61850-6-extract, cei-tr-57-126, cei-0-16-allegato-t
```

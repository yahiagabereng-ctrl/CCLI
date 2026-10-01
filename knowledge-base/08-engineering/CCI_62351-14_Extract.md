# IEC 62351-14 — CCLI Engineering Extract

**Document ID:** CCLI-SEC-62351-006  
**Revision:** 1.6  
**Date:** 2026-08-12  
**RAG source_id:** `ccli-62351-14-extract`  
**Normative basis:** IEC **62351-14** — *Power systems management and associated information exchange — Data and communications security — Part 14: Cyber security event logging* (**CDV 2025** per capture)  
**Capture status:** **HAVE** — full normative body pp. 6–91 · §8 French (p. 45) skipped  
**Parents:** `ccli-k63-ceremony-sop` · `ccli-62351-9-extract` · `ccli-62351-3-extract` · `ccli-62351-4-extract` · `ccli-62443-4-2-extract`  
**Programme link:** **CR 6.2** · K6.3 syslog evidence · **62351-9 Annex D** log target

---

## Summary

**62351-14** standardises **cyber security event** definition (**Table 1**) and **logging** (**Table 2**), **event ID allocation** (§4.4), **dynamic parameters** (§4.5), **Syslog RFC 5424** wire mapping (§6), and **Annex B.1.6** normative **62351-9 PKI event catalogue** (**Tables 33–37**).

**TG-524 / K6.3:** Full PKI syslog vocabulary — **66 events** across groups **1–5**; C1 P0 = groups **1–4** (**58 events**). Wire format: SD **`62351-14@41912`**, ID e.g. **`IEC62351-9:1.5`**, PRI per Table 7.

**vs 62351-7:** Part **14** = **event logs** (Syslog → repository); Part **7** = **health monitoring** (SNMP situational awareness).

---

## Requirements

| REQ-ID | Source | Requirement | CCLI |
|--------|--------|-------------|------|
| **REQ-3514-SCOPE-001** | §1 | Abstract event define + log structures; standard event list; **secure Syslog** transport | CR 6.2 |
| **REQ-3514-EVT-001** | Table 1 | Every defined event: **IECVersion**, **LogType**, **MNEMONIC**, **Severity**, **ID**, **Text** | Event DB |
| **REQ-3514-EVT-002** | Table 2 | Every log record: **Timestamp** (UTC ISO8601), **SqNum**, **Source**, **App-Name** | Anti-replay |
| **REQ-3514-EVT-003** | §4.4 | **ID** = `DomainID:EventGroup.EventNumber` (ASCII ≤32) | Map 62351-9 Annex D |
| **REQ-3514-EVT-004** | Table 1 Severity | **alarm · error · warning · notice** (aligns 62351-9 §6.2) | SIEM policy |
| **REQ-3514-EVT-005** | §4.5 | **P.Label** dynamic insertion into **Text** | Parametrised logs |
| **REQ-3514-EVT-006** | Table 2 PeerInfo | Log peer on conditional events; TLS recommended anti-spoof | C1 MMS |
| **REQ-3514-K63-001** | Cross-ref | 62351-9 events use Domain **`IEC62351-9`** | K6.3 SOP |
| **REQ-3514-SYS-001** | §6.1 Table 4 | Map Table 2 → RFC 5424 **HEADER**; **Source** → **HOSTNAME** (mandatory on wire) | OpenWrt |
| **REQ-3514-SYS-002** | §6.2 | Mandatory SD-ELEMENT **`62351-14@41912`** (IANA enterprise **41912**) | Parser |
| **REQ-3514-SYS-003** | §6.2 Table 5 | **ID** mandatory SD-PARAM; **Text** or **MNEMONIC** (or both) | Event emit |
| **REQ-3514-SYS-004** | §6.4 Table 7 | **PRI** = `8 × 13 + severity`; alarm **105** · error **107** · warning **108** · notice **109** | SIEM filter |
| **REQ-3514-SYS-005** | §6.5 | **TCP/6514** + **RFC 5425 TLS**; **UDP prohibited**; TLS params from **62351-3** | Eth_mgmt |
| **REQ-3514-SYS-006** | §6.2.2 Table 6 | Optional **`timeQuality`** SD-ELEMENT when GNSS/NTP trust uncertain | GNSS lab |
| **REQ-3514-B16-001** | Annex B.1.6 | All **62351-9** events: **IECVersion=1** · Domain **`IEC62351-9`** | Event DB |
| **REQ-3514-B16-002** | Tables 33–36 | K6.3 C1 events groups **1–4** shall be emit-capable on TG-524 | Firmware |
| **REQ-3514-B16-003** | Table 37 | GDOI group **5** events — **P2** only | Not C1 P0 |
| **REQ-3514-B11-001** | Annex B.1.1 | **62351-3** TLS events Domain **IEC62351-3** · groups **1** handshake · **2** cert | Eth_A :3782 |
| **REQ-3514-B12-001** | Annex B.1.2 | **62351-4** E2E events Domain **IEC62351-4** · groups **1–7,10–13,15,18** | MMS C1 |
| **REQ-3514-CONF-001** | §11 Table 8 | Log generation + collection: Table 2 mandatory on Syslog | Conformance |
| **REQ-3514-ANN-A-001** | Annex A | Generic events Domain **IEC62351-14** groups **1–9** | CR 2.8 bridge |
| **REQ-3514-ANN-D-001** | Table 50 | Map **62443-4-2** category → Annex A groups | ccli-62443-4-2-extract |

---

## Architecture

### Event model (Figure 1)

```
Table 1 (Event Definition)          Table 2 (Logged Event)
─────────────────────────          ────────────────────────
M: IECVersion, LogType, MNEMONIC   + M: Timestamp, SqNum
M: Severity, ID, Text              + M: Source, App-Name
O: Desc                            + C: PeerInfo, UsrID, Role
M: Params[] → P.Label              + C: P.Label (from Params)
M: ExtraInfo[] → X.Label           + C: X.Label
O: SupersededBy                    + C: RefEvtID
```

**Legend:** M mandatory · O optional · C conditional (capability present).

### ID grammar (§4.4)

```
ID = DomainID + ":" + EventGroup + "." + EventNumber
```

| Field | Size / type | Rule |
|-------|-------------|------|
| **DomainID** | 1–20 ASCII | **`IEC62351-14`** (Annex A generic) · **`IEC62351-3`**, **`IEC62351-9`**, … (part-specific Annex B) · vendor = IANA enterprise number |
| **EventGroup** | uint16 **1–65535** | Clusters events within domain |
| **EventNumber** | uint16 **1–65535** | Per group; **0 reserved** |

**62351-9 cross-map:** Annex D `IEC 62351-9:1.5` → Domain **`IEC62351-9`** (or `IEC62351-9` per spacing in source tables) · Group **`1`** · Event **`5`** → logged ID **`IEC62351-9:1.5`**.

**Allocation authority:** **62351-14** maintains global space; existing part definitions shall not change meaning across editions.

---

## §1 Scope (p. 9)

**In scope:**

1. Abstract metadata structure for **defining** and **logging** cyber security events  
2. Standardised event list (Annex A generic + Annex B part-specific)  
3. **Secure Syslog** transfer method  

**Out of scope:** Non-Syslog logging protocols; automated root-cause analysis (logging only supplies data).

**62351-3 precedence:** As sibling parts evolve, their event descriptions take precedence where referenced.

---

## §2 Normative references (pp. 10–11)

| Reference | Use |
|-----------|-----|
| **62351-1 … 62351-12** | TC 57 security parts |
| **62351-13** | Security topics guidelines |
| **62351-90-3** | NSM workplace (with 62351-7/14) |
| **62443** · **62443-4-2** | IACS component security |
| **ISO/IEC 27001:2022** | ISMS |
| **RFC 5424** · **RFC 5425** | **Syslog** (+ TLS transport) |
| **RFC 5246** · **RFC 8446** | TLS 1.2 / 1.3 |
| **RFC 3647** · **X.509** (9594-8) | PKI / certificates |
| **RFC 3629** | UTF-8 |
| **IEEE 1686** | IED cyber security capabilities |
| **NERC-CIP-007-6** | System security management |

---

## §3 Terms and definitions (pp. 12–15)

| Term | Definition |
|------|------------|
| **3.1 Cyber security event** | Event in cybersecurity aspect of an entity |
| **3.2 Cyber security event log** | Stored/logged occurrence |
| **3.2.1 Log generation entity** | Device/app that creates log |
| **3.2.2 Log collection entity** | Receiver/collector |
| **3.3 Event** | Condition or action occurrence |
| **3.4 Entity** | Person, place, process, device, … |
| **3.5 Type** | Datatype + size limit of attribute |
| **3.6 Characters** | Named ASCII + ABNF; **UTF-8** for text fields |
| **3.7 User** | Human or non-human (software/device) |
| **3.8 Abbreviations** | DER, DNS, E2E, FQDN, MMS, OSI, SD, TLS, … (+ **62351-2**) |

**Named character sets (§3.6.2):** ASCII-PRINT, ASCII-HOSTNAME, ASCII-IPV4, ASCII-IPV6, ASCII-CIDR, …

---

## §4 Structure for cyber security event (pp. 15–25)

Agonistic of logging protocol; maps to **Syslog RFC 5424** (§6) and **62443-4-2** attributes (Annex D).

### Table 1 — Define an event (§4.1, pp. 16–18)

| Attribute | M/O | Type / rule |
|-----------|-----|-------------|
| **IECVersion** | M | Integer **1–255**; this edition starts at **1** |
| **LogType** | M | Fixed **`IEC62351-14`** |
| **MNEMONIC** | M | ASCII ≤64; `ASCII-ALPHANUMERIC` + `_`; e.g. `LOGIN_SUCCESS` |
| **Severity** | M | **`alarm`** · **`error`** · **`warning`** · **`notice`** |
| **ID** | M | ASCII ≤32; see §4.4 |
| **Text** | M | UTF-8 ≤128; may contain `{P.Name}` tokens |
| **ExtraInfo[]** | M | `.Name` prefix **`X.`** · optional `.Desc` |
| **Desc** | O | UTF-8 description of event purpose |
| **Params[]** | M | `.Name` prefix **`P.`** · optional `.Desc` |
| **SupersededBy** | O | ID of replacement event |

**Stability rules:** `LogType` and `MNEMONIC` constant across editions; **ID** cannot be reused for different purpose; changing IECVersion/Severity/ID/Params superseded → new definition.

### Table 2 — Log an event (§4.2, pp. 19–22)

| Attribute | M/O/C | Rule |
|-----------|-------|------|
| **IECVersion** | M | From Table 1 |
| **LogType** | M | From Table 1 |
| **Timestamp** | M | **UTC ISO 8601-1:2019**; ≤1 s skew; ms recommended |
| **SqNum** | M | **1–4294967295**; increment; wrap; **0** if unsupported; resume after reboot +1 |
| **ID** | M | From Table 1 |
| **Severity** | M | From Table 1 |
| **Text** | C | **Text or MNEMONIC** required (one or both) |
| **MNEMONIC** | C | From Table 1 |
| **Source** | M | ASCII ≤255: **HOSTNAME / IPv4 / IPv6 / CIDR** |
| **App-Name** | M | ASCII ≤48 printable; distinguishes app within device |
| **PeerInfo** | C | ASCII ≤64; remote peer; **TLS** recommended against spoofing |
| **X.Label** | C | ExtraInfo key/value pairs (`X.` prefix) |
| **UsrID** | C | UTF-8 ≤32; **GDPR** if human |
| **Role** | C | UTF-8 ≤29; **62351-8** triplet `roleID:revision:roleDefinition` if supported |
| **P.Label** | C | Dynamic param values for `{P.*}` in Text |
| **RefEvtID** | C | Root-cause cross-reference event ID |

**Privacy:** Certificate subject / UsrID may be personal data — operator policy required.

### §4.3 Figure 1 (p. 23)

Maps Table 1 definition fields → Table 2 log fields; **Desc** and **SupersededBy** not logged directly.

### §4.5 Dynamic parameters (p. 25, Table 3)

Example:

| MNEMONIC | Severity | ID | Text |
|----------|----------|-----|------|
| `USER_ACCNT_CREATE_SUCCESS` | notice | `IEC62351-14:1.10` | `User account created successfully: "{P.NewUser}"` |

`Params[] = [{ Name: P.NewUser, Value: arijit }]` → resolved text substitutes `{P.NewUser}`.

Non-sequential `{P.*}` tokens allowed. Collectors may substitute via **Params[]** / **STRUCTURED-DATA** or localised event DB.

---

## §5 Electronic event description (p. 26)

| Rule | Detail |
|------|--------|
| Format | **XML** per XSD (§7); domain owner provides event catalogue |
| Syslog servers | Event definitions **shall** be available in XML for parser pre-load |
| Field devices | XML export **optional** |
| Translations | XSD supports text localisation files (§7.2) |

---

## §6 Syslog RFC 5424 mapping (pp. 26–32)

Maps **Table 2** abstract attributes → RFC 5424 **HEADER** · **STRUCTURED-DATA** · **MSG** · **PRI**.

**Note:** Mandatory/optional status may differ from Table 2 when mapped to Syslog (e.g. **Source** mandatory as **HOSTNAME**). All HEADER fields present in formatted message; omitted values use **NIL** (`-`).

### §6.1 Table 4 — HEADER mapping (pp. 26–27)

| Table 2 attribute | RFC 5424 field | Value / rule |
|-------------------|----------------|--------------|
| **Severity** | **PRI** | Derived per §6.4 / Table 7 |
| — | **VERSION** | Fixed **`1`** |
| **Timestamp** | **TIMESTAMP** | UTC per RFC 5424 §6.2.3 |
| **Source** | **HOSTNAME** | FQDN · hostname · IPv4 · IPv6 · **NIL** (RFC 5424 preference order) |
| **App-Name** | **APP-NAME** | Functional area; **NIL** if unknown |
| — | **PROCID** | **Not meaningful** for 62351-14 interoperability — **ignore** on receive; **NIL** if unset |
| **LogType** + **IECVersion** | **MSGID** | Concatenate: `LogType` + `:` + `IECVersion` → **`IEC62351-14:1`** (Ed 1) |

### §6.2 STRUCTURED-DATA (pp. 28–29)

**Mandatory SD-ELEMENT:** **`62351-14@41912`** (IEC IANA enterprise number **41912**).

Vendor SD-ELEMENTs permitted; interoperability not guaranteed. Put higher-priority SD data nearer HEADER (truncation drops from message end).

#### Table 5 — SD-ELEMENT `62351-14@41912` (p. 28)

| Attribute | SD-PARAM | Clause |
|-----------|----------|--------|
| **ID** | `SD-PARAM("ID")` | **Mandatory** |
| **MNEMONIC** | `SD-PARAM("MNEMONIC")` | **Conditional** — Text and/or MNEMONIC |
| **Text** | `SD-PARAM("Text")` | **Conditional** — value **before** `{P.*}` substitution |
| **X.Label** | `SD-PARAM("X.Label")` | **Optional** — one per unique label |

#### Table 5 cont. — conditional SD-PARAMs (p. 29)

| Attribute | SD-PARAM | Clause |
|-----------|----------|--------|
| **SqNum** | `SD-PARAM("SqNum")` | Conditional (Table 2) |
| **UsrID** | `SD-PARAM("UsrID")` | Conditional |
| **PeerInfo** | `SD-PARAM("PeerInfo")` | Conditional |
| **Role** | `SD-PARAM("Role")` | Conditional |
| **P.Label** | `SD-PARAM("P.Label")` | Conditional — all unique `P.*` for instance |
| **RefEvtID** | `SD-PARAM("RefEvtID")` | Conditional |

#### §6.2.2 Table 6 — SD-ELEMENT `timeQuality` (optional, p. 29–30)

Derived from **Timestamp** capture instant. Recommended when timezone or sync status uncertain; if included and sync doubtful, place **first** in STRUCTURED-DATA.

| SD-PARAM | Clause | Rule |
|----------|--------|------|
| **tzKnown** | Optional | RFC 5424 §7.1.1 |
| **isSynced** | Optional | If omitted → collector trusts TIMESTAMP (1 s); if doubt → include **`0`** |
| **syncAccuracy** | Conditional | Only if **`isSynced=1`** and accuracy known; else collector assumes 1 s |

### §6.3 MSG (p. 30)

**Optional** unstructured field for vendor-specific or legacy display. Manufacturers document MSG build rules. Legacy collectors without STRUCTURED-DATA may use MSG with **substituted** dynamic parameters (§4.5).

### §6.4 Table 7 — PRI / severity (pp. 30–31)

```
PRI = 8 × Facility + Severity   →  formatted as <PRI>
Default Facility = 13  (log audit)
```

| 62351-14 Severity | Syslog severity | PRI |
|-------------------|-----------------|-----|
| **alarm** | 1 | **`<105>`** |
| **error** | 3 | **`<107>`** |
| **warning** | 4 | **`<108>`** |
| **notice** | 5 | **`<109>`** |

Syslog levels **6 (Information)** and **7 (Debug)** are **out of scope**.

### §6.5 Secure transport (p. 31)

| Rule | Value |
|------|-------|
| Transport | **TCP** only — **UDP shall not** carry 62351-14 events |
| Security | **RFC 5425** (Syslog TLS) mandatory on TCP |
| Port | **6514** (`syslog-tls`) |
| TLS | RFC 5425 requires **TLS 1.2** minimum; **TLS 1.3 strongly recommended**; cipher/session params reuse **62351-3** |
| Relays | Must preserve all logging attributes |
| Time quality | RFC 5424 §7.1 via **`timeQuality`** SD-ELEMENT |

### §6.6 Raw storage (p. 32)

Mapped RFC 5424 message (§6.1–§6.4) may be stored verbatim in a file.

## §7 Cyber security event logging using XML (pp. 32–44) — Batch F

XML method to **define** events (Table 1) and **log** events (Table 2). Syslog remains **lab default** on TG-524; XML is **optional** per §11 Table 8 but **mandatory on log-collection entities** that export/import XML catalogues.

### §7 overview (pp. 32–33)

| Item | Value |
|------|-------|
| **Namespace** | `http://iec.ch/TC57/IEC-62351-14-ed1` |
| **Schema version** | **1.0** |
| **Purpose** | XSD framework for security-event definitions + logged event instances |

Three schema files:

| File | Role |
|------|------|
| **`SECEVT_BaseTypes.xsd`** | Shared base / simple types (Table 1 + Table 2 field constraints) |
| **`SECEVT_EventsDefinition.xsd`** | Event **definition** catalogue per domain (Table 1) |
| **`SECEVT_EventList.xsd`** | **Logged** event instances (Table 2) |

### §7.1.1 — `SECEVT_BaseTypes.xsd` (pp. 32–35)

#### Abstract foundation

| Type | Role |
|------|------|
| **`tBaseElement`** | Abstract root — `xs:any` / `xs:anyAttribute` from other namespaces (**vendor extensions**, backward-compatible evolution) |
| **`tDescribedObject`** | Extends `tBaseElement` + **`agDesc`** (`desc` optional normalizedString) |

#### ID and classification types (§4.4 / Table 1)

| Type | Constraint | Maps to |
|------|------------|---------|
| **`tIECVersion`** | integer **1–255** | Table 1/2 **IECVersion** |
| **`tEventGroupID`** | **1–65535** | Event group number |
| **`tEventNumericID`** | **`G.N`** (two 1–65535 parts) | ID without domain prefix (e.g. `2.1`) |
| **`tEventDomainID`** | string **1–20** | Domain part (e.g. `IEC62351-14`) |
| **`tEventID`** | **`DomainID:G.N`** pattern | Table 1/2 **ID** (fully qualified) |
| **`tSeverity`** | `alarm` · `error` · `warning` · `notice` | Table 1/2 **Severity** |
| **`tMnemonic`** | **1–64** chars · pattern **`[A-Z,_]*`** | Table 1/2 **MNEMONIC** |

#### Dynamic / extra-info naming (§4.5)

| Type | Pattern | Max | Maps to |
|------|---------|-----|---------|
| **`tParameterName`** | **`P.[0-9A-Za-z_]*`** | 32 | **Params[].Name** / **P.Label** |
| **`tExtraInfoName`** | **`X.[0-9A-Za-z_]*`** | 32 | **ExtraInfo[].Name** / **X.Label** |

#### Localisation types

| Type | Rule |
|------|------|
| **`tLangID`** | ISO-639-1 alpha-2 (`[a-z]{2}`) — e.g. `en`, `fr`, `de` |
| **`tEventText`** | Text **1–128** chars · optional `@lang` (default **`en`**) → Table 1/2 **Text** |
| **`tEventDetailDesc`** | Unbounded string · `@lang` (default **`en`**) → Table 1 **Desc** |
| **`tASCIIString`** | Basic Latin only — base for Source, App-Name, etc. |

### §7.1.2 — `SECEVT_EventsDefinition.xsd` (pp. 36–40)

#### Definition structures (Table 1)

| Type | Contents |
|------|----------|
| **`tParameterDef`** / **`tParameterDefList`** | **`PDef`** elements · attr **`name`** → **Params[]** |
| **`tExtraInfoDef`** / **`tExtraInfoDefList`** | **`EIDef`** elements → **ExtraInfo[]** |
| **`tEventDef`** | **Text** · **Desc** · **Params** · **ExtraInfo** + attrs **`ID_GrAndNumber`** · **`MNEMONIC`** · **`Severity`** · **`SupersededBy`** |
| **`tEventGroupDef`** | **`EventDef`** list + **`ID_GrNumber`** · unique **`@lang`** per Text/Desc |
| **`tEventDefList`** | **`EventGroup`** list + attrs **`LogType`** (fixed **`IEC62351-14`**) · **`IecVersion`** · **`DomainID`** · **`ListVer`** · **`PreRelease`** |
| **`tEventDefinition`** | **`DomainEventsDef`** — global container; **unique event ID per domain** |

#### Translation structures (§7.1.2.3)

| Type | Role |
|------|------|
| **`tEventTranslationDef`** | Translated **Text** / **Desc** for one **`ID_GrAndNumber`** |
| **`tEventTranslationDefList`** | **`EventTrDef`** list (replaces EventGroup in translation files) |
| **`tEventTranslationDefinition`** | **`DomainEventsTrDef`** container |

#### Root element (§7.1.2.4)

```xml
<xs:element name="SECEVTDef" type="tEventDefTopElement"/>
```

| Child | Content |
|-------|---------|
| **`EventDefinitions`** | Domain event catalogue |
| **`EventTranslations`** | Language translation overlay |

**Publication rules (§7.2):** public file = **one domain**; **either** definitions **or** translations (not both). Translation file = **one domain + one language**.

### §7.1.3 — `SECEVT_EventList.xsd` (pp. 40–44)

#### Table 2 simple types

| Type | Constraint | Table 2 field |
|------|------------|---------------|
| **`tTimeStamp`** | generalized time | **Timestamp** |
| **`tSqNum`** | integer **1–4294967295** | **SqNum** |
| **`tSource`** | ASCII ≤ **255** | **Source** |
| **`tAppName`** | ASCII ≤ **48** | **App-Name** |
| **`tPeerInfo`** | ASCII ≤ **64** | **PeerInfo** |
| **`tUsrID`** | ASCII ≤ **32** | **UsrID** |
| **`tRole`** | ASCII ≤ **29** | **Role** |
| **`tParameterText`** / **`tExtraInfoText`** | normalizedString ≤ **128** | **P.Label** / **X.Label** |

#### Log instance structures

| Type | Role |
|------|------|
| **`tParameter`** / **`tParameterList`** | **`P`** elements (name + value) |
| **`tExtraInfo`** / **`tExtraInfoList`** | **`X`** elements |
| **`tOtherData`** / **`tOtherDataList`** | **`O`** elements — **vendor payload; content out of scope** |
| **`tEventEntry`** | Single log record — all Table 2 mandatory/optional attrs as XML attributes + nested Params/ExtraInfo/OtherData |
| **`tEventEntryList`** | **`EventEntry`** list + **`LogType`** · **`IecVersion`** · optional list-level **`Source`** (applies to all entries) |

#### Root element

```xml
<xs:element name="SECEVTLogs" type="tEventLogs"/>
```

**`EventLog`** may contain entries from **one or more devices** (multi-device archive).

### §7.2 — XML file versioning rules (p. 44)

| Rule | Detail |
|------|--------|
| **`ListVer`** | Integer ≥ 1 — definition-file version |
| **`PreRelease`** | Optional pre-release tag |
| Supersession | Higher **`ListVer`** supersedes lower |
| Backward compat | New **`ListVer`** **shall** retain **all** events from prior version |
| Text updates | Permitted; **shall not** remove/rename events incompatibly (p. 44 truncated in capture) |

### CCLI / TG-524 mapping

| Use case | Binding |
|----------|---------|
| **Pi / TG-524 lab** | **Syslog §6** wire format — XML export not required on device |
| **SIEM / collector** | May ingest **`SECEVTLogs`** XML or RFC 5424; **`SECEVTDef`** pre-load for parser |
| **62351-9 / K6.3** | Domain catalogue **`IEC62351-9`** events align Annex B.1.6 + Annex D mnemonics |
| **62443 CR 6.2** | XML optional on generation entity; collector may require XML raw storage (§11 Table 8) |

**Firmware artefact (future):** `SECEVTDef` XML bundling Table 9 + Annex B domain catalogues for offline SIEM import.

---

## CCLI — K6.3 Syslog wire example (Batch B)

**Event:** EST enrolment success (**62351-9 Annex D** · Group 1 Event 5)

```
<109>1 2026-08-12T08:00:00.123Z cci-lab-01 ccli-pki - IEC62351-14:1 [62351-14@41912 ID="IEC62351-9:1.5" MNEMONIC="CRED_ENR_EST_SUCC" Text="Enrolment using EST successfully performed" SqNum="42"]
```

| Component | Value |
|-----------|-------|
| **PRI** | `<109>` — notice · facility 13 |
| **VERSION** | `1` |
| **TIMESTAMP** | UTC RFC 5424 |
| **HOSTNAME** | `cci-lab-01` (Eth_mgmt or FQDN) |
| **APP-NAME** | `ccli-pki` |
| **PROCID** | `-` (NIL — ignored by collectors) |
| **MSGID** | `IEC62351-14:1` |
| **SD-ELEMENT** | `62351-14@41912` |

**OpenWrt sketch:** `logread` → `syslog-ng` / `rsyslog` forward **`@@collector:6514`** with **62351-3** TLS profile.

### Abstract fields (Table 2 reference)

| Field | TG-524 example |
|-------|----------------|
| **Timestamp** | `2026-08-12T08:00:00.123Z` |
| **SqNum** | monotonic per device |
| **ID** | `IEC62351-9:1.5` |
| **MNEMONIC** | `CRED_ENR_EST_SUCC` |
| **Severity** | `notice` |
| **Text** | `Enrolment using EST successfully performed` |
| **Source** | CCI hostname or Eth_mgmt IP |
| **App-Name** | `ccli-pki` |

---

## Annex B.1.6 — IEC 62351-9 cybersecurity event logs (pp. 79–81)

Normative **62351-14** catalogue for **`ccli-62351-9-extract` Annex D**. Cross-validated **PASS** — mnemonics and severities align; ID format here is **`IEC62351-9:G.N`** (no space; Annex D may show `IEC 62351-9:`).

**Rules (B.1.6 intro):**

| Rule | Value |
|------|-------|
| **IECVersion** | **`1`** for all 62351-9 events in this annex |
| **LogType** | **`IEC62351-14`** (Table 1) |
| **ExtraInfo / X.Label** | Optional — only if underlying platform exposes data |

### Event groups

| Group | Domain suffix | Topic | CCLI |
|-------|---------------|-------|------|
| **1** | `:1.x` | Credential transport · PKCS#12/8 · SCEP · EST enrolment | **K6.3 P0** |
| **2** | `:2.x` | Public-key certificate verification | **C1 TLS/E2E P0** |
| **3** | `:3.x` | Attribute certificate verification | **62351-8 RBAC P1** |
| **4** | `:4.x` | CRL / OCSP revocation status | **K6.3 operate P0** |
| **5** | `:5.x` | GDOI / IKEv1 group key | **P2** (not C1 MMS) |

### Table 33 — Credential transport and enrolment (Group 1, p. 79)

| MNEMONIC | Severity | ID | Text | ExtraInfo |
|----------|----------|-----|------|-----------|
| `CRED_PKCS12_FORMAT` | warning | IEC62351-9:1.1 | PKCS #12 format mismatch. | — |
| `CRED_PKCS8_FORMAT` | warning | IEC62351-9:1.2 | PKCS #8 format mismatch. | — |
| `CRED_ENR_SCEP_SUCC` | notice | IEC62351-9:1.3 | Enrolment using SCEP successfully performed. | Issued public-key certificate. |
| `CRED_ENR_SCEP_FAIL` | error | IEC62351-9:1.4 | Enrolment using SCEP aborted due to SCEP failure. | — |
| `CRED_ENR_EST_SUCC` | notice | IEC62351-9:1.5 | Enrolment using EST successfully performed. | Issued public-key certificate. |
| `CRED_ENR_EST_FAIL` | error | IEC62351-9:1.6 | Enrolment using EST aborted due to SCEP failure. | — |

*Note: `CRED_ENR_EST_FAIL` Text references SCEP in CDV capture — treat as **EST** failure in implementation.*

### Table 34 — Public-key certificate verification (Group 2, p. 80)

| MNEMONIC | Severity | ID | Text |
|----------|----------|-----|------|
| `CERT_V_FORMAT` | warning | IEC62351-9:2.1 | Certificate format mismatch. Failed verification. |
| `CERT_V_PK_SIG_WRONG` | **alarm** | IEC62351-9:2.2 | Public-key certificate signature could not be verified. |
| `CERT_V_PK_VERSION` | error | IEC62351-9:2.3 | Wrong public-key certificate version. |
| `CERT_V_PC_ISSUER` | error | IEC62351-9:2.4 | Public-key certificate issuer not trusted. |
| `CERT_PK_SIG_ALG` | error | IEC62351-9:2.5 | Unsupported signature algorithm. |
| `CERT_V_PK_EXPIRED` | error | IEC62351-9:2.6 | Public-key certificate expired. |
| `CERT_V_PK_EARLY` | error | IEC62351-9:2.7 | Public-key certificate not valid yet. |
| `CERT_V_PK_SUBJECT` | warning | IEC62351-9:2.8 | Subject not included in public-key certificate. |
| `CERT_V_PK_ALG_MISMATCH` | error | IEC62351-9:2.9 | Native public key algorithm in public-key certificate not supported. |
| `CERT_V_PK_TA` | error | IEC62351-9:2.10 | Trust anchor not supported. |
| `CERT_V_PK_PATH` | error | IEC62351-9:2.11 | Certification path could not be verified. |
| `CERT_V_PKCE_CEXT` | error | IEC62351-9:2.12 | Unknown critical extension in public-key certificate. |
| `CERT_V_PKCE_EXT_VALUE` | error | IEC62351-9:2.13 | Unknown information in critical extension. |
| `CERT_V_PK_AKID` | error | IEC62351-9:2.14 | Authority key identifier not included in public-key certificate. |
| `CERT_V_PKCE_SKID` | error | IEC62351-9:2.15 | Subject key identifier not included in public-key certificate. |
| `CERT_V_PKCE_SAN` | error | IEC62351-9:2.16 | Subject alternative Name not included in TLS server certificates. |
| `CERT_V_PKCE_BC` | error | IEC62351-9:2.17 | Basic constraints not included in CA certificate. |
| `CERT_V_PKCE_PL` | error | IEC62351-9:2.18 | Path len constraints in CA certificate violated. |
| `CERT_KU_DIGSIG` | error | IEC62351-9:2.19 | Key usage for digital signatures not included in TLS certificate. |
| `CERT_PKCE_KU` | error | IEC62351-9:2.20 | Key usage for key encipherment not included when TLS cipher used with key encryption is used. |
| `CERT_PKCE_KU_KCS` | error | IEC62351-9:2.21 | Key usage not included for signing certificates. |
| `CERT_PKCE_KU_CRLS` | error | IEC62351-9:2.22 | Key usage not included for signing CRLs in CRL signature certificate. |
| `CERT_PKCE_EKU_TLS_SA` | error | IEC62351-9:2.23 | Extended Key Usage for server authentication not included in TLS server certificate. |
| `CERT_PKCE_EKU_TLS_CA` | error | IEC62351-9:2.24 | Extended Key Usage for client authentication not included in TLS client certificate. |
| `CERT_PKCE_EKU_OCSPS` | error | IEC62351-9:2.25 | Extended Key Usage not included for OCSP signing in OCSP responder certificate. |
| `CERT_V_PKCE_NCDP` | warning | IEC62351-9:2.26 | CRL distribution point not contained in public-key certificate. |
| `CERT_V_PKCE_NOCSP` | warning | IEC62351-9:2.27 | OCSP responder information not contained in public-key certificate. |

**C1-critical subset:** **2.2** (alarm) · **2.6** · **2.10** · **2.11** · **2.19** · **2.23** · **2.24** · **2.27**.

### Table 35 — Attribute certificate verification (Group 3, p. 81)

| MNEMONIC | Severity | ID | Text |
|----------|----------|-----|------|
| `CERT_V_FORMAT` | warning | IEC62351-9:3.1 | Certificate format mismatch. Failed verification. |
| `CERT_V_AC_SIG_WRONG` | **alarm** | IEC62351-9:3.2 | Public-key certificate signature could not be verified. |
| `CERT_V_AC_VERSION` | error | IEC62351-9:3.3 | Wrong attribute certificate version. |
| `CERT_V_AC_HOLDER_PK` | warning | IEC62351-9:3.4 | Holder of attribute certificate could not be verified based on public-key certificate |
| `CERT_V_AC_HOLDER_NPK` | warning | IEC62351-9:3.5 | Holder of attribute certificate could not be verified based on alternative authentication. |
| `CERT_V_AC_ISSUER_CONFIG` | error | IEC62351-9:3.6 | Attribute certificate issuer not trusted (not configured). |
| `CERT_V_AC_ISSUER_TCA` | error | IEC62351-9:3.7 | Attribute certificate issuer not trusted (by a trusted CA). |
| `CERT_V_AC_EXPIRED` | error | IEC62351-9:3.8 | Attribute certificate expired. |
| `CERT_V_AC_EARLY` | error | IEC62351-9:3.9 | Attribute certificate not yet valid. |
| `CERT_V_ACE_EXT` | error | IEC62351-9:3.10 | unknown critical extension in attribute certificate. |
| `CERT_V_ACE_EXT_VALUE` | error | IEC62351-9:3.11 | unrecognized information in critical extension of attribute certificate. |
| `CERT_V_ACCE_EXT` | warning | IEC62351-9:3.12 | unknown extension in attribute certificate. |
| `CERT_V_ACE_AKID` | error | IEC62351-9:3.13 | authority key identifier in attribute certificate could not be verified. |
| `CERT_V_ACE_NRII` | notice | IEC62351-9:3.14 | no revocation information intended in attribute certificate. |
| `CERT_V_ACE_NCDP` | warning | IEC62351-9:3.15 | CRL distribution point not contained in attribute certificate. |
| `CERT_V_ACE_NOCSP` | warning | IEC62351-9:3.16 | OCSP responder information not contained in attribute certificate. |

*Groups 2 and 3 share mnemonic `CERT_V_FORMAT` — disambiguate by **ID** group digit.*

### Table 36 — Certificate revocation status (Group 4, p. 81)

| MNEMONIC | Severity | ID | Text |
|----------|----------|-----|------|
| `CERT_V_REVOKED` | **alarm** | IEC62351-9:4.1 | Certificate has been revoked. |
| `CERT_V_R_NR_CRL` | warning | IEC62351-9:4.2 | CRL distribution point not accessible. |
| `CERT_V_CRL_EXP` | warning | IEC62351-9:4.3 | CRL expired. |
| `CERT_V_CRL_SIG_FAIL` | warning | IEC62351-9:4.4 | CRL signature verification failed. |
| `CERT_V_R_NR_OCSP` | warning | IEC62351-9:4.5 | OCSP responder not accessible. |
| `CERT_V_R_TO_OCSP` | warning | IEC62351-9:4.6 | OCSP responder connection timeout. |
| `CERT_V_R_CU_OCSP` | warning | IEC62351-9:4.7 | Certificate not known to OCSP responder. |
| `CERT_V_R_OCSP_EXP` | warning | IEC62351-9:4.8 | OCSP response expired. |
| `CERT_V_OCSP_SIG_FAIL` | warning | IEC62351-9:4.9 | OCSP response signature verification failed. |

**K6.3 operate:** **4.1** = immediate alarm · **4.5–4.9** = OCSP path · **4.2–4.4** = CRL path.

### Table 37 — GDOI cyber security events (Group 5, p. 81) — **P2**

| MNEMONIC | Severity | ID | Text |
|----------|----------|-----|------|
| `IKEv1_CALG_MISMATCH` | error | IEC62351-9:5.1 | Proposal of unsupported cryptographic algorithms. |
| `IKEv1_PADDING` | error | IEC62351-9:5.2 | Padding error during decryption of IKEv1 message. |
| `GROUP_PULL_EXPIRED_SA` | warning | IEC62351-9:5.3 | Use of Expired SA Requested. |
| `GROUP_PULL_PHASE2_DECRYPTION` | warning | IEC62351-9:5.4 | Unable to Decrypt Phase 2 message. |
| `GROUP_PULL_PHASE2_GROUP_NONEXISTENT` | warning | IEC62351-9:5.5 | Key for unknown group requested. |
| `GROUP_PULL_PHASE1_KEY_NEGOTIATION` | notice | IEC62351-9:5.6 | Unable to negotiate GDOI PHASE 1. |
| `GROUP_PUSH_UNREACHABLE_DESTINATION` | error | IEC62351-9:5.7 | Attempted to send to unreachable group member. |
| `KDA_FAILURE` | notice | IEC62351-9:5.8 | KDA cannot be provided to publisher. |

### K6.3 phase → event map

| Ceremony phase | Primary events (Group) |
|----------------|------------------------|
| Credential import | 1.1 · 1.2 |
| EST/SCEP enrol | **1.5** / **1.6** · 1.3 / 1.4 |
| TLS connect (62351-3) | 2.x verification failures |
| E2E cert check (62351-4 G) | 2.x on Cert B |
| Revocation monitor | **4.1** alarm · 4.2–4.9 warnings |
| Renewal | 1.5 (re-enrol success) |

### PRI quick reference (Group 1–5)

| Severity | PRI |
|----------|-----|
| **alarm** | `<105>` |
| **error** | `<107>` |
| **warning** | `<108>` |
| **notice** | `<109>` |

---

## Annex B.1.1 — IEC 62351-3 TLS event logs (pp. 58–60)

Domain **`IEC62351-3`**. **IECVersion = 1**. Event groups: **1** = TLS handshake · **2** = TLS certificate validation.

### Table 10 — TLS handshake (Group 1, pp. 58–59)

| MNEMONIC | Severity | ID | Text (summary) | ExtraInfo |
|----------|----------|-----|----------------|-----------|
| `TLS_HS_SUCCESS` | notice | IEC62351-3:1.1 | TLS session successfully established | — |
| `TLS_DEPRECATED_VERSION` | alarm | IEC62351-3:1.2 | Deprecated TLS version; pre-1.2 disabled | TLS version |
| `TLS_WEAK_VERSION` | warning | IEC62351-3:1.3 | Deprecated TLS version; pre-1.2 enabled | TLS version |
| `TLS_DISALLOWED_VERSION` | alarm | IEC62351-3:1.4 | Disallowed TLS version (prior to 1.0) | TLS version |
| `TLS_VERSION_CHANGE` | alarm | IEC62351-3:1.5 | TLS version change (potential downgrade) | TLS version |
| `TLS_NO_PEER_CERT` | alarm | IEC62351-3:1.6 | Peer did not provide endpoint certificate | — |
| `TLS_NO_LOCAL_CERT` | alarm | IEC62351-3:1.7 | Local endpoint certificate not available | — |
| `TLS_SESSION_CLOSED_REV` | alarm | IEC62351-3:1.8 | Session closed — revoked peer cert | Revocation reason |
| `TLS_DISALLOWED_CIPHER` | warning | IEC62351-3:1.9 | Disallowed cipher suite proposed | Cipher suite |
| `TLS_SESSIONID_EXPIRED_FULL_HS` | warning | IEC62351-3:1.10 | SessionID expired; full handshake | — |
| `TLS_NO_RENEG` | alarm | IEC62351-3:1.11 | Renegotiation interval exceeded (TLS 1.2) | Reneg end time |
| `TLS_NO_RENEG_SIG` | warning | IEC62351-3:1.12 | TLS 1.2: no secure renegotiation in initial HS | — |
| `TLS_NO_RENEG_TICKET` | alarm | IEC62351-3:1.13 | TLS 1.2: no session tickets in renegotiation | — |
| `TLS_NO_TR_CA_MATCH_S` | alarm | IEC62351-3:1.14 | TLS 1.2: no server support of client trusted CA | Client trusted CA |
| `TLS_NO_SIG_ALGO_EXT` | warning | IEC62351-3:1.15 | TLS 1.2: no signature-algorithms extension | — |
| `TLS_DEP_SIG_ALGO` | warning | IEC62351-3:1.16 | TLS 1.2: deprecated signature algorithm | Cipher suite |
| `TLS_NO_EPSK_MODE` | alarm | IEC62351-3:1.17 | TLS 1.2: no encrypt-then-MAC | — |
| `TLS_NO_ENCRYPT_THEN_MAC` | warning | IEC62351-3:1.18 | TLS 1.3: non-ephemeral PSK mode only | — |
| `TLS_NO_SK_UPDATE` | alarm | IEC62351-3:1.19 | TLS 1.3: peer does not perform key update | — |
| `TLS_NO_TR_CA_MATCH_SC` | alarm | IEC62351-3:1.20 | TLS 1.3: no support of peer trusted CA | Trusted CA |
| `TLS_EARLY_DATA` | warning | IEC62351-3:1.21 | TLS 1.3: early data neglected | — |

*CDV note: rows **1.17/1.18** mnemonics may be swapped vs event description in capture — verify against FDIS.*

### Table 11 — TLS certificate (Group 2, p. 59)

| MNEMONIC | Severity | ID | Text (summary) |
|----------|----------|-----|----------------|
| `TLS_CERT_SIZE_MISMATCH` | alarm | IEC62351-3:2.1 | Certificate size exceeds limits |
| `TLS_NO_CA_MATCH` | alarm | IEC62351-3:2.2 | Root CA for validation not available |
| `TLS_NO_TRUSTED_CERT_MATCH` | alarm | IEC62351-3:2.3 | Endpoint cert not in trust list |
| `TLS_CERT_REVOKED` | alarm | IEC62351-3:2.4 | Endpoint certificate revoked |
| `TLS_NO_CRL` | warning | IEC62351-3:2.5 | Local CRL not accessible |
| `TLS_CRL_EXP` | warning | IEC62351-3:2.6 | CRL expired |
| `TLS_OCSP_RES_EXP` | warning | IEC62351-3:2.7 | OCSP response expired |
| `TLS_CERT_EXP` | alarm | IEC62351-3:2.8 | Endpoint certificate expired |
| `TLS_SIG_ALG_MISMATCH` | alarm | IEC62351-3:2.9 | Signature algorithm not supported |
| `TLS_SIG_V_FAILED` | alarm | IEC62351-3:2.10 | Certificate signature verification failed |
| `TLS_SHORT_RSA_KEY` | alarm | IEC62351-3:2.11 | RSA key < 2048 bit |
| `TLS_MIN_KEY` | warning | IEC62351-3:2.12 | RSA 1024 bit allowed by config |
| `TLS_SHORT_KEY` | alarm | IEC62351-3:2.13 | RSA key < 1024 bit *(CDV ID typo `62391`)* |
| `TLS_DEP_HASH` | warning | IEC62351-3:2.14 | Deprecated hash algorithm |

### CRLReason ExtraInfo (p. 60)

For **`TLS_CERT_REVOKED`** / **`TLS_SESSION_CLOSED_REV`**: revocation reason from RFC 5280 **`CRLReason`** ENUMERATED:

`unspecified(0)` · `keyCompromise(1)` · `cACompromise(2)` · `affiliationChanged(3)` · `superseded(4)` · `cessationOfOperation(5)` · `certificateHold(6)` · `removeFromCRL(8)` · `privilegeWithdrawn(9)` · `aACompromise(10)` · `weakAlgorithmOrKey(11)`

### C1 TLS critical subset (TG-524 Eth_A :3782)

| Event | ID | When |
|-------|-----|------|
| `TLS_HS_SUCCESS` | 1.1 | MMS/TLS session up |
| `TLS_CERT_REVOKED` | 2.4 | K6.3 revocation path |
| `TLS_CERT_EXP` | 2.8 | Renewal trigger |
| `TLS_NO_PEER_CERT` | 1.6 | Client/server auth fail |
| `TLS_SIG_V_FAILED` | 2.10 | Chain/signature fail |
| `TLS_DEPRECATED_VERSION` | 1.2 | Policy reject TLS < 1.2 |

---

## Annex B.1.2 — IEC 62351-4 E2E event logs (pp. 61–73)

Domain **`IEC62351-4`**. **IECVersion = 1**.

### Event groups (p. 61)

| Group | E2E PDU / scope | Role |
|-------|-----------------|------|
| **1** | **HandshakeReq** | Server |
| **2** | **HandshakeAcc** | Client |
| **3** | **ApplicationReject** | Client |
| **4** | **HandshakeSecReject** | Client |
| **5** | **HandshakeSecAbort** | Server |
| **6** | **DtSecAbort** | Client or server |
| **7** | **ApplAbort** | Client or server |
| **10** | **ClearTransfer** | Either |
| **11** | **EncrTransfer** | Either |
| **12** | **ReleaseReq** | Either |
| **13** | **ReleaseRsp** | Either |
| **15** | **OSI** operational (ACSE/AARQ/AARE) | — |
| **18** | **XMPP** operational | — |

*Groups **8–9,14,16–17** not used in this CDV annex.*

### Table 12 — HandshakeReq (Group 1, pp. 62–63) — **22 events**

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `SIGNATURE_ALGO_NOT_SUP_ASS_REQ` | error | IEC62351-4:1.1 |
| `SIGNATURE_ALGO_MISMATCH_REQ` | alarm | IEC62351-4:1.2 |
| `INV_SIGNATURE_ASS_REQ` | alarm | IEC62351-4:1.3 |
| `PROTECTED_PROT_NOT_SUP_REQ` | error | IEC62351-4:1.4 |
| `PROTOCOL_ERR_ASS_REQ` | error | IEC62351-4:1.5 |
| `PKCERT_ERROR_ASS_REQ` | error | IEC62351-4:1.6 |
| `PKCERT_ALARM_ASS_REQ` | alarm | IEC62351-4:1.7 |
| `ADDR_MISMATCH_ASS_REQ` | alarm | IEC62351-4:1.8 |
| `UNEXP_VERSION_ASS_REQ` | error | IEC62351-4:1.9 |
| `INV_TIME_ASS_REQ` | error | IEC62351-4:1.10 |
| `REPLAY_DETEC_ASS_REQ` | alarm | IEC62351-4:1.11 |
| `UNSUP_DH_GROUP_ASS_REQ` | error | IEC62351-4:1.12 |
| `HMAC_ALGO_NOT_SUP_ASS_REQ` | error | IEC62351-4:1.13 |
| `AEAD_NOT_SUP_ASS_REQ` | error | IEC62351-4:1.14 |
| `AEAD_WHEN_NO_ENCR_ASS_REQ` | error | IEC62351-4:1.15 |
| `AE_ALGO_NOT_SUP_ASS_REQ` | error | IEC62351-4:1.16 |
| `AE_IS_REQUIRED_ASS_REQ` | error | IEC62351-4:1.17 |
| `ENCR_NOT_REQ_ASS_REQ` | error | IEC62351-4:1.18 |
| `ENCR_ALGO_NOT_SUP_ASS_REQ` | error | IEC62351-4:1.19 |
| `ICV_ALGO_NOT_SUP_ASS_REQ` | error | IEC62351-4:1.20 |
| `ENCR_NOT_REQUIRED_ASS_REQ` | error | IEC62351-4:1.21 |
| `ENCR_IS_REQUIRED_ASS_REQ` | error | IEC62351-4:1.22 |

**C1 alarms:** **1.2** · **1.3** · **1.7** · **1.8** · **1.11**

### Table 13 — HandshakeAcc (Group 2, pp. 63–64) — **21 events**

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `SIGNATURE_ALGO_NOT_SUP_ASS_ACC` | error | IEC62351-4:2.1 |
| `SIG_ALGO_MISMATCH_ASS_ACC` | alarm | IEC62351-4:2.2 |
| `INV_SIGNATURE_ASS_ACC` | alarm | IEC62351-4:2.3 |
| `PROTOCOL_ERR_ASS_ACC` | error | IEC62351-4:2.4 |
| `PKCERT_ERROR_ASS_ACC` | error | IEC62351-4:2.5 |
| `PKCERT_ALARM_ASS_ACC` | alarm | IEC62351-4:2.6 |
| `ADDR_MISMATCH_ASS_ACC` | alarm | IEC62351-4:2.7 |
| `UNEXP_VERSION_ASS_ACC` | error | IEC62351-4:2.8 |
| `INV_TIME_ASS_ACC` | error | IEC62351-4:2.9 |
| `REPLAY_DETEC_ASS_ACC` | alarm | IEC62351-4:2.10 |
| `INV_DH_GROUP_ASS_ACC` | error | IEC62351-4:2.11 |
| `HMAC_ALGO_NOT_SUP_ASS_ACC` | error | IEC62351-4:2.12 |
| `INV_AE_ALGO_ASS_ACC` | error | IEC62351-4:2.13 |
| `SINGLE_AE_ALGO_REQ_ASS_ACC` | error | IEC62351-4:2.14 |
| `AEAD_NOT_USED_ASS_ACC` | error | IEC62351-4:2.15 |
| `INV_ENCR_ALGO_ASS_ACC` | error | IEC62351-4:2.16 |
| `SINGLE_ENCR_ALGO_ASS_ACC` | error | IEC62351-4:2.17 |
| `INV_ICV_ALGO_ASS_ACC` | error | IEC62351-4:2.18 |
| `SINGLE_AE_ALGO_ASS_ACC` | error | IEC62351-4:2.19 |
| `ENCR_NOT_REQUIRED_ASS_ACC` | error | IEC62351-4:2.20 |
| `ENCR_IS_REQUIRED_ASS_ACC` | error | IEC62351-4:2.21 |

### Table 14 — ApplicationReject (Group 3, p. 64) — **10 events**

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `SIGNATURE_ALGO_NOT_SUP_ASS_REJ` | error | IEC62351-4:3.1 |
| `SIG_ALGO_MISMATCH_APPL_REJECT` | alarm | IEC62351-4:3.2 |
| `INV_SIG_APPL_REJECT` | alarm | IEC62351-4:3.3 |
| `PROTOCOL_ERR_APPL_REJECT` | error | IEC62351-4:3.4 |
| `PKCERT_ERROR_APPL_REJECT` | error | IEC62351-4:3.5 |
| `PKCERT_ALARM_APPL_REJECT` | alarm | IEC62351-4:3.6 |
| `ADDR_MISMATCH_APPL_REJECT` | alarm | IEC62351-4:3.7 |
| `UNEXP_VERSION_APPL_REJECT` | error | IEC62351-4:3.8 |
| `INV_TIME_APPL_REJECT` | error | IEC62351-4:3.9 |
| `REPLAY_DETEC_APPL_REJECT` | alarm | IEC62351-4:3.10 |

### Table 15 — HandshakeSecReject (Group 4, pp. 65–66) — **27 events**

Base events **4.1–4.11** (signature, protocol, cert, replay, alarm) + diagnostic **`_SEC_REJ_DIA`** events **4.12–4.27** mapping HandshakeSecReject diagnostic codes (invalid-signatureAlgorithm, protected-protocol-not-supported, protocol-error, unexpected-version, invalid-time-value, dhGroup-not-supported, hmac/ae/encrypt/icv algorithm codes, encryption-required/not-required, etc.).

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `SIG_ALGO_NOT_SUP_ASS_SEC_REJ` | error | IEC62351-4:4.1 |
| `SIG_ALGO_MISMATCH_ASS_SEC_REJ` | alarm | IEC62351-4:4.2 |
| `INV_SIG_ASS_SEC_REJ` | alarm | IEC62351-4:4.3 |
| `PROTOCOL_ERR_ASS_SEC_REJ` | error | IEC62351-4:4.4 |
| `PKCERT_ERROR_ASS_SEC_REJ` | error | IEC62351-4:4.5 |
| `PKCERT_ALARM_ASS_SEC_REJ` | alarm | IEC62351-4:4.6 |
| `ADDR_MISMATCH_ASS_SEC_REJ` | alarm | IEC62351-4:4.7 |
| `UNEXP_VERSION_ASS_SEC_REJ` | error | IEC62351-4:4.8 |
| `INV_TIME_ASS_SEC_REJ` | error | IEC62351-4:4.9 |
| `REPLAY_DETEC_ASS_SEC_REJ` | alarm | IEC62351-4:4.10 |
| `ALARM_RCV_ASS_SEC_REJ` | alarm | IEC62351-4:4.11 |
| `SIG_ALGO_NOT_SUP_SEC_REJ_DIA` | error | IEC62351-4:4.12 |
| `PROTEC_PROT_NOT_SUP_SEC_REJ_DIA` | error | IEC62351-4:4.13 |
| `PROTOCOL_ERR_SEC_REJ_DIA` | error | IEC62351-4:4.14 |
| `UNEXP_VERSION_SEC_REJ_DIA` | error | IEC62351-4:4.15 |
| `INV_TIME_SEC_REJ_DIA` | error | IEC62351-4:4.16 |
| `UNSUP_DH_GROUP_SEC_REJ_DIA` | error | IEC62351-4:4.17 |
| `HMAC_ALGO_NOT_SUP_SEC_REJ_DIA` | error | IEC62351-4:4.18 |
| `AEAD_NOT_SUP_SEC_REJ_DIA` | error | IEC62351-4:4.19 |
| `AEAD_WHEN_NO_ENCR_SEC_REJ_DIA` | error | IEC62351-4:4.20 |
| `AE_ALGO_NOT_SUP_SEC_REJ_DIA` | error | IEC62351-4:4.21 |
| `AE_IS_REQUIRED_SEC_REJ_DIA` | error | IEC62351-4:4.22 |
| `ENCR_NOT_REQ_SEC_REJ_DIA` | error | IEC62351-4:4.23 |
| `ENCR_ALGO_NOT_SUP_SEC_REJ_DIA` | error | IEC62351-4:4.24 |
| `ICV_ALGO_NOT_SUP_SEC_REJ_DIA` | error | IEC62351-4:4.25 |
| `ENCR_NOT_REQUIRED_SEC_REJ_DIA` | error | IEC62351-4:4.26 |
| `ENCR_IS_REQUIRED_SEC_REJ_DIA` | error | IEC62351-4:4.27 |

### Table 16 — HandshakeSecAbort (Group 5, pp. 66–67) — **25 events**

Events **5.1–5.11** (base) + **`_SEC_ABT_DIA`** **5.12–5.25** (diagnostic codes: invalid-signatureAlgorithms, protocol-error, unexpected-version, invalid-time-value, illegal-dhGroup, invalid-ae-algorithm, single-ae/encrypt/icv-algo-required, encryption-not-required/is-required).

### Table 17 — DtSecAbort (Group 6, pp. 67–68) — **16 events**

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `SIG_ALGO_NOT_SUP_DTSEC_ABT` | error | IEC62351-4:6.1 |
| `SIG_ALGO_MISMATCH_DTSEC_ABT` | alarm | IEC62351-4:6.2 |
| `INV_SIG_DTSEC_ABT` | alarm | IEC62351-4:6.3 |
| `PROTOCOL_ERR_DTSEC_ABT` | error | IEC62351-4:6.4 |
| `PKCERT_ERROR_DTSEC_ABT` | error | IEC62351-4:6.5 |
| `PKCERT_ALARM_DTSEC_ABT` | alarm | IEC62351-4:6.6 |
| `ADDR_MISMATCH_DTSEC_ABT` | alarm | IEC62351-4:6.7 |
| `UNEXP_VERSION_DTSEC_ABT` | error | IEC62351-4:6.8 |
| `INV_TIME_DTSEC_ABT` | error | IEC62351-4:6.9 |
| `REPLAY_DETEC_DTSEC_ABT` | alarm | IEC62351-4:6.10 |
| `ALARM_DTSEC_ABT` | alarm | IEC62351-4:6.11 |
| `PROT_ERR_DTSEC_ABT_DIA` | error | IEC62351-4:6.12 |
| `ENCR_NOT_SEL_DTSEC_ABT_DIA` | error | IEC62351-4:6.13 |
| `ENCR_REQ_DTSEC_ABT_DIA` | error | IEC62351-4:6.14 |
| `UNEXP_RE_KEY_DTSEC_ABT_DIA` | error | IEC62351-4:6.15 |
| `UNEXP_CHG_KEYS_DTSEC_ABT_DIA` | error | IEC62351-4:6.16 |

### Table 18 — ApplAbort (Group 7, p. 68) — **10 events**

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `SIGNATURE_ALGO_NOT_SUP_APPL_ABT` | error | IEC62351-4:7.1 |
| `SIG_ALGO_MISMATCH_APPL_ABT` | alarm | IEC62351-4:7.2 |
| `INV_SIG_APPL_ABT` | alarm | IEC62351-4:7.3 |
| `PROTOCOL_ERR_APPL_ABT` | error | IEC62351-4:7.4 |
| `PKCERT_ERROR_APPL_ABT` | error | IEC62351-4:7.5 |
| `PKCERT_ALARM_APPL_ABT` | alarm | IEC62351-4:7.6 |
| `ADDR_MISMATCH_APPL_ABT` | alarm | IEC62351-4:7.7 |
| `UNEXP_VERSION_APPL_ABT` | error | IEC62351-4:7.8 |
| `INV_TIME_APPL_ABT` | error | IEC62351-4:7.9 |
| `REPLAY_DETEC_APPL_ABT` | alarm | IEC62351-4:7.10 |

### Table 19 — ClearTransfer (Group 10, p. 69) — **10 events**

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `CLEAR_DATA_PROT_ERROR_TRF` | error | IEC62351-4:10.1 |
| `ENCR_WAS_SEL_TRF` | error | IEC62351-4:10.2 |
| `CLRAR_INV_ICV_ALG_TRF` | alarm | IEC62351-4:10.3 |
| `CLRAR_GMAC_NONCE_REQ` | error | IEC62351-4:10.4 |
| `CLEAR_BAD_ICV` | alarm | IEC62351-4:10.5 |
| `CLEAR_DATA_INV_TIME_TRF` | error | IEC62351-4:10.6 |
| `CLEAR_REPLAY_DETECTED` | alarm | IEC62351-4:10.7 |
| `CLEAR_INV_SEQ_NR_TRF` | error | IEC62351-4:10.8 |
| `CLEAR_UNEXP_RE_KEY_REQ` | error | IEC62351-4:10.9 |
| `CLEAR_UNEXP_CHG_KEYS_IND` | error | IEC62351-4:10.10 |

### Table 20 — EncrTransfer (Group 11, p. 70) — **11 events**

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `ENCR_DATA_PROT_ERROR_TRF` | error | IEC62351-4:11.1 |
| `ENCR_NOT_SEL_TRF` | error | IEC62351-4:11.2 |
| `ENCR_INV_ICV_ALG_TRF` | alarm | IEC62351-4:11.3 |
| `ENCR_GMAC_NONCE_REQ` | error | IEC62351-4:11.4 |
| `ENCR_BAD_ICV` | alarm | IEC62351-4:11.5 |
| `ENCR_DATA_INV_TIME_TRF` | error | IEC62351-4:11.6 |
| `ENCR_DATA_REPLAY_DETECTED` | alarm | IEC62351-4:11.7 |
| `ENCR_INV_SEQ_NR_TRF` | error | IEC62351-4:11.8 |
| `AES_IV_REQ` | error | IEC62351-4:11.9 |
| `ENCR_UNEXP_RE_KEY_REQ` | error | IEC62351-4:11.10 |
| `ENCR_UNEXP_CHG_KEYS_IND` | error | IEC62351-4:11.11 |

### Table 21 — ReleaseReq (Group 12, p. 71) — **10 events**

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `SIGNATURE_ALGO_NOT_SUP_REL_REQ` | error | IEC62351-4:12.1 |
| `SIG_ALGO_MISMATCH_REL_REQ` | alarm | IEC62351-4:12.2 |
| `INV_SIG_REL_REQ` | alarm | IEC62351-4:12.3 |
| `PROTOCOL_ERR_REL_REQ` | error | IEC62351-4:12.4 |
| `PKCERT_ERROR_REL_REQ` | error | IEC62351-4:12.5 |
| `PKCERT_ALARM_REL_REQ` | alarm | IEC62351-4:12.6 |
| `ADDR_MISMATCH_REL_REQ` | alarm | IEC62351-4:12.7 |
| `UNEXP_VERSION_REL_REQ` | error | IEC62351-4:12.8 |
| `INV_TIME_REL_REQ` | error | IEC62351-4:12.9 |
| `REPLAY_DETECT_REL_REQ` | alarm | IEC62351-4:12.10 |

### Table 22 — ReleaseRsp (Group 13, p. 71) — **10 events**

Same pattern as Table 21 with `_REL_RSP` suffix · IDs **13.1–13.10**.

### Table 23 — OSI operational environment (Group 15, p. 72) — **16 events**

| MNEMONIC | Sev | ID | Text (summary) |
|----------|-----|-----|----------------|
| `OSI_ENV_PROT_ERR` | error | IEC62351-4:15.1 | OSI operational environment protocol error |
| `OSI_INV_INDR_REF` | error | IEC62351-4:15.2 | Invalid indirect reference |
| `OSI_INV_PCI` | error | IEC62351-4:15.3 | Invalid PCI security |
| `ACSE_PROT_ERR` | error | IEC62351-4:15.4 | ACSE protocol error |
| `INV_APPL_CNTX_RQ` | error | IEC62351-4:15.5 | Unsupported application context in **AARQ** |
| `INV_APPL_CNTX_RE` | error | IEC62351-4:15.6 | Unsupported application context in **AARE** |
| `INV_CALLING_AP_TITLE_RQ` | error | IEC62351-4:15.7 | Invalid calling AP-title in **AARQ** |
| `INV_CALLING_AP_TITLE_RE` | error | IEC62351-4:15.8 | Invalid calling AP-title in **AARE** |
| `INV_CALLING_AE_QUAL_RQ` | error | IEC62351-4:15.9 | Invalid calling AE-qualifier in **AARQ** |
| `INV_CALLING_AE_QUAL_RE` | error | IEC62351-4:15.10 | Invalid calling AE-qualifier in **AARE** |
| `INV_CALLED_AP_TITLE_RQ` | error | IEC62351-4:15.11 | Invalid called AP-title in **AARQ** |
| `INV_CALLED_AP_TITLE_RE` | error | IEC62351-4:15.12 | Invalid called AP-title in **AARE** |
| `INV_CALLED_AE_QUAL_RQ` | error | IEC62351-4:15.13 | Invalid called AE-qualifier in **AARQ** |
| `INV_CALLED_AE_QUAL_RE` | error | IEC62351-4:15.14 | Invalid called AE-qualifier in **AARE** |
| `AUTH_MECH_NOT_USED_RQ` | error | IEC62351-4:15.15 | mechanism-name not allowed in **AARQ** |
| `AUTH_MECH_NOT_USED_ABT` | error | IEC62351-4:15.16 | mechanism-name not allowed in **AARE** |

**C1 MMS binding:** AP-title / AE-qualifier mismatch events tie to **62351-4 Annex G** cert OIDs.

### Table 24 — XMPP operational environment (Group 18, p. 73) — **6 events**

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `XMPP_IQ_MISSING` | error | IEC62351-4:18.1 |
| `XMPP_MSG_MISSING` | error | IEC62351-4:18.2 |
| `XMPP_ENV_NS_INV` | error | IEC62351-4:18.3 |
| `XMPP_UNKNOW_IQ_SECPDU` | error | IEC62351-4:18.4 |
| `XMPP_UNKNOW_MSG_SECPDU` | error | IEC62351-4:18.5 |
| `XMPP_INVTYPE_SPEC` | error | IEC62351-4:18.6 |

*XMPP namespace: `http://www.iec.ch/62351/2018/ENV_4` — not C1 P0 (MMS/OSI path).*

### C1 protocol stack → event domains

```
Eth_A TLS (:3782)     → IEC62351-3:1.x / 2.x   (Tables 10–11)
MMS E2E handshake     → IEC62351-4:1.x – 7.x   (Tables 12–18)
MMS data transfer     → IEC62351-4:10.x – 11.x (Tables 19–20)
MMS release           → IEC62351-4:12.x – 13.x
ACSE/AARQ binding     → IEC62351-4:15.x        (Table 23)
PKI (parallel)        → IEC62351-9:1.x – 4.x   (Batch C)
```

**62351-4 event count:** **~194** (Tables 12–24). **62351-3:** **35** (Tables 10–11).

---


## §9–§11 Operations + conformance (pp. 46–48) — Batch E

### §9 Mapping SNMP to Syslog (p. 46)

| Rule | Detail |
|------|--------|
| Source | **62351-7** SNMP MIB notifications |
| Mapping | **RFC 5675** → Syslog **RFC 5424** |
| Protection | **RFC 5425** TLS on syslog transport |

### §10 Storage recommendations (p. 46)

| Rule | Detail |
|------|--------|
| Reference | **IEEE 1686** storage capability |
| Integrity | Log files **read-only** for users at collector |
| Risk | Recursive/volumetric logging → **resource starvation / DoS** — filter by severity |

### §11 Conformance — Table 8 (p. 46)

Two entity roles:

| Role | Example |
|------|---------|
| **Log generation entity** | Power-system device generating events |
| **Log collection entity** | Log server / SIEM collector |

| Entity | Syslog Table 2 attrs | XML | Raw storage |
|--------|---------------------|-----|-------------|
| **Log generation** | **Mandatory** | Optional | Optional |
| **Log collection** | **Mandatory** | Optional | **Mandatory** |

**Conformance rules (p. 46):**

| Rule | Detail |
|------|--------|
| Minimum | Both entity types shall support **all mandatory** Table 2 attributes when Syslog is used |
| Conditional | Optional / conditional Table 2 attributes only when the related informative feature exists and conditions are met |
| Custom attrs | Permitted only as **additional** **SD-ELEMENT** with **SD-PARAM** name/value pairs per **RFC 5424** §6.3 |
| Fail | Conformance **not** met if entity logs **only** custom attributes and omits mandatory Table 2 fields |

---

## Annex A — Generic cyber security events (pp. 47–56)

**Domain `IEC62351-14`**. Fixed per Table 2: **`LogType = IEC62351-14`**, **`IECVersion = 1`**. **`ID`** unique per event; allocated per **§4.4**.

Generic events useful for power-system cyber security; independent of part-specific events in **62351-3/5/6/7/8**. Listed in **Table 9**.

**Sources (normative lineage, p. 47):**

| Source | Reference |
|--------|-----------|
| IEEE | **1686:2013** |
| NERC | **CIP-007-6** |
| IEC | **62443-4-2** |
| IETF | **RFC 5425** (TLS syslog) |
| ISO | **27001** |
| Process | IEC **TC57 WG15** discussions |

Also cited: **NERC-CIP** (general), **IETF** (general).

### Implementation policy (p. 47)

| Rule | Detail |
|------|--------|
| Feature-gated | Events **not mandatory in all cases** — support only when the related feature exists (e.g. no login feature → login events not required) |
| Redundancy | If entity already logs equivalent stimuli, end user may **administratively deactivate** redundant Table 9 entries |
| Response | Actions on logged events governed by **operator security policy**; preventative measures per **target environment** policy |
| Safety | Security-event handling may interrupt control/measurement comms — consider **business continuity** without jeopardising operational continuity |

### Event groups (Table 9 taxonomy)

| Group | Topic |
|-------|-------|
| **1** | Access control |
| **2** | Access control management |
| **3** | Control system / operational |
| **4** | Communication |
| **5** | Request errors |
| **6** | Device configuration / update |
| **7** | Device integrity |
| **8** | Device health / status |
| **9** | Audit logging |

### Table 9 — General cyber security events (pp. 49–56)

**61 events** · Domain **`IEC62351-14`** · ID format **`IEC62351-14:G.N`** (group.event within group).

**CDV note:** Annex C Example 1 and §4.5 Table 3 use `IEC62351-14:1.10` / `USER_ACCNT_CREATE_SUCCESS`; Table 9 canonical entry is **`ACCOUNT_CREATION_SUCCESS`** · **`IEC62351-14:2.1`** — see **R-3514-010**.

#### Group 1 — Access control (pp. 50)

| MNEMONIC | Sev | ID | ExtraInfo |
|----------|-----|-----|-----------|
| `LOGIN_SUCCESS` | notice | IEC62351-14:1.1 | — |
| `LOGIN_SUCCESS_CRED_EXPIRY` | warning | IEC62351-14:1.2 | X.CredType optional |
| `LOGIN_FAIL_WRONG_CRED` | notice | IEC62351-14:1.3 | X.CredType optional |
| `LOGIN_FAIL_CRED_EXPIRY` | error | IEC62351-14:1.4 | X.CredType optional |
| `LOGIN_FAIL_THRESHOLD` | alarm | IEC62351-14:1.5 | attempt count |
| `LOGIN_FAIL_SESSIONS_LIMIT` | alarm | IEC62351-14:1.6 | — |
| `ACCESS_LOCK_WRONG_CRED` | alarm | IEC62351-14:1.7 | X.CredType optional |
| `ACCESS_LOGOUT` | notice | IEC62351-14:1.8 | — |
| `ACCESS_LOGOUT_TIMEOUT` | notice | IEC62351-14:1.9 | — |

**Refs:** IEEE 1686 login/logout · NERC CIP-007-6 R4.1.1 / R4.1.2

#### Group 2 — Access control management (pp. 50–51)

| MNEMONIC | Sev | ID | ExtraInfo |
|----------|-----|-----|-----------|
| `ACCOUNT_CREATION_SUCCESS` | notice | IEC62351-14:2.1 | target account |
| `ACCOUNT_CREATION_FAIL` | warning | IEC62351-14:2.2 | target account |
| `ACCOUNT_ACTIVATION_SUCCESS` | notice | IEC62351-14:2.3 | target account |
| `ACCOUNT_ACTIVATION_FAIL` | warning | IEC62351-14:2.4 | target account |
| `ACCOUNT_DEACTIVATION_SUCCESS` | notice | IEC62351-14:2.5 | target account |
| `ACCOUNT_DEACTIVATION_FAIL` | warning | IEC62351-14:2.6 | target account |
| `ACCOUNT_DELETION_SUCCESS` | notice | IEC62351-14:2.7 | target account |
| `ACCOUNT_DELETION_FAIL` | warning | IEC62351-14:2.8 | target account |
| `CRED_CHANGE_SUCCESS` | notice | IEC62351-14:2.9 | target account · X.CredType optional |
| `CRED_CHANGE_FAIL` | notice | IEC62351-14:2.10 | target account · X.CredType optional |
| `CRED_CHANGE_FAIL_POLICY_VIOLATION` | notice | IEC62351-14:2.11 | target account · policy reason optional |
| `CRED_RESET` | warning | IEC62351-14:2.12 | target account if applicable |

#### Group 3 — Control system / operational (p. 51)

| MNEMONIC | Sev | ID | ExtraInfo |
|----------|-----|-----|-----------|
| `VALUE_FORCING` | warning | IEC62351-14:3.1 | value type · old/new values |

#### Group 4 — Communication (pp. 51–52)

| MNEMONIC | Sev | ID | ExtraInfo |
|----------|-----|-----|-----------|
| `COMM_SUCCESS` | notice | IEC62351-14:4.1 | transport connect (e.g. TCP) |
| `COMM_FAIL` | error | IEC62351-14:4.2 | transport failure |
| `COMM_SA_ESTABLISHMENT_FAIL` | error | IEC62351-14:4.3 | protocol error code · SA type optional |
| `COMM_PEER_AUTHENTICATION_FAIL` | alarm | IEC62351-14:4.4 | — |
| `COMM_PACKET_AUTHENTICATION_FAIL` | error | IEC62351-14:4.5 | packet ID optional |

#### Group 5 — Request errors (p. 52)

| MNEMONIC | Sev | ID | ExtraInfo |
|----------|-----|-----|-----------|
| `REQUEST_INVALID` | error | IEC62351-14:5.1 | error code optional |
| `REQUEST_RESOURCE_UNAVAILABLE` | error | IEC62351-14:5.2 | error code optional |
| `REQUEST_TIMEOUT` | error | IEC62351-14:5.3 | error code optional |
| `REQUEST_ERROR` | error | IEC62351-14:5.4 | error code optional |

#### Group 6 — Device configuration / update (pp. 53–54)

| MNEMONIC | Sev | ID | ExtraInfo |
|----------|-----|-----|-----------|
| `FW_SW_UPDATE_SUCCESS` | notice | IEC62351-14:6.1 | prev + new version |
| `FW_SW_UPDATE_FAIL` | error | IEC62351-14:6.2 | existing + attempted version |
| `CRED_READ_SUCCESS` | notice | IEC62351-14:6.3 | — |
| `CRED_READ_FAIL` | warning | IEC62351-14:6.4 | — |
| `CRED_TRANSFER_SUCCESS` | notice | IEC62351-14:6.5 | direction · method optional |
| `CRED_TRANSFER_FAIL` | warning | IEC62351-14:6.6 | direction · method optional |
| `CONFIG_FILE_EXPORT` | notice | IEC62351-14:6.7 | **Backup** if backup · method optional |
| `CONFIG_FILE_IMPORT` | notice | IEC62351-14:6.8 | **Restore** if restore · method optional |
| `UNAUTHORIZED_CONFIG_SRC` | alarm | IEC62351-14:6.9 | — |
| `CONFIG_CHANGE` | notice | IEC62351-14:6.10 | setting · value · category optional; skip if specific event exists |
| `TIME_DATE_CHANGE` | notice | IEC62351-14:6.11 | — |
| `INVALID_CONFIG_OR_FW` | alarm | IEC62351-14:6.12 | invalidity reason optional |
| `UNAUTHORIZED_CONFIG_OR_FW` | alarm | IEC62351-14:6.13 | auth failure reason optional |

**62443 bridge:** `CONFIG_FILE_EXPORT`/`IMPORT` ExtraInfo **Backup**/**Restore** per Annex D Table 50.

#### Group 7 — Device integrity (p. 54)

| MNEMONIC | Sev | ID | ExtraInfo |
|----------|-----|-----|-----------|
| `FILE_HASH_INVALID` | alarm | IEC62351-14:7.1 | filename |
| `FILE_DS_INVALID` | alarm | IEC62351-14:7.2 | filename |
| `MALWARE_DETECTED` | alarm | IEC62351-14:7.3 | — |
| `SYSTEM_TAMPER_DETECTED` | warning | IEC62351-14:7.4 | — |
| `UNKNOWN_PROCESS` | warning | IEC62351-14:7.5 | — |
| `UNEXPECTED_PROCESS` | warning | IEC62351-14:7.6 | — |

#### Group 8 — Device health / status (p. 55)

| MNEMONIC | Sev | ID | ExtraInfo |
|----------|-----|-----|-----------|
| `TIME_SYNC_SRC_SUCCESS` | notice | IEC62351-14:8.1 | sync source ID |
| `TIME_SYNC_SRC_FAIL` | alarm | IEC62351-14:8.2 | sync source · reason optional |
| `TIME_SYNC_LOST` | alarm | IEC62351-14:8.3 | sync source · reason optional |
| `REBOOT` | alarm | IEC62351-14:8.4 | — |
| `INVALID_HW_CHANGE` | alarm | IEC62351-14:8.5 | HW change details |
| `HW_TAMPER_DETECTED` | alarm | IEC62351-14:8.6 | tamper details |

**CCLI:** GNSS/PTP events map here · lab **`timeQuality`** SD-ELEMENT per Annex C.

#### Group 9 — Audit logging (pp. 55–56)

| MNEMONIC | Sev | ID | ExtraInfo |
|----------|-----|-----|-----------|
| `CYBSECEV_LOG_READ_SUCCESS` | notice | IEC62351-14:9.1 | — |
| `CYBSECEV_LOG_READ_FAIL` | notice | IEC62351-14:9.2 | — |
| `CYBSECEV_LOG_HASH_INVALID` | alarm | IEC62351-14:9.3 | — |
| `UNKNOWN_SYSLOG_EV` | notice | IEC62351-14:9.4 | collector-side |
| `COMM_LOG_SUBSCRIBER_FAIL` | alarm | IEC62351-14:9.5 | TLS syslog subscriber unreachable (**RFC 5425**) |

**CCLI:** **`COMM_LOG_SUBSCRIBER_FAIL`** required when **TCP/6514 TLS** remote syslog fails at connect — complements §6.5 transport.

**Table 9 count:** **61** events (groups **1–9**).

### Table 9 column layout (p. 48)

| Column | Req | Purpose |
|--------|-----|---------|
| **Security Event** | Conditional *(or Event Condition)* | Brief text from referring standard — mapping hook to 62351-14 |
| **Subclause** | Conditional *(mandatory if Security Event used)* | Section in referring standard defining the event |
| **Event Condition** | Conditional *(or Security Event)* | Explicit stimuli/status when referring standard lacks detail (e.g. login-failure circumstances) |
| **MNEMONIC** | **Mandatory** | Per Table 1 |
| **Severity** | **Mandatory** | Per Table 1 |
| **ID** | **Mandatory** | Per Table 1 / §4.4 |
| **Text** | **Mandatory** | Per Table 1 |
| **Logging Attributes** | Optional | Table 2 attrs available at event time; extra context via **ExtraInfo** |
| **Comment** | Optional | Ancillary notes, influencing standards, context |

Referring standards shall provide **Event Condition** detail sufficient for manufacturers to generate events as intended.

---

## Annex D — 62443-4-2 mapping (p. 92, Table 50) — Batch E

| 62443-4-2 attribute | 62351-14 mapping |
|---------------------|------------------|
| **timestamp** | **Timestamp** |
| **source** | **Source** |
| **category** a) access control | Annex A **Group 1** |
| b) request errors | **Group 5** |
| c) control system events | **Group 3** |
| d) backup/restore | **Group 6** + **ExtraInfo** Backup/Restore |
| e) configuration changes | **Groups 2 + 6** |
| f) audit log events | **Group 9** |
| **event ID** | **ID** |
| **event result** | **Text** or **MNEMONIC** + dynamic params |

**NIST AU-2 "Type":** map to closest Table 2 attr per policy; **`LogType`** locates 62351-14 events.

**CCLI link:** `ccli-62443-4-2-extract` CR 2.8 evidence bridge.

---

## Annex B.1.3 — IEC 62351-5 events (pp. 73–74) — Batch G · **C2**

Domain **`IEC62351-5`**. Groups: **1** secure comms · **2** certificate verification.

### Table 25 — Secure communication (Group 1)

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `STAS_PROC_SUCC` | notice | IEC62351-5:1.1 |
| `STAS_PROC_FAIL` | alarm | IEC62351-5:1.2 |
| `SKEY_PROC_SUCC` | notice | IEC62351-5:1.3 |
| `SKEY_PROC_FAIL` | alarm | IEC62351-5:1.4 |
| `SKEY_INV_USETOUT` | alarm | IEC62351-5:1.5 |
| `SKEY_INV_USECNT` | alarm | IEC62351-5:1.6 |
| `PROT_INF_ERR` | alarm | IEC62351-5:1.7 |
| `KEY_AUTNALG_SUP` | alarm | IEC62351-5:1.8 |
| `KEY_WRAPALG_SUP` | alarm | IEC62351-5:1.9 |
| `DATA_PROTALG_SUP` | alarm | IEC62351-5:1.10 |
| `KEY_AUTN_ERR` | alarm | IEC62351-5:1.11 |
| `DATA_AUTN_ERR` | warning | IEC62351-5:1.12 |
| `UNXP_MSG_ERR` | warning | IEC62351-5:1.13 |
| `MAX_REPLY_TOUT` | warning | IEC62351-5:1.14 |

### Table 26 — Certificate (Group 2)

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `NODE_NOT_AUTR` | alarm | IEC62351-5:2.1 |
| `OPER_NOT_AUTR` | warning | IEC62351-5:2.2 |
| `REM_CERT_NOTVALID` | alarm | IEC62351-5:2.3 |
| `REM_CERT_EXPIRED` | warning | IEC62351-5:2.4 |
| `REM_CERT_REVOKED` | alarm | IEC62351-5:2.5 |
| `KEYS_INV_CERTREV` | alarm | IEC62351-5:2.6 |

*60870/DNP3 path — **C2**, not C1 MMS P0.*

---

## Annex B.1.4 — IEC 62351-6 GOOSE/SV (p. 75) — Batch G · **P2**

Domain **`IEC62351-6`**. Local policy may filter by severity (computational load note).

### Table 27 — GOOSE (Group 1)

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `GOOSE_SEC_CHECK_FAIL` | alarm | IEC62351-6:1.1 |
| `GOOSE_REPLAY_DETECTED` | alarm | IEC62351-6:1.2 |
| `GOOSE_NO_AUTHVALUE` | notice | IEC62351-6:1.3 |
| `GOOSE_MAC_GEN_FAIL` | alarm | IEC62351-6:1.4 |
| `GOOSE_MAC_VERIFY_FAIL` | alarm | IEC62351-6:1.5 |
| `GOOSE_UNEXPECTED_KEYID` | alarm | IEC62351-6:1.14 |
| `GOOSE_KEYID_CREDENTIAL_MISSING` | alarm | IEC62351-6:1.14 |

*CDV duplicate ID **1.14** for two mnemonics.*

### Table 28 — SV (Group 2)

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `SV_SEC_CHECK_FAIL` | alarm | IEC62351-6:2.1 |
| `SV_REPLAY_DETECTED` | alarm | IEC62351-6:2.2 |
| `SV_NO_AUTHVALUE` | notice | IEC62351-6:2.3 |
| `SV_MAC_GEN_FAIL` | error | IEC62351-6:2.4 |
| `SV_MAC_VERIFY_FAIL` | alarm | IEC62351-6:2.5 |
| `SV_KEYID_MISSING` | alarm | IEC62351-6:2.6 |

---

## Annex B.1.5 — IEC 62351-8 RBAC (pp. 76–78) — Batch G · **P1**

Domain **`IEC62351-8`**. TLS events → reuse **Annex B.1.1** / **62351-3** tables. Revocation → **62351-9**.

### Event groups

| Group | Topic |
|-------|-------|
| **1** | General access token |
| **2** | Profiles A / B / C |
| **3** | Repository (LDAP / OAuth / RADIUS) |
| **4** | RBAC engineering + O2OS |

### Table 29 — Access token (Group 1)

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `MULTIPLE_AT` | notice | IEC62351-8:1.1 |
| `AT_NOT_AVAIL` | warning | IEC62351-8:1.2 |
| `AT_VAL_PERIODE_FAIL` | alarm | IEC62351-8:1.3 |
| `AT_ROLEID_UNKNOWN` | warning | IEC62351-8:1.4 |
| `AT_RV_NR_MISMATCH` | warning | IEC62351-8:1.5 |
| `AT_AOR_UNKNOWN` | warning | IEC62351-8:1.6 |
| `AT_ROLEDEF_UNKNOWN` | warning | IEC62351-8:1.7 |
| `AT_ROLEDEF_EMPTY` | warning | IEC62351-8:1.8 |
| `AT_OP_UNKNOWN` | warning | IEC62351-8:1.9 |
| `AT_SEQ_NR_FAIL` | warning | IEC62351-8:1.10 |

### Table 30 — Profiles A/B/C (Group 2)

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `AT_SUB_AUTH_FAIL` | alarm | IEC62351-8:2.1 |
| `AT_SIGALG_NO_MATCH` | warning | IEC62351-8:2.2 |
| `AT_INTEGRITY_FAIL` | alarm | IEC62351-8:2.3 |
| `AT_ISSUER_FAIL` | error | IEC62351-8:2.4 |
| `AT_ISSUER_REVOKE` | alarm | IEC62351-8:2.5 |

### Table 31 — Backend (Group 3)

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `AT_NO_LDAP_SERVER` | warning | IEC62351-8:3.1 |
| `AT_NO_OAUTH_SERVER` | warning | IEC62351-8:3.2 |
| `AT_NO_RADIUS_SERVER` | warning | IEC62351-8:3.3 |

### Table 32 — RBAC / O2OS (Group 4) — selected

| MNEMONIC | Sev | ID |
|----------|-----|-----|
| `ROLE_CREATE_OK` | notice | IEC62351-8:4.1 |
| `ROLE_DELETE_OK` | notice | IEC62351-8:4.2 |
| `ROLE_ASSIGN_OK` | notice | IEC62351-8:4.3 |
| `ROLE_REMOVE_OK` | notice | IEC62351-8:4.4 |
| `ROLE_PERMISSION_CHANGE_OK` | notice | IEC62351-8:4.5 |
| `ROLE_PERMISSION_ADD_OK` | notice | IEC62351-8:4.6 |
| `ROLE_PERMISSION_REMOVE_OK` | notice | IEC62351-8:4.7 |
| `ROLE_CREATE_FAIL` | warning | IEC62351-8:4.8 |
| `ROLE_DELETE_FAIL` | warning | IEC62351-8:4.9 |
| `ROLE_ASSIGN_FAIL` | warning | IEC62351-8:4.10 |
| `ROLE_REMOVE_FAIL` | warning | IEC62351-8:4.11 |
| `ROLE_PERMISSION_CHANGE_FAIL` | warning | IEC62351-8:4.12 |
| `ROLE_PERMISSION_ADD_FAIL` | warning | IEC62351-8:4.13 |
| `ROLE_PERMISSION_REMOVE_FAIL` | warning | IEC62351-8:4.14 |
| `ROLE_UNKNOWN_PERMISSION` | notice | IEC62351-8:4.15 |
| `O2OS_UNKNOWN_PERMISSION` | notice | IEC62351-8:4.16 |
| `O2OS_UNKNOWN_OBJECT` | warning | IEC62351-8:4.17 |
| `O2OS_UNKNOWN_OPERATIONSET` | notice | IEC62351-8:4.18 |
| `ROLE_PERMISSION_INT_OK` | warning | IEC62351-8:4.19 |
| `ROLE_PERMISSION_INT_FAIL` | warning | IEC62351-8:4.20 |
| `O2OS_INT_OK` | notice | IEC62351-8:4.21 |
| `O2OS_INT_FAIL` | warning | IEC62351-8:4.15 |

*CDV duplicate **4.15** on `O2OS_INT_FAIL` — verify FDIS.*

---

## Annex B.1.7 — IEC 62351-11 XML (p. 82)

| MNEMONIC | Sev | ID | Text |
|----------|-----|-----|------|
| `XML_SIG_GEN_SUCCESS` | notice | IEC62351-11:1.1 | XML signature generation successful. |
| `XML_SIG_GEN_FAIL` | alarm | IEC62351-11:1.2 | XML signature generation failed. |

---

## Annex C — Syslog examples (pp. 83–91) — Batch G

### C.1 Message grammar (p. 83)

```
SYSLOG-MSG = HEADER SP STRUCTURED-DATA [SP MSG]
HEADER = <PRI>VERSION SP TIMESTAMP SP HOSTNAME SP APP-NAME SP PROCID SP MSGID
STRUCTURED-DATA = SD-ELEMENT ...
SD-ELEMENT = [SD-ID SP PARAM-NAME="PARAM-VALUE" ...]
MSG = MSG-ANY / MSG-UTF8
```

### Example 1 — Account creation (Tables 39–43, pp. 84–87)

| Field | Value |
|-------|-------|
| MNEMONIC | `ACCOUNT_CREATION_SUCCESS` |
| ID | `IEC62351-14:1.10` |
| Severity | notice → PRI **`<109>`** |
| P.NewUser | `arijit` |
| UsrID | `robin` (creator, not new account) |
| SqNum | `0` (unsupported counter) |
| timeQuality | `tzKnown=1 isSynced=1 syncAccuracy=1000` |

**Complete line (p. 87):**

```
<109>1 2019-01-10T13:40:20.12Z 100.1.1.0 appA - IEC62351-14:1 [62351-14@41912 ID="IEC62351-14:1.10" Text="Account creation successful: \"{P.NewUser}\"." SqNum="0" PeerInfo="100.1.1.1" X.0="Connection has been successfully established." UsrID="robin" P.NewUser="arijit"] [timeQuality tzKnown="1" isSynced="1" syncAccuracy="1000"]
```

### Example 2 — TLS weak version + SqNum 57 (Tables 44–48, pp. 88–91)

| Field | Value |
|-------|-------|
| MNEMONIC | `TLS_WEAK_VERSION` |
| ID | `IEC62351-3:1.4` *(example uses 1.4; mnemonic implies 1.3 — CDV example inconsistency)* |
| Severity | warning → PRI **`<108>`** |
| SqNum | **57** |
| P.Version | `1.0` |
| timeQuality | `tzKnown=0 isSynced=0` (syncAccuracy omitted) |

**Complete line (p. 91):**

```
<108>1 2019-04-07T14:10:30.32Z 101.0.2.0 appB - IEC62351-14:1 [62351-14@41912 ID="IEC62351-3:1.4" Text="Deprecated TLS version proposed ({P.Version}) and support of TLS versions prior to TLSv1.2 enabled." SqNum="57" PeerInfo="101.0.2.1" X.DepVer="Proposed TLS version number is 1.0." P.Version="1.0"] [timeQuality tzKnown="0" isSynced="0"]
```

### Example 3 — Custom SD-ELEMENT (Table 49, p. 91)

Vendor SD-ID **`XYZTest@32473`** (IANA doc PEN) · `L7app="HTTP"` for OSI L7 over TLS — append after mandatory SD-ELEMENTs.

---
## Interface matrix

| Interface | Source | Destination | Protocol |
|-----------|--------|-------------|----------|
| Event generation | CCLI apps / OpenWrt | Local collector | Table 2 + RFC 5424 SD |
| Secure transport | TG-524 Eth_mgmt | SIEM | **TCP/6514** · **RFC 5425 TLS** |
| SD namespace | All 62351-14 events | Collector parser | **`62351-14@41912`** |
| Event definitions | Annex B.1.6 Tables 33–37 | Firmware event DB | Static catalogue |
| 62351-9 bridge | Annex D ↔ B.1.6 | Same mnemonics/IDs | **`IEC62351-9:G.N`** |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| §8 French translation | Not captured | p. **45** | Skip |
| Title page edition | CDV 2025 header | FDIS string | Confirm on publish |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-3514-001 | CDV not final IS | ID drift | Track FDIS; IECVersion field |
| R-3514-002 | Text vs MNEMONIC only | Parser mismatch | Support both per Table 2 |
| R-3514-003 | SqNum reset on reboot | Replay false positive | Document reboot policy |
| R-3514-004 | 62351-9 ID spacing | Parser errors | Normalise `IEC62351-9` domain |
| R-3514-005 | UDP syslog habit | Non-compliant transport | Enforce TCP/6514 only |
| R-3514-006 | Text SD-PARAM pre-substitution | Collector mismatch | Log template text; P.Label separate |
| R-3514-007 | CDV typos (62391 ID, swapped TLS mnemonics) | Wrong parser mapping | Track FDIS; normalise in firmware |
| R-3514-008 | Annex C Ex.2 ID 1.4 vs TLS_WEAK_VERSION | Lab parser confusion | Use B.1.1 ID **1.3** |
| R-3514-009 | GOOSE duplicate ID 1.14 | Ambiguous events | Disambiguate by MNEMONIC |
| R-3514-010 | Annex C/§4.5 ID **1.10** vs Table 9 **2.1** | Parser / firmware mismatch | Use Table 9 canonical IDs |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| Batch A Table 1 | Extract complete | **PASS** |
| Batch A Table 2 | Extract complete | **PASS** |
| Batch A §4.4 ID | Parse `IEC62351-9:1.5` | **PASS** |
| Batch B §6 HEADER/SD | Tables 4–5 mapped | **PASS** |
| Batch B Table 7 PRI | notice → `<109>` | **PASS** |
| Batch B §6.5 transport | TCP/6514 TLS; no UDP | **PASS** |
| K6.3 sample log | Emit RFC 5424 line | **PASS** (template) |
| Batch C B.1.6 | Tables 33–37 vs Annex D | **PASS** |
| 62351-9 Annex D | MNEMONIC ↔ ID | **PASS** |
| Batch D B.1.1 | Tables 10–11 TLS | **PASS** |
| Batch D B.1.2 | Tables 12–24 E2E | **PASS** |
| Batch E §11 + Annex D | Conformance + 62443 map | **PASS** |
| Batch E Annex A | Table 9 groups 1–9 · **61 events** | **PASS** |
| Batch G B.1.3–8,11 | 62351-5/6/8/11 | **PASS** |
| Batch G Annex C | Worked syslog examples | **PASS** |
| Batch F §7 | XML XSD pp. 32–44 | **PASS** |

---

## Source pages — Batch A

| Page | Content | Status |
|------|---------|--------|
| **6** | Foreword — TC 57; **62351-14** IS | PASS |
| **7** | Stability date notice (CDV admin) | PASS |
| **8** | Introduction — logging for holistic cyber situational awareness; tissue-db | PASS |
| **9** | **§1 Scope** — Syslog; vs 62351-7 | PASS |
| **10** | **§2** refs 62351-1…12 | PASS |
| **11** | §2 cont — 62443, RFC 5424/5425, TLS, PKI, IEEE 1686 | PASS |
| **12** | **§3.1–3.6** terms | PASS |
| **13** | **§3.6.1** named ASCII characters | PASS |
| **14** | **§3.6.2–3.6.3** character sets · UTF-8 | PASS |
| **15** | **§3.7–3.8** · **§4** intro | PASS |
| **16–18** | **§4.1 Table 1** define event | PASS |
| **19** | **§4.2 Table 2** start — Timestamp · SqNum | PASS |
| **20** | Table 2 — Source · App-Name · PeerInfo | PASS |
| **21** | Table 2 — X.Label · UsrID | PASS |
| **22** | Table 2 — Role · P.Label · RefEvtID | PASS |
| **23** | **§4.3 Figure 1** · **§4.4** start | PASS |
| **24** | **§4.4.1–4.4.3** ID · Domain · Group | PASS |
| **25** | **§4.4.4** Event Number · **§4.5** · **Table 3** | PASS |

---

## Source pages — Batch B

| Page | Content | Status |
|------|---------|--------|
| **26** | §4.5 conclusion · **§5** XML · **§6** intro · **§6.1** HEADER start | PASS |
| **27** | **Table 4** HEADER · MSGID · PROCID note | PASS |
| **28** | **§6.2** · **Table 5** SD `62351-14@41912` · ID/MNEMONIC/Text/X.Label | PASS |
| **29** | Table 5 cont · **§6.2.2** timeQuality | PASS |
| **30** | **Table 6** timeQuality · **§6.3 MSG** · **§6.4 PRI** start | PASS |
| **31** | **Table 7** severity ↔ PRI · **§6.5** TLS transport | PASS |
| **32** | **§6.6** raw storage · **§7** XML intro | PASS |
| **33–35** | **§7.1.1** BaseTypes XSD | PASS |
| **36–40** | **§7.1.2** EventsDefinition + **SECEVTDef** | PASS |
| **41–44** | **§7.1.3** EventList + **SECEVTLogs** · **§7.2** | PASS |

---

## Source pages — Batch C

| Page | Content | Status |
|------|---------|--------|
| **79** | **Annex B.1.6** intro · event groups · **Table 33** enrolment (1.1–1.6) | PASS |
| **80** | **Table 34** pub-key verification (2.1–2.27) | PASS |
| **81** | **Table 35** attribute cert (3.1–3.16) · **Table 36** revocation (4.1–4.9) · **Table 37** GDOI (5.1–5.8) | PASS |

---


---

## Source pages — Batch D

| Page | Content | Status |
|------|---------|--------|
| **58** | **Table 10** TLS handshake 1.1–1.15 | PASS |
| **59** | Table 10 cont 1.16–1.21 · **Table 11** cert 2.1–2.14 | PASS |
| **60** | **CRLReason** ExtraInfo enum | PASS |
| **61** | **Annex B.1.2** intro · event groups | PASS |
| **62–63** | **Table 12** HandshakeReq 1.1–1.22 | PASS |
| **63–64** | **Table 13** HandshakeAcc 2.1–2.21 | PASS |
| **64** | **Table 14** ApplicationReject 3.1–3.10 | PASS |
| **65–66** | **Table 15** HandshakeSecReject 4.1–4.27 | PASS |
| **66–67** | **Table 16** HandshakeSecAbort 5.1–5.25 | PASS |
| **67–68** | **Table 17** DtSecAbort 6.1–6.16 | PASS |
| **68** | **Table 18** ApplAbort 7.1–7.10 | PASS |
| **69** | **Table 19** ClearTransfer 10.1–10.10 | PASS |
| **70** | **Table 20** EncrTransfer 11.1–11.11 | PASS |
| **71** | **Tables 21–22** ReleaseReq/Rsp | PASS |
| **72** | **Table 23** OSI/ACSE 15.1–15.16 | PASS |
| **73** | **Table 24** XMPP 18.1–18.6 | PASS |


---

## Source pages — Batch E

| Page | Content | Status |
|------|---------|--------|
| **46** | **§9** SNMP · **§10** storage · **§11** Table 8 conformance | PASS |
| **47** | **Annex A** intro · groups 1–4 | PASS |
| **48** | Annex A groups 5–9 · Table 9 column layout | PASS |
| **49** | Table 9 header row | PASS |
| **50–56** | **Table 9** events groups **1–9** (**61** events) | PASS |
| **92** | **Annex D** · **Table 50** 62443-4-2 mapping | PASS |

---

## Source pages — Batch G

| Page | Content | Status |
|------|---------|--------|
| **73** | **B.1.3** · **Table 25** 62351-5 start | PASS |
| **74** | Table 25 cont · **Table 26** 62351-5 cert | PASS |
| **75** | **B.1.4** · **Tables 27–28** GOOSE/SV | PASS |
| **76** | **B.1.5** · **Table 29** access token | PASS |
| **77** | **Tables 30–32** RBAC start | PASS |
| **78** | Table 32 cont · CRLReason note | PASS |
| **82** | **B.1.7** · **Table 38** 62351-11 XML | PASS |
| **83** | **Annex C.1** syslog grammar | PASS |
| **84–87** | **Example 1** account creation · Tables 39–43 | PASS |
| **88–91** | **Example 2–3** SqNum · custom SD-ELEMENT | PASS |

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-08-12 | **Batch A** pp. 6–25: §1–§4 event model · Tables 1–3 · Figure 1 |
| **1.1** | 2026-08-12 | **Batch B** pp. 26–32: §5–§6 Syslog · Tables 4–7 · K6.3 wire example |
| **1.2** | 2026-08-12 | **Batch C** pp. 79–81: Annex B.1.6 · Tables 33–37 · K6.3 phase map |
| **1.3** | 2026-08-12 | **Batch D** pp. 58–73: B.1.1 TLS · B.1.2 E2E Tables 10–24 |
| **1.4** | 2026-08-12 | **Batch E** pp. 46–48,92 + **Batch G** pp. 73–82,83–91 |
| **1.4.1** | 2026-08-12 | **pp. 46–48** refine: §11 entity roles · Annex A policy · Table 9 column layout |
| **1.5** | 2026-08-12 | **Table 9** pp. **49–56** · **61** generic events |
| **1.6** | 2026-08-12 | **Batch F** §7 XML XSD pp. **32–44** |

---

## RAG cross-links

| source_id | Document |
|-----------|----------|
| `ccli-62351-14-extract` | This document |
| `ccli-62351-14-capture-plan` | Capture batches |
| `ccli-k63-ceremony-sop` | K6.3 PKI ceremony + sample events |
| `ccli-62351-9-extract` | Annex D mnemonics + severities |
| `ccli-62351-3-extract` | TLS events (Annex B pending) |
| `ccli-62351-4-extract` | E2E events (Annex B pending) |
| `ccli-62443-4-2-extract` | CR 6.2 · Annex D mapping (p. 92 pending) |

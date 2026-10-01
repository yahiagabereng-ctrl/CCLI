# IEC 61850-8-2 — CCLI Engineering Extract (XMPP SCSM)

**Document ID:** CCLI-PROTO-61850-8-2-001  
**Revision:** 1.17  
**Date:** 2026-09-21  
**Edition (TOC):** IEC 61850-8-2:2018  
**RAG source_id:** `ccli-61850-8-2-extract`  
**Normative basis:** IEC 61850-8-2 — *SCSM — Mapping to XMPP*  
**Licensed PDF:** `regulations/standards-drop-20260918/IEC 61850-8-2.pdf` — **awaiting file on disk** (TOC captured from user Contents photo)  
**Capture status:** **PARTIAL body** — **pp. 13–26**, **30–56**, **57–189**, **190–222** HAVE (2026-09-21); **pp. 27–29** (§6–§7) pending; **223+** (Annex F/G) pending  
**Parents:** `ccli-61850-7-2-extract` · `ccli-61850-6-extract` · `cei-tr-57-126`  
**Programme link:** Optional XMPP profile · **not** Italian CCI MMS path

---

## Summary

**61850-8-2** specifies how **ACSI** (7-2) is carried over **XMPP**: XML payloads, **TLS + SASL**, stream compression, **XEP 0199 PING** and **XEP 0198** stream management, and XSD mappings from **LN / DO / DA** to **VariableAccessSpecifications**.

**HiTEKS CCLI / CEI TR 57-126:** production DSO interface is **`8-MMS`** (**61850-8-1**), not **`8-XMPP`**. Keep 8-2 in corpus for competitor-parity and future optional profiles.

---

## Requirements

| REQ-ID | Source | Requirement | CCLI |
|--------|--------|-------------|------|
| **REQ-882-SCOPE-001** | §1 | Defines XMPP SCSM for 61850 client/server | **P2** — not Eth_A default |
| **REQ-882-NS-001** | §1.2 | Namespace name and version for XMPP binding | If XMPP enabled |
| **REQ-882-MAP-001** | §5.2 | Map client/server services to XML payloads + XMPP | vs MMS in 8-1 |
| **REQ-882-XMPP-001** | §6.2 | Connection: TLS, SASL, optional stream compression | Align 62351-3 if used |
| **REQ-882-ACSI-001** | §6.3 | Mapping of ACSI services onto XMPP stanzas | Cross-ref 7-2 extract |
| **REQ-882-PING-001** | §6.6–6.7 | XEP 0199 PING agreements | Keepalive |
| **REQ-882-SEC-001** | §7 | End-to-end security | 62351-4/3 if deployed |
| **REQ-882-PAY-001** | §8.2 | LN/DO/DA refs → VariableAccessSpecifications | Parallel to MMS VMD |
| **REQ-882-TYPE-001** | §8.3 | Map 7-2 BasicTypes and common ACSITypes in XML | ICD tooling |
| **REQ-882-Q-001** | §8.3.4 | Map **Quality** CDC type from 7-2 in XML | Telemetry q flags |
| **REQ-882-XML-001** | §8.4 | General mapping of data values in XML payloads | Parser |
| **REQ-882-OPT-001** | §8.5 | Bandwidth optimization extensions | Optional |
| **REQ-882-SRV-001** | §9.2 | **GetServerDirectory** XMPP binding | Parallels Annex T Listobjects |
| **REQ-882-ASSOC-001** | §10.2 | Two-party association; secured E2E establishment | cf. 62351-4 |
| **REQ-882-LD-001** | §11 | Logical device model + responses | |
| **REQ-882-LN-001** | §12.2–12.3 | **GetLogicalNodeDirectory**, **GetAllDataValues** | Annex T read path |
| **REQ-882-DO-001** | §13.2–13.5 | **Get/SetDataValues**, **GetDataDirectory**, **GetDataDefinition** | Control/config gated |
| **REQ-882-DS-001** | §14.2–14.6 | **Get/SetDataSetValues**, **Create/DeleteDataSet**, **GetDataSetDirectory** | TR: read-only DataSet profile |
| **REQ-882-RPT-001** | §17.1–17.2 | **BRCB/URCB**; **Report**; **Get/SetBRCBValues**, **Get/SetURCBValues** | TR `intgPd=4000`; Annex T Set N/A |
| **REQ-882-LOG-001** | §17.3 | **Log model**, LCB attributes, log services | Optional audit |
| **REQ-882-GOOSE-001** | §18 | **GOOSE** GoCB; Layer 2 vs **Routable GOOSE** | P1 plant; wire detail in **8-1** |
| **REQ-882-SV-001** | §19 | **Sampled values** MSVCB | Out of P0 CCI |
| **REQ-882-CTL-001** | §20 | **Control class model** | cf. 7-2 + Annex T CONTROL |
| **REQ-882-CTL-002** | §20.2–20.6 | **Select**, **SelectWithValue**, **Cancel**, **Operate**, **CommandTermination** | Curtailment SBO/direct |
| **REQ-882-CTL-003** | §20.7–20.8 | **TimeActivatedOperate** + termination | Optional |
| **REQ-882-TIME-002** | §21 | Time / time synchronization model | Annex T UTC ±100 ms |
| **REQ-882-FILE-001** | §23 | **GetFile / SetFile / DeleteFile / GetFileAttributeValues** | Firmware/config transfer |
| **REQ-882-CONF-001** | §24 | **PICS** — profile + **XML payload** conformance | Lab quote |
| **REQ-882-SCL-001** | §25 | **SCL** references | Cross-ref `ccli-61850-6-extract` |
| **REQ-882-CODE-001** | §1.3 | **Code Components** (XSD) — PDF vs zip precedence | Download zip for parser; PDF secondary |
| **REQ-882-TBL1-001** | §5.2.1, **Table 1** | Client/server profile shall cover listed **7-2** services | Cross-ref Annex T / **8-1** MMS P0 |
| **REQ-882-GOOSE-UNMAP-001** | **Table 1** note ᵃ | **SendGOOSEMessage**, **SendMSVMessage**, ref/element lookup **not mapped** on XMPP (performance) | Plant GOOSE stays **8-1** |
| **REQ-882-PAYLAY-001** | §5.2.2 | Three-layer XML: **Service PDU** (Annex **G.3**) → **62351-4** security → association context in stanza | cf. MMS mapping in **8-1** |
| **REQ-882-E2E-001** | §5.2.3 | **E2E security mechanism mandatory** to implement; may be **disabled by deployment config** (out of scope); both peers must match | Align **62351-4** |
| **REQ-882-TIME-001** | §5.3 | **7-2** time sync mapped to **IETF SNTP** (not NTP full profile in this clause) | TR Annex T: NTP/NTS on MMS path |
| **REQ-882-JID-001** | §6.1 | IED **XMPP account**; connection **resource fixed** — generic value **`IEC`** (e.g. `myIED@DSO.org/IEC`) | Lab only if XMPP profile |
| **REQ-882-TLS-001** | §6.2.2 | **TLS shall** be used; per **IEC 62351-6** — XMPP stream TLS **or** TLS tunnel then stream | Product Eth_A: **62351-3/4** on MMS |
| **REQ-882-FC-XX-001** | §8.2.2 Note 2 | **FC `XX`** wildcard **not mapped** — `FC=XX` → negative ack | Same rule on **8-1** MMS |
| **REQ-882-FLAT-001** | §8.2.2–8.2.6 | Flat **`itemId`**: `LN$FC$DO…$DA`; max **64** chars; **alternateAccess** for arrays (no `$` in `<component>`) | TR report **DataRef** |
| **REQ-882-ERR-001** | §8.3.3.4, Tables 6–10 | XMPP / **62351-4** / **rejectPDU** / Read-Write-File errors; **Table D.1** mapping | P2 lab |
| **REQ-882-QUAL-001** | §8.3.4, Table 18 | **Quality** 13-bit PACKED LIST per **Table 18** | MMS telemetry **q** P0 |
| **REQ-882-EXTGL-001** | §8.5 | **`extendedGetNameList`** optional at Associate for directory services | cf. Annex T Listobjects |
| **REQ-882-PDU-001** | §10.2.2.1.3 | Negotiated **`max_mms_pdu_size`**; **≥8192** octets recommended; oversize → reject / **pdu-size** | Same concept on **8-1** MMS |
| **REQ-882-INIT-001** | §10.2.2, **Tables 23–26** | **Associate** → `<initiate-*PDU/>`; **SAPRef** = peer **JID**; **Table 26** error mapping | XMPP-only wire |
| **REQ-882-LDFIL-001** | §11, **Table 29** | **GetLogicalDeviceDirectory** returns flattened names; client strips **`$`** to derive **LN** list | Directory client logic |
| **REQ-882-DS-DEL-001** | §14.5 | **DeleteDataSet** only **`scopeOfDelete="specific"`**; bulk **aa/domain/vmd** delete **out of scope** | TR: Create/Delete **N/A** |
| **REQ-882-DS-RES-001** | §14.2 | **GetDataSetValues** per-member **success/failure** in one response | Partial dataset read |
| **REQ-882-TRK-001** | §15, **Tables 50–53** | **serviceType** / **errorCode** enumerations for **LTS/GTS** CDC service tracking | Audit cross-ref **62351-14** |
| **REQ-882-SG-001** | §16 | **SGCB** on **LLN0$SP$SGCB**; SG services = **read/write** specializations (**Figs 54–63**) | Optional DSO protection |
| **REQ-882-RPT-WIRE-001** | §17.2.1 | **Report** → `<unconfirmed-PDU/>` + `<informationReport/>`, **vmd-specific** `RPT`; **Table 57** AccessResult order | TR reporting on **8-1** P0 |
| **REQ-882-BRCB-001** | §17.1.1, **Table 55** | **BRCB** **FC=BR**; **Owner** = client **JID** (OCTET STRING 64); **RptEna** fails if **DatSet** NULL | **intgPd=4000** TR |
| **REQ-882-URCB-001** | §17.1.2, **Table 56** | **URCB** **FC=RP**; **OptFlds** buffer-overflow/entryID ignored; **SqNum** reset on **RptEna** FALSE→TRUE | `urcb_*` TR |
| **REQ-882-LOG-LCB-001** | §17.3, **Tables 60–64** | **LCB** merges 7-2 log + log-control attrs; **QueryLog** via `<readJournal/>` | Optional vs **62351-14** Syslog |
| **REQ-882-GOCB-001** | §18, **Table 65** | **GoCB** **FC=GO/RG**; **SendGOOSEMessage** **not mapped** on XMPP — use **8-1** | Plant GOOSE **8-1** |
| **REQ-882-MSVCB-001** | §19, **19.2.1** | **MSVCB** **FC=MS/RS/US**; **SendMSVMessage** / ref lookup **not mapped** on XMPP | SV wire **9-2** / **8-1** |
| **REQ-882-CTL-CO-001** | §20.1, **Tables 66–67** | **CO** FC; **`CO_CtrlObjectRef`** = `LD/LN$CO$DO`; components **SBO/SBOw/Oper/Cancel** per **ctlModel** | Map PF2 DO **ctlModel** in CID |
| **REQ-882-CTL-MAP-001** | **Table 68** | Control ACSI ↔ **Read/Write-Request/Response** + **informationReport** for negatives | **8-1 MMS** P0 same semantics |
| **REQ-882-CTL-NEG-001** | §20.4.3, **20.5.3**, **20.6.3** | Negative control: **(1)** `<unconfirmed-PDU/>` **LastApplError** then **(2)** `<confirmed-ResponsePDU/>` **write/failure** (**Table 72**) | Client must correlate **ctlNum/origin** |
| **REQ-882-LASTERR-001** | §20.9, **Table 78**, **Fig 80** | **LastApplError** VMD variable — volatile; create/report/delete; **CntrlObj/Error/Origin/ctlNum/AddCause** | Diagnostics on failed curtail |
| **REQ-882-ADDCAUSE-001** | **Table 79** | **AddCause** enum **0–27** (e.g. **Object-not-selected**, **Locked-by-other-client**) | Log + HMI on control reject |
| **REQ-882-CMDTERM-001** | §20.6.4–5, **Table 77**, **Figs 78–79** | **CommandTermination** → `<unconfirmed-PDU/>` **informationReport** on **`$Oper`**; **operTm=0** in term+ | SBO enhanced security path |
| **REQ-882-CTS-001** | §20.10, **Table 80** | **CTS** CDC inherits **CST** + **ctlVal…Check**, **respAddCause** (**Table 79**) | Control audit tracking |
| **REQ-882-SNTP-001** | §21 | Time sync via **SNTP** (**§5.3**); GPS hardware out of scope | TR Annex T: **NTP/NTS** on **8-1** P0 |
| **REQ-882-FILE-002** | §23.1, **Tables 81–82** | **7-2 file class** → MMS file object; **LD/** root; suffixes **Bin/Xml/Xsd/Zip**…; COMTRADE dir | Signed CID/firmware policy P2 |
| **REQ-882-FILE-003** | §23.2.1, **Table 83**, **Fig 81** | **GetFile** = **FileOpen** (pos 0) → **FileRead** loop (**moreFollows**) → **FileClose**; **Table 84** errors | **GraphicString** path chunks ≤64 |
| **REQ-882-PICS-T-001** | **Table 95** | **T-Profile:** **T1/T7** TCP+security **m**; **T5** time **c** if logging/TA control; GOOSE/SV T-profiles **i** | Product **8-MMS**, not T-Profile |
| **REQ-882-ALTACC-001** | §24.2.2.2.1.1, **Figs 97–102** | **AlternateAccess** for arrays only; **`component`/`componentName` shall not contain `$`** | Same flatten rule as **§8.2** |
| **REQ-882-JID-SCL-001** | §25, **Table 123** | SCL **P-Type `JID`** (RFC 6122); resource part per **§6.1** (`/IEC`) or absent | TR CID stays **IP/MMS** addressing |
| **REQ-882-PROF-A-001** | **Annex A**, **Tables A.1–A.2** | Client/server **A-Profile:** MMS (**ISO 9506**) + **62351-6** E2E + **XER** (**ITU X.693**) + **62351-4** association context; **T-Profile:** XMPP (**RFC 6120–6122**, **XEP-0198/0199**), **SASL**, **TLS**, **TCP**; **IPv4/IPv6** per **c1/c2** | Product stack = **8-1** BER, not XMPP |
| **REQ-882-PROF-SNTP-001** | **Annex A**, **Tables A.3–A.4** | Time-sync profile mandatory if **Timestamp** objects declared; **SNTP** subset **RFC 5905**; modes **3** (client) / **4** (server); UDP dest port **123** | TR **NTP/NTS** on Eth_A |
| **REQ-882-MSGTYPE-001** | **Annex A.1.1** | **61850-5** message types **1, 1A, 4** (fast/trip/raw) **out of scope** on XMPP; **2, 3, 5, 6** mapped | GOOSE/SV stay **8-1** / **9-2** |
| **REQ-882-ASSOID-001** | **Annex A.2.2.2** | On **Associate**, both peers generate locally unique **AssociationID** for the **JID** pair; include in all messages | Parallels MMS context on **8-1** |
| **REQ-882-STANZA-001** | **Annex D**, **Table D.1** | **ClearTransfer/EncrTransfer** payloads: **confirmed-RequestPDU** → **`iq` `get`/`set`** by service; **confirmed-ResponsePDU** → **`iq` `result`**; **unconfirmed-PDU** **`informationReport`** → **`message` `normal`** | Normative wire ref for P2 XMPP lab |
| **REQ-882-STANZA-ERR-001** | **Annex D**, **Table D.2** | Stream/stanza errors per **RFC 6120** namespaces; **initiate-*** handshakes **See IEC 62351-4**; association errors on **confirmed-ErrorPDU** / **rejectPDU** / **cancel/conclude-ErrorPDU** | Cross-ref **62351-4** extract |
| **REQ-882-DER-SEC-001** | **Annex C** (informative) | Assumption **A6:** XMPP operator trusted for routing, **not** for reading/modifying payload; **R1–R6** E2M (TLS) vs E2E (**62351-4**) | **62351-3/4** on MMS P0 |

---

## Body capture log (batch W)

| Sub-batch | IEC pages | Status | Source |
|-----------|-----------|--------|--------|
| **W1a** | **13–15** | **HAVE** | Foreword, code-component notice, **Introduction** (DER / smart grid / XMPP SCSM) |
| **W1b** | **16–21** | **HAVE** | **§1–§4** scope, namespace, refs, terms, abbreviations |
| **W1c** | **22–26** | **HAVE** | **§5–§6.2.2** (Figure 1, **Table 1**, XML payload, XMPP connect) |
| **W1d** | **27–29** | **PENDING** | **§6.2.3–§7** — not in this photo batch (jump **26→30**) |
| **W2** | **30–56** | **HAVE** | **§8** payload: **Table 2–19**, LN/`$` flattening, types, **Quality**, **§8.5** `extendedGetNameList` |
| **W3** | **57–98** | **HAVE** | **§9–§14.6.1** ACSI→XML: directory, Associate, LD/LN, data, DataSets (**Tables 20–47**) |
| **W3b** | **99–101** | **HAVE** | **GetDataSetDirectory** tail (**Tables 48–49**, **Figs 52–53**); **§15** (**Tables 50–52** start) |
| **W4** | **102–131** | **HAVE** | **§15** (**Tables 51–53**), **§16** SG (**Tables 54**, **Figs 54–63**), **§17–§18.4** reports/logs/GOOSE start |
| **W5** | **132–152** | **HAVE** | **§19** SV (**19.2**); **§20** control (**Tables 66–79**, **Figs 73–80**) |
| **W6** | **153–189** | **HAVE** | **§20.10–§25** CTS, SNTP, files (**Tables 80–92**, **Figs 81–96**), **PICS** (**Tables 95–122**), **AltAccess** (**Figs 97–102**), **Table 123** |
| **W7** | **190–222** | **HAVE** | **Annex A** (**Tables A.1–A.4**, **Figs A.1–A.2**); **Annex B** deploy/federation (**Figs B.1–B.18**, **B.4**); **Annex C** DER security (**Figs C.1–C.8**); **Annex D** **Tables D.1–D.2** |

---

## Front matter (pp. 13–15)

| Page | Content | CCLI note |
|------|---------|-----------|
| Foreword | IEC TC 57; FDIS **57/2020/FDIS**, report **57/2039/RVD** | Edition traceability |
| **14** | **Code Components** marked `<CODE BEGINS>` / `<CODE ENDS>`; licence **www.iec.ch/CCv1**; colour-inside logo | XSD lives in zip, not PDF body |
| **15** | **Introduction** — layered architecture; **DER** over public networks; maps **7-2 / 7-3 / 7-4** to XMPP; read **61850-5** + **7-1** with **7-2** for tutorial | Confirms **8-2 ≠** Italian **8-MMS** CCI path |

---

## §1 Scope (body pp. 16–17)

**1.1 General**

- Exchange data over **any network including public**; today: **client/server** + **time synchronization** from **61850-7-2** (GOOSE/SMV **control-block** services also mapped — **Table 1**; wire performance services excluded — note ᵃ).
- Mapping: **ACSI → XML messages over XMPP**.
- Three mapping pillars: **§6** XMPP usage; **§7** E2E security; **§9+** XML payloads per ACSI service (schemas + examples).
- **Note 3:** Compared to **61850-8-1 (MMS)** — XML derived from 8-1 structure but **XML-encoded**; **8-1 ↔ 8-2 gateways** straightforward; 8-1 knowledge not required except for **GOOSE mappings** called from 8-1.

**1.2 Namespace**

| Parameter | Value |
|-----------|--------|
| Namespace URI | `http://www.iec.ch/61850/2018/SCSM_8_2` |
| Version | 2018 |
| Revision | A |
| Release | 1 |
| Release date | 2018-12 |

**1.3 Code Component distribution**

- Zip: `http://www.iec.ch/tc57/supportdocuments/IEC_61850-8-2.2018_ed1.0.XSD.2018A1.full.zip` (also `…/tc57/supportdocuments`).
- Pick file with highest `{VersionStateInfo}`.
- **If PDF disagrees with downloadable code → code wins.**

---

## §2 Normative references (body pp. 17–19) — CCLI cross-refs

| Reference | Relevance |
|-----------|-----------|
| **61850-6**, **61850-7-2** (+ AMD1:2018), **7-3**, **7-4**, **61850-5**, **61850-7-1** | Model + ACSI parent |
| **61850-8-1:2011** (+ AMD1:2018) | **MMS/GOOSE wire** — **CCLI P0** |
| **62351 (all)**, **62351-4:2018**, **62351-6** | TLS / E2E / GOOSE security |
| **ISO 9506** (MMS), **ISO/IEC 8824/8825** (ASN.1/XER) | Informative stack in PICS tables |
| **RFC 5246** (TLS 1.2), **RFC 5905** (NTP v4), **RFC 6120–6122** (XMPP core/IM/addressing) | XMPP profile |
| **RFC 4422** (SASL), **RFC 3629** (UTF-8) | Auth + encoding |
| **XEP-0198**, **XEP-0199** | Stream management + ping |

Footnotes on **62351-4** / **62351-6**: new editions in preparation at publication (2018).

---

## §3 Terms and §4 Abbreviations (pp. 19–21)

- Terms **3.1–3.12** largely cite **61850-7-2:2010** (client, device, LD, LN, server, …).
- **3.7** **A-Profile** and **T-Profile** — application and transport profile sets.
- **3.11** **private network** — performance guaranteed; else **public**.
- **4** Abbreviations include **JID**, **SCSM 8-1**, **SCSM 8-2**, **DER**, **DSO**, **VPP**, **PICS**, **SCL**, **BRCB/URCB**, **MMS**, **XSD**, **XMPP**.

---

## §5 Overview (body pp. 22–26)

**5.1** — Implement **7-2 ACSI** over **XMPP** + **SNTP** for time sync.

**Figure 1 — profiles (p. 22)**

| Branch | Stack (top → IP) |
|--------|------------------|
| Time sync | **TimeSync (SNTP)** → **UDP** → **IP** |
| Client/server | **XML payloads** → **62351-6** → **XMPP** → **TLS** → **TCP** → **IP** |

Profiles start at **IP**; layers below IP out of scope for this SCSM.

**5.2.1 + Table 1 (p. 23)** — Conformance to this document + declared **7-2** service ⇒ use client/server communication profile for services in **Table 1**:

| 7-2 model | Services (summary) |
|-----------|-------------------|
| Server | GetServerDirectory |
| Association | Associate, Abort, Release |
| Logical Device | GetLogicalDeviceDirectory |
| Logical Node | GetLogicalNodeDirectory, GetAllDataValues |
| Data | Get/SetDataValues, GetDataDirectory, GetDataDefinition |
| Data Set | Get/SetDataSetValues, Create/DeleteDataSet, GetDataSetDirectory |
| SGCB | SelectActiveSG, SelectEditSG, SetEditSGValue, ConfirmEditSGValues, GetEditSGValue, GetSGCBValues |
| Report | Report, Get/Set BRCB/URCB |
| LCB | Get/Set LCB, GetLogStatusValues, QueryLogByTime/After |
| GOOSE ᵃ | Get/SetGoCBValues |
| MSV ᵃ | Get/SetMSVCBValues |
| Control | Select, SelectWithValue, Cancel, Operate, CommandTermination, TimeActivatedOperate |
| FILE | Get/Set/DeleteFile, GetFileAttributeValues |

**ᵃ Not mapped:** SendGOOSEMessage, GetGoReference, GetGOOSEElementNumber, SendMSVMessage, GetMsvReference, GetMSVElementNumber — XMPP cannot meet performance.

**5.2.2 XML payloads (pp. 24–25)** — ACSI req/resp as XML in XMPP **stanzas** after stream to XMPP server; layers per **Figure 2**:

1. Inner: **Service PDU** — e.g. `<confirmed-RequestPDU>`, `<confirmed-ResponsePDU>` — schema **Annex G.3**.
2. Middle: **62351-4** E2E security elements (certs, session keys).
3. Outer: **Association context** (stanza child) — association IDs.

**Example (Fig. 2, p. 25):** `read` → `domainId` **IEDNameLDInst**, `itemId` **LLN0$ST$Beh$stVal** (MMS-style name in XML VMD).

**5.2.3** — E2E security **shall** be supported; may be **off** via deployment config (config out of scope). Security off ⇒ no security wrapper; PDU in association context only (**G.4**); stanza usage still **62351-4**. **Mismatch** (one peer secured, one not) ⇒ association **fails**.

**5.2.4** — XMPP: **RFC 6120–6122**; extensions = **XEPs**.

**5.3 Time sync** — **7-2** time services → **IETF SNTP**; public-network SNTP may need trusted/redundant clocks (**Note 1**); layers **Annex A.2.3**.

---

## §6 Usage of XMPP (body pp. 26 — partial)

| § | Normative detail | CCLI |
|---|------------------|------|
| **6.1** | Distributed **XMPP servers**; IED = **XMPP client**; **account per IED**; **fixed resource** **`IEC`** → `myIED@DSO.org/IEC` | Not TG-524 Eth_A default |
| **6.2.1** | Connect: **TCP** → XML stream → features → **SASL auth** → **resource** → **roster** + **initial presence** (**RFC 6120** + §6.2.2/3) | |
| **6.2.2** | **TLS mandatory**, **62351-6**; (1) TLS in XMPP stream negotiation, or (2) TLS tunnel then stream inside | Same cyber family as MMS TLS |

**Still pending (pp. 27–29):** **6.2.3** stream compression; **6.3** ACSI on stanzas; **6.4–6.7** presence, roster, **XEP 0198/0199**; **§7** E2E security clause text.

---

## §7 End-to-end security (p. 29 — pending body)

Cross-reference **62351-4** when XMPP profile is secured; **not** the Annex T default **MMS + 62351-3/4** profile on Eth_A.

---

## §8 Payload description (body pp. 30–56)

### 8.1–8.2 ACSI → MMS concepts (**Table 2**, p. 30)

| ACSI class | MMS concept |
|------------|-------------|
| Server | VMD |
| Logical Device | Domain |
| Logical Node / DO / all CB classes | NamedVariable |
| DataSet | NamedVariableList |
| Log | Journal |
| Files | Files |

**8.2.2 LN mapping (pp. 30–33)** — Each LN → one **NamedVariable** tree; type from **Figure 4** algorithm over **Figure 5** FC order (MX, ST, CO, CF, DC, SP, SG, RP, LG, BR, GO, …). Empty FC omitted. **`FC=XX` not mapped**. Flat names: path components joined with **`$`**, max **64** chars (**Fig 6–7**, e.g. `CSWI1$ST$Pos$stVal`).

**8.2.3–8.2.6 References (pp. 33–39)** — LN/DO/DA → `<variableAccessSpecification>` / `<domain-specific>` (`domainId`, `itemId`). Examples: **Fig 8** `CONTROL/LLN0`; **Fig 9** alternate **ST** on `GGIO1`; FCD **`XCBR1$ST$Pos`**, FCDA **`XCBR1$ST$Pos$stVal`**. Arrays ⇒ **alternateAccess** with `<index>` (**Figs 12–14**); prefer **`<unnamed/>`** choice; **`$` forbidden** in `<component>` values.

### 8.3 Type mapping (pp. 40–53)

**Table 3 — BasicTypes → XML (`complexType Data`, Annex G):** e.g. `<boolean>`, `<integer>`, `<unsigned>`, `<floating-point>` as **4-octet IEEE754 hex**, `<bit-string>`, `<octet-string>`, `<visible-string>`, `<mMSString>` (control chars as `<nul/>`, etc.).

**8.3.2 rules:** **BIT STRING** — bit(0) leftmost; **no zero-length** bit-strings. **ENUMERATED** → `<integer>`; extensible values **no protocol error**. **CODED ENUM** → `<bit-string>`. **OCTET/VISIBLE/UNICODE** and **array** rules (**8.3.2.4–7**).

**8.3.3 ObjectReference (pp. 43–44)** — Always **VISIBLE STRING**, max **129** octets. Control block scopes: `@name`, `domain/name`, `/name` (name/domain max **64**). Report **DataRef** FCD/FCDA: e.g. `LD1/MMHA1$MX$HA$phsAHar(7)`, `…$cVal$mag$f`.

**PhyComAddr — Table 4 (L2), Table 5 (UDP/IP):** MAC 6 octets multicast; **Addr** 4 or 16 octets IPv4/IPv6 + **isIPv6**; **APPID** per **8-1** CSV Annex C; routable fields **gwAddr**, **tOS**, **iGMPSrc**.

**8.3.3.4 ServiceError (pp. 45–50):** Layers — XMPP stream/stanza errors (**RFC 6120**); **62351-4** association + security; non-**Table D.1** mapping → **`rejectPDU`** `unrecognized-service`; unsupported service → **reject** with `servicesSupportedCalled` / **10.2.2.1.4** rules. **Tables 6–10:** GetNameList conflicts, Read/Write **DataAccessError**, FileDirectory, etc.

**Encodings (pp. 50–53):** **EntryID** 8-octet fixed; **PACKED LIST** bit order; **Timestamp** `<utc-time>` 8 octets + **Table 11** TimeQuality; **EntryTime** `<binary-time>` 6 octets; **Tables 12–17** TriggerConditions, ReasonForInclusion (report/log), **RCBReportOptions**, **SVMessageOptions**, **CheckConditions**; **LCBLogEntryOptions: not mapped**.

### 8.3.4 Quality + 8.4–8.5 (pp. 54–56)

**Quality** — PACKED LIST / bit-string, bits **0–15** (**Table 18**: Validity, Overflow, …, Source, Test, OperatorBlocked). Default validity **good = 00**.

**8.4** — Values in `<structure>` / `<Data>` wrappers; **Table 19** example `CONTROL/GGIO1$ST$Ind3` vs `$stVal` only.

**8.5 Bandwidth optimization** — Optional **`extendedGetNameList`** negotiated at **Associate** (`servicesSupportedCalled`); replaces `getNameList` / `getVariableAccessAttributes` for **GetLogicalDeviceDirectory**, **GetLogicalNodeDirectory**, **GetDataDirectory** — same structure, **mandatory-only** response fields.

**CCLI:** Same **`$`-flattening** and **Quality** semantics apply on **61850-8-1 MMS** for Eth_A; XMPP-specific parts are **P2**.

---

## §9–§14 ACSI service mapping (body pp. 57–98)

**Wire pattern:** ACSI services → MMS-style XML inside `<confirmed-RequestPDU/>` / `<confirmed-ResponsePDU/>` (or `<initiate-*PDU/>`, `<rejectPDU/>`, `<confirmed-ErrorPDU/>`) in XMPP stanzas (**§6.3**, **Annex D**). Pagination: **`moreFollows`** + **`<continueAfter/>`**.

### §9 GetServerDirectory (pp. 57–59, **Tables 20–21**, **Figs 15–18**)

| ACSI class | XML | Notes |
|------------|-----|--------|
| **LOGICAL-DEVICE** | `<getNameList/>`, `<objectClass><domain/></objectClass>`, `<objectScope><vmdSpecific/></objectScope>` | **Fig 15–16**; LD names in `<listOfIdentifier/>`, ASCII lexicographic order; client-visible LDs only |
| **FILE** | `<fileDirectory/>`, optional `<fileSpecification>*</fileSpecification>`, `<continueAfter/>` | **Table 21**, **Figs 17–18**; errors **8.3.3.4.11**; long names split across multiple `<GraphicString/>` (64 chars) |

**Annex T / CCLI:** Same logical operation as **Listobjects** / server directory on **8-1 MMS** Eth_A.

### §10 Association (pp. 60–66, **Tables 22–28**, **Figs 19–20**)

| Service | XML PDU | Key rules |
|---------|---------|-----------|
| **Table 22** | Client/server + Time Sync → **two-party** association | |
| **Associate** | `<initiate-RequestPDU/>` / `<initiate-ResponsePDU/>` / `<initiate-ErrorPDU/>` | **SAPRef** → XMPP `<iq to="JID"/>` (**Table 23**); auth in **62351-4** wrapper |
| Negotiation | **Tables 24–25** | PDU size, max outstanding, nesting level **≥5** if LN model; **`servicesSupportedCalling/Called`** incl. **`extendedGetNameList`** for §8.5 |
| **Fig 20** example | Server may advertise: read, write, getNameList, **extendedGetNameList**, file*, etc. | PICS subset in production |
| Errors | **Table 26** | `version-incompatible`, CBB/service insufficient, `object-access-denied` |
| PDU size | **10.2.2.1.3** | `max_mms_pdu_size` negotiated; **≥8192** octets recommended; oversize → **rejectPDU** or **pdu-size** |
| Versioning | **10.2.2.1.4** | Optional **version/revision/release** on initiate PDUs; unknown elements w/o `mustUnderstand` ignored |
| **Abort** | **No wire PDU** — local indication (XMPP link loss → abort all assocs on JID; presence **unavailable** → abort that peer) | **10.2.1** |
| **Release** | `<conclude-RequestPDU/>` / `<conclude-ResponsePDU/>` / `<conclude-ErrorPDU/>` | **Tables 27–28** |

### §11 GetLogicalDeviceDirectory (pp. 66–69, **Table 29**, **Figs 21–22**)

- Request: `<getNameList/>`, `<objectClass><namedVariable/></objectClass>`, `<domainSpecific><LDName>`.
- Response: flattened LN variable names — client **filters out entries containing `$`** to recover **LN names** only; lexicographic order; **`moreFollows`** pagination.
- Extended behaviour (**11.3**): same pattern as **§8.5** with **`extendedGetNameList`** when negotiated.

### §12 Logical Node (pp. 70–76)

| Service | MMS/XML | Annex T |
|---------|---------|---------|
| **GetLogicalNodeDirectory** | `<getNameList/>` on domain; lists components under LN (Tables **30–33**, **Figs 25–26**) | Listobjects |
| **Extended** | **`extendedGetNameList`** per FC list (**Figs 27–30**) | |
| **GetAllDataValues** | `<read/>` on LN reference (**Figs 31–32**) | Readvalues |

### §13 Data / directory (pp. 79–87)

| Service | Mapping | TR / notes |
|---------|---------|------------|
| **GetDataValues** | `<read/>` + `<variableAccessSpecification/>` (**Figs 33–34**) | Readvalues |
| **SetDataValues** | `<write/>` (**Figs 35–36**) | CONTROL gated |
| **GetDataDirectory** | `<getVariableAccessAttributes/>` default; **Fig 38** type spec (stVal, q bit-string 13, t utc-time) | Listobjects |
| **Extended GetDataDirectory** | **`extendedGetNameList`** per **Table 39**, **Figs 39–40** — one FC per request; `domainSpecific` = `LDName/flattenedRef` e.g. `CONTROL/LLN0$ST$Mod`; response lists **one level** of child names | Bandwidth opt |
| **GetDataDefinition** | Same XML as **GetDataDirectory** but **no** application-level filtering (**13.5**) | |
| Errors | **Table 38** + **8.3.3.4.8–9** | |

### §14 DataSet (pp. 87–98, **Tables 40–47**, **Figs 41–53** start)

**14.1 References** — DataSetRef → `<variableListName/>`: **domain-specific** `domainId` + `itemId` **`LNName$DSName`** (**Fig 41**); **vmd-specific** / **aa-specific** for other persistence classes (**Figs 42–43**).

| Service | XML | TR 57-126 |
|---------|-----|-----------|
| **GetDataSetValues** | `<read/>` on variable list; **Table 40**, **Figs 44–45**; per-member `<success/>`/`<failure/>` in `<listOfAccessResult/>` | **Dataset read** — core TR path |
| **SetDataSetValues** | `<write/>` + `<listOfData/>` (**Figs 46–47**, **Tables 41–43**) | Often restricted |
| **CreateDataSet** | `<defineNamedVariableList/>` + member FCD/FCDA list (**Table 44**, **Figs 48–49**) | Annex T **N/A** |
| **DeleteDataSet** | `<deleteNamedVariableList scopeOfDelete="specific"/>` only — **aa/domain/vmd bulk delete out of scope** (**Figs 50–51**, **Table 46–47**) | Annex T **N/A** |
| **GetDataSetDirectory** | `<getNamedVariableListAttributes/>` — **Table 48**, **Figs 52–53**; members as **FCD/FCDA** in `<listOfVariable/>`; **Table 49** errors (**pdu-size**) | Dataset |

**DeleteDataSet semantics:** not found → `numberMatched=0`; exists but not deletable → matched=1, deleted=0; locked (e.g. RCB) → **Response-** with **object-constraint-conflict**.

**CCLI P0:** Implement **GetDataSetValues** + directory/read path on **61850-8-1** against TR CID; **Create/Delete** dataset not required for Annex T profile.

---

## §15–§19 body (pp. 99–131)

### §15 ServiceTracking (pp. 101–103)

- Based on **7-3** CDCs; modified **LTS** / **GTS** for this SCSM (**Tables 52–53**).
- **Table 50–51:** **serviceType** (0=Unknown … 55=GetLogicalNodeDirectory) and **errorCode** (0=no-error … 12=failed-due-to-server-constraint).
- Footnote **ᵃ:** services **not mapped** in 8-2 (e.g. **SendGOOSEMessage**, **GetGoReference**, **SendMSVMessage**) may still appear in tracking when executed via another SCSM.

### §16 Setting groups (pp. 104–109)

- **SGCB** on **LLN0**, **FC=SP** — **Table 54** (`NumOfSG`, `ActSG`, `EditSG`, `CnfEdit`, `LActTm`, `ResvTms`).
- All SG services are **specializations** of **GetDataValues** / **SetDataValues**:
  - **SelectActiveSG** → write **`LLN0$SP$SGCB$ActSG`** (**Fig 54**).
  - **SelectEditSG** → **`EditSG`** (**Fig 57**).
  - **SetEditSGValue** → target **FC=SE** (after **SelectEditSG**) (**Fig 58**).
  - **ConfirmEditSGValues** → **`CnfEdit=true`** then returns **FALSE** after NV write (**PIXIT**) (**Figs 59**).
  - **GetEditSGValue** → **FC=SE** or **SG** (**Figs 60–61**).
  - **GetSGCBValues** → read **`LLN0$SP$SGCB`** structure (**Figs 62–63**).
- SG write errors use **8.3.3.4.10** / per-attribute **success/failure** (**Fig 56** `object-non-existent`).

**CCLI:** optional; not TR 57-126 core CID path.

### §17 Reporting and logging (pp. 110–130)

**17.1 RCBs**

| Block | FC | Key rules |
|-------|-----|-----------|
| **BRCB** **Table 55** | **BR** | Many attrs locked when **RptEna=TRUE**; **PurgeBuf** on RptID/DatSet/BufTm/TrgOps/IntgPd/PurgeBuf writes; **indexed** RCB names **01–99** per **61850-6**; **Owner** = reserving client **JID** (diag only) |
| **URCB** **Table 56** | **RP** | **Resv**; **OptFlds** buffer-overflow/entryID forced false; **SqNum→0** on enable |

**17.2 Reporting services**

- **Report (17.2.1):** `<unconfirmed-PDU/>` + `<informationReport/>`; **variableListName** **vmd-specific** `RPT`; **Table 57** fixed **AccessResult** order (RptID, Reported OptFlds, conditional SeqNum/TimeOfEntry/DatSet/…, inclusion bitstring, values, ReasonCodes).
- **GetBRCBValues (17.2.2):** `<read/>` on BRCB ref **FC=BR** → one **structure** of all attrs (**Tables 58**, **Figs 65–66**); per-member data-ref/value/reason order matches dataset.
- **SetBRCBValues (17.2.3):** one **`<variableSpecification/>` + `<Data/>` per attribute** (**Table 59**, **Figs 67–68**); independent per-attr success/failure; may limit to one attr/call (**PICS/PIXIT**).
- **Get/SetURCBValues (17.2.4–5):** same pattern as BRCB with **FC=RP** (body on pp. 122 — same wire model).

**17.3 Log model**

- **LCB** merges 7-2 log-control + log object attrs (**Fig 69**, **Table 60**); **OptFlds** / **bufTm** not mapped (reasonCodes always in log; no extra buffer).
- **LogRef** default **`LD/LLN0$GeneralLog`**; writes blocked when **LogEna=TRUE** for LogRef/DatSet/TrgOps/IntgPd.
- **Get/SetLCBValues**, **GetLogStatusValues** ≡ BRCB pattern (**FC=LG**).
- **QueryLogByTime / QueryLogAfter:** `<readJournal/>` on **`domainId` + `LNName$LogName`** (**Tables 61–63**, **Figs 70–72**); entries: **entryIdentifier** (8-byte hex, not all-zero), **occurenceTime** (6-byte), **listOfVariables** = dataRef+value sequence + **ReasonCode** bit-string; **LogEna** state → **disabled/idle/active** event entries.
- **Conformance Table 64:** server **m** on GetLCB, QueryLog*, SetLCB; client **c1** = one of QueryLogByTime or QueryLogAfter.

**CCLI P0:** **URCB/BRCB + Report** on **61850-8-1** (TR **intgPd=4000**); security audit via **62351-14**, not 61850 log.

### §18 GOOSE (pp. 130–131)

- **GoCB Table 65:** GoEna (rw), GoID, DatSet, ConfRev, NdsCom, **DstAddress** (L2 **Table 4** / routable **Table 5**), MinTime/MaxTime/FixedOffs optional from SCD.
- **18.2 / 18.3:** **FC=GO** (L2) vs **FC=RG** (routable UDP/IP).
- **18.4.1:** **GetGoReference**, **GetGOOSEElementNumber**, **SendGOOSEMessage** **not mapped** on XMPP (performance) — use **61850-8-1**.
- **GetGoCBValues / SetGoCBValues:** like BRCB; **Set** limited to **GoEna**.

### §19 Sampled values (pp. 131–132)

- **19.1.1:** Multicast/unicast MSVCB per **61850-9-2** except **FC** and **DstAddress**.
- **19.1.2 Layer 2:** **FC=MS**; **DstAddress** = **PhyComAddr** L2 (**8.3.3.3.1**).
- **19.1.3 Routable:** **FC=RS**; **DstAddress** = UDP/IP (**8.3.3.3.2**).
- **19.1.4 Unicast:** **FC=US**; **DstAddress** = L2 PhyComAddr.
- **19.2.1:** Only **Get/Set MSVCB** attrs mapped; **SendMSVMessage**, **GetMsvReference**, **GetMSVElementNumber** **not mapped** on XMPP (performance) — use **8-1** / **9-2** wire.
- **19.2.2 GetMSVCBValues:** like **GetBRCBValues**; read MSVCB **FC=MS** or **RS**.
- **19.2.3 SetMSVCBValues:** like **SetBRCBValues** (enable/config attrs).

**CCLI:** out of P0 scope; no SV on TG-524 Eth_A.

### §20 Control class model (body pp. 132–152)

**20.1 General**

- Maps **7-2** control service parameters to **CO** data-attribute sub-fields on controllable DOs (**ctlModel ≠ status-only**).
- Ref: **`CO_CtrlObjectRef`** = `<LD>/<LN>$CO$<DO>`; components **OPER / CANCEL / SBO / SBOw** with sub-attrs per **Table 66** (`ctlVal`, `operTm`, `origin`, `ctlNum`, `T`, `Test`, `Check`, `AddCause` types in **§8.3**).
- **Table 67:** **ctlModel** → mandatory components (e.g. SBO-normal: **SBO+Oper+Cancel**; SBO-enhanced: **SBOw+Oper+Cancel**; direct-normal: **Oper** + optional **Cancel**).

**20.2 Overview — Table 68**

| ACSI | Wire pattern |
|------|----------------|
| **Select** | **Read-Request** on **`$SBO`**; **Response+** visible-string **CO_CtrlObjectRef**; **Response−** empty visible-string |
| **SelectWithValue / Cancel / Operate / TimeActivatedOperate** | **Write-Request** on **`$SBOw` / `$Cancel` / `$Oper`**; **Response+** write **success**; **Response−** **informationReport** (**LastApplError**) + write **failure** |
| **CommandTermination / TimeActivatedOperateTermination** | **informationReport** on **`$Oper`** (or **LastApplError** only on TA term **Request−**) |

**20.3 Select (Tables 69, Figs 73–74)**

- Request: `<read/>` on **`itemId` … `$CO$…$SBO`** (**Fig 73**).
- **Response+:** `<success/>` with **visible-string** full **CO_CtrlObjectRef** (e.g. `CONTROL/GGIO1$CO$DPCSO1`).
- **Response−:** `<success/>` with **empty** visible-string.

**20.4 SelectWithValue (Tables 70–71, Figs 75–77)**

- Write structured **SBOw** sub-attrs (**Table 70**); example **`GGIO1$CO$DPCSO1$SBOw`** (**Fig 75**).
- **Response+:** `<write><CHOICE><success/></CHOICE>` (**Fig 76**).
- **Response−:** ordered messages — **(1)** `<unconfirmed-PDU/>` **LastApplError** (**§20.9**); **(2)** `<confirmed-ResponsePDU/>` **failure** with **DataAccessError** (**Table 72**: TEMPORARILY-UNAVAILABLE, HARDWARE-FAULT, OBJECT-ACCESS-DENIED, OBJECT-UNDEFINED) (**Fig 77**).

**20.5 Cancel (Tables 73–74)**

- Write **`$Cancel`** sub-attrs (no **Check** in **listOfData**); **AddCause** via **LastApplError** on negative.
- **Response−** same two-message pattern; **informationReport** optional for **normal security**, mandatory for **enhanced** (footnote **b**).

**20.6 Operate + CommandTermination (Tables 75–77, Figs 78–79)**

- Write **`$Oper`** including **Check** (**Table 75**); negative mapping as Cancel (**20.6.3**).
- **CommandTermination request+:** `<unconfirmed-PDU/>` **informationReport** — read **`$Oper`** structure in **listOfAccessResult** (**Fig 78**); **operTm=0** in termination when applicable.
- **CommandTermination request−:** two variables — **LastApplError** then **`$Oper`** (**Fig 79**).

**20.7–20.8 TimeActivatedOperate**

- Same as **Operate** but **operTm** present with valid timestamp; **operTm=all-zero** ≡ enhanced security / non-time-activated.
- **TimeActivatedOperateTermination:** like **CommandTermination**; **Request+** **operTm** matches request; **Request−** only **LastApplError**.

**20.9 LastApplError (Table 78, Fig 80)**

- **AddCause** in negative responses → instant **vmd-specific** **LastApplError** structure: **CntrlObj** (visible-string, max 129 octets), **Error** (0–3: No-Error, Unknown, TimeOut-Test-Not-OK, Operator-Test-Not-OK), **Origin**, **ctlNum**, **AddCause**.
- Volatile — defaults after association; **origin/ctlNum** let client match **informationReport** to pending **write**.

**20.10 Control tracking (CTS) — Table 80**

- **CTS** CDC inherits **CST** components; adds **ctlVal**, **operTm**, **origin**, **ctlNum**, **T**, **Test**, **Check**, **respAddCause** (enum per **Table 79**).

**CCLI P0:** implement **Select/Operate/Cancel/CommandTermination** on **61850-8-1 MMS** for Annex T **CONTROL** DOs; use **Table 79** **AddCause** in logs; XMPP wire **P2** reference only.

**Annex T:** CONTROL privilege on DSO-reserved DOs; **ctlModel** from **61850-6** templates.

---

### §21 Time and time synchronization (p. 153)

- **§21:** Time synchronization shall use **SNTP** per **§5.3**; GPS/hardware sync mechanisms out of scope of this document.

**CCLI:** production **Eth_A** uses **NTP/NTS** per **TR 57-126 Annex T**, not XMPP T5 profile.

### §22 Naming conventions (p. 153)

- Points to **§8.2** flat **`itemId`** / object-reference rules (already captured in **W2**).

### §23 File transfer (body pp. 153–165)

**23.1 Model — Tables 81–82**

- **7-2 file class** mapped to MMS-like file object: **FileName**, **FileSize**, **LastModified** (mandatory).
- **FileName:** path required; dir names ≤**32** octets; max **255** octets total; delimiter PIXIT (`/` or `\`); LD-related files under root **`LD/<LDName>/`**.
- Reserved suffixes: **Bin, Dtd, Gif, Htm, Pqd, txt, Xml, Xsd, Zip**; COMTRADE under **`COMTRADE/`** (`.cfg/.dat/.hdr`, optional `.zip`).
- **LastModified:** **UTC** recommended (`xsd:dateTime`); **FileSize** is estimate (0 = unknown/empty).

**23.2 Services**

| Service | Wire (XMPP XML) | Notes |
|---------|-----------------|-------|
| **GetFile** | **FileOpen** (`initialPosition=0`) → **FileRead** (`moreFollows` true/false) → **FileClose** (**Fig 81**, **Table 83**, **Figs 82–89**) | **fileData** hexbinary; concat for **File-Data**; errors **Table 84** (`file-access-denied`, `file-non-existent`, …) |
| **SetFile** | **FileOpen** + **FileWrite** sequence + **FileClose** | Same unitary PDU pattern as GetFile (tables **85–88** region) |
| **ObtainFile** | Remote fetch + **destinationFile** | Errors **Table 89** (access / source-file / destination-file) |
| **DeleteFile** | `<fileDelete/>` + **GraphicString** path (**Table 90**, **Figs 93–94**) | |
| **GetFileAttributeValues** | `<fileDirectory/>` + single **fileSpecification**; response one **directoryEntry**, **moreFollows=false** (**Table 92**, **Figs 95–96**) | |

**CCLI P2:** firmware/CID via **8-1** file services when enabled; policy **62443-4-2** + Secure Boot.

### §24 Conformance (body pp. 166–189)

**24.2 PICS**

- **A-Profile / T-Profile** tables (**93–95** region): **Table 95** — **T1** TCP/IP **m**, **T7** security **m**, **T5** time sync **c** if time-activated control or logging; **T3/T6** GOOSE/SV T-profiles **i** (out of scope for XMPP client/server focus).
- **24.2.2 XML payload conformance** — MMS-style **Initiate** parameters (**Table 96**: **proposedVersionNumber=1**, PDU size, nesting ≥5 if LN model, **Table 99** CBB).
- **Variable access:** **AlternateAccess** only for **arrays**; **`component`/`componentName` must not include `$`** (**24.2.2.2.1.1**); conformant examples **Figs 97–98, 101–102** → **DataRef** `LDevice/MHAI1$MX$HA$phsAHar(7)$cVal$mag$f`; non-conformant **Figs 99–100** (`mag$f`, `MX$HA$phsAHar` in component).
- **Tables 106–122:** conformance for **AlternateAccess**, **Read/Write**, **InformationReport**, NVL define/get/delete, **ReadJournal** + **EntryContent**, **FileDirectory/Open/Read/Close** (all **frsmID** **m** on close).
- **Cancel/Identify MMS services (Tables 104–105):** not used in 61850 mapping but suggested for MMS interoperability.

**24.3 PICS statement**

- Complete full PICS + **7-2** basic statement; **SCL** support per **61850-6** (**24.3.2**); online SCL guidance **Annex F** (informative).

### §25 SCL extensions (p. 189)

- **ConnectedAP** extended for this SCSM; use **61850-6** XSD.
- **Table 123:** allowed **P-Type** for Address — **`JID`** (Jabber ID, RFC 6122); optional in ICD/SCD; XMPP resource per **§6.1** or omitted.

**CCLI:** TR **8-MMS** CID uses IP/subnet P-types from **61850-6** extract — **JID** only if lab XMPP profile.

---

## Annex A — Communication stack (normative, pp. 190–195)

**A.1 Overview**

- Layered view for readers of **61850-8-1** / OSI; not required except **Clause 24** PICS references profiles.
- **Fig A.1:** two stacks on **IP** — (1) **TimeSync (SNTP)** Type **6**, **non-XMPP**; (2) **Client/server ACSI** — **XML payloads (XER MMS PDUs)** Types **2, 3, 5** over **XMPP T-Profile**.
- **61850-5** message types **1, 1A, 4** are **out of scope** for this SCSM; **2, 3, 5, 6** are in scope per mapping.

**A.1.2–A.1.3:** Each profile = one **A-Profile** (OSI layers 5–7) + one **T-Profile** (layers 1–4). Physical/link below IP out of scope.

**A.2 Communication stack (Fig A.2)**

- **A.2.2 Client/server:** Conformance shall declare **Table 1** services.

**Table A.1 — Client/server A-Profile**

| Layer | Name | Spec | m/o |
|-------|------|------|-----|
| Application | MMS | ISO 9506-1/2 | m |
| Application | End-to-end security | IEC 62351-6 | m |
| Presentation | Abstract syntax | ISO/IEC 8824-1 + **ITU X.693 (XER)** | m |
| Presentation | M-Services → E2E mapping | **Annex D** | m |
| Session | Association context | IEC 62351-4 | m |

- Reuses **8-1** ACSI→MMS object mapping; encoding is **XER** (not **BER**); normative XSD **Annex G.3**.
- **Associate:** both sides generate locally unique **AssociationID** within the **JID** pair; present in all applicative messages.

**Table A.2 — XMPP T-Profile**

| Layer | Name | Spec | m/o |
|-------|------|------|-----|
| Transport | XMPP | RFC 6120, 6121, 6122; XEP-0198, 0199 | m |
| Transport | SASL | RFC 4422 | m |
| Transport | TLS | RFC 5246; IEC 62351-6 | m |
| Transport | TCP | RFC 793 | m |
| Network | ICMP | RFC 792 | m |
| Network | IPv4 | RFC 791 | c1, c2 |
| Network | IPv6 | RFC 2460 | c1, c2 |

- **c1:** client uses IPv4 and/or IPv6; **c2:** server supports **both** IPv4 and IPv6.

**A.2.3 Time sync (Tables A.3–A.4)**

- Mandatory if implementation declares **Timestamp** attributes.
- **Table A.3:** Application **SNTP** — **RFC 5905** (**m**); limited to SNTP subset; also **RFC 6864**, **RFC 7766**; modes **3** (client), **4** (server), both if dual role.
- **Table A.4:** Transport **UDP** (**RFC 768**), **ICMP**; Network **IPv4** (**m**), **IPv6** (**o**), ARP/multicast RFCs as listed.
- **A.2.3.3.2:** SNTP client **shall** use IANA **UDP port 123** as destination.

**CCLI:** Eth_A = **8-1** MMS + plant GOOSE; Annex A documents optional **8-2** stack only.

---

## Annex B — XMPP infrastructure deployment (informative, pp. 196–209)

**B.1:** XMPP may be federated or standalone; public or private infrastructure.

**B.2 Facility domain (Fig B.1):** Example domain `facil.org` — central **XMPP server**; **DER Management** `management@facil.org` (61850 client+server / XMPP client); field **DER** units `DERa@facil.org` … as **61850 server / XMPP client**.

**B.3–B.3.3:** Multi-domain **DSO.org**, **VPP.com**, **VPP.net** — hierarchy of control centres, domain XMPP servers, DER systems (**Figs B.2–B.16**).

**B.3.3.4–B.3.3.5 (pp. 208–209):** Federated paths — e.g. `management@VPP.com` ↔ `DER@DSO.org` via **inter-domain XMPP federation** (**Figs B.17–B.18**).

**B.4 Communication path outage and recovery (p. 209):**

- XMPP servers may be **clustered** with **connection managers**; DER clients connect outbound (NAT/firewall friendly).
- Discovery: static IP or **DNS SRV** (weighted list); on path failure, client tries next SRV entry.

**CCLI:** Informative only — TR **8-MMS** has no XMPP federation requirement.

---

## Annex C — Security for DER integration (informative, pp. 210–219)

**C.1:** DER integration context; **MUC/multicast** mentioned but **8-2 SCSM scope = unicast** only. **Fig C.1:** control side vs loads/generators; hop-to-hop vs E2E unicast/multicast; **NAT/firewall** on prosumer side.

**C.2 Assumptions A1–A6 (selected):**

- **A4:** XMPP server may be DSO/TSO/telecom; may support **MUC**.
- **A5:** Server operator **trusted** for discovery and message transfer.
- **A6:** Server operator **not trusted** to read or modify message content → **E2E** required for payload confidentiality/integrity.

**C.3:** Security goals per **62351-1**; requirements **R1–R6:** E2M auth/integ/conf (client↔XMPP server) vs E2E auth/integ/conf (61850 client↔server), unicast and (for R4/R6) multicast/MUC.

**C.4 Unicast mapping:**

- Hop-to-hop: **TLS** (**RFC 6120**, settings **62351-6**), **X.509**, CA at server, optional client whitelist; addresses **R1, R3, R5**.
- E2E: **IEC 62351-4** on applicative payloads — **R2, R4a, R6a**.

**C.5 Sequence diagrams:**

- **C.5.2 Figs C.2–C.3:** `openCommunication(Jid, Credentials)` → `OpenStream`; **SASL EXTERNAL** (cert includes JID) or **SASL PLAIN** (password per JID); E2E **Configuration(Security Policy)**.
- **Fig C.4:** Roster get/push, **presence(available)**; subscriber relationship between client JIDs.
- **C.5.4 Figs C.5–C.6:** Outage → **CloseSession(Jid, assoID)** → **Abort** to 61850 peer (local link loss vs remote **presence unavailable**); loop all `{Jid, assoID}` contexts.
- **Figs C.7–C.8:** **clearTransfer** / **encrTransfer** over **`iq`** for **Write** req/resp; verify token/authenticator; failures → **ClearTransferError**, **dtSecAbort**, **Abort**.

**CCLI:** Use **62351-3/4/6** on **MMS** path; Annex C is DER/XMPP reference architecture.

---

## Annex D — Services and errors over XMPP stanzas (**normative**, pp. 220–222)

Wire companion to **§6.3** and **62351-4** wrapping. **Note 1:** E2E security wraps envelopes; **`<ApplData/>`** at security/applicative boundary. **Note 3:** Service element names align with **61850-8-1** MMS services.

### Table D.1 — ACSI services mapping (excerpt)

Context column: **ClearTransfer or EncrTransfer**.

| Envelop | Service element (examples) | Stanza | Type |
|---------|---------------------------|--------|------|
| **confirmed-RequestPDU** | getNameList, Read, getVariableAccessAttributes, readJournal, fileRead, fileDirectory, obtainFile, extendedGetNameList, … | iq | **get** |
| **confirmed-RequestPDU** | Write, defineNamedVariableList, deleteNamedVariableList, fileOpen, fileClose, fileDelete | iq | **set** |
| **confirmed-ResponsePDU** | (same service set) | iq | **result** |
| **confirmed-ErrorPDU** | — | iq | **result** |
| **unconfirmed-PDU** | informationReport | message | **normal** |
| **rejectPDU** | — | message | **normal** |
| **cancel-RequestPDU** | — | iq | **get** |
| **cancel-ResponsePDU** / **cancel-ErrorPDU** | — | iq | **result** |
| **conclude-RequestPDU** | — | iq | **set** |
| **conclude-ResponsePDU** / **conclude-ErrorPDU** | — | iq | **result** |
| **HandshakeReq/Acc**, **ApplicationReject** | initiate-Request/Response/Error PDU | — | **See IEC 62351-4** |

### Table D.2 — Error mapping over XMPP stanzas

| Layer / condition | XMPP concept | Type | Namespace / detail |
|-------------------|--------------|------|-------------------|
| Unrecoverable stream/stanza fault | stream:error / iq / message | error | RFC 6120 `urn:ietf:params:xml:ns:xmpp-streams` |
| Recoverable stanza fault | iq / message | error | RFC 6120 `urn:ietf:params:xml:ns:xmpp-stanzas` |
| Unknown **assoID**, E2E security on initiate PDUs | — | — | **IEC 62351-4** |
| initiate-ErrorPDU, mapping/reject errors | message | normal | Payload as **rejectPDU** (**Table D.1**) |
| Errors during established association | iq / message | result / normal | **confirmed-ErrorPDU**, **rejectPDU**, **cancel/conclude-ErrorPDU**, etc. |

Footnotes **a:** **§8.3.3.4.5–4.6**; **b:** 7-2 service errors and SCSM-specific errors.

**CCLI:** P0 stack testers use **8-1** MMS PDUs; keep **Table D.1** for optional XMPP interop / competitor analysis.

---

## Annex F — SCL conformance (informative, TOC p. 224)

Informative SCL conformance notes — primary SCL grammar remains **`ccli-61850-6-extract`**.

---

## Annex G — XML schema definitions (**normative**, TOC p. 225)

| § | Schema | Page |
|---|--------|------|
| **G.2** | **Virtual API** for IEC 61850-8-2 | 225 |
| **G.3** | **Applicative payload** for IEC 61850-8-2 | 225 |

**Ingest/parser:** validate XML payloads against G.3 when implementing XMPP client/server (P2).

---

## Figures index (TOC — selected for mapping)

| Fig | Title | Page |
|-----|--------|------|
| **1** | Overview of functionality and profiles | 22 |
| **2** | Example of **XML payload** | 25 |
| **3** | Generic client/server ACSI services | 27 |
| **4** | Algorithm for logical node mapping | 30 |
| **6** | Example LN type description | 32 |
| **7** | Flattened Named Variables for an LN | 33 |
| **8–9** | LNReference XML — direct vs alternate access | 34 |
| **10–11** | Direct XML mapping of **FCD** / **FCDA** | 35 |
| **12** | Alternate access without array element | 36 |
| **13** | Alternate access with array element | 38 |
| **14** | Alternate access with flattened variable and array element | 39 |

### Figures — XML request/response structures (TOC p. 7)

| Fig | Service / topic | Page |
|-----|-----------------|------|
| **15–16** | **GetServerDirectory-Request/Response (LD)** | 58 |
| **17–18** | **GetServerDirectory-Request/Response (FILE)** | 60 |
| **19–20** | **Associate-Request/Response** | 63 |
| **21–22** | **GetLogicalDeviceDirectory-Request/Response** | 68 |
| **23–24** | Extended **GetLogicalDeviceDirectory** | 70 |
| **25–26** | **GetLogicalNodeDirectory-Request/Response** | 72–73 |
| **27–30** | Extended **GetLogicalNodeDirectory** (steps 1–2) | 75–76 |
| **31–32** | **GetAllDataValues-Request/Response** | 78–79 |
| **33–34** | **GetDataValues-Request/Response** | 80–81 |
| **35–36** | **SetDataValues-Request/Response** | 82 |
| **37–38** | **GetDataDirectory-Request/Response** | 83–84 |
| **39–40** | Extended **GetDataDirectory** | 86–87 |
| **41–43** | DataSet reference mapping (persistent in/out LD, non-persistent) | 87–88 |
| **44–45** | **GetDataSetValues-Request/Response** | 89–90 |
| **46–47** | **SetDataSetValues-Request/Response** | 92 |
| **48–49** | **CreateDataSet-Request/Response** | 94–95 |
| **50–51** | **DeleteDataSet-Request/Response** | 97 |
| **52–53** | **GetDataSetDirectory-Request/Response** | 99–100 |
| **54–55** | **SelectActiveSG-Request/Response+** | 105 |

**Test vectors:** Fig. 15–36 align with **Annex T** read path; Fig. 44–45 with TR **DataSet** reads; Fig. 35–36 gated by CONTROL privilege.

### Figures 56–97 — SG, reports, logs, control, files (TOC p. 8)

| Fig | Service / topic | Page |
|-----|-----------------|------|
| **56** | SelectActiveSG-Response | 105 |
| **57** | SelectEditSG-Request | 106 |
| **58** | SetEditSGValue-Request | 107 |
| **59** | ConfirmEditSGValues | 108 |
| **60–61** | GetEditSGValue Request/Response | 108–109 |
| **62–63** | GetSGCBValues Request/Response | 109–110 |
| **64** | **Report** (XML structure) | 115 |
| **65–66** | **GetBRCBValues** Request/Response | 117–119 |
| **67–68** | **SetBRCBValues** Request/Response | 121–122 |
| **69** | LCB attributes ↔ **7-2** log definitions | 123 |
| **70–71** | **QueryLogByTime** Request/Response | 126–127 |
| **72** | QueryLogAfter-Request | 129 |
| **73–74** | **Select** Request/Response | 135–136 |
| **75–77** | **SelectWithValue** Request/Response (+/−) | 138–139 |
| **78–79** | **CommandTermination** Request (+/−) | 146–148 |
| **80** | InformationReport + **AdditionalCauseDiagnosis** | 150 |
| **81** | **GetFile** → FileOpen / FileRead / FileClose mapping | 156 |
| **82–89** | FileOpen / FileRead / FileClose XML examples | 158–159 |
| **90** | **SetFile** service mapping | 161 |
| **91–92** | **ObtainFile** Request/Response | 162 |
| **93–94** | **DeleteFile** Request/Response | 164 |
| **95–96** | **GetFileAttributeValues** Request/Response | 165–166 |
| **97** | VariableSpecification example (`LDevice/.../mag.f`) | 178 |

**CCLI mapping:** Figs **64–68** ↔ TR **`brcb_*` / `urcb_*`** and **intgPd=4000**; Figs **73–79** ↔ curtailment **Operate** path; Figs **81–96** ↔ §23 signed file policy.

### Figures 98–102 — VariableSpecification examples (TOC p. 9)

| Fig | Topic | Page |
|-----|--------|------|
| **98–102** | `VariableSpecification` for `LDevice/MHAI1.HA.phsAHar(7).cVal.mag.f` (detail, short, non-conformant) | 179–183 |

### Annex figures (TOC p. 9)

| Fig | Topic | Page |
|-----|--------|------|
| **A.1–A.2** | Functionality/profiles; OSI model and profiles | 191–192 |
| **B.1–B.11** | XMPP domains, DER hierarchy, DSO/VPP **JID** control | 196–204 |
| **B.12–B.14** | XMPP **federation** (DSO / facility / VPP) | 205–206 |
| **B.15–B.18** | VPP/DSO communication flows (direct/indirect control) | 207–209 |
| **C.1** | IT security base system | 210 |
| **C.2–C.4** | XMPP stream + roster/presence (61850 client/server ↔ XMPP server) | 214–215 |
| **C.5–C.8** | Outage, link loss, presence loss, req/resp, abort | 216–219 |

**CCLI:** DER/VPP/XMPP federation is **informative** — Italian CCI path is **TR 57-126 MMS** on Eth_A.

---

## List of Tables (TOC p. 9 — start)

| Table | Title | Page |
|-------|--------|------|
| **1** | Services requiring client/server communication profile | 23 |
| **2** | Mapping of ACSI classes onto communication concepts | 30 |
| **3** | Mapping of ACSI **BasicTypes** | 40 |
| **4** | **PhyComAddr** structure (Layer 2) | 44 |
| **5** | **PhyComAddr** for UDP/IP | 45 |
| **6** | `GetNameList` — conflicting objectClass / objectScope | 47 |
| **7** | Service error mappings — `GetNameList` | 47 |
| **8** | **Read** service error mappings | 48 |

### Tables 9–51 (TOC p. 10)

| Table | Title | Page |
|-------|--------|------|
| **9** | **Write** service error mappings | 49 |
| **10** | **GetFileAttributeValues** error mappings | 50 |
| **11** | Encoding **TimeQuality** (7-2) | 51 |
| **12** | Encoding **TriggerConditions** | 52 |
| **13** | Encoding **ReasonForInclusionInReport** | 52 |
| **14** | Encoding **ReasonForInclusionInLog** | 53 |
| **15** | Encoding **RCBReportOptions** | 53 |
| **16** | Encoding **SVMessageOptions** | 53 |
| **17** | Encoding **CheckConditions** | 54 |
| **18** | Encoding **quality** (7-2) | 55 |
| **19** | Examples of **data values** encoding | 56 |
| **20–21** | **GetServerDirectory** (LD / FILE) parameter mapping | 57, 59 |
| **26** | **Associate** error mappings | 64 |
| **28** | **Release** error mappings | 66 |
| **29–30** | **GetLogicalDeviceDirectory** (+ extended) | 67, 69 |
| **31** | Object classes for **GetLogicalNodeDirectory** | 70 |
| **32–33** | **GetLogicalNodeDirectory** (+ extended) | 72, 74 |
| **34** | **GetAllDataValues** mapping | 77 |
| **35–36** | **GetDataValues / SetDataValues** parameters | 80, 81 |
| **37** | **GetDataDirectory** parameters | 83 |
| **38** | **GetDataDirectory** error mappings | 85 |
| **39** | Extended **GetDataDirectory** | 86 |
| **40, 42** | **GetDataSetValues / SetDataSetValues** parameters | 88, 91 |
| **41, 43** | Get/SetDataSetValues **error** mappings | 91, 93 |
| **44, 46** | **CreateDataSet / DeleteDataSet** parameters | 94, 96 |
| **45, 47** | Create/DeleteDataSet **errors** | 96, 98 |
| **48, 49** | **GetDataSetDirectory** parameters / errors | 99, 101 |
| **50** | **ServiceType** values | 101 |
| **51** | **errorCode** values | 103 |

### Tables 52–94 (TOC p. 11)

| Table | Title | Page |
|-------|--------|------|
| **52–54** | CDC **LTS**, **GTS**; **SGCB** mapping | 103–104 |
| **55–56** | **BRCB** / **URCB** structure | 111–112 |
| **57** | Order of AccessResults for **Report** | 113 |
| **58–59** | **GetBRCBValues** / **SetBRCBValues** parameters | 116–120 |
| **60** | **LCB** structure | 123 |
| **61–63** | **QueryLogByTime** / **QueryLogAfter**; log params | 125–129 |
| **62, 64** | Log **ServiceError** mappings; log conformance | 129–130 |
| **65** | **GoCB** TypeDescription | 130 |
| **66–68** | Controllable params; **7-2 ctlModel** → components; control services | 133–134 |
| **69–74** | **Select**, **SelectWithValue**, **Cancel** mappings + DataAccessError | 135–141 |
| **75–77** | **Operate**, **CommandTermination** | 142–145 |
| **78–79** | **LastApplError** structure; **AddCause** values | 149–152 |
| **80** | CDC **CTS** (control tracking) | 153 |
| **81–82** | ACSI file class ↔ file object; reserved suffixes | 153–154 |
| **83–87** | **GetFile** mapping + FileOpen/Read/Close errors | 157–161 |
| **88–91** | **SetFile**, **ObtainFile**, **DeleteFile** + errors | 162–164 |
| **92** | **GetFileAttributeValues** parameters | 165 |
| **93–94** | **PICS** A-Profile; **Time Sync** A-Profile | 167 |

**Tables 22–25, 27:** not on TOC pp. 9–11 — locate in PDF if present.

**CCLI mapping (reference only — XMPP SCSM):**

| Area | Tables | Product note |
|------|--------|----------------|
| **Reports** | 55–59, 57 | Same semantics as TR **`urcb_*` / `brcb_*`** — implement via **8-1 MMS** |
| **Control / curtail** | 66–77, 79 | **Operate** path ↔ PF2 DO |
| **Logs** | 60–64 | Prefer **62351-14** Syslog on TG-524 |
| **Files** | 81–92 | Signed firmware/CID policy |
| **Conformance** | 93–95, 123, F.1–F.2 | Phase 3 lab PICS; **P-Type** addressing (123) parallels **8-1** |

Product **MMS** wire tables: **61850-8-1** when procured.

### Tables 95–123 + Annex tables (TOC p. 12)

| Table | Title | Page |
|-------|--------|------|
| **95** | **PICS** for **T-Profile** support | 168 |
| **96–97** | **MMS InitiateRequest** / **InitiateResponse** general parameters | 168–169 |
| **98** | **MMS service supported** conformance table | 169 |
| **99** | **MMS Parameter CBB** | 172 |
| **100–118** | MMS **conformance statements** (GetNameList, Read/Write, NVL, ReadJournal, EntryContent, …) | 173–187 |
| **119–122** | **FileDirectory**, **FileOpen**, **FileRead**, **FileClose** conformance | 188–189 |
| **123** | Allowed **P-Type** definitions for client/server addressing | 189 |
| **A.1** | Service and protocols — client/server **A-Profile** | 193 |
| **A.2** | Service and protocols — client/server **XMPP T-Profile** | 193 |
| **A.3** | **Time sync A-Profile** | 194 |
| **A.4** | **Time sync T-Profile** | 195 |
| **D.1** | **ACSI services** mapping over **XMPP stanzas** (normative) | 220 |
| **D.2** | **Error** mapping over XMPP stanzas | 222 |
| **F.1** | **SCL conformance** degrees | 224 |
| **F.2** | Supported **ACSI services** for **SCL.2** | 224 |

**CCLI mapping (TOC index — XMPP + reference MMS):**

| Area | Tables | Product note |
|------|--------|----------------|
| **Profiles** | 93–95, A.1–A.4 | **A-Profile** = MMS stack ref; **T-Profile** = XMPP — TR uses **8-MMS**, not T-Profile |
| **MMS stack (informative ref in 8-2)** | 96–122 | Same service surface as **61850-8-1** PICS — use **8-1 PDF** for Eth_A implementation |
| **Addressing** | **123** | **P-Type** URIs — cross-check **61850-6** Communication / **8-1** when CID validated |
| **Wire binding** | **D.1–D.2** | Authoritative **stanza ↔ ACSI** map for XMPP only |
| **SCL tooling** | **F.1–F.2** | Align with **61850-6** extract + TR CID import rules |

**Table index:** TOC capture **complete** (main **1–123** + annex **A/D/F** listed tables). **Tables 22–25, 27** still unlisted on TOC pp. 9–12 — locate in PDF if present.

---

## Knowledge gaps (post-TOC)

| Gap | Action |
|-----|--------|
| Licensed **PDF** on disk | `IEC 61850-8-2.pdf` in standards drop |
| **Annex G XSD** files | Extract from PDF pp. 225+ when OCR |
| **61850-8-1** MMS/GOOSE wire | **P0 procure** for product Eth_A |

---

## Annex T service parity (XMPP vs MMS)

| Annex T §7 ACSI class | 8-2 TOC § | MMS (8-1) target for CCLI |
|-----------------------|-----------|---------------------------|
| GetServerDirectory | §9.2 | **P0** libiec61850 |
| GetLogicalNodeDirectory, GetAllDataValues | §12 | **P0** |
| Get/SetDataValues, GetDataDirectory | §13 | **P0** / gated Set |
| GetDataSetValues, GetDataSetDirectory | §14.2, 14.6 | **P0** |
| Buffered / Unbuffered **Report** | §17.1–17.2 | **P0** — `brcb_*`, `urcb_*`, **intgPd 4000** |
| **Reporting** Set on RCB | §17.2.3–17.2.5 | Annex T: limited **Set** |
| **GOOSE** | §18 | **P1** — **8-1** wire + **62351-6** |
| **Control** (Select/Operate/…) | §20.2–20.10 | Phase 2/3 curtailment mapping |
| Time sync | §21 | **P0** NTP/NTS |
| File transfer | §23 | P2 firmware/CID ops |
| PICS / SCL | §24–25 | Phase 3 conformance |

---

## CCLI vs TR 57-126

| Item | TR 57-126 / Annex T | 61850-8-2 |
|------|---------------------|-----------|
| SubNetwork `type` | **`8-MMS`** | Would be **`8-XMPP`** |
| SCSM in 61850-6 Services | `iec61850_8_1` default | `iec61850_8_2` optional flag |
| DSO lab target | **8-1** + 62351-3/4 | Out of P0 scope |

---

## Distinction — still need **61850-8-1**

| Part | Wire protocol | CCLI P0 |
|------|---------------|---------|
| **61850-8-1** | MMS (TCP 102) + GOOSE/Ethernet | **YES** — Eth_A |
| **61850-8-2** | XMPP | **NO** — optional future |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-882-SCOPE-001 | Read TR CID | No `8-XMPP` SubNetwork in TR example |
| PDF ingest | File in standards drop | `IEC 61850-8-2.pdf` present |
| RAG | `ccli-61850-8-2-extract` | Ingest job enqueued |

---

## RAG routing

```
ccli-61850-8-2-extract, ccli-61850-7-2-extract, ccli-61850-6-extract, cei-tr-57-126
```

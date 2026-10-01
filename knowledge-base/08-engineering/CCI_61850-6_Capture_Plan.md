# IEC 61850-6 — Capture Plan (CCLI / TR 57-126 CID)

**Document ID:** CCLI-PROTO-61850-6-001  
**Revision:** 2.1  
**Date:** 2026-09-21  
**RAG source_id:** `ccli-61850-6-capture-plan` → `ccli-61850-6-extract`  
**Normative basis:** IEC 61850-6:2009+AMD1:2018+AMD2:2024 CSV — *Configuration language for communication in electrical substations related to IEDs (SCL)*  
**Licensed PDF:** `07-protocols/regulations/standards-drop-20260918/IEC 61850-6.pdf` (**296 pp.**) — see `ccli-standards-pdf-inventory`  
**Companion:** `cei-tr-57-126` · `cei-0-16-allegato-t` · `apps/ccli/config/icd/cei-tr-57-126-example.cid`  
**Programme link:** K4.1 · K4.3 · REQ-61850-001

---

## Summary

**61850-6** defines **SCL** syntax — how to read, edit, and validate the **CID** shipped on the CCI. **CEI TR 57-126** is the national worked example; **61850-6** is the grammar reference.

**CCLI scope:** ingest **§7 file types**, **§8 SCL language**, **§9.3–9.5 IED/Comm/Templates** + attribute **Tables 3–4, 10–24, 35–36, 39, 40–49, 50** — **not** substation §9.2, **not** Annex A/E/F XSD dumps.

**TR 57-126 CID uses:** `version="2007" revision="B" release="4"` — validate against §8.2.2–8.2.3 and Annex I when DSO tools differ.

---

## Capture status roll-up

| Batch | Pages | Focus | Status |
|-------|-------|-------|--------|
| **A** | 33–45 | §6.6, §7, §8.1–8.4, Fig 8–9, Table 1–2 | **COMPLETE** (screenshots 2026-09-16) |
| **B** | 54–58 | §9 Header, History, file refs — Tables 3–4, 53 — Figs 25–26 | **COMPLETE** (screenshots 2026-09-16) |
| **C** | 87–108 | IED shell — Tables 10–14, 50 — Figs 18–19 | **COMPLETE** (screenshots 2026-09-16) |
| **D** | 90, 109–112 | LD, LN0, LN — Tables 15–17 — Fig 21 | **COMPLETE** (screenshots 2026-09-16) |
| **E** | 114–117 | DOI, DAI, SDI — Tables 18–20 | **COMPLETE** (screenshots 2026-09-16) |
| **F** | 89, 118–123 | DataSet, FCDA, Report — Tables 21–24 — Fig 20 | **COMPLETE** (screenshots 2026-09-16) |
| **G** | 143–150 | Communication — Tables 35–37, 39 — Fig 22 | **COMPLETE** (screenshots 2026-09-16) |
| **H** | 153, 156–170 | DataTypeTemplates — Fig 23, Tables 40–49 | **COMPLETE** (screenshots 2026-09-16) |
| **I** | 125–142, 137–141 | ExtRef, Log, ClientLN — Tables 25–34, 33, 51–52 | **COMPLETE** (2026-09-21 → extract §17) |
| **J** | 129–130, 148–149 | GOOSE comm — Tables 27–28, 37–38 | **COMPLETE** (2026-09-21 → extract §18) |
| **K** | 255, 267–285 | Annex G Table G.1, Annex I mixed version | **COMPLETE** (2026-09-21 → extract §19) |
| **SKIP** | 52–85, 175–254, 180–182 | Substation §9.2, XSD annexes, Annex D examples | — |

---

## Batch A — COMPLETE (pp. 33–45)

Screenshots ingested into extract `CCI_61850-6_Extract.md` §1–§8.

```
33   ✓ §6.6 Data flow modelling · §7 intro (six SCL file kinds)
34   ✓ §7 ICD · IID · SSD · SCD · CID definitions + extensions
35   ✓ §7 CID/SED · Fig 8 (ICD implementable subsets)
36   ✓ §8.1 Specification method (tBaseElement, tNaming, tIDNaming)
37   ✓ Table 1 (XSD files) · Fig 9 (SCL root UML)
38   ✓ §8.2 Language versions and compatibility
39   ✓ §8.2.1 MustUnderstand rules
40   ✓ §8.2.1 example (GSEControl/Protocol) · §8.2.2 namespace/versions
41   ✓ §8.2.3 Incompatibilities to earlier versions (2003→2007)
42   ✓ §8.3.1–8.3.5 SCL extensions · XML namespaces
43   ✓ §8.3.6 Private data (tPrivate schema)
44   ✓ Table 2 (Private attrs) · §8.3.7–8.3.9 extension conformance
45   ✓ §8.4 General structure (prolog, <SCL> root, sections, referencing)
```

**Optional next in Batch A′:** pp. **30–31** (§6.3–6.4 IED/comm model), **46–50** (§8.5 naming + Fig 10–13).

---

## Batch B — COMPLETE (pp. 54–58)

Ingested into extract `CCI_61850-6_Extract.md` §9.

```
54   ✓ Table 3 (Header attributes) · uuid/baseUuid rules · tSclFileUUIDReference
55   ✓ Table 53 (SclFileReference attrs) · tHeaderSclRef · SourceFiles ordering
56   ✓ Fig 25 (Header SourceFiles) · Fig 26 (IEDSourceFiles) · proxy-gateway note
57   ✓ tText · tHitem schema (History item structure)
58   ✓ Table 4 (Hitem attributes) · Header XML example · §9.2.1 start (SKIP body)
```

**TR 57-126 maps:** `<Header id="CCI_Oss_Contr" version="1" revision="1" nameStructure="IEDName">`, `<Hitem who="RMS" …>`.

**Gap flagged:** TR 57-126 Header has **no `uuid`** — 2024 CSV marks `uuid` mandatory on first creation; add at CCLI commissioning if DSO tool requires.

---

## Batch C — IED shell (P0) — COMPLETE

Ingested into extract `CCI_61850-6_Extract.md` §9.

```
87–88  ✓ Fig 18 (class IED UML — tIED→AccessPoint→Server→LDevice→LN0/LN)
93   ✓ Table 10 COMPLETE
94   ✓ IED naming rules · identity constraints · tServices schema
96–101 ✓ Table 11 COMPLETE
102  ✓ tAccessPoint XSD schema
103  ✓ Table 12 (AccessPoint)
104–106 ✓ Table 50 COMPLETE (IED vs Server placement · pp.104–106)
108  ✓ Tables 13, 14 (Server, Authentication)
```

**Fig 19:** not found as separate page — merged with Fig 18 or omitted in this PDF edition; **SKIP**.

**TR 57-126 maps:** `<IED name="CCI016_01" engRight="full" originalSclVersion="2007" originalSclRevision="B">`, `<Services>`, `<AccessPoint name="accessPoint1">`, `<Server>`, `<Authentication />`.

---

## Batch D — LD / LN (P0) — COMPLETE

Ingested into extract `CCI_61850-6_Extract.md` §10 + §13.

```
90   ✓ Fig 21 (LN/LN0 UML)
109  ✓ Table 15 (LDevice) · tLDevice XSD
110  ✓ LDevice restrictions (inst/ldName unique · 64 char · uuid rules) · §9.3.5 intro
111  ✓ Table 16 (LN0) · tLN0/tLN/tAnyLN XSD · LLN0 reference rule
112  ✓ Table 17 (LN) · tAnyLN completion (Inputs/Outputs/Log/Labels) · Log name default
```

**TR 57-126 maps:** `<LDevice inst="LD_Plant">`, `<LN0 lnClass="LLN0" lnType="LLN01" inst="">`, `<LN prefix="PdC" lnClass="MMXU" lnType="MMXU1" inst="1">`.

---

## Batch E — DOI / values (P0) — COMPLETE

Ingested into extract `CCI_61850-6_Extract.md` §11.

```
114  ✓ Table 18 (DOI) · tDOI attrs · one DOI per DO name
115  ✓ Table 19 (DAI) · tDAI XSD · Val elements · valImport
116  ✓ tSDI XSD · recursive SDI/DAI choice
117  ✓ Table 20 (SDI) · restrictions · structured/array XML examples
```

**TR 57-126 maps:** LN0 `Mod/stVal=on`, `ctlModel=status-only`; `WRtg/setMag/f=200`; LPHD `PhyNam` vendor/swRev/location; `lnNs`/`ldNs` namespace URIs.

---

## Batch F — DataSet / Report (P0) — COMPLETE

Ingested into extract `CCI_61850-6_Extract.md` §12.

```
89   ✓ Fig 20 (control blocks UML)
118  ✓ Table 21 (DataSet) · tDataSet/tFCDA XSD
119  ✓ Table 22 (FCDA) · restrictions · name length 32/20
120  ✓ DataSet same-Server rules · XML example · §9.3.8 tReportControl XSD start
121  ✓ Table 23 (ReportControl — name, datSet, intgPd, rptID, confRev) · tControl XSD
122  ✓ RCB instance rules · indexed naming · ClientLN/tClientLN · TrgOps
123  ✓ OptFields attrs · Table 24 (RptEnabled max) · tRptEnabled XSD
```

**TR 57-126 maps:** `DS_R_PdC_Mis4sec`, `urcb_PdC_Mis4sec` (`intgPd="4000"`, `TrgOps period`), `brcb_Stato_Allarmi_Segnali` (`buffered`, `dchg/qchg`), `RptEnabled max="2"`.

**Verification:** Extract §12.5 cites all four TR report pairs; REQ-61850-6-022 testable via CID grep.

---

## Batch G — Communication (P0) — COMPLETE

Ingested into extract `CCI_61850-6_Extract.md` §15.

```
143  ✓ §9.4.1 · Fig 22 (Communication UML) · tCommunication XSD
144  ✓ uniqueSubNetwork · uniqueConnectedAP · §9.4.2 tSubNetwork · GSE/SMV unique
145  ✓ Table 35 (SubNetwork) · tConnectedAP XSD start
146  ✓ Table 36 (ConnectedAP) · §9.4.3 Address · tAddress XSD
147  ✓ tP/tPAddr · xsi:type validation · §9.4.4 GSE start
148  ✓ tGSE · Table 37 · MinTime/MaxTime
149  ✓ Table 38 (SMV) · §9.4.6 PhysConn tPhysConn
150  ✓ Table 39 (PhysConn P-types Type/Plug/Cable/Port) · restrictions
```

**TR 57-126 maps:** `subnetwork1` `type="8-MMS"`, `ConnectedAP iedName=CCI016_01 apName=accessPoint1`, P-types IP/IP-SUBNET/IP-GATEWAY/OSI-*.

**Note:** Table 39 @ p.150 = **PhysConn** only. MMS **Address** P-types (`IP`, `IP-SUBNET`, …) are in **61850-8-1** / `tPTypeEnum` (referenced p.147).

---

## Batch H — DataTypeTemplates (P0) — COMPLETE

Ingested into extract `CCI_61850-6_Extract.md` §16 (Fig 23 + Tables 40–49).

```
153  ✓ Fig 23 (DataTypeTemplates UML — tDataTypeTemplates hierarchy)
156  ✓ Table 40 (template elements overview) · tLNodeType XSD
157  ✓ Table 41 (LNodeType) · tDO XSD
158  ✓ Table 42 (DO) · tDOType XSD
159  ✓ Table 43 (DOType) · tSDO · restrictions
160  ✓ Table 44 (SDO) · §9.5.4 DA · Table 45 start
161  ✓ Table 45 cont. (Octet64, VisString, ObjRef, Timestamp, opaque types)
162  ✓ Table 46 (valKind) · tValKindEnum
163  ✓ tDA · tAbstractDataAttribute · agDATrgOp
164  ✓ Table 47 (DA attributes)
165  ✓ tPredefinedBasicTypeEnum
166  ✓ tBasicTypeEnum · §9.5.4.3 sAddr
167  ✓ tVal/sGroup · tDAType start
168  ✓ tProtNs · tBDA
169  ✓ Table 48 (BDA)
170  ✓ Table 49 (EnumType) · tEnumVal ord
```

**TR 57-126 maps:** `<DataTypeTemplates>` with `LNodeType id=LLN01/MMXU1/…`, `EnumType id=CtlModelKind`, `DA name=ctlModel bType=Enum type=CtlModelKind fc=CF`.

---

## Batch I — ExtRef / signal_map (P1)

```
125  ← Table 25 (ClientLN)
127  ← Table 26 (LogControl)
137  ← Table 33 (ExtRef)
139  ← Table 51 (ExtRef use cases)
141  ← Table 52 @ p.141 (ExtCtrl) — NOT Table 52 @ p.174 (SCT actions)
```

---

## Batch J — GOOSE comm (P1) — COMPLETE

```
148–149  ✓ Tables 37–38 (GSE/SMV ConnectedAP attrs) — captured with Batch G
129–130  ✓ Tables 27–28 (GSE control comm) — 2026-09-21 → extract §18
```

---

## Batch K — Conformance / mixed version (P1)

```
255     ← Annex G + Table G.1 (IED configurator SICS)
267–285 ← Annex I + Fig I.1–I.7 (downgrade/upgrade)
261–266 ← Annex H ExtRef use cases (optional with Batch I)
```

---

## SKIP list

| Pages | Content |
|-------|---------|
| 52–85 | §9.2 Substation/process, Tables 5–9, Fig 15–17 |
| 132–133 | SMV Tables 29–31 |
| 174 | Table 52 SCT engineering actions |
| 175–254 | Annexes A, E, F (XSD) |
| 176–177 | Annex B, C |
| 180–182 | Annex D + Fig D.1–D.3 — **use TR 57-126 instead** |
| 257 | Table G.2 system configurator |
| 291+ | Bibliography |

---

## File naming convention (screenshots)

```
Architecture/_extracted_reg_analysis/iec_61850-6/
  P0_batch_A/p033.png … p045.png
  P0_batch_B/p054.png …
  P0_batch_F/p121_t23.png   # page + primary table
```

---

## CCLI mapping

| 61850-6 concept | CCLI artefact |
|-----------------|---------------|
| **CID** file type (§7) | Deploy `cei-tr-57-126-example.cid` on MMS server |
| **LLN0** holds DataSets/Reports (§6.6) | All `DS_R_*` / `urcb_*` / `brcb_*` under LN0 |
| **version/revision/release** (§8.2.2) | Match TR: `2007` / `B` / `4` |
| **Private** (§8.3.6) | Optional `signal_map.yaml` → `<Private type="ccli:…">` |
| **Tables 21–23** | 4 s periodic + buffered status reports |
| **Tables 35–36, Address** | Eth_A static IP from DSO plan (`ConnectedAP` P-types) |
| **Tables 40–49** | Validate TR `lnType` / `ctlModel` / EnumType refs |
| **Annex G.1** | DSO SICS evidence (Phase 3) |

---

## Verification

| Item | Pass criteria |
|------|---------------|
| Batch A | Extract §1–§8 documents ICD vs CID |
| Batch F | Extract §12 cites Table 23 ↔ TR `intgPd="4000"` — **PASS** |
| Batch G | Extract §15 ↔ TR `192.168.8.167` ConnectedAP — **PASS** |
| Batch H | Extract §16 cites EnumType ↔ TR `CtlModelKind` — **PASS** |
| Batch H′ | Extract §16.0 Fig 23 UML ↔ LN→DO→DOType reference chain — **PASS** |
| K4.1 | libiec61850 loads CID without schema errors |
| Batch E | Extract §11 ↔ TR DOI/DAI/Val patterns — **PASS** |
| Corpus | `ccli-61850-6-extract` P0+P1 **COMPLETE** — **61850-8-1** P-types via `ccli-61850-8-1-extract` / OCR corpus |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-09-16 | Initial plan; **Batch A complete** (pp. 33–45) |
| **1.1** | 2026-09-16 | **Batch B complete** (pp. 54–58) |
| **1.2** | 2026-09-16 | **Batch C partial** (pp. 93–94, 96, 103, 108) |
| **1.3** | 2026-09-16 | **Table 10 complete** p.93 · **Table 11 complete** pp.96–101 · **tAccessPoint** p.102 · **Fig 21** p.90 (Batch D partial) |
| **1.4** | 2026-09-16 | **Fig 18** p.87–88 · **Table 50 partial** p.104 · **Fig 20** p.89 · **Fig 21** expanded |
| **1.5** | 2026-09-16 | **Batch F COMPLETE** pp.89, 118–123 — Tables 21–24 · TR 4 s report crosswalk |
| **1.6** | 2026-09-16 | **Batch C COMPLETE** — Table 50 pp.104–106 · IED-level Services whitelist |
| **1.7** | 2026-09-16 | **Batch D COMPLETE** pp.90, 109–112 — Tables 15–17 · LD_Plant/LN0/LN crosswalk |
| **1.8** | 2026-09-16 | **Batch G COMPLETE** pp.143–150 · **Batch H COMPLETE** pp.156–170 · Table 39 corrected (PhysConn) |
| **1.9** | 2026-09-16 | **Batch E COMPLETE** pp.114–117 — Tables 18–20 DOI/DAI/SDI |
| **2.0** | 2026-09-16 | **Fig 23 COMPLETE** p.153 — Batch H closed · P0 corpus complete |

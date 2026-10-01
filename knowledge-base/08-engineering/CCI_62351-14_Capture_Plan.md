# IEC 62351-14 — Capture Plan (CCLI / TG-524)

**Document ID:** CCLI-SEC-62351-005  
**Revision:** 1.7  
**Date:** 2026-08-12  
**RAG source_id:** `ccli-62351-14-capture-plan` → `ccli-62351-14-extract`  
**Normative basis:** IEC **62351-14** (**CDV 2025** — Foreword p. **6** confirms IS)  
**Capture status:** **COMPLETE** — Batches A–G + **F** pp. **32–44**  
**Parent:** `ccli-k63-ceremony-sop` · `ccli-62351-9-extract` · `ccli-62351-3-extract` · `ccli-62351-4-extract` · `ccli-62443-4-2-extract`  
**Programme link:** **CR 6.2** · K6.3 event logging · **62351-9 Annex D** target schema

---

## Summary

**62351-14** defines **cyber security event** structure, **event ID allocation**, and **logging bindings** (primarily **Syslog RFC 5424** + optional **XML**). **Annex B** maps part-specific events from **62351-3/4/5/6/8/9/11** into the common log format — this closes the gap between **62351-9 Annex D** mnemonics and field SIEM/syslog output.

**TG-524:** Lab + field logging for **K6.3** (`CRED_ENR_EST_*`, `CERT_V_*`) and **C1** TLS/E2E diagnostics.

**Document length:** ~**92+** pp. (Annex D ends p. **92**).

**Edition note:** ToC shows **IEC CDV 62351-14 © IEC 2025** — capture title page to lock edition string before extract.

---

## ToC (confirmed from PDF pp. 2–4)

| § / Annex | Title | Page |
|-----------|-------|------|
| — | Foreword | **6** |
| — | Introduction | **8** |
| **1** | Scope | **9** |
| **2** | Normative references | **10** |
| **3** | Terms, definitions, abbreviations | **12–14** |
| **4** | Structure for cyber security event | **15–25** |
| **4.1** | Information structure — **describing** an event | **16** (**Table 1**) |
| **4.2** | Information structure — **logging** an event | **19** (**Table 2**) |
| **4.3** | Relation Table 1 ↔ Table 2 | **23** (**Figure 1**) |
| **4.4** | Event identifier numeric range allocation | **23–25** |
| **4.5** | Dynamic parameter insertion | **25** (**Table 3**) |
| **5** | Electronic version to describe events | **26** |
| **6** | Logging using **Syslog RFC 5424** | **26–32** |
| **6.1** | Mapping to HEADER | **26** (**Table 4**) |
| **6.2** | Mapping to STRUCTURED-DATA | **28** (**Table 5**, SD-ELEMENT `62351-14@41912`) |
| **6.3** | Mapping to MSG | **30** |
| **6.4** | Mapping to PRI (severity) | **30** (**Table 6**, **Table 7**) |
| **6.5** | Secure/reliable Syslog transport | **31** |
| **6.6** | Raw storage of mapped information | **32** |
| **7** | Logging using **XML** | **32–44** |
| **7.1** | XSD schemas (basic / events def / events list) | **32–40** |
| **7.2** | Rules for XML definition + translation files | **44** |
| **8** | French translation (1st version) | **45** |
| **9** | Mapping SNMP to Syslog | **46** |
| **10** | Storage recommendations | **46** |
| **11** | Conformance | **46** |
| **Annex A** | Generic cyber security events | **47** (**Table 9** p. **48**) |
| **Annex B** | **62351 parts** specific events | **57–82** |
| **B.1.1** | Part **-3** (TLS) | **57–61** (**Tables 10–11** pp. **58–59**) |
| **B.1.2** | Part **-4** (MMS/E2E) | **62–73** (**Tables 12–24** pp. **62–73**) |
| **B.1.3** | Part **-5** (60870/DNP3) | **73–74** (**Tables 25–26**) |
| **B.1.4** | Part **-6** (GOOSE/SV) | **75** (**Tables 27–28**) — P2 |
| **B.1.5** | Part **-8** (RBAC) | **76–77** (**Tables 29–32**) |
| **B.1.6** | Part **-9** (key mgmt / PKI) | **79–81** (**Tables 33–37**) |
| **B.1.7** | Part **-11** | **82** (**Table 38**) |
| **Annex C** | Syslog examples | **83–91** |
| **Annex D** | **62443-4-2** ↔ 62351-14 attribute mapping | **92** (**Table 50**) |

---

## Capture batches — **send these pages**

### Batch A — Event model + ID scheme — **COMPLETE**

```
6–8     ✓ Foreword · stability notice · Introduction
9–11    ✓ §1 Scope · §2 Normative references
12–14   ✓ §3 Terms · named characters · UTF-8
15–25   ✓ §4 · Tables 1–3 · Figure 1 · §4.4 ID · §4.5 dynamic params
```

### Batch B — Syslog mapping (lab default) — **COMPLETE**

```
26–32   ✓ §5 XML intro · §6 · Tables 4–7 · §6.3 MSG · §6.4 PRI · §6.5–6.6 transport · §7.1 XSD start
```

### Batch C — 62351-9 PKI events (K6.3) — **COMPLETE**

```
79–81   ✓ Annex B.1.6 · Tables 33–37 (credential · pub-key · attr cert · revocation · GDOI)
```

*Cross-check against `ccli-62351-9-extract` Annex D mnemonics (`CRED_ENR_EST_SUCC`, `CERT_V_REVOKED`, etc.).*

### Batch D — 62351-3 + 62351-4 C1 events — **COMPLETE**

```
58–60   ✓ Annex B.1.1 · Tables 10–11 · CRLReason
61–73   ✓ Annex B.1.2 · Tables 12–24 (E2E handshake · transfer · OSI · XMPP)
```

### Batch E — Conformance + generic + 62443 bridge — **COMPLETE**

```
46      ✓ §9 SNMP · §10 storage · §11 conformance
47–56   ✓ Annex A · Table 9 groups 1–9 (61 events)
92      ✓ Annex D · Table 50 (62443-4-2 mapping)
```

### Batch F — XML schema — **COMPLETE**

```
32      ✓ §6.6 raw storage · §7 intro
33–35   ✓ §7.1.1 SECEVT_BaseTypes.xsd
36–40   ✓ §7.1.2 SECEVT_EventsDefinition.xsd · SECEVTDef root
41–44   ✓ §7.1.3 SECEVT_EventList.xsd · SECEVTLogs · §7.2 versioning
45      §8 French translation — skip
```

### Batch G — Remaining Annex B + examples — **COMPLETE**

```
73–74   ✓ Annex B.1.3 · Tables 25–26 (62351-5) — C2
75      ✓ Annex B.1.4 · Tables 27–28 (62351-6 GOOSE/SV)
76–78   ✓ Annex B.1.5 · Tables 29–32 (62351-8 RBAC)
82      ✓ Annex B.1.7 · Table 38 (62351-11)
83–91   ✓ Annex C · Tables 39–49 (Syslog examples)
```

---

## CCLI mapping template

| 62351-14 concept | CCLI artefact | Sibling |
|------------------|---------------|---------|
| §4 event ID `IEC 62351-9:1.5` | K6.3 EST success log line | `ccli-k63-ceremony-sop` |
| §6 Syslog SD-ELEMENT | OpenWrt `logd`/remote syslog | CR 6.2 |
| Table 7 severity ↔ PRI | Alarm vs warning policy | 62351-9 §6.2 |
| Annex B.1.6 Tables 33–37 | Full PKI event catalogue | 62351-9 Annex D |
| Annex B.1.2 E2E tables | MMS handshake diagnostics | 62351-4 §14 |
| Annex D Table 50 | 62443 CR evidence mapping | `ccli-62443-4-2-extract` |

---

## Verification

| Item | Pass criteria |
|------|---------------|
| Batch A §4 | Event ID grammar captured | **PASS** |
| Batch B §6 | Syslog HEADER/SD/MSG/PRI mapped | **PASS** |
| Batch C B.1.6 | 62351-9 Annex D IDs align Tables 33–37 | **PASS** |
| Batch D B.1.1–2 | C1 TLS + E2E events captured | **PASS** |
| Batch E §11 + Annex D | Conformance + 62443 map | **PASS** |
| Batch E Annex A | Table 9 · 61 generic events | **PASS** |
| Batch G B.1.3–8,11 | 62351-5/6/8/11 events | **PASS** |
| Batch F §7 | XML XSD SECEVTDef / SECEVTLogs | **PASS** |
| K6.3 SOP | Sample log line for `CRED_ENR_EST_SUCC` | **PASS** |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-08-12 | ToC from PDF pp. **2–4**; capture batches A–G defined |
| **1.1** | 2026-08-12 | **Batch A** pp. 6–25 captured |
| **1.2** | 2026-08-12 | **Batch B** pp. 26–32 captured |
| **1.3** | 2026-08-12 | **Batch C** pp. 79–81 · Tables 33–37 |
| **1.4** | 2026-08-12 | **Batch D** pp. 58–73 · Tables 10–24 |
| **1.5** | 2026-08-12 | **Batch E** pp. 46–48,92 + **Batch G** pp. 73–82,83–91 |
| **1.6** | 2026-08-12 | **Table 9** pp. **49–56** · 61 generic events |
| **1.7** | 2026-08-12 | **Batch F** §7 XML XSD pp. **32–44** |

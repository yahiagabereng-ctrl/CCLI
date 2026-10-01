# IEC 61850-5 — CCLI Engineering Extract

**Document ID:** CCLI-PROTO-61850-5-001  
**Revision:** 1.0  
**Date:** 2026-09-16  
**RAG source_id:** `ccli-61850-5-extract`  
**Normative basis:** IEC 61850-5:2003 — *Communication requirements for functions and device models*  
**Capture status:** **HAVE (P1 P0)** — §11.2 + §13.5–13.7 from `61850-5_screenshots.pdf` OCR  
**Parents:** `ccli-61850-5-capture-plan` · `cei-0-16-allegato-t` · `cei-tr-57-126`  
**Programme link:** K4.1 · Annex T timing class

---

## Summary

**61850-5** defines **communication requirements** between **logical nodes**: **PICOMs** (per-function messages) roll up into **message types** with **transfer-time budgets**. **Performance classes P1–P3** (control/protection) and **M1–M3** (metering) subdivide types by substation criticality.

**CEI Annex T** assigns CCI **MMS** traffic to **Type 3 — low speed messages** (**< 500 ms** end-to-end). Optional **GOOSE** control uses **Type 1 — fast messages**.

**Edition note:** Corpus PDF is **61850-5:2003**. Annex T references **§11.2.2/11.2.3** (interfaces) and performance via **§13.7**. A **2013 edition** exists — re-validate numbering if DSO cites it.

---

## Requirements

| ID | Requirement | Source | CCLI implication |
|----|-------------|--------|------------------|
| REQ-850-5-TYPE3-001 | **Type 3** total transmission **< 500 ms** | §13.7.3 | Annex T measurements + setpoints |
| REQ-850-5-TYPE2-001 | **Type 2** total transmission **< 100 ms** | §13.7.2 | State with time-tag — not TR 4 s path |
| REQ-850-5-TYPE1-001 | **Type 1A Trip** P1 ≈ **10 ms**; P2/3 ≈ **3 ms** | §13.7.1.1 | GOOSE subscribe if enabled |
| REQ-850-5-TYPE1-002 | **Type 1B** other fast: P1 ≤ **100 ms**; P2/3 ≈ **20 ms** | §13.7.1.2 | Interlocking / GOOSE general |
| REQ-850-5-TCI-001 | **ITCI** telecontrol interface for remote control | §11.2.2 | DSO/Aggregator MMS client role |
| REQ-850-5-TMI-001 | **ITMI** telemonitoring — subset, no control | §11.2.2 | Read-only maintenance |
| REQ-850-5-CLASS-001 | **P1** distribution / low requirement bays | §13.6.1 | Default smaller plant |
| REQ-850-5-CLASS-002 | **P2** transmission bay default | §13.6.1 | Customer override |
| REQ-850-5-CLASS-003 | **P3** top sync + breaker differential | §13.6.1 | Highest protection tier |
| REQ-850-5-TR-001 | TR **4 s periodic reports** fit **Type 3** not Type 2 | §13.7.3 + Annex T | `intgPd=4000` MMS OK |
| REQ-850-5-CHAIN-001 | Transfer time = full chain (PD1→network→PD2) | §13.5 Fig 16 | SAT verifies installed network |

---

## Architecture

### Functional (message typing)

```
LN function (PICOM) ──► message type (Type 1–7)
                              │
                              └── performance class P1/P2/P3 or M1/M2/M3
```

### Network

| Interface (§11.2.2) | LN examples | CCLI role |
|---------------------|-------------|-----------|
| **IHMI** | Local/bay HMI | Web UI (non-SCADA) |
| **ITCI** | Telecontrol | **C1 MMS server** to DSO |
| **ITMI** | Telemonitoring | Read-only client |
| **IARC** | Archiving | Historian (optional) |

Roles of HMI/TCI/TMI fixed in **engineering phase**.

---

## Interface matrix

| Interface | Source | Destination | Protocol | Performance |
|-----------|--------|-------------|----------|-------------|
| DSO read/control | CCI (ITCI) | DSO SCADA | 61850-8-1 MMS | **Type 3** |
| 4 s measurements | CCI MMXU | DSO client | MMS reports | **Type 3** |
| GOOSE subscribe | External IED | CCI | 61850-8-1 GOOSE | **Type 1** (if used) |
| Time sync | GNSS/NTP | CCI | SNTP/PTP | Type 6 (§13.7.6) |

---

## 1. Message types (§13.5–13.7)

**Message types** group PICOM **performance attributes** (content, length, worst-case transfer time, security). Independent of substation size.

| Type | Name | Transfer time | Typical use |
|------|------|---------------|-------------|
| **1** | Fast | See P-class | Trip, close, GOOSE |
| **2** | Medium speed | **< 100 ms** | Time-tagged state |
| **3** | Low speed | **< 500 ms** | Setpoints, events, **SCADA data** |
| **4** | Raw data | ms–µs budgets | SV / process bus |
| **5** | File transfer | Application-dependent | COMTRADE, configs |
| **6** | Time sync | Per profile | PTP/SNTP |
| **7** | Command + access control | Type 3 + password | Secured ops |

---

## 2. Performance classes — control (§13.6.1)

| Class | Typical application |
|-------|---------------------|
| **P1** | Distribution bay / relaxed requirements |
| **P2** | Transmission bay (customer default) |
| **P3** | Transmission + top sync + breaker differential |

Different links in one substation may use **different classes**.

---

## 3. Type 1 — Fast messages (§13.7.1)

### Type 1A “Trip”

| Class | Total transmission time |
|-------|-------------------------|
| **P1** | ≈ **half cycle → 10 ms** defined |
| **P2/P3** | < quarter cycle → **3 ms** defined |

### Type 1B “Others” (interlocking, etc.)

| Class | Time |
|-------|------|
| **P1** | ≤ **100 ms** |
| **P2/P3** | ≈ one cycle → **20 ms** |

Typical interfaces: **IF3, IF5, IF8**.

---

## 4. Type 2 — Medium speed (§13.7.2)

- Time-tag from sender; receiver acts after internal delay.
- **Total transmission < 100 ms**.
- Typical: **IF3, IF8, IF9**.

---

## 5. Type 3 — Low speed (§13.7.3) — **CCI primary**

Includes: slow auto-control, **event records**, **set-point read/change**, system data presentation, time-tagged alarms, non-electrical measurands.

**Total transmission time shall be less than 500 ms.**

Typical interfaces: **IF1–IF9** (parameters, SCADA).

**Annex T mapping:**

| Annex T data | Mode | 61850-5 class |
|--------------|------|---------------|
| Plant static/dynamic config | On request | **Type 3** |
| Operating status | On change + request | **Type 3** |
| Measurements | **Every 4 s** | **Type 3** |
| Setpoints P/Q | On change + request | **Type 3** |

Annex T “Performance Class Type 3 ≈ P5” is **informative naming** — normative budget is **§13.7.3 500 ms**.

---

## Crosswalk — TR 57-126 / Annex T

| Artefact | 61850-5 basis |
|----------|---------------|
| `urcb_* intgPd=4000` | Type 3 periodic (≪ 500 ms budget) |
| `DS_R_*` measurement DataSets | Low speed MMS |
| No GOOSE in TR CID | Type 1 not required for lab P0 |
| DSO MMS client on ITCI | §11.2.2 telecontrol interface |
| 61850-8-1 “Type 2/3 typologies” (Annex T) | Maps to message types 2/3 here |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| **61850-5:2013** edition | Not on disk | If DSO cites 2013 § numbers | Optional PDF |
| §13.7.4–7 Types 4–7 detail | **PART** | SV/file/sync depth | P2 |
| §14 multi-message scenarios | **MISSING** | Large substation SAT | Informative |
| Metering M1–M3 | **PART** | Revenue meter class | P2 metrology |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-8505-001 | Treat 4 s reports as Type 2 (100 ms) | Over-engineering | Classify as **Type 3** |
| R-8505-002 | Ignore network chain in SAT | Miss 500 ms breach | Fig 16 end-to-end test |
| R-8505-003 | 2003 vs 2013 clause drift | Audit mismatch | Confirm DSO edition |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-850-5-TYPE3-001 | MMS RTT + report latency lab | < 500 ms on Eth_A |
| REQ-850-5-TR-001 | TR CID report config | 4000 ms period ≠ Type 2 |
| Annex T | Trace T.3.2.1 table | All rows → Type 3 |
| K4.1 | Load TR CID + ping DSO plan | Functional + timing budget doc |

---

## RAG routing

```
ccli-61850-5-extract, cei-0-16-allegato-t, cei-tr-57-126, ccli-61850-6-extract
```

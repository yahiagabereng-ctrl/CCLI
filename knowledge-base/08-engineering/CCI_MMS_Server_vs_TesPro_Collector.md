# MMS server (CCLI) vs TesPro collector — programme decision

**Document ID:** CCLI-ARCH-MMS-DUAL-001  
**Date:** 2026-09-29  
**RAG source_id:** `ccli-mms-server-vs-tespro-collector`  
**Parent:** `ccli-tespro-61850-manual-extract` · V-005 · REQ-104-001 · P3/P5  
**Status:** **DECIDED** — implement DSO path in `apps/ccli`; TesPro APK optional for plant poll only

---

## Decision

| Role | Owner | Model file | Port | Status |
|------|-------|------------|------|--------|
| **MMS server** (DSO / Eth_A) | **`apps/ccli`** | **`lab_tg544_eth_a.cid`** (→ product CID) | **3782** TLS + 62351-4 | **IMPLEMENTED** (lab) |
| **MMS client** (poll plant IEDs) | TesPro `iec61850-mmsd` **or** future `ccli` client | **User-uploaded ICD/CID per remote IED** | **102** | **Optional** — not CEI DSO gate |
| **SCD** (whole substation) | DSO / integrator engineering tool | N/A at CCI firmware | — | **Not in lab scope** |

**We do not expect TesPro to provide an SCD/CID for TG544 as MMS server.**  
Supplier 61850 packages are **client polling + northbound upload** only.

---

## Architecture

```text
  Eth_A LAN1  192.168.10.1:3782
  ┌─────────────────────────────────────┐
  │  ccli — IedServer (CCI016_01)       │
  │  CID: lab_tg544_eth_a.cid           │
  │  62351-3 TLS + 62351-4 AARE auth    │
  └──────────────▲──────────────────────┘
                 │ MMS client
           DSO / TSP / IEDScout

  Plant LAN3 / remote  :102
  ┌─────────────────────────────────────┐
  │  TesPro iec61850-mmsd (optional)     │
  │  OR future ccli MMS client adapter    │
  │  ICD path = each polled IED (user)    │
  └──────────────▲──────────────────────┘
                 │ poll
           Inverter / relay IEDs
```

---

## File map (lab)

| File | Purpose |
|------|---------|
| `apps/ccli/config/icd/lab_tg544_eth_a.cid` | **Authoritative** DSO server model (IED `CCI016_01`) |
| `apps/ccli/config/icd/cei-tr-57-126-example.cid` | TR 57-126 reference |
| `apps/ccli/config/icd/signal_map.yaml` | Internal point map (MVP) |
| TesPro UI “Upload ICD” | **Remote IED only** — not this CID unless reusing for a simulator |

**No `*.scd` in repo** — correct for single-IED CCI lab. SCD appears at substation integration (multiple IEDs), not on TG544 firmware deliverable.

---

## Requirements

| REQ-ID | Requirement | Owner |
|--------|-------------|-------|
| REQ-MMS-SRV-001 | DSO MMS server on Eth_A :3782 | `ccli` + CID |
| REQ-MMS-SRV-002 | Model matches Annex T / lab TR Table 1 | `lab_tg544_eth_a.cid` + runtime MVP |
| REQ-MMS-CLT-001 | Optional poll plant IEDs :102 | TesPro APK **or** deferred `ccli` client |
| REQ-MMS-CLT-002 | User supplies ICD/CID for each polled device | Integrator / inverter vendor |
| REQ-VEND-008 | Document vendor 61850 scope | **HAVE** — manual extract |

---

## TesPro supplier (V-005)

**Question sent / recorded:**

> Do you provide an SCD/CID for the TG544 as MMS server, or only client polling with user-uploaded ICD?

**Programme answer (pending supplier confirm):**

- **We implement MMS server in `ccli`** with our CID — CEI Allegato T path.
- TesPro **iec61850-*** APKs = **optional northbound collector**; coexistence lab **TBD** (CPU/RAM, no port conflict :102 vs :3782).

---

## Verification

| Check | Pass |
|-------|------|
| TSP / client loads `lab_tg544_eth_a.cid` | IED name **CCI016_01** |
| DUT serves model on `192.168.10.1:3782` | `ccli` log + browse |
| TesPro web UI Add Device | Uses **remote** host:102 + **their** ICD — not DSO path |
| No SCD required for P3 exit | Phase 3 **CLOSED** (lab) |

---

## Risks

| ID | Risk | Mitigation |
|----|------|------------|
| R-MMS-001 | Team assumes TesPro APK = DSO 61850 | This doc + manual extract |
| R-MMS-002 | Missing plant ICD for collector | Separate from DSO gate; P5 plant path |
| R-MMS-003 | Two MMS stacks on DUT | Document coexistence test (V-005) |

---

## Keywords

`61850`, `CID`, `SCD`, `ICD`, `ccli`, `TesPro`, `iec61850-mmsd`, `MMS server`, `collector`, `V-005`

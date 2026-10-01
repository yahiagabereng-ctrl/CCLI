# IEC 62351-6 — CCLI Engineering Extract

**Document ID:** CCLI-SEC-62351-006  
**Revision:** 1.0  
**Date:** 2026-09-16  
**RAG source_id:** `ccli-62351-6-extract`  
**Normative basis:** IEC 62351-6:2020 — *Security for IEC 61850 profiles including GOOSE, SV, and SNTP*  
**Capture status:** **HAVE (P1 core)** — Batches A–F from `62351-6.pdf` OCR  
**Parents:** `ccli-62351-6-capture-plan` · `ccli-62351-1-extract` · `ccli-62351-9-extract` · `ccli-61850-6-extract`  
**Programme link:** K6.5 Phase 3 · plant GOOSE/SV

---

## Summary

**62351-6:2020** secures **61850 multicast and time-sync profiles**: **HMAC-SHA256(-128)** authentication on **GOOSE/SV** PDUs, optional **AES-GCM** encryption (mainly **routable** GOOSE/SMV), **GDOI group keys** (**62351-9** / RFC 8052), **replay protection** state machine, and **SCL** capability flags (`McSecurity`, `kdaParticipant`).

**CCLI stack split:** **C1 MMS** = **62351-3 + 62351-4**; **62351-6** applies when TG-524 **publishes/subscribes GOOSE/SV** on plant Ethernet or enables **McSecurity** in SCL.

**Corpus status:** **PART** — normative core captured; full §5/§6.3 VLAN/SNTP detail **MISSING** (P2).

---

## Requirements

| ID | Requirement | Source | CCLI implication |
|----|-------------|--------|------------------|
| REQ-3516-SCOPE-001 | Applies to **61850-8-1, 8-2, 9-2, 6** (Table 1) | §1.1 | GOOSE/SV/SCL extensions; MMS mapping via 8-1 uses 62351-4 for TCP |
| REQ-3516-PERF-001 | L2 GOOSE/SV with **3 ms** budget: **encryption not recommended** | §4.1 | Plant bus default = **auth only**; matches 62351-1 Table 2 |
| REQ-3516-THREAT-001 | Without encryption: counter **tampering** via message auth | §4.2 | HMAC extension on PDU |
| REQ-3516-THREAT-002 | With encryption: counter access, tampering, disclosure | §4.2 | Routable / non-3 ms paths only |
| REQ-3516-REPLAY-001 | GOOSE subscriber maintains **stNum/sqNum/TAL** state machine | §6.2.1 | Implementation + supervision (LGOS) |
| REQ-3516-KEY-001 | Publishers/subscribers conform to **GDOI** (**62351-9**) | §8.2.2.4.1 | K6.3 group-key ceremony |
| REQ-3516-KEY-002 | **Key ID** (4 octets) references KDC-assigned group key | §8.2.2.3.6 | Per-DataSet keys |
| REQ-3516-PDU-001 | Security **extension** appended to GOOSE/SV (ASN.1 BER) | §8.2.2 | AuthenticationValue + optional MAC |
| REQ-3516-SCL-001 | **`McSecurity`** on GSE/SMV/ClientServices: `signature`, `encryption` | §9.1 | Only if `Authentication certificate="true"` (61850-6) |
| REQ-3516-SCL-002 | Missing **McSecurity** ⇒ no security extensions supported | §9.1.1 | Default TR CID: no GOOSE security |
| REQ-3516-PICS-001 | L2 GOOSE: **62351-9 GDOI** + **HMAC-SHA256-128** mandatory (`m`) | Table 9 | Lab PICS if GOOSE secured |
| REQ-3516-PICS-002 | Routable GOOSE/SMV: separate PICS Tables 11–12 | §11 | WAN plant links — Phase 3+ |

---

## Architecture

### Functional

```
Publisher IED                    Subscriber IED
     │                                │
     ├─ GDOI (62351-9) group key ────┤
     ├─ Build GOOSE/SV APDU           │
     ├─ Extension: AuthValue + HMAC ──┼─ Verify MAC / replay FSM
     └─ Optional AES-GCM (R-GOOSE)   └─ Application update
```

### Security

| Layer | Mechanism | CCLI phase |
|-------|-----------|------------|
| Multicast auth | HMAC-SHA256-128 on APDU | P1 plant |
| Multicast encrypt | AES-GCM (routable) | P2 if required |
| Key mgmt | GDOI / GROUP-PULL | 62351-9 §8 |
| Coexistence | Secure + non-secure PDUs on same LAN | Phased rollout |

### SCL integration (61850-6 cross-ref)

| Element | Attribute | Meaning |
|---------|-----------|---------|
| `GSESettings/McSecurity` | `signature`, `encryption` | Publisher GOOSE caps |
| `SMVSettings/McSecurity` | same + `kdaParticipant` | SV publisher / KDA |
| `ClientServices/McSecurity` | subscriber caps | GOOSE/SV subscribe |

---

## Interface matrix

| Interface | Source | Destination | Protocol |
|-----------|--------|-------------|----------|
| L2 GOOSE secure | IED publisher | Multicast subscribers | 61850-8-1 + 62351-6 ext |
| L2 SV secure | MU publisher | SV subscribers | 61850-9-2 + 62351-6 ext |
| Group keys | KDC | IEDs | GDOI (62351-9) |
| SCL capability | Engineering tool | IED config | 61850-6 + 62351-6 §9 |

---

## 1. Scope (§1.1, Table 1)

Secures protocols **based on IEC 61850 series**, at minimum:

| Standard | Profile |
|----------|---------|
| **61850-8-1** | MMS + L2 GOOSE |
| **61850-8-2** | XMPP mapping |
| **61850-9-2** | Sampled values |
| **61850-6** | SCL extensions (§9) |

Measures take effect when referenced by the base protocol specs. **62351-4** covers **MMS/TCP**; this part covers **multicast/time**.

---

## 2. Operational constraints (§4.1)

For **Layer 2 GOOSE/SV** with **3 ms** response, multicast, low CPU:

- **Encryption not recommended** — confidentiality via **L2 LAN segmentation**.
- Mechanism still defined for cases where 3 ms is not required.
- **Secure and non-secure PDUs may coexist**.

---

## 3. GOOSE replay protection (§6.2.1)

Subscriber state machine variables: `lastRcvStNum`, `lastRcvSqNum`, `lastRcvT`, `intTAL`.

States (summary): **Non-Existent** → **Wait for GOOSE** → **Security Checks** → **Replay check** → **Application update** → supervision / loss / failure paths.

- Subscription configured via **SCL + ICT** (out-of-band otherwise).
- Unknown key ID → **GROUP-PULL** (62351-9).
- Security check verifies **AuthenticationValue** per **61850-6** config (§6.2.1.2).

---

## 4. PDU extension (§8.2)

**Extension** ASN.1 structure includes optional **AuthenticationValue** and **mAC**.

Key fields:

| Field | Role |
|-------|------|
| **InitializationVector** | 16 octets, per-APDU if required |
| **Key ID** | 4 octets, KDC reference per DataSet |
| **VLAN CRC** | ISO/IEC 13239 over extension when Extension Length ≠ 0 |

**Encryption:** full GOOSE/SV APDU; MAC may cover encrypted payload. **L2 GOOSE/SV encryption out of scope**; **R-GOOSE** uses AES-GCM.

---

## 5. SCL extensions (§9)

### 5.1 GOOSE publisher (`GSESettings`)

- **`kdaParticipant="true"`** — KDA supported (62351-9).
- **`McSecurity signature="true"`** — supports §8.2 auth extensions.
- **`McSecurity encryption="true"`** — supports PDU encryption (Clause 8).
- **Absent McSecurity** — no security extensions.

### 5.2 SV publisher / subscriber / client

Same **`McSecurity`** pattern on **`SMVSettings`** and **`ClientServices`**.

---

## 6. Conformance PICS (§11, Tables 8–12)

**Table 8 — VLAN profiles (summary):**

| Code | Profile |
|------|---------|
| S2a | SCL extensions |
| S2b | 61850-8-1 **L2 GOOSE** security |
| S2c | **Routable GOOSE** |
| S2d | 61850-9-2 **L2 SMV** security |
| S2e | **Routable SMV** |

**Table 9 — L2 GOOSE (mandatory markers):**

| ID | Capability |
|----|------------|
| L2G1 | IEC 62351-9 GDOI Key |
| L2G3 | **HMAC-SHA256-128** |
| L2G4 | HMAC-SHA256 |

Tables **10–12** mirror for **L2 SV** and **Routable** profiles.

---

## Crosswalk — TR 57-126 / CCLI

| TR 57-126 CID | 62351-6 rule |
|---------------|--------------|
| No GOOSE control blocks in example | 62351-6 not active on C1 MMS lab |
| `<Authentication />` empty | No `McSecurity` until Phase 3 |
| 4 s MMS reports | **62351-4** domain, not 62351-6 |
| Future plant GOOSE | Enable §9 SCL + GDOI before field |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| §5 mandatory procedures | **MISSING** | Full publisher/subscriber rules | P2 re-scan |
| §6.3 VLAN profiles | **MISSING** | Switched LAN security | P2 |
| §10 SNTP profile | **PART** (Table 13 ref) | Time-sync security | P2 |
| PDF page order | Scrambled doc88 | Ordered CSV | Re-source PDF |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-3516-001 | Enable GOOSE encryption on 3 ms bus | Missed trips | §4.1 — auth-only on L2 |
| R-3516-002 | McSecurity without 62351-9 keys | Reject all GOOSE | GROUP-PULL + K6.3 |
| R-3516-003 | Mixed secure/non-secure subscribers | Split plant | Engineering policy + SCL |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-3516-PICS-001 | Review PICS Table 9 | GDOI + HMAC-128 marked `m` |
| REQ-3516-SCL-001 | Grep CID for `McSecurity` | Absent in Phase 1 TR CID |
| REQ-3516-PERF-001 | Design review plant bus | L2 auth-only default documented |
| K6.5 Phase 3 | libiec61850 + secured GOOSE sample | MAC verify + replay FSM pass |

---

## RAG routing

**Pack C — Protocols / Cyber:**

```
ccli-62351-6-extract, ccli-62351-9-extract, ccli-61850-6-extract, cei-0-16-allegato-t
```

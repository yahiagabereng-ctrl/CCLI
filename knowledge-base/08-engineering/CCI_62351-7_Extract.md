# IEC 62351-7 — CCLI Engineering Extract (NSM / SNMP)

**Document ID:** CCLI-SEC-62351-007  
**Revision:** 1.0  
**Date:** 2026-09-21  
**RAG source_id:** `ccli-62351-7-extract`  
**Normative basis:** IEC 62351-7:2025 — *Network and system management (NSM) data objects and profiles*  
**Licensed PDF:** `07-protocols/regulations/standards-drop-20260918/IEC 62351-7 2025.pdf`  
**Capture status:** **HAVE (P1)** — Batches A–B per `ccli-62351-7-capture-plan`  
**Parents:** `ccli-62351-1-extract` · `ccli-62351-14-extract` · `ccli-62443-3-3-extract`  
**Programme link:** K6.5 · management zone · optional SNMP v2/v3 trap (datasheet)

---

## Summary

**62351-7** standardizes **NSM** information — health, access attempts, protocol errors, resource exhaustion — mappable to **SNMP**, web services, or **61850**-named objects. It complements **62351-14** (**event logs** / Syslog) which is the **P0** audit path for K6.3; **62351-7** is **P1** unless DSO mandates SNMP situational awareness.

---

## Requirements

| REQ-ID | Source | Requirement | CCLI |
|--------|--------|-------------|------|
| **REQ-357-SCOPE-001** | §1 | NSM data for power-system devices | OpenWrt + CCLI metrics |
| **REQ-357-USM-001** | SNMPv3 | **USM** users + auth/priv protocols | If SNMP enabled |
| **REQ-357-VACM-001** | SNMPv3 | **VACM** view-based access control | Align 62351-8 RBAC story |
| **REQ-357-MON-001** | NSM objects | Invalid access, malformed PDUs, DoS indicators | CR 6.2 monitoring |
| **REQ-357-EVT-001** | vs 62351-14 | **62351-7** = health/MIB; **62351-14** = security event syslog | Do not duplicate without mapping |

---

## Batch A — Scope, USM/VACM

| Topic | CCLI |
|-------|------|
| **NSM object naming** | Align with 61850 logical names where exposed |
| **SNMPv3 USM** | authPriv for management VLAN only |
| **VACM** | Least privilege; no read-write on control OIDs from WAN |

Management interface on **separate conduit** from Eth_A/Eth_B plant paths (`CCI_62443_Zones.md`).

---

## Batch B — Link to 62351-14

| Channel | Use |
|---------|-----|
| **62351-14 Syslog** | PKI, TLS, 62351-5/4 security events (K6.3) |
| **62351-7 SNMP trap** | Optional infra alarms (link down, CPU, disk) |

Cross-reference event IDs if both enabled — SIEM normalization table in ops runbook (future).

---

## CCLI mapping

| Item | Note |
|------|------|
| SNMP trap | Datasheet claim — default **off** on product profile |
| Health | procd, netifd, ccli service watchdog |
| Audit | Prefer **62351-14** TCP/6514 to collector |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-357-VACM-001 | SNMP walk from Eth_B | Denied without auth |
| REQ-357-EVT-001 | Security fail | Appears in 62351-14, not only SNMP |

---

## RAG routing

```
ccli-62351-7-extract, ccli-62351-14-extract, ccli-62351-1-extract, ccli-62443-3-3-extract
```

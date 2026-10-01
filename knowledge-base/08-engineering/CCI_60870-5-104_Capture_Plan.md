# IEC 60870-5-104 — Capture Plan (operator telecontrol)

**Document ID:** CCLI-PROTO-60870-104-001  
**Revision:** 1.0  
**Date:** 2026-09-21  
**RAG source_id:** `ccli-60870-5-104-capture-plan` → `ccli-60870-5-104-extract`  
**Revision:** 1.1  
**Normative basis:** IEC 60870-5-104:2006 (+ compatible AMDs per utility)  
**PDF:** `regulations/standards-drop-20260918/IEC 60870-5-104-2006.pdf` (149 pp.)  
**Programme link:** K4.6 · REQ-104-001 · `iec104_adapter.cpp` (stub)

---

## Summary

**60870-5-104** is the **Eth_B / Operatore Abilitato** protocol for Italian CCI class products. Capture focuses on: **STARTDT/STOPDT**, **TESTFR**, ASDU types used for monitoring/commands, **redundancy** (if Allegato requires), and coupling to **62351-3/5** for TLS.

---

## Capture batches

| Batch | Focus | Status |
|-------|--------|--------|
| **A** | Scope, TCP 2404, APCI | **COMPLETE** |
| **B** | Station / ASDU address structure | **COMPLETE** |
| **C** | Monitor direction (M_SP_NA, M_ME_NA, …) | **COMPLETE** |
| **D** | Command / arming (if in AiLux / Allegato cross-ref) | **COMPLETE** |
| **E** | Security pointer to 62351-3/5 | **COMPLETE** |

---

## CCLI mapping

| Item | Path |
|------|------|
| Adapter stub | `apps/ccli/adapters/iec60870_104/iec104_adapter.cpp` |
| Config | `lab_tr400.yaml` → `eth_b` (future) |
| Zone | C2 conduit `CCI_62443_Zones.md` |

---

## Keywords

`60870-5-104`, `Eth_B`, `capture`, `ccli-60870-5-104-capture-plan`

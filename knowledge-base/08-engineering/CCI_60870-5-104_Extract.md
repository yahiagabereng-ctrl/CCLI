# IEC 60870-5-104 — CCLI Engineering Extract

**Document ID:** CCLI-PROTO-60870-104-001  
**Revision:** 1.0  
**Date:** 2026-09-21  
**RAG source_id:** `ccli-60870-5-104-extract`  
**Normative basis:** IEC 60870-5-104:2006 — *Network access for IEC 60870-5-101 using standard transport profiles*  
**Licensed PDF:** `07-protocols/regulations/standards-drop-20260918/IEC 60870-5-104-2006.pdf` (149 pp.)  
**Capture status:** **HAVE (P1)** — Batches A–E per `ccli-60870-5-104-capture-plan`  
**Parents:** `ccli-62351-3-extract` · `ccli-62351-5-extract` · `ccli-62443-zones` · `cei-0-16-cci-manual-ailux`  
**Programme link:** K4.6 · REQ-104-001 · Eth_B · `iec104_adapter.cpp` · lib60870

---

## Summary

**60870-5-104** carries **60870-5-101 application data (ASDU)** over **TCP/IP**. The **APCI** wraps ASDUs with **start/stop**, **test**, **sequence numbering**, and **timeout** supervision. Default **TCP port 2404**. **CCLI** uses 104 on **Eth_B (Operatore Abilitato)** with **62351-3 TLS** + **62351-5** application authentication when secured profile is required.

---

## Requirements

| REQ-ID | Source | Requirement | CCLI |
|--------|--------|-------------|------|
| **REQ-104-SCOPE-001** | §1 | Telecontrol compatible with 101 ASDU set over TCP | lib60870 CS104 |
| **REQ-104-TCP-001** | Transport | TCP connection; port **2404** (default) | `lab_tr400.yaml` eth_b |
| **REQ-104-APCI-001** | APCI | **STARTDT act/con**, **STOPDT act/con** before user data | Connection FSM |
| **REQ-104-APCI-002** | APCI | **TESTFR act/con** keepalive; t0 supervision | Watchdog in adapter |
| **REQ-104-APCI-003** | APCI | **I-format** numbered ASDUs; **S-format** ack; **k** / **w** window | lib60870 defaults + config |
| **REQ-104-ADDR-001** | ASDU | **Type identification**, **VSQ**, **COT**, **CA** common address | Station plan from DSO/OA |
| **REQ-104-MON-001** | Monitor | Single-point / measured values (e.g. **M_SP_NA_1**, **M_ME_NA_1**, **M_ME_NC_1**) | Telemetry to OA |
| **REQ-104-CMD-001** | Command | Commands / setpoints (e.g. **C_SC_NA_1**, **C_SE_NC_1**) if arming profile requires | Phase 4+ |
| **REQ-104-RED-001** | Redundancy | Dual connection / redundancy optional per utility | Document if Allegato requires |
| **REQ-104-SEC-001** | 62351-3 | TLS on separate secure port when enabled | C2 conduit |
| **REQ-104-SEC-002** | 62351-5 | Application-layer auth with TLS on networked path | After 62351-5 extract tests |

---

## Batch A — TCP and APCI

| Element | Description | Typical parameter |
|---------|-------------|-----------------|
| **TCP** | Reliable stream | Port **2404** |
| **U-format** | Unnumbered control | STARTDT, STOPDT, TESTFR |
| **I-format** | Information transfer | Send/recv sequence counters |
| **S-format** | Supervisory ack | Ack up to N(R) |
| **t0** | Connection timeout | e.g. 30 s (PIXIT) |
| **t1** | Send or test APDU timeout | e.g. 15 s |
| **t2** | Ack for I-format timeout | e.g. 10 s |
| **t3** | Idle TESTFR period | e.g. 20 s |
| **k** | Max unconfirmed I APDUs | e.g. 12 |
| **w** | Latest ack after w I APDUs | e.g. 8 |

**CCLI:** expose k/t/w/t0–t3 in config; log state transitions for CR 6.2.

---

## Batch B — Addressing

| Field | Size / role |
|-------|-------------|
| **Common address (CA)** | Station / RTU address |
| **Information object address (IOA)** | Point index |
| **Originator address (OA)** | Optional on commands |

Map IOA → internal `signal_map.yaml` (future) for PF2 and plant tags.

---

## Batch C — Monitor direction (examples)

| Type ID | Meaning | CCLI use |
|---------|---------|----------|
| **M_SP_NA_1** | Single-point | Status / alarms |
| **M_DP_NA_1** | Double-point | Breaker position |
| **M_ME_NA_1** | Normalized measured value | Legacy scaling |
| **M_ME_NC_1** | Short floating point | **P**, **Q** export to OA |
| **M_IT_NA_1** | Integrated totals | Energy counters |

Quality descriptor **IV NT SB BL OV** — treat invalid as **REQ-PF2** stale-data analogue.

---

## Batch D — Commands and arming

AiLux / Italian CCI manuals reference **104** for **Gestione Armamenti** (multicast/unicast arming). Capture utility-specific **COT** and **IOA** from **OA workbook** when issued.

| Pattern | Intent |
|---------|--------|
| Select/Execute command | Interlocking before curtailment |
| Set-point | Active power limits |

**CCLI:** gate 104 commands through same **permissive + PF2** policy as local logic where applicable.

---

## Batch E — Security stack (C2)

```
OA ── TLS (62351-3) ── TCP 2404 secure port ── 62351-5 auth ── 104 ASDU ── CCI
```

Modbus RS485 **C3** remains outside 62351 — document in `CCI_62443_Zones.md`.

---

## CCLI mapping

| Item | Path |
|------|------|
| Adapter | `apps/ccli/adapters/iec60870_104/iec104_adapter.cpp` |
| Library | lib60870 CS104 (see `ccli-github-protocol-libs`) |
| Network | Eth_B `192.168.30.20/24` (target VLAN) |
| Zone | C2 in `CCI_62443_Zones.md` |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-104-APCI-001 | Simulator STARTDT | User data accepted after con |
| REQ-104-MON-001 | Inject M_ME_NC_1 | OA HMI shows P within scaling |
| REQ-104-SEC-001 | TLS handshake | TLS 1.2+ per 62351-3 |
| REQ-104-SEC-002 | 62351-5 STAS | Auth success/fail events in 62351-14 |

---

## RAG routing

```
ccli-60870-5-104-extract, ccli-62351-3-extract, ccli-62351-5-extract, ccli-62443-zones, ccli-cci-module-regs-class
```

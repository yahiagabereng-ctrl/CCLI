# Cloud operator UI / HMI — product architecture

**Document ID:** CCI-CLOUD-HMI-001  
**Date:** 2026-10-05  
**Status:** **DECISION** — HiTEKS product uses **cloud-hosted UI/HMI**; no local REGALGRID-class panel on TG544  
**Platform:** TesPro TG544 / TG-524 (`platform_tg500`) · OpenWrt 25.12 FROZEN  
**Companion:** [P4_OPERATOR_ROLE.md](../../lab/evidence/phase4/P4_OPERATOR_ROLE.md) · [REGALGRID_SNOCU_HMI_INGEST.md](../../lab/evidence/field/regalgrid_snocu/REGALGRID_SNOCU_HMI_INGEST.md) · [LAN2_READINESS.md](../../lab/evidence/phase4/LAN2_READINESS.md)

---

## Decision summary

| Layer | Product choice | Lab / interim |
|-------|----------------|---------------|
| **Operator HMI (human UX)** | **Cloud web/mobile app** | `mock_console.html` — **lab only** (P7-12) |
| **Local LCD panel** | **Not in v1 scope** | REGALGRID photos = parity reference only |
| **DSO regulation (Annex T)** | **Direct Eth_A MMS** — never via cloud | LAN1 `:102` / `:3782` |
| **Operatore Abilitato wire (O.13 Eth_B)** | **IEC 60870-5-104 TLS** on LAN2 | Phase 4 PASS (monitor-only) |
| **Telemetry to cloud** | **Northbound uplink** (LTE primary) | TesPro MQTT collector (P5-H cross-check) |
| **Engineering access** | SSH / LuCI on **engineering zone** — not OA HMI | P2-04 PART |

**Rationale:** AiLux-class “web interface for diagnostics, logs, SCADA” is delivered as **SaaS/cloud**, not firmware-bundled panel UI. On-device flash/RAM stays for regulation + protocols; rich HMI lives in cloud.

---

## Requirements

| ID | Requirement | Source | Status |
|----|-------------|--------|--------|
| REQ-OP-CLOUD-001 | Operator **human** interface is cloud-hosted (web/app), not mandatory local panel | Product · REGALGRID parity defer | **DECISION** |
| REQ-OP-CLOUD-002 | DSO **Annex T** commands remain **direct Eth_A MMS** (62351-3/4); cloud does not proxy DSO | O.13.1.1.1 · REQ-PH-004 | **HAVE** (architecture) |
| REQ-OP-CLOUD-003 | Cloud receives **telemetry** (P, Q, V, PF2 state, comms, alarms) on agreed cadence | O.8.3 · operator supervision | **OPEN** — northbound API |
| REQ-OP-CLOUD-004 | Cloud **events / O.14** read path (syslog or API export) for authorised users | O.14 · P7-03 | **OPEN** |
| REQ-OP-CLOUD-005 | Eth_B **60870-5-104** remains available for **direct SCADA/aggregator** clients (no cloud dependency) | O.13.1.1.1 Eth_B | **HAVE** (protocol r18–r30) |
| REQ-OP-CLOUD-006 | Cloud command path (if any) is **distinct from DSO** and subject to Operating Rule + O.14 audit | O.10.3.1 MSD · O.14 | **OPEN** — scope TBD |
| REQ-OP-CLOUD-007 | Lab mock UI (`mock_console.html`) **not shipped** in product image | P7-12 | **OPEN** |
| REQ-OP-CLOUD-008 | LTE uplink is **primary** cloud path; Eth_B/LAN2 not required for cloud HMI | WAN backup architecture | **PART** — Quectel on DUT |

---

## Architecture

### Functional

```text
                    ┌─────────────────────────────────────┐
                    │     HiTEKS Cloud (UI / HMI)         │
                    │  dashboards · alarms · history      │
                    │  authorised user login (OIDC/TBD)   │
                    └──────────────▲──────────────────────┘
                                   │ HTTPS / MQTT (TLS)
                                   │ telemetry + events (+ optional cmds)
                    ┌──────────────┴──────────────────────┐
                    │  CCI (ccli) · TG544                 │
                    │  PF2 · EventStore · MeasurementStore  │
                    └──┬──────────────┬──────────────┬────┘
                       │              │              │
              Eth_A LAN1         Eth_B LAN2      LTE / WAN
              MMS server         104 TLS slave   northbound agent
              DSO direct         OA / SCADA       → cloud broker
```

| Actor | Path | Function |
|-------|------|----------|
| **DSO** | Eth_A `192.168.10.1` MMS | Wlim, WSd, VArSd — **mandatory direct** |
| **Operatore / aggregator (SCADA)** | Eth_B `192.168.1.130` 104 TLS | Supervision GI + monitored points — **regulatory wire** |
| **Operator (human)** | **Cloud UI** via LTE | Status, logs, alarms, history — **product HMI** |
| **Plant** | RS485 / LAN3 GOOSE | Modbus / GOOSE inputs |

### Network

| Domain | Interface | Cloud relevance |
|--------|-----------|-----------------|
| Z1 DSO | LAN1 Eth_A | **No cloud** on DSO path |
| Z2 Operator | LAN2 Eth_B | 104 for **machine** clients; cloud is parallel |
| Z3 Plant | LAN3 / RS485 | Plant data → ccli → cloud telemetry |
| WAN | LTE `usb0` / backup | **Primary cloud uplink** |
| Eng | USB / restricted | Factory; not operator HMI |

### Security

| Topic | Policy |
|-------|--------|
| DSO isolation | Cloud agent **must not** listen on Eth_A or bridge zones |
| Cloud auth | Mutual TLS or token-based device registration (62443-4-2 SL-T) — **TBD** |
| O.14 | Cloud-visible actions logged on device; remote syslog/API = P7-03 |
| Lab mock UI | Disabled in product (`P7-12`) |

### Power

Cloud HMI is **non-real-time** for regulation; PF2 actuation stays on-device. LTE loss → local regulation continues; Eth_B 104 fallback per P4-04.

---

## Interface matrix

| Interface | Source | Destination | Protocol | Notes |
|-----------|--------|-------------|----------|-------|
| DSO commands | DSO SCADA | ccli Eth_A | MMS TLS 62351-3/4 | Annex T — **not cloud** |
| OA supervision | External SCADA | ccli Eth_B | 60870-5-104 TLS | P4 closed; r30 regression pending |
| Cloud telemetry | ccli / northbound agent | HiTEKS cloud | MQTT or HTTPS **TBD** | REQ-OP-CLOUD-003 |
| Cloud events | EventStore export | HiTEKS cloud | RFC 5424 / REST **TBD** | P7-03 |
| Cloud HMI (human) | Browser / app | HiTEKS cloud | HTTPS | REQ-OP-CLOUD-001 |
| Lab mock | Engineer PC | DUT `:80` mock_console | HTTP | Lab only |
| TesPro collector (interim) | TesPro iec61850d | PC/cloud broker | MMS client + MQTT | P5-H; not product northbound |

---

## BOM matrix

| Block | Function | Candidate / approach | Status |
|-------|----------|------------------------|--------|
| Cloud platform | UI, auth, multi-tenant | HiTEKS SaaS **TBD** | **OPEN** |
| Device northbound | Telemetry + heartbeat | First-party `ccli` MQTT/HTTPS **or** interim TesPro | **OPEN** |
| LTE modem | WAN | Quectel EC200A (on TR544) | **HAVE** |
| Local panel HMI | Field operator display | REGALGRID-class LCD | **N/A** — cloud replaces |
| Eth_B 104 | Direct SCADA | lib60870 in ccli | **HAVE** |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| Cloud API contract | TesPro MQTT JSON (lab) | HiTEKS canonical schema | **MISSING** |
| Cloud command scope | MSD O.10.3.1 deferred | Operating Rule decision | **MISSING** |
| Device provisioning | Manual lab broker | PKI / tenant onboarding | **MISSING** |
| REGALGRID page parity | 13-page inventory | Cloud screen map | **PART** — see field ingest |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-CLOUD-01 | Operators assume cloud replaces Eth_B 104 | CEI non-compliance | Keep 104 on LAN2; document roles |
| R-CLOUD-02 | Cloud path used for DSO commands | Security / audit failure | REQ-OP-CLOUD-002 hard rule |
| R-CLOUD-03 | LTE outage hides plant alarms | Ops blind spot | Local O.14 store + Eth_B 104 + SMS Annex M |
| R-CLOUD-04 | TesPro northbound mistaken for product | Wrong architecture | Interim lab only; first-party agent |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-OP-CLOUD-002 | Zone audit | No cloud listener on Eth_A |
| REQ-OP-CLOUD-005 | LAN2 104 session | GI + TotW read (P4-02 replay on r30) |
| REQ-OP-CLOUD-003 | Uplink test | Cloud dashboard shows P/Q within 4 s of plant change |
| REQ-OP-CLOUD-007 | Product image audit | No `mock_console` in release rootfs |
| REQ-OP-CLOUD-001 | UX review | Operator tasks doable without local panel |

---

## Programme placement

| Track | Focus | Bench |
|-------|-------|-------|
| **Now (LAN1)** | P7 DSO demo · P5-R · P7-01 | LAN1 + COM5 |
| **LAN2 prep (parallel)** | 104 monitor points · O.14 operator events · r30 regression script | Yaml/docs — [LAN2_READINESS.md](../../lab/evidence/phase4/LAN2_READINESS.md) |
| **LAN2 wire day** | P4 r30 regression · `operator_asdu_*` · TotVAr on 104 | Cable on LAN2 |
| **Cloud (new stream)** | Northbound API · cloud UI MVP · provisioning | LTE + lab MQTT broker pattern |
| **Deferred** | Local panel · MSD commands on 104 | Commercial / v2 |

---

## Related evidence

| Artifact | Role |
|----------|------|
| [P5_TESPRO_NORTHBOUND.md](../../lab/evidence/phase5/P5_TESPRO_NORTHBOUND.md) | Lab MQTT pattern (interim) |
| [REGALGRID_SNOCU_HMI_INGEST.md](../../lab/evidence/field/regalgrid_snocu/REGALGRID_SNOCU_HMI_INGEST.md) | Field HMI reference → cloud screen backlog |
| [P4_OPERATOR_ROLE.md](../../lab/evidence/phase4/P4_OPERATOR_ROLE.md) | Eth_B 104 role vs DSO |

---

*Update when cloud API schema or northbound agent is specified.*

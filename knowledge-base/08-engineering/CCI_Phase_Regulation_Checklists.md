# CCLI phases — regulation checklists

**Document ID:** CCLI-PLAN-PHASE-REGS-001  
**Revision:** 1.3  
**Date:** 2026-10-01  
**RAG source_id:** `ccli-phase-regulation-checklists`  
**Audience:** Lab, firmware, DSO interface, certification  
**Companions:** `CCI_CCLI_Product_Spec_and_Roadmap.md` · `CCI_Annex_O_Extract.md` · `CCI_Annex_T_Extract.md` · `CCI_Annex_M_Extract.md` · `lab/PHASE1_TRACEABILITY.md` · `lab/CCI_Reactive_Roadmap_Gap_Report.md`  
**Lab master log (tools · IPs · commands · log excerpts · evidence index):** `lab/CCLI_PHASE_LAB_MASTER_LOG.md`

---

## Phase close register (lab programme)

Per **REQ-PH-002**: a phase closes when all **Gate** rows are **PASS** or **WAIVED**/**PART** with named owner. Product certification remains **Phase 7**.

| Phase | Lab exit | Closed | Residual (owner) | Next |
|-------|----------|--------|------------------|------|
| **0** Platform bind | P0-01…05 PASS/PART/WAIVED | **CLOSED** (2026-09-25) | Photo polish → lab | — |
| **1** Local PF2 | Engineering evidence; DIO walk optional | **CLOSED** (eng.) 2026-09-22 freeze | Real permissive walk → lab | — |
| **2** Isolation | P2-01…03 hard gates | **CLOSED** (2026-09-25) | P2-04/05 PART · P2-06 OPEN → net/architect | — |
| **3** DSO MMS | P3-01 + P3-04 + P3-06 + soft cyber/time | **CLOSED** (lab) 2026-09-25 | P3-10/12/13/14 · §13 · SCEP → model/PKI | **Phase 4** |
| **4** Operator 104 + TesPro 61850 | P4-01…05 **PASS** 2026-09-30 | P4-00 **PART** (LuCI/plant optional) | **CLOSED** (lab) 2026-09-30 |
| **5** Observability + reactive | P5_FULLCIRCLE **CLOSED** (lab) 2026-10-01 · build **0.1.0-r25** · `P5_FINAL_CLOSEOUT.md` | **CLOSED** (lab gate) | P5-R02–R09 · P5-04 accuracy · P5-06 meter map · REQ-MET-002 grid | **Phase 6** |
| **6** Defence I/O | Not started | **OPEN** | C4 GAP | After P1 |
| **7** Evidence pack | Not started | **OPEN** | Lab evidence ≠ O.15 cert | After P3+P5+P6 |

**Phase 3 lab close statement:** DSO client on Eth_A TLS can browse the MVP model, write `Wlim` into the P1 actuator path, and receive TimeQuality with chrony UTC ≤ ±100 ms. Residuals below are **PART/WAIVED**, not exit blockers.

---

## Requirements

| ID | Requirement |
|----|-------------|
| REQ-PH-001 | Every phase has a **regulation scope** and an **explicit out-of-scope** list |
| REQ-PH-002 | A phase **cannot close** until all **Gate** rows are **PASS** or **WAIVED** with written owner |
| REQ-PH-003 | DSO MMS **active P** (`Wlim` / `WSd` O.9.2.x) is **Phase 3** — not Phase 1 GPIO |
| REQ-PH-004 | Annex **M** teledistacco is **Phase 6** — not Eth_A |
| REQ-PH-005 | Product cert evidence (O.15) is **Phase 7** — not a lab exit |
| REQ-PH-006 | DSO **reactive** functions (**O.9.1** / T Tables 88–92) and mandatory **TotVAr** (T.3.1.3) are **Phase 5** (P5-M measure · P5-R control) — not Phase 3 reopen |

---

## Architecture

Phases are **regulation gates**, not calendar weeks. Numbers **0–7** are unchanged so existing lab docs still apply. Names are adjusted so each phase owns **one CEI/IEC cluster**.

```text
P0  Platform bind     →  no CEI function yet (traceability of ports)
P1  Local PF2         →  Annex O actuation + stale-safe (no DSO)
P2  Isolation         →  O.13.1 no-bridge + 62443 zones
P3  DSO MMS (active)  →  Annex T + O.9.2.2/3 Wlim/WSd + 62351
P4  Operator 104      →  Eth_B / 60870-5-104 (optional vs CEI core)
P5  Observability + Q →  O.8 PF1 + T MMXU P/Q/V + O.9.1 reactive DSO
P6  Defence I/O       →  Annex M + O.11 defence priority + C4 product DI/DO
P7  Evidence          →  O.14 logger + O.15 cert pack
```

**Critical path:** P0 → P1 → P2 → P3.  
**P4** may run after P2. **P5** after P1 (lab) / after P3 (DSO publish). **P5-R** (reactive control) needs plant Q path + P5-M TotVAr. **P6** after P1. **P7** after P3+P5+P6.

**P5 split (same phase number — two gate groups):**
- **P5-M** — measurements DSO must **see** (O.8 · T.3.1.3 · TotW/TotVAr/PPV)
- **P5-R** — reactive functions DSO may **command** (O.9.1 · O.7.3.1 TsQ · O.11 indices 5–7)

### Status legend

| Status | Meaning |
|--------|---------|
| **OPEN** | Not started |
| **PEND** | Implemented or planned; no signed evidence |
| **PART** | Same intent, incomplete vs clause |
| **PASS** | Evidence recorded |
| **WAIVED** | Out of this phase; owner + later phase named |
| **N/A** | Clause does not apply to this phase |

### How to close a row

Fill **Evidence** with date, APK/`ccli` version, log excerpt, screenshot, or test ID. Do not delete **OPEN** rows.

---

## Interface matrix

| Phase | Primary annex / standard | Source | Destination | Protocol |
|-------|--------------------------|--------|-------------|----------|
| P1 | O.9.2 mode (i), O.13 stale | Meter / mock | Plant actuator | Modbus RTU + DIO |
| P2 | O.13.1.1.1 | Eth_A / Eth_B / plant | Isolated domains | L2/L3 firewall |
| P3 | T + O.9.2.2/3 | DSO | CCI MMS server | IEC 61850-8-1 + 62351 |
| P4 | O.13.1.1.1 Eth_B | Operatore Abilitato | CCI | IEC 60870-5-104 + 62351-3/5 |
| P5-M | O.8 / T.3.1.3 | Plant / analyzer | DSO (via T) | 4 s MMXU TotW + **TotVAr** + PPV |
| P5-R | O.9.1 / T Tables 88–92 | DSO | Plant Q / cosφ path | MMS Operate → inverter/plant |
| P6 | M + O.11 defence | Defence modem / SPI | CCI inhibit + DDI | Dry contact / DI |
| P7 | O.14 / O.15 | CCI | Auditor / DSO | Syslog, cert files |

---

## BOM matrix

| Block | Function | Candidate | Phase |
|-------|----------|-----------|-------|
| TR400 DIO IO1–IO2 | Lab PF2 | TesPro GD32 / `dido_v2` | P1 |
| Eth ports | Domain isolation | WAN/LAN silkscreen + firewall | P2 |
| MMS stack | DSO server | libiec61850 + CID | P3 |
| 104 stack | Operator | lib60870 | P4 |
| Analyzer | POC P/Q/V | Licensed meter + map | P5-M |
| **61850 client** | **Eth_A verification** | **Triangle MicroWorks 61850 Test Suite Pro** | P3+ / P5 |
| Plant Q path | VArSd / PFSP / curves | Inverter Modbus / GOOSE / vendor | P5-R |
| Plant GOOSE RX | Annex T Type 1 optional subscribe | libiec61850 GooseReceiver on plant IF | **P5-G** |
| C4 / M I/O | 10–120 V DI, teletrip | Expansion / ISO1212 class | P6 |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| 61850-8-1 | 2004 OCR | 2011 Ed.2 + AMD2018 | P3 corpus |
| 61850-7-3 | PDF **HAVE** (drop) | Engineering extract **HAVE** | `ccli-61850-7-3-extract` 2026-09-30 |
| 61850-7-4 | PDF **HAVE** (drop) | Engineering extract **HAVE** | `ccli-61850-7-4-extract` 2026-09-30 |
| 62351-100-3 | Cited in O.15 | PDF + lab plan | P7 |
| ARERA 385 / 564 | O §2.1 summary | Full delibere | P7 legal |
| Allegato U | In consolidata | English extract + timers | P3 fallback |
| DSO ICD / `signal_map.yaml` | TR 57-126 example only | Plant-specific CID | P3 |
| Analyzer registers | Lab float 40001 (+ Q reg) | Real model map P+Q+V | P5-M |
| Plant Q actuation | None (PF2→DIO1 is P only) | Path to inverters for O.9.1 | **P5-R** — **MISSING** |
| O.9.1 LN APC + handlers | STUB Mod=5 only | VArSd/PFSP/VArV/PFW operable | P5-R |
| TotVAr / PPV in MMS | TotW only (P3) | T.3.1.3 mandatory set | P5-M |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-PH-01 | Close P1 as “CEI PF2” | False DSO claim | P1 statement: local loop only |
| R-PH-02 | Run P3 on `br-lan` | Invalid MMS evidence | P2 Gate first |
| R-PH-03 | Treat GPIO as Annex M | Wrong voltage class | P6 expansion |
| R-PH-04 | Skip O.14 until last week | Demo fails audit | Start event store in P1, finish P7 |
| R-PH-06 | Close P5 on TotW only | Fail T.3.1.3 (TotVAr/PPV mandatory) | P5-M07/08 Gate |
| R-PH-07 | Operate VArSd without plant Q path | Silent / false DSO compliance | P5-R Gate + GAP if no inverter API |

---

## Verification — master phase table

| Phase | Name | Regulation cluster | Exit (one sentence) | Gate on | Lab status |
|-------|------|--------------------|---------------------|---------|------------|
| **0** | Platform bind | Traceability (not a CEI function) | Ports, devices, YAML named and photographed | — | **CLOSED** |
| **1** | Local PF2 | **O.9.2** actuation, **O.13** stale, **62443 CR 3.6** | Local P → FSM → DIO1; stale → safe | P0 | **CLOSED** (eng.) |
| **2** | Isolation | **O.13.1.1.1** no bridge, **62443-3-2** | No L2 path Eth_A ↔ Eth_B ↔ plant ↔ WAN | P0 | **CLOSED** |
| **3** | DSO MMS | **T**, **O.9.2.2**, **62351-3/4/8/9**, **TR 57-126** | DSO client reads model and writes `Wlim` on Eth_A TLS | P2 | **CLOSED** (lab) |
| **4** | Operator 104 + supplier 61850 | **P4-00** TesPro collector; **O.13 Eth_B**, **60870-5-104**, **62351-3/5** | Supplier apk + services; 104 on Eth_B only; no leak to Eth_A | P2 | **CLOSED** (lab) 2026-09-30 |
| **5** | Observability + reactive | **O.8**, **T.3.1.3 P/Q/V**, **O.9.1**, **O.7.3 TsQ/ΔT**, **61557-12** | 4 s TotW+TotVAr; VArSd (or WAIVE) operable | P1/P3 | **OPEN** |
| **6** | Defence I/O | **M**, **O.11**, **O.9.3**, **O.12**, **REQ-IO-004** | Teletrip inhibit + product DI/DO class or written GAP | P1 | **OPEN** |
| **7** | Evidence | **O.14**, **O.15**, **62443-4-1/4-2**, **ARERA** | Logger + cert launch pack; not accredited cert | P3+P5+P6 | **OPEN** |

---

## Phase 0 — Platform bind

**CEI function:** none. This phase exists so later clauses have a **named physical interface**.

**In scope:** SSH, silkscreen ↔ Linux, `lab_tr400.yaml`, photo evidence.  
**Out of scope:** PF2, MMS, 104, cert.

| Check | Regulation | Shall | Pass criteria | Status |
|-------|------------|-------|---------------|--------|
| P0-01 | REQ-NET-003 / O.13.1.1.1 roles | Each RJ45 has role **Eth_A / Eth_B / plant / eng** | Table + photo; `ip link` names | **PASS** — superseded by P2 annex map 2026-09-25 |
| P0-02 | REQ-SER-001 | RS485 A2/B2 ↔ `/dev/ttyS2` (or recorded alternate) | `ls -l` + wiring photo | **PART** — DUT uses `/dev/rs485_2_uart` (recorded) |
| P0-03 | REQ-IO-001/002 | DIO1=curtail DO, DIO2=permissive DI via `dido_v2` | Matches `lab_tr400.yaml` | **PASS** — `lab/TR400_HW_IO_CONFIG.md` |
| P0-04 | 62443-3-2 ZCR | Zones sketched on same port table | `CCI_62443_Zones.md` updated | **PART** — annex map; ZCR asset owner MISS |
| P0-05 | REQ-BLD-002 | `ccli` package + config path documented | `/usr/sbin/ccli`, `/etc/ccli/lab.yaml` | **PASS** |

**Exit:** P0-01…05 all PASS or WAIVED/PART with owner.  
**Lab close:** **CLOSED** 2026-09-25 (P0-02/04 PART owned by HW/architect).

---

## Phase 1 — Local PF2 (Annex O without DSO)

**This is not DSO PF2.** It proves the **plant actuation path** that O.9.2 later slaves to `Wlim`.

**In scope:** Modbus P, FSM, DIO1/DIO2, stale-safe, deploy.  
**Out of scope:** Eth_A MMS, `Wlim`, Annex M, 104, 5 DI / 10–120 V product class.

| Check | Regulation | Shall | Pass criteria | Status |
|-------|------------|-------|---------------|--------|
| P1-01 | O.9.2 (concept) / REQ-PF2-001 | Limitation function exists | FSM states + debounce 30 s (L1-P-03) | **PEND** |
| P1-02 | O.9.2 mode (i) | Reduce generation via plant output | DIO1 relay ON after threshold hold | **PEND** |
| P1-03 | O.9.2 hysteresis | Release below release threshold | 850 kW path (L1-P-04) | **PASS** (unit) |
| P1-04 | O.13 / VAL §9 / REQ-PF2-002 | Lost or stale meter → safe; **no stale-as-valid curtail** | Stop slave → DIO1 OFF ≤ 10 s (L1-C-01) | **PEND** |
| P1-05 | L1-C-03 | Bad CRC / exception ≠ Good | Frame rejected; quality not Good | **PEND** |
| P1-06 | 62443 CR 3.6 / REQ-IO-003 | Boot → defined safe DO | Coil de-energised at init (L1-IO-01) | **PASS** (unit) |
| P1-07 | VAL §7 / REQ-LAB-003 | Permissive gates actuation | `permissive_bypass: false`; DIO2 open blocks DIO1 | **PEND** |
| P1-08 | O.14 (start) / REQ-PF2-004 | Log start/stop of limitation | Timestamped reason in log | **PART** |
| P1-09 | — (lab) | Deployed binary is the one under test | APK/version on DUT | **PASS** |

**Must remain WAIVED in Phase 1 (do not mark PASS):**

| Check | Regulation | Move to |
|-------|------------|---------|
| P1-W1 | O.9.2.2 DSO command / T `Wlim` | **P3** |
| P1-W2 | O.9.2.1 V ≈ 110 % Un | **P5** (needs voltage) |
| P1-W3 | O.11 / Annex M inhibit | **P6** |
| P1-W4 | T `MMXU1.PdC.TotW` 4 s | **P5** |
| P1-W5 | REQ-IO-004 5 DI + 3 DO @ 10–120 V | **P6** |

**Exit:** P1-01…09 PASS; P1-W* listed as WAIVED.  
**Sign-off sentence:** Phase 1 is **engineering evidence** of local limitation — **not** CEI 0-16 DSO PF2.  
**Lab close:** **CLOSED** (engineering) 2026-09-22 freeze · residual DIO/permissive walk optional.

---

## Phase 2 — Isolation (O.13 + 62443)

**In scope:** No internal bridge; WAN/LTE cannot reach DSO MMS; zone paper.  
**Out of scope:** MMS payload, 104 application, PF2 thresholds.

| Check | Regulation | Shall | Pass criteria | Status |
|-------|------------|-------|---------------|--------|
| P2-01 | O.13.1.1.1 | **No** switch/bridge among Eth_A, Eth_B, local config, plant | Role PC↔PC ping blocked (not TG IPs); evidence `P2_01_CROSS_PING_MATRIX.txt` | **PASS** — 2026-09-25 |
| P2-02 | O.13.1.1.1 / REQ-NET-001 | Three (or more) **separate** L3 domains | Firewall / VLAN rules committed | **PASS** — zones lan1_sec/lan_sec/lan2_sec, no role→role forward |
| P2-03 | O.13.1.1.1 / REQ-NET-002 | LTE/WAN **backup only** — no path to DSO MMS | `ping -I usb0` to DSO FAIL; firewall no wan→lan1_sec | **PASS** — 2026-09-25 `P2_03_LTE_ISOLATION.txt` |
| P2-04 | O.13.1.1.2 | Local config is USB/serial (or documented engineering exception) | SSH/LuCI only on eng port, not Eth_A | **PART** — LuCI still on OA `.1.130` (2026-09-25) |
| P2-05 | 62443-3-2 | Zones/conduits signed by architect | `CCI_62443_Zones.md` Rev 1.6 + `Architecture/Zones_62443_TG544_PortMap.drawio` | **PART** — annex map 2026-09-25; ZCR 7 asset owner MISS |
| P2-06 | O.14 (link status) | Physical + data-link status of Eth_A/B logged | Log fields present | **WAIVED** (lab) → **P7** O.14 logger · owner: firmware |
| P2-G01 | O.13.1.1.1 plant | Plant GOOSE (L2) **not** on DSO Eth_A bind | GOOSE listener on plant IF only; no bridge to MMS zone | **OPEN** — `lab/GOOSE_ADAPTATION.md` |

**Exit:** P2-01…03 **PASS** (hard gates). P2-04…06 PASS or dated PART/WAIVED with owner.  
**Lab close:** **CLOSED** 2026-09-25 (hard gates). Residuals P2-04/05 PART → net/architect.

---

## Phase 3 — DSO MMS (Annex T + O.9.2.2)

**This is the DSO channel.** GPIO does not receive DSO commands.

**In scope:** 61850 server on **Eth_A**, CID, `Wlim` → same PF2 actuator as P1, TLS/PKI.  
**Out of scope:** Plant Modbus map (P5), 104 (P4), teletrip modem (P6).

| Check | Regulation | Shall | Pass criteria | Status |
|-------|------------|-------|---------------|--------|
| P3-01 | T.1 / T.3 | IEC 61850 **mandatory** toward DSO | MMS listen on Eth_A only | **PASS** — 2026-09-25 `192.168.10.1:102` (`P3_01_MMS_LISTEN_ETH_A.txt`) |
| P3-02 | T.3 / TR 57-126 | CID implements T data model | Load `cei-tr-57-126-example.cid` then plant CID | **PART** — lab CID file + runtime **dynamic MVP** (Wlim/TotW/urcb); full genconfig CID load TBD |
| P3-03 | T.3.2.1 | Measurements every **4 s**, Type 3 | Client sees periodic TotW | **PASS** — 2026-09-25 avg 3961 ms (`P3_03_MMS_CLIENT_STEP5.txt`) |
| P3-04 | T control / O.9.2.2 | DSO **`Wlim`** slaves active-power limit | Write `Wlim` → P1 actuator path | **PASS** — 2026-09-25 WMaxSptPct=10→enter=21 kW + DIO1 ON (`P3_04_WLIM_ACTUATOR.txt`) |
| P3-05 | O.9.2.2 / Annex U | If no comms: Operating Rule fallback | Timer + autonomous mode documented | **PASS** — 2026-09-25 fallback_s=15 clears live Wlim (`P3_05_COMMS_LOSS_FALLBACK.txt`) |
| P3-06 | T.3.3 / 62351-3 | TLS 1.2+ transport | No cleartext MMS on wire | **PASS** — 2026-09-25 `192.168.10.1:3782` tls=on (`P3_06_MMS_TLS_ETH_A.txt`) |
| P3-07 | T / 62351-4 | 61850 security association | Mutual cert auth | **PASS** — 2026-09-25 ACSE TLS cert + mutual TLS (`P3_07_SECURITY_ASSOC.txt`); full §13 E2E native deferred |
| P3-08 | T.3.3.4 / 62351-8 | Role-based access (DSO vs others) | Wrong role cannot write `Wlim` | **PASS** — 2026-09-25 DSO OK / VIEWER DENY (`P3_08_SECURITY_RBAC.txt`); IECUserRoles product follow-up |
| P3-09 | T.3.3.4.9 / 62351-9 | PKI (not self-signed only) | Enrolment / CRL path | **PASS** (lab) — CA-signed + CRL deny revoked (`P3_08_09_RBAC_PKI.txt`); SCEP/EST deferred |
| P3-10 | T / XCBR1.IDG | Breaker / DG position in model | DO present (may be simulated) | **WAIVED** (lab) → product model · owner: 61850 model · reopen when CID expands |
| P3-11 | T.3.3.4.5 / T time | UTC quality ±100 ms | TimeQuality in reports | **PASS** (lab) — 2026-09-25 **chrony** ≤±100 ms; ccli observe-only + GNSS fix (`P3_11_CHRONY_TRACKING.txt`) |
| P3-12 | Programme | DSO ICD workbook + `signal_map.yaml` | Files in `apps/ccli/config/icd/` | **PART** — `signal_map.yaml` MVP freeze; plant workbook MISSING · owner: DSO interface |
| P3-13 | 61850-8-1:2011 | Mapping baseline for conformance | Ed.2 PDF + PICS draft | **PART** (2004 only) · owner: corpus · procure 2011 |
| P3-14 | 61850-7-3 / 7-4 | CDC/DA + LN/DO in CID justified | 7-3 + 7-4 extract | **PASS** (corpus) · lab runtime subset documented |

**Deferred (not exit blockers):** 62351-4 §13 native E2E · SCEP/EST enrolment · GNSS PPS preferred over NTP (C5).

**Exit:** P3-01, P3-04, P3-06 **PASS**. Others PASS or PART/WAIVED with named owner.  
**Blocked until:** P2-01 PASS.  
**Lab close:** **CLOSED** 2026-09-25 · `SESSION_REPORT_P3_LAB_CLOSE_2026-09-25.md`.

---

## Phase 4 — Operator 104 (Eth_B) + TesPro supplier 61850

**CEI core is 61850 on Eth_A (`ccli`).** 104 is **AiLux-class / O.13 Eth_B**, not a substitute for Annex T.

**In scope:** 104 slave/reverse on Eth_B; 62351-3/5; **TesPro IEC 61850 add-on** (supplier MMS client/collector).  
**Out of scope:** Replacing DSO MMS; TesPro as DSO server; plant GOOSE.

| Check | Regulation | Shall | Pass criteria | Status |
|-------|------------|-------|---------------|--------|
| P4-00 | REQ-VEND-008 / V-005 | TesPro **61850 collector** on DUT | 4× apk installed; mmsd + service running; optional LAN3 poll | **PART** (install PASS 2026-09-30) |
| P4-01 | O.13.1.1.1 Eth_B | Operator traffic on **Eth_B only** | No 104 listener on Eth_A | **PASS** — 2026-09-30 `P4_01_ETH_B_ONLY.txt` |
| P4-02 | IEC 60870-5-104 | Session + supervision | Connect, GI, reconnect | **PASS** — 2026-09-30 `P4_02_104_SESSION.txt` |
| P4-03 | 62351-3 / 62351-5 | Secure 104 | No cleartext ASDU on Eth_B | **PASS** — 2026-09-30 `P4_03_104_TLS.pcapng` |
| P4-04 | O.13.1.3 | Loss of Eth_B → agreed fallback | Timer + restore | **PASS** — 2026-09-30 `P4_04_104_FALLBACK.txt` |
| P4-05 | O.14 | Log operator commands | Command + params stored | **PASS** (lab param audit) · Phase 7 persistent store · `P4_05_OPERATOR_RULE_VERIFY.txt` |

**Exit:** P4-01…03 PASS if 104 is in product scope; else entire phase **WAIVED** in writing.  
**Operator role:** Eth_B 104 = supervision/monitor only; DSO commands on Eth_A — `lab/evidence/phase4/P4_OPERATOR_ROLE.md`.

---

## Phase 5 — Observability + reactive (PF1 / MMXU / O.9.1)

**In scope:** What DSO must **see** (O.8, T.3.1.3) **and** reactive functions DSO may **command** (O.9.1, T Tables 88–92).  
**Out of scope:** Reopening Phase 3 active-P exit; Annex M teletrip (P6); replacing Wlim/WSd.

**Normative anchors (RAG HAVE):**
- `CCI_Annex_T_Extract.md` §3 — TotW / **TotVAr** / **PPV** **Mandatory** @ PdC, period 4 s  
- `CCI_Annex_T_Extract.md` §5.4–5.8 — VArSd / VArSa / PFSP / VArV / PFW  
- `CCI_Annex_O_Extract.md` §3.1 — **TsQ ≤ 10 s**; §3.2 — ΔT 10–600 s (default 60 s)  
- `CCI_Annex_O_Extract.md` §8 Table 1 — O.11 priority indices **5–7** (reactive tier)  
- `apps/ccli/config/icd/signal_map.yaml` — LN freeze (STUB / DEFERRED today)

### P5-M — Measurements (DSO see)

| Check | Regulation | Shall | Pass criteria | Status |
|-------|------------|-------|---------------|--------|
| P5-01 | O.8 PF1 / ARERA 36/2020 | Observability quantities acquired | List vs O.8.4 complete or GAP table | **OPEN** |
| P5-02 | T.3.1.3 | PdC **P** every 4 s as `PdCMMXU1.TotW` | Client log 4.0 s ± tolerance | **PASS** (lab) — TotW=450 kW Modbus live · `P5_FINAL_VERIFY.txt` |
| P5-03 | O.7 / 61557-12 | MC200 **200 ms** fixed (non-sliding) blocks | Aggregator test | **OPEN** |
| P5-04 | O.13.2 | Accuracy class **≤ 0.2 %** **or** V5 **≤ 5 %** (100–500 kW wind/PV) | Chain calculation + instrument class | **OPEN** |
| P5-05 | O.9.2.1 | Optional: V ≈ 110 % Un limit | Only if voltage input exists | **N/A** until meter |
| P5-06 | K4.5 | Real analyzer register map (**P + Q + V**) | YAML + datasheet pages | **OPEN** |
| P5-M06 | 62351-3 / P3-06 | MMS TLS on Eth_A `:3782`; no cleartext `:102` | Wireshark TLS + mms_tls_client | **PASS** (lab) 2026-09-30 · `P5_MMS_TLS.pcapng` · `P5_MMS_TLS_ANALYSIS.txt` |
| P5-M07 | T.3.1.3 / O.8.3 | PdC **Q** every 4 s as `PdCMMXU1.TotVAr` | Client log TotVAr + q/t; Modbus `q_kvar` → MMS | **PASS** (lab) — TotVAr=45 kvar · `P5_FINAL_VERIFY.txt` |
| P5-M08 | T.3.1.3 | PdC **PPV** (phase-phase V) mandatory | MMS DO present + value or written meter GAP | **PASS** (lab) — PPV.phsAB=20 kV yaml · meter reg **GAP** P5-06 |
| P5-M09 | O.8.2 / T Table 80 | Qind / Qcap nameplate exposed (Operating Rule) | Matches `plant.q_*` / VARtg | **PART** — yaml only |

### P5-R — Reactive DSO control (DSO command)

| Check | Regulation | Shall | Pass criteria | Status |
|-------|------------|-------|---------------|--------|
| P5-R01 | O.9.1.4 / T Table 88 | `VArSdDVAR1.{Mod,VArSptPct}` operable on Eth_A | DSO Operate → live command + plant Q path **or** GAP | **PASS** (lab r25) |
| P5-R02 | O.9.1.1 / T Table 90 | `PFSPDFPF1` cosφ set-point | Operate + plant path **or** WAIVE (Operating Rule off) | **OPEN** |
| P5-R03 | O.9.1.3 / T Table 91 | `VArVDVVR1` Q=f(V) + δQ=5 % Qmax + ΔT | Curve + slow ring **or** WAIVE | **OPEN** |
| P5-R04 | O.9.1.2 / T Table 92 | `PFWDPFW1` cosφ=f(P) + δcosφ=0.02 + ΔT | Curve + slow ring **or** WAIVE | **OPEN** |
| P5-R05 | O.7.3.1 | **TsQ ≤ 10 s** to ±5 % of expected Q | Bench or unit evidence when P5-R01 on | **OPEN** |
| P5-R06 | O.7.3.2 / O.7.3.3 | ΔT 10–600 s (default 60); external SP spacing ≥ 3 s | Config + reject test | **OPEN** |
| P5-R07 | O.11 indices 5–7 | Reactive tier vs active (W110/Wlim/WSd) priority | Arbiter test matrix (extend `derive_*`) | **OPEN** |
| P5-R08 | O.10.3.2 / T Table 89 | `VArSa` MSD reactive SP | **Discretionary** — PASS or **WAIVE** | **N/A** until MSD contract |
| P5-R09 | O.14 | Log reactive Operate (Mod, set-point, result) | Event category present | **OPEN** |

### P5-G — Plant GOOSE (optional Annex T Type 1)

| Check | Regulation | Shall | Pass criteria | Status |
|-------|------------|-------|---------------|--------|
| P5-G01 | Annex T / plant bus | Subscribe to plant IED GOOSE when site uses L2 path | RX log + O.14 `goose` event + Wireshark | **OPEN** — code r28+ · lab evidence pending |
| P5-G02 | Site export | Optional GOOSE publish (status) | GoCB in CID + `IedServer_enableGoosePublishing` + wire | **PART** — r29 MMS/GoCB PASS (`P5_GOCB_MMS_2026-10-05.txt`); wire deferred — DUT `br-lan` NO-CARRIER |
| P5-G03 | O.8 / P5-M | GOOSE dataset → TotW/TotVAr **or** parallel Modbus | Measurement merge policy documented | **OPEN** |

**Note:** Modbus RTU (A2/B2) remains lab-default plant path; GOOSE does **not** replace P5-R01 Modbus Q write until P5-G03.

**Exit:**
- **P5-M minimum:** P5-02 + **P5-M07** + P5-04 (or V5 5 %) PASS. P5-M08 PASS or meter GAP.  
- **P5-R minimum:** **P5-R01** PASS **or** written plant-Q GAP with owner; other O.9.1 rows PASS or **WAIVE** per Operating Rule / TR Figura 2 (Inactive).  
- P5-01 GAP table allowed for unused generation sources (Gen* MMXU2).

**Code freeze today:** `signal_map.yaml` — TotVAr **HAVE** (r21+); PPV **HAVE** (r22); VArSd/PFSP/VArV/PFW **STUB** Mod=5; Modbus `q_kvar` **HAVE** → MMS TotVAr (lab PASS).

---

## Phase 6 — Defence I/O (Annex M + product C4)

**In scope:** Teletrip **must not fight** CCI (O.11); product DI/DO class.  
**Out of scope:** DSO MMS; replacing the M modem with GPIO.

| Check | Regulation | Shall | Pass criteria | Status |
|-------|------------|-------|---------------|--------|
| P6-01 | M.1 | Defence plan ≥ 100 kW: telescatto path exists **or** site waiver | Modem/SPI diagram or DSO letter | **PART** (lab mock) |
| P6-02 | M.5.1 | DO → PI “scatto da segnale esterno”; DI = DDI/PI feedback | Wiring photo vs M.5.1 | **DEFERRED** |
| P6-03 | O.11 / O.9.3 | CCI **inhibits conflicting** PF2 when M trips | IO3 (or equivalent) → FSM block | **PASS** (r26) |
| P6-04 | O.12 | Teledistacco predisposition on installation drawing | Fig. 128-class sketch | **PART** |
| P6-05 | O.14 | Log Annex M trip | Event category present | **PASS** (code) |
| P6-06 | REQ-IO-004 | Product **5× DI 10–120 V**, **3× DO 250 Vac / 3 A** **or** written expansion | Schematic + ratings | **GAP** |
| P6-07 | M.3.1 vs TR400 | Do **not** claim G6K/SMBJ33 = M modem class | Delta table | **PART** |

**Exit:** P6-03 PASS (software). P6-01/P6-06 PASS or **documented GAP** with hardware owner.

---

## Phase 7 — Evidence (O.14 / O.15)

**In scope:** Logger completeness, cyber evidence, legal pack.  
**Out of scope:** Buying the UCA/62351-100-3 certificate in this phase (launch **pack** only).

| Check | Regulation | Shall | Pass criteria | Status |
|-------|------------|-------|---------------|--------|
| P7-01 | O.14 | ≥ **2048** events, user cannot overwrite | Store + wrap test | **PART** — r27 `EventStore`, `--event-wrap-test` |
| P7-02 | O.14 | Timestamp `yyyy/mm/dd hh:mm:ss` | Sample dump | **PART** — `--event-dump` UTC |
| P7-03 | O.14 / 62351-14 | Remote read syslog RFC 5424 | SIEM receive | **OPEN** |
| P7-04 | O.14 list | Mandatory categories (DG/DI, comms, auth, DSO cmds, M trip, …) | Coverage matrix | **PART** — [P7-04 matrix](../../lab/evidence/phase7/P7-04_EVENT_COVERAGE_MATRIX.md) |
| P7-05 | O.15 / 62443-4-1 | SDLC evidence | Threat model + test records | **PART** |
| P7-06 | O.15 / 62443-4-2 | Component CR / SL-T 2 proposed | Gap vs CR 3.6 already in P1 | **PART** |
| P7-07 | O.15 / 62351-100-3 | Transport conformance **plan** (cert later) | Lab procedure + PICS | **OPEN** |
| P7-08 | O.15 / FIPS 140-2 L3 | Crypto module claim or **no-claim** | TPM evidence or drop claim | **PART** |
| P7-09 | O.15 / 61557-12 + 61010 | Product insulation/EMC path | Test list | **PART** |
| P7-10 | ARERA 385 / 564 | Scope ≥100 kW PF2 + dates | Legal memo | **PART** |
| P7-11 | K7.5 | DSO demo script | Client steps + expected `Wlim` | **PART** — [P7-11 script](../../lab/evidence/phase7/P7-11_DSO_DEMO_SCRIPT.md) |
| P7-12 | Programme | `--lab-demo` **off** in product image | Config audit | **OPEN** |

**Exit:** P7-01, P7-04, P7-11 PASS. Cert **lab booking** is after this pack — not a Phase 7 software exit.

---

## Cross-walk — clause → phase (do not reassign)

| Clause | Topic | Phase |
|--------|-------|-------|
| O.2 / V5 / 385 | When CCI + PF2 mandatory | P7 (legal); P3 (function) |
| O.7.3.1 TsP | Active settle ≤ 60 s | P3 (WSd/Wlim lab); P7 evidence |
| O.7.3.1 **TsQ** | Reactive settle ≤ **10 s** | **P5-R** |
| O.7.3.2 / O.7.3.3 | ΔT slow ring / SP spacing | **P5-R** |
| O.8 | PF1 observability | **P5-M** |
| O.8.2 | Smax / Qind / Qcap nameplate | P1 math · **P5-M09** expose |
| O.9.1.1 | cosφ set-point (PFSP) | **P5-R02** |
| O.9.1.2 | cosφ=f(P) (PFW) | **P5-R04** |
| O.9.1.3 | Q=f(V) (VArV) | **P5-R03** |
| O.9.1.4 | DSO Q set-point (VArSd) | **P5-R01** |
| O.9.2.1 | 110 % Un | P5-05 (needs V) |
| O.9.2.2 | DSO P limit Wlim | **P3** |
| O.9.2.3 | DSO P modulation WSd | **P3** (lab IMPL 2026-09-26) |
| O.9.2 mode (i) | Actuate units | **P1** (path), P3 (slave P) |
| O.10.3.2 | MSD Q set-point VArSa | **P5-R08** (discretionary) |
| O.9.3 / O.11 / M | Teletrip **absolute** priority | **P6** |
| O.11 indices 5–7 | Reactive vs active arbiter | **P5-R07** |
| O.12 | Installation / predisposition | P6 |
| O.13.1.1.1 | No bridge; Eth_A/B | **P2** |
| O.13.1.2 | Comms-loss fallback | P3 / P4 |
| O.13.2 | Accuracy | **P5-M** |
| O.13.5 | UTC ±100 ms | **P3** (chrony lab PASS) / P7 |
| O.14 | Data logger | P1 start → **P7** close (+ P5-R09) |
| O.15 | Tests / cert | **P7** |
| T.3.1.3 TotW | PdC active P 4 s | P3 lab · **P5-M** live |
| T.3.1.3 **TotVAr** | PdC reactive P 4 s | **P5-M07** |
| T.3.1.3 **PPV** | PdC voltages | **P5-M08** |
| T Tables 88–92 | Reactive LNs | **P5-R** |
| T (active P LNs) | Wlim/WSd | **P3** |
| 62351-3/4/8/9 | DSO cyber | **P3** |
| 62351-14 | Syslog | **P7** |
| 62443-3-2 | Zones | **P2** |
| 62443 CR 3.6 | Safe DO | **P1** |
| 60870-5-104 | Operator | **P4** |

---

## Related

| Doc | Role |
|-----|------|
| `lab/CCLI_PHASE_LAB_MASTER_LOG.md` | Tools · IPs · evidence index · close board |
| `lab/CCI_Reactive_Roadmap_Gap_Report.md` | RAG check: O.9.1 / TotVAr HAVE vs MISSING |
| `lab/CCI_TestSuitePro_Verification_Layer.md` | Triangle MicroWorks Test Suite Pro on Eth_A |
| `lab/evidence/.../SESSION_REPORT_P3_LAB_CLOSE_2026-09-25.md` | Phase 3 lab exit |
| `lab/PHASE1.md` | How to run P1 bench |
| `lab/PHASE1_TRACEABILITY.md` | P1 code ↔ clause (detail) |
| `apps/ccli/config/icd/signal_map.yaml` | LN hierarchy freeze + runtime_status |
| `CCI_CCLI_Product_Spec_and_Roadmap.md` | Weeks + REQ IDs |
| `CCI_Annex_OTM_Verification.md` | O/T/M PDF pages |

# CCLI — 3-Month Project Plan (Italy Only · No Fabrication)

**Document ID:** CCLI-PLAN-3M-001  
**Revision:** 1.0  
**Date:** 2026-07-29  
**RAG source_id:** `ccli-3m-italy-nofab-plan`  
**Companions:** `CCI_Project_Roadmap.md` · `CCI_Module_Regulations_Classification.md` · `CCI_Validation_and_Mocking_Strategy.md` · `CCI_Clone_RE_Option.pdf`

---

## 0. Planning constraints (locked)

| Constraint | Decision |
|------------|----------|
| **Duration** | **3 months** (≈ 13 weeks) from kick-off |
| **Market** | **Italy only** — CEI 0-16, ARERA, Italian DSO / Operatore Abilitato |
| **Fabrication** | **None** — no custom carrier PCB, no Gerbers to fab, no enclosure tooling |
| **Hardware in period** | Buy / use **COTS** only — SoM + communication box within budget caps below |
| **Budget — SoC / SoM** | **≤ €350** hard cap (compute module only) |
| **Budget — communication box** | **≤ €350** hard cap (eval carrier / comms board that exposes Eth + serial; not custom fab) |
| **Budget — HW subtotal** | **≤ €700** for SoM + communication box combined |
| **Design approach** | **Option A** original architecture (paper + SW). AiLux = behaviour reference only — **do not clone** firmware |
| **Certification in 3 months** | **Prepare + quote + evidence pack** — do **not** expect full 62443 / 61850 / DSO acceptance certificates in 13 weeks |
| **Out of market scope** | Non-IT DSO profiles, DNP3 / OPC-UA / C37.118 / S7 / DLMS parity, ATEX re-cert (already held at org level) |

### Hardware budget fit (Italy lab path)

| Line | Cap | Lead candidate | List / estimate | Fit? |
|------|-----|----------------|-----------------|------|
| **SoC (SoM)** | **€350** | ~~Toradex Verdin 0063~~ → **integrated in TG-524** | N/A (gateway) | **TG-524** is compute + comm box |
| **Communication box** | **€350** | **TesPro TG-524** (4G, OpenWrt) | Quote TBD | **FROZEN** — confirm ≤€350 |
| **Combined** | **€700** | Single **TG-524** gateway | TBD | One device replaces SoM + dev board |

**Rules**
1. Do **not** exceed **€350** on TG-524 quote — if over, renegotiate or use **Pi-only** lab until price fits.  
2. **Communication box** = **TesPro TG-524** (TG-500 series). Not AiLux CCI. Not custom fab.  
3. **TesPro TG544 / TR500** is the only on-target DUT (Pi path removed).  
4. Bench extras (USB–RS485, cables, relay module) are **outside** the €350 cap.  
4. Software licenses and lab certification quotes are **not** inside the €700 HW envelope.

**Honest outcome:** after 3 months you have an **Italy-ready engineering & certification launch package** + **kit/software demo**, not a field-certified shipped CCI.

---

## Requirements

| ID | Requirement | 3-month deliverable |
|----|-------------|---------------------|
| REQ-MKT-001 | Italy-only product definition | Market/regs scope freeze; non-IT protocols deferred |
| REQ-REG-001 | CEI 0-16 Allegato O/T traceability | Req ↔ function ↔ test matrix |
| REQ-REG-002 | ARERA 540/2021 obligations mapped | Gap list + actions (PDF ingest if obtainable) |
| REQ-HW-001 | No custom fab | Kit-only lab; carrier = **paper ADR** for later phase |
| REQ-BUD-001 | SoC ≤ €350 | PO / quote ≤ €350 for SoM |
| REQ-BUD-002 | Comm box ≤ €350 | PO / quote ≤ €350 for 99991105-class board |
| REQ-SW-001 | C/C++ control path skeleton | libiec61850 MMS stub + Modbus master stub + PF2 FSM stub |
| REQ-SW-002 | Stack license path | MZ commercial quote + build-vs-buy ADR |
| REQ-VAL-001 | L1 (and start L2) validation | pymodbus / protocol sims; kit Ethernet if kit arrives |
| REQ-SEC-001 | 62443-4-1 start | SDLC folder + certified vs tooling boundary |
| REQ-CERT-001 | Italy cert briefing pack | Quotes, bodies, cost envelope, apply checklist |
| REQ-DEM-001 | Investor / partner demo | Eth_A MMS + Modbus mock + simulated curtailment command |

---

## Architecture (3-month slice)

### Functional
- PF2 / observability logic (stub → demo-grade)
- IEC 61850 MMS server (Eth_A role) — Allegato T–aligned ICD draft
- Modbus RTU/TCP master (analyzer) — L1 on PC; kit RS-485 if available
- Eth_B / IEC 104 — **design + optional stub** (full stack optional if time)
- Commissioning console — outside certified path (Python/web OK)

### Hardware (no fab)
| Layer | 3-month use | Field product later |
|-------|-------------|---------------------|
| SoM + Dev Board 99991105 | Lab DUT | Not field CCI |
| Custom carrier C1–C10 | **Architecture docs + interface/BOM matrices only** | Fab after M3 |
| DI/DO / isolated RS485 / GNSS | Mock on PC or USB-GPIO/RS485 dongle | Carrier |

### Network (Italy)
| Port role | Protocol focus (3 months) |
|-----------|---------------------------|
| Eth_A → DSO | IEC 61850 MMS (+ GOOSE stretch goal) |
| Eth_B → Operatore Abilitato | IEC 60870-5-104 design; stub if capacity |
| Plant | Modbus TCP optional |
| Serial | Modbus RTU (sim first) |

### Security
- Document certified-path boundary
- Start 62443-4-1 artefacts (reuse ATEX QMS habits)
- Secure boot on kit = research spike only (no production keys)
- IEC 62351 = requirements map; TLS demo if stack allows

### Power
- Kit PSU only; C10 power tree remains **paper** (no PCB)

---

## Interface matrix (lab / demo)

| Interface | Source | Destination | Protocol / means |
|-----------|--------|-------------|------------------|
| Eth_A | libiec61850 server | PC client / IEDScout-class | IEC 61850 MMS |
| Eth_B | stub or lib60870 | PC IEC 104 client | IEC 60870-5-104 |
| Modbus | C++/PC master | pymodbus simulator | Modbus TCP (L1) / RTU via USB-RS485 |
| DO curtailment | PF2 FSM | GPIO or relay USB module | Dry contact sim |
| DI status | Test box / sim | PF2 FSM | Digital / mock |
| Console | Engineer laptop | Tooling service | USB / HTTP (out of cert path) |

---

## BOM matrix (buy in 3 months — no fab)

| Block | Function | Candidate | Cap / est. | Status |
|-------|----------|-----------|------------|--------|
| LAB-GW | Lab DUT (frozen) | **TesPro TG-524** | **≤ €350** (quote) | **BUY** — OpenWrt comm gateway |
| LAB-DUT | On-target validation | **TesPro TR400** | In gateway budget | **ON BENCH** |
| LAB-MOCK | — | — | — | **REMOVED** |
| LAB-REF | Reference only | Historical SoM study docs | — | **NOT** lab DUT |
| LAB-ETH | Extra ports / tap | Unmanaged switch / TAP | Outside €700 | Optional |
| LAB-RS485 | Serial HIL | USB–RS485 adapter(s) | Outside €700 | Wave A/B |
| LAB-SW | Protocol stacks | libiec61850 (+ lib60870) OSS for lab | €0 lab | Free / GPLv3 lab |
| LAB-SIM | L1 mocks | pymodbus, 61850 client tools | €0–low | Free / license as needed |
| SW-LIC | Ship path | MZ commercial quote | Not in €700 | **QUOTE** (not must-buy in M3) |
| DOCS | Regs PDFs | ARERA 540/2021, CEI extracts | Separate | Download missing |
| CERT | Lab scoping | 62443 + 61850 Italian/EU labs | Separate | **QUOTE** |

**Do not buy:** custom PCB fab, enclosure tooling, Toradex kit as lab DUT (reference only), any gateway that breaks the **€350** comm-box cap without re-baselining budget.

---

## 13-week plan

### Month 1 — Freeze Italy scope + start evidence (Weeks 1–4)

| Week | Focus | Exit checkpoint |
|------|-------|-----------------|
| **1** | Kick-off; lock constraints; Italy regs inventory from AiLux baseline; Architecture Freeze **lite** (no fab) | Signed scope: Italy / no fab / kit-only |
| **2** | CEI O/T ↔ function traceability; ARERA map; ingest missing regs if available; SW ADR (MZ vs SISCO/TMW) | Traceability v0.9; quote emails sent |
| **3** | Order kit + Wave A bench SW; L1 pymodbus scenarios; PF2 requirements (hysteresis, timeout, safe state) | Kit ordered; L1 runner skeleton |
| **4** | ICD/SCL outline (Allegato T); certified-path vs tooling ADR; 62443-4-1 folder skeleton | Architecture Freeze gate (paper) |

**Month 1 deliverables**
- `Architecture/` matrices updated for Italy-only deferred list  
- Certification briefing pack draft (bodies, costs, apply steps)  
- Lab + MZ scoping quotes in flight  

### Month 2 — Software on kit / PC (Weeks 5–8)

| Week | Focus | Exit checkpoint |
|------|-------|-----------------|
| **5** | BSP boot on kit (or PC Linux fallback); repo `ccli-app` skeleton + CMake | Boots + hello world on target |
| **6** | libiec61850 MMS server example → CCI data model stub | Client reads demo LN/DO |
| **7** | Modbus master + L1 vectors; PF2 FSM stub driven by meter + simulated DSO command | Curtailment stub toggles DO sim |
| **8** | Eth_A/Eth_B logical separation doc + firewall/RBAC notes; optional 104 stub | Mid-demo dry run |

**Month 2 deliverables**
- Working **demo path**: MMS + Modbus mock + PF2 stub  
- SDLC evidence pack started (req, reviews, issue log)  
- Knowledge: GOOSE = stretch only if Week 6–7 green  

### Month 3 — Italy demo + cert launch pack (Weeks 9–13)

| Week | Focus | Exit checkpoint |
|------|-------|-----------------|
| **9** | Harden demo; test report templates; fill Verification matrix for REQ-* | Evidence folder convention live |
| **10** | Italy GTM: DSO engagement checklist; partner/pilot narrative; AiLux vs HiTEKS capability gap table | One-pager for sales/pilot |
| **11** | Consolidate cert quotes; select preferred 62443 / 61850 labs; calendar Phase 4+ | Signed preferred-lab choice |
| **12** | Carrier **paper** freeze for post-M3 fab (interfaces, power, isolation) — still **no fab** | Carrier ADR ready for later NRE |
| **13** | Gate review: demo + docs + budget + next 6–9 month cert roadmap | **M3 Exit Review** |

**Month 3 deliverables**
- Demo-ready kit/PC system (Italy narrative)  
- Certification apply pack + cost envelope confirmed by quotes  
- Post-3-month roadmap (fab + accredited tests) frozen  

---

## Workstreams (parallel)

```text
WS-A  Italy regulations & CEI/ARERA traceability
WS-B  Software (libiec61850, Modbus, PF2) — kit or PC
WS-C  Validation L1→L2 (no custom carrier)
WS-D  Certification prep (quotes, 62443-4-1, briefing pack)
WS-E  Commercial (MZ license, Italy GTM, pilot story)
WS-F  Paper hardware architecture (carrier later — no fab)
```

Suggested staffing (minimum): **1 firmware**, **1 systems/architect** (can share), **0.2 commercial/legal** for quotes & IP.

---

## Italy certification in this plan (what we do vs defer)

| Track | In 3 months | After M3 |
|-------|-------------|----------|
| CEI 0-16 O/T | Traceability + demo evidence | DSO acceptance demo |
| ARERA 540/2021 | Map obligations | Close gaps with counsel/DSO |
| IEC 61850 | Lab stack + ICD draft + quote conformance | Paid conformance |
| IEC 62443-4-1 | SDLC start | Audit |
| IEC 62443-4-2 | Scope + quote | Lab test |
| IEC 62351 | Requirements map | With cyber/protocol lab |
| CE / EMC / RED | Note for later EU product | After carrier exists |
| CRA | Awareness in technical file outline | Full conformity later |
| ATEX | Reuse org process; confirm CCI usually non-Ex | Only if Ex install |

**Italy-only protocol priority:** 61850 + Modbus + (104). Defer DNP3, OPC-UA, C37.118, S7, DLMS.

---

## Budget envelope (3 months, no fab)

### Hard HW caps (locked)

| Line | Cap EUR | Allowed spend |
|------|---------|---------------|
| **SoC (SoM)** | **350** | One industrial SoM — prefer 0063 if ≤€350 |
| **Communication box** | **350** | One COTS carrier/eval board — prefer 99991105 (~€300) |
| **HW total (these two)** | **700** | Do not combine into one PO that exceeds either line |

### Other costs (not inside €700)

| Item | Rough EUR | Notes |
|------|-----------|-------|
| Bench adapters / cables / small relays | 0–200 | Prefer PC L1 if over budget |
| SW tools (optional commercial clients) | 0–2,000 | Prefer OSS first |
| Lab **scoping** (often free / low) | 0–2,000 | Get written quotes |
| Standards PDFs / CEI purchases | 500–2,000 | As needed |
| MZ / stack **quotes** | 0 now | Purchase may fall **after** M3 |
| Labour (internal) | Dominant cost | Plan FTEs explicitly |
| **Custom PCB fab** | **€0** | Locked out |
| **Accredited cert execution** | **€0 in M3** | Budget reserved post-M3 (€45–110k cyber/grid core per roadmap) |

---

## Knowledge gaps

| Area | Current | Required by M3 | Gap |
|------|---------|----------------|-----|
| Custom carrier | Specs only | Paper ADR complete | Fab deferred OK |
| ARERA 540/2021 PDF | MISSING | Ingested + mapped | Download |
| Analyzer Modbus map | MISSING | Pick 1 meter model or stay on sim | Optional real meter |
| MZ commercial quote | MISSING | Written quote | Email MZ |
| Lab quotes 62443/61850 | MISSING | ≥1 written scope | Email labs |
| Application repo | NOT STARTED | Skeleton + demo | Create |
| GOOSE / 104 full | Partial ambition | Stretch | Timebox |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-3M-001 | Expecting full certification in 13 weeks | Schedule failure | Gate: prep only; cert execution post-M3 |
| R-3M-002 | Kit lead time slips SW | Demo delay | PC-first L1; kit parallel |
| R-3M-007 | SoM quote > €350 | Blocks preferred Plus path | Fall back to Mini IT or distributor deal; keep Eth_A/B ADR honest |
| R-3M-008 | Comm box + shipping/VAT pushes > €350 | Over budget | Buy 99991105 only if landed cost ≤€350; else cheaper Verdin carrier SKU |
| R-3M-003 | Scope creep (fab / full AiLux protocols) | Overrun | Change control; Italy-only list |
| R-3M-004 | GPLv3 ship assumption | Legal | Lab OSS only; commercial quote before product |
| R-3M-005 | Kit ≠ field I/O | False confidence | Document kit limitations; paper carrier ADR |
| R-3M-006 | No DSO contact | Weak GTM | Week 10 checklist + one outreach |

---

## Verification (3-month exit criteria)

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-MKT-001 | Scope review | Written Italy-only / no-fab decision signed |
| REQ-REG-001 | Traceability audit | Every Annex O/T CCI function mapped to REQ + demo or deferred-with-date |
| REQ-SW-001 | Demo script | MMS read OK; Modbus poll OK; PF2 stub issues curtailment command |
| REQ-VAL-001 | L1 report | ≥1 normal + ≥1 fault (timeout/stale) vector recorded |
| REQ-SEC-001 | Folder review | Certified vs tooling boundary + SDLC skeleton present |
| REQ-CERT-001 | Pack review | Cost table + apply steps + ≥1 lab quote or documented chase |
| REQ-DEM-001 | Live demo | 15-minute Italy narrative demo on kit or PC |
| REQ-HW-001 | BOM check | Zero fab POs; carrier only in docs |
| REQ-BUD-001 | Invoice / quote | SoM landed cost **≤ €350** |
| REQ-BUD-002 | Invoice / quote | Communication box landed cost **≤ €350** |

**Explicit non-goals (fail if treated as M3 must-haves):** custom PCB, field MT install, full 62443 certificate, full 61850 UCA certificate, AiLux firmware modification, HW spend above €350+€350.

---

## Post–Month 3 (next phase preview)

1. Carrier NRE + fab (first time fabrication allowed)  
2. Execute 62443 / 61850 lab bookings  
3. Commercial stack license purchase  
4. Italian pilot / DSO demo campaign  
5. CE technical file when product HW exists  

---

## Decision log (fill at kick-off)

| Decision | Owner | Date | Outcome |
|----------|-------|------|---------|
| Confirm no fab for 3 months | | | |
| Confirm Italy-only market | | | |
| Confirm **€350 SoC + €350 comm box** caps | | | **LOCKED** |
| SoM SKU if 0063 quote > €350 (Mini fallback?) | | | |
| Kit buy vs already owned | | | |
| PC-first vs wait-for-kit | | | |
| Eth_B / 104 in M3: stub vs full | | | |
| Preferred stack vendor chase list | | | |

---

## Keywords

`3-month plan`, `Italy only`, `no fabrication`, `budget 350`, `SoC`, `communication box`, `CEI 0-16`, `ARERA`, `Toradex kit`, `libiec61850`, `PF2`, `certification prep`, `ccli-3m-italy-nofab-plan`

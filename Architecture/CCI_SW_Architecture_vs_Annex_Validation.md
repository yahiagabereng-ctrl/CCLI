# CCLI — SW Architecture vs CEI Annex validation

**Document ID:** CCLI-ARCH-SW-ANNEX-VAL-001  
**Revision:** 1.0  
**Date:** 2026-09-30  
**Methodology:** `knowledge-base/08-engineering/CCI_Architecture_Framework.md`  
**SW Architecture:** `Architecture/Software_Architecture.md` · `Software_Architecture.drawio`  
**Runtime SoT:** `Architecture/CCI_Runtime_Dataflow_Architecture.md`  
**Phase gates:** `knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md`  
**Annex extracts:** `CCI_Annex_O_Extract.md` · `CCI_Annex_T_Extract.md` · `CCI_Annex_M_Extract.md`  
**Corpus:** Annex extracts **HAVE** · SW Architecture.md rev 1.0 (2026-09-18) **STALE** on MMS/104 colours vs lab

This matrix freezes **Annex clause → SW layer/path → phase status → evidence**. Use it for design reviews; do **not** use draw.io green alone as CEI compliance.

---

## Requirements

| ID | Requirement |
|----|-------------|
| REQ-VAL-SW-001 | Every Annex O / T / M cluster maps to a **layer + path** or an explicit GAP/phase |
| REQ-VAL-SW-002 | Layer rule holds: core never calls adapters; `ccli_main` / `svc_*` wire edges |
| REQ-VAL-SW-003 | Gaps are explicit — no “IMPL/green” for missing annex functions |
| REQ-VAL-SW-004 | SW Architecture draw.io colours reconcile with Runtime DF + Phase checklist within one rev |

---

## Architecture

### Functional — Annex → SW layer

| Annex cluster | Shall | SW layer / component | Arch colour (draw.io 1.0) | Validation |
|---------------|-------|----------------------|---------------------------|------------|
| **O.8 PF1** observability (P/Q/V @ PdC) | Measure + publish | L1 `MeasurementStore` · L2 Modbus · L2 MMS TotW | Green P / Amber Q,V | **PART** — TotW **IMPL**; TotVAr/PPV **P5 OPEN** (T.3.1.3) |
| **O.9.2** mode (i) local limit | P → curtail | L1 `Pf2Fsm` · L3 `svc_pf2` · L0 HAL DIO1 | Green (Phase 1) | **FIT** path — eng. evidence vs P1-0x may be PASS/PEND |
| **O.9.2.2 / O.9.2.3** DSO Wlim/WSd | Eth_A setpoints → limit | L2 `MmsAdapter` · L1 `dso_phase1` → `Pf2Fsm` | Not on SW Arch page 1 (2026-09-18) | **FIT** in Runtime DF · **SW Arch outdated** |
| **O.9.2.1** ~110 % V | Autonomous V limit | — | Red / missing | **GAP** (P5) |
| **O.9.1** Q(V), cosφ, VArSd… | Reactive FSMs + plant Q | Reactive LNs STUB | Red | **GAP** (P5-R) — plant Q path **MISSING** |
| **O.11** priority | Arbiter among FRs | `dso_active_power` O.11 **min** only | Amber | **PART** — not full Tab. O.11 |
| **O.13** stale / safe | No stale-as-valid | `Pf2Fsm` + quality | Green | **FIT** design — verify P1-04 |
| **O.13.1.1.1** IF isolation | No bridge Eth_A↔plant | L1 `net_policy` + OpenWrt zones | Grey/target on SW Arch | Phase checklist **P2 CLOSED** — use **current** isolation evidence as SoT |
| **O.14** logger | ≥2048 events | L1 `EventRing` | Amber | **PART** |
| **T MMS** Type 3 / CID | Server + reports + SBO | L2 `iec61850_mms` + libiec61850 | Red→stub in old SW Arch | **IMPL** lab P3 (**update draw.io**) |
| **T cyber** 62351 | TLS / RBAC | MMS adapter + `config/tls` | — | **PART/IMPL** lab |
| **104 Eth_B** | Operator path | L2 `iec60870_104` | Red stub in SW Arch | Checklist **P4 CLOSED** lab — **reconcile** draw.io |
| **Annex M** telescatto | Inhibit conflicting PF2 | DIO3 target | Red | **OPEN** P6 |
| **PF3 / MSD** O.10 | WSa / market | — | Red | **OUT** / later |

### Hardware / Network / Security / Power (SW view)

| View | Annex need | SW Architecture coverage | Verdict |
|------|------------|--------------------------|---------|
| Hardware | DIO / RS485 / Eth | L0 `platform_tg500` only; HW in Functional/TR400 draw.io | **OK split** |
| Network | Eth_A/B domains | Phase 2 / `net_policy`; not full L0–L4 | Validate via Network draw.io + P2 |
| Security | 62351, CR 3.6 | Safe DO in Phase 1; MMS TLS in Runtime DF | Cite Runtime DF until SW Arch rev |
| Power | O.7 vs PSU | Out of SW Arch scope | **N/A** to SW layers |

**Dependency rule:** upper → lower; core never calls adapters — **still valid**.

### Layer → path cheat sheet

| Layer | CMake | Key paths |
|-------|-------|-----------|
| L4 | `ccli` | `apps/ccli/services/ccli_main.cpp` |
| L3 | `ccli_services` | `svc_modbus` · `svc_pf2` · `svc_io` · `svc_mms` |
| L2 | `ccli_adapters` | `modbus_master/` · `iec61850_mms/` · `iec60870_104/` · `io/drv_gpio.*` |
| L1 | `ccli_core` | `pf2/` · `dso/` · `measurement/` · `event/` · `policy/` · `config/` |
| L0 | `ccli_platform` | `platform_tg500/hal_gpio_ll.c` |

---

## Interface matrix

| Interface | Source | Destination | Protocol | Annex | SW path |
|-----------|--------|-------------|----------|-------|---------|
| Meter P(/Q) | Field / PC slave | `MeasurementStore` | Modbus RTU | O.8 / O.13 | `modbus_adapter` → store |
| DSO setpoints | DSO / TSP | `Pf2Fsm` thresholds | IEC 61850 MMS | O.9.2.2/3 · T | `mms_adapter` → `dso_phase1` |
| PdC TotW | Store | DSO client | MMS report ~4 s | T.3.1.3 · O.8 | `mms_adapter` update |
| Curtail | `Pf2Fsm` | Plant | DIO1 / `dido_v2` | O.9.2 (i) | `svc_io` → `hal_gpio_ll` |
| Permissive | Field DI | Actuation gate | DIO2 | **Lab only** (not O) | `svc_io` |
| Operator | OA | CCI | IEC 104 | O.13 Eth_B | `iec104_adapter` |
| Defence | Modem/SPI | CCI inhibit | DI (target IO3) | M · O.11 | **not in SW loop yet** |

---

## BOM matrix

| Block | Function | Candidate / path | Status vs Annex |
|-------|----------|------------------|-----------------|
| L4 | Orchestrator | `services/ccli_main.cpp` | **HAVE** |
| L3 | Services | `svc_{modbus,pf2,io,mms}` | **HAVE** |
| L2 MMS | Annex T | `adapters/iec61850_mms/` + libiec61850 | **HAVE** (draw.io colour wrong) |
| L2 Modbus | Meter | `modbus_master/` | **HAVE** |
| L2 104 | Eth_B | `iec60870_104/` | **HAVE/lab** — Arch colour stale |
| L1 PF2 | O.9.2 | `core/pf2/pf2_fsm.*` | **HAVE** partial PF2 |
| L1 DSO | O.9.2.2/3 · O.11 min | `core/dso/*` | **HAVE** |
| L1 Store | O.8 | `measurement_store.*` | **HAVE** |
| L0 HAL | DIO | `platform_tg500/hal_gpio_ll.c` | **HAVE** lab class |
| Reactive core | O.9.1 | — | **MISSING** |
| O.11 arbiter full | Priority table | — | **MISSING** |
| Annex M hook | Teletrip | — | **MISSING** |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| SW Architecture.md / draw.io | Phase 1 + “MMS/104 stubs” | Reflect P2–P4 closed lab + MMS IMPL | **Doc drift** |
| TotVAr / PPV / O.9.1 | Deferred / STUB | T.3.1.3 + O.9.1 | **P5** |
| Full O.11 | Min active-P only | Full priority table | **GAP** |
| Allegato M in SW | Table allocation only | DI inhibit in main / FSM | **P6** |
| Product DI class | 4× TG DIO | REQ-IO-004 10–120 V | **HW GAP** |
| Evidence packs | Split Runtime DF / Phase / lab/evidence | Link P2 isolation + P3 MMS into this matrix | Ongoing |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-SWA-01 | Treat SW Arch green as full Annex O PF2 | False cert claim | Use this matrix + Phase checklist |
| R-SWA-02 | Draw.io still shows MMS/104 as red stubs | Wrong design reviews | Revise page 1/4 colours |
| R-SWA-03 | Validate only L1 PF2, skip T/M | Incomplete CCI | Gate reviews on full table |
| R-SWA-04 | Count permissive DI as Annex O | Audit noise | Label **lab-only** |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-VAL-SW-001 | Walk O.6–O.15 / T.3 / M against L0–L4 | Every shall → layer or GAP/phase |
| O.9.2 local | P1 bench / unit FSM | DIO1 + stale-safe |
| O.9.2.2/3 + T | TSP Operate Wlim/WSd on Eth_A | `mms→pf2` + DO policy |
| O.8 / T.3.1.3 | Report TotW (+ TotVAr when P5) | ~4 s integrity |
| O.13.1 | P2 isolation suite | No Eth_A↔Eth_B↔plant bridge |
| O.9.1 / M | P5 / P6 gates | Or written WAIVE/GAP |
| Doc sync | Diff SW Arch vs Runtime DF | No “stub” for IMPL components |

### Verdict

| Verdict | Meaning |
|---------|---------|
| **Layer model FIT** | L0–L4 + services wiring is valid for Annex-facing CCI |
| **Annex coverage PART** | Strong: O.9.2 actuation + T active-P MMS + O.13 stale · Weak/open: O.9.1, full O.11, M, full O.14/O.15, full PF1 metrology |
| **SW Architecture artefact STALE** | Do not use draw.io colours alone for annex validation |

---

## Clause → path → phase (traceability freeze)

| Clause | SW path | Phase | Status |
|--------|---------|-------|--------|
| O.9.2 mode (i) | `core/pf2/pf2_fsm.*` → `svc_io` → `hal_gpio_ll.c` | P1 | **FIT** |
| O.9.2.2 Wlim | `mms_adapter` → `dso_phase1` → PF2 | P3 | **IMPL** lab |
| O.9.2.3 WSd | same | P3 | **IMPL** lab |
| O.9.2.1 110 % V | — | P5 | **GAP** |
| O.9.1.* reactive | CID STUB · no core FSM | P5-R | **GAP** |
| O.8 / T.3.1.3 TotW | Modbus → store → MMS | P3/P5 | **IMPL** TotW |
| T.3.1.3 TotVAr/PPV | store + MMS | P5-M | **OPEN** |
| O.11 full | `dso_active_power` min only | P5/P6 | **PART** |
| O.13 stale | `pf2_fsm` quality/stale | P1 | **FIT** |
| O.13.1.1.1 | zones + `net_policy` | P2 | Checklist **CLOSED** |
| O.14 | `event_ring` | P1→P7 | **PART** |
| T MMS + 62351 | `adapters/iec61850_mms/` | P3 | **IMPL** lab |
| Eth_B 104 | `adapters/iec60870_104/` | P4 | Checklist **CLOSED** lab |
| Annex M | DIO3 target | P6 | **OPEN** |
| O.10 PF3 | — | later | **OUT** |

---

## Related artefacts

| File | Role |
|------|------|
| `Architecture/Software_Architecture.md` | Layer guide (needs colour rev) |
| `Architecture/CCI_Runtime_Dataflow_Architecture.md` | As-built data path |
| `lab/PHASE1_TRACEABILITY.md` | Clause → file → test (Phase 1) |
| `lab/evidence/O13_1_isolation_ssh_*.log` | Historical bridge evidence (pre/post P2) |
| `lab/evidence/PF2_DIO_mapping_ssh_*.log` | IO1–IO4 vs O.9.2 / M / XCBR |

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-30 | Initial Annex ↔ L0–L4 validation matrix |

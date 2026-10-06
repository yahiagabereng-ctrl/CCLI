# CCLI lab evidence — master index

**Programme:** HiTEKS CCLI · TesPro TG544 (`platform_tg500`)  
**Last consolidated:** 2026-10-06  
**Lab PC:** `192.168.10.10` · **DUT:** `192.168.10.1`  
**Product build (lab gate):** `0.1.0-r34` (P7-ZONE-DASHBOARD)

This folder is the **authoritative lab evidence chain**. It is **not** O.15 certification (Phase 7).

---

## Phase close register (lab)

| Phase | Gate | Status | Closeout / index |
|-------|------|--------|------------------|
| **0** | Platform bind | **CLOSED** 2026-09-25 | `lab/CCLI_PHASE_LAB_MASTER_LOG.md` |
| **1** | Local PF2 | **CLOSED** (eng.) 2026-09-22 | `lab/PHASE1_TRACEABILITY.md` |
| **2** | Isolation O.13.1 | **CLOSED** 2026-09-25 | `lab/evidence/O13_1_isolation_2026-09-25/` |
| **3** | DSO MMS TLS | **CLOSED** (lab) 2026-09-25 | `lab/evidence/testsuite-pro/inbox/TSP_P3_07_*` |
| **4** | Operator 104 + TesPro 61850 | **CLOSED** (lab) 2026-09-30 | `lab/evidence/phase4/` |
| **5** | Observability + reactive (lab) | **CLOSED** (lab) 2026-10-01 · **P5-R CLOSED** 2026-10-05 (r31) | **[phase5/P5_FINAL_CLOSEOUT.md](phase5/P5_FINAL_CLOSEOUT.md)** · [P5_REACTIVE_CLOSEOUT_2026-10-05.md](phase5/P5_REACTIVE_CLOSEOUT_2026-10-05.md) |
| **6** | Defence I/O Annex M | **CLOSED** (lab) 2026-10-02 | **[phase6/P6_FINAL_CLOSEOUT.md](phase6/P6_FINAL_CLOSEOUT.md)** |
| **7** | Evidence pack / cert | **OPEN (kickoff)** | **[phase7/P7_README.md](phase7/P7_README.md)** — not accredited cert |

**Phase 5 residual (product, non-blocking lab):** REQ-MET-002 grid alignment · LN-GAP-08 · REQ-CTL-004 · P5-R05/R07 · P5-G01/P5-G03 runtime · P5-04 accuracy · P5-06 meter map.

**Product operator HMI:** **Cloud UI** — [CCI_Cloud_Operator_HMI.md](../../knowledge-base/08-engineering/CCI_Cloud_Operator_HMI.md). LAN2 = 104 SCADA wire only; wire-day pack when LAN1 exit done.

---

## Folder map

| Path | Contents |
|------|----------|
| **[phase5/](phase5/)** | P5 full-circle gate — annex sequence, LN matrix, northbound, GNSS, closeout |
| **Chronos EMT432** | P5-06 meter candidate — [CCI_Chronos_EMT432_Extract.md](../knowledge-base/08-engineering/CCI_Chronos_EMT432_Extract.md) · [P5_EMT432_BENCH_PLAN.md](phase5/P5_EMT432_BENCH_PLAN.md) |
| **[phase6/](phase6/)** | P6 Annex M — SMS trip, O.11 inhibit, voltage-class gaps |
| **[phase7/](phase7/)** | P7 O.14 event store, coverage matrix, K7.5 DSO demo |
| **[phase4/](phase4/)** | P4 Eth_B 104 TLS, operator role, TesPro supplier SW, **[LAN2_READINESS.md](phase4/LAN2_READINESS.md)** |
| **[testsuite-pro/](testsuite-pro/)** | Triangle MicroWorks TSP exports (historical + Compare waivers) |
| **[O13_1_isolation_2026-09-25/](O13_1_isolation_2026-09-25/)** | P2/P3 isolation session reports |
| **[field/regalgrid_snocu/](field/regalgrid_snocu/)** | Field competitor ingest (reference only) |

---

## Verification clients (DSO MMS lab)

| Tool | Role | Status 2026-10-01 |
|------|------|-------------------|
| **Test Suite Pro** v4.7.4.5037 | Primary DSO client (Compare, Report Viewer, Sequencer) | **BLOCKED** — expired Sentinel trial; see [phase5/P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md](phase5/P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md) |
| **[IEDExplorer](https://sourceforge.net/projects/iedexplorer/)** | Alternate DSO MMS client (connect, read, Operate, URCB) | **ACTIVE** for re-runs |
| **`mms_lab_client` / scripts** | Automated cleartext chain | **PASS** — `P5_FULLCIRCLE_CLEARTEXT_2026-10-01_111628.txt` |
| **TesPro `iec61850d` + MQTT** | Northbound collector (not DSO path) | **PASS** — [phase5/P5_TESPRO_NORTHBOUND_2026-10-01.md](phase5/P5_TESPRO_NORTHBOUND_2026-10-01.md) |

Historical TSP evidence under `testsuite-pro/inbox/` remains valid for gates closed before license expiry.

---

## Quick links — Phase 5

| Doc | Purpose |
|-----|---------|
| [P5_README.md](phase5/P5_README.md) | Phase 5 index + requirement summary |
| [P5_ANNEX_TEST_SEQUENCE.md](phase5/P5_ANNEX_TEST_SEQUENCE.md) | Annex O/T → test IDs |
| [P5_TSP_FULLCIRCLE_SEQUENCER.md](phase5/P5_TSP_FULLCIRCLE_SEQUENCER.md) | Operator checklist (TSP or IEDExplorer) |
| [P5_FINAL_CLOSEOUT.md](phase5/P5_FINAL_CLOSEOUT.md) | Gate sign-off |
| [P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md](phase5/P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md) | TSP license + IEDExplorer fallback |

---

## Corpus & methodology

- Phase checklists: `knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md`
- Annex validation matrix: `Architecture/CCI_SW_Architecture_vs_Annex_Validation.md`
- Lab master log: `lab/CCLI_PHASE_LAB_MASTER_LOG.md`
- TSP procedure: `lab/CCI_TestSuitePro_Verification_Layer.md`

---

## Ingest workflow

1. Drop raw exports in `testsuite-pro/inbox/` (naming per [testsuite-pro/README.md](testsuite-pro/README.md)).
2. Session reports → phase folder with `P5_*` / `P4_*` prefix and date stamp.
3. Update closeout + this index when a gate is re-signed.

```powershell
pwsh scripts/ingest-testsuite-pro.ps1
```

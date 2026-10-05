# Annex equation utilization matrix — RAG · draw.io · code

**Document ID:** CCLI-FLOW-EQ-UTIL-001  
**Revision:** 1.0 (2026-10-03, ccli r29)  
**Parent:** `Flowcharts/README.md` · `Flowcharts/FLOWCHARTS_CROSSCHECK_AUDIT.md`  
**Purpose:** one row per Annex O / T / TR 57-126 equation or timing rule, answering four questions:
is it in the **RAG corpus**, is it in a **draw.io** flowchart, which **code symbol** implements it, and is it **utilized at runtime**.

Companion single plane: `apps/ccli/config/icd/signal_map.yaml` (`equations:` index + `equation` / `drawio_ref` / `regulation_id` per LN).

---

## 1. Verdict legend

| Verdict | Meaning |
|---------|---------|
| **USED** | Equation executed in the product path (`ccli` runtime) and covered by unit test or bench row |
| **USED-LAB** | Executed in `ccli` but only via lab tooling / mock (not accredited path) |
| **PART** | Partially executed — see "Gap" column |
| **TEST-ONLY** | Helper exists and is unit-tested, but not invoked by the runtime loop |
| **DOC** | Present in RAG and/or draw.io only — no `ccli` code |

---

## 2. Matrix

| Eq | Clause | Rule / formula | RAG source | draw.io | `regulation_map` | Code symbol | Verdict | Gap / backlog |
|----|--------|----------------|-----------|---------|------------------|-------------|---------|---------------|
| **(1)** | O.8.2 | \(S_{\max}=\sqrt{\max(P_{imm}^2,P_{ass}^2)+\max(Q_{ind}^2,Q_{cap}^2)}\) | PARAMETERS §2 · Figura2 §Table 1 · Annex O extract | — (parameter box in Wlim/WSd charts) | R01 | `plant_envelope.hpp` `calc_smax_kva()`, `smax_kva_effective()` | **USED** | — |
| **(2)** | O.9.2.x | \(P_{kW}=\frac{pct}{100}S_{\max}\) | PARAMETERS §3 | `DSO_P_limit O_9_2_2` · `DSO_P_modulation_O_9_2_3_` · `Lim W110 O.9.2.1` | R02 R03 R04 | `plant_envelope.hpp` `pct_smax_to_kw()` | **USED** | — |
| **(3)** | O.9.1.4 / O.10.3.2 | \(Q_{kVAr}=\frac{VArSptPct}{100}S_{\max}\) (signed) | PARAMETERS §10 · Annex T Tab. 88/89 | `V.ctrl.Q.on.DSO.NotConfirmed` | R13 | `dso_reactive_power.cpp` `derive_reactive_kvar()` → `modbus_adapter.write_reactive_kvar` | **USED-LAB** | P5-R01 lab Modbus FC16 @40003; Sun2000 register TBD; TsQ not timed |
| **(4)** | O.9.2.3 | \(WSptPct\in[-100,+100]\), − = import | PARAMETERS §3 | `DSO_P_modulation_O_9_2_3_` | R03 | `derive_active_power_kw()` uses `max(0,pct)` | **PART** | Negative (import) set-point ignored — P1 backlog |
| **(5)(6)** | O.11 | \(P_{eff}=\min(\text{active export caps})\); priority W110 → Wlim → WSd | PARAMETERS §3 · Annex O Table 1 (`CCI_Annex_O_Extract.md` §8) | all three P charts (`P_bounded = min(...)`) | R05 | `dso_active_power.cpp` `derive_active_power_kw()` | **USED** | Only active-P tier; WSa (index 4) and reactive tiers (5–7) not arbitrated — P3 backlog |
| **(7)(8)** | O.7.3.1 | \(\lvert P-P_{exp}\rvert/\lvert P_{exp}\rvert\le5\%\) within \(T_{sP}\le60\,s\); \(T_{sQ}\le10\,s\) | PARAMETERS §4 §10 | Q(V) fast ring (±5 % Q) | R06 | `settled_within_band_kw()` (`dso_phase1_test`) · live bench `LIVE-CONF-R06` band at meter | **TEST-ONLY / PART** | No runtime settling timer; no TsQ helper — **P0** backlog |
| **(9)** | O.7.3.3 | \(\Delta t=t_{now}-t_{last}\ge3\,s\) else reject | Annex O extract §3 · PARAMETERS §4 | `DSO_P_limit O_9_2_2` ("DSO command validation" node) | R07 | `core/dso/setpoint_gate.hpp` `SetpointSpacingGate` → `mms_adapter.cpp` `setpoint_spacing_accept()` on `WMaxSptPct` / `WSptPct` / `VArTgtSptPct` · yaml `mms.setpoint_min_interval_s` (default 3) · O.14 event `mms/setpoint_reject_spacing_o733` · test `setpoint_gate_test` | **USED** (r29) | DUT evidence pending (write two set-points < 3 s apart from TSP → second rejected) |
| **(10)** | O.9.1.3 | Q(V): zones V2i/V1i/V1s/V2s, \(Q_{tgt}=K\,Q_{\max}\), lock-in 0.20 Pn / lock-out 0.05 Pn, δQ 5 % Qmax, slow ring ΔT 10–600 s (O.7.3.2) | PARAMETERS §10 · Audit §5 | `Q(V).not confirmed` | R10 | — (LNs `VArVDVVR1`, `VArVDPMC1/2`, `VArVDECP1/2` present, Mod=5) | **DOC** | P2 backlog — needs PdC V input + ΔT scheduler |
| **(11)** | O.9.1.2 | cosφ(P): points A/B/C (0.20/0.50/0.80 Pn → 1.00/1.00/0.95), δcosφ 0.02, \(Q=P\tan(\arccos\cos\varphi)\), VLkIn 1.05 Vn | PARAMETERS §10 · Audit §5 | `Cosfi(P).not confirmed` | R11 | — (`PFWDPFW1` WSetA..C / PFSetA..C / VLkIn / VLkOut in CID, Mod=5) | **DOC** | P2 backlog |
| **(12)** | O.9.1.1 | PFSP: direct cosφ set-point, gen [−1..0] / load [0..1] | PARAMETERS §10 | `cosfi. not confirmed` | R12 | — (`PFSPDFPF1` PFGnTgtSpt / PFLodTgtSpt in CID, SBO-ES) | **DOC** | P2 backlog |
| **(13)–(17)** | Lab PF2 | enter `P>thr` + debounce · release hysteresis · stale fail-safe | PF2_REGULATION §Equations · PARAMETERS §5 | — (`PF2_Equation_Dataflow.mermaid`) | L01 L02 L03 | `core/pf2/pf2_fsm.cpp` | **USED** | — |
| **(18)** | Lab | \(P=400+550\lvert1-\lvert\phi/60-1\rvert\rvert\), \(\phi=t\bmod120\) | PARAMETERS §5 | — | L04 | `lab/modbus_rtu_slave.py --ramp` · `ccli --lab-demo` | **USED-LAB** | — |
| — | O.9.2.1 | W110 arm when V ≈ 110 % Un → cap `w110_pct` | PARAMETERS §3 · Audit §2 | `Lim W110 O.9.2.1` | R04 | yaml `dso.w110_*` in `derive_active_power_kw()` | **PART** | No PdC voltage input (PPV is yaml-fed until P5-06) — P1 backlog |
| — | O.9.2.3 | WSd closed-loop tracking to set-point within TsP | PF2_REGULATION "Known limits" | `DSO_P_modulation_O_9_2_3_` | R03 | maps to PF2 export threshold only | **PART** | Needs (7)(8) runtime + plant P write path |
| — | O.10.3.1 / O.10.3.2 | WSa MSD P / Q set-points (Eth_B origin) | PARAMETERS §10 · Audit §3 | `S.P.W.(MSD).not.Confirmed` · `S.P.Q(MSD).Not. confirmed` | — | `WSaDAGC1.Mod` STUB | **DOC** | P3 backlog |
| — | O.13.1.2 / Annex U | DSO channel loss → Operating Rule fallback | PARAMETERS §4 | Wlim chart page 2 | R08 | `mms.comms_loss_fallback_s` (P3-05) · `pf2.stale_data_s` | **PART** | Full Annex U timer set not modelled |
| — | O.8.3 / T.3.2.1 | TotW 4 s blocks, 0/4/8… s aligned | Annex O extract §4 | — | R09 | `urcb_PdC_Mis4sec` intgPd 4000 ms · Modbus poll 4000 ms | **PART** | Boundary alignment not proven |
| — | O.14 | ≥ 2048 events, yyyy/mm/dd hh:mm:ss | P7-04 matrix | — | R14 | `core/event/event_store.cpp` | **USED** | P7-03 syslog open |
| — | Annex M | Teledistacco relay → inhibit | P6-03 | — | R15 | `annex_m_trip_monitor` + events | **PART** | DIO3 inhibit of PF2 not wired |

---

## 3. Roll-up

| Verdict | Equations / rules |
|---------|-------------------|
| **USED** | (1) (2) (5)(6) (9) (13)–(17) O.14 |
| **USED-LAB** | (3) (18) |
| **PART** | (4) O.9.2.1 W110 · O.9.2.3 tracking · O.13 fallback · O.8.3 alignment · Annex M |
| **TEST-ONLY** | (7)(8) |
| **DOC** | (10) (11) (12) · O.10.3 WSa |

RAG coverage: **all rows HAVE** a RAG source. draw.io coverage: **3 confirmed** (active P) · **6 draft** (Q / MSD) · **no chart** for (1) alone, (13)–(18), O.14, Annex M (by design — not DSO functions).

---

## 4. Code backlog (priority order)

| Prio | Item | Clause | Touches | Done when |
|------|------|--------|---------|-----------|
| **P0** | ~~Set-point spacing gate~~ | O.7.3.3 | `setpoint_gate.hpp`, `mms_adapter.cpp` | **r29 — done**, DUT evidence pending |
| **P0** | Runtime TsP / TsQ settling timer + O.14 event when band missed | O.7.3.1 | `core/dso/`, `svc_pf2.cpp` | bench row R06 goes from TEST-ONLY to live PASS |
| **P1** | PdC voltage input → auto-arm W110 | O.9.2.1 | Modbus PPV register, `dso_active_power.cpp` | `--regulation-check --live` R04 shows V path |
| **P1** | Negative WSptPct import path | O.9.2.3 / Eq (4) | `derive_active_power_kw()`, plant write | import set-point reaches plant |
| **P2** | Q(V) slow ring Eq (10) | O.9.1.3 / O.7.3.2 | new `dso_qv.cpp`, VArV* LN ctl | R10 PASS |
| **P2** | cosφ(P) Eq (11) | O.9.1.2 | new `dso_pfw.cpp`, PFW LN ctl | R11 PASS |
| **P2** | PFSP Eq (12) | O.9.1.1 | `dso_reactive_power.cpp`, PFSP SBO | R12 PASS |
| **P3** | WSa MSD P/Q (Eth_B) | O.10.3 | 104 adapter → `DsoLiveCommand` | WSa Mod → plant |
| **P3** | Full O.11 arbiter (indices 1–7) | O.11 | `dso_active_power.cpp` + reactive | Annex O Table 1 examples all pass |

---

## 5. Draw.io actions

| File | Action |
|------|--------|
| `DSO_P_modulation_O_9_2_3_.drawio` | Fix O.11 index label **4 → 3** (Audit §2) |
| `DSO_P_limit O_9_2_2.drawio` | Annotate "DSO command validation" node with **Eq (9) · R07 · `SetpointSpacingGate`** |
| 6 "not confirmed" charts | Confirm against Annex T Tables 88–92 when P2 work starts; add `Eq (10)/(11)/(12)` + `R10/R11/R12` labels |
| `CCI_Control_not final.drawio` | Replace with one "single plane" page: Field → bindings → `signal_map` → `core/dso` → MMS/GOOSE |

---

## 6. Verification

```bash
# Equation unit tests — R01 R03 R05 R06 R07 + L01–L03
ctest --test-dir apps/ccli/build-host -R 'pf2_fsm|dso_phase1|setpoint'

# Bench rows (R07 must read mms.setpoint_min_interval_s=3 → PASS)
ccli --regulation-check --config config/lab_tr400_phase1_regulation.yaml
ccli --regulation-check --live --json --config config/lab_tr400_cleartext_tsp.yaml

# DUT evidence for Eq (9): TSP → Operate WSdDAGC1.WSptPct twice within 3 s
#   expect stderr "REJECT — set-point spacing" + events.jsonl mms/setpoint_reject_spacing_o733
ccli --event-dump --count 20
```

---

## Traceability

| Corpus | Path / source_id |
|--------|------------------|
| Equations master | `Flowcharts/PARAMETERS_AND_EQUATIONS.md` (`ccli-flowcharts-params-equations`) |
| Phase 1 map | `lab/PF2_REGULATION_PHASE1.md` |
| Annex O extract | `knowledge-base/08-engineering/CCI_Annex_O_Extract.md` (`ccli-annex-o-extract`) |
| Draw.io text | `knowledge-base/08-engineering/CCI_Flowcharts_Drawio_Extract.md` (`ccli-flowcharts-drawio-extract`) |
| Code registry | `apps/ccli/core/dso/regulation_map.hpp` |
| Single plane | `apps/ccli/config/icd/signal_map.yaml` |

# Flowcharts folder — ingest & cross-check audit

**Document ID:** CCLI-FLOW-AUDIT-001  
**Date:** 2026-09-30  
**Scope:** `Flowcharts/` vs `lab/CCI_Figura2_Parameters.md` · `lab/PF2_REGULATION_PHASE1.md` · `apps/ccli/core/dso/` · `CCI_Annex_O_Extract.md`

---

## Summary

| Area | Result |
|------|--------|
| **Confirmed draw.io (3 files)** | Align with Annex O/T + Phase 1 `ccli` for W110, Wlim, WSd |
| **Not confirmed draw.io (6 files)** | Parameters captured in §10 of PARAMETERS_AND_EQUATIONS.md; **no code** |
| **New markdown/mermaid (4 files)** | Align with lab network + PF2 dataflow; minor README section ref fixed |
| **Code vs PARAMETERS status table** | **4 rows drift** — see §4 below (corrected in PARAMETERS v1.1) |
| **O.11 priority indices** | Draw.io WSd file mixed index 3/4; **Annex O Table 1** = WSd **index 3** |

---

## 1. File inventory (`Flowcharts/`)

| File | Type | Ingest status |
|------|------|---------------|
| `README.md` | Index | **HAVE** |
| `PARAMETERS_AND_EQUATIONS.md` | Master params/equations | **HAVE** (updated after audit) |
| `ARCHITECTURE_Network_Protocols.mermaid` | Network/TesPro vs ccli | **HAVE** — matches P4-00 + Phase 3 |
| `PF2_Equation_Dataflow.mermaid` | Code dataflow | **HAVE** — matches `core/dso/README.md` |
| `Reactive_Equation_Dataflow.mermaid` | Phase 5 Q dataflow | **HAVE** — Eq (3)(10)(11)(12) + TsQ |
| `DSO_P_limit O_9_2_2.drawio` | Wlim O.9.2.2 | **INGESTED** — confirmed |
| `DSO_P_modulation_O_9_2_3_.drawio` | WSd O.9.2.3 | **INGESTED** — confirmed |
| `Lim W110 O.9.2.1.drawio` | W110 O.9.2.1 | **INGESTED** — confirmed |
| `Q(V).not confirmed.drawio` | VArV O.9.1.3 | **INGESTED** — params only |
| `Cosfi(P).not confirmed.drawio` | PFW O.9.1.2 | **INGESTED** — params + equations |
| `cosfi. not confirmed.drawio` | PFSP O.9.1.1 | **INGESTED** — params only |
| `V.ctrl.Q.on.DSO.NotConfirmed.drawio` | VArSd O.9.1.4 | **INGESTED** — params only |
| `S.P.W.(MSD).not.Confirmed.drawio` | WSa P O.10.3.1 | **INGESTED** — Eth_B |
| `S.P.Q(MSD).Not. confirmed.drawio` | WSa Q O.10.3.2 | **INGESTED** — Eth_B |
| `CCI_Control_not final.drawio` | Overall CCI | **NOT INGESTED** — draft; open in draw.io separately |

---

## 2. Confirmed flowcharts vs code (Phase 1)

### Wlim — `DSO_P_limit O_9_2_2.drawio`

| Flowchart element | PARAMETERS / code | Match |
|-------------------|-------------------|-------|
| O.11 `P_bounded = min(...)` | `derive_active_power_kw()` min chain | **YES** (simplified) |
| Wlim priority **index 2** | Annex O Table 1 index **2** | **YES** |
| `WMaxSptPct` % Smax | yaml `dso.wmax_spt_pct` | **YES** |
| DSO channel loss → fallback | `mms.comms_loss_fallback_s` P3-05 | **PART** (not full Annex U) |
| Fast control ring output | PF2 threshold only | **PART** |

### WSd — `DSO_P_modulation_O_9_2_3_.drawio`

| Flowchart element | PARAMETERS / code | Match |
|-------------------|-------------------|-------|
| `WSptPct` signed % Smax | yaml `dso.wspt_pct: 20` → 42 kW | **YES** |
| Function status 2 = Automatic(Priority) | MMS `WSdDAGC1` | **YES** (model) |
| O.11 index **3** (label also says 4 in one node) | Annex O Table 1 index **3** | **FIX label** — use **3** |
| Negative WSptPct import | Code **ignores** negative | **GAP** documented |

### W110 — `Lim W110 O.9.2.1.drawio`

| Flowchart element | PARAMETERS / code | Match |
|-------------------|-------------------|-------|
| O.11 index **1** | `w110_*` yaml before Wlim in min() | **YES** |
| Qualitative V ≈ 110 % Un | No voltage meter in Phase 1 | **PART** |

---

## 3. Annex O Table 1 — O.11 priority (canonical)

Source: `knowledge-base/08-engineering/CCI_Annex_O_Extract.md` §8.

| Index | Function | Clause | Draw.io file | `ccli` Phase 1 |
|-------|----------|--------|--------------|----------------|
| **1** | 110 % V P limit | O.9.2.1 | `Lim W110 O.9.2.1.drawio` | yaml `w110_*` PART |
| **2** | DSO P limit (Wlim) | O.9.2.2 | `DSO_P_limit O_9_2_2.drawio` | **IMPL** |
| **3** | DSO P modulation (WSd) | O.9.2.3 | `DSO_P_modulation_O_9_2_3_.drawio` | **IMPL** |
| **4** | MSD active P set-point | O.10.3.1 | `S.P.W.(MSD)...` | STUB |
| **5** | DSO Q voltage control (VArSd) | O.9.1.4 | `V.ctrl.Q.on.DSO...` | STUB |
| **6** | PFSP / VArV / PFW | O.9.1.x | cosfi / Q(V) / Cosfi(P) | STUB |
| **7** | MSD reactive set-point | O.10.3.2 | `S.P.Q(MSD)...` | STUB |

**Phase 1 code note:** `derive_active_power_kw()` applies **min()** of simultaneous **export kW caps** (W110 → Wlim → WSd order). It does **not** implement full O.11 index pre-emption or reactive tier.

---

## 4. Status table drift (PARAMETERS v1.0 vs `regulation_map.hpp`)

| ID | `regulation_map.hpp` | PARAMETERS v1.0 | Resolution (v1.1) |
|----|----------------------|-----------------|-------------------|
| R04 | Pass | PART | **PART** — no V meter; yaml policy only |
| R08 | Pass | PART | **PART** — meter stale yes; full Annex U timers no |
| R09 | Pass | PART | **PART** — P3 TotW lab PART; not full O.8.3 proof |
| R14 | Pass | PART | **PART** — EventRing RAM-only |
| R07 | Na | NOT IMPL | **Na** — align wording with code enum |

---

## 5. Reactive flowcharts — extracted parameters (not in `core/dso/`)

### Q(V) — `Q(V).not confirmed.drawio`

- lock-in **0.20 × Pn**, lock-out **0.05 × Pn**
- **Vpu = V / Vn**; zones V2i, V1i, V1s, V2s
- **K** piecewise; **Q_target = K × Qmax**
- δQ default **5 % Qmax** (Annex T)

### PFW — `Cosfi(P).not confirmed.drawio`

- **P_A = 0.20×Pn**, **P_B = 0.50×Pn**, **P_C = 0.80×Pn**
- **cosφ_A = 1.00**, **cosφ_B = 1.00**, **cosφ_C = 0.95**
- Piecewise linear **cosφ_calculated(P)**; **δcosφ = 0.02**
- **Q_target = P × tan(arccos(cosφ))** if no native cosφ register
- Voltage lock-in **1.05×Vn**

### PFSP — `cosfi. not confirmed.drawio`

- Activation 1/5; Fst default **Autonomous (1)**
- Gen vs load cosφ set-point ranges **−1..+1**

### VArSd / MSD

- Eth_A (VArSd) vs Eth_B (MSD) origin documented in draw.io titles
- Same Mod/OpSt/Fst pattern as WSd

---

## 6. RAG ingest (2026-09-30)

| Item | Status |
|------|--------|
| Telematry mirror | **SYNCED** → `documents/projects/ccli/flowcharts/` |
| Draw.io text extract | **HAVE** → `knowledge-base/08-engineering/CCI_Flowcharts_Drawio_Extract.md` (10 files, ~40 KB) |
| HTTP RAG jobs | **PENDING** — `localhost:8001` not reachable at sync time |
| Telematry `ingest-ccli.ps1` | **UPDATED** — 4 `source_id`s registered |

**Rebuild:**

```powershell
powershell -File scripts/ingest-flowcharts.ps1          # sync + RAG when stack up
powershell -File scripts/ingest-flowcharts.ps1 -SkipRag # sync only
python scripts/extract-flowcharts-rag.py                # regenerate draw.io extract
```

| `source_id` | Path under Telematry `documents/` |
|-------------|-----------------------------------|
| `ccli-flowcharts-params-equations` | `projects/ccli/flowcharts/PARAMETERS_AND_EQUATIONS.md` |
| `ccli-flowcharts-audit` | `projects/ccli/flowcharts/FLOWCHARTS_CROSSCHECK_AUDIT.md` |
| `ccli-flowcharts-readme` | `projects/ccli/flowcharts/README.md` |
| `ccli-flowcharts-drawio-extract` | `projects/ccli/engineering/CCI_Flowcharts_Drawio_Extract.md` |

When RAG is up: `cd "C:\Yahia\projects\Telematry System"; .\scripts\ingest-ccli.ps1 -ProjectOnly` (includes flowcharts sources).

---

## 7. Equation cross-check — RAG extract vs PARAMETERS vs code

Verified after draw.io extract + numeric sanity check (2026-09-30).

| Eq | Draw.io / RAG text | PARAMETERS § | Code | Verdict |
|----|-------------------|--------------|------|---------|
| **(1)** Smax polygon | `Pn` / plant config nodes | §2 | `calc_smax_kva()` | **MATCH** — calc **206.16 kVA**; auth **210 kVA** |
| **(2)** pct → kW | Wlim: `L = (s/100) × Pn,plant` | §3 | `pct_smax_to_kw()` | **MATCH** — CEI uses **% Smax**; Pn≈Smax at cosφ≈1 |
| **(5)(6)** O.11 min | Wlim/WSd: `P_bounded = min(...)` | §3 Table 1 | `derive_active_power_kw()` min chain | **MATCH** (Phase 1 export caps only) |
| **(7)(8)** TsP ±5 % | Q(V) fast ring ±5 % Q | §4 | `settled_within_band_kw()` | **PART** — P band in code; Q band draw.io only |
| **(9)** 3 s spacing | Wlim/WSd: `Δt ≥ 3 s` | §4 R07 **Na** | not impl | **GAP** — documented |
| Figura 2 WSd | `WSptPct` +20 % | §3 | `tr57126_table1_dso()` | **MATCH** — **42 kW** (= 0.20 × 210) |
| O.11 custom | — | lab yaml 50/70/80 % | min caps | **MATCH** — **105 kW** |
| Wlim index | priority **2** | §3 index 2 | yaml order in min() | **MATCH** |
| WSd index | label **3** and **4** (conflict) | §3 index **3** | — | **FIX label** in draw.io node 9 |
| **(3)** VArSd/VArSa | **VArSptPct → kVAr** = pct/100 × Smax | §10 | — | **MATCH** draw.io + Annex T Table 88/89; **no code** |
| **(7)(8)** TsQ | ±5 % Q within **10 s** | §10 | — | **GAP** — P helper only in code |
| **(10)** Q(V) | lock-in **0.20×Pn**, δQ **5% Qmax**, K zones | §10 | — | **MATCH** PARAMETERS; **no code** |
| **(11)** PFW | P_A/B/C, cosφ_A/B/C, δcosφ=0.02, Q=tan | §10 | — | **MATCH** PARAMETERS; **no code** |
| **(12)** PFSP | Mod 1/5, cosφ direct | §10 | — | **MATCH** PARAMETERS; **no code** |
| Negative WSptPct | import set-point | §3 note | ignored in `dso_active_power.cpp` | **GAP** Phase 1 |
| Wlim curtail | `ΔP_curtail = max(0, P_base − L)` | — | threshold FSM only | **PART** — ceiling not closed-loop |

**Confirmed draw.io (3):** all Phase 1 active-power equations align with PARAMETERS and `core/dso/`.

**Not confirmed draw.io (6):** reactive parameters in RAG extract match PARAMETERS §10; no `ccli` implementation (Phase 5).

**Draft `CCI_Control_not final.drawio`:** ingested to RAG extract (O.11 overview, fast/slow rings); not equation-authoritative until finalized.

---

## 8. Gaps — remaining

| Gap | Action |
|-----|--------|
| `CCI_Control_not final.drawio` | Finalize or split per function |
| RAG HTTP ingest | Start Telematry stack; re-run `ingest-flowcharts.ps1` |
| WSd draw.io index 4 label | Correct node to **index 3** |
| ~~Eq **(9)** 3 s gate~~ | **Done r29** — `core/dso/setpoint_gate.hpp` + `mms_adapter.cpp`; R07 bench PASS; DUT evidence pending |

---

## 8a. Code utilization (2026-10-03) — see `ANNEX_EQUATION_UTILIZATION_MATRIX.md`

| Verdict | Equations |
|---------|-----------|
| **USED** | (1) (2) (5)(6) **(9)** (13)–(17) |
| **USED-LAB** | (3) (18) |
| **PART** | (4) import · O.9.2.1 V input · O.9.2.3 tracking |
| **TEST-ONLY** | (7)(8) TsP / TsQ |
| **DOC** | (10) (11) (12) · O.10.3 WSa |

Single plane: `apps/ccli/config/icd/signal_map.yaml` now carries `equations:` (eq1…eq18), `application_groups` A0–A8, `goose_map`, and `logical_nodes` for all **31** CID LNs with `equation` / `regulation_id` / `drawio_ref` / `binding`.

Row R07 in §4 above is superseded: `regulation_map.hpp` R07 = **Pass / Eq (9)** since r29.

---

## 9. Verification commands

```bash
ccli --regulation-check --config apps/ccli/config/lab_tr400_phase1_regulation.yaml
ctest --test-dir apps/ccli/build-native -R 'pf2_fsm|dso_phase1'
```

---

## Traceability

| Corpus | source_id / path |
|--------|------------------|
| Annex O Table 1 | `ccli-annex-o-extract` |
| Figura 2 numbers | `lab/CCI_Figura2_Parameters.md` |
| Master map | `regulation_map.hpp` |
| Flowcharts equations | `ccli-flowcharts-params-equations` |
| Draw.io RAG text | `ccli-flowcharts-drawio-extract` |
| This audit | `ccli-flowcharts-audit` |

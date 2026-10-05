# Flowcharts — parameters & equations (CCLI programme)

**Purpose:** Canonical home for **regulation flowcharts**, **parameter tables**, and **equations** used in the HiTEKS CCLI / CEI 0-16 PF2 controller.

**Normative sources:** CEI 0-16 Allegato **O** · Allegato **T** · **TR 57-126** Figura 2 · AiLux operator path (104).

**Code traceability:** `apps/ccli/core/dso/` · `apps/ccli/core/pf2/` · `apps/ccli/config/icd/signal_map.yaml`

---

## Start here

| Document | Contents |
|----------|----------|
| **[PARAMETERS_AND_EQUATIONS.md](PARAMETERS_AND_EQUATIONS.md)** | Master equation list, yaml parameters, lab network ports, implementation status |
| **[FLOWCHARTS_CROSSCHECK_AUDIT.md](FLOWCHARTS_CROSSCHECK_AUDIT.md)** | Ingest audit vs code + Annex O Table 1 |
| **[ANNEX_EQUATION_UTILIZATION_MATRIX.md](ANNEX_EQUATION_UTILIZATION_MATRIX.md)** | Per-equation verdict: RAG · draw.io · code symbol · USED / PART / DOC + backlog |
| **[ARCHITECTURE_Network_Protocols.mermaid](ARCHITECTURE_Network_Protocols.mermaid)** | LAN1/LAN2/LAN3 · ports 102 / 3782 / 2404 · TesPro vs `ccli` |
| **[PF2_Equation_Dataflow.mermaid](PF2_Equation_Dataflow.mermaid)** | DSO commands → O.11 min → PF2 FSM → DIO |
| **[Reactive_Equation_Dataflow.mermaid](Reactive_Equation_Dataflow.mermaid)** | VArSd/VArSa Eq (3) · Q(V)/PFW/PFSP · TsQ · TotVAr (Phase 5) |

**Deep dives (lab):**

- `lab/CCI_Figura2_Parameters.md` — TR Table 1 worked example
- `lab/PF2_REGULATION_PHASE1.md` — R01–L04 master map
- `apps/ccli/core/dso/README.md` — code layer stack

---

## Draw.io flowcharts (function logic)

| File | Function | Clause | Status |
|------|----------|--------|--------|
| `DSO_P_limit O_9_2_2.drawio` | **Wlim** — active power limit | O.9.2.2 | Confirmed |
| `DSO_P_modulation_O_9_2_3_.drawio` | **WSd** — DSO P modulation | O.9.2.3 | Confirmed |
| `Lim W110 O.9.2.1.drawio` | **110 % V** autonomous limit | O.9.2.1 | Confirmed |
| `Q(V).not confirmed.drawio` | **VArV** — Q(V) | O.9.1.3 | Not confirmed |
| `Cosfi(P).not confirmed.drawio` | **PFW** — cosφ(P) | O.9.1.2 | Not confirmed |
| `cosfi. not confirmed.drawio` | **PFSP** — set-point cosφ | O.9.1.1 | Not confirmed |
| `V.ctrl.Q.on.DSO.NotConfirmed.drawio` | **VArSd** — DSO Q set-point | O.9.1.4 | Not confirmed |
| `S.P.Q(MSD).Not. confirmed.drawio` | **WSa** — MSD Q (Eth_B) | O.10.3 / O.13.1.3 | Not confirmed |
| `S.P.W.(MSD).not.Confirmed.drawio` | **WSa** — MSD P (Eth_B) | O.10.3.1 | Not confirmed |
| `CCI_Control_not final.drawio` | Overall CCI control (draft) | — | Not final |

Open with [draw.io](https://app.diagrams.net/) or VS Code Draw.io extension.

---

## Equation index (quick)

| Eq | Formula | See |
|----|---------|-----|
| **(1)** | \(S_{\max}=\sqrt{\max(P_{\mathrm{imm}}^2,P_{\mathrm{ass}}^2)+\max(Q_{\mathrm{ind}}^2,Q_{\mathrm{cap}}^2)}\) | PARAMETERS §2 |
| **(2)** | \(P_{\mathrm{kW}}=(\mathrm{pct}/100)\,S_{\max,\mathrm{kVA}}\) | PARAMETERS §3 |
| **(3)** | \(Q_{\mathrm{kVAr}}=(\mathrm{VArSptPct}/100)\,S_{\max,\mathrm{kVA}}\) — VArSd / VArSa | PARAMETERS §10 |
| **(5)(6)** | \(P_{\mathrm{eff}}=\min(\text{active export caps in kW})\) | PARAMETERS §3 + O.11 Table 1 |
| **(7)(8)** | ±5 % band — TsP ≤ 60 s (P) · TsQ ≤ 10 s (Q) | PARAMETERS §4 · §10 |
| **(9)** | Δt ≥ 3 s between external set-points (O.7.3.3) — **IMPL r29** `SetpointSpacingGate` | UTILIZATION MATRIX §2 |
| **(10)–(12)** | Curves Q(V), PFW, PFSP | PARAMETERS §10 — Phase 5 |
| **(13)–(17)** | PF2 FSM debounce / hysteresis / stale | PARAMETERS §5 |
| **(18)** | Lab demo power ramp | PARAMETERS §5 |

---

## Implementation phases

| Phase | What flowcharts cover | Owner SW |
|-------|----------------------|----------|
| 1 | Equations + PF2 FSM (yaml mock) | `ccli` |
| 3 | MMS Wlim/WSd live on Eth_A :3782 | `ccli` |
| 4 | Operator 104 Eth_B :2404 | `ccli` |
| 4 P4-00 | Plant poll + northbound MQTT/TCP | TesPro 61850 |
| 5 | Reactive LNs (VArSd, PFW, VArV, PFSP) | `ccli` STUB |

---

## Regenerate / export

```bash
# Word parameter tables (optional)
python scripts/generate_cci_figura2_docx.py

# Regulation bench CLI
ccli --regulation-check --config apps/ccli/config/lab_tr400_phase1_regulation.yaml
```

## RAG ingest (Telematry)

```powershell
python scripts/extract-flowcharts-rag.py
powershell -File scripts/ingest-flowcharts.ps1 -SkipRag   # sync mirror only
powershell -File scripts/ingest-flowcharts.ps1            # sync + enqueue RAG jobs
```

**source_id:** `ccli-flowcharts-params-equations` · `ccli-flowcharts-drawio-extract` · `ccli-flowcharts-audit`

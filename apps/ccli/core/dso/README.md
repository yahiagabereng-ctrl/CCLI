# DSO / plant equation layer (Phase 1 + Eth_A live)

**core** logic: plant Smax + O.9.2.x / O.11 active-P arbiter → PF2 kW thresholds.
Live Eth_A MMS populates the same `DsoActivePowerCommands` via `DsoLiveCommand`.

## Hierarchy → code (Annex T T.3.3.1)

```text
IED CCI016_01 / LD_Plant
  WlimDWMX1.{Mod,WMaxSptPct}   O.9.2.2 lim W     → live.wlim_*   IMPL
  WSdDAGC1.{Mod,WSptPct}       O.9.2.3 s.p. W    → live.wsd_*    IMPL (Figura 2)
  WSaDAGC1 / VArSd / PFSP / VArV / PFW           → Mod=5 STUB
  PdCMMXU1.TotW + urcb 4 s     O.8.3 / T.3.2.1   → MeasurementStore PART
```

Freeze file: `apps/ccli/config/icd/signal_map.yaml` (clauses + `maps_to` + runtime_status).

## Layer stack

```text
adapters/iec61850_mms (ControlAction_getControlObject)
        │  DsoLiveCommand { wlim_*, wsd_* }
        ▼
core/dso/dso_phase1  apply_live_dso_commands_to_pf2 / apply_dso_to_pf2_config
        │  derive_active_power_kw  (O.11 min of active caps)
        ▼
core/pf2/pf2_fsm     set_thresholds → DIO1
```

## Data flow

1. **Load** `CcliConfig` (yaml) — optional TR mock when `dso.enabled && pf2.use_dso_mock`.
2. **Live MMS** Operate on Wlim/WSd → `poll_dso_live_command` → `apply_live_dso_commands_to_pf2`.
3. **Pf2Fsm** compares Modbus `p_kw` to enter/release thresholds.
4. **P3-05** no Eth_A clients → clear live Wlim+WSd (Operating Rule yaml resumes).

## Files

| File | Responsibility |
|------|----------------|
| `regulation_map.hpp` | Master map **R01–R15**, **L01–L04**, equation labels |
| `plant_envelope.hpp` | O.8.2 polygon **Smax**; **pct_smax_to_kw** |
| `dso_active_power.*` | W110 / Wlim / WSd → **p_effective_export_kw** |
| `dso_phase1.*` | yaml apply + **apply_live_dso_commands_to_pf2** |
| `../adapters/iec61850_mms/mms_adapter.*` | LN model + ctlVal → `DsoLiveCommand` |

## Config gates (yaml mock)

| Key | Meaning |
|-----|---------|
| `dso.enabled` | Run equation layer |
| `pf2.use_dso_mock` | Allow overwriting PF2 kW thresholds |

**`lab_tr400_dso_tr57126.yaml`**: WSd 20 % → **~42 kW** (Figura 2).

Normative: `lab/CCI_Figura2_Parameters.md` · `lab/PF2_REGULATION_PHASE1.md`.

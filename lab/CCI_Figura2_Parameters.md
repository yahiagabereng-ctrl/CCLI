# CEI TR 57-126 Figura 2 — Parameters, equations & CCLI mapping

**Document ID:** CCLI-LAB-FIG2-PAR-001  
**Revision:** 1.1  
**Date:** 2026-09-26  
**Normative:** CEI 0-16 Allegato **O** / **T** · CEI **TR 57-126** §6.2 Table 1, Figura 2  
**Code:** `apps/ccli/core/dso/` · `apps/ccli/adapters/iec61850_mms/` · `apps/ccli/config/icd/signal_map.yaml`  
**Word export:** `lab/CCI_Figura2_Parameters.docx` (generated) · regenerate: `python scripts/generate_cci_figura2_docx.py`

---

## Table numbering note (Italian consolidated vs English extract)

| Function | Italian consolidated Allegato T | English `CCI_Annex_T_Extract.md` §5 |
|----------|--------------------------------|-------------------------------------|
| **Wlim** (O.9.2.2) | **Tab. 84** | Table **85** |
| **WSd** (O.9.2.3) | **Tab. 85** | Table **86** |
| **WSa** (MSD set-point) | Tab. 86 | Table **87** |

All **percentage** control values are referred to **Smax** (kVA at PdC). Voltage p.u. values refer to **Un** at PdC.

---

## 1. Figura 2 use case (TR Table 1 — what is ON)

| Function | TR Table 1 state | Annex O | Role in example |
|----------|------------------|---------|-----------------|
| **Wlim** | Not operative / Inactive | O.9.2.2 | **Off** — no slaved generation cap |
| **WSd** | Operative / Active | O.9.2.3 | **On** — DSO modulation (**green s.p. W**) |
| **WSptPct** | **+20 %** Smax | O.9.2.3 / O.10.3.1 | Export set-point |
| VArSd, PFSP, VArV, PFW | Not operative / Inactive | O.9.1.x | Reactive paths **off** |

Source: `Architecture/_extracted_reg_analysis/cei_tr_57-126_en.txt` (Table 1, ~pp. 13–15).

---

## 2. TR Table 1 — plant envelope & identity (O.8.2)

| Parameter | TR value | Unit | Annex T (IEC 61850) | CCLI yaml (Phase 1 mock) | Code |
|-----------|----------|------|---------------------|--------------------------|------|
| IED / FDI name | CCI016_01 | — | `IED name` in CID | — | `config/icd/cei-tr-57-126-example.cid` |
| IP / mask / gateway | 192.168.8.167 / 255.255.255.0 / 192.168.8.1 | — | Eth_A (product) | `interfaces.eth_a` (lab bridged) | `config/lab_tr400.yaml` |
| location (POD) | IT000E123456789 | — | — | — | — |
| Max active power **export** (Pfed) | **200** | kW | `DPCC1.PdC_Wi.WRtg` | `plant.p_imm_kw` | `plant_envelope.hpp`, `pf2_fsm.hpp` |
| Max active power **import** (Pass) | **200** | kW | `DPCC1.PdC_Wa.WRtg` | `plant.p_ass_kw` | same |
| Max inductive Q | **50** | kVAr | `DPCC2.PdC_Qi.VArRtg` | `plant.q_ind_kvar` | same |
| Max capacitive Q | **50** | kVAr | `DPCC2.PdC_Qc.VArRtg` | `plant.q_cap_kvar` | same |
| **Smax** (nameplate) | **210** | kVA | `DPCC3.PdC_VA.VARtg` | `plant.smax_kva` | `smax_kva_effective()` |
| Regulation availability (plant/gen/storage) | 1 = Available | — | various `Beh` | **Not mocked** | — |

---

## 3. TR Table 1 — active power controls (Figura 2)

| Parameter | TR value | CCLI struct / yaml | Preset `tr57126_table1` |
|-----------|----------|--------------------|-------------------------|
| Wlim operating / activation | 5 / Inactive | `wlim_active: false`, `wmax_spt_pct: 0` | Wlim **off** |
| WSd operating / activation | 1 / Active | `wsd_active: true` | WSd **on** |
| **WSptPct** | **+20** (% Smax, signed) | `wspt_pct: 20` | **20 %** |
| O.9.2.1 110 % V (W110) | Not in Table 1 row | `w110_active: false` | off |

---

## 4. Equations & worked values (Phase 1 code)

| ID | Equation | Result for TR Table 1 | Implementation |
|----|----------|----------------------|----------------|
| **(1)** | \(S_{\max}=\sqrt{\max(P_{\mathrm{imm}}^2,P_{\mathrm{ass}}^2)+\max(Q_{\mathrm{ind}}^2,Q_{\mathrm{cap}}^2)}\) | **206.15 kVA** | `calc_smax_kva()` |
| **(1b)** | Authoritative Smax from Operating Rule | **210 kVA** used as p.u. base | `smax_kva_effective()` |
| **(2)** | \(P_{\mathrm{kW}}=(\mathrm{pct}/100)\,S_{\max,\mathrm{kVA}}\) | WSd: **42 kW** | `pct_smax_to_kw()` |
| **(5)(6)** | \(P_{\mathrm{eff}}=\min(\text{active export caps})\) | **42 kW** (WSd only) | `derive_active_power_kw()` |
| **(7)(8)** | \(\|P-P_{\mathrm{exp}}\|/\|P_{\mathrm{exp}}\|\le 0.05\), TsP ≤ 60 s | Unit test only | `settled_within_band_kw()` |
| **(13)–(17)** | Lab FSM: P vs threshold, debounce, stale | TR mock: enter **42**, release **37** kW (`release_delta_kw: 5`) | `apply_dso_to_pf2_config()`, `pf2_fsm.cpp` |
| **(18)** | Demo ramp 400–950 kW / ~120 s | `--lab-demo` / `modbus_rtu_slave.py --ramp` | `ccli_main.cpp` |

**Config gates:** `dso.enabled: true` **and** `pf2.use_dso_mock: true` → see `config/lab_tr400_dso_tr57126.yaml`.

**Measured P (not a Table 1 %):** Modbus holding reg **40001** float32 BE → `MeasurementStore.p_kw` (`modbus_adapter.cpp`).

---

## 5. O.7 timing (Annex O — all Figura 2 functions)

| Parameter | CEI limit | Annex O | Phase 1 CCLI |
|-----------|-----------|---------|--------------|
| **TsP** (active P) | ≤ **60 s** to ±**5 %** | O.7.3.1 | Band helper only; FSM **debounce_s: 30** ≠ TsP proof |
| **TsQ** (reactive) | ≤ **10 s** | O.7.3.1 | Not implemented |
| **ΔT** (slow ring) | **10–600 s**, default **60 s** | O.7.3.2 | Not implemented |
| External set-point spacing | ≥ **3 s** | O.7.3.3 | Not implemented |
| PF1 / TotW alignment | **4 s** blocks | O.8.3 | Modbus poll **1 s** |

---

## 6. Annex T control parameters (ranges & defaults — English extract §5)

Figura 2 **activates WSd (Table 86 EN / Tab. 85 IT)** only; others stay at **Not-Operating / Inactive** defaults.

### 6.1 Wlim — O.9.2.2 (English Table 85)

| Parameter | Range | Default | Figura 2 TR |
|-----------|-------|---------|-------------|
| Operating status | 1=Operating / 5=Not-Operating | 5 | **5** |
| Active power limit (generation) | 0..100 % Smax | 0 | **0 %** (inactive) |
| Activation | 5=Inactive / 1=Active | 5 | **Inactive** |
| Setpoint function status | 0=N/A / 1=Autonomous / 2=Automatic | 1 | N/A (off) |
| Limit function status 110 % V | 0=N/A / 1=Autonomous | 2 | N/A |

**Trigger (O.9.2.1):** qualitative — V near **110 % Un** (no numeric equation in O).

### 6.2 WSd — O.9.2.3 (English Table 86)

| Parameter | Range | Default | Figura 2 TR |
|-----------|-------|---------|-------------|
| Operating status | 1=Operating / 5=Not-Operating | 5 | **1** |
| Feed-in / feed-out setpoint | 0..100 % (+/−) Smax | 100 / 0 | **+20 %** |
| Activation | 5=Inactive / 1=Active | 5 | **Active** |
| Function status | 0=N/A / 2=Automatic | 2 | Automatic |

**Timing:** TsP ≤ 60 s (O.7.3.1) + update ≥ 3 s (O.7.3.3) — **not enforced in Phase 1 code**.

### 6.3 WSa — O.10.3.1 MSD (English Table 87)

Same shape as WSd; **discretionary** MSD participation. TR Figura 2 example uses **WSd**, not WSa.

### 6.4 Reactive functions (English Tables 88–92)

| Function | Clause | TsQ / ΔT | Figura 2 TR |
|----------|--------|----------|-------------|
| VArSd / s.p. Q (DSO) | O.9.1.4 / O.10.3.2 | TsQ ≤ 10 s | **Off** |
| PFSP / s.p. cosφ | O.9.1 | TsQ ≤ 10 s | **Off** |
| VArV / Q(V) | O.9.1.3 | ΔT 10–600 s; δQ = **5 % Qmax** | **Off** |
| PFW / cosφ(P) | O.9.1.2 | ΔT 10–600 s; δcosφ = **0.02** | **Off** |

Full default numerics: `knowledge-base/08-engineering/CCI_Annex_T_Extract.md` §5 (Tables 88–92).

---

## 7. Hierarchy · clauses · code (product map)

Canonical freeze: **`apps/ccli/config/icd/signal_map.yaml`**.

```text
IED CCI016_01
  └─ LD LD_Plant                          (T.3.3.1)
       ├─ LLN0 + urcb_PdC_Mis4sec         T.3.2.1 Type 3, intgPd=4000
       ├─ WlimDWMX1                       O.9.2.2 / EN Table 85 — lim W
       ├─ WSdDAGC1                        O.9.2.3 / EN Table 86 — s.p. W (Figura 2 green)
       ├─ WSaDAGC1                        O.10.3.1 / EN Table 87 — STUB
       ├─ VArSdDVAR1                      O.9.1.4 / EN Table 88 — STUB
       ├─ PFSPDFPF1                       O.9.1.1 / EN Table 90 — STUB
       ├─ VArVDVVR1                       O.9.1.3 / EN Table 91 — STUB
       ├─ PFWDPFW1                        O.9.1.2 / EN Table 92 — STUB
       └─ PdCMMXU1                        O.8.3 / T.3.1.3 — TotW IMPL; Q/V DEFERRED
```

| Figura 2 | Clause | LN reference | Status | Code path |
|----------|--------|--------------|--------|-----------|
| lim W | O.9.2.2 · T.3.3.1.2.1 | `LD_Plant/WlimDWMX1.{Mod,WMaxSptPct}` | **IMPL** | `mms_adapter.cpp` → `DsoLiveCommand.wlim_*` → `derive_active_power_kw` |
| **s.p. W** | O.9.2.3 · T.3.3.1.2.2 | `LD_Plant/WSdDAGC1.{Mod,WSptPct}` | **IMPL** | same → `live.wsd_*` (ControlAction_getControlObject) |
| lim W110 | O.9.2.1 · O.11 | yaml only (not a DSO LN) | **PART** | `DsoActivePowerCommands.w110_*` |
| s.p. Q | O.9.1.4 | `VArSdDVAR1` | **STUB** | Mod=5 ENS; no ctlVal path |
| s.p. cosφ | O.9.1.1 | `PFSPDFPF1` | **STUB** | same |
| Q(V) | O.9.1.3 | `VArVDVVR1` | **STUB** | same |
| Cosφ(P) | O.9.1.2 | `PFWDPFW1` | **STUB** | same |
| PdC TotW | O.8.3 · T.3.2.1 | `PdCMMXU1.TotW` | **IMPL** | `update_tot_w_kw` ← Modbus |
| Gen* / XCBR | O.8.4 · T.3.3.1.1.7 | — | **DEFERRED** | document only |

**Actuation chain (active P):**

`Operate` → `on_control(ControlAction)` → `DsoLiveCommand` → `apply_live_dso_commands_to_pf2` → `Pf2Fsm::set_thresholds` → DIO1.

**O.11 priority (Phase-1 export arbiter):** W110 > Wlim > WSd > WSa (stub) > reactive (stub) — `min()` of simultaneous kW caps in `dso_active_power.cpp`.

## 8. Code reference index

| Topic | Path | Key symbols |
|-------|------|-------------|
| Object freeze + clauses | `apps/ccli/config/icd/signal_map.yaml` | `controls.wlim/wsd/...`, `runtime_status` |
| MMS model + ctlVal | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp` | `WlimDWMX1`, `WSdDAGC1`, `on_control`, `poll_dso_live_command` |
| Live command struct | `mms_adapter.hpp` | `DsoLiveCommand` |
| Eq (1)(2) | `apps/ccli/core/dso/plant_envelope.hpp` | `calc_smax_kva`, `pct_smax_to_kw` |
| Eq (5)(6) O.11 | `apps/ccli/core/dso/dso_active_power.cpp` | `derive_active_power_kw`, `tr57126_table1_*` |
| Live → PF2 | `apps/ccli/core/dso/dso_phase1.cpp` | `apply_live_dso_commands_to_pf2` |
| Main poll | `apps/ccli/services/ccli_main.cpp` | `poll_dso_live_command` |
| PF2 FSM | `apps/ccli/core/pf2/pf2_fsm.cpp` | L01–L03 |
| P input | `apps/ccli/adapters/modbus_master/modbus_adapter.cpp` | reg 40001 |
| Tests | `apps/ccli/tests/unit/dso_phase1_test.cpp` | 206.15, 42, 37 kW |

## 9. Limitations (lab vs full product)

1. **WSd** maps to **export ceiling** for threshold FSM — not closed-loop O.9.2.3 tracking to ±5 % in TsP.  
2. **Negative WSptPct** (import) ignored (`max(0, wspt_pct)`).  
3. Reactive / WSa LNs are **present** (Mod=5) but **not operable** (no APC / handlers).  
4. PdC **TotVAr / PPV / A** and Gen* MMXU2 — DEFERRED.  
5. Product ctlModel should be **SBO-with-enhanced-security**; lab uses DIRECT_ENHANCED.

## 10. Verification

| Check | Command / artifact | Pass |
|-------|-------------------|------|
| Eq layer | `ctest -R dso_phase1` | 206.15, 42, 37 kW |
| TR profile on device | `ccli --config …/lab_tr400_dso_tr57126.yaml --lab-demo` | log `p_effective=42 kW` |
| Live WSd MMS | Operate `WSdDAGC1.Mod=1` + `WSptPct=20` | log `mms→pf2: … WSd=on@20%` |
| Object map | `signal_map.yaml` statuses | Wlim/WSd IMPL; reactives STUB |

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.1 | 2026-09-26 | WSd MMS IMPL; full Figura 2 hierarchy ↔ clause ↔ code matrix |
| 1.0 | 2026-09-22 | Initial repo doc + Word generator; TR Table 1 + CCLI mapping |

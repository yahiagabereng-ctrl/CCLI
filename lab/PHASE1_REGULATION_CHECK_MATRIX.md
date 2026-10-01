# Phase 1 — Regulation check matrix (values & actions)

**Document ID:** CCLI-LAB-REG-CHECK-MATRIX-001  
**Tool:** `ccli --regulation-check [--live] [--json]`  
**Configs:** `lab_tr400.yaml` (900 kW) · `lab_tr400_phase1_regulation.yaml` (CEI ~42 kW) · `lab_tr400_dso_custom_o11.yaml` (105 kW O.11)

Run on TG544 after deploy:

```bash
/usr/sbin/ccli --regulation-check --config /etc/ccli/lab.yaml
/usr/sbin/ccli --regulation-check --live --live-watch 8 --config /etc/ccli/lab.yaml
/usr/sbin/ccli --regulation-check --live --json --config /etc/ccli/lab.yaml
```

---

## A. Static check (`--regulation-check`)

| ID | Clause | Expected (reference) | Actual source | Pass action | Fail action |
|----|--------|----------------------|---------------|-------------|-------------|
| **R01** | O.8.2 Eq (1) | S_calc ≈ **206.15 kVA** (200/200 kW, 50/50 kVAr) | Computed from yaml `plant:` | **PASS** — corners match TR Table 1 | Fix `plant.p_*` / `q_*` in yaml |
| **R02** | O.9.2.2 | Wlim **off** (TR Figura 2) | `dso.wlim_*` | **PASS** when inactive | Set `wlim_active: false` |
| **R03** | O.9.2.3 | WSd **20 %** → **42 kW** on Smax **210 kVA** | `p_wsd_kw` | **PASS** when `wsd_active` + 20 % | Set `dso.wsd_active: true`, `wspt_pct: 20` |
| **R04** | O.9.2.1 | w110 **off** (TR Table 1) | `w110_*` | **PASS** when inactive | `w110_active: false` |
| **R05** | O.11 Eq (5)(6) | **p_effective** = min(export caps) e.g. **42 kW** | `derive_active_power_kw` | **PASS** when mock gates on | Enable `dso.enabled` + `pf2.use_dso_mock` |
| **R06** | O.7.3.1 | ±5 % band (unit test) | `settled_within_band_kw` | **PASS** | `dso_phase1_test` |
| **R07** | O.7.3.3 | 3 s spacing | MMS | **N/A** Figura 2 | Phase 3+ |
| **R08** | O.13.1.2 | Comm loss safe | **`stale_data_s=10`** | **PASS** = 10 | Match yaml L03 |
| **R09** | O.8.3 | 4 s TotW | Modbus **`poll_ms=4000`** | **PASS** = 4000 | Do not use 1000 ms on regulation yaml |
| **R10–R13** | O.9.1 / T | Reactive functions | TR Table 1 **inactive** | **N/A** Phase 1 | **→ Phase 5-R** (P5-R01…R09) |
| **R14** | O.14 | Logger ≥2048 | `EventRing::kMaxEvents` | **PASS** ≥2048 | Built into binary |
| **R15** | Annex M | DIO3 inhibit | Lab 2-wire | **N/A** | Phase 4 DIO3 |
| **L01** | Lab | Enter threshold **900 kW** *or* **42 kW** (DSO) | `pf2.threshold_kw` after apply | **PASS** matches profile | See CFG row |
| **L02** | Lab | Release **850 kW** *or* **37 kW** (Δ5) | `release_threshold_kw` | **PASS** | Set `dso.release_delta_kw` |
| **L03** | Lab | Stale **10 s** → safe OFF | `stale_data_s` | **PASS** = 10 | Match yaml |
| **L04** | Lab Eq (18) | Ramp **400–950 kW** / 120 s | `modbus_rtu_slave.py --ramp` | **PASS** (tool exists) | Run PC slave with `--ramp` |
| **TR-T1** | TR Table 1 | Smax **210 kVA** | `plant.smax_kva` | **PASS** | Set `smax_kva: 210` |
| **PLANT** | O.8.2 | **200/200 kW**, **50/50 kVAr** | yaml `plant:` | **PASS** | Align with Figura 2 doc |
| **MOCK** | yaml | All **w110/wlim/wsd** flags + % | `dso:` section | **PASS** | Tune mocks per scenario |
| **CFG** | yaml | **`dso.enabled` + `use_dso_mock`** both true for Eq→kW | Both keys | **PASS** when CEI path wanted | Set both **true** for ~42 kW |
| **IO-DI** | REQ-LAB-003 | **active_low** (TG544 + 12 V key) | `di_permissive` | **PASS** | Do not set `active_low: false` |
| **IO-byp** | REQ-LAB-003 | **permissive_bypass: false** | yaml | **PASS** | Remove bypass for interlock test |
| **REQ-61850** | Annex T | MMS live | yaml mock active-P | **N/A** Phase 1 | Phase 3 MMS |

---

## B. Live check (`--regulation-check --live`)

Requires: Modbus slave (or `--lab-demo`), GD32 **online**, permissive wired.

Use **`--live-watch SEC`** to print each **FC03 Read Holding** poll (default watch ≈ `max(4 s, poll_ms)`).

| ID | Input / condition | Expected | Pass criteria | Fail — action |
|----|-------------------|----------|---------------|---------------|
| **LIVE-MB** | RTU master A2/B2 | FC03 @ **40001** | **PASS** read OK | Slave / ttyS2 / baud |
| **LIVE-MB-R** | Raw regs | float32 BE **P** | hex in report | vs PC slave |
| **LIVE-MB-W** | Curtailment | no FC06/16 P1 | **N/A** | Curtail = **DIO1** |
| **LIVE-CONF-*** | Per map row **R01–L04** | Modbus or **N/A** | **PASS/N/A** in report | See static §A |
| **LIVE-P** | Modbus reg **40001** P (kW) | Good quality reading | **PASS** `quality=good` | Start PC slave; check A2/B2 `/dev/ttyS2` |
| **LIVE-FSM** | P > threshold ≥ debounce | `curtailment_commanded=yes` | **PASS** + reason `p_above_threshold` / `debounce` | Raise slave P (e.g. **950** legacy or **>42** CEI) |
| **LIVE-DIO** | FSM ON ∧ permissive OK | **DIO1 ON** (`do_curtail_on`) | **PASS** relay energised | Close permissive key; check GD32 |
| **LIVE-DI** | Key closed | **permissive OK** | **PASS** | Fix DIO2 / yaml polarity |

---

## C. Bench actions (recommended order)

| Step | Action | Command / check |
|------|--------|-----------------|
| 1 | Deploy binary + yaml | `/usr/sbin/ccli --version` |
| 2 | GD32 online | `ubus call dido_v2 gd32.status` → `"online": true` |
| 3 | Static regulation | `ccli --regulation-check --config /etc/ccli/lab.yaml` |
| 4 | Legacy path | yaml: mock **off** → L01 **900 kW**; slave **950 kW** → DIO1 after **30 s** |
| 5 | CEI path | Use `lab_tr400_phase1_regulation.yaml` → L01 **~42 kW**; slave **>42 kW** |
| 6 | Live finalize | `lab/modbus-ccli-live-bench.ps1` or `ccli --regulation-check --live --live-watch 8 --json` → `regulation_bench/index.html` |
| 7 | Stale safe | Stop slave → within **10 s** DIO1 **OFF**, FSM safe |

---

## D. Numeric quick reference (TR Figura 2 / Phase 1 regulation yaml)

| Parameter | Value | Maps to |
|-----------|-------|---------|
| Smax (authoritative) | **210 kVA** | Eq (2) % base |
| S_calc Eq (1) | **~206.15 kVA** | R01 |
| WSptPct | **+20 %** | R03 |
| P_WSd / p_effective | **42 kW** | L01 enter (DSO mock) |
| Release Δ | **5 kW** (yaml) | L02 → **37 kW** |
| Legacy lab enter / release | **900 / 850 kW** | mock off |
| Debounce / stale / poll | **30 s / 10 s / 4000 ms** | L01 / L03 / R09 |

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.1 | 2026-09-22 | Align PASS/N/A with `tr57126_table1`, `poll_ms=4000`, EventRing 2048 |

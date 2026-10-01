# Parameters & equations — master reference

**Document ID:** CCLI-FLOW-PAR-EQ-001  
**Revision:** 1.2 (reactive Eq (3) VArSd/VArSa 2026-09-30)  
**Parent:** `Flowcharts/README.md` · **`Flowcharts/FLOWCHARTS_CROSSCHECK_AUDIT.md`**  
**Code:** `apps/ccli/core/dso/regulation_map.hpp` · `plant_envelope.hpp` · `dso_active_power.cpp` · `pf2_fsm.cpp`

---

## 1. Lab network parameters (TG544)

| Parameter | Value | Interface | Protocol | Software |
|-----------|-------|-----------|----------|----------|
| `eth_a_ip` | **192.168.10.1/24** | LAN1 | MMS **TCP 3782** TLS | `ccli` DSO server |
| `eth_b_ip` | **192.168.1.130/24** | LAN2 | **104 TCP 2404** (planned) | `ccli` |
| `plant_ip` | **192.168.30.1/24** | LAN3 | MMS **TCP 102** (poll target) | TesPro client → plant IED |
| `eng_ip` | 192.168.0.1/24 | br-lan | LuCI / SSH | TesproOS |
| DSO PC | 192.168.10.10 | LAN1 | MMS client → :3782 | TSP / IEDScout |
| Plant sim | 192.168.30.10 | LAN3 | MMS server :102 | IED Simulator |
| Northbound MQTT | broker host:port | WAN/LTE | MQTT/TCP | TesPro `iec61850d` |

**Rule:** Port **3782** (DSO) ≠ port **102** (plant MMS) ≠ northbound **8883/1883** (MQTT).

---

## 2. Plant envelope parameters (O.8.2 — yaml `plant:`)

| Yaml key | TR 57-126 Table 1 | Unit | Example (Figura 2) |
|----------|-------------------|------|----------------------|
| `plant.p_imm_kw` | Max export P (Pfed) | kW | **200** |
| `plant.p_ass_kw` | Max import P (Pass) | kW | **200** |
| `plant.q_ind_kvar` | Max inductive Q | kVAr | **50** |
| `plant.q_cap_kvar` | Max capacitive Q | kVAr | **50** |
| `plant.smax_kva` | Nameplate Smax | kVA | **210** (authoritative) |

### Equation (1) — calculated Smax

\[
S_{\max,\mathrm{calc}} = \sqrt{\max(P_{\mathrm{imm}}^2, P_{\mathrm{ass}}^2) + \max(Q_{\mathrm{ind}}^2, Q_{\mathrm{cap}}^2)}
\]

**TR example:** \(\sqrt{200^2 + 50^2} =\) **206.15 kVA**  
**Product base:** `smax_kva_effective()` uses **`plant.smax_kva` = 210 kVA** when set.

**Code:** `apps/ccli/core/dso/plant_envelope.hpp` — `calc_smax_kva()`, `smax_kva_effective()`

---

## 3. DSO active-power parameters (O.9.2.x — yaml `dso:`)

| Yaml key | Annex T LN | Parameter | Range | Figura 2 TR |
|----------|------------|-----------|-------|-------------|
| `dso.w110_active` | (policy, not LN) | 110 % V limit gate | bool | false |
| `dso.w110_pct` | — | Export cap % Smax | 0..100 | — |
| `dso.wlim_active` | WlimDWMX1.Mod | Wlim activation | 1/5 | **Inactive** |
| `dso.wmax_spt_pct` | WlimDWMX1.WMaxSptPct | Gen limit % Smax | 0..100 | **0** |
| `dso.wsd_active` | WSdDAGC1.Mod | WSd activation | 1/5 | **Active** |
| `dso.wspt_pct` | WSdDAGC1.WSptPct | Feed-in/out % Smax | −100..+100 | **+20** |
| `dso.release_delta_kw` | — | PF2 release hysteresis | kW | **5** |

**Config gates:**

| Key | Meaning |
|-----|---------|
| `dso.enabled: true` | Run equation layer |
| `pf2.use_dso_mock: true` | Overwrite PF2 kW thresholds from DSO math |

### Equation (2) — percent Smax to kW

\[
P_{\mathrm{kW}} = \frac{\mathrm{pct}}{100} \times S_{\max,\mathrm{kVA}}
\]

**Figura 2 WSd:** \(P = 0.20 \times 210 =\) **42 kW** (using authoritative Smax)

**Code:** `pct_smax_to_kw()` in `plant_envelope.hpp`

### Equations (5)(6) — O.11 priority (export caps)

\[
P_{\mathrm{effective}} = \min(P_{110}, P_{\mathrm{Wlim}}, P_{\mathrm{WSd}}, \ldots)
\]

**Order (Phase 1 export arbiter):** W110 → Wlim → WSd → (WSa stub) → reactive (stub)

**Annex O Table 1 indices (normative — lower = higher priority):**

| Index | Function | Draw.io |
|-------|----------|---------|
| 1 | W110 O.9.2.1 | `Lim W110 O.9.2.1.drawio` |
| 2 | Wlim O.9.2.2 | `DSO_P_limit O_9_2_2.drawio` |
| 3 | WSd O.9.2.3 | `DSO_P_modulation_O_9_2_3_.drawio` |
| 4 | WSa P O.10.3.1 | `S.P.W.(MSD)...` |
| 5–7 | Reactive / MSD Q | not confirmed draw.io |

**Code simplification:** Phase 1 uses **min(kW caps)** only — not full index pre-emption from flowcharts.

**Figura 2:** only WSd active → **42 kW**

**Code:** `derive_active_power_kw()` in `dso_active_power.cpp`

---

## 4. Timing parameters (O.7 — Annex O)

| Parameter | CEI limit | Yaml / code (lab) | Status |
|-----------|-----------|-------------------|--------|
| **TsP** | ≤ 60 s to ±5 % of set-point | `settled_within_band_kw()` test | Unit test only |
| **TsQ** | ≤ 10 s (reactive) | — | NOT IMPL — see **Eq (7)(8)** reactive form §10 |
| **ΔT** | 10–600 s, default 60 s | — | NOT IMPL |
| Set-point spacing | ≥ 3 s (O.7.3.3) | — | NOT IMPL |
| PF1 / TotW blocks | **4 s** (O.8.3) | Modbus poll 1 s; RCB intgPd 4000 ms | PART (P3-03 lab) |
| Meter stale | O.13 safe state | `pf2.stale_data_s: 10` | IMPL |
| DSO comms loss | Annex U | `mms.comms_loss_fallback_s: 15` (lab) | IMPL (P3-05) |

### Equations (7)(8) — settling band

\[
\frac{|P - P_{\mathrm{exp}}|}{|P_{\mathrm{exp}}|} \leq 0.05 \quad \text{within } T_{sP} \leq 60\,\mathrm{s}
\]

**Code:** `settled_within_band_kw()` — unit test in `dso_phase1_test.cpp`

---

## 5. PF2 FSM parameters (yaml `pf2:`)

| Yaml key | Meaning | Legacy lab | TR Figura 2 mock |
|----------|---------|------------|------------------|
| `pf2.threshold_kw` | Enter curtailment when P > | 900 | **42** (from DSO apply) |
| `pf2.release_threshold_kw` | Release when P ≤ | 850 | **37** |
| `pf2.debounce_s` | Sustained exceed before DIO | 30 | 30 |
| `pf2.min_on_s` / `min_off_s` | Minimum pulse widths | 60 / 30 | 60 / 30 |
| `pf2.stale_data_s` | No valid meter → safe | 10 | 10 |

### Equations (13)–(17) — lab FSM

| ID | Behaviour | Code |
|----|-----------|------|
| L01 | `P_meas > threshold_kw` → curtailment request | `pf2_fsm.cpp` |
| L02 | Debounce `debounce_s` before asserting DIO | same |
| L03 | Release at `release_threshold_kw` (hysteresis) | same |
| L03b | Stale meter > `stale_data_s` → safe / no false valid | same |
| L04 | Permissive DI before DO (Phase 1 bench) | `ccli_main` + DIO2 |

### Equation (18) — lab demo ramp

\[
P = 400 + 550\left|1 - \left|\frac{\phi}{60} - 1\right|\right|, \quad \phi = t \bmod 120
\]

**Trigger:** `ccli --lab-demo` or `modbus_rtu_slave.py --ramp`

---

## 6. IEC 61850 object parameters (Annex T)

**IED:** `CCI016_01` · **LD:** `LD_Plant` · **CID:** `apps/ccli/config/icd/lab_tg544_eth_a.cid`

| Object | MMS path | FC | Period / note |
|--------|----------|-----|---------------|
| TotW @ PdC | `LD_Plant/PdCMMXU1.TotW` | MX | RCB `urcb_PdC_Mis4sec` **intgPd=4000 ms** |
| Wlim set-point | `LD_Plant/WlimDWMX1.WMaxSptPct` | CO | SBO / lab DIRECT_ENHANCED |
| WSd set-point | `LD_Plant/WSdDAGC1.WSptPct` | CO | Figura 2 green path |
| TotVAr / PPV | `PdCMMXU1.TotVAr`, `.PPV` | MX | **Phase 5** — STUB publish |

**Report controls (CID):**

| RCB | Type | intgPd | DataSet |
|-----|------|--------|---------|
| `urcb_PdC_Mis4sec` | unbuffered | 4000 ms | PdC measurements |
| `urcb_GenAcc_Mis4sec` | unbuffered | 4000 ms | Gen/accum |
| `urcb_SingGen_Mis4sec` | unbuffered | 4000 ms | Single generator |
| `brcb_Stato_Allarmi_Segnali` | buffered | 0 | Alarms/status |

Full map: `apps/ccli/config/icd/signal_map.yaml`

---

## 7. Modbus parameters (plant meter — Phase 1)

| Parameter | Value |
|-----------|--------|
| Device | `/dev/rs485_2_uart` (lab TG544) |
| Baud / parity | 9600 8N1 |
| Slave ID | 1 |
| P register | **40001** float32 BE → `MeasurementStore.p_kw` |
| Poll interval | 4000 ms (yaml `modbus.poll_ms`) |

---

## 8. TesPro IEC 61850 (supplier — P4-00)

| Parameter | Value | Note |
|-----------|-------|------|
| Southbound MMS port | **102** | Target plant IED |
| Northbound | MQTT/TCP user-defined | Not MMS |
| ICD upload | Per **remote** device | Not DSO CID unless target runs that model |
| Packages | libopen62541, libiec61850, mmsd, proto-combined | apk on TesproOS |

**Gap vs `ccli`:** No DSO server :3782, no 104, no PF2 — see `lab/evidence/phase4/P4_00_TESPRO_61850_SUPPLIER_SW.md`

---

## 9. Master regulation map (R01–R15 + L01–L04)

Aligned with `regulation_map.hpp` after audit (see `FLOWCHARTS_CROSSCHECK_AUDIT.md` §4).

| ID | Clause | Equation | Status | Notes |
|----|--------|----------|--------|-------|
| R01 | O.8.2 Smax | (1) | **PASS** | |
| R02 | O.9.2.2 Wlim | (2) | **PASS** | yaml + MMS live P3 |
| R03 | O.9.2.3 WSd | (2)(4) | **PASS** | TR 20 % → 42 kW |
| R04 | O.9.2.1 W110 | (2) | **PART** | yaml; no V meter |
| R05 | O.11 | (5)(6) | **PASS** | min() caps only |
| R06 | O.7.3.1 | (7)(8) | **PASS** | unit test only |
| R07 | O.7.3.3 | (9) | **Na** | not implemented |
| R08 | O.13.1.2 | — | **PART** | stale meter; not full Annex U |
| R09 | O.8.3 TotW | — | **PART** | P3-03 lab; poll 1 s vs 4 s block |
| R10 | O.9.1.3 VArV | (10) | **Na** | draw.io only |
| R11 | O.9.1.2 PFW | (11) | **Na** | draw.io only |
| R12 | O.9.1.1 PFSP | (12) | **Na** | draw.io only |
| R13 | O.9.1.4 / O.10.3.2 VArSd/VArSa | **(3)** | **Na** | direct Q SP; Phase 5-R01 / P5-R08 |
| R14 | O.14 logger | — | **PART** | EventRing RAM |
| R15 | Annex M teletrip | — | **Na** | |
| L01–L03 | Lab FSM | (13)–(17) | **PASS** | |
| L04 | Lab ramp | (18) | **PASS** | |

Source of truth in code: `apps/ccli/core/dso/regulation_map.hpp` → `kRegulationMasterMap[]`

---

## 10. Reactive equations (Phase 5 — flowcharts not confirmed)

Documented in draw.io; **not implemented** in `core/dso/` (see `Reactive_Equation_Dataflow.mermaid`).

### Equation (3) — percent Smax to kVAr (VArSd / VArSa)

Direct DSO / MSD reactive set-point — **active-power analog of Eq (2)**:

\[
Q_{\mathrm{kVAr}} = \frac{\mathrm{VArSptPct}}{100} \times S_{\max,\mathrm{kVA}}
\]

| Item | Value |
|------|-------|
| **Annex T LN** | `VArSdDVAR1.VArSptPct` (Table 88, Eth_A) · `VArSaDVAR1.VArSptPct` (Table 89, Eth_B MSD) |
| **Sign** | **+** capacitive / **−** inductive at PdC (CEI Table 88/89) |
| **Range** | **−100…+100** % Smax (Annex T: 0..100 with signed feed-in/out semantics) |
| **O.11 index** | **5** (VArSd) · **7** (VArSa / MSD Q) |
| **Trigger** | **Direct tracking** — no slow-ring ΔT formula (unlike Eq (10)(11)) |
| **Figura 2 TR** | VArSd **Inactive** → default **0 / 0** |
| **Code (planned)** | `pct_smax_to_kvar()` mirroring `pct_smax_to_kw()` in `plant_envelope.hpp` |
| **MMS today** | STUB Mod=5 — `signal_map.yaml` |

**Draw.io:** `V.ctrl.Q.on.DSO.NotConfirmed.drawio` · `S.P.Q(MSD).Not. confirmed.drawio` — target node “0..100% of Smax”.

### Equations (7)(8) — reactive settling (TsQ)

Same ±5 % band as active **TsP**, with **TsQ ≤ 10 s** (O.7.3.1):

\[
\frac{|Q - Q_{\mathrm{exp}}|}{|Q_{\mathrm{exp}}|} \leq 0.05 \quad \text{within } T_{sQ} \leq 10\,\mathrm{s}
\]

**Code:** `settled_within_band_kvar()` — **NOT IMPL** (P band helper exists: `settled_within_band_kw()`).

### Function map

| Function | Flowchart file | Key parameters / equations |
|----------|----------------|----------------------------|
| **VArSd** DSO Q | `V.ctrl.Q.on.DSO.NotConfirmed.drawio` | Eth_A O.9.1.4; O.11 index **5**; **Eq (3)** |
| **VArSa** MSD Q | `S.P.Q(MSD).Not. confirmed.drawio` | Eth_B O.10.3.2; O.11 index **7**; **Eq (3)** |
| **Q(V)** VArV | `Q(V).not confirmed.drawio` | lock-in **0.20×Pn**, lock-out **0.05×Pn**; **Vpu=V/Vn**; **K** piecewise; **Eq (10)** |
| **cosφ(P)** PFW | `Cosfi(P).not confirmed.drawio` | **P_A/B/C**, cosφ_A/B/C; **δcosφ=0.02**; **Eq (11)** |
| **cosφ SP** PFSP | `cosfi. not confirmed.drawio` | Mod 1/5; Fst Autonomous; **Eq (12)** |
| **WSa P** MSD | `S.P.W.(MSD).not.Confirmed.drawio` | Eth_B O.10.3.1; O.11 index **4**; uses **Eq (2)** (active) |

### Equation index (Annex T — Phase 5)

| Eq | Description | Status |
|----|-------------|--------|
| **(3)** | **VArSptPct → kVAr** (VArSd / VArSa direct Q) | draw.io + Annex T; **no code** |
| **(9)** | Min **3 s** between external set-point updates (O.7.3.3) | NOT IMPL |
| **(10)** | Q(V) characteristic + δQ band | draw.io only |
| **(11)** | cosφ(P) piecewise + δcosφ | draw.io only |
| **(12)** | PFSP direct cosφ set-point | draw.io only |

---

## Related files

| Path | Role |
|------|------|
| `lab/CCI_Figura2_Parameters.md` | TR Table 1 narrative |
| `lab/PF2_REGULATION_PHASE1.md` | Phase 1 equation master |
| `lab/PHASE1_REGULATION_CHECK_MATRIX.md` | Pass/fail matrix |
| `Architecture/mermaid/CCLI_Fig02_Phases_Regulations.mermaid` | Phase vs regulation diagram |

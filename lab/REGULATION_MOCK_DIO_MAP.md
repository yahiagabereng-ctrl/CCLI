# Regulation mock → DIO action map (Phase 1 lab)

**Document ID:** CCLI-LAB-MOCK-DIO-MAP-001  
**UI:** `http://192.168.1.130/ccli/mock_console.html`  
**Config on DUT:** `/etc/ccli/lab.yaml`  
**Formula:** `DIO1 ON ⇔ PF2.curtailment_commanded ∧ permissive OK` (DIO2 closed / 12 V key)

---

## 1. Where to change values

| What you change | Mock UI control | YAML section | Affects (regulation) |
|-----------------|-----------------|--------------|----------------------|
| Plant corners | P_imm, P_ass, Q_*, Smax | `plant:` | **R01** S_calc, **TR-T1** |
| WSd export cap | WSd on, WSptPct % | `dso:` | **R03**, **R05**, **L01** enter kW |
| Wlim cap | Wlim on, WMaxSptPct % | `dso:` | **R02**, **R05**, **L01** |
| 110 % V cap | w110 on, w110_pct % | `dso:` | **R04**, **R05** |
| Release hysteresis | release_delta kW | `dso:` | **L02** release kW |
| Enable equation path | dso.enabled, use_dso_mock | `dso:` + `pf2:` | **CFG**, **MOCK** |
| Enter debounce | debounce_s | `pf2:` | **L01** time to DIO ON |
| Comm fail-safe | stale_data_s | `pf2:` | **R08**, **L03** → DIO OFF |
| Modbus period | poll_ms | `modbus:` | **R09** |
| Skip permissive (lab only) | permissive_bypass | `io:` | **IO-byp** — DIO follows PF2 only |
| **Measured P (not yaml)** | PC `modbus_rtu_slave.py --power-kw` | — | **LIVE-P**, PF2 FSM, **DIO** |

After edits: **Apply mock → TG544 & restart ccli** in the UI (or `/etc/init.d/ccli restart`).

---

## 2. Computed thresholds (Figura 2 preset)

With **WSd 20 %**, **Smax 210 kVA**, **Wlim/w110 off**, **use_dso_mock on**:

| Symbol | Value | Clause |
|--------|-------|--------|
| S_calc | ~206.16 kVA | R01 |
| p_wsd / p_effective | **42 kW** | R03, R05 |
| PF2 **enter** | **42 kW** | L01 |
| PF2 **release** | **37 kW** (Δ5) | L02 |
| Debounce | **30 s** before curtail commanded | L01 |
| Stale | **10 s** without good Modbus → safe OFF | L03, R08 |

Legacy preset (**use_dso_mock off**): enter **900 kW**, release **850 kW**.

---

## 3. DIO action map (every lab outcome)

| # | You set / do | PF2 state (typ.) | Curtail commanded | Permissive (DIO2) | **DIO1** | Log / reason |
|---|----------------|------------------|-------------------|-------------------|----------|--------------|
| 1 | P ≤ enter (e.g. slave **40 kW**) | Normal | no | OK | **OFF** | `ok` |
| 2 | P > enter, t < **debounce_s** | CurtailPending | no | OK | **OFF** | `debounce` |
| 3 | P > enter, t ≥ **debounce_s** | CurtailActive | **yes** | OK | **ON** | `p_above_threshold`, `curtail_on` |
| 4 | PF2 ON, **open permissive** (DIO2) | CurtailActive | yes | **BLOCKED** | **OFF** | `curtail_blocked_permissive` |
| 5 | P < release after min-on/off rules | Normal | no | OK | **OFF** | `released`, `curtail_off` |
| 6 | **Stop** PC Modbus slave > **stale_data_s** | SafeState | no | — | **OFF** | `stale_or_invalid` |
| 7 | Good Modbus returns | Normal | no | OK | **OFF** | `recovered` |
| 8 | Boot / `ccli` restart | — | no | — | **OFF** | safe default |
| 9 | `permissive_bypass: true` (lab) | as per P | yes/no | forced OK | **= PF2 cmd** | bypass warning in log |

**Watch live:** mock UI tiles **DIO1**, **Poll live**, or `logread -f | grep -E 'io:|modbus:'`.

---

## 4. Bench scenarios (see every DIO action)

| Goal | Mock UI / yaml | PC Modbus slave | Physical | Expected DIO |
|------|----------------|-----------------|----------|--------------|
| **A. Curtail ON** | Preset Figura 2, Apply | `--power-kw 45` | Close DIO2 key (12 V) | **ON** after **30 s** |
| **B. Curtail blocked** | Same | `--power-kw 45` | **Open** permissive | **OFF** (PF2 may still command) |
| **C. No curtail** | Same | `--power-kw 40` | Perm OK | **OFF** |
| **D. Stale safe** | stale 10 s | **Stop** slave | — | **OFF** within ~10 s |
| **E. Legacy 900 kW** | Preset Legacy, Apply | `--power-kw 950` | Perm OK | **ON** after 30 s |
| **F. Higher cap (O.11)** | Preset O.11 custom | tune P above **105 kW** | Perm OK | ON after debounce |

---

## 5. Regulation validation vs DIO

| Clause group | Validated without DIO motion | Needs DIO / live |
|--------------|------------------------------|------------------|
| R01–R06, R08–R09, R14, L01–L04, TR-T1, CFG | **Validate all (static)** in UI | — |
| LIVE-P, LIVE-MB, LIVE-CONF-R03/R05/R06 | — | Modbus slave + **Validate all (live)** |
| **LIVE-DIO**, **IO**, **L03 stale** | — | Scenarios **A–D** above |
| R07, R10–R13, R15, REQ-61850 | N/A Phase 1 | — |

---

## 6. Quick commands (SSH)

```bash
# Live snapshot (same as UI poll)
/usr/sbin/ccli --bench-status --json

# Full clause list
/usr/sbin/ccli --regulation-check --config /etc/ccli/lab.yaml
/usr/sbin/ccli --regulation-check --live --live-watch 8 --config /etc/ccli/lab.yaml

# DIO manual (GD32)
ubus call dido_v2 set_relay '{"channel":1,"state":1}'
ubus call dido_v2 gd32.status
```

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-22 | Operator map for mock UI + DIO scenarios |

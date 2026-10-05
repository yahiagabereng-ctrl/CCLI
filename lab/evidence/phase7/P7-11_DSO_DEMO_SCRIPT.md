# P7-11 — K7.5 DSO demonstration script (client-facing)

**Gate:** P7-11  
**Date:** 2026-10-02  
**Audience:** DSO / connection engineer (non-developer)  
**Build:** 0.1.0-r27  
**Lab profile:** `lab_tr400_cleartext_tsp_ttyS1.yaml` (cleartext `:102` for IEDExplorer)  
**Product profile:** `lab_tr400_phase4_eth_b.yaml` (TLS `:3782` — use for final client demo if required)  
**Prior art:** [P5_TSP_FULLCIRCLE_SEQUENCER.md](../phase5/P5_TSP_FULLCIRCLE_SEQUENCER.md)

---

## Purpose

Demonstrate **active power export limitation** per **O.9.2.2 Wlim** — the primary DSO-visible control function for PF2 plants ≥100 kW.

Expected outcome: DSO sets `WlimDWMX1` → CCI reduces export (curtail DO) → measurements visible on `PdCMMXU1.TotW`.

---

## Preconditions

| Item | Value |
|------|-------|
| DUT IP | `192.168.10.1` |
| MMS port (lab) | `102` cleartext |
| MMS port (product) | `3782` TLS |
| IED name | `CCI016_01` |
| Modbus plant sim | P ≈ **450 kW** (below PF2 yaml threshold until Wlim applied) |
| Client tool | **IEDExplorer** or Test Suite Pro Advanced Client |
| CID reference | `apps/ccli/config/icd/lab_tg544_eth_a.cid` |

Check DUT:

```text
logread | tail -5          # ccli running
chronyc tracking           # |offset| ≤ 100 ms
```

---

## Demo sequence (≈15 min)

| Step | Action | Object / command | Expected result |
|------|--------|------------------|-----------------|
| 1 | Connect | IEDExplorer → `192.168.10.1:102` | Association OK |
| 2 | Read baseline | `PdCMMXU1.TotW` | ≈450 kW |
| 3 | Read Wlim mode | `WlimDWMX1.Mod` | **5** (off) @ baseline |
| 4 | Explain | Show `PfCMMXU1` / PF2 thresholds in config | Enter threshold ≈900 kW (yaml) |
| 5 | **Direct Operate Wlim** | `WlimDWMX1.Mod=1`, `WMaxSptPct=10` | Operate success |
| 6 | Observe export cap | Read `PdCMMXU1.TotW` after 5–10 s | P drops toward **10% × Smax** (~21 kW lab mock) |
| 7 | Observe DO | DUT shell: `ubus call dido_v2 get_relay` | Curtail relay **ON** (DIO1) |
| 8 | Event log | `ccli --config /etc/ccli/lab.yaml --event-dump --count 10` | Line `mms wlim_on` with O.14 timestamp |
| 9 | Release Wlim | `WlimDWMX1.Mod=5` | Mod off; P returns toward plant set-point |
| 10 | Optional WSd | `WSdDAGC1.Mod=1`, `WSptPct=20` | Secondary limit per O.9.2.3 |

---

## Talking points (K7.5)

1. **Separation:** Eth_A (DSO MMS) vs Eth_B (operator IEC 104) vs LTE Annex M (defence trip) — three independent paths.
2. **Operating Rule:** Disconnect MMS 15 s → `Wlim`/`WSd` revert to autonomous yaml thresholds (P3-05).
3. **Annex M:** SMS `TRIP` → DIO3 → PF2 curtail **inhibited** (O.11) — show only if client asks defence plan.
4. **Evidence:** Event log is **read-only** export; 2048-event ring (`--event-wrap-test` on request).

---

## Pass criteria

| Criterion | Pass |
|-----------|------|
| Wlim Operate accepted | Client sees OK in IEDExplorer |
| TotW reflects limitation | Visible reduction within one Modbus poll |
| Curtail DO energizes | ubus / LED on terminal block |
| Event logged | `wlim_on` in `--event-dump` |

---

## Evidence capture

Save to `lab/evidence/phase7/`:

- `P7-11_DSO_DEMO_YYYY-MM-DD.txt` — copy/paste IEDExplorer reads + `--event-dump` tail
- Screenshot: Wlim Operate + TotW before/after

**Operator script (automated):** extend `run-p5-fullcircle-cleartext.ps1` steps 6–8 only for repeatability.

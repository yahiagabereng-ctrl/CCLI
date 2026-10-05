# P5-G03 — Plant measurement merge policy (Modbus vs GOOSE)

**Date:** 2026-10-05  
**Gate:** P5-G03 (policy)  
**Build reference:** ccli **0.1.0-r29**  
**Status:** **PASS** (policy documented) · runtime merge **NOT IMPLEMENTED** (tracked below)

---

## Requirements

| ID | Requirement | Source | Policy |
|----|-------------|--------|--------|
| REQ-P5-G03-001 | Single authoritative plant P/Q path for MMS URCB + DSO reports | O.8 / P5-M | **Modbus RTU** is authoritative in lab and default product |
| REQ-P5-G03-002 | Optional GOOSE subscribe must not silently override Modbus without config | Annex T Type 1 | Per-signal `bind_kind` + explicit primary source |
| REQ-P5-G03-003 | GOOSE RX events logged even when not merged | O.14 | **HAVE** — `GooseAdapter` → `EventStore` category `goose` |
| REQ-P5-G03-004 | Reactive command (VArSd) remains Modbus FC16 until plant GOOSE Q map exists | P5-R01 / O.9.1.4 | Modbus write path unchanged |
| REQ-P5-G03-005 | Publish path (P5-G02) uses same MMS values as DSO reports | P5-G02 | Modbus → `MeasurementStore` → `MmsAdapter` → GoCB dataset |

---

## Architecture

### Functional (today, r29)

```
Plant RS485 (Modbus RTU) ──► ModbusAdapter.poll() ──► MeasurementStore
                                                              │
                                                              ▼
                                                    ccli_main main loop
                                                              │
                    ┌─────────────────────────────────────────┼─────────────────────────┐
                    ▼                                         ▼                         ▼
            MmsAdapter.update_tot_w/var              IedServer URCB reports      IedServer GOOSE publish
            (DSO Eth_A :102/:3782)                   (4 s integrity)             (lan3, P5-G02 PASS)

Plant LAN (GOOSE subscribe) ──► GooseAdapter ──► stderr + O.14 event ONLY
                                 (when goose.enabled=true)     (no MeasurementStore write today)
```

### Network

| Domain | Interface | Role in merge |
|--------|-----------|---------------|
| Eth_A (LAN1) | MMS server | Exposes merged result (today = Modbus-only) |
| Plant RS485 | `/dev/ttyS1` lab / A2/B2 product | **Primary** P/Q input |
| Plant LAN (LAN3) | GOOSE L2 | **Optional** parallel input; subscribe only when `goose.enabled=true` |

### Security / isolation

- GOOSE RX binds to **plant interface only** (`lan3` on TR544) — P2-G01.
- Subscribed GOOSE must **not** bridge to DSO zone or alter MMS bind on Eth_A.
- Merge policy is configuration-driven (`GOOSE_DATA_MAP.csv` / future yaml); no hard-coded cross-domain forwarding.

---

## Merge rules (normative for implementation)

### Rule 1 — Default primary source

| Signal group | Primary source | MMS path | GOOSE map (if any) |
|--------------|----------------|----------|-------------------|
| POC active power | Modbus | `PdCMMXU1.TotW` | Publish only (outbound APPID 0x1000) |
| POC reactive power | Modbus | `PdCMMXU1.TotVAr` | Publish only |
| POC voltage | Config / Modbus | `PdCMMXU1.PPV` | Lab: yaml `plant.poc_ppv_kv`; field: meter map P5-06 |
| POC current | Modbus (when wired) | `PdCMMXU1.A` | Deferred until A wired |
| Alarm / status ST | MMS model static + future DI | `DS_R_Stato_Allarmi` members | GOOSE publish APPID 0x1001 |
| External bay GOOSE (Berlin matrix) | **Subscribe** when enabled | Per `mms_path` in CSV | `bind_kind=goose_sub` |

**Lab default:** `apps/ccli/config/lab_tr400_cleartext_tsp_ttyS1.yaml` — Modbus on `/dev/ttyS1`, `goose.enabled: false`.

### Rule 2 — When both Modbus and GOOSE provide the same quantity

Use **per-signal priority** from plant map:

| Priority | Condition | Action |
|----------|-----------|--------|
| 1 | Modbus `quality == Good` and primary=`modbus` | Use Modbus value |
| 2 | Modbus stale/invalid and GOOSE `valid` and primary=`goose` or `goose_fallback` | Use GOOSE value; set quality from GOOSE dataset q |
| 3 | Both stale/invalid | Keep last good with `quality=Stale`; log `meter quality_stale` |
| 4 | Conflict (both Good, values diverge > threshold) | Keep primary; log `goose merge_conflict` O.14 event |

**Thresholds (implementation backlog):**

- `|ΔP| > 0.5 kW` or `|ΔQ| > 0.5 kvar` between sources → conflict event, no silent override.

### Rule 3 — GOOSE subscribe without merge (current r29 behaviour)

When `goose.enabled=true`:

1. `GooseService` receives frames on `goose.interface`.
2. Logs: `goose: rx appId=… stNum=…` (stderr + O.14).
3. **Does not** call `MeasurementStore::update()`.
4. MMS TotW/TotVAr continue from Modbus loop in `ccli_main.cpp`.

This is intentional until P5-G03 implementation lands.

### Rule 4 — GOOSE publish (P5-G02)

Publish dataset reflects **MMS model values** updated from Modbus each poll cycle:

```text
modbus.tick() → MeasurementStore → mms.update_tot_w_kw / update_totvar_kvar → IedServer dataset → GoCB TX on lan3
```

External subscribers see the same numbers as DSO URCB (validated 2026-10-05).

### Rule 5 — Reactive path (unchanged)

| Path | Mechanism | Merge interaction |
|------|-----------|-------------------|
| DSO VArSd Operate | MMS → `modbus.write_reactive_kvar()` | Independent of GOOSE subscribe |
| Plant Q from inverter GOOSE | **Not implemented** | Future: map only if `bind_kind=goose_sub` on Q signal |

---

## Interface matrix

| Interface | Source | Destination | Protocol | Merge role |
|-----------|--------|-------------|----------|------------|
| RS485 A1/B1 (lab) | PC Modbus slave / inverter | `MeasurementStore` | Modbus RTU | **Primary P/Q** |
| LAN3 (plant) | External IED GoCB | `GooseAdapter` | GOOSE L2 | Optional RX; merge TBD |
| LAN3 (plant) | DUT GoCB | Wire / peer IED | GOOSE L2 | **Export** (P5-G02) |
| LAN1 (DSO) | DSO client | `MmsAdapter` | MMS | Read merged P/Q |
| Internal | `MeasurementStore` | `MmsAdapter` + `Iec104Adapter` | — | Single snapshot per tick |

---

## Configuration references

| Artifact | Purpose |
|----------|---------|
| `apps/ccli/config/plant/GOOSE_DATA_MAP.csv` | Wide plant GOOSE matrix; rows 68–71 = CCLI publish IMPL |
| `apps/ccli/config/plant/generated/goose_subscribe.yaml` | Generated publish dataset members |
| `apps/ccli/config/icd/signal_map.yaml` | MMS LN bindings; Modbus refs for VArSd |
| `apps/ccli/config/lab_tr400_cleartext_tsp_ttyS1.yaml` | Lab: Modbus + publish on `lan3`, subscribe off |
| `apps/ccli/config/lab_tr400_goose_plant.yaml` | Template for P5-G01 subscribe (fix `interface: lan3` before deploy) |
| `lab/GOOSE_ADAPTATION.md` | Protocol map + lab gates |

**CSV `bind_kind` values:**

| bind_kind | Meaning |
|-----------|---------|
| `mms_int` | Internal MMS/GOOSE publish from Modbus-driven model |
| `goose_sub` | Subscribe from external publisher → future merge target |
| (empty) | Berlin template / STUB |

---

## Knowledge gaps

| Area | Current (r29) | Required | Gap |
|------|---------------|----------|-----|
| Runtime merge | None | `GooseAdapter` → parser → `MeasurementStore.update()` | **OPEN** — software |
| Per-signal primary flag | CSV `bind_kind` only | Yaml `plant_source.primary: modbus\|goose` | **OPEN** — config schema |
| GOOSE timeout | RX logged only | O.14 plant comms loss + quality stale | P5-G04 |
| Sun2000 / inverter GOOSE profile | STUB rows in CSV | Site SCD GoCB ref | Field engineering |
| PPV from GOOSE | Not mapped | Optional MMTR IED | P5-06 meter map |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-G03-01 | Silent GOOSE override of Modbus | Wrong DSO reports / regulation | Rule 2: explicit primary + conflict event |
| R-G03-02 | Dual-path divergence undetected | Certification fail O.8 | Conflict threshold + O.14 logging |
| R-G03-03 | Subscribe enabled without publisher | False stale if merge treats GOOSE as primary | Default primary=modbus; GOOSE fallback only when configured |
| R-G03-04 | `lab_tr400_goose_plant.yaml` uses `br-lan` | Subscribe bind fails on TR544 | Deploy with `interface: lan3` (same as P5-G02) |

---

## Verification

| Requirement | Test | Pass criteria | Status |
|-------------|------|---------------|--------|
| REQ-P5-G03-001 | Read policy + code review | Modbus → `MeasurementStore` documented | **PASS** (this doc + `ccli_main.cpp`) |
| REQ-P5-G03-003 | P5-G01 when LAN3 available | `goose` event in `--event-dump` | **OPEN** (P5-G01) |
| REQ-P5-G03-005 | P5-G02 wire + MMS | TotW in GOOSE = TotW in URCB | **PASS** (2026-10-05 pcap) |
| Runtime merge | Inject GOOSE + Modbus conflict | Primary wins + conflict event | **OPEN** (implementation) |

---

## Implementation backlog (post-policy)

1. **`GooseAdapter::on_goose_received`** — decode dataset members per `goose_subscribe.yaml` / CSV.
2. **`PlantMergeService`** (or extend `ModbusService`) — apply Rule 2; write `Measurement.source` (`"modbus"` / `"goose"`).
3. **Config** — `plant.merge:` section: primary, fallback, conflict thresholds.
4. **P5-G04** — GOOSE RX timeout → `goose link_down` + meter stale.
5. **Lab evidence** — `P5_G03_MERGE_LAB_*.txt` when LAN3 returns (dual-source bench).

---

## Related gates

| Gate | Status | Notes |
|------|--------|-------|
| P5-G02 publish | **PASS** | Outbound GOOSE from Modbus-driven MMS |
| P5-G01 subscribe | **OPEN** | Blocked on LAN3 bench |
| P5-G03 policy | **PASS** | This document |
| P5-G03 runtime | **OPEN** | Implementation backlog above |
| P5-R01 VArSd | **PASS** | Modbus Q write unaffected |

---

*Policy anchor for P5-G03. Update when runtime merge is implemented or P5-G01 lab evidence is captured.*

# REGALGRID SNOCU CCI vs HiTEKS CCLI — interface parity matrix

**Document ID:** FIELD-SNOCU-CCLI-MAP-001  
**Date:** 2026-09-30  
**Companion:** `REGALGRID_SNOCU_HMI_INGEST.md` · `lab/evidence/phase5/P5_LN_VALUE_MATRIX.md`  
**CID:** `apps/ccli/config/icd/lab_tg544_eth_a.cid`  
**Lab config:** `apps/ccli/config/lab_tr400_cleartext_tsp.yaml`

---

## Requirements

| ID | Requirement |
|----|-------------|
| **REQ-UI-001** | CCLI operator surface shall expose the same **plant topology** (DG / DDI / DDG) and POC P/Q as field SNOCU. |
| **REQ-UI-002** | DSO setpoints visible on SNOCU page 2/13 (**P.Limit**, **P.Sp**, **Q.Sp**) shall map to MMS **`WlimDWMX1`** and **`WSdDAGC1`**. |
| **REQ-UI-003** | POC measurements on SNOCU pages 1/3/4 shall map to **`PdCMMXU1$MX$TotW`**, **`TotVAr`**, **`PPV`**, per-phase extensions. |
| **REQ-UI-004** | Field alarm classes (DSO com, Modbus DSO, time sync) shall be verifiable on CCLI; battery/e-mail alarms are out of lab scope until hardware present. |
| **REQ-NET-001** | Eth_A (DSO trusted) on CCLI remains **`192.168.10.1`**; field **`192.168.1.1`** is role-equivalent, not IP-clone. |
| **REQ-NET-002** | Field Eth2 (**10.99.99.205** + AirGate) and Eth3 Modbus-TCP (**10.99.99.210**) documented as **Phase 7** gaps unless DSO mandates earlier. |

---

## Architecture

### Functional

| SNOCU function | CCLI equivalent | Parity |
|----------------|-----------------|--------|
| Central plant controller | `ccli-bin` on TG544 | **PART** — MMS/Modbus wired; no local SLD HMI |
| DSO Annex T MMS | Eth_A `:3782` TLS (product) / `:102` cleartext (lab) | **IMPL** |
| Operator 104 | Eth_B `:2404` | **IMPL** (P4) |
| RS485 Modbus fieldbus | `/dev/rs485_2_uart` | **IMPL** (P5) |
| Built-in REGALGRID HMI | SSH + OpenWrt LuCI | **GAP** — different UX |
| External Weintek SCADA | not in CCLI scope | **N/A** |

### Hardware

| Block | SNOCU (field) | CCLI TG544 |
|-------|---------------|------------|
| HMI panel | REGALGRID integrated LCD + keypad | None — headless gateway |
| Ethernet ×3 | ETH1 trusted, ETH2 WAN/AirGate, ETH3 Modbus-TCP | Eth_A DSO, Eth_B operator, 2× plant ports (TesPro) |
| RS485 | 9600 8N1, server addr 1 | RS485_2 lab wiring |
| DI/DO | 12+12 labelled | TG544 DI/DO — partial PF2 wiring |

### Network

| Interface | SNOCU field | CCLI lab | Notes |
|-----------|-------------|----------|-------|
| Trusted / DSO | 192.168.1.1/24 | 192.168.10.1/24 | Same **role** (Annex T bind) |
| Operator | (not shown on Eth pages) | 192.168.1.130/24 | CCLI adds explicit Eth_B |
| Untrusted WAN | 10.99.99.205 + AirGate | not configured | LTE backup path TBD |
| Modbus IP | 10.99.99.210 | N/A (RTU) | Protocol same, transport differs |
| Weintek WAN | 10.99.99.112 | N/A | Plant HMI only |

### Security

| Topic | SNOCU | CCLI |
|-------|-------|------|
| DSO MMS | Unknown TLS profile on field unit | TLS `:3782` + IEC 62351-4 (P3) |
| ETH1 firewall | Disabled | OpenWrt zones — **stricter** in lab product yaml |
| Direct telnet :23 | Enabled on ETH1 setpoint | **Disabled** on CCLI (SSH only) |
| Access lock | Optional | RBAC / cert-based MMS |

### Power

Field Basic Settings: **1200 kW** nominal import, **20 kV** Ph-Ph, **50 Hz**.  
CCLI lab: **210 kVA** (`plant.smax_kva`), **20 kV** PPV yaml, **50 Hz** assumed — scale differs, voltage class **matches**.

---

## Interface matrix

### HMI page → MMS / config

| SNOCU screen | Primary data | CCLI source | Status |
|--------------|--------------|-------------|--------|
| 1 Impianto — GRID P/Q | −4 kW, 1 kVAr | `PdCMMXU1$MX$TotW`, `TotVAr` ← Modbus 40001/40003 | **IMPL** (values differ) |
| 1 Impianto — LOAD | −3 kW | derived / future load LN | **DEFERRED** |
| 1 Impianto — PV P.Req | 80 kW | `GenPV` / `SSGG` LNs in CID | **STUB** |
| 2 DSO — P.Limit | 27% | `WlimDWMX1` set-point DO | **PART** |
| 2 DSO — P.Sp | 8% | `WSdDAGC1` ← yaml mock 20% lab | **PART** |
| 2 DSO — Q.Sp | 0% | reactive LNs (RBRF/RREC/…) | **STUB** |
| 3 Misure gauge | −4 kW | same as TotW | **IMPL** |
| 4 Misure dirette | V, I, f, Ubat | PPV + future meter map P5-06 | **PART** |
| 5 Misure calcolate | per-phase P/Q/PF | CID has PdC MMXU; per-phase **DEFERRED** | **PART** |
| 8 DI Stato DG/DDI | 1/1 | `DisFR*`, breaker LNs | **STUB** |
| 9 Statistiche | kWh counters | MMTR / billing LNs | **DEFERRED** |
| 10–12 Ethernet | IP/MAC/AirGate | OpenWrt UCI / `network` | **PART** (LuCI only) |
| Alarmlist | 5 warnings | event engine | **PART** |

### Setpoints → CID / yaml

| SNOCU setpoint | Value (field) | CCLI bind | Status |
|----------------|---------------|-----------|--------|
| Controller Name | CCI0CN12GM | `IED` name in CID | **MATCH** (different string OK) |
| Nominal Mains Import | 1200 kW | `plant.smax_kva` 210 | **GAP** (scale) |
| Bus/Mains V Ph-Ph | 20000 V | `plant.poc_ppv_kv: 20.0` | **MATCH** |
| RS485 Modbus Speed | 9600 8N1 | modbus adapter yaml | **MATCH** |
| System Baseload | 1000 kW | no yaml key | **MISSING** |
| Load Ramp | 200 s | no yaml key | **MISSING** |
| Q Ramp / Q Deadband | 10 s / 2% | grid code engine | **MISSING** |

---

## BOM matrix

| Block | Function | SNOCU field | CCLI candidate | Status |
|-------|----------|-------------|----------------|--------|
| CCI CPU | Annex T + plant logic | REGALGRID SNOCU internal | TG544 + `ccli-bin` | **HAVE** (lab) |
| HMI | Local operator UI | REGALGRID panel | LuCI / future web SLD | **GAP** |
| DSO port | MMS server | ETH1 192.168.1.1 | Eth_A 192.168.10.1:3782 | **HAVE** |
| Remote access | AirGate | ETH2 | LTE OpenWrt | **PART** |
| Fieldbus | Modbus | RS485 + ETH3 TCP | RS485_2 RTU | **PART** |
| Plant SCADA | Weintek HMI | Energy Synt | — | **N/A** |

---

## Knowledge gaps

| Area | Current (CCLI) | Required (SNOCU parity) | Gap |
|------|----------------|-------------------------|-----|
| Local SLD HMI | none | 13-page REGALGRID UI + DG/DDI/DDG diagram | **LuCI or dedicated HMI phase** |
| Measurement defaults | 450 kW / 45 kvar sim | −4 kW / 1 kvar field | **lab profile switch** |
| Per-phase P/Q/PF | TotW/TotVAr only | pages 4–5 full vectors | **P5-04 / meter map** |
| PV / DDG live | STUB LNs | P.Req 80 kW, breaker state | **P5-R + DI wiring** |
| AirGate / Eth2 | not in yaml | 10.99.99.205 profile | **Phase 7** |
| Modbus-TCP server | RTU only | ETH3 10.99.99.210 | **optional adapter** |
| Energy statistics | none | kWh import/export page 9 | **MMTR / logger** |
| Battery / UPS alarms | none | DI08, Wrn Avaria batterie | **hardware** |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| **R-UI-01** | Operators expect REGALGRID 13-page HMI; CCLI is headless | Acceptance friction | Publish this map; plan LuCI SLD MVP |
| **R-UI-02** | Lab sim values (450 kW) disagree with field photos (−4 kW) | TSP false failures vs operator expectation | Add `lab_field_snocu.yaml` profile |
| **R-NET-01** | Field DSO on 192.168.1.0/24; lab on 192.168.10.0/24 | IP confusion only | Document role mapping (REQ-NET-001) |
| **R-MMS-01** | SNOCU may use different LN naming | Compare mismatches | CID already CEI-aligned; TSP subset + waivers |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-UI-001 | TSP read + optional LuCI | TotW/TotVAr within ±0.5 kW/kvar of configured sim **−4/+1** |
| REQ-UI-002 | MMS read `Wlim`/`WSd` | Set-points readable; P.Limit/P.Sp match DSO Operate |
| REQ-UI-003 | `mms_lab_client` poll | PPV ≈ 20 kV; TotW/TotVAr track Modbus |
| REQ-UI-004 | Inject Modbus fault / stop MMS | Alarm or Health transition logged |
| REQ-NET-001 | `ss -lntup` on DUT | MMS on Eth_A 192.168.10.1 only |

### Lab profile for photo parity (recommended)

```yaml
# apps/ccli/config/lab_field_snocu_parity.yaml (proposed)
plant:
  smax_kva: 210
  poc_ppv_kv: 20.0
modbus:
  # PC slave: python modbus_rtu_slave.py --port COM5 --p-kw -4 --q-kvar 1
dso:
  wsd_active: true
  wspt_pct: 8    # match field P.Sp 8%
```

---

## Implementation roadmap (interface parity)

| Priority | Deliverable | Phase |
|----------|-------------|-------|
| P0 | Archive photos + this map | **DONE** (2026-09-30) |
| P1 | Modbus sim **−4 kW / 1 kvar** + yaml **P.Sp 8%** | P5 lab session |
| P2 | TSP subset compare with SNOCU-relevant LNs only | P5 close |
| P3 | OpenWrt LuCI status page: POC P/Q + DG/DDI/DDG schematic | P7 / HMI |
| P4 | ETH2 LTE + AirGate-equivalent remote access | P7 |
| P5 | Full 13-page local HMI (if product requires panel) | Product HW decision |

# REGALGRID SNOCU CCI — field HMI ingest

**Document ID:** FIELD-SNOCU-HMI-001  
**Date ingested:** 2026-09-30  
**Source:** `C:\Yahia\projects\CCLI\Screenshot\` (33 WhatsApp photos, 2026-09-29)  
**Archived photos:** `lab/evidence/field/regalgrid_snocu/photos/`  
**CCLI parity map:** `REGALGRID_SNOCU_vs_CCLI_MAP.md`  
**Platform target:** HiTEKS CCLI on TesPro TG544 (`platform_tg500`)

---

## Device identity (field)

| Item | Value |
|------|-------|
| Vendor | REGALGRID EUROPE |
| Product | **SNOCU CCI** — *Controllore Centrale di Impianto* |
| Controller name (setpoint) | `CCI0CN12GM` |
| CEI role | Same as CCLI **CCI016_01** (central plant controller) |
| Field capture date | 29/09/2026 ~10:43–11:36 (History screen) |
| Plant state during capture | Header **OFF** on most runtime pages |

---

## Physical HMI (REGALGRID panel)

Built-in LCD with membrane keypad — not the external Weintek panel.

| Control | Function |
|---------|----------|
| ← → | Page / column navigation |
| ↑ ↓ | List scroll |
| ☰ | Menu (Setpoints tree) |
| ↵ | Enter / confirm |
| 🔇̸ | Alarm mute |
| ⚠̸ | Alarm acknowledge |
| **I** (green) | Start / enable |
| **O** (red) | Stop / disable |
| 5 soft keys | Info · Home · History · Alarms |
| STATUS LED | Green = healthy (field); yellow = warning state on some pages |

---

## Runtime navigation — 13 pages `[n/13]`

Primary operator loop (left/right arrows). Footer soft keys constant on all pages.

| Page | Title (Italian) | Photo | Key live values @ capture |
|------|-----------------|-------|---------------------------|
| **1/13** | SNOCU CCI — **Impianto** | `01_hmi_01_impianto.jpeg` | DG/DDI/DDG SLD; GRID **−4 kW / 1 kVAr**; LOAD **−3 kW**; PV **####**; P.Req **80 kW** |
| **2/13** | SNOCU CCI — **DSO** | `02_hmi_02_dso.jpeg` | P.Limit **27%**; P.Sp **8%**; Q.Sp **0%**; Cosfi **0.944**; DDI breaker **open** |
| **3/13** | SNOCU CCI — **Misure** | `03_hmi_03_misure.jpeg` | Gauge P **−4 kW**; Q **1 kVAr**; S **4 kVA**; 20 kV class voltages; f **50.007 Hz**; PF **−0.939** |
| **4/13** | Misure **dirette** | `04_hmi_04_misure_dirette.jpeg` | V Ph-N ~11.7–11.8 kV; V Ph-Ph ~20.4 kV; I **1 A**/phase; f **50.010 Hz**; Ubat **27.4 V** |
| **5/13** | Misure **calcolate** | `05_hmi_05_misure_calcolate.jpeg` | Per-phase P/Q; aggregate PF **−0.952** |
| **6/13** | Ingressi **analogici** | `06_hmi_06_ingressi_analogici.jpeg` | Solarimetro, Temperature, CU-AIN-03/04 — all **####** (Ohm) |
| **7/13** | Uscite **digitali** | `07_hmi_07_uscite_digitali.jpeg` | 12 DO slots; only **01 CCI Off** named; all **0** |
| **8/13** | Ingressi **digitali** | `08_hmi_08_ingressi_digitali.jpeg` | DG/DDI **1**; Modbus DSO fault **0**; battery/230V/CCI/comms faults **1** |
| **9/13** | **Statistiche** | `09_hmi_09_statistiche.jpeg` | kWh/kVArh import/export counters |
| **10/13** | **Ethernet 1** | `10_hmi_10_eth1.jpeg` | **192.168.1.1/24**, GW 192.168.1.1, MAC 68:69:f2:54:c3:3a |
| **11/13** | **Ethernet 2** + AirGate | `11_hmi_11_eth2_airgate.jpeg` | **10.99.99.205/24**; AirGate ID 304262239; status Connected |
| **12/13** | **Ethernet 3** | `12_hmi_12_eth3_modbus.jpeg` | **10.99.99.210/24**; MAC 68:69:f2:a4:c3:3a |
| **13/13** | **Info** | `13_hmi_13_info.jpeg` | REGALGRID support contact |

### SLD semantics (pages 1–2)

```
[GRID/DG]——●——[LOAD/DDI]——○——[PV/DDG]
   tower      factory       solar
```

| Symbol | Italian | CEI meaning | CCLI CID prefix |
|--------|---------|-------------|-----------------|
| **DG** | Dispositivo Generale | Grid connection point | `DisFR` (DECP/DGEN/DSTO) |
| **DDI** | Dispositivo Di Interfaccia | Point of connection / load interface | `PdC` (MMXU @ POC) |
| **DDG** | Dispositivo Di Generatore | DER / PV group | `GenPV`, `IDGXCBR1`, `SSGG` |

Breaker glyphs: ● closed, ○ open (field DSO page shows DDI open, DDG open).

---

## Overlay screens (soft keys)

| Screen | Photo | Content |
|--------|-------|---------|
| **Alarmlist** | `14_alarmlist.jpeg` | 5 active warnings (battery, time sync, e-mail, DSO com, Modbus DSO) |
| **History** | `15_history.jpeg` | Event log: Time Stamp, User Login, Wrn Avaria batterie |

---

## Setpoints menu tree

Access via ☰ → **Setpoints**. Two-pane master/detail on overview; full-page detail on drill-down.

| Category | Photo(s) | Notable parameters |
|----------|----------|-------------------|
| Login | `16_setpoints_login.jpeg` | Access control |
| Process Control | `19_*`, `20_*` | Import Load 0 kW; System Baseload **1000 kW**; Load Ramp **200 s**; breaker close retries |
| Basic Settings | `18_*`, `21_*` | Name **CCI0CN12GM**; Nominal Import **1200 kW**; **3Ph4Wire**; **20 kV** Ph-Ph; **50 Hz** |
| Communication | `17_*`, `22_*` | Modbus addr **1**; RS485 **9600 8N1** |
| ETH1 Trusted | `23_*` | Manual **192.168.1.1/24**; Modbus server **Disabled**; Direct port **23** (Telnet) |
| ETH2 Untrusted | `24_*` | **10.99.99.205**; AirGate **global.airgate.link:54440**; DNS 8.8.8.8 / 4.4.4.4 |
| ETH3 Modbus | *(setpoints page not in set; runtime 12/13)* | Runtime IP **10.99.99.210** |
| Mains Settings | `25_*` | OV/UV thresholds, IDMT, short-circuit **150%** |
| Protections | `26_*` | Vector Shift, ROCOF1–4 — all **Disabled** |
| Bus Settings | `27_*` | Bus >V **120%**, <V **90%**, >f **1.5 Hz**, <f **−2.5 Hz** |
| Grid Codes | `28_*` | Q Ramp **10 s**; Q Deadband **2%**; most CEI functions **Disabled** |
| Load Shedding | `29_*` | **Disabled** |

Editable fields marked with red hand icon on Communication page.

---

## External plant HMI (Weintek / Energy Synt)

Separate cabinet panel — not the SNOCU built-in display.

| Photo | Notes |
|-------|-------|
| `30_weintek_energy_dashboard.jpeg` | FV **46 kW**, RETE **0 kW**, BESS **0%**, CARICO **46 kW**; WAN label **10.99.99.112** |
| `31_weintek_splash_wan.jpeg` | Energy Synt splash; same WAN IP |

Treat as **plant SCADA**, not CCI HMI. CCLI lab has no Weintek target in Phase 5.

---

## Field measurements — lab alignment reference

Use these values when validating TSP reads or Modbus slave defaults against field photos:

| Quantity | Field SNOCU | CCLI lab default (P5) | Align? |
|----------|-------------|----------------------|--------|
| POC P (export = negative) | **−4 kW** | **+450 kW** (Modbus) | **GAP** — use `modbus_rtu_slave.py --p-kw -4` for photo parity |
| POC Q | **+1 kVAr** | **+45 kvar** | **GAP** |
| P.Limit | **27%** of Smax | Wlim via MMS (DSO) | map to `WlimDWMX1` |
| P.Sp (WSd) | **8%** | yaml mock **20%** → 42 kW | map to `WSdDAGC1` |
| Nominal voltage | **20 kV** Ph-Ph | yaml `poc_ppv_kv: 20.0` | **MATCH** |
| Nominal import | **1200 kW** | `plant.smax_kva: 210` kVA | **GAP** (different plant scale) |
| ETH trusted | **192.168.1.1** | Eth_A **192.168.10.1** | subnet differs; role = DSO port |
| ETH Modbus IP | **10.99.99.210** | RS485 RTU `/dev/rs485_2_uart` | transport differs |

---

## Active alarms @ capture (`14_alarmlist.jpeg`)

| # | Alarm | CCLI test hook |
|---|-------|----------------|
| 01 | Wrn Avaria batterie | DI08 / UPS telemetry — **MISSING** on TG544 lab |
| 02 | Wrn CCI time synch | chrony / GNSS — **PART** (P3 GNSS evidence) |
| 03 | Wrn Alarm e-mail 1 Fail | not in CCLI scope |
| 04 | Wrn CCI DSO com | MMS session / heartbeat — **testable** TSP |
| 05 | Wrn Avaria Modbus (DSO) | Modbus RTU path — **testable** P5 |

---

## Digital I/O map (field labels)

### Inputs (`08_hmi_08_ingressi_digitali.jpeg`)

| DI | Label | Value | CCLI mapping |
|----|-------|-------|--------------|
| 01 | Stato DG | 1 | `DisFR*` Beh / XCBR |
| 02 | Stato DDI | 1 | POC breaker status |
| 03 | Disponibile | 1 | spare |
| 06 | Avaria Modbus (DSO) | 0 | Modbus adapter fault |
| 07 | Anomalia 230Vac/24Vdc | 1 | PSU monitor — **GAP** |
| 08 | Avaria batterie | 1 | UPS — **GAP** |
| 10 | Avaria CCI | 1 | LLN0 Health / internal fault |
| 11 | Avaria comunicazione | 1 | comms aggregate |

### Outputs (`07_hmi_07_uscite_digitali.jpeg`)

| DO | Label | CCLI mapping |
|----|-------|--------------|
| 01 | CCI Off | PF2 curtail / trip command — partial via DIO |

---

## Corpus status

| Artifact | Status |
|----------|--------|
| Field photos archived | **HAVE** — 33 files in `photos/` |
| HMI page inventory | **HAVE** — this document |
| CCLI parity matrix | **HAVE** — `REGALGRID_SNOCU_vs_CCLI_MAP.md` |
| LuCI / web HMI clone | **N/A** — product uses **cloud UI/HMI** ([CCI_Cloud_Operator_HMI.md](../../../knowledge-base/08-engineering/CCI_Cloud_Operator_HMI.md)) |
| TG544 local LCD HMI | **N/A** — REGALGRID panel = reference for **cloud screen parity** only |
| ETH2 AirGate profile | **MISSING** in lab yaml |
| ETH3 Modbus-TCP | **MISSING** — lab uses RS485 RTU |

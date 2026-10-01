# TesPro TR500 / TR400 — I/O & Serial Peripheral Map (Bench-Confirmed)

**Document ID:** CCLI-HW-TR500-IO-001  
**Revision:** 1.0  
**Date:** 2026-09-17  
**RAG source_id:** `ccli-tr500-io-peripheral`  
**Parent:** `ccli-tg500-lab-platform` · `ccli-tr400-user-manual-extract` · `ccli-tr400-capture-plan`  
**Lab DUT:** hostname **TG544**, model **Tespro TR500** (`tespro,tr500`), TesproOS **32-2.1.0**  
**Diagram:** `Architecture/TR400_Lab_Platform.drawio` (pages 1–2, 4–5)  
**OEM schematic:** `reference/vendor/tespro/Schematic-GPIO.pdf` · extract `CCI_TR400_GPIO_Schematic_Extract.md` (`ccli-tr400-gpio-schematic-extract`)  
**Config:** `apps/ccli/config/lab_tr400.yaml`  
**Runbook:** `lab/TR400_DIO_SMOKE_TEST.md` · **PF2 limits/actions:** `lab/PF2_Limits_and_Actions.md`

---

## Summary

This document is the **authoritative CCLI mapping** from **physical connectors** on the received TesPro gateway to **Linux devices**, **TesproOS services**, and **CCLI configuration keys**. It closes **K5 (field I/O & serial)** for Phase 1 lab work on **TG544**.

**Critical finding:** Digital I/O is **not** on the MediaTek SoC GPIO. A **GD32 coprocessor** exposes four configurable DIO channels through **`ubus` object `dido_v2`** (service **`didoservice_v2`**). Do **not** use `libgpiod` / `gpiodetect` on this platform.

| Layer | Technology | Status |
|-------|------------|--------|
| Serial plant bus | `/dev/ttyS1` RS485-1 (A1/B1) | **CONFIRMED** (manual p.29 + SSH) |
| Serial spare | `/dev/ttyS2` RS485-2 (A2/B2) | **CONFIRMED** |
| Service port | `/dev/ttyS0` RS232 (RXD/TXD/GND) | **CONFIRMED** |
| Digital I/O | GD32 → `ubus dido_v2` | **CONFIRMED** — smoke PASS 2026-09-15 |
| CCLI HAL | `platform_tg500` → ubus wrapper | **TODO** |
| Analog I/O | AI1/2, AO1/2 on 10-pin block | **NOT BENCH-TESTED** |

---

## Requirements

| ID | Requirement | TR500 / TG544 mapping | Status |
|----|-------------|----------------------|--------|
| REQ-IO-001 | Curtailment digital output for PF2 relay | **DIO1** → ubus ch **1** → `io.do_curtail` | **HAVE** (bench) |
| REQ-IO-002 | Permissive digital input interlock | **DIO2** → ubus ch **2** → `io.di_permissive` | **HAVE** (bench) |
| REQ-IO-003 | ≥2 spare digital lines for expansion | **DIO3**, **DIO4** → ubus ch 3–4 (default DO) | **PARTIAL** |
| REQ-HW-004 | Modbus RTU on plant RS485 | **A1/B1** → `/dev/ttyS1` 9600 8N1 | **HAVE** (device node) |
| REQ-HW-004b | Second RS485 for future plant segment | **A2/B2** → `/dev/ttyS2` | **HAVE** (device node) |
| REQ-SW-010 | CCLI config file maps silkscreen → API | `lab_tr400.yaml` | **HAVE** |
| REQ-SW-011 | Safe default: curtailment OFF at boot | `set_relay ch1 value=0`; no lab blink | **POLICY** |

---

## Architecture

### Functional

```
PF2 FSM ──► io_adapter ──► platform_tg500 HAL ──► ubus dido_v2 ──► GD32 ──► DIO1 (relay)
Permissive ───────────────────────────────────────► ubus read_di ──► GD32 ──► DIO2 (switch)
Modbus adapter ──► libmodbus RTU ──► /dev/ttyUSB0 (USB dongle) or /dev/ttyS1 (A1/B1)
```

### Hardware stack

| Block | Function | Implementation |
|-------|----------|----------------|
| MT798X SoC | Application CPU, Ethernet, UART | OpenWrt / TesproOS |
| RS485 transceivers ×2 | Field bus isolation / line drive | On-board — polarity per manual |
| GD32 MCU | DIO + AI/AO front-end | Coprocessor — **not** direct SoC GPIO |
| 10-pin I/O block | DIO1–4, AI1/2, AO1/2, AOG, GND | Front panel silkscreen |
| 7-pin serial block | RS232 + RS485-1 + RS485-2 | RXD/TXD/GND, A1/B1, A2/B2 |

### Software stack (TesproOS on TG544)

| Component | Path / API | Role |
|-----------|------------|------|
| Serial service | LuCI → Services → Serial | UART params; default 9600 8N1 |
| Modbus gateway | LuCI → Modbus service | Vendor gateway — **disable** for CCLI product profile |
| DIDO V2 service | `/etc/config/didoservice_v2` | Channel mode DO/DI |
| ubus | `dido_v2` | `set_relay`, `read_di`, `set_mode`, `status` |
| CCLI (future) | `/etc/ccli/lab.yaml` | PF2 + Modbus + IO via `platform_tg500` |

### Security / isolation

- Field I/O is **Z0 plant-adjacent** — treat DIO/RS485 wiring as **untrusted plant interface**.
- Commissioning (SSH/LuCI/USB) is **Z4 engineering** — separate from DSO/operator VLANs (Phase 2).
- No L2 bridge between Eth_A and plant serial — **policy**, not hardware isolation on RS485.

### Power

- DIO outputs are **open-drain sinks** — **do not** assume the gateway supplies relay coil power.
- Relay modules need **external +5 V** (or plant-rated supply) per bench runbook.
- RS485 GND reference: use terminal **GND** on 7-pin block for bench sim; confirm isolation for product wiring.

---

## Physical connectors

### 7-pin serial block (confirmed)

| Silkscreen | Signal | Linux device | CCLI key | Default params |
|------------|--------|--------------|----------|----------------|
| **RXD** | RS232 RX | `/dev/ttyS0` | `serial.rs232.device` | 9600 8N1 (configurable) |
| **TXD** | RS232 TX | `/dev/ttyS0` | — | Service / debug |
| **GND** | RS232 ground | — | — | — |
| **A1** | RS485-1 A | `/dev/ttyS1` | `serial.rs485_a.device` | 9600 8N1 |
| **B1** | RS485-1 B | `/dev/ttyS1` | — | Modbus RTU plant |
| **A2** | RS485-2 A | `/dev/ttyS2` | `serial.rs485_b.device` | 9600 8N1 |
| **B2** | RS485-2 B | `/dev/ttyS2` | — | Spare plant bus |

**Manual reference:** TR400 User Manual ch. 7.1 — RS485-1 `/dev/ttyS1`, RS485-2 `/dev/ttyS2`, RS232 `/dev/ttyS0`.  
**Bench wiring (Phase 1):** USB-RS485 dongle on **TG544 USB** → `/dev/ttyUSB0` (CCLI master); slave at far end of bus (PC pymodbus / inverter). Alternate: onboard A1/B1 → `/dev/ttyS1`.

### 10-pin I/O block (bench-confirmed DIO; AI/AO unverified)

| Silkscreen | ubus channel | Default mode | CCLI role | Config key |
|------------|--------------|--------------|-----------|------------|
| **DIO1** | 1 | DO | PF2 curtailment relay drive | `io.do_curtail.gpio: 1` |
| **DIO2** | 2 | DI (after UCI prep) | Permissive interlock | `io.di_permissive.gpio: 2` |
| **DIO3** | 3 | DI (prep) | PF2 spare — PI trip / external alarm | `io.di_pf2_spare.gpio: 3` |
| **DIO4** | 4 | DO | PF2 spare — curtail ack / annunciator | `io.do_pf2_spare.gpio: 4` |
| **AI1** | — (fw: AI0?) | Analog in | DRC-60A battery voltage (divider) | `analog.ai_battery_v.pin: AI1` |
| **AI2** | — | Analog in | DRC-60A AC OK + bat low ladder | `analog.ai_ups_status.pin: AI2` |
| **AO1** | — | Analog out | Optional load-shed → K2 (+V break) | `analog.ao_load_shed.pin: AO1` |
| **AO2** | — | Analog out | Spare | `analog.ao_spare.pin: AO2` |
| **AOG** | — | Analog ground | Tie to Bat− / divider return | `analog.analog_gnd: AOG` |
| **GND** | — | Digital ground | DIO reference | — |

**I/O policy (lab):** all **four DIO** channels are **PF2 / CEI only**. UPS/charger (“BMS”) is **MEAN WELL DRC-60A** on **AI/AO**, not Modbus. Wiring: `lab/DRC60A_UPS_WIRING.md`; diagram page 6 in `Architecture/TR400_Lab_Platform.drawio`.

**Electrical behaviour (TG544 bench, 2026-09-15):**

| Channel | Mode | Measured / observed | CCLI polarity flag |
|---------|------|---------------------|-------------------|
| DIO1 | DO | OFF **1.5 V**, ON **0 V** vs GND; continuity ON **~1 kΩ** to GND | `active_high: false` (open-drain sink) |
| DIO2 | DI | Open → state **0**; short to GND → state **1** | `active_low: true` |

---

## ubus dido_v2 API

### One-time prep (DIO2 as input)

UCI alone may leave `read_di` stale (`last_poll: 0`). Also call ubus **`set_mode`** and **`gd32.set_mode`** (see `lab/DIDO_V2_API.md` — no public vendor doc on the web as of 2026-09).

```bash
uci set didoservice_v2.channel_2.mode=di
uci commit didoservice_v2
/etc/init.d/didoservice_v2 restart
ubus call dido_v2 set_mode '{"channel":2,"mode":"DI"}'
ubus call dido_v2 gd32.set_mode '{"channel":2,"mode":"DI"}'
ubus call dido_v2 status    # expect channel 2 "mode": "DI"
ubus call dido_v2 gd32.read_di '{}'   # di[1] toggles 0/1 with DIO2 open/short GND
```

### Runtime commands

```bash
# Curtailment DO (DIO1 / channel 1)
ubus call dido_v2 set_relay '{"channel":1,"value":1}'   # energize relay path
ubus call dido_v2 set_relay '{"channel":1,"value":0}'   # safe default OFF

# Permissive DI (DIO2 / channel 2)
ubus call dido_v2 read_di '{"channel":2}'

# Full channel status
ubus call dido_v2 status
```

### CCLI YAML mapping (`lab_tr400.yaml`)

```yaml
io:
  backend: ubus_dido_v2
  do_curtail:      { gpio: 1, active_high: false }
  di_permissive:   { gpio: 2, active_low: true }
  di_pf2_spare:    { gpio: 3, active_low: true }
  do_pf2_spare:    { gpio: 4, active_high: false }
analog:
  ai_battery_v:    { pin: AI1 }
  ai_ups_status:   { pin: AI2, mode: ladder }
  ao_load_shed:    { pin: AO1, default_v: 0.0 }
ups:
  model: MEAN_WELL_DRC-60A
  divider: { r1_ohm: 120000, r2_ohm: 39000 }
```

Full keys: `apps/ccli/config/lab_tr400.yaml`.

Deploy target: `/etc/ccli/lab.yaml` on TG544.

---

## Interface matrix

| Interface | Source | Destination | Protocol / API | Phase | Status |
|-----------|--------|-------------|----------------|-------|--------|
| Plant serial | Energy analyzer / PC sim | TG544 A1/B1 | Modbus RTU on `/dev/ttyS1` | 1 | CONFIG |
| Spare RS485 | Future plant segment | TG544 A2/B2 | Modbus RTU on `/dev/ttyS2` | 2+ | CONFIRMED node |
| Curtailment DO | PF2 FSM | Relay module IN | `dido_v2 set_relay` ch1 | 1 | HAL TODO |
| Permissive DI | Field switch / jumper | PF2 FSM | `dido_v2 read_di` ch2 | 1 | HAL TODO |
| PF2 spare DI | External trip / alarm | PF2 FSM | `dido_v2 read_di` ch3 | 1+ | UNTESTED |
| PF2 spare DO | Ack / annunciator | Plant indicator | `dido_v2 set_relay` ch4 | 1+ | UNTESTED |
| RS232 service | Engineer terminal | TG544 | `/dev/ttyS0` | 0 | CONFIRMED node |
| UPS battery V | DRC-60A Bat+ divider | CCLI analog | AI1 ADC | 1+ | NOT TESTED |
| UPS status | DRC-60A alarm contacts | CCLI analog | AI2 ladder | 1+ | NOT TESTED |
| UPS load-shed | CCLI / policy | K2 on +V feed | AO1 → NPN | 2+ | NOT TESTED |

---

## BOM matrix (lab peripherals)

| Block | Function | Candidate / PN | Status |
|-------|----------|----------------|--------|
| DUT-GW | Gateway with DIO + RS485 | TesPro TR500 (TG544) | **ACTIVE** |
| LAB-RELAY | Curtailment contact sim | COTS 5 V relay module | **ACTIVE** |
| LAB-RS485 | Modbus RTU slave sim | USB-RS485 adapter + pymodbus | **ACTIVE** |
| LAB-SW | Permissive sim | Jumper DIO2→GND or toggle | **ACTIVE** |
| LAB-UPS | 12 V UPS/charger | MEAN WELL DRC-60A | **PLANNED** |
| LAB-BAT | AGM backup | 12 V sealed lead-acid | **PLANNED** |
| LAB-DIV | Bat voltage sense | R1 120k + R2 39k + 100nF | **PLANNED** |
| LAB-K2 | Optional load-shed | 5 V relay + 2N2222 | **PLANNED** |
| SW-HAL | ubus DIO wrapper | `platform_tg500/hal_gpio_ll.c` | **TODO** |
| SW-MB | libmodbus RTU poll | `modbus_adapter.cpp` | **TODO** |
| SW-AIO | ADC/AO scaling | vendor path TBD | **TODO** |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| DIO → ubus map | **HAVE** ch1–4 | — | **CLOSED** (bench) |
| DIO voltage/current class | Logic-level open-drain | AiLux 10–120 V DI class (C4) | **OPEN** — expansion module for product |
| AI/AO pin function | Silkscreen + policy doc | ubus/API + scaling + bench V-01 | **PARTIAL** — circuit defined, not tested |
| DRC-60A integration | Datasheet | Lab wire-up + calibration | **OPEN** |
| RS485 isolation / termination | Manual tip p.48 | Product wiring diagram | **PARTIAL** |
| Modbus register map | pymodbus sim | Real analyzer map | **MISSING** |
| SKU TR500 vs TR400 vs TG-524 | `tespro,tr500` on DUT | Written TesPro confirm | **OPEN** (K1.6) |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-IO-001 | Assumed SoC GPIO — used libgpiod | HAL fails silently or wrong pin | This doc + `backend: ubus_dido_v2` |
| R-IO-002 | Open-drain DO mis-wired as push-pull | Relay never drops out | Document active-low; bench DMM procedure |
| R-IO-003 | DIO2 left in DO mode | Permissive always wrong | UCI prep in commissioning checklist |
| R-IO-004 | 4 DIO ≠ AiLux 5 DI + 3 DO | Incomplete C4 demo | Wave B expansion; document delta |
| R-IO-005 | Vendor Modbus service conflicts with CCLI | Double master on ttyS1 | Disable vendor gateway in product profile |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-IO-001 | `ubus call dido_v2 set_relay '{"channel":1,"value":1}'` | Relay click or DIO1↔GND continuity |
| REQ-IO-001 | `set_relay` value 0 | Relay off; continuity open |
| REQ-IO-002 | DIO2 open vs short GND | `read_di` returns 0 / 1 respectively |
| REQ-HW-004 | `ls -l /dev/ttyS1` + pymodbus RTU | Slave responds on A1/B1 |
| REQ-SW-010 | Deploy `lab_tr400.yaml`; start ccli | PF2 uses ch1/ch2 via HAL |
| L1-P-03 | End-to-end PF2 + Modbus + DIO | Threshold trip drives DIO1 with permissive gating |

Full bench procedure: `lab/TR400_DIO_SMOKE_TEST.md`.

---

## Cross-reference

| Document | RAG source_id | Relationship |
|----------|---------------|--------------|
| TG-500 lab platform freeze | `ccli-tg500-lab-platform` | Product port roles |
| TR-400 manual extract | `ccli-tr400-user-manual-extract` | Serial tty names (manual p.29) |
| TR-400 capture plan | `ccli-tr400-capture-plan` | Batch E DI/DO — **closed by bench** |
| Lab platform diagram | — | `Architecture/TR400_Lab_Platform.drawio` (6 pages) |
| DIO smoke runbook | — | `lab/TR400_DIO_SMOKE_TEST.md` |
| DRC-60A UPS wiring | — | `lab/DRC60A_UPS_WIRING.md` |

---

## Keywords

`TR500`, `TR400`, `TG544`, `TesPro`, `GD32`, `dido_v2`, `didoservice_v2`, `ubus`, `DIO`, `RS485`, `ttyS1`, `Modbus RTU`, `lab_tr400.yaml`, `platform_tg500`, `ccli-tr500-io-peripheral`

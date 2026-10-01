# TesPro TR400 / TG500 — GD32 GPIO schematic extract (DIO vs regulation)

**Document ID:** CCLI-HW-TR400-SCH-GPIO-001  
**Revision:** 1.0  
**Date:** 2026-09-21  
**RAG source_id:** `ccli-tr400-gpio-schematic-extract`  
**Source:** `reference/vendor/tespro/Schematic-GPIO.pdf` (Altium `Sheet1.SchDoc`, drawn 2026-09-17)  
**Parent:** `ccli-tr500-io-peripheral` · `ccli-pf2-limits-actions` · `cei-0-16-allegato-o` · `cei-0-16-otm-verification`  
**Lab config:** `apps/ccli/config/lab_tr400.yaml`

---

## Summary

TesPro supplied a **single-sheet GD32F330** schematic for the **10-pin field I/O block**: **4× field channels (IO1–IO4)**, **2× AI**, **2× AO**, plus debug/SWD and FT232 debug UART. Each digital channel uses **opto-isolated relay drive (TLP291)**, **Omron G6K-2F-Y3VDC**, field **TVS SMBJ33CA**, **self-reset fuse TR600-350 (350 mA hold / 700 mA trip)**, and separate **`GND` / `GND_GE`** references.

**Regulation check (start):** Allegato **O** defines **functions** (PF2 actuation, DG/DD status, Annex M teledistacco) — **not** 10–120 V pin counts. **Phase 1 lab** can bind **REQ-IO-001/002** to **ubus ch1/ch2** on this hardware. **Product class REQ-IO-004** (5× DI 10–120 V, 3× relay DO @ plant voltage) is **not met** by this OEM front-end alone (**4 channels**, ~**33 V TVS** class field protection).

---

## Requirements (DIO scope)

| ID | Requirement | Schematic evidence | vs regulation | Status |
|----|-------------|-------------------|---------------|--------|
| REQ-IO-001 | PF2 **curtailment** digital output | IO1 → K1 → relay contacts on **IO1** terminal | O.9.2 mode (i) actuation at plant | **FIT** — assign ubus **ch1** / DIO1 |
| REQ-IO-002 | **Permissive** interlock (lab) | IO2 field + GD32 path (GPIO35/41/47…) | **Not in Annex O/T** — engineering only | **FIT** — ubus **ch2** / DIO2 as DI |
| REQ-IO-003 | Boot / fault → **defined safe DO** (62443 CR 3.6) | Relay coil de-energised when MCU offline; contact wiring defines plant safe state | O.9.2 + O.13 stale policy | **PART** — prove NC/NO wiring + FSM OFF at boot |
| REQ-IO-004 | Product **5 DI + 3 DO** @ **10–120 V** | **4** channels only; **SMBJ33CA** on field | AiLux product class; Annex O silent on voltage | **GAP** — custom carrier or expansion |
| REQ-PF2-001 | DSO **Wlim** slaved limit (O.9.2.2) | No extra hardware — MMS on SoC | Software + same actuation path | **N/A** on this sheet |
| REQ-PF2-002 | **Annex M teledistacco** — no conflicting action (O.11) | Spare channel **IO3/IO4** available as DI | O.9.3 / O.12 / B.8 in `PF2_Limits_and_Actions.md` | **PART** — wire **DIO3**, not in Phase 1 code |
| REQ-PF2-003 | **DG / interface switch** status (O.14 / T `XCBR1.IDG.Pos`) | Spare DI on IO3 or IO4 | Data logger + MMS publish | **OPEN** — allocation TBD |
| REQ-VEND-003 | Linux / silkscreen **port map** | Net names `GPIO*_IF`, `IO1`–`IO4`, `ADC_IN0/1` | Closes part of V-003 | **PART** — map GPIO→`dido_v2` channel in TesPro FW |
| REQ-VEND-007 | **Galvanic isolation** proof | TLP291 per relay path; `GND_GE` vs `GND` | RS485 not on this sheet | **PART** for DIO only |

---

## Architecture

### Hardware (from schematic)

| Block | Part / net | Role |
|-------|------------|------|
| MCU | **GD32F330CBT6** (U12) | Coprocessor — matches `ccli-tr500-io-peripheral` |
| Digital channels | **IO1–IO4** (J2 **3.5-10P**) | Field screw terminals |
| Relay | **G6K-2F-Y3VDC** (K1–K4) | **3 V coil**; dry contact to plant |
| Opto | **TLP291(GB-TP,SE)** (U20–U27, U28–U29) | Coil / GPIO isolation |
| Field protection | **SMBJ33CA** (TVS16–19), **TR600-350**, **B3D090L-C** | Transient / overcurrent on field side |
| GPIO nets (examples) | `GPIO45_IF`, `GPIO39_IF` (IO1 relay path); `GPIO35_IF`, `GPIO41_IF`, `GPIO47_IF` (IO2) | GD32 ↔ isolation ↔ power stage |
| Additional GPIO | `GPIO17`–`GPIO21`, `GPIO16`, `GPIO9`, `GPIO13`, `GPIO10`–`GPIO12`, `GPIO6` + `_IF` variants | DI sense / mode lines — **channel mode in firmware** |
| Analog in | **ADC_IN0/1** → AI1, AI2; note *Actual value = Return value × 0.001* | UPS / divider path (lab DRC-60A) |
| Analog out | **AO1, AO2** via TLP291 + `ISO_CAN` / `VDD_CAN_5V` domain | Isolated AO — not Phase 1 PF2 |
| Debug | J10 SWD; FT232 `RXD_FT232_IF` / `TXD_FT232_IF` | Factory — not field |
| Clock | 8 MHz + 32.768 kHz | — |

**Supplier email (2 DI + 2 DO):** Hardware is **four identical channel blocks**; **DO/DI mode is firmware/UCI** (`didoservice_v2`), not fixed at two and two.

### Functional (CCLI Phase 1 vs CEI target)

```text
Phase 1 (lab):
  Modbus P → PF2 FSM → ubus ch1 → GD32 → IO1 relay → plant curtail (REQ-IO-001)
  Permissive → ubus ch2 read_di → GD32 → IO2 field (REQ-IO-002)

CEI target (product):
  Eth_A Wlim (O.9.2.2) → same actuation path as IO1
  Annex M trip DI → IO3 (proposed) → FSM inhibit (O.11) — NOT in code yet
  DG/DD status DI → IO4 or MMS from breaker — O.14 / T Table 85+
```

### Network / security

- Field I/O is **plant-adjacent (Z0)** — treat **IO1–IO4** as untrusted; no CEI requirement to encrypt DIO.
- **Permissive (DIO2)** is **SL-2 engineering interlock**, not a DSO MMS object.

### Power

- Relay coils on **3V3** — **external plant voltage** appears on **relay contacts** only; coil supply from gateway.
- Bench note in `lab_tr400.yaml`: relay module may need **external +5 V** — reconcile with **G6K 3 V coil** on schematic (likely powered from 3V3 on board).

---

## Interface matrix (schematic → ubus → CEI)

| Terminal | Schematic | Bench ubus ch | CCLI key (lab) | CEI / Annex O function | Phase |
|----------|-----------|---------------|----------------|------------------------|-------|
| **IO1** | K1 contacts | **1** (DO default) | `io.do_curtail` | O.9.2 **active power limitation** actuation (mode i) | 1 ✓ |
| **IO2** | K2 + DI front-end | **2** (DI after UCI) | `io.di_permissive` | *(none — lab interlock)* | 1 ✓ |
| **IO3** | K3 | **3** | `io.di_pf2_spare` | **Annex M teledistacco** feedback / inhibit (O.11, O.9.3) | 6 target |
| **IO4** | K4 | **4** | `io.do_pf2_spare` or DI | **DG / interface switch** status (O.14) or spare DO | 6 target |
| **AI1/AI2** | ADC_IN0/1 | analog svc | `analog.ai_*` | UPS / auxiliary — not PF2 DIO | 1 partial |
| **AO1/AO2** | Isolated AO | analog svc | `analog.ao_*` | Optional shed — not Annex O mandatory | later |

---

## BOM matrix (DIO-relevant)

| Block | Function | MPN (schematic) | Status |
|-------|----------|-----------------|--------|
| MCU | DIO + AI/AO | GD32F330CBT6 | **CONFIRMED** on sheet |
| Relay | Plant dry contact | G6K-2F-Y3VDC | **HAVE** — verify contact rating vs curtail load |
| Opto | Isolation | TLP291(GB-TP,SE) | **HAVE** |
| TVS | Field transients | SMBJ33CA | **~33 V clamp** — not 120 V DI class |
| Fuse | Field overcurrent | TR600-350 | **350 mA hold** (note: 4× DIO) |
| Product DI conditioner | 10–120 V opto | e.g. ISO1212 (proposed in BOM review) | **MISSING** on TG OEM board |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| GPIO pin ↔ **ubus channel** | Net names only | TesPro `didoservice_v2` map | **MISSING** — request or trace FW |
| NC/NO **safe state** on relay | G6K on sheet | Wiring diagram for fail-safe curtail | **MISSING** |
| Field **voltage range** per IOx in DI mode | TVS33 + divider resistors (47k, 14.3k, 5.6k) | Datasheet + TesPro spec | **PART** — likely **24 V class**, not 10–120 V |
| **Channel count** vs REQ-IO-004 | 4 | 5 DI + 3 DO product | **GAP** |
| Annex **M** wiring | Not in PDF | DI threshold / polarity | **MISSING** extract |
| Bench **1.5 V / 0 V** vs relay | Smoke test open-drain wording | Confirm if measuring driver vs contact | **VERIFY** on TG544 |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-SCH-001 | Treat OEM 4× IO as **REQ-IO-004** compliant | False cert / wrong fixture | Phase 6 expansion or custom C4 carrier |
| R-SCH-002 | **33 V TVS** field DI with **120 V** plant switchgear | Damage / false trips | ISO1212 front-end on product carrier |
| R-SCH-003 | **Teledistacco not wired** on DIO3 | O.11 conflict risk | Wire + FSM inhibit before field PF2 |
| R-SCH-004 | Supplier **2 DI + 2 DO** vs **4 flexible** channels | Procurement / test plan drift | Document UCI mode per channel |
| R-SCH-005 | Relay **safe state** undefined | Unsafe curtail on fault | CR 3.6 test + contact form choice |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-IO-001 | `ubus call dido_v2 set_relay '{"channel":1,"value":1}'` | K1 pulls in; IO1 contact continuity changes |
| REQ-IO-002 | UCI `channel_2.mode=di`; open/short IO2 to **GND_GE** | `read_di` 0/1 per `lab_tr400.yaml` polarity |
| REQ-IO-003 | Reboot GD32 / stop `didoservice_v2` | Curtail **de-energised** (document NC/NO) |
| REQ-IO-004 | — | **N/A** on OEM sheet — track on C4 carrier |
| O.11 inhibit | Force DI on IO3 (when allocated) | PF2 **does not** assert curtail while trip active |
| V-007 DIO isolation | DMM / hipot per TesPro | TLP291 barrier; **GND** ≠ **GND_GE** under stress |

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-21 | Initial extract from `Schematic-GPIO.pdf`; DIO vs REQ-IO / Allegato O start matrix |

---

## Keywords

`TesPro`, `GD32F330`, `G6K-2F`, `TLP291`, `dido_v2`, `IO1`, `REQ-IO-001`, `O.9.2`, `O.11`, `Annex M`, `ccli-tr400-gpio-schematic-extract`

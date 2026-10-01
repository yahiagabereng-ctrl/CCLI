# CCI Mocking / PF2 Bench BOM

**Document ID:** CCLI-BOM-BENCH-001  
**Revision:** 1.1  
**Date:** 2026-09-15  
**RAG source_id:** `ccli-mocking-bench-bom`  
**Companion strategy:** `CCI_Validation_and_Mocking_Strategy.md` (`ccli-validation-strategy`)  
**Companion product BOM:** `CCI_Prototype_BOM.md` (field CCI parts — separate budget)

**Purpose:** Buy list for the progressive PF2 validation bench (L1 software mock -> L2 RS-485/Ethernet HIL -> L3 real analyzer). L4 power-system HIL is listed as deferred / study-only.

**Scope rule:** Lab DUT is **TesPro TG544 / TR500** (TesproOS 25.12, on bench). **Raspberry Pi removed from scope** (2026-09-17). Product SKU remains **TG-524**.

Status keys: **BUY NOW** · **BUY NEXT** · **DEFER** · **HAVE** · **TBD** (model not locked) · **OPTIONAL**

---

## 0. Buy waves (recommended)

| Wave | Level | Goal | Est. incremental EUR | Decision |
|------|-------|------|---------------------:|----------|
| **A** | L1 + L2 | Software mock + physical RS-485/Ethernet HIL | 180-350 | **BUY NOW** (excl. PC / kit already owned) |
| **B** | L2+ C4 | DI test box (24 V first) + DO lamps | 80-180 | **BUY NEXT** after Wave A works |
| **C** | L3 | Real energy analyzer + register map | 400-1,200+ | **BUY NEXT** before field pilot |
| **D** | L4 | Typhoon / OPAL-RT / RTDS campaign | 10k-100k+ or rental | **DEFER** until L1-L3 pass |

**Assumed already on lab path (do not double-count):**

| Item | PN | Est. EUR | Notes |
|------|-----|--------:|-------|
| **Lab gateway (FROZEN)** | **TesPro TR400** / TG-524 | TBD (≤€350 comm-box cap) | **TR400 ON BENCH** — see `CCI_TG500_Lab_Platform.md` Rev 3.0 |
| **L1 mock** | — | — | **REMOVED** — use TG544 |
| **Bench I/O** | USB-RS485 + relay module | ~20–40 | A1/B1 Modbus HIL; DIO1/DIO2 relay |
| Engineering laptop/PC | — | — | Host for simulators / Wireshark / clients |

**Reference only (not lab DUT):** historical SoM vendor study documents on disk.

**Wave A+B bench-only subtotal (typical):** ~**EUR 260-530** (adapters, switch, cabling, DI/DO fixture — not including analyzer or kit).

---

## 1. Topology (what this BOM builds)

```text
Engineering PC
+-- pymodbus / ModbusPal / QModMaster (L1)
+-- libiec61850 client / IEC104 test client
+-- Wireshark + serial capture
+-- USB-RS485 adapter(s)  ---- RS-485 cable ---- TG-524 RS485 (or Pi USB-RS485)
+-- Ethernet ----------------- unmanaged switch --- TG-524 LAN1/LAN2/LAN3 (or Pi eth0)
                                                      |
                                    +-- DI test box (24 V Wave B)
                                    +-- DO indicator lamps (Wave B)
```

Later (Wave C): replace/supplement Modbus emulator with a real analyzer on the same RS-485 bus.

---

## 2. Wave A — L1 software mock + L2 interface HIL (**BUY NOW**)

### 2.1 Software (zero or low HW cost)

| Ref | Item | Qty | Est. EUR | Status | Notes |
|-----|------|----:|--------:|--------|-------|
| SW-MB | **pymodbus** simulator + scenario runner | 1 | 0 | **BUY NOW** | Preferred for scripted P/Q/PF2 ramps, timeout, exceptions |
| SW-MB2 | ModbusPal / QModMaster / ModRSsim2 | 0-1 | 0 | **OPTIONAL** | Manual UI debugging beside pymodbus |
| SW-61850 | libiec61850 client tools | 1 | 0 | **BUY NOW** | GPLv3 for lab; commercial license before product ship |
| SW-104 | IEC 60870-5-104 test client (open or licensed) | 1 | 0-TBD | **TBD** | Pick after stack choice; lib60870 is one option |
| SW-CAP | Wireshark (+ USB-serial capture if needed) | 1 | 0 | **BUY NOW** | Eth_A/Eth_B and GOOSE/MMS evidence |
| SW-TV | Versioned test-vector pack (JSON/YAML) | 1 | eng time | **BUY NOW** | L1-P-01..04, L1-C-01..03, PF2 sequence from strategy doc |

### 2.2 Serial / Modbus physical (L2)

| Ref | Item | Suggested class / example | Qty | Est. EUR | Status | Doc |
|-----|------|---------------------------|----:|--------:|--------|-----|
| A-USB485 | Isolated USB-to-RS-485 adapter | FTDI/CH340 industrial isolated (e.g. Waveshare / DSD TECH / Moxa UPort class) | **2** | 25-60 ea | **BUY NOW** | Prefer **isolated**; buy 2 so one can stay on bus while second injects faults |
| A-CAB485 | RS-485 twisted-pair cable | Belden 3105A-class or CAT5e pair A/B + GND | 10 m | 15-30 | **BUY NOW** | Mark A/B/GND; keep a spare 2 m jumper |
| A-TERM | 120 ohm termination + bias kit | Plug-in 120R, optional 680R bias | 2 sets | 5-15 | **BUY NOW** | For terminate / unterminate fault cases |
| A-DB9 | DB9 male/female adapters / breakout | Matches Verdin Dev Board RS485 DB9 | 1-2 | 8-20 | **BUY NOW** | Kit has 1x RS485 DB9 — not opto RJ45 |
| A-POL | A/B polarity switch or screw-terminal block | DIN rail terminal + label | 1 | 5-15 | **BUY NOW** | Controlled A/B reverse tests |

### 2.3 Ethernet / protocol (L2)

| Ref | Item | Suggested class | Qty | Est. EUR | Status | Notes |
|-----|------|-----------------|----:|--------:|--------|-------|
| A-SW | Unmanaged GbE switch (5-8 port) | TP-Link / Netgear / industrial DIN if available | 1 | 20-60 | **BUY NOW** | Separate Eth_A / Eth_B / plant / PC ports with labels |
| A-ETH | CAT6 patch leads (labelled) | Short 0.5-2 m | 6 | 10-20 | **BUY NOW** | Labels: Eth_A, Eth_B, Plant1, Plant2, PC, Spare |
| A-TAP | Optional Ethernet TAP / mirror | Soft (Wireshark) first; hardware TAP later | 0-1 | 0-150 | **OPTIONAL** | Soft capture enough for Wave A |

### 2.4 Lab power / misc (Wave A)

| Ref | Item | Qty | Est. EUR | Status | Notes |
|-----|------|----:|--------:|--------|-------|
| A-PSU24 | Bench PSU 24 Vdc (shared with DI box later) | 1 | 40-90 | **BUY NOW** if not owned | Start DI tests at 24 V only |
| A-ESD | ESD mat / wrist strap | 1 | 15-30 | **OPTIONAL** | Recommended for SoM handling |
| A-LAB | Label printer or cable tags | 1 | 10-25 | **BUY NOW** | Non-negotiable for fault-injection clarity |

**Wave A hardware subtotal (typical):** ~**EUR 180-350** (excluding existing PC and Toradex kit).

---

## 3. Wave B — DI test box + DO lamps (**BUY NEXT**)

Product target remains **DI 10-120 Vdc x5** and **DO relay x3**. On the **eval kit**, DI/DO are GPIO/stub only — Wave B either:

1. exercises **application mock DI/DO** via USB-GPIO / relay hat until carrier exists, or  
2. waits for **custom carrier** and then uses the real field DI/DO circuits.

**Recommendation:** build a **24 V guarded fixture now** (safe), design the 48/110 V paths for the **carrier**, and do not energize 110 Vdc until carrier input design review passes.

### 3.1 DI fixture (24 V first)

| Ref | Item | Qty | Est. EUR | Status | Notes |
|-----|------|----:|--------:|--------|-------|
| B-PSU24 | 24 Vdc PSU (if not A-PSU24) | 1 | — | **HAVE** if Wave A bought | Fuse both rails |
| B-PSU48 | 48 Vdc PSU | 0-1 | 40-80 | **DEFER** | After carrier DI design freeze |
| B-PSU110 | 110 Vdc source | 0-1 | TBD | **DEFER** | Hazardous — competent person + current limit + E-stop |
| B-SW | Selector / toggle switches (named states) | 5 | 10-25 | **BUY NEXT** | Breaker Closed/Open, Alarm, Meter Fault, Permissive |
| B-FUSE | Fuses + holders (per channel / supply) | set | 10-20 | **BUY NEXT** | Current-limit every injection path |
| B-TB | DIN terminal blocks + DIN rail + enclosure | 1 kit | 25-50 | **BUY NEXT** | Guarded terminals; no bare 110 V banana plugs |
| B-ESTOP | Emergency disconnect / key switch | 1 | 15-30 | **BUY NEXT** | Required before any >24 V |
| B-LED | Status LEDs on fixture | 5 | 5-10 | **OPTIONAL** | Show injected DI state |

Named DI stimuli (map in software ICD later):

| Channel label | Meaning |
|---------------|---------|
| DI1 | Breaker Closed |
| DI2 | Breaker Open |
| DI3 | Alarm Active |
| DI4 | Meter Fault |
| DI5 | External Permissive |

### 3.2 DO observation

| Ref | Item | Qty | Est. EUR | Status | Notes |
|-----|------|----:|--------:|--------|-------|
| B-LAMP | 24 V indicator lamps / LED panels | 3 | 10-25 | **BUY NEXT** | One per PF2 / command / spare DO |
| B-PLCIN | Optional isolated DI module (PLC input) | 0-1 | 40-120 | **OPTIONAL** | Cleaner bounce/timing capture than lamps alone |
| B-SCOPE | Portable scope / logic analyzer | 0-1 | 0-TBD | **OPTIONAL** | Contact bounce / response time |

**Wave B subtotal (24 V fixture):** ~**EUR 80-180**.

---

## 4. Wave C — L3 real energy analyzer (**BUY NEXT** before field pilot)

Do **not** buy the analyzer until Wave A Modbus master + PF2 vectors pass on the simulator.

| Ref | Item | Example families | Qty | Est. EUR | Status | Blocks |
|-----|------|------------------|----:|--------:|--------|--------|
| C-AN | Energy analyzer (Modbus RTU) | Schneider PM / Janitza UMG / Carlo Gavazzi / Siemens PAC | 1 | 400-1,200+ | **TBD** lock model | L3 gate |
| C-MAP | Official Modbus register map PDF | Vendor portal for chosen model | 1 | 0 | **MISSING** until model lock | Software + ingest `ccli-modbus-analyzer-map` |
| C-PS | Analyzer supply + VT/CT test path | Per vendor | 1 | TBD | **TBD** | May need secondary injection; not always "type registers by hand" |
| C-CAB | RS-485 plant cable to analyzer | Same class as A-CAB485 | 1 | — | reuse | Match product pinout when carrier exists |

**L3 acceptance:** map on disk + ingested; every consumed register has address/type/scale/unit; readings agree with analyzer display within tolerance; disconnect/reboot cases pass.

---

## 5. Wave D — L4 power-system HIL (**DEFER**)

| Ref | Item | Qty | Est. EUR | Status | Notes |
|-----|------|----:|--------:|--------|-------|
| D-HIL | Typhoon HIL / OPAL-RT / RTDS access | campaign | rental or 10k-100k+ | **DEFER** | Closed-loop grid/inverter/protection |
| D-IED | Optional IED / GOOSE publisher HW | TBD | TBD | **DEFER** | Only if L4 scope needs physical IEDs |
| D-OMICRON | Omicron IEDScout / StationScout | 0-1 | license | **OPTIONAL** | Strong for 61850; not required to start L1/L2 |

L4 is **not** on the Phase 1 EUR 1,000 product prototype budget.

---

## 6. Controller under test (already on product BOM)

| Ref | Item | PN | Qty | Est. EUR | Status | Bench role |
|-----|------|-----|----:|--------:|--------|------------|
| U-KIT | Verdin Development Board + HDMI | **99991105** | 1 | ~300 | product BOM | L2 DUT: 2x GbE, RS485 DB9 |
| U-SOM | Verdin iMX8M Plus Quad 4GB IT | **0063** | 1 | 150-350 | product BOM | Compute / HAB / BSP |
| U-PSU | Kit 7-24 V supply (brick or bench) | — | 1 | 15-40 | **BUY NOW** (if not in kit) | Separate from DI PSU |

**C2 reminder:** kit RS485 is **not** product C2 (need 2x opto RJ45 on carrier). Wave A/B prove software + drivers; repeat on carrier.

---

## 7. Consolidated buy list (print / purchase)

### 7.1 Order this week (Wave A)

| Priority | Ref | Item | Qty | Est. EUR |
|---------:|-----|------|----:|--------:|
| 1 | A-USB485 | Isolated USB-RS485 adapters | 2 | 50-120 |
| 2 | A-SW | 5-8 port GbE switch | 1 | 20-60 |
| 3 | A-CAB485 + A-ETH + A-DB9 + A-TERM | Cables / DB9 / termination | 1 kit | 40-80 |
| 4 | A-PSU24 | 24 V bench PSU | 1 | 40-90 |
| 5 | A-LAB | Cable labels | 1 | 10-25 |
| Soft | SW-MB + SW-61850 + SW-CAP + SW-TV | Simulators + vectors | — | 0 + eng |

### 7.2 Order after first Modbus poll works (Wave B)

| Priority | Ref | Item | Qty | Est. EUR |
|---------:|-----|------|----:|--------:|
| 1 | B-TB + B-SW + B-FUSE + B-ESTOP | 24 V DI fixture kit | 1 | 60-125 |
| 2 | B-LAMP | DO indicator lamps x3 | 3 | 10-25 |

### 7.3 Order before field pilot (Wave C)

| Priority | Ref | Item | Qty | Est. EUR |
|---------:|-----|------|----:|--------:|
| 1 | C-AN | Lock analyzer model + buy | 1 | 400-1,200+ |
| 2 | C-MAP | Download + ingest register map | 1 | 0 |

---

## 8. Cost roll-up

| Bucket | Est. EUR | In product EUR 1k budget? |
|--------|---------:|---------------------------|
| Wave A bench HW | 180-350 | No — lab tooling |
| Wave B DI/DO fixture | 80-180 | No — lab tooling |
| Wave C analyzer | 400-1,200+ | No — lab/plant instrument |
| Toradex kit + SoM | 450-650 | **Yes** — product Phase 1 |
| Wave D HIL | rental / capital | No — later campaign |

Treat Waves A-C as a **lab CapEx** line separate from the ~EUR 1,000 single-unit CCI prototype BOM.

---

## 9. Documents still required for this bench

| Doc | Status | `source_id` / path |
|-----|--------|--------------------|
| Validation & mocking strategy | **HAVE / INGESTED** | `ccli-validation-strategy` |
| This bench BOM | **HAVE** | `ccli-mocking-bench-bom` |
| PF2 state-machine spec | **MISSING** | `ccli-app-pf2` (planned) |
| pymodbus scenario pack | **MISSING** | repo under `06-application/` |
| Analyzer Modbus map | **MISSING** | `ccli-modbus-analyzer-map` |
| USB-RS485 adapter DS (chosen MPN) | **MISSING** | `08-engineering/reference/` |
| DI fixture wiring diagram | **MISSING** | author with Wave B |

---

## 10. Mapping to C1-C10 / validation levels

| Control | Bench coverage |
|---------|----------------|
| C1 Eth_A/B | Wave A switch + dual kit GbE (Partial vs product 4-port) |
| C2 RS-485 | Wave A/B L2 on kit DB9; product 2x opto still on carrier |
| C3 IEC 61850 | Wave A clients on Ethernet; L4 for closed-loop GOOSE |
| C4 DI/DO | Wave B 24 V fixture; full 10-120 Vdc on carrier |
| C5 GNSS | Fault-inject "GPS lost" in SW first; HW later |
| C6 secure boot | Not this BOM — SoM/HAB path on product BOM |
| C10 power | Kit PSU for DUT; separate fused PSU for DI injection |

---

## 11. To do

1. Freeze **A-USB485** MPN (isolated, Linux `ttyUSB` stable) and download DS.  
2. Author `PF2` state machine + `tests/modbus/l1_vectors.yaml`.  
3. Build Wave A; run L1-P/C vectors end-to-end.  
4. Build 24 V DI fixture; **do not** buy 110 V source yet.  
5. Shortlist 2 analyzer models; obtain register maps before purchase.  
6. Keep L4 as a separate CapEx decision after L1-L3 evidence pack exists.

## 12. Keywords

`mocking bench bom`, `PF2 bench`, `USB-RS485`, `DI test box`, `DO lamp`, `energy analyzer`, `Wave A`, `Wave B`, `Wave C`, `pymodbus`, `99991105`, `ccli-mocking-bench-bom`

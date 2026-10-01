# CCI Validation & Mocking Strategy - PF2 Bench Ladder

**Document ID:** CCLI-VAL-001  
**Revision:** 1.1  
**Date:** 2026-09-15  
**RAG source_id:** `ccli-validation-strategy`  
**Purpose:** Define a progressive, evidence-based bench strategy for validating Modbus metering, PF2 logic, IEC 61850/104, digital I/O, logging, and safe fault handling before plant deployment.  
**Companions:** `CCI_Mocking_Bench_BOM.md` | `CCI_Prototype_BOM.md` | `CCI_Vendor_Selection.md` | `CCI_Project_Roadmap.md`

> This ladder validates engineering behavior; it does **not** replace accredited CEI 0-16 demonstrations, IEC 61850 conformance testing, EMC/safety testing, or IEC 62443 product/process assessment.

---

## 1. Recommended progression

| Level | Bench | Main value | Phase 1 gate |
|-------|-------|------------|--------------|
| **L1 - Software mock** | PC-hosted Modbus/IEC protocol simulators | Fast, deterministic application tests | **Required first** |
| **L2 - Interface HIL** | Controller + physical Ethernet/RS-485 + emulator PC | Drivers, timing, framing, cable faults | **Recommended Phase 1** |
| **L3 - Real analyzer** | Controller + selected energy analyzer | Vendor register map and device behavior | **Required before field pilot** |
| **L4 - Power-system HIL** | Typhoon HIL / OPAL-RT / RTDS-class real-time simulator | Closed-loop grid/inverter/protection behavior | Later system-validation campaign |

Do not jump directly to L3 or L4. Keep the same versioned test vectors and expected outcomes across all levels so failures can be isolated to software, interface, device integration, or closed-loop behavior.

---

## 2. Level 1 - software-only mocking

Run a Modbus server on an engineering PC. The controller polls it using the same application code, register map abstraction, scaling, timeout policy, and PF2 state machine intended for the product.

```text
Engineering PC
+-- Modbus TCP/RTU simulator
    +-- P = 850 kW
    +-- Q = 100 kvar
    +-- power factor = 0.98
```

Candidate tools: **pymodbus simulator**, **ModbusPal**, **QModMaster**, and **ModRSsim2**. Tool selection is not frozen; record the exact version and configuration in each test report.

### 2.1 Minimum scenarios

| Test ID | Stimulus | Expected controller behavior |
|---------|----------|------------------------------|
| L1-P-01 | P = 500 kW | No curtailment |
| L1-P-02 | P = 700 kW | No curtailment |
| L1-P-03 | P = 950 kW | Curtailment request after configured debounce |
| L1-P-04 | P = 1,000 kW | Curtailment remains active; no repeated command storm |
| L1-C-01 | Meter timeout | Quality invalid, alarm/event, configured safe state |
| L1-C-02 | Illegal function / exception response | Diagnostic event; no stale value treated as valid |
| L1-C-03 | Bad CRC (RTU) | Frame rejected and counted; retry policy applied |
| L1-D-01 | Out-of-range / NaN-equivalent register pattern | Input rejected or clamped per ICD; alarm raised |
| L1-R-01 | Process restart | State recovery follows persistence and fail-safe policy |

**Important:** bad-CRC and malformed-frame injection may require a purpose-built serial fault proxy or test script; many general Modbus simulators only emit valid frames.

### 2.2 PF2 reference sequence

Illustrative rule only: **if valid active power P > 900 kW, request curtailment**.

| Time | P | Expected |
|------|--:|----------|
| 00:00 | 500 kW | No action |
| 00:30 | 700 kW | No action |
| 01:00 | 950 kW | Curtail after debounce |
| 01:30 | 1,000 kW | Curtail remains active |

The production test must also define hysteresis, debounce, minimum on/off time, stale-data timeout, command acknowledgement, manual override, restart behavior, and release threshold. A single `P > 900` comparison is not sufficient for a safe controller.

Verify every transition through:

- PF2 state-machine state and reason code
- digital-output intent (mocked at L1)
- IEC 61850 / IEC 104 / Modbus command intent, where applicable
- alarm generation and acknowledgement
- timestamped event and measurement logs
- input quality, source, engineering units, and scaling

---

## 3. Level 2 - physical-interface HIL (recommended)

Use the real controller serial and Ethernet interfaces while the plant-side device remains emulated.

```text
CCI controller
    |
    +-- RS-485 cable -- USB-to-RS-485 adapter -- Linux PC emulator
    +-- Ethernet ----- engineering switch ----- protocol test client
```

This validates:

- Linux UART/serial driver and device-tree mapping
- RS-485 direction control, polarity, termination, and bias
- configured baud rate, parity, stop bits, slave address, retries, and timeout
- real frame timing and recovery after disconnect/reconnect
- Ethernet interface binding and Eth_A/Eth_B policy
- controller CPU/load behavior under protocol traffic

### 3.1 C2 limitation on lead eval hardware

**TesPro TR400** (active DUT) provides **two RS485 ports** (A1/B1 → `/dev/ttyS1`, A2/B2 → `/dev/ttyS2`) plus **four configurable DIO** on the front panel. Lab wiring: USB-RS485 adapter on PC as Modbus slave on A1/B1; relay module on **DIO1** (curtail DO) and **DIO2** (permissive DI). Isolation class vs product AiLux requirement remains **PART** — confirm with TesPro before claiming plant-grade isolation.

Historical note: Toradex Verdin Dev Board **99991105** (archived path) had one non-isolated RS-485 DB9 — **not** the active lab DUT.

### 3.2 L2 fault injection

- unplug/reconnect RS-485 while polling
- reverse A/B during a controlled test
- remove termination or introduce a long/noisy cable
- use wrong baud/parity/slave ID
- delay responses beyond timeout
- disconnect Eth_A, Eth_B, and plant Ethernet independently
- restart the emulator while the controller remains online

For each case verify bounded retries, no task deadlock, invalid data quality, event logging, alarm lifecycle, and a defined safe state.

---

## 4. Level 3 - real energy analyzer

Connect the selected production-candidate analyzer, for example a Schneider PM, Janitza UMG, Carlo Gavazzi, or Siemens PAC family device. The exact model and firmware must be frozen in the test record.

A real analyzer validates:

- vendor register addresses, word order, signedness, scaling, and units
- update rate and communication latency
- device exception behavior and configuration persistence
- voltage, current, active/reactive/apparent power, frequency, power factor, and quality flags

No high-power plant is required for communications testing. Use the analyzer's supported test/simulation mode, low-voltage injection equipment, or a calibrated secondary-injection source as appropriate.

**Do not assume arbitrary register values can be manually configured on every analyzer.** Confirm the chosen model's test mode and obtain its official **Modbus register map**; this remains a P1 missing document in the prototype BOM.

### 4.1 L3 acceptance gate

- official register map is on disk and ingested
- every consumed register has address, type, scale, unit, validity rule, and polling rate
- controller readings agree with analyzer display/reference within declared tolerance
- loss/recovery and analyzer reboot tests pass
- firmware/model-specific deviations are documented

---

## 5. Level 4 - real-time power-system HIL

Use a Typhoon HIL, OPAL-RT, RTDS, or equivalent real-time platform when closed-loop interaction with a modeled grid, inverter, transformer, protection relay, and communications network is justified.

The controller may receive and emit:

- IEC 61850 MMS, reports, and GOOSE
- IEC 60870-5-104
- Modbus RTU/TCP
- DI/DO and analog/measurement signals through suitable I/O interfaces

Use L4 to test dynamic ramps, oscillation, command delays, protection events, communications congestion, failover, and interactions among PF2 logic, inverter controls, and DSO/operator commands.

L4 is valuable but expensive. It is **not required to start Phase 1**, and it does not itself certify CEI 0-16 or IEC 61850 conformance.

---

## 6. Digital-input test box (C4)

Product target: **5 opto-isolated DI channels, 10-120 Vdc, external field supply**.

Use a guarded, fused test fixture with selectable supported voltages (typically 24 V, 48 V, and 110 V where the final input design permits). Inject named states such as breaker closed/open, alarm active, meter fault, and external permissive.

Verify:

- asserted/deasserted thresholds and hysteresis
- polarity protection and channel isolation
- debounce and contact chatter behavior
- Linux GPIO/application state mapping
- event timestamp and source identity
- simultaneous-channel transitions
- open-wire/power-loss behavior where detectable

**Safety:** 110 Vdc is hazardous. Use current limiting, fusing, guarded terminals, an emergency disconnect, and competent personnel. Start at 24 V until the carrier input circuit has passed design review.

---

## 7. Dry-contact output tests (C4)

Connect each relay output to a safe indicator lamp, PLC input, or isolated digital-input module. Do not switch mains loads during early bench testing.

```text
PF2 active -> output policy permits actuation -> relay closes -> lamp/PLC input ON
```

Measure:

- command-to-contact response time
- contact bounce and application filtering
- power-up, reboot, watchdog, and power-loss state
- normally-open/normally-closed interpretation
- stuck relay / feedback mismatch handling if feedback is available
- manual override and command interlock behavior

The safe state must be specified by the system safety/control analysis; "relay open" is not automatically safe for every plant.

---

## 8. IEC 61850 and IEC 104 bench

```text
IED test client / protocol emulator
              |
          Ethernet switch
              |
         CCI controller
```

Test MMS reads, control writes with the configured control model, buffered/unbuffered reports, GOOSE publish/subscribe, dataset/configuration revision handling, time quality, disconnect/reconnect, and malformed or unauthorized requests.

Candidate tools include **Omicron IEDScout**, **Omicron StationScout**, and test clients built from **libiec61850**. libiec61850 is suitable for prototyping under GPLv3; a commercial license is required for a closed shipped product.

For IEC 60870-5-104, test connection supervision, sequence counters, time-tagged events, command select/execute if used, spontaneous transmissions, and reconnect behavior.

---

## 9. Cross-interface failure matrix

| Failure | Required checks |
|---------|-----------------|
| Meter disconnected / RS-485 cut | Invalid quality, alarm, event, bounded retry, safe PF2 behavior |
| Eth_A disconnected | DSO link alarm; plant functions remain bounded |
| Eth_B disconnected | Operator link alarm; no unauthorized fallback |
| GNSS lost | Time-quality downgrade; holdover policy; event |
| LTE lost | Backup-link alarm; primary control path unaffected |
| Process crash / watchdog reset | Controlled restart; outputs go to defined state |
| Stale measurement | Never treated as current; command inhibition or fallback per policy |
| Conflicting DSO/operator/local command | Deterministic priority and audit trail |

The most important acceptance criterion is not "an alarm appeared"; it is that a communication fault cannot cause uncontrolled curtailment or silent use of stale data.

---

## 10. Practical Phase 1 PF2 bench (TR400 — as-built)

```text
Engineering PC (192.168.0.x)
+-- IEC 61850 / IEC 104 client and traffic capture
+-- pymodbus RTU slave (USB-RS485)
+-- SSH to TR400 (192.168.0.1)
             |
        LAN2 (or mapped Eth_A/B — confirm ip link)
             |
       TesPro TR400 (TesproOS / OpenWrt)
        +-- A1/B1 (/dev/ttyS1) <---> PC Modbus slave
        +-- DIO1 --> relay module (curtailment DO)
        +-- DIO2 <-- permissive DI
        +-- A2/B2 spare plant serial
```

**On-target path:** same `test_vectors/` on **TG544** (`platform_tg500`, `lab_tr400.yaml`). Pi mock **removed from scope**.

This bench can validate most application behavior without a solar plant, inverter, or grid connection:

- Modbus RTU/TCP acquisition
- PF2 state machine and command arbitration
- IEC 61850 and IEC 104 application behavior
- DI/DO mapping on suitable hardware
- logging, alarms, time quality, and fault handling
- basic cybersecurity controls and interface separation

It cannot prove final carrier isolation/EMC, analyzer accuracy, full power-system dynamics, or product conformance.

---

## 11. Test evidence required

Every test case should record:

| Field | Required content |
|-------|------------------|
| Test ID / revision | Stable identifier linked to requirement C1-C10 |
| Build identity | Git commit, image/BSP version, configuration hash |
| Equipment | Model, serial number, firmware, calibration status where relevant |
| Topology | Ports, cables, addresses, baud/parity, VLAN/interface binding |
| Stimulus | Exact values and fault timing |
| Expected result | State, output, alarm, log, timing limit |
| Actual result | Captures, logs, contact measurement, screenshots |
| Verdict | PASS / FAIL / BLOCKED with defect reference |

Prefer machine-readable test vectors (JSON/YAML/CSV) plus an automated runner for L1/L2. Preserve packet captures, serial traces, and controller logs as evidence artifacts.

---

## 12. To do

1. Author the PF2 state-machine specification: thresholds, hysteresis, debounce, stale-data timeout, priorities, and safe states.  
2. Create a versioned Modbus meter register-map abstraction and `pymodbus` scenario runner.  
3. Build L1 test vectors for normal ramps, boundary values, timeout, exceptions, malformed data, and restart.  
4. Buy two known-quality isolated USB-to-RS-485 adapters for engineering use.  
5. Obtain the selected real analyzer's official Modbus register map.  
6. Define DI/DO safe states before energizing the carrier test box.  
7. Add IEC 61850 SCL/test model and IEC 104 interoperability cases.  
8. Define which L4 scenarios justify external HIL cost after L1-L3 results.

## 13. RAG keywords

`validation ladder`, `PF2 bench`, `Modbus simulator`, `pymodbus`, `hardware in the loop`, `HIL`, `RS-485`, `energy analyzer`, `bad CRC`, `meter timeout`, `curtailment`, `DI test box`, `dry contact`, `IEC 61850`, `GOOSE`, `MMS`, `IEC 104`, `safe state`, `fault injection`, `ccli-validation-strategy`

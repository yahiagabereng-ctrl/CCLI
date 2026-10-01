# DRC-60A UPS wiring — TG544 lab (10-pin AIO)

**Policy:** All four **DIO** channels are reserved for **PF2 / CEI** regulation. The MEAN WELL **DRC-60A** UPS/charger (“BMS” in lab notes) is monitored and optionally controlled via the **AI/AO** block only.

**Diagram:** `Architecture/TR400_Lab_Platform.drawio` → page **6-DRC60A-UPS-Full-IO-Map**.

**Corpus:** `knowledge-base/08-engineering/CCI_TR500_IO_Peripheral.md`  
**Config:** `apps/ccli/config/lab_tr400.yaml` → `/etc/ccli/lab.yaml`

---

## Requirements

| ID | Requirement |
|----|-------------|
| REQ-PWR-UPS-001 | TG544 runs from DRC-60A +V/−V with 12 V AGM backup |
| REQ-PWR-UPS-002 | Battery voltage readable on AI1 without exceeding ADC limit |
| REQ-PWR-UPS-003 | AC OK and battery-low status available (AI2 ladder or AI1 software) |
| REQ-PWR-UPS-004 | Optional AO1 load-shed fails safe (default = power ON) |
| REQ-IO-PF2-001 | DIO1–4 not used for UPS — PF2 only |

---

## Full I/O mapping (10-pin block)

| Silk | ubus / API | Mode | Domain | Role | CCLI key | Bench |
|------|------------|------|--------|------|----------|-------|
| **DIO1** | ch 1 | DO | PF2 | Curtailment relay **K1** | `io.do_curtail` | **PASS** |
| **DIO2** | ch 2 | DI | PF2 | Permissive interlock | `io.di_permissive` | **PASS** |
| **DIO3** | ch 3 | DI | PF2 | Spare (PI trip, external alarm) | `io.di_pf2_spare` | UNTESTED |
| **DIO4** | ch 4 | DO | PF2 | Spare (curtail ack, annunciator) | `io.do_pf2_spare` | UNTESTED |
| **AI1** | vendor AI0? | AI | UPS | Battery voltage (divider) | `analog.ai_battery_v` | NOT TESTED |
| **AI2** | — | AI | UPS | AC OK + Bat low ladder | `analog.ai_ups_status` | NOT TESTED |
| **AO1** | — | AO | UPS | Load-shed drive → **K2** | `analog.ao_load_shed` | NOT TESTED |
| **AO2** | — | AO | — | Spare | `analog.ao_spare` | NOT TESTED |
| **AOG** | — | GND | UPS | Analog return (Bat−) | `analog.analog_gnd` | — |
| **GND** | — | GND | Common | Digital / DIO reference | — | **PASS** |

---

## DRC-60A TB2 → TG544

| DRC-60A TB2 | Net | TG544 / circuit |
|-------------|-----|-----------------|
| Pin 1 (−V) | UPS − | 12–30 V **−** input, AOG, divider R2, ladder return |
| Pin 2 (+V) | UPS + | 12–30 V **+** input (via **K2** NC if load-shed fitted) |
| Pin 3 (Bat+) | Battery + | R1 top of divider → **AI1** |
| Pin 4 (Bat−) | Battery − | AOG, GND reference |
| Pins 5–6 | AC OK relay | AI2 ladder (see below) |
| Pins 7–8 | Battery low relay | AI2 ladder (see below) |

**Power:** DRC-60A AC in → charges 12 V AGM; TB2 +V/−V feeds TG544 primary input.

Reference: [MEAN WELL DRC-60 datasheet](https://componentstree.com/wp-content/uploads/2024/10/DRC-60-Datasheet.pdf) — alarm contacts ≤ 30 V / 1 A.

---

## Analog circuits

### AI1 — battery voltage divider

```
Bat+ ── R1 120 kΩ ──┬── AI1
                    │
                   R2 39 kΩ ── AOG ── Bat−
                    │
                   100 nF to AOG
Optional: 3.3 V zener AI1 → AOG
```

Approx ratio: V(AI1) ≈ Vbat × 39k / (120k + 39k) ≈ **0.245 × Vbat**  
At 14.4 V charge: ~3.5 V at AI1 — **confirm ADC range** via vendor status before energising.

### AI2 — status ladder (optional hardware)

Use **PS1 +5 V** reference and pull-ups; read combined or separate channels after bench characterisation:

| Contact | Condition | Suggested sense |
|---------|-----------|-----------------|
| AC OK (5–6) | Closed = mains present | Pull AI2 high via 10 kΩ when open |
| Bat low (7–8) | Closed = Vbat < 11 V | Second divider or digital OR into AI2 |

**Software fallback:** derive `battery_low` from `ai_battery_v` if AI2 ladder not wired.

### AO1 — optional load-shed (fail-safe)

```
AO1 ── 10 kΩ ── base 2N2222 ── emitter GND
                      collector ── K2 coil (−) 
K2 coil (+) ── PS1 +5 V
K2 contacts: NC in series with DRC-60A +V → TG544 +V
Flyback: 1N4148 across K2 coil
```

- **AO1 = 0 V** → transistor off → **K2 de-energised** → NC closed → **power ON** (default).
- **AO1 high** → K2 energised → breaks +V (load shed). Use only after AO scaling confirmed.

---

## PF2 DIO wiring (unchanged from smoke test)

| Device | Connection |
|--------|------------|
| **K1** (5 V relay module) | Coil: external **+5 V** and GND; IN ← **DIO1**; COM/NO → curtailment sim |
| **Permissive** | **DIO2** → switch → **GND** (open = 0, closed = 1) |

Prep DIO2 once:

```bash
uci set didoservice_v2.channel_2.mode=di
uci commit didoservice_v2
/etc/init.d/didoservice_v2 restart
```

For DIO3 as DI:

```bash
uci set didoservice_v2.channel_3.mode=di
uci commit didoservice_v2
/etc/init.d/didoservice_v2 restart
```

---

## BOM (lab)

| Ref | Function | Part | Status |
|-----|----------|------|--------|
| UPS1 | 12 V UPS/charger | MEAN WELL DRC-60A | PLANNED |
| BAT1 | Backup | 12 V AGM (AH TBD) | PLANNED |
| K1 | PF2 curtailment | 5 V relay module | HAVE |
| K2 | Load-shed (optional) | Songle SRD-05VDC-SL-C or G5V-2 | PLANNED |
| Q1 | AO driver | 2N2222 | PLANNED |
| D1 | Flyback | 1N4148 | PLANNED |
| R1,R2 | Divider | 120 kΩ, 39 kΩ 1% | PLANNED |
| C1 | Filter | 100 nF | PLANNED |
| PS1 | Relay / ladder | +5 V bench supply | HAVE |

---

## Verification

| Step | Action | Pass |
|------|--------|------|
| V-01 | DMM Bat+ vs AOG (no TG544 AI connected) | V(AI1 node) ≤ 3.3 V at max charge |
| V-02 | Connect AI1; read vendor ADC / future CCLI | Monotonic with charger on/off |
| V-03 | Open AC OK contact | AI2 or software shows mains lost |
| V-04 | Discharge to < 11 V (sim or bench PSU) | Bat low asserted |
| V-05 | AO1 = 0 | K2 off, TG544 powered |
| V-06 | AO1 ramp (after scale known) | K2 toggles without overshoot |
| V-07 | DIO1/DIO2 smoke | Same as `lab/TR400_DIO_SMOKE_TEST.md` |

---

## Risks

| ID | Risk | Mitigation |
|----|------|------------|
| R-AIO-001 | ADC input > 3.3 V | Divider + optional zener; V-01 before wire-up |
| R-AIO-002 | AO assumed 0–10 V | Measure idle AO1 before K2 install |
| R-AIO-003 | DIO used for UPS | Policy: DIO PF2-only; review YAML in PR |
| R-UPS-001 | K2 fails short | NC contact wiring; default de-energised |

---

## Cross-reference

- DIO smoke: `lab/TR400_DIO_SMOKE_TEST.md`
- Platform diagram: `Architecture/TR400_Lab_Platform.drawio` pages 1–6
- IO peripheral doc: `knowledge-base/08-engineering/CCI_TR500_IO_Peripheral.md`

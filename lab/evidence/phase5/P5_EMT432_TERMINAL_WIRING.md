# EMT432 Base Chronos — terminal wiring (230 V 1P bench)

**Photo:** lab asset `image-69fe627a-6354-4301-822c-32ac6da01db5.png` (2026-10-05)  
**Meter:** EMT432 **I** (TA 5 A) · order code suffix **I** = 5 A CT inputs, not Rogowski  
**Corpus:** [CCI_Chronos_EMT432_Extract.md](../../../knowledge-base/08-engineering/CCI_Chronos_EMT432_Extract.md)

---

## Front panel

| Item | Label | Note |
|------|-------|------|
| Display | — | 320×240 colour; local readout for V, f, P, Q |
| LEDs | POW · Fault · AL1 · USB | **POW** = aux supply OK |
| USB | USB-A | config / export (not used on P5 bench) |

---

## Terminal block (photo mapping)

```text
TOP ROW (left → right)
┌────┬────┬────┬────┬──────────┬──────┐
│ L3 │ L2 │ L1 │ N  │ Pow+ Pow-│ OUT1 │
│    │    │    │    │ 18…36 Vdc│  DO  │
└────┴────┴────┴────┴──────────┴──────┘

BOTTOM ROW (left → right)
┌─────────┬─────────┬─────────┬─────┬──────────┐
│ I3 S1 S2│ I2 S1 S2│ I1 S1 S2│ G B A│ Ethernet │
│  (CT L3)│  (CT L2)│  (CT L1)│ RS485│  RJ45    │
└─────────┴─────────┴─────────┴─────┴──────────┘
```

| Terminal | Function | 1P bench (230 V L-N) |
|----------|----------|----------------------|
| **L1** | Phase voltage | **230 V live** |
| **N** | Neutral voltage | **230 V neutral** |
| **L2, L3** | Other phases | **Leave open** — do not tie together |
| **Pow+ / Pow-** | Aux supply | **18–36 V DC only** (e.g. 24 V lab PSU) — **never mains 230 V** |
| **OUT1** | Digital output | Not used on P5 bench |
| **I1 S1, S2** | CT inputs line 1 | Split-core **5 A TA** on same conductor as L1 (when available) |
| **I2, I3** | CT lines 2/3 | **Leave open** (1P mode) |
| **G, B, A** | RS485 | **G** = GND · **B** = B− · **A** = A+ → TG544 **A1/B1** |
| **Ethernet** | Modbus TCP | Optional; P5 bench uses RS485 |

---

## Recommended bench wiring

### Step 1 — Aux power (first)

1. Connect **24 V DC** (within 18–36 V) to **Pow+ / Pow-**.
2. Confirm **POW** LED on front panel.
3. Meter boots without mains — safe to configure comms before L1/N.

### Step 2 — Voltage (230 V monofase)

1. **L1** ← phase (live) · **N** ← neutral.
2. Instrument is **Class II** — **do not bond PE** to meter chassis or terminals.
3. Set connection mode to **single phase** (register `40232` bits 1–2 = `0`, or meter menu).

### Step 3 — Current (optional for P/Q)

1. Without CT: **V** and **f** valid on display; **I, P, Q ≈ 0**.
2. With **5 A split-core CT** on the line feeding L1:
   - CT secondary → **I1 S1** and **I1 S2**.
   - Arrow on CT toward load (verify sign of P on display).
3. **Rogowski / 333 mV** inputs (**V** suffix) — **not available** on this bench.

### Step 4 — RS485 to TG544

| EMT432 | TG544 screw block | Notes |
|--------|-------------------|-------|
| **A** | **A1** (A+) | Twisted pair with B |
| **B** | **B1** (B−) | Swap A/B if no response |
| **G** | **GND** (if present) | Common reference; short cable may work without G |

- DUT path: **`/dev/ttyS1`** · **9600 8N1** · slave ID **1** (factory default).
- Termination: off for short lab hop; enable 120 Ω only if bus is long or noisy.

---

## Safety

| Risk | Mitigation |
|------|------------|
| 230 V on **Pow** | **Pow = 18–36 V DC only** — label PSU before energising |
| PE on Class II meter | No earth bond on instrument |
| Live RS485 work | Power down or use isolated USB-RS485 for first probe |
| CT open-circuit | Never open CT secondary while primary energised |

---

## Modbus registers (1P bench)

Use **P1 / Q1** (not P_SUM / Q_SUM):

| Quantity | PLC addr | Offset @40001 | Unit |
|----------|----------|---------------|------|
| P1 | 40933 | **932** | W (float32 BE) |
| Q1 | 40941 | **940** | VAR (float32 BE) |

ccli adapter stores raw float into `p_kw` / `q_kvar` — **divide by 1000** until scale support lands (see bench plan).

---

## Verification checklist

- [ ] POW LED on with 24 V aux only
- [ ] Display shows ~230 V L-N, ~50 Hz (IT grid)
- [ ] Modbus FC3 @ offset 932 returns plausible P1 (W)
- [ ] With CT: P sign matches load direction
- [ ] TG544 `ccli` poll on `/dev/ttyS1` matches meter display (×0.001 for kW)

**Status:** **WIRING DOCUMENTED** — awaiting first Modbus poll evidence (P5-06).

# TR400 / TG544 — Phase 1 bench hardware configuration

**DUT:** TG544 @ `192.168.1.130`  
**Status (2026-09-22):** **GD32 online** (reflash) · **Modbus A2/B2** → `/dev/ttyS2` · **DIO1 = DO (ch1)** · **DIO2 = DI (ch2)**  
**SSH verify 2026-09-22:** `gd32.status` **online true**, FW **GD32-DIDO-F v1.0.1**, `/dev/gd32_uart` → `ttyUSB0`, `set_relay` ch1 **OK**
**Runbook:** `lab/PHASE1.md` · **Modbus:** `lab/MODBUS_RTU_LAB.md` · **DIO:** `lab/TR400_DIO_SMOKE_TEST.md`

### Bench signal flow

```text
  PC Modbus slave (COMx, ID 1, 9600 8N1)
       │  RS485
       │  A ↔ A2
       │  B ↔ B2
       ▼
  TG544 /dev/ttyS2  ──►  CCLI Modbus master  ──►  PF2 FSM
                                                    │
  DIO2 permissive (12 V key) ───────────────────────┤
                                                    ▼
                                              DIO1 curtail relay
```

---

## 1. Connection map (single source of truth)

| Layer | Curtail (output) | Permissive (input) |
|-------|------------------|---------------------|
| **Silkscreen** | **DIO1** | **DIO2** |
| **Schematic** | IO1 | IO2 |
| **ubus `dido_v2`** | **channel 1** | **channel 2** |
| **UCI mode** | `channel_1.mode=do` (default) | `channel_2.mode=di` |
| **`lab_tr400.yaml`** | `io.do_curtail.gpio: 1` | `io.di_permissive.gpio: 2` |
| **CCLI HAL** | `set_relay` ch **1** | `gd32.read_di` → **`di[0]`** ch1, **`di[1]`** ch2 |
| **CEI / lab role** | REQ-IO-001 curtail | REQ-LAB-003 permissive gate |

**Code path:** `apps/ccli/platform/platform_tg500/hal_gpio_ll.c` — logical `gpio` **equals** ubus **channel** (1–4).

**Actuation:** `DIO1 ON ⇔ PF2 curtailment_active AND permissive OK` (`svc_io.c` / `ccli_main.cpp`).

---

## 2. Field wiring (2026 lab bench)

### DIO1 — output → SCH-32F-1A-12SQ coil (~0.038 A @ 12 V)

```text
  +12 V ──► coil (+) ──► coil (−) ──► DIO1 only
  PSU (−) ──► TG544 GND          (separate wire; do NOT tie coil (−) to GND at DIO1)
  Flyback diode across coil (cathode at +12 V side)
```

| DIO1 state | Pin vs GND | Relay |
|------------|------------|-------|
| OFF (safe) | ~1.5 V | de-energised |
| ON (sink) | ~0 V | energised |

Yaml: `do_curtail.active_high: false` (open-drain active-low drive).

### DIO2 — input → 12 V permissive key (NO)

```text
  +12 V ──► [ key NO ] ──► DIO2
  PSU (−) ──► TG544 GND
```

| Key | Expected `gd32.read_di` **`di[1]`** | Permissive (CCLI) |
|-----|-------------------------------------|-------------------|
| Open | **0** | BLOCKED |
| Closed | **1** | OK |

Yaml: **`di_permissive.active_low: true`** on TG544 — with this HAL, logical **OK** when GD32/ubus state is **1** (same as 2026-09-15 GND-short smoke test). Do **not** set `active_low: false` unless you re-measure and invert (see §4).

Production lab: **`permissive_bypass: false`** so DIO2 gates DIO1.

### Modbus RTU — three paths (do not mix)

> **P5 bench freeze (2026-10-01):** PC on **A1/B1** → **`/dev/ttyS1`**.  
> Canonical map: [`lab/evidence/phase5/P5_A1B1_RS485_HW_MAP.md`](evidence/phase5/P5_A1B1_RS485_HW_MAP.md)

| Silkscreen | LuCI name | Linux device | TG544 r601 status |
|------------|-----------|--------------|-------------------|
| **A1 / B1** | **RS485-1** | **`/dev/ttyS1`** | **PASS** — **current P5 bench** (P=450 kW verified) |
| A2 / B2 | TR400 manual “RS485-2” | `/dev/ttyS2` | **FAIL** — `uart:unknown` in `/proc/tty/driver/serial` |
| *(internal FTDI)* | LuCI **RS485-2** | `/dev/rs485_2_uart` → `ttyUSB1` | Worked Phase 1 (2026-09-22) — **not** wired to A2/B2 screws |

| Layer | Value (**A1/B1 bench — active**) |
|-------|----------------------------------|
| **`lab.yaml`** | `modbus.device: /dev/ttyS1` |
| **Repo yaml** | `apps/ccli/config/lab_tr400_cleartext_tsp_ttyS1.yaml` |
| **Parameters** | 9600 8N1 · slave ID **1** · holding reg **40001** = P kW (float32 BE) |
| **LuCI** | RS485-1 enabled · device `/dev/ttyS1` · 9600 8N1 |

**Wiring to PC USB-RS485 adapter (A1/B1 — verified 2026-10-01):**

| TG544 | PC dongle |
|-------|-----------|
| **A1** | **A** (+) |
| **B1** | **B** (−) |
| **GND** | **GND** |

Swap A↔B once if no FC03 trace. Short lab cable; 120 Ω termination only if the bus is long or noisy.

**End-to-end pass:** PC `lab/modbus_rtu_slave.py` → CCLI `P=450kW` every ~4 s → MMS TotW → TesPro `PdC_TotW=450`.

---

## 3. One-time software prep (TG544)

```bash
# Channel 2 = DI (persist)
uci set didoservice_v2.channel_2.mode=di
uci commit didoservice_v2
/etc/init.d/didoservice_v2 restart

# GD32 mode (required for live DI — see hal_gpio_ll.c)
ubus call dido_v2 set_mode '{"channel":2,"mode":"DI"}'
ubus call dido_v2 gd32.set_mode '{"channel":2,"mode":"DI"}'

# Channel 1 = DO (usually default)
ubus call dido_v2 status
# expect: channel 1 mode DO, channel 2 mode DI
```

Deploy config from repo:

```bash
# On PC: pscp apps/ccli/config/lab_tr400.yaml → /etc/ccli/lab.yaml
/usr/sbin/ccli --config /etc/ccli/lab.yaml
# expect stderr:
#   svc_io: ready do_curtail=GPIO1(active_low) di_permissive=GPIO2(active_low)
#   (no "permissive_bypass=on" warning)
```

---

## 4. Connection verification (run on TG544)

Copy or run `lab/tr400-io-verify.sh` on the device, or execute manually:

```bash
# --- Map check ---
ubus call dido_v2 status | grep -E '"channel"|"mode"|"state"'

# --- DO ch1 / DIO1 (relay click or DMM DIO1–GND) ---
ubus call dido_v2 set_relay '{"channel":1,"value":0}'   # OFF safe
ubus call dido_v2 set_relay '{"channel":1,"value":1}'   # ON  (~0 V vs GND)
ubus call dido_v2 set_relay '{"channel":1,"value":0}'   # OFF

# --- DI ch2 / DIO2 (key open vs closed) ---
ubus call dido_v2 gd32.read_di '{}'
# key OPEN:   "di":[?,0,...]  → index 1 == 0
# key CLOSED: "di":[?,1,...]  → index 1 == 1

# --- Modbus A1/B1 (/dev/ttyS1) — P5 bench ---
ls -l /dev/ttyS1 /dev/ttyS2
grep -A8 '^modbus:' /etc/ccli/lab.yaml
# expect: device: /dev/ttyS1

# --- CCLI mapping (optional) ---
/usr/sbin/ccli --gpio-test --config /etc/ccli/lab.yaml
# toggles DO on gpio 1; prints permissive from gpio 2
```

**Pass criteria**

| Check | Pass |
|-------|------|
| ubus ch **1** toggles **DIO1** / relay | ☐ |
| ubus ch **2** `di[1]` follows key | ☐ |
| `lab.yaml` `gpio` **1** and **2** match above | ☐ |
| **`modbus.device`** = **`/dev/ttyS2`** (A2/B2) | ☐ |
| PC slave + CCLI → **`P=`** kW in log | ☐ |
| CCLI log `permissive=OK` only when key closed | ☐ |
| `ubus call dido_v2 gd32.status` → **`online`: true** | ☐ |

### GD32 offline (`/dev/gd32_uart` missing)

On TG544, field DIO is on the **GD32 coprocessor** over an internal **FT232** USB link (hotplug creates `/dev/gd32_uart` → `ttyUSB*`).

If `gd32.status` shows **`online`: false**, `set_relay` returns **GPIO write failed**, or `gd32.read_di` **GD32 timeout**:

```bash
ls -la /dev/gd32_uart /dev/ttyUSB*
logread | grep -iE 'gd32|FT232' | tail -20
```

Expect **`GD32 FT232R not present yet`** or **`open /dev/gd32_uart failed`** when the coprocessor USB is not enumerated. **UCI ch1=DO / ch2=DI can still look correct** while I/O does not move — fix GD32 link first (power cycle, cold boot, vendor service), then re-run `lab/tr400-dio-factory.cmds`.

---

## 5. Polarity note (yaml vs 12 V “HIGH”)

`hal_gpio_ll.c` maps ubus DI **1 → raw 0**, **0 → raw 1**. `drv_gpio.c` treats yaml **`active_low: true`** as logical permit when **raw == 0**, i.e. when **ubus/GD32 state is 1**.

So **12 V key closed (state 1)** still uses **`active_low: true`** in `lab_tr400.yaml`. Setting **`active_low: false`** inverts permissive unless HAL changes.

---

## 6. Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.1 | 2026-09-22 | GD32 online post-reflash; SSH verify; GD32 offline troubleshooting § |
| 1.0 | 2026-09-22 | DIO1 DO / DIO2 DI; Modbus A2/B2 → ttyS2; verify procedure |

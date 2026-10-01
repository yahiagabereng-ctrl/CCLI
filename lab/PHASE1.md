# Phase 1 lab — Modbus RTU → PF2 → DIO (TG544)

**Document ID:** CCLI-LAB-PHASE1-001  
**Revision:** 1.0  
**Date:** 2026-09-17  
**Platform:** TesPro TG544 / TR500 (`platform_tg500`) — **192.168.1.130**  
**Status (frozen 2026-09-22 EOD):** Modbus **PASS** on **`/dev/rs485_2_uart`** (LuCI RS485-2); static regulation **PASS**; **DIO curtail walk pending** (permissive key). Resume: **`lab/PHASE1_LAB_SESSION_FREEZE_2026-09-22.md`**

---

## 1. Objective

Prove the **Phase 1 control path** on the TG544 lab DUT:

```text
Modbus P (kW)  →  PF2 FSM  →  DIO1 (curtail DO)
                      ↑
              DIO2 (permissive DI)
```

This is a **local lab loop**. It does **not** implement DSO **`Wlim`** (Allegato T) or IEC 61850 on Eth_A — those are later phases.

| In scope (Phase 1) | Out of scope |
|--------------------|--------------|
| Modbus RTU master on TG544 | IEC 61850 / IEC 60870-5-104 |
| PC or bench Modbus slave (P kW) | DSO `Wlim` setpoint path |
| PF2 threshold FSM (900/850 kW) | Product-grade RS485 isolation sign-off |
| DIO1/DIO2 via `ubus dido_v2` | Secure boot / TPM |
| Deploy `ccli` binary + `lab.yaml` | Pi / `platform_pi` |

---

## 2. Bench topology

**Current wiring (2026-09-22):** PC USB-RS485 ↔ TG544 **LuCI RS485-2** (`/dev/rs485_2_uart`); DIO1/DIO2 per **`lab/TR400_HW_IO_CONFIG.md`**. (A2/B2 `/dev/ttyS2` not used — UART unbound on current FW.)

```text
  ┌──────────────────── PC ────────────────────┐
  │  pymodbus slave (COMx)                     │
  │  lab/modbus_rtu_slave.py                   │
  │  Reg 40001 = P kW (float32 BE)             │
  └──────────────────┬─────────────────────────┘
                     │ RS485 (9600 8N1, ID 1)
                     │  A ↔ A2
                     │  B ↔ B2
  ┌──────────────────▼─────────────────────────┐
  │  TG544 @ 192.168.1.130                     │
  │  /usr/sbin/ccli  (Modbus master)           │
  │  device: /dev/ttyS2                        │
  │       ↓                                    │
  │  PF2 FSM (900 kW / 30 s debounce)          │
  │       ↓                                    │
  │  DIO1 DO → relay K1 (curtail)              │
  │  DIO2 DI ← 12 V permissive key (NO)       │
  └────────────────────────────────────────────┘
```

**I/O map:** `lab/TR400_HW_IO_CONFIG.md` — **ubus ch1 = DIO1 DO**, **ch2 = DIO2 DI**; yaml `gpio: 1` / `gpio: 2`.

### Alternate Modbus ports

| Bench wiring | `modbus.device` | Notes |
|--------------|-----------------|-------|
| **A2/B2 screw terminals** | **`/dev/ttyS2`** | **Use this for A2/B2** |
| A1/B1 screw terminals | `/dev/ttyS1` | RS485-1 |
| USB dongle on TG544 USB | `/dev/ttyUSB0` | Check `ls /dev/ttyUSB*` |
| TesPro UI “RS485-2” path | `/dev/rs485_2_uart` | **Not A2/B2 on lab unit** — resolves to `ttyUSB1` |

> **Important:** The TesPro web UI label “RS485-2” (`/dev/rs485_2_uart`) is a **vendor symlink** and may **not** match the **A2/B2** pins. When wired to A2/B2, always configure **`/dev/ttyS2`**.

Confirm on device:

```bash
ls -l /dev/ttyS2 /dev/rs485_2_uart
readlink -f /dev/rs485_2_uart   # lab unit: /dev/ttyUSB1
```

---

## 3. Requirements (Phase 1)

| ID | Requirement | Verification |
|----|-------------|--------------|
| REQ-LAB-001 | CCLI reads P (kW) from Modbus RTU slave | Status logs every ~5 s; no perpetual RTU timeout stall |
| REQ-LAB-002 | PF2 enters curtail when P > 900 kW for ≥ 30 s | Log / event: curtailment active |
| REQ-LAB-003 | DIO1 actuates only when permissive OK | DIO2 open → no actuation; DIO2→GND → `curtail_on` |
| REQ-LAB-004 | Stale Modbus → safe state (no spurious curtail) | Stop PC slave → DIO1 OFF within ~10 s |
| REQ-LAB-005 | DIO via `dido_v2` ubus (not libgpiod) | `hal_gpio_ll: TesPro dido_v2 via ubus` in log |

---

## 4. Key files

| File | Role |
|------|------|
| `apps/ccli/config/lab_tr400.yaml` | Master config → `/etc/ccli/lab.yaml` |
| `lab/modbus_rtu_slave.py` | PC Modbus RTU slave (inverter mock) |
| `lab/tg544-openwrt/deploy-phase1.ps1` | Windows deploy script |
| `lab/tg544-openwrt/DEPLOY_CCLI.md` | Build + APK install |
| `lab/TR400_DIO_SMOKE_TEST.md` | DIO bench procedure (PASS) |
| `lab/PF2_Limits_and_Actions.md` | Thresholds, FSM, CEI comparison |
| **`lab/PHASE1_TRACEABILITY.md`** | **Code vs regulation matrix (sign-off)** |
| **`knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md`** | **P0–P7 regulation gates** — Phase 1 = **O.9.2 local path only** |
| `Architecture/Software_Architecture.drawio` | SW layer model + Phase 1 runtime (5 pages) |
| `lab/MODBUS_RTU_LAB.md` | Modbus-focused addendum |

---

## 5. Configuration summary

Deployed path: **`/etc/ccli/lab.yaml`**

```yaml
modbus:
  backend: rtu              # or simulator / --lab-demo for open-loop PF2 test
  device: /dev/ttyS2        # A2/B2 terminals
  baud: 9600
  parity: N
  slave_id: 1
  poll_ms: 1000
  timeout_ms: 2000

pf2:
  threshold_kw: 900
  release_threshold_kw: 850
  debounce_s: 30
  min_on_s: 60
  min_off_s: 30
  stale_data_s: 10

io:
  permissive_bypass: false
  do_curtail:
    gpio: 1                 # DIO1 / ubus ch1
    active_high: false
  di_permissive:
    gpio: 2                 # DIO2 / ubus ch2
    active_low: true          # gd32 state 1 = OK (12 V key closed)
```

**Actuation policy:** `DIO1 ON ⇔ PF2.curtailment_active AND permissive OK`

**Config parser note:** Only `modbus.backend` selects simulator vs RTU. The key `modbus.mode` is a transport label and must **not** override `backend`.

---

## 6. Procedure

### 6.1 One-time DIO prep (TG544)

```bash
ssh root@192.168.1.130
uci set didoservice_v2.channel_2.mode=di
uci commit didoservice_v2
/etc/init.d/didoservice_v2 restart
ubus call dido_v2 status
```

See `lab/TR400_DIO_SMOKE_TEST.md` and **`lab/TR400_HW_IO_CONFIG.md`** for DIO verification.

### 6.2 Deploy CCLI (from Windows PC)

**APK is optional for Phase 1.** LuCI **Startup** will not list `ccli` unless `apk add` succeeded. Lab testing only needs:

| File on TG544 | Required? |
|---------------|-----------|
| `/usr/sbin/ccli` | **Yes** |
| `/etc/ccli/lab.yaml` | **Yes** |
| `/etc/init.d/ccli` (APK or `install-ccli-procd.sh`) | **Optional** — LuCI **Startup** lists `ccli` only if this exists and is **enabled** |

**Check what you have (SSH):**

```bash
ls -l /usr/sbin/ccli /etc/ccli/lab.yaml
/usr/sbin/ccli --version
```

**Binary-only deploy (no LuCI Startup until you add procd):**

```powershell
& "C:\Program Files\PuTTY\pscp.exe" -pw YOUR_PASSWORD -hostkey "SHA256:..." `
  lab\tg544-openwrt\ccli-bin root@192.168.1.130:/usr/sbin/ccli
& "C:\Program Files\PuTTY\pscp.exe" -pw YOUR_PASSWORD -hostkey "SHA256:..." `
  apps\ccli\config\lab_tr400_phase1_regulation.yaml root@192.168.1.130:/etc/ccli/lab.yaml
& "C:\Program Files\PuTTY\plink.exe" -ssh -batch -pw YOUR_PASSWORD -hostkey "SHA256:..." `
  root@192.168.1.130 "chmod +x /usr/sbin/ccli"
```

**Add Startup / procd service (binary-only, no APK):**

```powershell
$env:CCLI_TG544_PW = "your-root-password"
.\lab\tg544-openwrt\deploy-phase1.ps1 -BinaryOnly
```

Or on the TG544 after copying `package/ccli/files/ccli.init` and `ccli.config` to `/tmp/`:

```bash
sh /tmp/install-ccli-procd.sh /tmp/ccli.init /tmp/ccli.config
/etc/init.d/ccli status
```

The daemon starts as **`/usr/sbin/ccli --config /etc/ccli/lab.yaml`** (from UCI `ccli.main.config_file`).  
**`--regulation-check` is not part of boot** — run it manually after deploy (see §7).

**Full package (APK — also installs Startup entry + procd):**

```powershell
$env:CCLI_TG544_PW = "your-root-password"
.\lab\tg544-openwrt\deploy-phase1.ps1
# or: apk add --allow-untrusted /tmp/ccli.apk
```

### 6.3 Wire RS485 (A2/B2 ↔ PC)

| TG544 | PC dongle |
|-------|-----------|
| **A2** | **A** (+) |
| **B2** | **B** (−) |

Swap A/B once if no communication. Short cable; 120 Ω termination only if the bus is long or unstable.

### 6.4 Start PC Modbus slave

```powershell
pip install "pymodbus>=3.6,<3.8"    # 3.8+ breaks ModbusSlaveContext API
python lab\modbus_rtu_slave.py --port COM5 --power-kw 950
# Or ramp:  --ramp  (400..950 kW / 120 s)
```

Find COM port: Device Manager → **Ports (COM & LPT)** → USB Serial (FTDI/CH340).

### 6.5 Run CCLI on TG544 (no APK / no Startup entry)

Copy `lab/phase1-run.sh` to the device, or run directly:

**A — Open-loop PF2 test (easiest; no PC Modbus slave):**

```bash
killall ccli 2>/dev/null
/usr/sbin/ccli --config /etc/ccli/lab.yaml --lab-demo
```

**B — Full Modbus RTU (PC slave on A2/B2):**

```bash
# PC first: python lab\modbus_rtu_slave.py --port COM5 --power-kw 950
killall ccli 2>/dev/null
/usr/sbin/ccli --config /etc/ccli/lab.yaml
```

**C — Background + log file:**

```bash
killall ccli 2>/dev/null
/usr/sbin/ccli --config /etc/ccli/lab.yaml >> /tmp/ccli.log 2>&1 &
tail -f /tmp/ccli.log
```

(`--lab-demo` ramps simulated P internally; use **A** to test PF2→DIO without RS485 wiring.)

### 6.6 Permissive for curtail test

Short **DIO2 → GND** on the 10-pin block before expecting DIO1 to actuate.

---

## 7. Regulation check (Phase 1 equation mock)

**Web mock UI (TesPro):** `http://192.168.1.130/ccli/mock_console.html` — edit all regulation mocks, see **standards actions**, live **DIO1** / P. Deploy: `lab/tg544-openwrt/deploy-regulation-ui.ps1` · doc: `lab/REGULATION_MOCK_UI.md`.

Static report (no hardware):

```bash
ccli --regulation-check --config config/lab_tr400.yaml
ccli --regulation-check --config config/lab_tr400_phase1_regulation.yaml
ccli --regulation-check --config config/lab_tr400_dso_custom_o11.yaml
```

With live Modbus + DIO (after deploy):

```bash
ccli --regulation-check --live --json --config /etc/ccli/lab.yaml
```

Master map **R01–L04** + yaml mocks: `lab/PF2_REGULATION_PHASE1.md` · vectors: `apps/ccli/test_vectors/l2_dso_phase1.yaml`.

---

## 8. Expected logs

```text
ccli config loaded from /etc/ccli/lab.yaml
hal_gpio_ll: TesPro dido_v2 via ubus
svc_io: ready do_curtail=GPIO1(active_low) di_permissive=GPIO2(active_low)
ccli started. config=/etc/ccli/lab.yaml
io: curtail_do=OFF permissive=BLOCKED     # DIO2 open
io: curtail_do=OFF permissive=OK          # DIO2 shorted to GND
io: DO curtail_on                         # P > 900 kW for ≥ 30 s + permissive OK
io: curtail_do=ON permissive=OK
```

Status lines appear every **~5 s** when the main loop is healthy. Long gaps (~15 s+) usually mean Modbus RTU timeouts (wrong port, slave not running, or wiring).

---

## 9. Pass criteria

| # | Test | Pass |
|---|------|------|
| 1 | `ubus call dido_v2 status` | `running: true`, ch2 mode DI |
| 2 | `/dev/ttyS2` exists | `ls -l /dev/ttyS2` |
| 3 | PC slave listening | `Listening COMx @ 9600 8N1 slave 1` |
| 4 | CCLI loop alive | Status line every ~5 s (not one line then hang) |
| 5 | P = 950 kW, permissive open | DIO1 stays OFF; log `permissive=BLOCKED` |
| 6 | P = 950 kW ≥ 30 s, DIO2→GND | `io: DO curtail_on`; DIO1 ~0 V vs GND |
| 7 | Stop PC slave | DIO1 OFF within ~10 s (stale data safe state) |
| 8 | `--lab-demo` without slave | Triangle crosses 900 kW; same DIO behaviour |

---

## 10. Troubleshooting

| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| One log line then long silence | Wrong `modbus.device` or no slave | Use `/dev/ttyS2` for A2/B2; start PC slave |
| `Segmentation fault` when piping to `head` | SIGPIPE from closed pipe | Run without `\| head`; use log file |
| `backend: simulator` ignored | Old YAML had `mode: rtu` overriding | Use current parser; set `backend: rtu` only |
| `permissive=BLOCKED` always | DIO2 open, wrong pin, stale `read_di`, or GD32 DI fault | UCI + `set_mode` + `gd32.set_mode` (see `lab/DIDO_V2_API.md`); `gd32.read_di` → `di[1]` with DIO2→GND; interim: `io.permissive_bypass: true` in `lab.yaml` |
| No `curtail_on` at 950 kW | Debounce not met (< 30 s) | Wait ≥ 30 s above threshold |
| PC slave import error | pymodbus 3.8+ | `pip install "pymodbus>=3.6,<3.8"` |
| COM port access denied | Slave already running | Kill other terminal using COMx |
| UI shows `/dev/rs485_2_uart` | Vendor path ≠ A2/B2 | Wire to A2/B2 → config **`/dev/ttyS2`** |
| APK install fails | TesproOS feed gaps | Deploy static `/usr/sbin/ccli` binary |

---

## 11. Modbus register map (slave)

| Modbus | Offset (0-based) | Type | Unit |
|--------|------------------|------|------|
| 40001 | 0 | float32 BE | P (kW) |
| 40003 | 2 | float32 BE | Q (kvar) |

Slave ID **1**, **9600 8N1**, 8 data bits, no parity, 1 stop bit.

---

## 12. Interface matrix (Phase 1)

| Interface | Source | Destination | Protocol |
|-----------|--------|-------------|----------|
| Plant power | PC pymodbus slave | TG544 CCLI | Modbus RTU 9600 |
| Curtail command | PF2 FSM | DIO1 / relay K1 | ubus `dido_v2` |
| Permissive | Bench jumper | DIO2 | ubus `dido_v2` |
| Management | Lab PC | TG544 | SSH / LuCI |

---

## 13. Risks & gaps

| ID | Risk / gap | Mitigation |
|----|------------|------------|
| R-LAB-001 | TesPro UI path ≠ A2/B2 UART | Documented; use `ttyS2` for screw terminals |
| R-LAB-002 | RS485 isolation not product-grade | Lab only; track as **PART** in BOM |
| R-LAB-003 | No automated Phase 1 CI on hardware | Manual checklist §8 |
| G-LAB-001 | Vendor serial service may hold port | Disable RS485-2 gateway in UI if conflicting |

---

## 14. Related documents

- `lab/PF2_Limits_and_Actions.md` — thresholds, FSM states, Allegato O/T comparison  
- `lab/MODBUS_RTU_LAB.md` — Modbus RTU details  
- `lab/TR400_DIO_SMOKE_TEST.md` — DIO smoke test (PASS)  
- `lab/tg544-openwrt/DEPLOY_CCLI.md` — SDK build and APK deploy  
- `knowledge-base/08-engineering/CCI_TR500_IO_Peripheral.md` — TR500 I/O map  
- `apps/ccli/config/lab_tr400.yaml` — canonical lab (900 kW, DSO mock off)
- `apps/ccli/config/lab_tr400_phase1_regulation.yaml` — Phase 1 RTU + DSO mock (~42 kW)
- `lab/PF2_REGULATION_PHASE1.md` — equation master map R01–L04

---

## 15. Phase 2 preview (not implemented)

- Dedicated Eth_A / Eth_B VLANs (today all LAN on `br-lan`)
- IEC 61850 MMS scaffold → live stack
- Onboard RS485-1 for second plant segment
- procd service hardening + logread integration

# P5 — TesPro 9-point import verify (2026-10-01)

**Document ID:** P5-TESPRO-PTS-VERIFY-001  
**Date:** 2026-10-01  
**Agent check:** PC + MMS client (no SSH — `CCLI_TG544_PW` not set)

---

## TesPro LuCI — 9 points imported

| Point | Realtime | Expected | Status |
|-------|----------|----------|--------|
| `PdC_TotW` | **450.0** | 450.0 | **PASS** (after A1/B1 map) |
| `PdC_TotVAr` | **45.0** | 45.0 | **PASS** |
| `PdC_PPV_AB` | 20.0000 | 20.0 | **PASS** |
| `Wlim_Mod` | 5 | 5 | **PASS** |
| `Wlim_WMax` | 0.0000 | 0.0 | **PASS** |
| `WSd_Mod` | 1 | 1 (yaml) | **PASS** |
| `WSd_WSpt` | 20.0000 | 20.0 (yaml) | **PASS** |
| `VArSd_Mod` | 5 | 5 | **PASS** |
| `VArSd_VArTgt` | 0.0000 | 0.0 | **PASS** |

**Import / ObjectRefs:** **PASS** (9 rows, correct paths)  
**Northbound MQTT:** **PASS** — LuCI Connected · [P5_TESPRO_NORTHBOUND_2026-10-01_142810.txt](P5_TESPRO_NORTHBOUND_2026-10-01_142810.txt) · TotW=450 @ 4 s cadence

---

## Root cause — Modbus RS485, not TesPro

### PC checks (agent)

| Check | Result |
|-------|--------|
| DUT ping `192.168.10.1` | **OK** |
| MMS `:102` open | **OK** |
| Modbus slave on COM5 P=450 | **Running** (PID python `modbus_rtu_slave.py`) |
| `mms_lab_client` Read TotW | **0.000000** |
| `mms_lab_client` PPV in report | **20.000000** |

### DUT log (`lab/tmp_dut_log_tail.txt` pattern)

```text
modbus: FC3 ReadHoldingRegisters unit=1 addr=40001 qty=4 -> P=0kW Q=0kvar ERR=read_fail
```

`ccli` polls `/dev/rs485_2_uart` every 4 s but **RS485 read fails** → TotW/TotVAr stay 0.  
PPV/Wlim/WSd/VArSd come from yaml/dso mock → still correct in TesPro.

---

## Root cause confirmed (2026-10-01 PM)

**Bench wiring:** PC USB-RS485 on **A2/B2** (`/dev/ttyS2`).  
**Deployed yaml:** `modbus.device: /dev/rs485_2_uart` (LuCI **RS485-2** — different port).

→ Port mismatch → `ERR=read_fail` → TotW/TotVAr = 0.

**A2/B2 yaml test (2026-10-01 PM):** `device: /dev/ttyS2` → **`ERR=rtu_connect`** — UART2 unbound on FW:

```text
/proc/tty/driver/serial: 2: uart:unknown port:00000000 irq:0
```

**A2/B2 cannot work on this TesproOS build.** Use **RS485-2** connector + `rs485_2_uart` yaml (restored on DUT).

**Credentials:** `lab/tg544-openwrt/lab-env.ps1` (gitignored, `000000`).

**RESOLVED 2026-10-01 PM:** Cable on **A1/B1** (after A/B swap) + yaml `device: /dev/ttyS1`:

| Check | Result |
|-------|--------|
| Modbus FC03 trace PC | **PASS** every ~4 s |
| DUT log | **P=450kW Q=45kvar** |
| MMS TotW / TotVAr | **450 / 45** |
| MMS `:102` + northbound | **PASS** — [P5_TESPRO_NORTHBOUND_2026-10-01.md](P5_TESPRO_NORTHBOUND_2026-10-01.md) |

**Persisted yaml:** `apps/ccli/config/lab_tr400_cleartext_tsp_ttyS1.yaml` → `/etc/ccli/lab.yaml` on DUT.

Refresh LuCI IEC 61850 — **PdC_TotW / PdC_TotVAr** should show **450 / 45** within ~8 s.

---

## HW map (canonical)

See **[P5_A1B1_RS485_HW_MAP.md](P5_A1B1_RS485_HW_MAP.md)** — silkscreen **A1/B1** → `/dev/ttyS1` → `lab_tr400_cleartext_tsp_ttyS1.yaml`.

```powershell
.\lab\tg544-openwrt\verify-tespro-points-live.ps1
```

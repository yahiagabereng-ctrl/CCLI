# RS485 inverter control and logical nodes — handoff for Alireza

**Date:** 2026-11-20  
**Audience:** @alirezavzand (PC-A / logger, COM5 → A1/B1)  
**Tree:** lab `apps/ccli` (not the product 17-LN ship yaml)

---

## What this is

The running controller could read Modbus and close the plant curtail relay. It could not command an inverter. This drop adds that command on the same RTU master, and records which logical nodes it belongs to.

`git pull origin main`

| Item | Path |
|------|------|
| **This note** | `handoff/alirezavzand/INVERTER_RS485_LN_ALIREZA.md` |
| **Check yaml** | `apps/ccli/config/lab_tr400_inverter_control.yaml` |
| **Writes** | `apps/ccli/adapters/modbus_master/modbus_adapter.cpp` |
| **Startup + MMS** | `apps/ccli/program/ccli_bridge.cpp` · `apps/ccli/services/ccli_main.cpp` · `apps/ccli/services/svc_modbus.cpp` |
| **Slave that reacts** | His repo `alirezavzand/Sun2000-100KTL-M2-Inverter-Simulator` · file `sun2000_100ktl_m2_rtu_real_register_mock.py` |
| **PC start** | `lab/start-bench-plant.ps1 -SunOnly -Port COM5` |

Other lab yamls keep `inverter_control` off. The EMT432 TCP meter is not written.

---

## Bench

```text
PC COM5  --USB-RS485--  TG544 A1/B1  /dev/ttyS1
9600 8N1 · slave 1 · ccli is the Modbus master
```

```powershell
powershell -File lab\start-bench-plant.ps1 -SunOnly -Port COM5
```

Deploy `apps/ccli/config/lab_tr400_inverter_control.yaml` as `/etc/ccli/lab.yaml` on a **lab** `ccli` built from this tree. The product ship yaml `product_tr400.yaml` still has `inverter_control` unset and `modbus.backend: tcp`. That image does not send these frames.

---

## Two commands

Example available power **80 kW** (the mock’s daytime power moves; the cap is always that power times the percent).

| Intent | Frames at start | What you should see |
|--------|-----------------|---------------------|
| Stay in service, cap at 20% | FC06 **40200** value 0, then FC06 **40125** value **200** (20 × gain 10) | Mock prints `P limit 20.0%` and `service=ON`. Holding **40001** float kW is about **16 kW**. |
| This slave out of service | Set `inverter_in_service: false` and restart. FC06 **40201** value 0 | Mock prints `Shutdown 40201`. **40001** reads **0 kW**. |

DIO1 is still the whole-plant curtail relay. It does not select this slave and it does not write 40125 or 40201.

---

## Logical nodes

The percent **is** mapped. Shutdown **is not**.

| Logical node | Data object | Code | Modbus |
|--------------|-------------|------|--------|
| `LD_Plant/WSdDAGC1` | `WSptPct` | MMS operate → `apply_dso_live_command` → `ModbusService::command_active_limit` | FC06 **40125** when `inverter_control` is true |
| `LD_Plant/WlimDWMX1` | `WMaxSptPct` | Same function. Used only when WSd is off | Same **40125** |
| — | yaml `modbus.inverter_in_service` | `apply_startup_inverter_control` | FC06 **40200** or **40201** |

Check yaml: `dso.profile: tr57126_table1`, `wsd_active: true`, `wspt_pct: 20`. With no MMS client yet, startup sends 40125 = 200 from that profile. A later operate of `WSdDAGC1.WSptPct` rewrites 40125 while the unit is in service.

`WlimDWMX1` and `WSdDAGC1` are still bound to **DIO1** in `apps/ccli/config/icd/signal_map.yaml` (PF2 enter 42 kW at Smax 210 kVA). The 40125 write is a second output of the same percent. The signal-map text still calls the plant P write a gap. The plant row that names the register is `DSO.WSd.PctWrite` / `DSO.Wlim.PctWrite` in `apps/ccli/config/plant/generated/plant_modbus_map.yaml`. The code writes **one** slave (`modbus.slave_id`), not the map’s fan-out to slaves 1–10.

`SGGMMXU1.TotW` is the measurement of one inverter (Huawei **32080**). It is not the command.

`IDGXCBR1.Pos` is the main grid breaker and stays status-only. `DisFRDGEN1.Beh` is an availability flag. No logical node operates 40201.

---

## What to look for on COM5

1. `FC06 inverter power on → holding 40200` on the DUT log.
2. `FC06 inverter P limit 20.0% → holding 40125 value 200`.
3. On the PC mock: `P limit 20.0%` then `service=ON` and a lower `P=`.
4. For out of service, flip `inverter_in_service: false`, restart, and expect `Shutdown 40201` and `P=0`.

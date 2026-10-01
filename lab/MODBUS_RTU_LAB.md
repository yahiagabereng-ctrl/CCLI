# Modbus RTU lab — addendum

> **Canonical runbook:** [`lab/PHASE1.md`](PHASE1.md) — start here for the full Phase 1 procedure.

**Goal:** Modbus **P (kW)** from RS485 slave → PF2 FSM → **DIO1/DIO2** on TesPro TG544.

**Bench topology (current):** PC USB-RS485 ↔ TG544 **A2/B2** (`/dev/ttyS2`). Full map: [`lab/TR400_HW_IO_CONFIG.md`](TR400_HW_IO_CONFIG.md).

| Role | Device | Config |
|------|--------|--------|
| **Modbus master** | TG544 CCLI | `modbus.backend: rtu`, `device: /dev/ttyS2` |
| **Modbus slave** | PC (`lab/modbus_rtu_slave.py`) | COM port, slave ID **1**, 9600 8N1 |

---

## Serial port mapping

| Physical | Linux device | Use in CCLI |
|----------|--------------|-------------|
| **A2/B2 terminals** | **`/dev/ttyS2`** | **Modbus RTU (current bench)** |
| A1/B1 terminals | `/dev/ttyS1` | Alternate |
| USB dongle on TG544 | `/dev/ttyUSB0` (typical) | Alternate |
| TesPro UI “RS485-2” | `/dev/rs485_2_uart` | **Not A2/B2** on lab TG544 → `ttyUSB1` |

```bash
readlink -f /dev/rs485_2_uart   # confirm before using UI path
```

---

## PC slave

```powershell
pip install "pymodbus>=3.6,<3.8"
python lab\modbus_rtu_slave.py --port COM5 --power-kw 950
python lab\modbus_rtu_slave.py --port COM5 --ramp
```

---

## Wiring (A2/B2 ↔ PC)

| TG544 | PC |
|-------|-----|
| A2 | A (+) |
| B2 | B (−) |

---

## Register map

| Modbus | Offset | Type | Unit |
|--------|--------|------|------|
| 40001 | 0 | float32 BE | kW |
| 40003 | 2 | float32 BE | kvar |

---

## Sanity checks

**TG544 — port exists:**

```bash
ls -l /dev/ttyS2
grep -A6 '^modbus:' /etc/ccli/lab.yaml
```

**Healthy CCLI loop:** status line every ~5 s (see `PHASE1.md` §7).

**Modbus failing:** long gaps between status lines (~2 s RTU timeout per failed poll).

---

## See also

- [`lab/PHASE1.md`](PHASE1.md) — full procedure, pass criteria, troubleshooting  
- [`lab/PF2_Limits_and_Actions.md`](PF2_Limits_and_Actions.md) — PF2 thresholds and DIO policy  

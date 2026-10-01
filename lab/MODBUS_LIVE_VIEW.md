# Modbus live view — PC slave + CCLI master (Phase 1)

**Document ID:** CCLI-LAB-MODBUS-LIVE-001  
**Bus:** TG544 **A2/B2** `/dev/ttyS2` ↔ PC USB-RS485 · **9600 8N1** · slave **1**

Phase 1: CCLI is **Modbus master** (FC03 read only). PC runs the **slave** meter. Curtailment is **DIO1**, not Modbus writes.

---

## 1. PC — live slave trace (requests from TG544)

```powershell
pip install "pymodbus>=3.6,<3.8"
python lab\modbus_rtu_slave.py --port COM5 --power-kw 45 --trace
```

Each line is a **master read** CCLI sent:

```text
[17:05:01.234] SLAVE RX FC03 ReadHolding addr=40001 qty=4 -> [0x4234 ...] P=45.00kW
```

Use **`--ramp`** instead of fixed power for Eq (18) tests.

---

## 2. TG544 — live CCLI Modbus + PF2 output

**A. Running service (procd)** — enable yaml trace:

```yaml
modbus:
  trace: true   # in /etc/ccli/lab.yaml
```

```bash
/etc/init.d/ccli restart
logread -f | grep -E 'modbus:|io:'
```

Example:

```text
modbus: FC3 ReadHoldingRegisters unit=1 addr=40001 qty=4 -> P=45kW raw=0x4234 ... P=45kW pf2=... curtail=no DO=OFF
```

**B. One-shot regulation finalize** (static + live + per-clause Modbus confirm):

```bash
/etc/init.d/ccli stop    # free /dev/ttyS2 for this test only
/usr/sbin/ccli --regulation-check --live --live-watch 8 --config /etc/ccli/lab.yaml
/usr/sbin/ccli --regulation-check --live --live-watch 8 --json --config /etc/ccli/lab.yaml > /tmp/reg.json
/etc/init.d/ccli start
```

Report sections:

| Section | Content |
|---------|---------|
| Static rows | R01–L04, CFG, IO |
| **LIVE-MB*** | Last FC03 PDU + raw regs |
| **LIVE-CONF-*** | Each clause vs live P / Modbus |
| Footer | **live Modbus master log** (one line per poll) |

---

## 3. PC — combined bench (automated)

```powershell
$env:CCLI_TG544_PW = "your-password"
.\lab\modbus-ccli-live-bench.ps1 -ComPort COM5 -PowerKw 45 -LiveWatchSec 8
```

1. Starts PC slave with **`--trace`** (new window).  
2. Runs TG544 **`--regulation-check --live --json`**.  
3. Saves **`lab/regulation_bench/last-reg.json`**.  
4. Open **`lab/regulation_bench/index.html`** → Import JSON → review **Finalize** summary + Modbus log.

---

## 4. Match PC vs CCLI (sanity)

| Check | PC slave trace | CCLI |
|-------|----------------|------|
| Function | FC03 ReadHolding | `LIVE-MB` FC03 |
| Address | `addr=40001 qty=4` | `addr=40001 qty=4` |
| P decode | `P=45.00kW` | `LIVE-P` / `LIVE-CONF-R03` |
| Period | ~every **poll_ms** (4000 ms regulation yaml) | `modbus_log` lines ~4 s apart |
| Writes | none (slave only responds) | `LIVE-MB-W` **N/A** |

---

## 5. Finalize criteria (Phase 1 CEI ~42 kW)

With **`lab_tr400_phase1_regulation.yaml`** and slave **`--power-kw 45`** (or 42):

- **LIVE-MB** PASS, **LIVE-P** good quality  
- **LIVE-CONF-R03/R05/R06** PASS (±5% of **42 kW**)  
- **LIVE-CONF-R08/R09** PASS  
- **LIVE-DIO** ON when P > 42 kW after **30 s** debounce and permissive OK  

Import JSON in web UI for sign-off snapshot.

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-22 | PC `--trace`, CCLI `modbus.trace`, live bench script |

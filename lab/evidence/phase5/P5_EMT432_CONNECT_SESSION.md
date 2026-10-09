# P5-06 — EMT432 connected (bench session)

**Date:** 2026-10-05  
**Meter:** Chronos EMT432 **I** (5 A TA) · no CT · no Rogowski  
**Plant path:** **Ethernet → TG544 LAN3** (`192.168.30.0/24`)  
**Aux:** Keysight **66332A** @ **20 V DC** → Pow (+/− per orange block symbols)  
**Mains:** 230 V **L1 + N** · L2/L3 open · single-phase mode  

---

## Wiring as-built

| Path | Connection | Status |
|------|------------|--------|
| Aux | 66332A (+/−) → Pow **+/−** | **WIRED** |
| Voltage | 230 V → **L1 + N** | **WIRED** (confirm display V) |
| Current | I1 CT | **OPEN** (P/Q ≈ 0 expected) |
| Plant comms | EMT432 **RJ45 → LAN3** | **WIRED** |
| RS485 A1/B1 | PC pymodbus slave (optional) | Unchanged if still on A1/B1 |

---

## Network plan (LAN3)

**Two bench IP plans** — use one consistently:

| Device | Plan A (zone docs) | Plan B (**EMT432 SN 261001102952**, 2026-10-05+) |
|--------|-------------------|--------------------------------------------------|
| TG544 **lan3** | **192.168.30.1/24** | **192.168.178.1/24** (`uci set network.lan2_sec.ipaddr`) |
| **EMT432** | **192.168.30.50/24** | **192.168.178.250/24** static, GW **.178.1** |
| PC plant NIC | **192.168.30.10/24** — `lab/set_pc_lan3_plant.cmd` | **192.168.178.10/24** — `lab/set_pc_lan3_plant_178.cmd` |

**Current as-built:** Plan B — L3 ping **PASS**; Modbus TCP **:502 still closed** (2026-10-09).

**PC note (2026-10-05):** bench PC on **LAN1** `192.168.10.10` only — **LAN3 ping to 192.168.30.1 timed out**. Probe from PC after LAN3 cable + `192.168.30.10`, or from DUT SSH.

---

## Verification checklist

| # | Check | Pass criteria | Result |
|---|-------|---------------|--------|
| 1 | POW LED | On with 20 V aux | ☐ |
| 2 | Display V L1–N | ~220–240 V | ☐ |
| 3 | Display f | ~50 Hz | ☐ |
| 4 | Single-phase mode | Menu or reg 40232 bits 1–2 = 0 | ☐ |
| 5 | Ethernet LINK LED | Green solid on RJ45 | ☐ |
| 6 | Ping meter from DUT | `ping 192.168.178.250` | **PASS** 2026-10-05 |
| 7 | Modbus TCP FC3 | `lab/modbus_emt432_tcp_probe.py` | ☐ (PC needs LAN3) |
| 8 | P1/Q1 without CT | P≈0 Q≈0, V/f OK | ☐ |

---

## Run probe

**PC on LAN3:**

```powershell
pip install pymodbus
python lab/modbus_emt432_tcp_probe.py --host 192.168.30.50
# or find IP:
python lab/modbus_emt432_tcp_probe.py --scan
```

**TG544 (SSH):**

```bash
ping -c 2 192.168.30.50
# if python3 + pymodbus on DUT:
python3 /tmp/modbus_emt432_tcp_probe.py --host 192.168.30.50
```

---

## Evidence to capture

- [ ] Photo: POW LED + display (V, f)
- [ ] Photo: Ethernet LINK LED
- [ ] Probe log → `P5_EMT432_MODBUS_EVENT_<date>.txt`
- [ ] Note meter IP if not `.50`

---

## P5-06 verdict

| Gate | Status |
|------|--------|
| HW wired | **WIRED** 2026-10-05 |
| Modbus read | **PENDING** — run TCP probe |
| MMS TotW from meter | **PENDING** — needs ccli Modbus TCP backend |
| Full metrology | **PART** — no CT |

**Next:** confirm meter IP → run probe → paste log for evidence close.

# P5-06 — Chronos EMT432 bench plan (230 V 1P)

**Meter:** Chronos EMT432 **I** (TA 5 A) · Base or Full  
**Corpus:** [CCI_Chronos_EMT432_Extract.md](../../../knowledge-base/08-engineering/CCI_Chronos_EMT432_Extract.md)  
**Modbus map:** [chronos_emt432_map.yaml](../../../apps/ccli/config/modbus/chronos_emt432_map.yaml)  
**Terminal wiring:** [P5_EMT432_TERMINAL_WIRING.md](./P5_EMT432_TERMINAL_WIRING.md)  
**Connect session:** [P5_EMT432_CONNECT_SESSION.md](./P5_EMT432_CONNECT_SESSION.md)  
**Lab yaml:** [lab_tr400_emt432_1p.yaml](../../../apps/ccli/config/lab_tr400_emt432_1p.yaml)  
**TCP probe:** [modbus_emt432_tcp_probe.py](../../../lab/modbus_emt432_tcp_probe.py)

---

## Status (2026-10-05)

| Item | Status |
|------|--------|
| Meter HW wired (Pow + L1/N + LAN3) | **CONNECTED** |
| Modbus TCP probe | **PENDING** — set meter IP, run probe |
| ccli reads meter on DUT | **PENDING** — Modbus TCP backend not in ccli yet |
| CT / P/Q metrology | **OPEN** — no CT |

---

## Bench constraints

| Item | Lab status |
|------|------------|
| Mains | **230 V L-N** monofase |
| 3P POC | **Not available** — 1P + single-phase mode |
| Rogowski / CT | **Not connected** — V/f OK; P/Q ≈ 0 |
| Aux supply | **66332A @ 20 V DC** → Pow (+/− on orange block) |
| Plant comms | **Ethernet → LAN3** (not RS485) |

---

## Wiring (as-built)

```text
66332A 20 V ──► Pow +/−     (symbols on orange block)
230 V     ──► L1 + N        (L2, L3 open)
EMT432 RJ45 ──► TG544 LAN3  (192.168.30.0/24)
A1/B1     ──► PC pymodbus   (optional Q command — unchanged)
```

Single-phase: menu or Modbus `40232` bits 1–2 = **0**.

---

## Network (LAN3)

| Device | IP |
|--------|-----|
| TG544 lan3 | **192.168.30.1/24** |
| EMT432 | **192.168.30.50/24** (assign static on meter) |
| PC plant NIC | **192.168.30.10/24** — `lab/set_pc_lan3_plant.cmd` |

Configure meter IP via display / web UI (`admin`/`admin` on device AP or LAN IP).

---

## First Modbus probe

```powershell
pip install pymodbus
python lab/modbus_emt432_tcp_probe.py --host 192.168.30.50
python lab/modbus_emt432_tcp_probe.py --scan   # if IP unknown
```

Reads: `feature_version`, `Measurement_Config`, **V L1–N**, **Frequency**, **P1/Q1** (offsets 932/940).

---

## ccli integration (open)

`ccli` **Modbus master = RTU only** today (`modbus_new_rtu`). For DUT to poll the meter:

1. Add **Modbus TCP** backend (`host` + `port` 502), or  
2. Temporary: read from PC probe until TCP lands in `modbus_adapter.cpp`

Yaml profile `lab_tr400_emt432_1p.yaml` holds target offsets **932/940**; `reg_*` yaml loader still needed for RTU path.

---

## Lab topology

| Path | Role |
|------|------|
| **EMT432 LAN3** | Real V/f (+ P/Q with CT) → P5-06 |
| **A1/B1 RS485** | PC pymodbus Q command (VArSd/PFSP) |
| **LAN1 MMS** | TotW/TotVAr to operator (from ccli) |

Use **P1/Q1** for 1P bench, not **P_SUM/Q_SUM**.

---

## Evidence target (P5-06)

| Step | Artifact |
|------|----------|
| Terminal photo + wiring doc | **HAVE** |
| Meter connected session | **HAVE** — `P5_EMT432_CONNECT_SESSION.md` |
| POW + display V/f | photo / operator note |
| Modbus TCP probe log | `P5_EMT432_MODBUS_EVENT_*.txt` |
| MMS TotW from live meter | after ccli TCP |
| 1P + no CT limitation | documented |

**Verdict:** **PART** — HW connected; comms probe + ccli TCP pending.

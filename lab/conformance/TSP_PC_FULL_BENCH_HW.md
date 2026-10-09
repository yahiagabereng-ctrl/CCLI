# Test Suite Pro PC — full bench (LAN1 + LAN2 + LAN3 + USB RS485)

**Document ID:** CCLI-LAB-TSP-BENCH-001  
**Revision:** 1.0  
**Date:** 2026-10-08  
**RAG source_id:** `ccli-tsp-full-bench-hw`  
**Goal:** One **Windows PC** runs **Test Suite Pro** (61850), **104 client** (optional), and **plant/inverter mock** (Modbus USB-RS485 and/or LAN3) so [`LAB_REQUIREMENT_TEST_MATRIX.md`](LAB_REQUIREMENT_TEST_MATRIX.md) and **P5 full-circle** can complete without swapping machines.

---

## Topology

```text
                    ┌─────────────────────────────────────────┐
                    │  Test Suite Pro PC (single host)         │
                    │  ┌─────────┐ ┌─────────┐ ┌──────────┐  │
                    │  │ NIC LAN1│ │ NIC LAN2│ │ NIC LAN3 │  │
                    │  │ .10.10  │ │ .1.183  │ │ .30.10   │  │
                    │  └────┬────┘ └────┬────┘ └────┬─────┘  │
                    │       │           │           │         │
                    │  Test Suite Pro   │104 client │ GOOSE/  │
                    │  Wireshark        │(optional) │ plant   │
                    │                   │           │         │
                    │  USB-RS485 (COMx) ──┼── Modbus slave ────┼──► DUT RS485
                    └───────────────────┼───────────┼─────────┘
                                        │           │
                    ┌───────────────────▼───────────▼─────────┐
                    │  TG544 / TR500 (DUT)                       │
                    │  LAN1 192.168.10.1  MMS TLS :3782 (Eth_A) │
                    │  LAN2 192.168.1.130 IEC 104 :2404 (Eth_B)│
                    │  LAN3 192.168.30.1  plant / GOOSE (opt.) │
                    │  RS485 A1/B1 or RS485-2 → ccli Modbus    │
                    └───────────────────────────────────────────┘
```

| Zone | DUT IP | DUT port / service | PC NIC static IP | PC role |
|------|--------|-------------------|------------------|---------|
| **DSO Eth_A (LAN1)** | `192.168.10.1` | MMS **3782** TLS | **`192.168.10.10`** | **Test Suite Pro** client → server `CCI016_01` |
| **Operator Eth_B (LAN2)** | `192.168.1.130` | IEC 104 **2404** TLS | **`192.168.1.183`** | lib60870 / lab 104 client (not TSP) |
| **Plant (LAN3)** | `192.168.30.1` | GOOSE L2 / plant | **`192.168.30.10`** | Wireshark GOOSE · optional inverter IED mock |
| **RS485 plant meter** | — | Modbus RTU on DUT | **USB-RS485 `COM5`** (typ.) | `python lab/modbus_rtu_slave.py` **inverter/PdC mock** |

**Cable rule:** One PC Ethernet port per DUT LAN port (isolation per Phase 2). Do not bridge LAN1↔LAN2 on the same switch.

---

## PC network setup (Admin CMD — rename interfaces to match)

Scripts in repo (edit `name="Ethernet"` if your adapters differ):

| Script | PC IP | Gateway | Use |
|--------|-------|---------|-----|
| [`lab/set_pc_lan1_dso.cmd`](../set_pc_lan1_dso.cmd) | `192.168.10.10/24` | `192.168.10.1` | **TSP / MMS** |
| [`lab/set_pc_lan2_oa.cmd`](../set_pc_lan2_oa.cmd) | `192.168.1.183/24` | `192.168.1.130` | **104** |
| [`lab/set_pc_lan3_plant.cmd`](../set_pc_lan3_plant.cmd) | `192.168.30.10/24` | (none or `.1`) | **Plant / GOOSE** |

Verify: `ping 192.168.10.1` · `ping 192.168.1.130` · `ping 192.168.30.1`

---

## USB RS485 — Modbus plant / “inverter” mock

Required for **live TotW/TotVAr** during TSP reads and **P5 step 13** (P sweep).

| Item | Value |
|------|--------|
| Tool | `lab/modbus_rtu_slave.py` |
| Typical port | **`COM5`** (USB-RS485 adapter) |
| Baud / parity | **9600 8N1**, slave ID **1** |
| P5 default | `--power-kw 450` · `--q-kvar 45` (adjust per test) |
| Wiring | **Verified bench:** PC A/B → TG544 **A1/B1** → DUT **`/dev/ttyS1`** — see [`P5_A1B1_RS485_HW_MAP.md`](../evidence/phase5/P5_A1B1_RS485_HW_MAP.md) |

**Start before Tier 2 (full circle):**

```powershell
cd C:\Yahia\projects\CCLI
python -u lab\modbus_rtu_slave.py --port COM5 --power-kw 450 --q-kvar 45 --trace
```

**Yaml on DUT:** `modbus.device` must match physical pair (`/dev/rs485_2_uart` or `/dev/ttyS1` per deploy). Mismatch = TotW stuck → TSP read failures.

**LAN3 vs USB:** LAN3 is for **Ethernet plant/GOOSE**; **USB RS485** is the **Modbus meter mock** (Annex T 4 s measures). Use **both** for a complete P5 report.

---

## DUT yaml profiles (pick one deploy)

| Profile | File | MMS | 104 | Modbus |
|---------|------|-----|-----|--------|
| **TSP / P3–P5 MMS only** | `lab_tr400_phase1_regulation.yaml` | `:3782` TLS | off | RS485 per yaml |
| **MMS + 104 same session** | `lab_tr400_phase4_eth_b.yaml` | `:3782` | `:2404` LAN2 | RS485 |

Deploy: `lab/tg544-openwrt/deploy-*.ps1` with matching `-Config`.

---

## Full test report — execution order (one session)

Use with [`01_CHECKLIST_LAB_REQUIREMENT_TEST_MATRIX.md`](LAB_REQUIREMENT_TEST_MATRIX.md) in field pack.

| Phase | HW | Software | Matrix rows |
|-------|-----|----------|-------------|
| **0 — Prep** | LAN1 cable · TLS staged · optional COM5 slave | `stage-tsp-field-pack` · TSP CID import | Session header |
| **0b — Plant** | USB RS485 wired · COM5 slave running | TotW stable | T1-04, Tier 2 steps 2–4 |
| **1 — MMS core** | LAN1 only required | TSP Connect → Compare → URCB → Wlim/WSd | T1-01…T1-10 |
| **2 — Annex T TLS** | LAN1 + COM5 | P5 full-circle sequencer on **3782** | Tier 2 · P5 doc |
| **3 — Security** | LAN1 | T3-01…T3-04 negatives | Tier 3 |
| **4 — 104 (optional)** | **LAN2** NIC | lib60870 client → `192.168.1.130:2404` | 104 row · `phase4/P4_*` evidence |
| **5 — GOOSE (optional PICS)** | **LAN3** | Wireshark / TSP GOOSE Tracker | Tier 4 |
| **6 — Actuation** | DIO bench (local to DUT) | SSH `ubus call dido_v2 status` after Wlim | Actuation row |
| **7 — Export** | USB stick | All `TSP_*` → repo `inbox/` | Return path in matrix |

---

## Field pack + GitHub on TSP PC

```powershell
git clone https://github.com/yahiagabereng-ctrl/CCLI.git
cd CCLI
powershell -File scripts\stage-tsp-field-pack.ps1
# Checklist: CCLI_TSP_FIELD_PACK\01_CHECKLIST_*.md
# This doc after pull: lab\conformance\TSP_PC_FULL_BENCH_HW.md
```

---

## Troubleshooting

| Symptom | Check |
|---------|--------|
| TSP Connect fail | LAN1 IP `.10.10` · PEMs in `tls\` · DUT listening `:3782` |
| TotW = 0 / stale | COM5 slave running · RS485 A1/B1 · DUT `modbus.device` |
| 104 fail | **LAN2** `.1.183` · yaml `iec104.enabled` · not on TSP |
| Compare Model noise | Run Compare **before** enabling URCB |

---

**RAG tags:** `TSP`, `bench`, `LAN1`, `LAN2`, `LAN3`, `RS485`, `Modbus`, `ccli-tsp-full-bench-hw`

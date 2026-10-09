# Lab session — GO for formal test (2026-10-09)

**Document ID:** CCLI-LAB-SESSION-START-2026-10-09-FINAL  
**DUT:** TG544 · **`ccli 0.1.0-r52`** · `lab_tr400_phase4_emt432_lan3_tcp.yaml`  
**Deploy:** `CCLI_DEPLOY_0.1.0-r52_2026-10-09_135140.md`

---

## Interface summary (all paths)

| Domain | DUT | Client PC | Role | Status |
|--------|-----|-----------|------|--------|
| **Eth_A MMS** | `192.168.10.1:3782` TLS | **PC-B** `192.168.10.10` | Test Suite Pro (P3) | **GO** |
| **Eth_B 104** | `192.168.1.130:2404` TLS | **PC-A** `192.168.1.183` | lib60870 / O.13 (P4) | **GO** (re-ping PC-A) |
| **LAN3 meter** | `192.168.178.1` | **PC-A** `192.168.178.10` | EMT432 TCP probe (P5) | **GO** (DUT polls `.250`) |
| **RS485 plant** | `/dev/ttyS1` A1/B1 | **PC-A** COM5 | **Huawei inverter mock** (parallel; not TotW source while TCP yaml) | **Start slave on PC-A** |

**TotW / TotVAr in MMS & 104:** from **EMT432 Modbus TCP** (`float_word_swap: true`).  
**RS485:** run `modbus_rtu_slave.py --huawei` for inverter / P5 rows — does not replace meter reads.

---

## DUT preflight (2026-10-09 ~20:08 UTC+2)

| Check | Result |
|-------|--------|
| Single `ccli` | **OK** — stop **`iec61850service`** then **`/etc/init.d/ccli restart`** if bind fails |
| Version | **0.1.0-r52** |
| MMS **3782** | **LISTEN** · PC-B `Test-NetConnection` → **True** |
| 104 **2404** | **LISTEN** on DUT |
| Modbus TCP → **192.168.178.250:502** | **OK** — P/Q ≈ 0 kW (no CT) |
| GNSS | **`gnss_start`** issued (`already_on: true`) — confirm fix before URCB |

Evidence: [`PREFLIGHT_DUT_2026-10-09_200828.txt`](PREFLIGHT_DUT_2026-10-09_200828.txt)

---

## PC-B (TSP) — before Connect

| □ | Action |
|---|--------|
| □ | Admin: `lab\set_pc_lan1_dso.cmd` → **192.168.10.10** |
| ☑ | `Test-NetConnection 192.168.10.1 -Port 3782` → **True** (post-restart) |
| □ | Certs: `C:\CCLI_product_tls\` (deploy staged r52) |
| □ | TSP: IED **`CCI016_01`** · **`192.168.10.1:3782`** · TLS |
| □ | CID: **`lab_tg544_eth_a.cid`** loaded in workspace |

**Phase 1 order:** Connect (T1-01) → browse (T1-03) → Compare (T1-02) → reads (T1-04) → …

---

## PC-A (logger / plant) — parallel

| □ | Action |
|---|--------|
| □ | Admin: `lab\set_pc_lan2_ethernet2.cmd` → **192.168.1.183** · ping **.1.130** · **:2404** open |
| □ | Admin: `lab\set_pc_lan3_plant_178.cmd` → **192.168.178.10** · optional probe |
| □ | RS485 **A1/B1** + **`python -u lab\modbus_rtu_slave.py --port COM5 --power-kw 450 --huawei --trace`** |
| □ | Optional: `python lab\modbus_emt432_tcp_probe.py --host 192.168.178.250` |

---

## Runbooks

- Annex + TSP steps: [`LAB_SESSION_2026-10-09_ANNEX_RUNBOOK.md`](../../conformance/LAB_SESSION_2026-10-09_ANNEX_RUNBOOK.md)
- Two-PC phases: [`TSP_TWO_PC_BENCH_RUNBOOK.md`](../../conformance/TSP_TWO_PC_BENCH_RUNBOOK.md)
- Matrix: [`LAB_REQUIREMENT_TEST_MATRIX.md`](../../conformance/LAB_REQUIREMENT_TEST_MATRIX.md)

**Verdict:** **START TEST** — PC-B **Phase 1 TSP** now; PC-A paths for **104 / meter / RS485** when those matrix rows run.

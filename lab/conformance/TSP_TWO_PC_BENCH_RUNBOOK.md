# Two-PC bench — phased test run (vs lab matrix)

**Document ID:** CCLI-LAB-TSP-TWO-PC-001  
**Revision:** 1.0  
**Date:** 2026-10-09  
**RAG source_id:** `ccli-tsp-two-pc-bench-runbook`  
**Maps to:** [LAB_REQUIREMENT_TEST_MATRIX.md](LAB_REQUIREMENT_TEST_MATRIX.md) · [TSP_TEST_IDENTIFICATION_POST_CONNECT.md](../evidence/testsuite-pro/TSP_TEST_IDENTIFICATION_POST_CONNECT.md)

Use this when **two Windows PCs** share one TG544 DUT:

| PC | Nickname | NICs / USB | Role |
|----|----------|------------|------|
| **PC-B** | **TSP / DSO** | **LAN1 only** → `192.168.10.10` | Test Suite Pro · **3782 TLS** · all matrix **T1 / T2 / T3** rows |
| **PC-A** | **Plant / logger** | **LAN2** + **LAN3** + **USB RS485** | 104 client · EMT432 Modbus **TCP** · **Huawei inverter mock** (RTU) · SSH/event-dump |

**Rule:** Do **not** run Test Suite Pro on PC-A. Do **not** bridge LAN1↔LAN2 on one switch with DSO traffic mixed with operator traffic without isolation (Phase 2).

---

## Topology

```text
  PC-B (TSP)                         TG544 DUT                         PC-A (plant/logger)
  ┌─────────────────┐                ┌──────────────────┐            ┌─────────────────────────┐
  │ LAN1 .10.10     │──── cable ────│ LAN1 .10.1 :3782 │            │ LAN2 .1.183             │
  │ Test Suite Pro  │                │ MMS TLS (Eth_A)  │◄── cable ──│ lib60870 → :2404        │
  └─────────────────┘                │                  │            │                         │
                                     │ LAN2 .1.130 :2404│            │ LAN3 .30.10             │
                                     │ 104 TLS (Eth_B)  │◄── cable ──│ probe / ping meter      │
                                     │                  │            │                         │
                                     │ LAN3 .30.1       │◄── cable ──│ EMT432 .30.50 :502      │
                                     │ plant Ethernet   │            │ (Modbus TCP meter)      │
                                     │                  │            │                         │
                                     │ RS485 A1/B1      │◄─ USB485 ──│ modbus_rtu_slave COMx   │
                                     │ /dev/ttyS1       │            │ (Huawei-style P/Q mock) │
                                     └──────────────────┘            └─────────────────────────┘
```

| Link | DUT | PC-A | PC-B |
|------|-----|------|------|
| **DSO MMS** | `192.168.10.1:3782` | — | `192.168.10.10` |
| **Operator 104** | `192.168.1.130:2404` | `192.168.1.183` | — |
| **Plant LAN3** | `192.168.30.1/24` | `192.168.30.10/24` | — |
| **EMT432 meter** | (polled by DUT TCP) | `192.168.30.50:502` | — |
| **RS485 plant** | A1/B1 → `ttyS1` | USB-RS485 **COM5** (typ.) | — |

**LAN3 IP alignment:** Zone docs use **`192.168.30.0/24`**; **Chronos EMT432 bench (SN 261001102952)** uses **`192.168.178.0/24`** (meter **.250**, DUT **.178.1**, PC **`set_pc_lan3_plant_178.cmd`** → **.178.10**). See [P5_EMT432_CONNECT_SESSION.md](../evidence/phase5/P5_EMT432_CONNECT_SESSION.md). Deploy **`lab_tr400_phase4_emt432_lan3_tcp.yaml`** only after **`modbus_emt432_tcp_probe.py`** PASS on **:502**.

---

## DUT deploy (pick before Phase 0)

| Goal | Yaml on DUT | MMS | 104 | Modbus P source |
|------|-------------|-----|-----|-----------------|
| **TSP + 104 same day** (recommended) | **`lab_tr400_phase4_eth_b_ttyS1.yaml`** (3782 + 104 + **ttyS1** COM5) | `:3782` | `:2404` | **RTU** A1/B1 slave |
| **TSP + 104 + EMT432 LAN3** | **`lab_tr400_phase4_emt432_lan3_tcp.yaml`** | `:3782` | `:2404` | **TCP** → **192.168.178.250:502** |
| **TSP only, RS485 mock P** | `lab_tr400_phase1_regulation.yaml` + `ttyS1` | `:3782` | off | **RTU** slave on A1/B1 |
| **Meter TCP + RS485 inverter mock** | `lab_tr400_emt432_1p.yaml` + enable `iec104` from phase4 | `:3782` | optional | **TCP** for TotW; RS485 for lab/Huawei path (see note below) |

**Modbus note:** With **`backend: tcp`**, DUT **TotW/TotVAr** come from the **EMT432 on LAN3**. USB RS485 **`modbus_rtu_slave.py`** still runs on PC-A for **Huawei inverter mock** (plant map / P5 step 13 / future FC06) and must stay wired to **A1/B1** — it does **not** replace TCP meter reads unless you switch DUT yaml to **RTU-only** (`phase1` / `cleartext_tsp_ttyS1`).

**Firmware:** deploy **`ccli` 0.1.0-r35+** (O.14 logger). Record `ccli --version` on PC-A via SSH.

---

## Phase 0 — Prep (both PCs, no TSP yet)

| □ | Who | Action | Matrix / gate |
|---|-----|--------|----------------|
| □ | Both | `git pull` · `powershell -File scripts\stage-tsp-field-pack.ps1` on **PC-B** | Field pack |
| □ | PC-B | Admin: `lab\set_pc_lan1_dso.cmd` · `ping 192.168.10.1` | LAN1 |
| □ | PC-A | Admin: `lab\set_pc_lan2_oa.cmd` · `ping 192.168.1.130` | LAN2 |
| □ | PC-A | Admin: `lab\set_pc_lan3_plant.cmd` · `ping 192.168.30.1` · `ping 192.168.30.50` | LAN3 + meter |
| □ | PC-A | RS485: A→A1, B→B1, GND common ([P5_A1B1_RS485_HW_MAP.md](../evidence/phase5/P5_A1B1_RS485_HW_MAP.md)) | Plant |
| □ | PC-B | Import CID `lab_tg544_eth_a.cid` · TLS from `CCLI_TSP_FIELD_PACK\tls\` | T1-01 |
| □ | PC-A/B | DUT: **`/etc/init.d/iec61850service stop`** · **`/etc/init.d/ccli restart`** · one `ccli` PID | Avoid 3782/2404 bind clash |
| □ | PC-A | SSH/plink DUT: confirm `mms: listening 192.168.10.1:3782` · `iec104: listening 192.168.1.130:2404` if phase4 | P3-01 / P4-01 |
| □ | PC-B | Fill session header in matrix (date, TSP version, `ccli --version`) | Specimen row |

**Do not** start Test Suite Pro until Phase 0b plant paths are up if you need live **TotW** (matrix T1-04, T1-05).

---

## Phase 0b — Plant / logger (PC-A only)

Start **before** PC-B enables URCB on DUT.

| □ | Who | Command / check | Matrix rows |
|---|-----|-----------------|-------------|
| □ | PC-A | EMT432 powered · L1+N · Rogowski/CT if you need **non-zero P** | T1-04, T1-06 |
| □ | PC-A | `python lab\modbus_emt432_tcp_probe.py` (or equivalent) → meter **502** OK | Plant comms |
| □ | PC-A | DUT log: modbus TCP trace · `link_up` in `--event-dump` | P7-04 / modbus |
| □ | PC-A | Start inverter mock (keep window open): | P5 step 13 |
|   |   | `python -u lab\modbus_rtu_slave.py --port COM5 --power-kw 450 --huawei --trace` | |
| □ | PC-A | Optional: `ccli --event-wrap-test` once on DUT · save log | P7-01 |
| □ | PC-A | Optional syslog listener on `514` if testing r35 forward | P7-03 |

**Huawei mapping:** Production plant uses registers in `apps/ccli/config/plant/PLANT_ASSIGNMENT_PLANE.csv` (e.g. `32080` TotW). Lab **`modbus_rtu_slave.py`** emulates **generic** FC03 kW at **40001** (PdC mock) — sufficient for **TSP TotW** when DUT yaml uses **RTU**. For **TCP meter** day, prioritize **EMT432** reads; keep RS485 running for bench continuity and P5 sweep.

---

## Phase 1 — MMS core (PC-B only on LAN1)

**2026-10-09:** Bench **GO** on PC-B — see [`../evidence/testsuite-pro/inbox/TSP_SESSION_START_2026-10-09.md`](../evidence/testsuite-pro/inbox/TSP_SESSION_START_2026-10-09.md).

**PC-A:** leave plant processes running; **no** 104 connect during this block unless you are on Phase 4 (104 is low traffic if monitor-only).

### Evidence logger (mandatory — every T1 row)

Before **each** TSP step: start capture + log stub + **DUT SSH PRE**. After the step: copy TSP **Output** → stop logger → **DUT SSH POST** + **Wireshark `_WIRE.txt` summary** + **`.pcapng`**.

```powershell
cd D:\CCLI\CCLI\CCLI
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\tsp-evidence-logger.ps1 -Action Init -SessionDate 2026-10-09   # once per day
powershell -File scripts\tsp-evidence-logger.ps1 -Action Start -TestId T1-08 -Operator "<name>"
# ... run one TSP action ...
powershell -File scripts\tsp-evidence-logger.ps1 -Action Stop -PasteClipboard
```

Full protocol: [`../evidence/testsuite-pro/EVIDENCE_PER_TEST_PROTOCOL.md`](../evidence/testsuite-pro/EVIDENCE_PER_TEST_PROTOCOL.md) · index `inbox/TSP_PHASE1_SESSION_INDEX_*.md`.

| □ | TSP ID | Action | Matrix □ row |
|---|--------|--------|--------------|
| □ | T1-01 | **Connect** `CCI016_01` @ `192.168.10.1:3782` TLS | Server, SCSM, TLS |
| □ | T1-02 | **Compare Model** vs CID (**before** URCB) | D3 MICS |
| □ | T1-03 | Browse LD/LN (`LD_Plant`, ~31 LNs) | Browse |
| □ | T1-04 | Read `PdCMMXU1` TotW, TotVAr, PPV | Read PdC |
| □ | T1-05 | Enable `urcb_PdC_Mis4sec` IntgPd=**4000** | URCB + dataset |
| □ | T1-06 | Observe integrity **3900–4100 ms** | URCB |
| □ | T1-07 | Operate **Wlim** enhanced | Wlim |
| □ | T1-08 | Operate **WSd** | WSd |
| □ | T1-09 | Release · wait **15 s** fallback | P3-05 |
| □ | T1-10 | Disconnect · reconnect | Associate |

**PC-A after T1-07:** `ubus call dido_v2 status` on DUT → append to `TSP_P3_04_OPERATE_Wlim_*.txt` | Actuation row |

Evidence: **one `.txt` + one `.pcapng` per T1-xx** via `tsp-evidence-logger.ps1`; names in [matrix](LAB_REQUIREMENT_TEST_MATRIX.md) and session index.

---

## Phase 2 — Annex T full circle (PC-B LAN1)

| □ | Who | Action | Matrix |
|---|-----|--------|--------|
| □ | PC-B | Run P5 sequencer on **3782** ([P5_TSP_FULLCIRCLE_SEQUENCER.md](../phase5/P5_TSP_FULLCIRCLE_SEQUENCER.md)) | P5 / Tier 2 |
| □ | PC-A | During step 13: change `--power-kw` on RTU slave **or** vary load on meter | Dynamic P |
| □ | PC-B | Export `TSP_P5_FULLCIRCLE_TLS_*.log` | Evidence |

---

## Phase 3 — Security negatives (PC-B LAN1)

| □ | TSP ID | Action | Matrix |
|---|--------|--------|--------|
| □ | T3-01 | Connect without client cert → fail | P3-06 |
| □ | T3-02 | Viewer read OK | P3-08 |
| □ | T3-03 | Viewer Operate Wlim → **deny** | P3-08 |
| □ | T3-04 | Revoked cert + CRL → fail | P3-09 |
| □ | T3-05 | DSO profile connect (if not already in T1-01) | P3-07 |

---

## Phase 4 — Operator 104 (PC-A LAN2) — optional, **not** in first-pass PICS

Run **after** Phase 1 green (or end of day). **PC-B:** disconnect TSP or idle (do not load LAN1 with heavy traffic during 104 GI if DUT is slow).

| □ | Who | Action | Matrix |
|---|-----|--------|--------|
| □ | PC-A | lib60870 / lab client → `192.168.1.130:2404` TLS | 104 row |
| □ | PC-A | GI · read TotW IOA **1001** · send **command** → expect **reject** (monitor_only) | P4 / O.13 |
| □ | PC-A | Save `phase4/P4_*` style log · `--event-dump` tail with `iec104` / `operator_asdu_*` | P7-04 |

---

## Phase 5 — P7 logger (PC-A SSH to DUT)

Can run in parallel with Phase 1 breaks.

| □ | Action | Matrix |
|---|--------|--------|
| □ | `ccli --event-dump --count 30` → UTC + `o14.*` category | P7-02 |
| □ | `ccli --event-clear` → must **fail** (exit 2) | P7-01 |
| □ | Archive `P7_01_WRAP_*` if not done in 0b | P7-01 |

---

## Phase 6 — Export & paperwork

| □ | Who | Action |
|---|-----|--------|
| □ | PC-B | Copy all `TSP_*` → USB → `lab/evidence/testsuite-pro/inbox/` |
| □ | PC-A | Copy 104 / event-dump / DUT syslog snippets |
| □ | Repo PC | Paste [PICS_FILL.md](PICS_FILL.md) → Excel (lab quote) |
| □ | Both | Tick [LAB_REQUIREMENT_TEST_MATRIX.md](LAB_REQUIREMENT_TEST_MATRIX.md) □ column |

**Exclude** unless PICS expanded: Tier 4 GOOSE/BRCB (matrix says N/A first quote).

---

## Timing diagram (one lab day)

```text
  PC-A:  [0b plant up………………………………………………] [104 optional……] [event-dump…]
  PC-B:       [0 prep][──── Phase 1 TSP core ────][ Phase 2 P5 ][ Phase 3 sec ]
  DUT:        ccli running · chrony · modbus poll 4 s · events.jsonl
```

---

## Troubleshooting (two-PC)

| Symptom | Check |
|---------|--------|
| TSP Connect fail on PC-B | Only **LAN1** on TSP PC · `.10.10` · PEMs · no second MMS client on LAN1 |
| TotW = 0 with TCP yaml | Meter **30.50** reachable from DUT · yaml `host` matches · Rogowski/CT |
| TotW = 0 with RTU yaml | PC-A **COM5** slave running · **A1/B1** · DUT `device: /dev/ttyS1` |
| 104 fail on PC-A | **LAN2** `.1.183` · phase4 `iec104.enabled` · not using TSP for 104 |
| PC-A ping 30.50 OK but DUT modbus fail | DUT LAN3 cable · firewall · wrong `modbus.host` in `/etc/ccli/lab.yaml` |

---

**RAG tags:** `TSP`, `two-PC`, `LAN1`, `LAN2`, `LAN3`, `RS485`, `EMT432`, `Huawei`, `lab-matrix`, `ccli-tsp-two-pc-bench-runbook`

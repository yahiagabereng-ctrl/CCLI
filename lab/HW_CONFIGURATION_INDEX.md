# Lab / DUT hardware configuration — master index

**Document ID:** CCLI-LAB-HW-INDEX-001  
**Date:** 2026-10-14  
**RAG source_id:** `ccli-lab-hw-configuration-index`  
**Purpose:** Single checklist of **all HW configuration artefacts** for TG544 bench + cert matrix. Use before wiring, deploy, or closing DUT evidence.

**Active session pin:** [conformance/LAB_UNIFIED_SESSION_PIN_2026-10-14.md](conformance/LAB_UNIFIED_SESSION_PIN_2026-10-14.md)  
**Cert matrix HW table:** [conformance/LAB_UNIFIED_CERT_TEST_MATRIX.md](conformance/LAB_UNIFIED_CERT_TEST_MATRIX.md) § Hardware map

---

## 1. Canonical sources (by domain)

| Domain | Document | Status | Notes |
|--------|----------|--------|-------|
| **Platform / ports** | [knowledge-base/08-engineering/CCI_TG500_Lab_Platform.md](../knowledge-base/08-engineering/CCI_TG500_Lab_Platform.md) | **HAVE** | TR400 panel, default IP/login, ttyS1/S2 |
| **DIO map (SoT)** | [TR400_HW_IO_CONFIG.md](TR400_HW_IO_CONFIG.md) | **HAVE** | DIO1 DO / DIO2 DI, yaml gpio 1/2 |
| **Relay + button install** | [CURTAIL_BUTTON_AND_RELAY_SETUP.md](CURTAIL_BUTTON_AND_RELAY_SETUP.md) | **HAVE** | Wiring + ubus + lab.yaml |
| **DIO smoke** | [TR400_DIO_SMOKE_TEST.md](TR400_DIO_SMOKE_TEST.md) | **HAVE** | PASS 2026-09-15/22 |
| **ubus API** | [DIDO_V2_API.md](DIDO_V2_API.md) | **HAVE** | GD32 / set_relay / read_di |
| **10-pin full map** | [DRC60A_UPS_WIRING.md](DRC60A_UPS_WIRING.md) | **HAVE** | AI/AO UPS; DIO1–2 PASS; see §3 conflict DIO3 |
| **Phase 1 loop** | [PHASE1.md](PHASE1.md) | **HAVE** | Modbus → PF2 → DIO |
| **DIO ↔ regulation** | [REGULATION_MOCK_DIO_MAP.md](REGULATION_MOCK_DIO_MAP.md) | **HAVE** | Scenarios A–F |
| **Modbus RTU (Phase 1)** | [MODBUS_RTU_LAB.md](MODBUS_RTU_LAB.md) | **HAVE** | Slave + wiring |
| **RS485 A1/B1 (P5 bench)** | [evidence/phase5/P5_A1B1_RS485_HW_MAP.md](evidence/phase5/P5_A1B1_RS485_HW_MAP.md) | **HAVE** | **`/dev/ttyS1`** — current cert default |
| **Two-PC / three-LAN** | [conformance/TSP_TWO_PC_BENCH_RUNBOOK.md](conformance/TSP_TWO_PC_BENCH_RUNBOOK.md) | **HAVE** | LAN1/2/3 roles |
| **Single-PC full bench** | [conformance/TSP_PC_FULL_BENCH_HW.md](conformance/TSP_PC_FULL_BENCH_HW.md) | **HAVE** | NIC IPs + COM5 |
| **Annex M trip (DIO3)** | [evidence/phase6/P6-03_ANNEX_M_INHIBIT.md](evidence/phase6/P6-03_ANNEX_M_INHIBIT.md) | **HAVE** | ch3 DO trip mock |
| **I/O voltage class gap** | [evidence/phase6/P6-06_IO_VOLTAGE_GAP.md](evidence/phase6/P6-06_IO_VOLTAGE_GAP.md) | **GAP doc** | Lab 12 V ≠ product 250 V/3 A |
| **Schematic extract** | [knowledge-base/08-engineering/CCI_TR400_GPIO_Schematic_Extract.md](../knowledge-base/08-engineering/CCI_TR400_GPIO_Schematic_Extract.md) | **HAVE** | Vendor GPIO |
| **I/O peripheral** | [knowledge-base/08-engineering/CCI_TR500_IO_Peripheral.md](../knowledge-base/08-engineering/CCI_TR500_IO_Peripheral.md) | **HAVE** | Corpus |
| **Runtime dataflow** | [Architecture/CCI_Runtime_Dataflow_Architecture.md](../Architecture/CCI_Runtime_Dataflow_Architecture.md) | **HAVE** | DIO1 path |
| **OpenWrt freeze L2** | [knowledge-base/08-engineering/CCI_OpenWrt_Freeze.md](../knowledge-base/08-engineering/CCI_OpenWrt_Freeze.md) | **HAVE** | ttyS1 + DIO1/2 |
| **Product REQ-IO** | [knowledge-base/08-engineering/CCI_Architecture_Framework.md](../knowledge-base/08-engineering/CCI_Architecture_Framework.md) | **GAP** | 5× DI 10–120 V, 3× DO 250 V/3 A |

---

## 2. Software configuration (repo → DUT)

| Asset | Path on PC | Path on DUT | Session (2026-10-14) |
|-------|------------|-------------|----------------------|
| **Lab yaml (full HW + 5-LN)** | `apps/ccli/config/product_tr400.yaml` | `/etc/ccli/lab.yaml` | **Active bench** — EMT432 TCP, DIO, **5-LN cfg** (r54+ binary) |
| **Alias** | `product_tr400_emt432_5ln.yaml` · `lab_tr400_phase4_emt432_lan3_tcp.yaml` | same | Equivalent plant/io/MMS path |
| **Lab yaml (cert mock)** | `apps/ccli/config/lab_tr400_phase4_eth_b_ttyS1_5ln.yaml` | (alternate) | DSO mock for TSP cert only |
| **Lab yaml (baseline)** | `apps/ccli/config/lab_tr400.yaml` | (alternate deploy) | Phase 1; Modbus **ttyS2** in file — do not mix with A1/B1 bench |
| **5-LN MMS cfg** | `apps/ccli/config/icd/product_tr400_eth_a_5ln.cfg` | `/etc/ccli/icd/` | Session pin |
| **HAL** | `apps/ccli/platform/platform_tg500/hal_gpio_ll.c` | in `ccli` binary | gpio == ubus channel 1–4 |
| **UCI DIO modes** | — | `/etc/config/didoservice_v2` | ch1=do, ch2=di (persist) |

**Required `io:` block (cert / production-style gating):**

```yaml
io:
  backend: ubus_dido_v2
  permissive_bypass: false
  do_curtail:    { gpio: 1, active_high: false }
  di_permissive: { gpio: 2, active_low: true }
```

Optional P6: `annex_m_trip_monitor: { enabled: true, gpio: 3 }` in `lab_tr400.yaml` (not in all phase4 yamls).

---

## 3. Bench wiring summary (2026 cert default)

| Signal | Physical | Linux / ubus | PC / peer |
|--------|----------|--------------|-----------|
| **MMS DSO** | LAN1 | `192.168.10.1:3782` | PC-B `192.168.10.10` |
| **IEC 104 OA** | LAN2 / br-lan | `192.168.1.130:2404` | PC-A `192.168.1.183` |
| **Plant / meter** | LAN3 | `192.168.30.x` or `192.168.178.x` | EMT432 TCP or GOOSE |
| **Plant meter (EMT432)** | **LAN3** | **`192.168.178.1` → `.250:502`** | Modbus **TCP** — **ccli** P/Q/PPV |
| **Inverter mock (SUN2000)** | **PC USB RS485 → A1/B1** | **`COM5` → `/dev/ttyS1`** | `lab/start-bench-plant.ps1 -SunOnly` — **not** ccli backend |
| **Modbus RTU meter (alt.)** | **A1/B1** | **`/dev/ttyS1`** | `product_tr400_rtu_a1b1.yaml` |
| **Curtail relay** | **DIO1** | ubus ch **1** DO | 12 V coil ~38 mA |
| **Permissive button** | **DIO2** | ubus ch **2** DI | 12 V NO key |
| **Annex M trip (opt.)** | **DIO3** | ubus ch **3** DO | P6-03 inhibit |

**PC setup scripts:** `lab/set_pc_lan1_dso.cmd` · `set_pc_lan2_oa.cmd` · `set_pc_lan3_plant.cmd` · `set_pc_lan3_plant_178.cmd`

---

## 4. Verification scripts

| Script | Where | Purpose |
|--------|-------|---------|
| [tr400-permissive-and-relay-setup.cmds](tr400-permissive-and-relay-setup.cmds) | plink → DUT | Modes + relay blink + DI read |
| [tr400-io-verify.sh](tr400-io-verify.sh) | On DUT | Interactive DIO walk |
| [tr400-io-verify-remote.ps1](tr400-io-verify-remote.ps1) | Windows | Non-interactive check |
| [tr400-dio-factory.cmds](tr400-dio-factory.cmds) | plink | Reset ch1–4 DO/DI modes |
| [tr400-dio2-fix.cmds](tr400-dio2-fix.cmds) | plink | DIO2 DI-only fix |
| [scripts/validate-dut-unified-matrix.ps1](../scripts/validate-dut-unified-matrix.ps1) | PC | Preflight + evidence · **`-FixServices`** enables GPIO |
| [scripts/validate-bench-hw-configuration.ps1](../scripts/validate-bench-hw-configuration.ps1) | PC-A | **Full bench** — TCP 1p meter · COM5 Sun · DIO · two-PC NICs · **`-RemoteDut`** |
| [enable-dut-gpio-full-cycle.ps1](enable-dut-gpio-full-cycle.ps1) | PC → DUT | **Enable DIO** before Wlim / U-P5-01 |
| [tg544-openwrt/enable-dut-gpio.sh](tg544-openwrt/enable-dut-gpio.sh) | On DUT | didoservice + ubus modes + ccli restart |

---

## 5. Known doc conflicts (do not ignore)

| Topic | Doc A | Doc B | **Use for bench** |
|-------|-------|-------|-------------------|
| Modbus device | `lab_tr400.yaml` → **ttyS2** (A2/B2) | P5 + session pin → **ttyS1** (A1/B1) | **ttyS1** for unified cert 2026-10-14 |
| DIO3 role | `DRC60A_UPS_WIRING.md` → spare **DI** | P6-03 + `lab_tr400.yaml` → **DO** trip monitor | **P6 / lab_tr400.yaml** when Annex M lab enabled |
| Pass criteria typo | `TR400_HW_IO_CONFIG.md` §4 table once listed ttyS2 | P5 map | Match **deployed** `grep modbus /etc/ccli/lab.yaml` |
| Default login | `CCI_TG500_Lab_Platform.md` → `root` / **000000** | — | Change after commissioning |

---

## 6. Missing / PART (product, not lab blockers)

| Item | Status | Ref |
|------|--------|-----|
| 10–120 V DI expansion | **MISS** | P6-06, REQ-IO-004 |
| 250 Vac / 3 A DO on GPIO | **MISS** | P6-06, product spec |
| AI/AO UPS bench test | **NOT TESTED** | DRC60A doc |
| DIO3/DIO4 production assignment | **PART** | P6 vs UPS spare table |
| LTE / modem M.3.1 I/O | **GAP** | P6-07 |

---

## 7. Live DUT snapshot (2026-10-15 — `192.168.1.130`, login `root` / change from factory default)

| Check | Result |
|-------|--------|
| `ccli --version` | **0.1.0-r55** (P5-5LN-CERT-MODEL) |
| GD32 | **online** · FW **GD32-DIDO-F v1.0.1** |
| ubus ch1 / ch2 | **DO** / **DI** |
| `/etc/ccli/lab.yaml` | **`product_tr400.yaml`** — `modbus.device: /dev/ttyS1` · gpio **1** / **2** |
| Full HW bench | Meter RS485 + **DIO1 relay** + **DIO2 button** — [evidence/hw/FULL_HW_BENCH_2026-10-15.md](evidence/hw/FULL_HW_BENCH_2026-10-15.md) |

SSH host key (PuTTY): `SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE`

---

## 8. Sign-off checklist (HW pack complete)

| □ | Item | Evidence |
|---|------|----------|
| □ | LAN1/2/3 IPs match matrix | ping + session pin |
| □ | `modbus.device` matches **physical** RS485 port | `lab.yaml` on DUT |
| □ | GD32 **online** | `ubus call dido_v2 gd32.status` |
| □ | DIO1 relay + DIO2 button wired | [CURTAIL_BUTTON_AND_RELAY_SETUP.md](CURTAIL_BUTTON_AND_RELAY_SETUP.md) §7 |
| □ | `permissive_bypass: false` | DUT `lab.yaml` |
| □ | Wlim → DIO1 with key closed | `dido_v2` after operate |
| □ | DUT backup yaml matches deploy | `lab/evidence/dut-fw-backup/*/lab.yaml` |

---

## 9. Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-14 | Master HW index + conflict table |

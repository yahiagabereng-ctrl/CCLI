# Bench status — software (SW) — 2026-10-09 (FINAL — test start)

**Last probe:** 2026-10-09 ~20:08 local · **3782 True** on PC-B · [`PREFLIGHT_DUT_2026-10-09_200828.txt`](PREFLIGHT_DUT_2026-10-09_200828.txt)  
**Session GO doc:** [`LAB_SESSION_START_2026-10-09_FINAL.md`](LAB_SESSION_START_2026-10-09_FINAL.md)  
**DUT:** **`0.1.0-r52`** · yaml **`lab_tr400_phase4_emt432_lan3_tcp.yaml`**

---

## SW summary (all nodes)

| Node | SW / role | Status | Notes |
|------|-----------|--------|--------|
| **PC-B** | TSP · LAN1 `192.168.10.10` | **GO** | TCP **3782** open (ICMP ping may be off) |
| **PC-B** | TLS staging | **GO** | `C:\CCLI_product_tls\` refreshed on r52 deploy |
| **PC-A** | LAN2 `192.168.1.183` | **GO** | User confirmed · re-check **:2404** before 104 tests |
| **PC-A** | LAN3 `192.168.178.10` | **READY** | `set_pc_lan3_plant_178.cmd` · EMT432 probe optional |
| **PC-A** | RS485 COM5 | **START ON PC-A** | **Huawei inverter mock** — parallel to meter TCP |
| **DUT** | `ccli` **r52** | **GO** | **`iec61850service` off** · single `ccli` · both listeners |
| **DUT** | MMS **:3782** | **GO** | TLS · full CID |
| **DUT** | 104 **:2404** | **GO** | lib60870 · monitor_only TotW@1001 |
| **DUT** | Modbus **TCP** meter | **GO** | **178.250:502** · `float_word_swap: true` · P/Q ≈ 0 |
| **DUT** | Chrony | **GO** | ~ms offset |
| **DUT** | GNSS | **RUN `gnss_start`** before URCB/time-quality tests | `ubus call sim-manager gnss_start` |
| **DUT** | O.14 logger | **GO** | `--event-dump` |

**Lab verdict:** **START FORMAL TEST** — TSP Phase 1 on PC-B; PC-A for 104 / plant when matrix row requires.

---

## DUT detail

```
ccli 0.1.0-r52 (P3-07-AARE-SIGN-RAW-GT)
yaml:   lab_tr400_phase4_emt432_lan3_tcp.yaml
listen: 192.168.10.1:3782 + 192.168.1.130:2404
modbus: tcp 192.168.178.250:502 (EMT432 TotW)
rs485:  PC-A inverter mock on A1/B1 (not ccli Modbus source in this yaml)
```

---

## Phase 0 checklist (two-PC)

| □ | Item | Status |
|---|------|--------|
| ☑ | PC-B LAN1 + **3782** | **GO** |
| ☑ | DUT r52 + listeners | **GO** |
| ☑ | PC-A LAN2 `.1.183` | **GO** (user) |
| ☑ | DUT 104 **:2404** | **GO** |
| ☑ | EMT432 TCP + word-swap | **GO** |
| ☐ | PC-A RS485 Huawei slave | **Operator** — start COM5 script |
| ☐ | PC-A LAN3 probe (optional) | Evidence P5 |
| ☐ | GNSS fix for URCB | **`gnss_start`** if not already `gnss_fix=1` |
| ☐ | TSP workspace + certs | **Operator** on PC-B |

---

## Evidence index

- [`LAB_SESSION_START_2026-10-09_FINAL.md`](LAB_SESSION_START_2026-10-09_FINAL.md)
- [`CCLI_DEPLOY_0.1.0-r52_2026-10-09_135140.md`](../../tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r52_2026-10-09_135140.md)
- [`P5_EMT432_METER_VALUES_2026-10-09.txt`](../../phase5/P5_EMT432_METER_VALUES_2026-10-09.txt)
- [`P4_03_104_LISTEN_DEPLOY_r51_2026-10-09.txt`](../../phase4/P4_03_104_LISTEN_DEPLOY_r51_2026-10-09.txt)

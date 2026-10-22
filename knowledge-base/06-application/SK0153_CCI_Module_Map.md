# SK0153 → CCI Product Module Map

**Document ID:** CCLI-APP-SK0153-MAP-001  
**Revision:** 1.0  
**Date:** 2026-10-14  
**RAG source_id:** `ccli-sk0153-module-map`  
**Product tree:** `product/apps/ccli/program/` (SK0153 entry). Lab conformance remains `apps/ccli/` (`ccli_main`). 5-LN + cyber: `product/PRODUCT_5LN_CYBER_MINIMAL.md`.  
**Reference firmware:** SK0153 – LINKY DIN 4G (Shitek / HiTEKS), `_sk0153_extract/components/ql-application/osi/` in the CCLI repo  
**Platform:** `platform_tg500` only — TesPro TG544 / TR500 (lab DUT), TG-524 / TR400 (field). OpenWrt 25.12 / TesproOS frozen.  
**Parents:** `ccli-app-structure` · `ccli-product-sw-freeze` · `ccli-openwrt-freeze`

---

## Purpose

The final CCI product follows the **SK0153 software structure**: a flat `program/` tree of C modules with Shitek banners, `Module_Function` naming and `MODULE__TYPE__NAME` constants, started as **Boot → Startup → Main**.

The protocol stacks and PF2/DSO domain logic stay in the existing C++ libraries (`core/`, `adapters/`, `services/`). `program/ccli_bridge.cpp` is the **only** C/C++ boundary. Wire behaviour (MMS :3782 TLS, IEC 104 :2404, Modbus, GOOSE), the ICD and the yaml schema are unchanged.

---

## Runtime shape

```text
main()                         program/main.c
 ├── Boot_TaskBoot()           program/boot.c      command line, one-shot modes (commands.c)
 ├── Startup_TaskStartup()     program/startup.c   bring-up order, startup type, recover
 └── Main_TaskMain()           program/main.c      100 ms cooperative supervisor
      ├── Boot_TaskDef[] pass  (one entry per SK0153-style task, fixed order)
      └── Queue main messages  (MAIN__TASK_MSG_ID__END_OF_SYNCHRONIZE, …)
```

SK0153 runs each module as a Quectel RTOS task. CCI runs the same modules as entries in `Boot_TaskDef[]`, executed once per 100 ms tick by `Main_TaskMain()`. The order **is** behaviour and matches the lab-validated `services/ccli_main.cpp` loop:

| # | Task | Period | Was in `ccli_main.cpp` |
|---|------|--------|------------------------|
| 1 | `ParserModbus_Task` | 100 ms | lab-demo ramp + `modbus.tick()` |
| 2 | `FPf2_Task` | 100 ms | `pf2.tick()` |
| 3 | `ParserModbus_LinkTask` | 100 ms | Modbus link up/down/recovered events |
| 4 | `Alarm_MeterQualityTask` | 100 ms | meter quality events |
| 5 | `TransmissionMms_PublishTask` | 100 ms | `mms.update_tot_w_kw / totvar / ppv` |
| 6 | `Transmission104_PublishTask` | 100 ms | `iec104.update_tot_w_kw` |
| 7 | `Synchronize_Task` | 100 ms | `mms.refresh_time_quality()` (+ first GNSS fix → Main message) |
| 8 | `Transmission104_Task` | 100 ms | `iec104.tick()` |
| 9 | `TransmissionMms_Task` | 100 ms | P3-05 comms-loss fallback + DSO live command → PF2 / PFSP / VArSd |
| 10 | `Transmission104_EventTask` | 100 ms | P4-04 comms-loss + operator ASDU audit |
| 11 | `Regulation_Task` | 100 ms | permissive / Annex M interlock → DO |
| 12 | `TransmissionGoose_LinkTask` | 1 s | GOOSE link monitor |
| 13 | `Main_StatusTask` | 5 s | iface watch, status line, `/var/run/ccli-status.json` |
| 14 | `ParserModbus_TraceTask` | 100 ms | `modbus.trace` |
| 15 | `ProgramFlash_Task` | 100 ms | (new) drains backup requests — stub |
| 16 | `Main_WatchdogTask` | `kDefaultWatchdogPingS` | procd watchdog ping |

Startup keeps the legacy order: platform init → yaml → **DI/DO forced safe** (IEC 62443-4-2 CR 3.6 direction) → supervision handlers → O.14 log open → startup type / crash recovery events → runtime objects → first Modbus + PF2 step → first DO drive → MMS, 104, GOOSE servers last.

---

## Module map

| SK0153 file (`osi/`) | CCI module (`program/`) | Role in CCI | Implementation behind it |
|----------------------|-------------------------|-------------|--------------------------|
| `boot.c` | `boot.c` | Command line, cooperative task table (`Boot_TaskDef[]`) | — |
| `startup.c` | `startup.c` | Bring-up order, `STARTUP__STARTUP_TYPE__*`, recover + `*_VerifyVarCrc()` | `ServiceSupervision::startup_check` |
| `main.c` | `main.c` | Supervisor loop, `Main_EndOfSynchronize`, status, watchdog, shutdown | — |
| `drvgpio.c` | `drvgpio.c` | DI/DO driver, polarity, safe state (`DrvGpio_*`, single definition) | `platform/platform_tg500/hal_gpio_ll.c` (ubus `dido_v2`) |
| `outputs.c` | `outputs.c` | Curtailment DO, permissive DI, Annex M trip monitor, DO gate | `drvgpio.c` |
| `regulation.c` | `regulation.c` | Interlock: DO = PF2 command ∧ permissive ∧ ¬Annex M (CCLI-VAL-001 §7, O.11) | `outputs.c`, `f_pf2.c` |
| `f_regulation.c`, `f_antifrost.c`, `f_chrono.c` | `f_pf2.c` | PF2 function step | `services/svc_pf2` → `core/pf2/pf2_fsm` |
| `transmission_gprs.c`, `transmission_sms.c` | `transmission_mms.c` | Eth_A IEC 61850 MMS (62351-3/-4), DSO Wlim/WSd/PFSP/VArSd | `services/svc_mms` → `adapters/iec61850_mms` |
| — | `transmission_104.c` | Eth_B IEC 60870-5-104 (operator) | `services/svc_iec104` → `adapters/iec60870_104` |
| — | `transmission_goose.c` | Plant LAN GOOSE subscriber link monitor | `services/svc_goose` → `adapters/iec61850_goose` |
| `parser_rx.c`, `parser_tx.c` | `parser_modbus.c` | Plant meter poll, link state, trace | `services/svc_modbus` → `adapters/modbus_master` |
| `alarm.c` | `alarm.c` | O.14 event append, meter quality alarms | `core/event/event_store` |
| `alarm_input.c` | `alarm_input.c` | Permissive DI + Annex M edge events | `alarm.c` |
| `alarm_power.c`, `alarm_tmin.c`, `alarm_tmax.c` | — | Not ported (PSU unmonitored on TG544: `psu_unmonitored` event) | — |
| `program_flash.c` | `program_flash.c` | Backup request API (`HANDLE_INDEX__*`, `FLASH_ID__*`) — **stub** | `queue_my.c` |
| `queue_my.c` | `queue_my.c` | Ring queues: Main messages, flash backup requests | — |
| `synchronize.c` | `synchronize.c` | Time quality refresh, first GNSS fix → `Main_EndOfSynchronize()` | `adapters/iec61850_mms/gnss_time` |
| `fw_config.c`, `default.c` | `fw_config.c` | Compile-time constants + loaded yaml snapshot | `core/config/ccli_config` |
| `commands.c`, `commands_ext.c` | `commands.c` | `--version`, `--regulation-check`, `--event-dump`, `--bench-*`, `--gpio-*` one-shot modes | `program/ccli_tools.cpp` |
| `typedef.h` | `inc/typedef.h` | `u8 … s64`, `ascii`, `TRUE/FALSE` | — |
| `debug_my.c` | (stderr) | Diagnostic text stays on stderr → procd/logread | — |
| `reset.c`, `id.c`, `device_id.c` | `startup.c` + `core/version` | Startup type, firmware identity (`ccli --version`) | — |
| `dota_*.c`, `boot_flash.c` | — | Not ported — OTA is OpenWrt sysupgrade / `.apk` | — |
| `phone*.c`, `sms_*.c`, `smtp.c`, `http_my.c`, `strings_*.c`, `led.c`, `ui_led.c`, `credit.c`, `pod.c` | — | Not applicable to CCI (GSM / LINKY UI domain) | — |

**C/C++ boundary:** `program/inc/ccli_bridge.h` (C API, `CcliBridge_*`) ↔ `program/ccli_bridge.cpp` (owns `MeasurementStore`, `EventStore`, Modbus, PF2, MMS, 104, GOOSE runtime objects). `program/ccli_tools.cpp` holds the one-shot command helpers shared with the legacy entry.

---

## Build

| Option | Default | Entry |
|--------|---------|-------|
| `CCLI_LEGACY_MAIN=OFF` | ✔ | `program/main.c` — SK0153 Boot → Startup → Main |
| `CCLI_LEGACY_MAIN=ON` | | `services/ccli_main.cpp` — bisect only |

```bash
cmake -S apps/ccli -B build -DBUILD_PLATFORM=tg500 -DCCLI_BUILD_TESTS=ON
cmake --build build && (cd build && ctest)
```

CMake ≥ 4 needs `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` (vendored libiec61850 / lib60870 declare old minimums). Targets: `ccli_core`, `ccli_platform`, `ccli_adapters`, `ccli_services`, **`ccli_program`** → `ccli` (`-static-libstdc++` on tg500).

**libiec61850 patch:** the 62351-4 ACSE/AARE hooks (`AcseConnection_setCcliAareAuthBuilder`, `AcseConnection_setCcliRespondingApTitle`) are applied in `third_party/libiec61850` from `third_party/patches/libiec61850-519b020-ccli-62351-4-acse.patch` (base commit `519b020`). Without it `ccli` does not link.

---

## Verification (2026-10-14, host WSL Ubuntu, gcc 15.2, BUILD_PLATFORM=tg500)

| Check | Result |
|-------|--------|
| `ccli` (program entry) and `ccli` (`CCLI_LEGACY_MAIN=ON`) build + link | PASS |
| Unit tests (8): pf2_fsm, dso_phase1, setpoint_gate, service_supervision, event_store_o14, io_safe_state, **program_queue_my**, mms_acse_auth | PASS |
| `DrvGpio_*` defined once (`program/drvgpio.c`) | PASS |
| Side-by-side `--lab-demo` run (`lab_regulation.yaml`), legacy vs program entry: stderr + O.14 | PASS — identical apart from config path |
| Side-by-side 70 s run (`lab_tr400_phase4_eth_b.yaml`, MMS TLS + ACSE auth + RBAC + CRL, 104 TLS) | PASS — both listen 127.0.0.1:3782 + :2404; 52 stderr lines each, diff only config path and host chrony offset; O.14: same 17 events in same order (power_on → … → curtail_on → comms_loss_fallback → power_off), timestamps only |

On-target (TG544) proof is still required before promoting to `releases/product-cyber/`: `.apk` build, TSP connect on LAN1 :3782, functional + cyber matrix regression.

---

## Intentional follow-ups

| ID | Item | Why open |
|----|------|----------|
| SK-F01 | Real `program_flash` persistence + `*_UpdateVarCrc` / `*_VerifyVarCrc` (Outputs, Regulation, DrvGpio, Queue) | Stubs return TRUE; outputs always restart from safe state (correct for CR 3.6). Needs P7 logger / `ccli-product-sw-freeze` persistence requirements |
| SK-F02 | Threaded MMS / 104 servers as separate tasks | Single-thread cooperative loop is lab-validated; split only with timing evidence (O.14 latency, 62351 handshake) |
| SK-F03 | Move `Main_StatusTask` bench JSON and iface watch into dedicated `log_status.c` | Cosmetic SK0153 parity |
| SK-F04 | Retire `services/svc_io.c` forwarders and `CCLI_LEGACY_MAIN` | After TG544 regression on the program entry (Phase 5 cutover) |
| SK-F05 | `commands.c` AT-style commission table (SK0153 / SK0146 conventions) | Only command-line one-shot modes today |
| SK-F06 | OpenWrt SDK packaging of this tree | `package/ccli/Makefile` accepts `CCLI_SRC_DIR=`; the folder name contains a non-printing character that breaks Make paths — build from a clean copy or rename the folder |
| SK-F07 | 5-LN product CID under `config/icd/product/` | **HAVE** `product_tr400_eth_a_5ln.cid` + `product_tr400.yaml` (2026-10-15) |

---

## Keywords

`SK0153`, `HiTEKS`, `Shitek`, `program/`, `Boot_TaskBoot`, `Startup_TaskStartup`, `Main_TaskMain`, `Boot_TaskDef`, `ccli_bridge`, `drvgpio`, `outputs`, `regulation`, `transmission_mms`, `transmission_104`, `parser_modbus`, `program_flash`, `queue_my`, `synchronize`, `fw_config`, `CCLI_LEGACY_MAIN`, `ccli-sk0153-module-map`

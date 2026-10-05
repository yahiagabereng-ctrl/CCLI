# CCLI release changelog (lab deploys)

Canonical version: `apps/ccli/VERSION` (semver + OpenWrt **PKG_RELEASE** + codename).  
Every DUT upload must have a matching entry here and a file under `lab/tg544-openwrt/deploy-manifests/`.

Format: **`semver-rN`** — bump **N** in `VERSION` line 2 and `package/ccli/Makefile` `PKG_RELEASE` together.

---

## 0.1.0-r29 — EQ-PLANE-R07-SPACING (2026-10-03)

**Codename:** EQ-PLANE-R07-SPACING  
**Phase:** Annex equation utilization — P0 item + single assignment plane

### Added

- **O.7.3.3 / Eq (9) — R07:** `core/dso/setpoint_gate.hpp` `SetpointSpacingGate` (header-only, monotonic clock).
  `mms_adapter.cpp` rejects `WMaxSptPct` / `WSptPct` / `VArTgtSptPct` writes arriving < 3 s after the last
  processed set-point (`CONTROL_RESULT_FAILED`, stderr `REJECT — set-point spacing`, O.14 event
  `mms/setpoint_reject_spacing_o733`). `Mod` activation writes are not gated.
- Config `mms.setpoint_min_interval_s` (default **3**; `0` disables — lab only). Added to `lab_tr400_cleartext_tsp.yaml`.
- Unit test `setpoint_gate_test` (ctest `dso_r07_setpoint_spacing`).
- `Flowcharts/ANNEX_EQUATION_UTILIZATION_MATRIX.md` — per-equation RAG · draw.io · code · verdict + backlog P0–P3.
- `signal_map.yaml` rev 2: `equations:` index (eq1…eq18), `application_groups` A0–A8, `goose_map`,
  `logical_nodes` for all **31** LNs of `lab_tg544_eth_a.cid` with `equation` / `regulation_id` / `drawio_ref` / `binding`.

### Changed

- `regulation_map.hpp` R07 → **Pass / Eq (9)**; `--regulation-check` R07 row now reads `mms.setpoint_min_interval_s`
  (≥3 PASS · 1–2 PART · 0 NOT IMPL).

### Fixed (build)

- Host/cross build blocker: `goose_receiver.h` needs `src/r_session` include dir (libiec61850 R-GOOSE header);
  `GooseAdapter::Impl` was defined inside an anonymous namespace (ill-formed) — moved out. `ccli` host build is green again.

### Behaviour note

Any client writing two DSO set-points within 3 s (TSP scripts, IEDScout bursts) will see the second Operate fail.
This is normative; set `setpoint_min_interval_s: 0` only for throughput bench profiles.

---

## 0.1.0-r28 — P7-04-O14-GAPS (2026-10-02)

**Codename:** P7-04-O14-GAPS  
**Phase:** 7 — O.14 category wiring (software gaps)

### Added

- O.14 events: `system` (power_on/off, firmware_boot, psu_unmonitored stub)
- `modbus` link_up / link_down / link_recovered on poll transition
- `meter` quality_* on measurement quality change
- `di` permissive_ok / permissive_blocked
- `mms` client_connect/disconnect + ACSE auth accept/reject + rbac_deny_control
- `dso` polygon_seed at Phase-1 config apply
- `annex_m` trip_relay_active / trip_relay_cleared edges
- Auto-create `/var/lib/ccli` in EventStore + deploy script

### Changed

- Pf2Service logs only on state/reason **change** (reduces event spam)

### Added (GOOSE — P5-G02)

- `lab_tg544_eth_a.cid`: `GSEControl` **gcb_PdC_Mis4sec** + **gcb_Stato_Allarmi** with GSE comms (APPID 0x1000 / 0x1001)
- Regenerated `lab_tg544_eth_a.cfg` (`GC(...)` blocks)
- `MmsAdapter::enable_goose_publishing()` — libiec61850 integrated IedServer GOOSE
- Lab yaml: `goose.publish_enabled: true` on cleartext TSP profile

### Still open

- PSU HW, PG/PI DI, net anomaly, firmware **update**, remote syslog (P7-03) — see `P7-04_GAPS_BACKLOG.md`
- GOOSE lab Wireshark evidence (P5-G01) · plant subscribe mapping (P5-G03)

---

## 0.1.0-r27 — P7-01-EVENT-STORE (2026-10-02)

**Codename:** P7-01-EVENT-STORE  
**Phase:** 7 — O.14 data logger foundation

### Added

- `EventStore` — 2048-event ring + append-only JSONL at `/var/lib/ccli/events.jsonl`
- CLI `ccli --event-dump [--count N] [--json]` — O.14 timestamp `yyyy/mm/dd hh:mm:ss` (UTC)
- CLI `ccli --event-wrap-test` — verifies ring wrap at 2048 + file compaction
- Config block `event_log:` (`enabled`, `path`)

### Note

Syslog remote read (P7-03) and full O.14 category coverage remain Phase 7 open items.

---

## 0.1.0-r25 — P5-COMPARE-FIX (2026-10-01)

**Codename:** P5-COMPARE-FIX  
**Phase:** 5 — TSP Compare Model alignment

### Changed

- CID `lab_tg544_eth_a.cid`: Wlim/WSd/VArSd `ctlModel` → **direct-with-enhanced-security** (matches lab Operate)
- CID: `PdCMMXU1` PPV/A **DOI** instance values (phsAB=20 kV) for TSP Compare tree
- Regenerated `lab_tg544_eth_a.cfg` from CID
- MMS server: disable **BRCB.ResvTms** (TSP Compare vs CID)
- Removed runtime `IedServer_updateCtlModel` override (redundant with CID)

### Note

Run TSP **Compare Model before enabling URCB** to avoid RCB OptFlds/TrgOps drift.

---

## 0.1.0-r24 — P5-R01 (2026-09-30)

**Codename:** P5-R01  
**Phase:** 5 — reactive power command path (O.9.1.4 VArSd)

### Added

- `dso_reactive_power` — VArSd pct × Smax → kVAr target (O.9.1.4 / T Table 88)
- MMS control handlers: `VArSdDVAR1.{Mod,VArTgtSptPct}` → `mms→plant` Q write
- Modbus FC16 write to reg 40003 (`write_reactive_kvar`) — lab plant path
- `lab/mms_varsd_client.c` — cleartext Operate helper for step 11

### Changed

- `ccli_main` — applies reactive command on `reactive_dirty` from MMS adapter
- `P5_LN_WIRE_STATUS.md` — VArSd **W-L-C**; control handler count **3**

---

## 0.1.0-r23 — P5-FULL-CID (2026-09-30)

**Codename:** P5-FULL-CID  
**Phase:** 5 — full CID model (31 LNs) + Wireshark LN evidence

### Added

- `mms.model_cfg` loads `lab_tg544_eth_a.cfg` via libiec61850 ConfigFileParser
- `scripts/gen-mms-model-cfg.ps1`, `lab/mms_ln_browser.c`, `run-p5-cleartext-ln-capture.ps1`

### Changed

- Lab yamls: `model_cfg: /etc/ccli/icd/lab_tg544_eth_a.cfg`
- Deploy script uploads `.cfg` to DUT `/etc/ccli/icd/`

---

## 0.1.0-r22 — P5-PPV (2026-09-30)

**Codename:** P5-PPV  
**Phase:** 5 — P5-M08 + LN audit

### Added

- `PdCMMXU1.PPV` (CDC **DEL** / phsAB) + URCB dataset member
- `update_ppv_kv()` — lab `plant.poc_ppv_kv` until Modbus V register (P5-06)
- `lab/evidence/phase5/P5_00_LN_MATRIX_AUDIT.md` — CID vs runtime matrix

### Fixed

- Reactive/MSD stub LNs: **Mod** CDC **ENS → ENC** (61850-7-3 / TR CID)

---

## 0.1.0-r22 — P5-PPV (2026-09-30)

**Codename:** P5-PPV  
**Phase:** 5 — P5-M08 + LN audit

### Added

- `PdCMMXU1.PPV` (CDC **DEL** / phsAB) + URCB dataset member
- `update_ppv_kv()` — lab `plant.poc_ppv_kv` until Modbus V register (P5-06)
- `lab/evidence/phase5/P5_00_LN_MATRIX_AUDIT.md` — CID vs runtime matrix

### Fixed

- Reactive/MSD stub LNs: **Mod** CDC **ENS → ENC** (61850-7-3 / TR CID)

---

## 0.1.0-r21 — P5-TotVAr (2026-09-30)

**Codename:** P5-TotVAr  
**Phase:** 5 — Observability kickoff (P5-M07)

### Added

- MMS `PdCMMXU1.TotVAr` dynamic model + `DS_R_PdC_Mis4sec` member
- `update_totvar_kvar()` — Modbus `q_kvar` → Eth_A MMS every poll (4 s)
- Lab client reads TotVAr; `lab/evidence/phase5/` + `run-p5-m07-totvar-test.ps1`

### Changed

- `signal_map.yaml` — PdC.TotVAr **IMPL**; urcb dataset includes TotVAr

---

## 0.1.0-r20 — P4-104-o14-params (2026-09-30)

**Codename:** P4-104-o14-params  
**Phase:** 4 — O.14 command audit with parameters + MSD command mode stub

### Added

- O.14 audit **detail string**: `type`, `ioa`, `value`/`state`, `select`, `result=accept|reject`
- **Command mode** accept path: `C_SE_NC_1` O.10.3.1 MSD set-point lab stub (`ioa_msd_sp`)
- `lab_tr400_phase4_eth_b_command.yaml` — command-mode lab profile
- Client `--accept-test` / `--msd-ioa` / `--msd-kw` for P4-05 positive test

### Changed

- EventRing stores full O.14 detail (not generic `operator_asdu_reject` only)
- P4-05 gate: parameter logging **PASS** on accept/reject; persistent store still Phase 7

---

## 0.1.0-r19 — P4-104-operator-rule (2026-09-30)

**Codename:** P4-104-operator-rule  
**Phase:** 4 — Operatore Abilitato Operating Rule config

### Added

- **Operating Rule** yaml keys under `iec104:` — mode, allow GI/commands/clock sync, monitor IOAs
- `iec104_operator_rule.hpp` — annex monitor/command catalog (TotW, TotVAr, PPV)
- `iec104_operator_rule.yaml` — reference template
- O.14 **reject audit** — command ASDUs rejected in `monitor_only`; EventRing `operator_asdu_reject`

### Changed

- `lab_tr400_phase4_eth_b.yaml` — full operator_rule profile
- P4-05 gate: **PART** (reject audit + config; full command log when command mode enabled)

---

## 0.1.0-r18 — P4-104-tls (2026-09-30)

**Codename:** P4-104-tls  
**Phase:** 4 — Operator 104 + 62351-3 TLS

### Added

- CS104 **TLS** (P4-03 / IEC 62351-3): `CS104_Slave_createSecure`, lab PEM paths
- `iec104_tls.cpp`, `iec104_regulation.hpp` — Annex O / gate traceability in code
- `CONFIG_CS104_SUPPORT_TLS` when shared mbedtls present (libiec61850 tree)
- Lab client `p4_cs104_client --tls` for P4-03 verification

### Changed

- `lab_tr400_phase4_eth_b.yaml`: `iec104.tls_enabled: true`

---

## 0.1.0-r17 — P4-104-eth-b (2026-09-30)

**Codename:** P4-104-eth-b  
**Phase:** 4 — Operator 104 on Eth_B

### Added

- IEC 60870-5-104 CS104 slave (lib60870 v2.3.5) on **Eth_B only** (`192.168.1.130:2404`)
- `iec104:` yaml section; lab profile `lab_tr400_phase4_eth_b.yaml`
- P4-01 bind guard (rejects `0.0.0.0`, `192.168.10.1`)
- GI + periodic TotW (M_ME_NC_1, IOA 1001)
- Embedded build version (`ccli --version` / `--version-json`)
- Deploy manifest workflow before TG544 upload

### Unchanged from r15/r16 lab line

- MMS Eth_A TLS :3782 (Phase 3 Solution C)
- PF2 / Modbus / DSO phase-1 regulation path

### Lab config default for P4 deploy

- `apps/ccli/config/lab_tr400_phase4_eth_b.yaml` → `/etc/ccli/lab.yaml`

---

## 0.1.0-r15 — Phase 3 lab close (2026-09-25)

**Reference:** `lab/evidence/O13_1_isolation_2026-09-25/SESSION_REPORT_P3_LAB_CLOSE_2026-09-25.md`

- MMS TLS :3782, RBAC, CRL, P3-05 fallback, chrony TimeQuality
- Package **`ccli-0.1.0-r15`** official Phase 3 lab gate

---

## Prior releases (summary)

| Release | Notes |
|---------|--------|
| r5–r14 | Phase 3 MMS/PF2 iteration on TG544 |
| r1–r4 | Phase 1–2 Modbus, DIO, segmentation |

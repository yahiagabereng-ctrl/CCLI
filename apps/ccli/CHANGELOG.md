# CCLI release changelog (lab deploys)

Canonical version: `apps/ccli/VERSION` (semver + OpenWrt **PKG_RELEASE** + codename).  
Every DUT upload must have a matching entry here and a file under `lab/tg544-openwrt/deploy-manifests/`.

Format: **`semver-rN`** — bump **N** in `VERSION` line 2 and `package/ccli/Makefile` `PKG_RELEASE` together.

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

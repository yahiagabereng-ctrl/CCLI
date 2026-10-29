# CCLI release changelog (lab deploys)

Canonical version: `apps/ccli/VERSION` (semver + OpenWrt **PKG_RELEASE** + codename).  
Every DUT upload must have a matching entry here and a file under `lab/tg544-openwrt/deploy-manifests/`.

Format: **`semver-rN`** — bump **N** in `VERSION` line 2 and `package/ccli/Makefile` `PKG_RELEASE` together.

## 0.1.0-r76 — Continuous RTU worker layout fix (2026-10-29)

- Keep ModbusAdapter layout independent of the private libmodbus build flag.
- Prevent its context pointer from overwriting ModbusService's adjacent stop flag after the first RTU read.
- Add a cross-translation-unit layout regression check for backend and caller builds.

## 0.1.0-r75 — Device CRL pull tooling (2026-10-29)

- Package signed CRL fetching, rollback rejection, complete multi-issuer snapshot publication and a configurable procd worker.
- Keep fetching disabled until issuer endpoints and pinned anchors are commissioned; preserve existing controller settings during upgrades.
- Add curl/OpenSSL CLI runtime dependencies and remove unconditional cleaning from development builds.
- Record host signed-fixture tests and a successful EJBCA CRL fetch on TesPro; full identity-session termination qualification remains open.

## 0.1.0-r74 — Logger reader and collector reliability (2026-10-29)

- Make log export a read-only EventStore operation, without extra producers, forwarding, directory creation or permission changes.
- Isolate the 2048-event wrap/reload test from live OpenWrt audit sources and clean both temporary segments.
- Fix unsigned auth-queue capacity underflow; bound pipe reads by available queue slots.
- Tie logread lifetime to its parent, close inherited pipe descriptors across exec, and reset partial input after collector restart.
- Retain the r73 independent RTU worker and commissioned Tespro configuration; remote Syslog stays optional and disabled.

## 0.1.0-r73 — Independent inverter RTU poller (2026-10-29)

- Read inverter P/Q/voltage on a dedicated Modbus RTU worker while the POC meter remains on its independent TCP poller.
- Sample inverter registers every 1 s; expose inverter quality and readings in CCLI status and publish active power through `GenPVMMXU1.TotW` when that LN is in the MMS model.
- Keep the inverter poller separate from the meter measurement path so serial timeouts do not block the CCI loop.

## 0.1.0-r72 — Multiple MMS TLS identities for IEC 62351-3 (2026-10-29)

- Keep the ECDSA P-256 MMS identity and add a separately validated RSA-3072 server identity so cipher suites can select a matching key type.
- Enable the required TLS 1.2 suites `TLS_RSA_WITH_AES_128_CBC_SHA256`, `TLS_DHE_RSA_WITH_AES_128_GCM_SHA256`, and `TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256`; honor the client cipher-suite order.
- Validate each configured endpoint identity's key pair, chain, CRL status, EKU, key policy, and certificate size before enabling the listener; recheck additional identities during CRL/session validation.
- Tespro `192.168.0.1:3782` verification: each suite completed a mutually authenticated TLS 1.2 handshake with server-certificate verification; a read-only IEC 61850 MMS `check` also passed. No write/control services were exercised.

## 0.1.0-r71 — Personal Tespro owner-branch integration (2026-10-27)

- Integrate owner e6d6ce0 meter/phasor and Operating Rule updates while retaining personal TLS/RBAC/native E2E and Aggregator implementation.
- Keep the validated personal application entry and 192.168.0.1:3782 / 23-node model defaults. Existing commissioned device configuration and credentials are preserved on upgrade.
- Guard dynamic-model MMS values before initialization; add the TLS CRL to the native product test fixture. All 19 selected host regressions passed before packaging.

## 0.1.0-r70 — Keep TLS listeners available with an expired CRL (2026-10-27)

- A correctly signed but expired CRL raises a warning instead of preventing MMS or IEC 104 listener startup.
- Preserve established sessions on CRL expiry and keep known revocations enforced. Initial handshakes still require current CRL data for the certificate issuer.
- Reject malformed, future-dated, missing-nextUpdate, unauthorized-issuer and bad-signature CRLs; certificate validity and trust checks remain enforced.
- Test listener startup with expired CRLs, recovery after publishing a fresh CRL without restart, and active-session survival across CRL expiry.

## 0.1.0-r69 — Correct suite ID and report TLS policy failure stage (2026-10-27)

- Use the registered TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 suite identifier `0x009E` (RFC 5288).
- Restore mandatory TLS_RSA_WITH_AES_128_CBC_SHA256 with the existing encrypt-then-MAC requirement; Table 1 marks RSA-NULL-SHA256, not RSA-CBC, conditional.
- Preserve mandatory TLS 1.2 suite validation while reporting the exact failed stage for timer/CA setup, endpoint chain/EKU, CRL validation, or unavailable cipher support.

## 0.1.0-r68 — Intermediate TLS suite-ID correction (2026-10-27)

- An attempted correction to `0xC09E` was invalid; RFC 5288 assigns this suite `0x009E`. r69 restores the registered value and adds stage-specific policy events.

## 0.1.0-r67 — Enable mandatory TLS 1.2 suites (2026-10-27)

- Align the bundled libiec61850 Mbed TLS build with the IEC 62351-3 mandatory
  DHE-RSA and ECDHE-RSA/ECDSA AES-128-GCM suites required by CCLI's TLS policy.
- Keep optional `TLS_RSA_WITH_AES_128_CBC_SHA256` disabled by default.

---

## 0.1.0-r66 — IEC 62351-3 TLS closure work (2026-10-26)

- Requires a configured, valid CRL for TLS-enabled MMS and IEC 104 endpoints.
- Adds distinct CRL and revocation-session alarms, certificate-size handling,
  and TLS-version stability checks across renegotiation.
- Documents operator CRL provisioning and the current profile's OCSP exclusion.

---

## 0.1.0-r65 — Native E2E bench build (2026-10-26)

- Integrates the current native IEC 62351-4 Annex G E2E association, codec,
  protected MMS service, and error-handling work for TesPro bench validation.
- This build is not a claim of full CEI conformance: application-encryption
  transfer support (AES-CBC/GCM) and on-device validation remain open.

---

## 0.1.0-r64 — CEI 57-142 Aggregator interface (2026-10-21)

- Implements the signed Aggregator P/Q targets, CEI mode values, function status,
  PdCi measurements, static dataset, and buffered/unbuffered reports in the
  hand-built and generated IEC 61850 models.
- Keeps Aggregator plant outputs disabled by default; startup rejects an explicit
  attempt to enable the unvalidated signed P/Q backend.
- Preserves the existing TLS and ACSE transport/session implementation and
  credentials. Device-specific signed policy activation is recorded in the
  commissioning evidence for this release.

---
## 0.1.0-r60 — Owner protocol / Profile A integration (2026-10-16)

- Merge owner `upstream/main` (ffb0f19) into the personal r59 branch (b185266).
- Retain Profile A RBAC, separate application/TLS identities and controlled APK
  preservation. Integrate the owner protocol fixes after source review/testing.
- Preserve both branches' historical release notes below; their r57 labels refer
  to different branches and do not supersede the merged r60 version.
- Archive the owner's legacy DER CRL for reference; active trust/CRLs unchanged.
- Merge verification is recorded in `lab/conformance/OWNER_BRANCH_MERGE_2026-10-16.md`.

---

## 0.1.0-r58 — Preserve TLS validation errors for pinned certificates (2026-10-15)

- Corrects the upstream allowlist verifier through a reproducible generated
  source overlay: certificate pins no longer clear certificate/CRL errors.
- With chain validation enabled, preserves all validation errors. Explicit
  pin-only mode waives only issuer trust, retaining expiry/revocation checks.
- Adds direct TLS handshake regressions for valid/revoked/expired/untrusted and
  unknown clients, stale/bad-signature CRLs, and explicit pin-only behavior.
- Keeps TLS 1.2 settings, existing credentials and controlled APK upgrade guards.

## 0.1.0-r57 — Profile A RBAC and controlled APK upgrade (2026-10-15)

- Integrates MMS Profile A roles, signed owner commissioning, certificate/CRL
  validation, identity binding and authorization. Profile A stays disabled.
- Removes bundled TLS credentials from the APK. Upgrade hooks preserve and
  restore existing configuration/credentials, retaining a persistent backup.
- Holds automatic starts until configuration is verified and the upgrade hold
  is explicitly released. Existing working TLS/ACSE behavior is a deployment gate.

## 0.1.0-r55 — Unified certificate package integration (2026-10-14)

### Included

- Preserves the local MMS/AARE signing and USB updater work from the personal branch.
- Includes the owner’s unified 5-LN certificate test matrix, product specimen, and deployment field pack.

---

## Owner branch 0.1.0-r62 — P5-EMT432-PDC-3PH-CMV (2026-10-22)

### Added

- **PdCMMXU1 full CMV path:** `PPV.phsAB/BC/CA` **mag.f + ang.f** and `A.phsA/B/C` **mag.f + ang.f** from EMT432 RMS block when **`modbus.poll_pdc_3ph: true`** (auto offsets 916–944).
- **1P Rogowski:** unused legs read ~0; **I** angle from **atan2(Q1,P1)**; **PPV** angles 0°. **3P:** nominal DEL voltage angles −120°/120° when I2/I3 or V_BC active.
- **`modbus_pdc_decode`**, **`PdcMmxuPhasors`**, **`MmsAdapter::update_pdc_phasors`**.

### Changed

- Lab/product yaml: **`poll_pdc_3ph: true`** on **`product_tr400_p3_07_connect_5ln.yaml`** and **`lab_tr400_phase4_emt432_lan3_tcp.yaml`**.
- Commissioning map §2 documents 3φ register → MMS mapping.

### Deploy

Manifest **`CCLI_DEPLOY_0.1.0-r62_2026-10-22_183656`**. ELF SHA `20a974f1…` · DUT **`192.168.1.130`** · Modbus block **40907 qty=40** with **`poll_pdc_3ph: true`**.

---

## Owner branch 0.1.0-r61 — P5-EMT432-A-PHSA-BRIDGE (2026-10-22)

### Fixed

- **TestPro `PdCMMXU1.A.phsA.cVal.mag.f` stuck at 0 A** while TotW/PPV updated: product path **`TransmissionMms_PublishTask`** now passes EMT432 phase-A current into **`CcliBridge_Mms_PublishMeasurement`** → **`MmsService::update_phs_a_a()`**.
- **MMS runtime:** `model_mx_float_f` + FC=MX fallback for `A.phsA.cVal.mag.f`; publish trace in adapter.
- **URCB dataset:** explicit `DE(PdCMMXU1$MX$A$phsA$cVal$mag$f)` in `product_tr400_eth_a_5ln.cfg`.

### Added

- Modbus **`reg_phase_a_current: 922`** (EMT432 I1), decode + **`I1=`** bench trace; lab **`mms_tls_client.c`** reads phase-A MX.

### Deploy

Manifest **`CCLI_DEPLOY_0.1.0-r61_2026-10-22_181748`**. Staging ELF SHA `448007a2…` · yaml **`product_tr400_p3_07_connect_5ln.yaml`** + P3-07 TLS.

---

## Owner branch 0.1.0-r60 — P5-EMT432-A-PHSA-PUBLISH (2026-10-22)

### Changed

- MMS float wire + URCB dataset leaf for phase-A; Modbus poll exposes **`phs_a_a`** on exchange record.

---

## Owner branch 0.1.0-r59 — P5-EMT432-A-PHSA-WIRE (2026-10-22)

### Fixed

- **`wire_cfg_runtime_nodes`:** bind `A.phsA.cVal.mag.f` at runtime (compile-safe in `MmsAdapter::Impl`).

---

## Owner branch 0.1.0-r58 — P5-EMT432-A-PHSA-MAP (2026-10-22)

### Added

- Config map EMT432 **I1 @922** → PdCMMXU1 phase-A; commissioning map + 5-LN product yamls.

---

## Owner branch 0.1.0-r57 — P5-5LN-ISO-PRES-AARE (2026-10-16)

### Fixed

- **Initiate Response Failed on r56:** ISO session CONNECT (payload=1332) then immediate DISCONNECT — `IsoPresentation_parseConnect` rejected TSP’s extra **mmsAuthenticationAS** PCDL entry. Unknown contexts are skipped; CPA now returns **3** accept results.
- Restored **62351-4 AARE** in `acse.c`: responding AP/AE (`1.1.1.999.1` / 12), tags **0x88 / 0x89 / 0xAA**, CERTIFICATE mechanism OID, prefer AARQ auth-value over TLS peer cert.

### Deploy

Keep 5-LN yaml `product_tr400_p3_07_connect_5ln.yaml` + P3-07 TLS. Rebuild then `deploy-product-fw-same-cyber.ps1 -ReplaceBinary`.

---

## Owner branch 0.1.0-r56 — P5-5LN-ISO-SESSION (2026-10-16)

### Fixed

- **ISO session X.225 extended LI** restored in `iso_session.c` / `iso_connection.c` (TSP MMS Security AARQ > 255 bytes). r55 `ccli-bin` lacked this patch → TLS OK then **Association timeout / Transport connection closed** with 5-LN CID.
- Inbound CONNECT: decode 3-octet LI + PGI 194 user data. Outbound ACCEPT: PGI 194 + extended SPDU LI when payload > 254.

### Deploy

Keep 5-LN yaml `product_tr400_p3_07_connect_5ln.yaml` + P3-07 TLS. Rebuild `wsl-build-ccli.sh` then `deploy-product-fw-same-cyber.ps1 -ReplaceBinary -LabYaml …_5ln.yaml`.


---

## 0.1.0-r54 — P5-5LN-CERT-MODEL (2026-10-14)

### Added

- **Product 5-LN certificate model** — `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid` (**CID rev 1**, `configRev 20261014`) + generated `.cfg` (5 LNs, URCB options **159**). `validate-5ln-cid.ps1` **PASS**.
- Cert-run yaml **`lab_tr400_phase4_eth_b_ttyS1_5ln.yaml`** — `model_cfg: /etc/ccli/icd/product_tr400_eth_a_5ln.cfg`.
- `deploy-ccli-session-fix.ps1` uploads product 5-LN cfg to `/etc/ccli/icd/` when present.

### Changed

- **MMS adapter:** `VArSdDVAR1` / `PFSPDFPF1` are now **optional** in `wire_cfg_runtime_nodes` — the 5-LN cert model binds 3782; O.9.1.4 / O.9.1.1 log *inactive* when the LNs are absent. All consumers were already nullptr-guarded (P3-05 fallback, control handler, Mod updates).

### Deploy

DUT 2026-10-14 15:04 UTC — binary SHA `705752f8…` · cfg SHA `7E13DE13…` verified on DUT · log: `mms: full CID model loaded from /etc/ccli/icd/product_tr400_eth_a_5ln.cfg`.

---

## 0.1.0-r53 — P5-PPV-EMT432-UNIFIED-CERT (2026-10-14)

### Added

- **PdCMMXU1.PPV** from Modbus when **`reg_line_voltage`** is set (EMT432 **V_L1_L2 @980**, **`line_voltage_scale: 0.001`**).
- Lab yaml **`lab_tr400_phase4_emt432_lan3_tcp.yaml`** includes P/Q/PPV meter block.

### Changed

- **`deploy-ccli-dut.sh`**: disable **iec61850service** before starting **ccli** (single owner of **3782**).
- Unified cert preflight: **`scripts/validate-dut-unified-matrix.ps1`** reads **`apps/ccli/VERSION`**.

### Build note

OpenWrt SDK build requires **CCLI `acse.c` fork** (62351-4 AARE tags + bridge getters) as documented in `lab/TSP_P3_07_LIVE_CONNECT_TEAM_HANDOFF.md`. Until that patch is re-applied to `third_party/libiec61850`, **keep DUT on shipped `ccli-bin` r52** and treat **r53** as repo source-of-truth for matrix/specimen pins.

### Deploy (after successful SDK build)

```powershell
lab/tg544-openwrt/wsl-build-ccli.sh
. lab/tg544-openwrt/lab-env.ps1
lab/tg544-openwrt/deploy-ccli-bench-lan2.ps1
```

---

## 0.1.0-r52 — EMT432 float word-swap (2026-10-09)

### Fixed

- **Modbus TCP** float decode: **`modbus.float_word_swap`** for Chronos EMT432 (low register word first). Lab yaml **`lab_tr400_phase4_emt432_lan3_tcp.yaml`**.

---

## 0.1.0-r49 — P7-O14-LOGGER (2026-10-09)

### Fixed

- Bench status **TotW `published`** is true only when IEC 60870-5-104 is **running** (not merely enabled in yaml).

### Lab (two-PC bench)

- **`plant-map-generate.py --lab-bench`**, Huawei **`modbus_rtu_slave.py`**, zone dashboard cache-bust + lab plant map deploy script host key.

### Deploy (other PC)

- Build with **`lab/tg544-openwrt/wsl-build-ccli.sh`** (`CCLI_WITH_LIB60870` default ON) then **`deploy-ccli-session-fix.ps1 -HostAddr 192.168.1.130`** with lab yaml **`lab_tr400_phase4_eth_b_ttyS1.yaml`**.

---

## 0.1.0-r35 — P7-O14-LOGGER (2026-10-09)

### Added

- **O.14 datalogger finalize** — annex category on every event; RFC 5424 UDP syslog (P7-03);
  firmware `old→new` detect; GOOSE RX timeout; Eth `watch_ifaces` operstate; `--event-clear` **rejected**.
- Thread-safe `EventStore` mutex; PF2 events persist to jsonl (was memory-only).
- File mode **0640**; wrap drops oldest only (no user erase API).
- Unit test `event_store_o14`.

### Notes

- PSU / PG-PI / IDS still HW **N/A**; syslog is local UDP `127.0.0.1:514` until SIEM is wired.
- Yaml: `event_log.syslog_*`, `event_log.watch_ifaces`, `goose.timeout_s`.

### Revert to r34

1. **Git:** `git checkout 0.1.0-r34` (or previous tag).
2. **DUT:** redeploy r34 `ccli-bin`.

## 0.1.0-r34 — P7-ZONE-DASHBOARD (2026-10-05)

### Added

- **Lab zone dashboard** — read-only web UI: DSO (Eth_A), Operator (Eth_B), Plant (LAN3/RS485), Events (O.14).
- `bench-status` JSON **`zones`** block + `q_kvar` in `ccli_main.cpp` (mock console fields unchanged).
- CGI `?action=events`; `plant-map-generate.py` → `plant_ui_map.json` + `plant_map.json`.
- `deploy-zone-dashboard.ps1` (web + CGI only; does not overwrite `/etc/ccli/lab.yaml`).

### Notes

- Inverter cards show CSV mapping; live poll remains single-slave meter only.
- Demo mode (`?demo=1`) for off-device layout review.

### Revert to r33

1. **Git:** `git checkout 0.1.0-r33`.
2. **DUT:** redeploy r33 `ccli-bin`; remove `/www/ccli/zone_dashboard.*` if needed.
3. **Verify:** `ccli --bench-status --json | grep -q zones` → absent on r33.

## 0.1.0-r33 — P5-EMT432-TCP-PFSP (2026-10-05)

### Added

- **P5-R02 completion** — MMS `PFSPDFPF1` Operate parsing in `mms_adapter` + `apply_live_pfsp_command()` /
  `derive_pfsp_kvar()` in `dso_reactive_power` (r32 main loop referenced PFSP but adapter/derive were missing
  from tagged source — **r33 makes PFSP buildable**).
- **Modbus TCP backend** — `LibmodbusTcp` for Chronos EMT432 on LAN3 (`modbus.host`, `tcp_port`, `power_scale`).
- Lab yaml `lab_tr400_emt432_1p.yaml`, `chronos_emt432_map.yaml`, probe `lab/modbus_emt432_tcp_probe.py`.
- P5 EMT432 / PFSP evidence, Chronos extract, `handoff/masomeh95/` package.

### Notes

- Revert **r34** first if only the zone dashboard is unwanted; revert **r33** to drop PFSP wire-up + Modbus TCP.
- Binary SHA256: run `lab/tg544-openwrt/wsl-build-ccli.sh` then update manifest.

### Revert to r32

1. **Git:** `git checkout 0.1.0-r32` (tag `42899bf8…`).
2. **DUT binary:** redeploy pre-r33 `ccli-bin` or rebuild from r32 tree (manifest r32 SHA256 was PENDING_BUILD).
3. **Verify:** `ccli --version` → `0.1.0-r32`; PFSP Operate will not parse on Eth_A until r33 is restored.

## 0.1.0-r32 — P7-SR62-WATCHDOG (2026-10-05)

### Added

- **62443 SR 6.2 / FR 7 (partial)** — `core/service/service_supervision.cpp`:
  - Fatal signal handlers (SIGSEGV, SIGABRT, SIGBUS, SIGFPE, SIGILL) write crash tombstone,
    best-effort `svc_io_safe_state()`, then `_exit`.
  - Startup detects tombstone → O.14 `security/crash_recovered:SIG*`; stale running marker →
    `security/unclean_restart`.
  - Main loop procd watchdog ping (`ubus call service event '{"type":"watchdog"}'`) every 30 s.
  - `ccli.init`: `procd_set_param watchdog 90` (hang → procd restart).
- O.14: `security/service_monitor:started|shutdown` on clean lifecycle.

### Notes

- HW WDT→DO failsafe (DRB) remains **OPEN** — software path only.
- SR 6.2 full compliance still needs SIEM/IDS; this closes service health + crash audit gap.

### Revert to r31

1. **Git:** `git checkout 0.1.0-r31` (tag after this commit) or revert the r32 commit on your branch.
2. **DUT binary:** redeploy `ccli-bin` from manifest `CCLI_DEPLOY_0.1.0-r31_2026-10-05_161342.*`
   (SHA256 `e3659ed074a355e100b5e5a0450c3c7174229547f262298e0ae36dda4fa8a38f`).
3. **Init script:** restore `/etc/init.d/ccli` without `procd_set_param watchdog 90` (r31 had respawn only).
4. **Verify:** `ccli --version` → `0.1.0-r31`; no `security/crash_recovered` events unless a crash occurred.

## 0.1.0-r31 — P5-R02-PFSP (2026-10-05)

### Added

- **P5-R02** — `PFSPDFPF1` Operate on `PFGnTgtSpt` / `PFLodTgtSpt` + `Mod` (O.9.1.1 Eq 12).
- `derive_pfsp_kvar()` — Q from measured P × tan(acos(|cosφ|)).
- O.14 event `pfsp_operate mod=… cosphi=… p_kw=… q_kvar=… result=…`.
- Lab client `lab/mms_pfsp_client.c`.

### Notes

- PFSP takes priority over VArSd in same poll tick (P5-R07 partial).
- Full O.11 reactive arbiter still OPEN.
- Lab client `mms_pfsp_client` uses **SBO** (`selectWithValue` + `operate`) — cfg `ctlModel=4` on PFSP APC DOs.
- Lab evidence: `lab/evidence/phase5/P5_R02_PFSP_EVENT_2026-10-05.txt` · P5-R **CLOSED** 2026-10-05.

## 0.1.0-r30 — P5-R09-O14-VARSD (2026-10-05)

**Codename:** P5-R09-O14-VARSD  
**Phase:** P5-R09 — O.14 reactive Operate event detail

### Changed

- `ccli_main.cpp` — VArSd plant path logs O.14 detail `varsd_operate mod=… pct=… q_kvar=… result=…`
  (replaces bare `varsd_update` / `varsd_off` strings; `std::to_string` concat for musl safety).

### Fixed (deploy)

- r30 requires **clean** SDK rebuild (`make package/feeds/ccli/ccli/clean`); first incremental r30 binary
  segfaulted on DUT after `dso-phase1` log (SHA a6adc8…); clean rebuild OK (SHA 188230…).

### Evidence

- `lab/evidence/phase5/P5_R09_VARSD_EVENT_2026-10-05.txt` (r29 category PASS; r30 detail re-test)

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

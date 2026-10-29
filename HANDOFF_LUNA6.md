# CCI / Tespro handoff for Luna 6

> Native E2E work of 22 October 2026 is recorded in [HANDOFF_E2E_2026-10-22.md](HANDOFF_E2E_2026-10-22.md). Read it before continuing E2E. Its ARM64 binary has not been packaged or installed; the r64 installation below remains historical device evidence.

Updated: 21 October 2026, Europe/Rome. The latest deployment and validation facts are at the top; older sections remain historical where explicitly superseded.
This is a record of user intent, work and evidence, not a declaration of standards conformance.

## Latest task — r64 Aggregator installation and live validation (21 October)

Installed manufacturer-signature-verified CCLI 0.1.0-r64 on Tespro at 192.168.0.1. APK SHA-256: 2694837eed4cdd59e00335e31e6b5508b3b050a4a4cac02906de82a9af380efb. CCLI process PID 20894 was running and listening on 192.168.0.1:3782. The active Profile A owner-signed policy is sequence 4, digest 5f4b7dde602f7ce20bc5004de633f8098404a9f5aeca000ebc4db1e337dfba8b. All 27 configured TLS/RBAC paths matched the pre-upgrade backup; TLS/ACSE/session/presentation implementation and credentials were preserved.

The IEC 61850 client passed mutual TLS/Profile A association, Aggregator status and dataset reads, P/Q setpoint operations with successful termination/readback, P/Q mode -1/on to -5/off transitions, and denial of a DSO-only measurement. The BRCB replayed 93 buffered reports, some with old or epoch measurement timestamps; these are retained reports, not 93 new simulator events. Buffer overflow was not checked. The first 3-second URCB listen was shorter than its 4-second integrity period, so zero reports was expected with static data; a 6-second retest received one periodic report and passed. Both report blocks ended disabled. Final P/Q targets are 100%/0%, both modes are -5/off, Aggregator active flags are false, and do_curtail_on is false. Plant outputs remained disabled throughout.

The user confirmed the inverter simulator was not running, so zero/stale Modbus measurements were expected in this validation. MMS returned Good quality flags with zero values while device status was stale; confirm quality propagation when the simulator is running. No simulator or COM8 action was performed. The running device still uses the handbuilt model; generated CID/CFG was not selected.

Detailed record: lab/evidence/aggregator-live-install-2026-10-21/RESULTS.md and lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r64_2026-10-21_093408.md.

## 0. Previous task — Tespro / TSP interoperability (20 October)

Latest follow-up: the user also requested that our existing client act like
TSP. It is now rebuilt with a client-only TSP profile. Actual COTP request
matches the capture byte-for-byte; the presentation request matches after
normalizing only fresh proof time/signature. Request fragments are 1021/572
(microsecond timestamp adds seven bytes); fragmented AARE 1021/247 is correctly
reassembled. The client validates the server certificate chain/time/signature
before allowing operations. GUI exposes the profile, verified server identity,
and an optional separate MMS server CA. Reopen `gui.cmd` for the new interface.
Tests: nine authentication cases, live read/browse/control-info/read on one
connection through an authenticated relay, CLI and GUI backend, GUI construction,
and DSO write/operate/readback versus Viewer denial on a synthetic port 3783
harness. The temporary harness was stopped. No live control was operated.
At the time of this earlier record the installed APK was r63. The 21 October r64 update is recorded above.

The preceding task fixed the Tespro server and deferred client changes until
the user's later explicit request. Observed and implemented behavior is recorded
in `apps/ccli/tools/mms-client/TSP_INTEROPERABILITY_NOTES.md`.

Running Tespro binary: **0.1.0-r63, TSP-PRESENTATION-CONTEXT**, PID 12191 at the
last check, owning 192.168.0.1:3782. Binary SHA-256:
`cd1b233b41c8d5360846ec447458fb1fbb599d9eaf8e57c8b162a1c2cdfda2de`.

r62 fixed the multi-second audit-log rewrite delay. Run 3 exposed a separate
parser defect: TSP context 5 uses authentication syntax `1.0.840.0.1.1.4.1`,
while our generated parser/client used an extra zero. r63 accepts the canonical
syntax and the exact old-client alias, with authentication/RBAC checks intact.
It also derives the generated context BER lengths from the corrected OID size.

Verified on the live device with the exact captured TSP framing, certificate,
context ordering, and two COTP fragments; only the timestamp/signature was
refreshed. Accepted AARE + MMS Initiate response in 91.6 ms, response signature
verified. This is a test probe, **not an actual run of the TSP application**.
Our unchanged client also authenticated/read TotW = 0.0. That value does not
prove inverter communication: the live Modbus log still reports read failures.
The actual TSP PC should now reconnect for final application-level confirmation.

The operator subsequently confirmed that the actual TSP connection succeeded,
then explicitly requested an APK, personal-repository checkpoint, and APK
installation. The manufacturer-signed APK was built with the canonical toolkit
from source commit `31f4b2abee3d0de5d34c3546ca83ebaf169579af` and installed over
SSH without rebooting or changing trust. The package database now reports r63.
The running APK binary SHA-256 is
`f37a783e5936baceed72f8265c08f42e40c4226cab10c7132035f1b5b13cf418`,
PID 26349 at verification. Its TSP-shaped signed association passed in 95.8 ms
and the unchanged client read succeeded after package installation. This
supersedes the earlier manually installed binary/PID recorded above.

Local artifact: `apps/firmware-updater/artifacts/ccli-0.1.0-r63.apk`;
SHA-256 `74df622f8b6bf584cefa020b4714a340409bec7e75b5ae239003ed02d04b148a`.
The artifact, build metadata and install receipt remain ignored. Package hooks
preserved all pre-existing configuration, credentials and audit files; checks
passed before releasing the upgrade hold. The package-hook backup is
`/etc/ccli-upgrade.hMLCKL`. There are no pending/hold markers left.
An older 9 MB historical aggregator backup was verified/copied to the ext4 SD
card at `/mnt/mmcblk0/ccli-history-20261020/aggregator-backup-20261019` to make
space for the upgrade backup. Its old `/etc/ccli/aggregator-backup-20261019`
path remains a symlink; both copies on the card are retained, root-only.

The APK binary's built timestamp is the SDK's reproducible SOURCE_DATE_EPOCH
(2026-06-29), not the actual packaging date. External release metadata records
the source commit and verified package/binary hashes; do not interpret that
embedded timestamp as evidence of an old executable.

Backup of r62 binary, configuration and settled audit files:
`/etc/ccli/tsp-r63-backup-20261020` (mode 700). Protected-file hashes verified
unchanged and both existing audit segments preserved. The isolated test server
on 3783 was stopped; only the live 3782 listener remains.

Evidence and deployment/test scripts:
`lab/evidence/delay-review-2026-10-20/run3/R63_FIX_AND_VALIDATION.md`.
Do not start/stop COM8 or the inverter mock for this connection task.

## 1. Current task and most important facts

The user wants to run the inverter simulator themselves and have our Test Suite Pro-style MMS app read its measurements from CCI on Tespro. **Do not start, stop or take COM8 away from their simulator unless asked.**

Last read-only check:
- MMS/TLS association and reads succeeded.
- P = 0 kW and Q = 0 kvar, repeatedly.
- Tespro CCI status reported RTU link **down**, `read_fail`, and measurement quality **stale**.
- MMS incorrectly reported measurement quality **Good** after GNSS time synchronization. This is a confirmed discrepancy, not valid simulator data. **Do not treat protocol PASS or q=Good as proof of healthy Modbus.**
- Clock synchronization was restored, and signed DSO TLS + ACSE authentication passed again.
- SSH is reachable again after an intermittent early boot failure. No flashing/reset/recovery was performed by the assistant.

The next work should distinguish RTU communication failure from register-map mismatch and fix the MMS quality discrepancy. Preserve the user's running simulator and all device configuration.

## 2. Workspace and constraints

Main source: `C:\Users\Administrator\Desktop\CCLI Alireza\CCLI Merged`.
Sibling `C:\Users\Administrator\Desktop\CCLI Alireza\CCLI` contains the original standards PDF and protected lab PKI. Do not switch development back to the sibling checkout.

There are many modified/untracked files from this work, with no commit requested. Do not reset/clean/revert them. `git status --short` shows the changes. On this host Git may need:

```powershell
git -c safe.directory='C:/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged' status --short
```

User's AGENTS instructions: APK signing and USB bundles must use only `apps/firmware-updater`; first read `apps/firmware-updater/docs/SIGNING_AND_USB_UPDATES.md`. Use canonical scripts, keys, tools, artifacts and bundles there. Never substitute SSH installation for a requested USB test. Keep private keys confidential/ignored and out of packages/bundles. Operator passphrase belongs only in the user's visible local prompt.

Do not force the clock, reboot, factory reset, flash or erase while diagnosing. The user specifically asked for natural GNSS synchronization and is concerned about losing configuration during recovery.

## 3. Device, tools and access

- Tespro SSH: `root@192.168.0.1`. Password was supplied in the chat; not duplicated here.
- SSH host key: `SHA256:fy0pZiLnyHHY6hKpcR7ukqBAb22qfNxU72Qpv/hseJM`.
- Boot console identifies **Tespro TR500 (Oolite-MT7987A SPI-NOR), MT7987A, 2 GiB RAM**. Older project paths/documents call the platform TG544/TG500; do not infer board recovery images from those names.
- TesproOS `32-2.1.0 r601-af70fcaaa2`; Linux 6.12.68 in the successful boot capture.
- MMS endpoint: **192.168.0.1:3782**, mutual TLS 1.2.
- PuTTY tools: `C:\Program Files\PuTTY\plink.exe`, `pscp.exe`; use `pscp -scp` because SFTP is unavailable.
- Windows Python: `C:\Users\Administrator\AppData\Local\Python\pythoncore-3.14-64\python.exe` (pyserial/pymodbus installed).
- WSL Python has cryptography and builds the native MMS client. WSL/network/CIM/process operations often need tool sandbox escalation.
- Wireshark: `C:\Program Files\Wireshark\{Wireshark,tshark,dumpcap,capinfos}.exe`.
- PC Ethernet 2 is the Tespro link. During outage DHCP fell back to 169.254.94.88; an **active-only 192.168.0.137/24 address** was added for diagnosis and was retained for recovery. Verify current PC network state before changing anything. Ethernet 4 is a different network.

## 4. Inverter simulator / Modbus — unresolved

User's original simulator:
`C:\MY\GIT\cci-software\Inverter Simulator\sun2000_100ktl_m2_rtu_real_register_mock.py`.
The user now runs this themselves in VS Code, or may choose the bridge wrapper below. Confirm which one is actually running before assuming register support.

Physical path: PC COM8 via RS485 adapter -> Tespro **RS485-2**, aliased `/dev/rs485_2_uart` -> `/dev/ttyUSB1` (FT232R USB port `1-1.5`). Settings: **9600 8N1, Modbus RTU unit 1**. RS485 only was requested; do not substitute Modbus TCP.

COM8 was temporarily moved to the RS232 boot console during recovery; user later explicitly confirmed it was reconnected to RS485-2. Adapter mode, polarity and wiring remain possible causes, not proven faults.

Bridge in main repo: `lab/inverter-simulator-bridge/bridge.py`, `start-rtu.cmd`, `state.json`.
- Reuses original Huawei model without editing its source.
- Adds CCI compatibility float32, high-word-first raw registers: **60000 P(kW), 60002 Q(kvar), 60004 V(V)**.
- Existing state: fixed **10 kW, 1 kvar, 400 V**.
- `--accept-cci-q-writes` accepts FC16 complete two-register Q write at 60002; changes simulated Q and state.json. P/V stay read-only. Original Huawei writable controls merely store/log and do not change P/Q.
- Tests of Q write/readback and invalid writes passed previously.
- Original Huawei integer power registers include 32080/32082; original script alone does not supply the bridge's CCI float view. CCI currently reads 60000..60005.
- CCI logs display `addr=100001 qty=6`, which is human display base 40001 + raw address 60000. Do not mistake 100001 for the 16-bit wire address.

Latest observations:
- CCI logs repeated FC03 `read_fail`; source links libmodbus (confirmed loaded in `/proc/<ccli-pid>/maps`). Config is RTU, 9600/N/unit1, raw 60000/60002/60004. Read failure closes the serial handle, so an instantaneous fd listing may show no tty handle.
- While assistant mocker was running, no decoded requests were logged. A 14-second passive COM8 raw read captured **zero incoming bytes**. This does not prove the same condition for the user's current run.
- Seriald also owns RS485-2. User explicitly approved disabling only `serialservice.port3.enabled` and reloading serialservice for a test. It **did not help**, so it was **restored to 1** as promised. Do not claim this conflict was the established cause.
- Backup: `/etc/ccli/rs4852-sharing-backup-20261019/serialservice.before`.
- RS232 and RS485-1 serial sharing stayed enabled. Reload restarts seriald; do not casually repeat changes.

Assistant mocker history: PIDs 6772, 19832, then 11572. **All stopped**; 11572 was stopped after user's own simulator got COM8 Access Denied. COM8 open/close was verified after release. `simulator.pid` still says 11572 and is **stale**: never kill by that file alone; inspect process name and full command first. No assistant boot capture should be assumed still running; earlier captures were time bounded.

## 5. Our Test Suite Pro-style MMS app

Location: `apps/ccli/tools/mms-client/`.
Files: `client.py`, `client.cpp`, `run.ps1`, `run.cmd`, `run-aggregator.cmd`, README and verification scripts.
Native executable: `apps/ccli/build-mms-client/ccli-mms-client` (Linux/WSL).

Features implemented: interactive browse/read/watch, quality/time decoding, typed writes/operates with APPLY confirmation, automatic control-model/type discovery, enhanced termination + same-session readback, datasets/reports, and persistent native connection until Exit/Reconnect. No automatic replay of writes after disconnect. A control request accepted/terminated successfully is not physical inverter proof.

PowerShell unsigned-script workaround:
```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File apps/ccli/tools/mms-client/run.ps1 check
```
This is process-scoped, not a global execution-policy change.

Default ignored `client.local.json` selects signed ACSE DSO identity; ignored `aggregator.local.json` selects commissioned TLS Aggregator identity. `run-aggregator.cmd` uses that profile. Do not expose private-key contents.

DSO authentication paths:
- Signed application certificate (default): TLS + ACSE proof requires client/server clocks within +/-120 seconds; failed while GNSS off, then **passed after latest GNSS repair**.
- Commissioned role-bearing TLS certificate: `dso/dso.pem` + `dso/dso.key`, `--auth-mode tls`; DSO association and `WlimDWMX1.Mod.stVal[ST]` read succeeded even while signed authentication failed. State 5 = off; 1 = on.
- Protected lab PKI root: sibling `CCLI/lab/ejbca-staging/rbac/`. EJBCA issues certificates but does not necessarily retain endpoint-generated private keys. Some lab PEM keys exist on the PC; not a verified full Tespro/TPM backup.

Repeatable read-only measurement helper:
```powershell
wsl.exe python3 '/mnt/c/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged/lab/evidence/gnss-startup-fix-2026-10-19/read-modbus-via-mms.py'
```
Uses one persistent Viewer-r0 TLS connection; reads P/Q, both q attributes, timestamp, then three P samples. Writes `modbus-via-mms.json`. Never sends controls and never touches COM8. Device cross-check: `/var/run/ccli-status.json`.

## 6. Confirmed MMS quality bug — next source investigation

Before GNSS synchronization MMS q was questionable/inaccurate. Once chrony synchronized, MMS q became all zero/Good even though RTU remained stale and `read_fail`, P/Q zero. Timestamp also continued to refresh. This can mislead an operator.

Inspect `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp`, especially `MmsAdapter::refresh_time_quality()` near line 1982 and assignment of QUALITY_VALIDITY_GOOD near line 2082. **Suspect** time-quality refresh overwrites measurement validity; not yet debugged or fixed. Preserve measurement validity/detail bits independently of clock status. Check stale/oldData and timestamp semantics with meaningful regression tests. Do not conflate clockNotSynchronized=0 with measurement health.

## 7. GNSS/chrony — latest fix installed, clock restored

Initial vendor `/etc/init.d/gnss` defined start_service but lacked USE_PROCD=1; boot start was a no-op. First one-line fix worked live but failed acquisition after boot: GNSS start returned no_response before modem readiness, and script falsely logged started with no retry.

Latest deployed fix:
- `lab/tg544-openwrt/gnss.init` -> `/etc/init.d/gnss`.
- `lab/tg544-openwrt/gnss-start-worker` -> `/usr/sbin/gnss-start-worker`.
- procd worker `startup` starts asynchronously (callback returned in **0.05s**).
- Waits until uptime >=90 seconds, matching modem quiet period, plus broker running/live PID/character port. Uses broker ubus, never directly opens modem tty.
- Bounded ubus calls, explicit powered_on verification, retry every15 seconds, health recheck every60 seconds. No false success on success=false/no_response. Stop kills worker before GNSS off request.
- Seven real-shell mocked-response/nonblocking checks in `lab/tg544-openwrt/test_gnss_startup.py` passed; shell syntax passed locally and on Tespro.
- Live: powered_on true, valid fix with6 satellites, feeder delivered samples; chrony selected `#* GNSS`, stratum1, Leap Normal. Existing `makestep 1.0 3` policy naturally corrected ~1984s; no manual clock setting or chronyc makestep was used.
- Signed DSO check subsequently passed, clockNotSynchronized=0.
- Initial estimator dispersion was high; **do not claim certified +/-100ms absolute UTC accuracy** from this startup check.
- No reboot was performed to test latest worker. Full cold-boot validation remains pending because of independent intermittent bootloader failure.

Chrony config `/etc/chrony/chrony.conf`; feeder `/usr/sbin/gnss_chrony_sock`, socket `/var/run/chrony.gnss.sock`. NTP pool fallback was unavailable without WAN/default route/DNS. CCI observes time; it does not discipline the clock.

Backups: original `/etc/init.d/gnss.before-20261019`; prior one-line fix `/etc/ccli/gnss-startup-retry-backup-20261019/gnss.init`.
Evidence: `lab/evidence/gnss-startup-fix-2026-10-19/{RETRY-FIX.md,retry-worker-verification.txt,retry-worker-chrony-final.txt}`.

## 8. Boot incident / LAN observation — do not flash

User reported SYS LED off, power/Wi-Fi on, no LAN. Captured COM8 115200 8N1 boot twice:
```
BL2 v2.13.0, OpenWrt v2025.07.11~78a0dfd9-1 (mt7987-nor-comb-gainstrong)
Built 12:59:20, Jun 29 2026
ERROR: xz: xz_dec_run() failed (err = 7)
ERROR: Failed to decompress image (err=-5)
ERROR: BL2: Failure in post image load handling (-5)
```
This is **before Linux and GNSS/updater scripts**. Earlier speculation that GNSS blocked this failure was corrected by console evidence.

User supplied `C:\Users\Administrator\Downloads\tespro_support_capture_2026-10-08 (2).zip`. Its Oct8 console capture has the **same build and identical three errors**, so failure predates today's GNSS change. Vendor previously said their BootLoad issue was fixed for future machines; no proof this unit received the fix.

Later passive capture successfully reached BL31, U-Boot and Linux. U-Boot identified TR500 and warned bad environment CRC/use defaults, customer NOR recovery environment applied in RAM, random Ethernet MAC. Kernel/device-tree/rootfs crc32+sha1 checks passed and existing JFFS2 overlay mounted. USB1-1.4/1-1.7 enumeration errors, TPM initial self-test warning, then NUL console bytes occurred. **No kernel panic captured.** Seriald config sets ttyS0 to9600, potentially related to output becoming unreadable; not established.

User says boot succeeded with LAN disconnected. Logs show external lan1 link first up ~69.4s after Linux started; consistent with reconnecting later. LAN-connected boot is a **suspected trigger, not confirmed cause**. One successful boot does not establish resolution.

Evidence under `lab/evidence/gnss-startup-fix-2026-10-19/`: `boot-console-115200.bin`, `boot-console-reread.bin`, `.txt`, `BOOT-INCIDENT.md`, `BOOT-REREAD.md`.

Updater investigation: our updater installs signed application APKs; source contains no bootloader-flashing commands. Recent persistent audit entries showed rejected/expired bundle authorization, not a new installation corresponding to this failure. Cannot prove every historical cause, but no evidence implicates our updater. Audit `/mnt/mmcblk0/firmware-updater/security-audit.log`.

No factory reset, erase, firmware flash or bootloader recovery was performed. User previously lost configuration during vendor recovery and wants preservation. Only partial CCI/config/PKI backups exist; **not a verified whole-device/TPM backup**. If recovery becomes necessary, preserve flash first using a verified board-specific procedure.

## 9. Historical CCI and Aggregator implementation (superseded by r64 deployment above)

Historical r61 deployment details follow; the current live binary is 0.1.0-r64 as recorded in Latest task above.
SHA256 binary: `861ec666a0aced3b9c97ac0d210c2bffe4c14233d7cd5ef8e3ef66ad4fbca4d0`.
SHA256 lab.yaml: `346e0b653d224231c542e664cf50ada443835e44436e3fe43705b5825a203c5c`.
Both matched after successful reboot. PID5045 was latest but re-check before use.

Implemented Aggregator enhanced-direct controls:
- `CCI016_01LD_Plant/WSaDAGC1.Mod`, `WSptPct`.
- `CCI016_01LD_Plant/VArSaDVAR1.Mod`, `VArTgtSptPct`.
- Modes1/on and5/off only. P0..100% export ceiling; Q-100..100%, clamped to plant envelope; shared3s setpoint gate.
- P ceiling cannot relax DSO/autonomous limits; **not signed import/export tracking**.
- Q priority: DSO direct Q > DSO PF > Aggregator Q > none.
- Merge changes preserves unrelated actors/baseline; latest status is recomputed. On global all-client loss,15s fallback resets controls. Not per-actor expiry.
- Physical P remains existing PF2/export-limit relay logic; no continuous inverter P tracking.
- Own Aggregator datasets/reports, full CID model regeneration and complete Annex O behavior remain absent.

Changed sources: `mms_adapter.{cpp,hpp}`, `core/dso/dso_active_power.*`, `core/dso/dso_phase1.*`, `services/ccli_main.cpp`; new unit `aggregator_power_test.cpp`; isolated harness `tests/integration/mms_rbac_device_server.cpp`.
Tests:2 regulation/arbitration unit tests,34 isolated Aggregator control checks,342 isolated role checks passed. **Physical live P/Q proof not completed**: one live run timed out waiting for effective P21kW; later run aborted on stale RTU before controls. Cleanup logic was hardened. Do not claim physical control works.
Evidence `lab/evidence/mms-aggregator-2026-10-19/`. Backups `/etc/ccli/aggregator-backup-20261019/{ccli,lab.yaml}`.

Build environment: WSL `/home/adminavz/ccli-tls-allowlist-20261019/{source,host-build,target-build}`; OpenWrt SDK25.12.5 mediatek-filogic/gcc14.3 musl. IEC104 is stubbed in these builds (disabled in live config); libIEC61850 + mbedTLS3.6 and libmodbus used. Do not claim IEC104 conformance.

## 10. Roles / allowlist and prior evidence

Repeatable `mms.tls_allowed_client_cert` support added in config parser + MMS adapter (custom parser permits repeated keys; not general YAML semantics). Enrolled missing public TLS certificate pins, retaining validation/CRL.19 commissioned identities include DSO, Aggregator, standard roles rev0/rev1, multi-role and dual-cert identities. All19 initially connected;143 live checks passed before later clock/boot problems. Keep historic test state distinct from current checks.

Active RBAC XML package: `/etc/ccli/rbac-dual/package/` with commissioning manifest/policy/scopes, identity-bindings, permission-library. Not edited for latest clock or RTU tests. Aggregator scope is private to Aggregator/operator; DSO is not automatically allowed Aggregator scope.
Scripts `verify_roles.py`, `verify_aggregator.py`, `verify_aggregator_live.py`, `build_allowlist.py` under client tools. Full write/control tests belong only on isolated harness; live read-only mode is guarded.
Evidence `lab/evidence/mms-tls-enrollment-2026-10-19/REPORT.md`, `mms-role-boundaries-2026-10-19/`.

## 11. IEC TS 62351-100-4 audit / Wireshark

User source PDF: sibling `CCLI/Normativa/IEC TS 62351-100-4 2023.pdf`.
SHA256 `93c73072bc18d08020489c40657691db5010dc3d19344729e642def7d96e7fa3`.
115-page viewer export with mostly unusable website text layer; relevant pages rendered and visually inspected. Printed page +2 = PDF page in inspected tables. Do not rely on old planning docs claiming PDF missing.

Report `lab/evidence/iec62351-100-4-2026-10-19/REPORT.md`, `mandatory-gaps.csv`.
Missing native E2E server-scope families: Table15 association/release;17/50 handshake resilience;18 ClearTransfer;20/51 transfer resilience;24/25 both rekey directions; PICS/PIXIT and algorithm evidence. Mandatory within declared OSI E2E scope; do not indiscriminately include client-only/XMPP cases. EncrTransfer families are optional in Table3. TLS+signed ACSE+RBAC are not native SecPDU/ClearTransfer proof.

Wireshark capture `tespro-mms.pcapng`:145 packets, zero reported drops, four TCP streams. Saved decrypted packet summaries and selected association fields. TLS1.2 cipher0x003c allowed local in-memory RSA decryption using existing server.key; **no private key/session secret embedded/exported into evidence**. GUI opened on capture. Frame139 proves revoked fixture rejected with TLS Certificate Revoked. Streams0/1 show Aggregator/Viewer boundary reads; signed DSO stream2 failed then while clock wrong.16 focused checks passed,1 signed DSO failed in that historical run. Signed DSO succeeds now after GNSS fix.

## 12. Suggested continuation

1. Read user’s latest instruction; they operate the simulator themselves. Do not seize COM8.
2. Repeat MMS measurement reads with status cross-check only if a new wiring/simulator change justifies it; avoid endless identical retries.
3. Determine actual simulator variant/registers and why RTU requests/responses fail. Both ends were configured9600N1/unit1; raw capture previously zero bytes. Investigate physical adapter mode/polarity and device TX diagnostics without assuming cause.
4. Fix and test MMS validity propagation so stale Modbus never becomes Good merely because time synchronized.
5. Complete live Aggregator physical proof only once fresh RTU data exists and controlled bench state is verified.
6. Keep GNSS worker installed; no cold-boot experiments until boot issue/backup strategy is considered with user. Do not claim the intermittent BL2 failure is fixed.

No model change was performed by this documentation step. The user intends to switch to Luna 6 and continue this chat.

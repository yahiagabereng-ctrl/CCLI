# CCLI project handoff for a new ChatGPT account

Prepared 20 October 2026 (Europe/Rome), for continuation on 21 October 2026.

This is the context to read before continuing work with Alireza on this repository. It consolidates the accessible project chats, the existing `HANDOFF_LUNA6.md`, current source and lab evidence. It is a project memory document, not a certificate of conformity or a fresh live-device test. Device state below is the latest recorded state; verify it before an action that depends on it.

## 1. Start here

We are developing **CCLI**, the user-space application for an Italian **CCI (Controllore Centrale di Impianto)** / PF2 controller under the CEI 0-16 project. The active platform is a TesPro OpenWrt gateway. This is a substantial existing project with working software, bench tests, security provisioning and unresolved product gaps. Do not restart it from scratch or assume old planning documents describe the current implementation.

The working repository is:

```text
C:\Users\Administrator\Desktop\CCLI Alireza\CCLI Merged
```

The sibling `C:\Users\Administrator\Desktop\CCLI Alireza\CCLI` is the older/original working copy. It holds some local reference documents, lab PKI and separate edits. **Continue development in CCLI Merged.** Do not silently switch to the sibling or import its working tree wholesale.

Read `AGENTS.md` first, then this document. For evidence, prefer a dated receipt/report and matching source/artifact hashes over an older README or a chat's provisional diagnosis. Preserve all existing changes and device configuration.

Most important current facts:

| Area | Latest supported state |
|---|---|
| Device application | Manufacturer-signed **CCLI 0.1.0-r63** APK was installed; package database and running binary were checked. |
| Test Suite Pro | Operator confirmed the actual TSP connection succeeded after the r63 repair. This is a connection result, not full conformance. |
| Audit latency | r62 removed the full-log rewrite performance cliff; that repair is carried into r63. |
| Our MMS client | A newer TSP-style connection profile, verified server authentication and GUI changes exist **locally after the pushed r63 checkpoint**. Preserve these uncommitted files. |
| Modbus measurements | Last checks still showed `read_fail`, stale data and P/Q zero. Simulator compatibility was subsequently edited, but no successful live RTU read was recorded. |
| MMS quality | A confirmed discrepancy remains: MMS can report Good after clock synchronization even while the Modbus measurement is stale. No completed fix is recorded. |
| Time | GNSS startup repair restored chrony synchronization and signed DSO authentication. Latest worker's cold-boot validation remains pending. |
| Boot | Intermittent BL2 decompression failures precede Linux and predate the latest GNSS work. No verified permanent repair or complete flash/TPM backup exists. |
| Security | TLS, signed association, RBAC and signed USB updates have useful evidence. Native 62351-4 E2E, PKI automation and other product/certification gaps remain. |
| SSH | Security and disabling were discussed; **no disabling action was recorded in that chat**. Do not infer it is disabled. |

## 2. User intent and working preferences

- Preserve configuration, certificates, trust, RBAC commissioning, audit history and recoverability. The user previously lost configuration during vendor recovery.
- Carry work through to a concrete result. Avoid repeated permission questions for already authorized routine work. Explain meaningful uncertainty plainly.
- The user runs the inverter simulator themselves. **Do not start/stop it, kill its process or take COM8 unless the current task explicitly calls for it.** Historical capture PIDs and `.pid` files can be stale.
- RS485 was explicitly requested for the simulator bench. Do not substitute Modbus TCP as an unannounced workaround.
- The user asked for natural GNSS synchronization. Do not manually set the clock or force a step merely to make authentication pass.
- Do not reboot, factory reset, erase, flash or begin bootloader recovery as an incidental diagnostic action. The intermittent early-boot fault and incomplete backup make these material actions.
- First fix the Tespro server when diagnosing TSP interoperability. The later request to imitate TSP in our client was also implemented separately; preserve that work.
- For standards gap analysis, the user wants **mandatory requirements**, with the actual applicable reference documents read. Clearly separate optional items and resolve edition conflicts before making compliance claims.
- Distinguish protocol acceptance, software behavior, physical inverter response and accredited certification. A read of zero or a successful Operate response is not physical proof.
- Do not publish, send messages, change account settings or migrate secrets merely because this handoff mentions them. This document does not authorize those actions.

## 3. Repository identity, checkpoints and local work

Snapshot taken during handoff preparation:

```text
Branch: codex/merge-owner-main
HEAD:   fac93a94233e500fe7a6e28c4c9c2b55de717e9c

origin:         https://github.com/alirezavzand/CCLI.git
personal-final: https://github.com/alirezavzand/CCLI-final.git
upstream:       https://github.com/yahiagabereng-ctrl/CCLI.git
```

The latest chat records the release pushed to **alirezavzand/CCLI-final**. The remote called `origin` still points to the earlier personal repository. Always check the intended destination before pushing; remote names alone do not identify the right project.

Important local history:

| Commit | Meaning |
|---|---|
| `b185266` | Personal checkpoint of RBAC, certificate authentication and firmware toolkit work. |
| `ffb0f19` | Owner's r57 / Connect-PASS upstream state used for integration. |
| `76139db` | Tested owner-protocol + Profile A merge, r60. |
| `953e644` | r63 source checkpoint after actual TSP connection success. |
| `31f4b2a` | Handles canonical signing-key paths containing spaces; source revision recorded for the signed r63 APK. |
| `fac93a9` | Records the signed r63 APK installation and successful TSP interoperability. |

The merge combined the owner's protocol fixes with the user's Profile A security. A missing owner dependency revision `d826f94` could not be obtained; with the user's approval, the documented behavior was ported onto tested libIEC61850 baseline **519b0208**. Do not claim this is the unavailable fork itself. Protocol modifications are generated overlays; preserve reviewed vendor source baselines and their integrity checks.

The current source version and package recipe both identify release **63**, codename `TSP-PRESENTATION-CONTEXT`.

At this snapshot the tracked modifications were:

```text
HANDOFF_LUNA6.md
apps/ccli/CMakeLists.txt
apps/ccli/cmake/mms_certificate_auth.cmake
apps/ccli/tools/mms-client/README.md
apps/ccli/tools/mms-client/TSP_INTEROPERABILITY_NOTES.md
apps/ccli/tools/mms-client/build.sh
apps/ccli/tools/mms-client/client.cpp
apps/ccli/tools/mms-client/client.py
apps/ccli/tools/mms-client/gui.local.json.example
apps/ccli/tools/mms-client/gui.py
apps/ccli/tools/mms-client/gui_backend.py
apps/ccli/tools/mms-client/run.ps1
```

Important new source files were untracked:

```text
apps/ccli/tools/mms-client/tsp_profile.py
apps/ccli/tools/mms-client/tests/test_tsp_profile.py
apps/ccli/tools/mms-client/tests/verify_tsp_wire.py
apps/ccli/tools/mms-client/tests/verify_tsp_gui_backend.py
apps/ccli/tools/mms-client/tests/verify_tsp_controls.py
scripts/generate_mms_tsp_client.py
```

There were also untracked dated evidence directories listed in section 14, simulator `state.json`, and a chrony staging directory. A new clone of `fac93a9` will not contain these local changes. The complete relevant working tree must be retained separately if moving machines. Do not `reset --hard`, `clean`, overwrite these files or assume they are disposable build debris.

On this Windows host Git may report dubious ownership. A command-scoped workaround is:

```powershell
git -c safe.directory='C:/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged' status --short
```

The handoff preparation used that per-command setting; it did not change global Git trust.

## 4. Project architecture and where to look

| Path | Purpose |
|---|---|
| `apps/ccli/core/` | Regulation, DSO state, RBAC/security, events and shared application behavior. |
| `apps/ccli/adapters/iec61850_mms/` | MMS model, TLS/association integration, permissions, measurement publication and controls. |
| `apps/ccli/adapters/iec60870_104/` | IEC 60870-5-104 adapter; build/runtime support depends on the profile. |
| `apps/ccli/services/ccli_main.cpp` | Runtime integration and application loop. |
| `apps/ccli/platform/platform_tg500/` | Active TesPro-specific platform. Raspberry Pi work is historical. |
| `apps/ccli/config/` | Lab/product YAML, model files, RBAC and TLS references. |
| `apps/ccli/config/icd/` | ICD/CID/CFG models; use the model matching the binary and test profile. |
| `apps/ccli/tools/mms-client/` | Our CLI/GUI commissioning client and verification scripts. |
| `apps/firmware-updater/` | Canonical APK signing and signed USB update toolkit. |
| `package/ccli/` | OpenWrt package recipe, init and package lifecycle safeguards. |
| `scripts/` | Protocol generation, commissioning, deployment and evidence helpers. |
| `lab/tg544-openwrt/` | SDK build/deploy scripts, GNSS service repair, revision/deployment pins. |
| `lab/conformance/` | Functional/cyber matrices, PICS/MICS/TICS/PIXIT/PID and lab scope. |
| `lab/evidence/` | Dated tests and diagnostic evidence; not every item is tracked. |
| `Architecture/` | Runtime/physical/network/software diagrams and requirements traceability. |
| `knowledge-base/` | Background, freezes, standards extracts and engineering notes. |
| `releases/product-cyber/` | Promoted product bundles after gates; not a separate application source. |
| `handoff/` | Operator handoffs and this migration document. |

Useful architecture entry points are `Architecture/CCI_Runtime_Dataflow_Architecture.md`, `Architecture/CCI_SW_Architecture_vs_Annex_Validation.md`, `Architecture/Software_Architecture.md` and `knowledge-base/08-engineering/CCI_62443_Zones.md`.

Names such as `tr400`, `tg500`, TG544, TR500 and TG-524 occur throughout historical paths. Product planning names TG-524/MT798X, while the **actual lab boot console identifies TR500 / MT7987A / SPI-NOR / 2 GiB**. Use the measured board identity for recovery and firmware compatibility. Some August READMEs still say application components are “NOT STARTED”; that is obsolete.

## 5. Bench, device and access

| Item | Recorded value |
|---|---|
| DUT | TesPro TR500 / TG544 lab unit, Oolite-MT7987A SPI-NOR, 2 GiB RAM. |
| OS | TesproOS `32-2.1.0 r601-af70fcaaa2`; successful capture shows Linux 6.12.68. |
| Address | `192.168.0.1`, root administrative account. Credentials are intentionally omitted. |
| Recorded SSH host key | `SHA256:fy0pZiLnyHHY6hKpcR7ukqBAb22qfNxU72Qpv/hseJM`. Verify identity if it changes. |
| Application command | `/usr/sbin/ccli --config /etc/ccli/lab.yaml`. |
| Secure MMS | `192.168.0.1:3782`, mutual TLS 1.2 in recorded tests. |
| Cleartext MMS | Port 102 is a lab-only alternative; not the product connection profile. |
| Status | `/var/run/ccli-status.json`. |
| Workstation | Ethernet 2, recorded `192.168.0.137/24`; Ethernet 4 is another network. |
| TSP PC | Recorded `192.168.0.115/24`; earlier `192.168.1.174` was a subnet mismatch. |
| Device ID for updater | `CCI-0008CCFD041F`; verify intended target before generating authorization. |

An active-only workstation IPv4 address was added during diagnosis after DHCP fallback to `169.254.94.88`. Check current network state before changing addresses. The TesPro UI showed LAN1/LAN2/LAN3 bridged as `br-lan`; this as-built bench does not establish the intended product's zone isolation.

Tools recorded on this PC:

- PuTTY `plink.exe` and `pscp.exe` under `C:\Program Files\PuTTY`. Use `pscp -scp`; SFTP was unavailable.
- Windows Python `C:\Users\Administrator\AppData\Local\Python\pythoncore-3.14-64\python.exe`; pyserial/pymodbus used for serial work.
- WSL Linux builds and Python with `cryptography` for the native MMS client.
- Wireshark 4.2.14 x64 plus matching `iec61850.4.2.dll` v0.1-alpha, installed for the requested third-party dissector. The DLL hash recorded was `45F7E7043D76D2B52B7B09A51F5376EB93B6B56B9E63AD014F1C019744AC101A`.
- Wireshark tools under `C:\Program Files\Wireshark`; plugin under the user's `AppData\Roaming\Wireshark\plugins\4.2\epan`.

Capture on the **TSP PC** or a mirrored switch/TAP to inspect its unicast traffic. A different PC on an ordinary switch port cannot see all of it. No SPAN control was found in the inspected TesPro UI. A PC capture did show ARP and MQTT traffic; that did not prove or disprove TSP application behavior.

## 6. Latest deployed release: distinguish three stages

The latest APK installation supersedes older binary-only deployment records:

| Stage | State and identifying evidence |
|---|---|
| r61 | Aggregator bench binary; SHA-256 `861ec666a0aced3b9c97ac0d210c2bffe4c14233d7cd5ef8e3ef66ad4fbca4d0`. Historical. |
| r62 | Audit repair installed directly as a binary. Package database then still said r60. Historical. |
| First r63 | Presentation repair installed directly; binary hash `cd1b233b41c8d5360846ec447458fb1fbb599d9eaf8e57c8b162a1c2cdfda2de`, PID 12191 in that receipt. Historical. |
| **Signed r63 APK** | Built from source `31f4b2abee3d0de5d34c3546ca83ebaf169579af`; installed and verified, with package database reporting r63. Latest recorded deployment. |

Latest local package:

```text
apps/firmware-updater/artifacts/ccli-0.1.0-r63.apk
APK SHA-256:
74df622f8b6bf584cefa020b4714a340409bec7e75b5ae239003ed02d04b148a

Installed/running APK binary SHA-256:
f37a783e5936baceed72f8265c08f42e40c4226cab10c7132035f1b5b13cf418
```

PID 26349 was recorded after package installation; PIDs are temporary. Verify the executable behind the process that actually owns port 3782. A LISTEN entry alone previously accepted an old inherited listener and produced a false readiness result.

The r63 APK was installed **over SSH, as explicitly requested then**. It was not a USB installation test. Existing configurations, credentials, trust and audit files passed preservation checks; no reboot was needed, and no pending/hold markers remained. The binary's embedded 2026-06-29 build timestamp is the SDK's reproducible `SOURCE_DATE_EPOCH`, not the actual packaging date.

The tracked final release evidence is `lab/releases/r63-tsp-connect/README.md` and `BUILD_AND_INSTALL.json`. It records TPM manufacturer-signature verification, inspection of all 37 payload files without credential/private-key material, three consecutive running-binary/listener checks, a post-install TSP-shaped signed association in **95.8 ms**, a successful unchanged-client MMS read, and four passing host tests (audit store, rotation, Aggregator arbitration and repeated TLS allowlist configuration). This is more current than the earlier binary-only repair report.

Rollback/history locations recorded on the DUT:

- `/etc/ccli/logfix-r62-backup-20261020`: pre-r62/r61 binary and protected-state backup.
- `/etc/ccli/tsp-r63-backup-20261020`: r62 binary, configuration and settled audit segments.
- `/etc/ccli-upgrade.hMLCKL`: package-hook upgrade backup.
- `/mnt/mmcblk0/ccli-history-20261020/aggregator-backup-20261019`: older backup moved to ext4 SD after verification to free overlay space. Its former `/etc/ccli/aggregator-backup-20261019` location is a symlink.

These are partial application/configuration backups, not a verified entire flash image or TPM private-key backup.

## 7. TSP delay and presentation repairs

### r62: audit-log performance cliff

At 2,048 records, the old event store truncated and rewrote the entire JSONL log for every event. TLS establishment and `client_connect` each synchronously logged before COTP processing. Flash writes on SPI-NOR/JFFS2 produced COTP delays of **4.13–5.18 seconds**; a fresh probe measured 5.51 seconds after a 46 ms TLS handshake. TSP gave up after roughly 1–2 seconds. A live probe sent no ACSE/MMS, so RBAC/transport binding was not the cause of that initial delay.

The repair appends one record and rotates by rename into `events.jsonl.1` at 2,048 lines. Each physical segment is bounded to 2,048; the memory ring and CLI expose the latest 2,048 valid records across them. Persistence stays synchronous. **Do not describe this fix as an asynchronous queue.** It also fixes escaped JSON/Unicode reload and interrupted-write/rotation handling; remote UDP syslog sends became nonblocking.

Native and real aarch64/musl rotation tests passed, including concurrent writers, failures and restart recovery. Five isolated probes and ten live probes passed; live COTP fell to **5.11–12.61 ms**. Audit retention and protected-state preservation were checked. See `lab/evidence/delay-review-2026-10-20/{FINDINGS.md,FIX_VALIDATION.md}`.

Why the fault appeared after earlier success: reaching full capacity creates a performance cliff. Exactly when the log filled was not captured. The old rewrite logic and extra TLS audit predated that day's failure; do not invent a newly introduced timing cause.

### r63: authentication presentation OID

Once delay was fixed, run 3 exposed another blocker before the RBAC callback. TSP uses authentication abstract syntax **`1.0.840.0.1.1.4.1`**; the generated parser/client expected the extra-zero variant **`1.0.840.0.0.1.1.4.1`**.

`scripts/generate_mms_certificate_auth.py` was repaired to accept the canonical OID and exactly that legacy alias. BER lengths derive from the corrected OID length. Unknown authentication syntaxes remain rejected; chain/CRL, transport binding, signature, freshness/replay and authorization were retained.

Seven parser/length regressions passed. A probe preserving TSP's captured framing, contexts, certificate and COTP fragmentation, with only fresh proof time/signature, received an accepted AARE + MMS Initiate in about **92 ms**; the response signature verified. Original stale proof was rejected. The old client still connected/read. The operator then confirmed the **actual TSP connection** succeeded. The signed APK was subsequently installed and checked again.

The earlier `run3/R63_FIX_AND_VALIDATION.md` says no APK had yet been built and actual TSP was pending. That was true at the binary-repair stage and is superseded by the later installation receipt/checkpoint and this section.

## 8. Our MMS commissioning client, including latest local changes

Main path: `apps/ccli/tools/mms-client/`. Entry points include `run.cmd`, `run.ps1`, `run-aggregator.cmd` and `gui.cmd`. Build the Linux backend from the repository root in WSL:

```sh
bash apps/ccli/tools/mms-client/build.sh
```

Read-only launcher example, with a provisioned local identity/profile:

```powershell
& .\apps\ccli\tools\mms-client\run.cmd check
```

If PowerShell's unsigned-script policy blocks the launcher, the historical workaround is process-scoped `powershell.exe -NoProfile -ExecutionPolicy Bypass -File ...\run.ps1 check`, not a global policy change.

Capabilities implemented include browse/read/watch, quality/time decoding, typed writes, direct/SBO operation discovery, enhanced termination, immediate same-session readback, datasets/reports and a persistent interactive association. Menu Exit/Reconnect closes it deliberately. Writes/operates are not automatically replayed after disconnection. `watch` is polling, distinct from a report subscription. Dynamic datasets, file and log services remain restricted by the server's claimed policy; client menu entries do not grant rights.

Local `*.local.json` profiles contain credential paths and are ignored. TLS transport identity and signed ACSE/application identity are separate. A policy identity does not automatically gain TLS allowlist membership. Signed proof requires sufficiently synchronized client/server UTC (recorded freshness window ±120 s). TLS-role identity mode also exists; use the commissioned certificate pairing and policy.

Latest **client-only TSP profile**, added after the server release checkpoint:

- Generated via `scripts/generate_mms_tsp_client.py` and `CCLI_MMS_TSP_CLIENT_PROFILE`. Normal server/package builds leave the client-only flag off.
- Reproduces captured COTP source reference/selectors/TPDU, presentation context order **3/1/5**, ACSE invocation fields and MMS Initiate parameters.
- AP title `1.1.1.999.1`, AE qualifier 12; auth context canonical OID above; MMS Initiate local detail 65,000, outstanding limits 10/10, nesting level 5.
- Verifies returned server certificate chain, signed timestamp and RSA/SHA-256 proof **before operations are allowed**. GUI displays verified identity/fingerprint and profile. Optional `--mms-ca` / GUI MMS server CA can differ from TLS CA; blank defaults to TLS CA.
- Reassembles fragmented signed AARE responses before parsing.
- The actual COTP request matched the capture byte-for-byte. Presentation matched after normalizing only fresh proof time/signature. Microsecond UTC proof timestamps add seven bytes, so current request fragments were **1021/572**, versus captured **1021/565**; response **1021/247** was accepted and verified.
- Nine server-authentication regressions passed. An authenticated relay verified live read/browse/control-info/read on one association. CLI, GUI backend and GUI construction checks passed. DSO write/operate/readback and Viewer denials passed on an isolated synthetic server on 3783, which was stopped afterwards. **No live plant control was sent by those tests.**

This implements the **observed connection profile**, not all TSP services, SCL editing or a formal conformance suite. Further service/report/failure comparisons need actual captures. Detailed behavior is in `apps/ccli/tools/mms-client/TSP_INTEROPERABILITY_NOTES.md`.

## 9. Modbus simulator: unresolved reception and quality

Original simulator, outside this repository:

```text
C:\MY\GIT\cci-software\Inverter Simulator\sun2000_100ktl_m2_rtu_real_register_mock.py
```

The user runs it in VS Code. It was originally a Huawei SUN2000 real-register mock. A repo bridge at `lab/inverter-simulator-bridge/` reuses it and adds the CCI float view. A later 20 October chat also records modifying the original simulator to add that view and a `[CCI READ]` trace. That external edit was not restarted/live-verified in the recorded result and will not come from cloning this repo. Determine which version is actually running.

Recorded physical path: PC **COM8**, RS485 adapter, TesPro **RS485-2**, `/dev/rs485_2_uart` → `/dev/ttyUSB1` (FT232R USB path `1-1.5`). Settings: **9600 baud, 8N1, unit 1**, FC03. COM8 was temporarily used for RS232 boot captures and later reconnected to RS485-2; do not assume its present role without checking.

Current recorded CCI map:

| Raw register offset | Value | Encoding |
|---|---|---|
| 60000–60001 | P in kW | Float32, big-endian/high word first, scale 1. |
| 60002–60003 | Q in kvar | Float32, same order, scale 1. |
| 60004–60005 | Line voltage in V | Float32; CCI config applies scale 0.001. |

The log's `addr=100001 qty=6` is display base 40001 + raw 60000, **not** a 100001 wire offset. Original Huawei P/Q offsets 32080/32082 and integer scaling do not match this profile.

Bridge state was 10 kW, 1 kvar, 400 V. Its optional `--accept-cci-q-writes` accepts a complete FC16 two-register Q write at 60002; P/V stay read-only. Original Huawei whitelisted control writes merely store/log values and do not inherently change simulated P/Q.

The latest diagnostic evidence still reports `ERR=read_fail`, `stale_or_invalid`, RTU down and zero power. A map/encoding mismatch is established for the original unmodified mock, but is **not proven to be the only reception fault**. A prior passive COM8 read captured zero bytes. Adapter mode/polarity, wiring and actual device TX/RX remain investigative possibilities. Calendar-clock alignment does not make FC03 RTU reads succeed.

Seriald also owned RS485-2. A user-approved test disabled only `serialservice.port3.enabled`; it did not help and was restored to **1**. Do not repeat the claim that seriald sharing was the established cause. Backup: `/etc/ccli/rs4852-sharing-backup-20261019/serialservice.before`.

**Separate confirmed software discrepancy:** after GNSS synchronized, MMS measurement q became Good/all-zero even though Modbus remained stale/read-failed; its timestamp continued refreshing. Suspect `MmsAdapter::refresh_time_quality()` and measurement quality assignment in `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp` (old references around lines 1982/2082; locate by symbol). Preserve measurement validity/detail bits independently from clock quality and verify stale/oldData/timestamp semantics. This cause is still a hypothesis; no completed fix is recorded.

A read-only helper exists at `lab/evidence/gnss-startup-fix-2026-10-19/read-modbus-via-mms.py`. It uses a persistent Viewer TLS connection, reads P/Q, q and t, writes a report, and does not send controls or take COM8. Cross-check `/var/run/ccli-status.json`; avoid endless identical retries without a changed bench condition.

## 10. GNSS, cellular and intermittent boot fault

GNSS and LTE are separate functions of the modem. GNSS does not require a SIM. Last SIM check: card READY, network searching, no registered operator, `usb0` down/no IP and no outbound route. Earlier 1NCE LTE/APN `iot.1nce.net` tests had succeeded; that does not prove current cellular service.

GNSS startup repair installed:

- `lab/tg544-openwrt/gnss.init` → `/etc/init.d/gnss`.
- `lab/tg544-openwrt/gnss-start-worker` → `/usr/sbin/gnss-start-worker`.
- Uses procd/asynchronous startup, waits for uptime ≥90 s and a ready broker/character port, bounded ubus requests, explicit powered-on verification, 15 s retry and 60 s health checks. Never opens the modem tty directly.
- Fixes missing `USE_PROCD=1` and the earlier one-shot request made before modem readiness. False-success logging was removed.
- Seven mocked real-shell/nonblocking checks and shell syntax checks passed. Live GNSS acquired six satellites, feeder samples reached chrony and chrony selected `#* GNSS`, stratum 1, Leap Normal. Existing `makestep 1.0 3` policy corrected the offset naturally; no manual clock setting was used. Signed DSO authentication subsequently passed.
- This proves restored operation in that run, **not certified ±100 ms absolute UTC accuracy**, NTS support or a validated cold boot. No reboot tested the latest worker.

Clock files: `/etc/chrony/chrony.conf`, `/usr/sbin/gnss_chrony_sock`, `/var/run/chrony.gnss.sock`. CCI observes time; it does not discipline it. NTP pool fallback was unavailable without WAN/default route/DNS. Original/previous GNSS backups: `/etc/init.d/gnss.before-20261019` and `/etc/ccli/gnss-startup-retry-backup-20261019/gnss.init`.

Intermittent boot captures show:

```text
ERROR: xz: xz_dec_run() failed (err = 7)
ERROR: Failed to decompress image (err=-5)
ERROR: BL2: Failure in post image load handling (-5)
```

This occurs **before Linux**. The 8 October support capture contains the same build and errors, so it predates the 19 October GNSS repair. It is not established that GNSS or the signed application updater caused it. The updater source does not flash bootloader images, and investigated audit entries showed rejected/expired authorizations rather than a matching installation.

A later passive capture reached BL31/U-Boot/Linux, with good image checksums and mounted overlay. Bad environment CRC/defaults, random Ethernet MAC, USB enumeration errors and a TPM self-test warning were observed. NUL serial bytes later are not proof of a kernel panic. User reported booting with LAN disconnected; LAN as a trigger remains unconfirmed.

Vendor UART rescue material was reviewed historically, with inconsistent archive/GUI availability across chats. Recovery package examples are not tests on this unit. Do not reuse old instructions to flash first and hope for preservation. Confirm exact board/image/tool, backup coverage and the current user request before any recovery. COM3 in vendor examples does not require renumbering the user's COM8.

## 11. Control behavior, RBAC and timers

Profile A is enabled in the recorded device setup. It combines commissioned identity/policy data, signed application authentication, TLS identity binding and MMS service/object permissions. Active package path recorded: `/etc/ccli/rbac-dual/package/` with commissioning manifest, policy/scopes, identity bindings and permission library. Aggregator scope is private to Aggregator/operator identities; DSO does not automatically inherit it.

Repeated `mms.tls_allowed_client_cert` keys are supported by the project's custom config parser. This is deliberate project syntax, not a claim that repeated YAML mappings are generally portable. Missing public certificate pins were enrolled while retaining chain/CRL checks. Nineteen commissioned identities included standard roles, DSO, Aggregator, role revisions, multi-role and dual-certificate cases. Historical runs recorded 143 live checks and 342 isolated role checks passing. The original r60 merge recorded five CTest and seven network cases, including replay, bad signature, stale proof, wrong AP and wrong transport pairing. These are scoped results, not complete certification.

Aggregator controls introduced in r61 and inherited by later source:

| Function | MMS objects / semantics |
|---|---|
| Active power | `CCI016_01LD_Plant/WSaDAGC1.Mod`, `WSptPct`; 0–100% export ceiling, cannot relax stricter DSO/autonomous/protection limits. Not signed import/export tracking. |
| Reactive power | `CCI016_01LD_Plant/VArSaDVAR1.Mod`, `VArTgtSptPct`; −100..100%, clamped to plant envelope. |
| Modes/timing | Mode 1 on, 5 off; shared 3 s setpoint gate. |
| Q priority | DSO direct Q > DSO PF > Aggregator Q > none. |

Updates preserve unrelated actors/baseline and recompute status. Physical P remains the existing PF2/export-limit relay path, not continuous inverter P tracking. Dedicated Aggregator datasets/reports, complete CID regeneration and complete Annex O behavior remain absent. Two regulation tests and 34 isolated Aggregator checks passed. Physical live P/Q proof did not complete: one run timed out, another stopped on stale RTU before controls. Do not claim real inverter control is proven.

**Communication-loss timing:** current MMS fallback is **15 s after the stack detects the last MMS client disconnecting**; a new connection resets it. It is global all-client loss, not per-actor expiry. Keeping a Viewer/client association open can therefore matter; do not describe it as a dedicated DSO-loss detector. Detection of a silent failure can add delay. The recorded fallback clears live controls so autonomous settings resume.

The separate `iec104.comms_loss_fallback_s: 60` is a configured Eth_B/operator value, historically `monitor_only`; that adapter path raises/logs an event rather than proving an autonomous control switch. Neither number is a universal legal constant. The standards discussion identified the DSO-agreed operating regulation as the basis for the actual production loss interval; re-read the applicable clause before changing it.

## 12. APK signing and USB updates: binding workflow

The current repository `AGENTS.md` requires **only `apps/firmware-updater`** for signing and USB bundles. First read:

```text
apps/firmware-updater/docs/SIGNING_AND_USB_UPDATES.md
```

Use its `scripts/new-bundle.sh`, local `keys/`, `tools/bin/apk`, `artifacts/` and `bundles/`. Do not default to old Desktop Firmware Update inputs, `.ccli-signing`, `C:\PKI` or a root-level historical helper.

Private-key material is intentionally absent from this handoff. Under the current merged-repo instructions it stays confidential, Git-ignored and out of packages/USB output. The sibling `APK Update` chat records a separate request that staged keys there and changed its guidance, with no commit/push recorded at that point. **That is a conflicting historical sibling state, not permission to copy or publish keys from this checkout.** Check intended source and current explicit instructions before any future Git publication.

Identity/trust distinction:

- Manufacturer key signs the OpenWrt APK; only its public key belongs in device trust.
- Encrypted operator key signs the time-limited, device-specific manifest. Operator certificate/CA/CRL authorize that action; they are not the manufacturer's signing key.
- EJBCA issuance does not prove it retains endpoint private keys, nor that TPM-bound keys are exportable.
- TPM NV recorded for public updater trust: operator CA `0x1500017`, CRL `0x1500018`, manufacturer public key `0x1500019`. Do not overwrite/recommission casually.
- Operator passphrase must be entered **only at the user's visible local prompt**. Never request it in chat, put it in command arguments, log it or store it in the handoff.

Fresh clones omit ignored keys, local APK tool, artifacts and bundles. Bundling an already signed APK requires the APK, matching manufacturer public key and matching operator cert/encrypted key; it does **not** require the manufacturer private key or SDK. Signing a new APK requires the manufacturer private key; building CCLI also requires sources/dependencies/SDK. Provision missing local material through the authorized secure transfer process.

Canonical commands, after reading the guide and verifying target/artifact:

```powershell
# From the merged repository root: build CCLI when needed
wsl bash ./apps/firmware-updater/scripts/build-ccli.sh

# From the toolkit: read-only signature/input preflight
Set-Location .\apps\firmware-updater
wsl bash ./scripts/check-bundle-inputs.sh ./artifacts/ccli-0.1.0-r63.apk

# Operator runs visibly; encrypted-key prompt remains local
wsl bash ./scripts/new-bundle.sh ./artifacts/ccli-0.1.0-r63.apk CCI-0008CCFD041F 60
```

The r63 filename is an existing recorded artifact, not an instruction to downgrade or reinstall it. Use a higher release for a normal upgrade. A same-version authorization does not force reinstallation. Preflight verifies the manufacturer signature before bundling. Only successful bundle creation confirms the encrypted-key signing step succeeded.

Copy **only the complete generated `update-<UUID>/` directory** to the USB root or `updates/`, verify hashes, eject and insert into TesPro. Never copy `keys/` or the whole toolkit. Keep the signed manifest intact, use a fresh transaction and current certificate/CRL/device UTC. Do not reuse a consumed authorization.

Updater version recorded: **firmware-updater 1.1.0-r8**. It verifies identity, device ID, time, replay, hashes and manufacturer signatures; quiesces CCLI; preserves protected state; checks `.pending`/`.hold`; and requires three healthy CCLI/MMS checks before success. Successful package installation consumes a transaction even if later startup fails. `active with no instances` is not healthy. Persistent audit: `/mnt/mmcblk0/firmware-updater/security-audit.log`.

Sixteen updater regressions and a disposable local bundle test passed; operator previously reported a USB recovery test worked. Those do not turn the later SSH r63 installation into a USB result. **Never substitute SSH for a requested USB installation test.** Removing CCLI for a recovery test is a separate destructive setup action that needs the actual user request and backup.

Updating the updater APK itself is separate from updating CCLI. The sibling checkout gained clearer self-update documentation; do not assume all those edits are merged here. A running updater can process its own signed update, but it cannot bootstrap USB self-update before being installed. Verify version bump, canonical artifacts and the exact docs in the chosen checkout.

## 13. Security and standards: known gaps and conflicting claims

This section preserves the engineering discussions; it does not make new legal/certification determinations. No certificate of full CEI/IEC conformity or IEC 62443 certification was established by these chats.

### Keep the security layers distinct

- TLS on 3782 protects the transport.
- Signed ACSE/Profile A proves aspects of association identity, freshness, transport pairing and authorization.
- Native IEC 62351-4 E2E **SecPDU/ClearTransfer**, message integrity/replay/sequence handling, release/abort and bidirectional rekey are a separate unfinished implementation.
- `ClearTransfer` is an E2E term for integrity/authenticity protection without E2E payload encryption; it does not mean disabling the TLS tunnel.
- Our stack uses MMS/ACSE/presentation/session/COTP/TPKT over TCP/TLS. XMPP is not the implemented path.

The 19 October audit of IEC TS 62351-100-4 found missing server-scope families including association/release (Table 15), handshake resilience (17/50), ClearTransfer (18), transfer resilience (20/51), both rekey directions (24/25), and PICS/PIXIT/algorithm evidence. Apply mandatory/optional selections within the declared server OSI scope; do not add client-only/XMPP families indiscriminately. `EncrTransfer` was optional in the inspected Table 3 scope. TLS + signed ACSE alone does not close native E2E.

### Required baseline reconciliation

Chats compared an older Annex T page/`Shitek 2.pdf` with newer controlled CEI material. Their conclusions **conflict in places**:

| Topic | Conflict to resolve from the applicable text |
|---|---|
| TLS 1.3 | The older supplied page was read as requiring it; the later current-edition review treated TLS 1.2 as mandatory and TLS 1.3 as a recommendation. |
| OCSP | Conditional in the older page discussion; later review of current CEI 57-142 identified CRL + OCSP with nonce as required. |
| CRL/renegotiation timing | Earlier discussions used a ≥24-hour refresh and fixed schedule; later current-text review did not treat those exact intervals as mandatory blockers. |
| E2E scope | Older PID/checklist deferred it; newer matrix/audit identifies a product gap. A lab booking deferral does not prove implementation or an accepted product exclusion. |
| Editions and stack | Old PID references older IEC editions and wolfSSL; build/source discussions use Mbed TLS. Freeze the actual edition, binary and linked TLS configuration. |

Do not merge these statements into an invented universal requirement. Read the exact controlled CEI 0-16 / CEI 57-142 and relevant IEC editions, identify clause and shall/should wording, and update the matrix/PID before implementing a closure plan. A checklist with blank boxes or supplier review notes is not a pass record.

Local reference PDFs currently found in the **sibling** `CCLI/Normativa/`:

```text
CEI 57-142-2026-07.pdf
cei-0.16.pdf
IEC 62351-4 2020.pdf
IEC 62351-8-2020.pdf
IEC TS 62351-100-3-2020 - 道客巴巴.pdf
IEC TS 62351-100-4 2023.pdf
```

These ignored/external files may not be in a new clone. Check their title/edition/source, not just filename. The 100-4 PDF hash is `93c73072bc18d08020489c40657691db5010dc3d19344729e642def7d96e7fa3`; it is a 115-page viewer export with a poor text layer. Relevant tables were visually inspected; old notes saying the PDF is missing are outdated. Other chat attachments such as Desktop `Shitek.pdf`, `Shitek 2.pdf`, `EJBCA.pdf` and Downloads `GetImage (1).png` are outside the repo and are not bundled here.

### Transport/cipher findings to retain

Observed wire evidence proves **TLS 1.2 / `TLS_RSA_WITH_AES_128_CBC_SHA256` (0x003c)**. It does not prove every suite listed in source. Current adapter discussions show min/max pinned to TLS 1.2.

Implementation locations:

```text
apps/ccli/adapters/iec61850_mms/mms_adapter.cpp
apps/ccli/third_party/libiec61850/hal/tls/mbedtls3/tls_mbedtls.c
apps/ccli/third_party/libiec61850/third_party/mbedtls/mbedtls-3.6.0/library/
```

Mbed TLS `aes.c` implements AES-CBC; `gcm.c` implements GCM; TLS 1.2 handshake code handles key exchange. The backend lists DHE-RSA/AES-128-GCM, but the inspected checked-in `mbedtls3/mbedtls_config.h` lacks `MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED` and contains `#error "MBEDTLS"`. This is a **build provenance/configuration concern**: inspect the actual successful build's configuration and generated/staged sources before claiming a fresh build or DHE negotiation. It does not invalidate a separately recorded successful binary installation.

Other unresolved transport items discussed: exact ECDHE-ECDSA/AES-128-GCM capability, NULL-SHA256 support disabled by default, resumption intervals/CRL relationship, automatic revocation reload, at least five trusted roots, boundary-size 8192-octet certificate tests and negative validation on one frozen release. A single configured root file does not prove the capability cannot handle more roots, but multi-root capability has not been demonstrated. Do not count a default cipher list as a negotiated result.

### Broader product gaps

- **PKI lifecycle:** device-side EST or SCEP, certificate/key renewal and logging, OCSP/nonce where applicable, validated CRL refresh/cache/reload and complete trust-anchor model. Private plant networking can distribute PKI data; public Internet is not inherently required. MQTT CRL delivery was discussed as a design, not implemented proof; retained messages still need issuer/signature/time/rollback checks.
- **RBAC:** much more has been implemented than older “Viewer/DSO only” summaries say, including Aggregator/roles/dual certs. Still reconcile complete service/object/role/AoR/SCL and audit coverage on the frozen release.
- **Logging:** r62 fixes performance/retention; it does not provide syslog TLS or complete required event coverage. Receiver-side evidence, severity mapping, tamper protection, power/PSU/DG/DI/PG/PI events and update/security coverage remain relevant.
- **Secure boot/local maintenance:** application signing/TPM public trust is not proof of secure boot or bootloader lockout. Passive boot output does not prove an interactive boot prompt is blocked.
- **Time/network:** GNSS/chrony success does not prove NTS or product zone isolation. Historical LuCI reachability from role networks and bridged LANs need architecture/acceptance evidence.
- **Formal evidence:** transport 62351-100-3/P7-07 accredited deliverable remains open; formal 100-4 scope is a separate lab decision. Functional 61850-10, transport, E2E and IEC 62443 are different claims. Resolve the exact required accredited report/certificate wording with the current controlled references and lab.
- **IEC 62443:** zones/conduits, 3-3 mapping, 4-1/4-2 extracts and implementation groundwork exist. SL-T 2 was discussed as a working target, not an awarded capability/certificate. Formal SDLC evidence, component requirements, assessment scope and certification remain open.

EJBCA is the lab issuer for TLS/application/operator identities. The Community-vs-Enterprise discussion was corrected: Community does have basic issuance, some HSM support and audit capabilities; a paid edition is not itself CCI certification. Production PKI acceptance, support/security maintenance and restricted administration need review against the actual deployed version. Public Super Administrator access was described as a lab setup to replace. No CA migration or Enterprise purchase was performed by these discussions.

SSH is encrypted, but recorded root password access/default bench credentials and broad network access are security concerns. No credential values are reproduced. Disabling Dropbear was discussed, not executed. Confirm a working authorized alternative before disabling the only admin path; front RS232 is not a proven login shell. Normal MMS/Modbus operation does not require SSH. Bootloader protection and OS-level SSH access are separate controls.

## 14. Evidence and continuation reading order

Read these first for the newest behavior:

1. `AGENTS.md` and this handoff.
2. `HANDOFF_LUNA6.md` — substantial prior continuity; some early r63/PID/build statements are historical, as explained above.
3. `apps/ccli/tools/mms-client/TSP_INTEROPERABILITY_NOTES.md` and client README — includes the latest local client work.
4. `lab/evidence/delay-review-2026-10-20/FINDINGS.md`, `FIX_VALIDATION.md`, and `run3/{FINDINGS.md,R63_FIX_AND_VALIDATION.md}` — ordered diagnosis/repair stages.
5. `lab/releases/r63-tsp-connect/{README.md,BUILD_AND_INSTALL.json}`, Git checkpoint `fac93a9` and local `apps/firmware-updater/artifacts/` metadata — final signed APK installation state.
6. `lab/evidence/modbus-reception-2026-10-20/device-check.txt` and simulator/quality sections above.
7. `lab/evidence/gnss-startup-fix-2026-10-19/{RETRY-FIX.md,BOOT-INCIDENT.md,BOOT-REREAD.md}`.
8. `lab/conformance/OWNER_BRANCH_MERGE_2026-10-16.md`, role implementation/commissioning documents, then the cyber/functional matrices with their dates understood.

Additional dated evidence directories present locally at this snapshot:

```text
lab/evidence/gnss-startup-fix-2026-10-19/
lab/evidence/iec62351-100-4-2026-10-19/
lab/evidence/inverter-simulator-2026-10-19/
lab/evidence/mms-aggregator-2026-10-19/
lab/evidence/mms-client-2026-10-19/
lab/evidence/mms-client-controls-2026-10-19/
lab/evidence/mms-persistent-session-2026-10-19/
lab/evidence/mms-role-boundaries-2026-10-19/
lab/evidence/mms-tls-enrollment-2026-10-19/
lab/evidence/mms-tls-live-2026-10-19/
lab/evidence/modbus-reception-2026-10-20/
lab/evidence/rs232-2026-10-19/
lab/evidence/tespro-bootloader-support-2026-10-19/
lab/evidence/tsp-client-2026-10-20/
```

`lab/evidence/iec62351-100-4-2026-10-19/REPORT.md` and `mandatory-gaps.csv` record the E2E audit. Its 145-packet capture included Aggregator/Viewer boundaries and revoked-certificate rejection. A signed DSO attempt failed while the clock was wrong; later GNSS repair restored signed DSO success. Do not perpetuate the historical failure as the current result. RSA-based capture decryption used a local key in memory; keys/session secrets were not exported into that evidence.

Conformance entry points: `lab/conformance/LAB_REQUIREMENT_TEST_MATRIX_CYBER.md`, `LAB_UNIFIED_CERT_TEST_MATRIX.md`, `D7_62351-3_PID_DRAFT.md`, `CCI_RBAC_ROLE_IMPLEMENTATION.md`, `CCI_RBAC_TSP_DUAL_CERTIFICATE_INTEGRATION.md`, `PRODUCT_SPECIMEN_5LN.md`, plus `lab/evidence/phase7/` logger matrices. These documents can lag newer code and newer reference editions.

Historical 5-LN and larger model/security profiles are intentionally different. Keep executable, YAML, CFG, CID, TLS/application identity and CRL as one compatible specimen. In the merged implementation `VArSdDVAR1` and `PFSPDFPF1` can be absent in a 5-LN model without aborting startup. Do not mix an old r51 security profile, a different model and arbitrary TLS files.

## 15. Build and migration requirements

CCLI is C/C++ on OpenWrt, not an Android application; **APK here means the OpenWrt package format**. The host commissioning tools also use Python. The active platform is `platform_tg500`; old Raspberry Pi images and studies are not the current DUT target.

Recorded build environment: WSL2/Linux, OpenWrt **25.12.5 mediatek/filogic**, gcc 14.3/musl, `aarch64_cortex-a53`, libIEC61850 + Mbed TLS 3.6 and libmodbus. Firmware-updater uses its documented **25.12.2 SDK**. SDKs should be extracted on WSL's Linux filesystem, not a Windows DrvFS tree with incompatible symlink behavior. Follow canonical wrapper environment overrides (`OPENWRT_SDK_HOME` for CCLI, `OPENWRT_SDK` where documented for updater/tool bootstrap).

Historical WSL build workspace: `/home/adminavz/ccli-tls-allowlist-20261019/{source,host-build,target-build}`. r60 merge validation used `/home/adminavz/ccli-owner-merge-r60-20261016/`. These are workstation paths, not portable dependencies. Some bench builds stubbed IEC104 and live config disabled it; never infer full IEC104 conformance from those builds.

The inherited tree has dependency Gitlinks and no root `.gitmodules` according to merge documentation. This checkout contains required local third-party source; a clean clone may not materialize it automatically. Verify the pinned libIEC61850 519b0208 and required Mbed TLS source/config/build provenance. Do not silently replace them with current upstream versions or bypass source-integrity guards.

For a new account on the **same machine**, retain this merged directory and its local state. For a different machine or uploaded project, distinguish:

| Transfer | Why |
|---|---|
| This handoff + `START_HERE.txt` | Standalone project memory and first message. |
| Intended repository/branch/checkpoint | The actual source baseline, not an old owner's snapshot. |
| Listed uncommitted source changes | Latest TSP client work is beyond `fac93a9`. |
| Selected dated evidence and approved references | Many are untracked/ignored or in sibling/outside folders. Transfer only appropriate material, after inspecting contents. |
| SDK/tool/dependency setup | A repository alone is insufficient to rebuild all artifacts. |
| Separately provisioned credentials | Never attach private keys, P12 files, passwords, full protected device backups or live identity profiles as generic project context. |

This context-only handoff package deliberately contains no repo snapshot, private keys, binaries, device backups, raw chats or raw evidence. It explains what must be preserved; it does not replace the local working tree. Do not upload the entire repo directory blindly: existing history/fixtures/backups can contain sensitive material even where new ignore rules exist.

## 16. Suggested next work, selected by the user's next request

1. Establish current context: inspect `git status`, identify the user's active bench task and use the local source changes. Do not redeploy r63 merely because a handoff was read.
2. If resuming measurement work, confirm the simulator variant/map and RS485 connection without taking COM8. Obtain fresh valid RTU measurements, then compare device status with MMS P/Q/q/t.
3. Investigate and fix the MMS validity/time-quality discrepancy with meaningful stale/healthy/clock-transition regression cases. Do not hide it by forcing time or substituting zero values.
4. Complete live Aggregator physical proof only after healthy RTU data and a verified controlled bench state. Synthetic role/control tests are already available; avoid live write suites as routine connection tests.
5. Preserve the latest client TSP profile and server-authentication changes. Capture additional TSP behavior only when needed for new service coverage.
6. For mandatory cyber closure, first freeze controlled editions/scope and reconcile the matrix/PID; then address transport evidence/PKI, native E2E, complete RBAC/logging and formal lab deliverables without claiming optional work is mandatory.
7. Leave GNSS worker in place and treat intermittent BL2 failure as unresolved. Plan cold-boot/recovery validation with the user and an appropriate preservation strategy.

## 17. Chat coverage and provenance

The handoff preparer inspected the visible app's recent thread inventory (50-entry limit), read relevant recent turns from the project chats below, checked archived inventories, and reconciled those with repository handoff/source/evidence. This is a synthesis of **important accessible project context**, not a verbatim export of every turn ever written. Some chats have older unread pages; attachments and older chats outside the exposed inventory may be missing. Unrelated personal chats were excluded. The supplied title “check of the modbus of tespro is recieving information” had no title in one read response; its inventory title is retained below.

Relevant chats reviewed (titles retained as listed):

| Chat | Context carried forward |
|---|---|
| Merged | Main integration, bench work, r62/r63 fixes, TSP pass, signed APK and repository checkpoint. |
| Assess and disable Tespro SSH | SSH/recovery discussion; Annex O/T, current-vs-old standards, cipher/PKI/logging gaps and timers. |
| Investigate delay review finding | Full 2,048-record audit-log trigger and why it appeared after earlier successes. |
| Diagnose Tespro LAN bridging | PC subnets, capture visibility, bridge/SPAN observations and initial TSP logs. |
| APK Update | Updater signing/self-update docs, r8 checks, and distinct sibling key-staging conflict. |
| Cybersecurity Test | IEC 62351 layers/E2E and IEC 62443 preparation; no certificate. |
| check of the modbus of tespro is recieving information | RTU failure, actual raw map, clock distinction and later external simulator compatibility edit. |
| Read the COM8 RS232 log | BL2 failure captures and serial-port ownership. |
| Check Tespro SIM status | SIM ready but LTE unregistered; GNSS/SIM distinction. |
| RBAC | r60 merge package, Profile A/dual identity and positive/negative tests. |
| Github | Personal/owner history, preservation before merge and specimen consistency. |
| ejbca | CA edition/support assessment, corrected Community capabilities and production administration. |
| Plan PKI lifecycle remediation | CRL cache/refresh, private-network distribution and MQTT proposal. |
| Set up CCI/Tespro connection | Secure MMS 3782 versus plain-client/102 incompatibility. |
| Watch dog | Historical watchdog design/package, not proof it remains installed/current. |
| how to check for MQTT on tespro? | Lab Mosquitto, telemetry/update ideas and USB-host distinction. |
| Check Tespro USB connection | Historical USB/security fixture and cellular diagnostics. |
| Check Tespro internet connection | LAN reachability is distinct from outbound Internet. |
| Check HTTPS on Tespro device | Historical removal of `usb-test` and `tespro-usb-apk-update`; host-key history. |
| Signing and Watchdog | Earlier update removed custom watchdog work while retaining package signing. |
| Connect to device at 192.168.0.1 | Historical vendor packages/services; not the current source of truth for port ownership. |
| Read Tespro COM8 RS232 log | October 8 support capture, UART rescue review and missing-GUI concern. |
| Install Wireshark 4.2 IEC 61850 | Completed Wireshark/plugin installation evidence. |
| MRT Ring Explained | MRP was a network-equipment design discussion, not CCI protocol implementation. |
| Fix Tespro SYS LED | Historical vendor recovery discussion; do not treat it as authorization to flash now. |
| IEC 61850 Annex Connection | Earlier Wireshark/plugin setup guidance, superseded by completed installation. |
| EJBCA Pricing Proposal | Accessible recent content did not provide usable substantive context; no pricing/purchase assumed. |

Other continuity is preserved through `HANDOFF_LUNA6.md`, dated lab evidence and the older conformance/handoff files. The `Watch dog` chat includes standalone watchdog APK experiments; `Signing and Watchdog` later records removal during an upstream update. Do not claim the standalone watchdog is part of the current r63 installation without checking source/package/device state. Historical MQTT development used the workstation broker `192.168.0.137:1883`, plaintext on the lab LAN; keepalive evidence is not a production secure-telemetry claim. MRP discussions concerned compatible industrial switches/gateways around single-homed inverter connections; no MRP support in CCI/TesPro was demonstrated.

Handoff preparation changed documentation only. No device connection, control operation, deployment, signing, bundle generation, Git commit/push or account migration was performed as part of creating this document.

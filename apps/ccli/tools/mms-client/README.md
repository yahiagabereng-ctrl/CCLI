# CCLI MMS commissioning client

## Native E2E development peer

The GUI and C++ commissioning client use compatibility ACSE. For a native-only E2E server, use `e2e_probe.py`; it performs a protected read, application-key renewal, another read and signed release over mutual TLS. It supports HMAC-SHA256 and AES-128/256-GMAC. Credential/policy prerequisites and complete commands are in [the Tespro E2E runbook](../../../../lab/conformance/e2e/TESPRO_LAB_READINESS.md). This project peer supports development testing and is not qualified conformance equipment.

Interactive libIEC61850 console for TLS 1.2 and the CCI Profile A certificate-authenticated MMS service. Runs in Linux/WSL through `run.cmd` on Windows. Requires CMake, a C/C++ compiler, Python 3 and Python `cryptography`. Uses the merged repository's libIEC61850 and certificate/session overlays. Server-side role permissions remain in force.

## Standalone desktop interface

On Windows, double-click `gui.cmd` (or run it from PowerShell) to open the lightweight desktop client. It uses the same persistent libIEC61850 session through WSL, so browsing and repeated reads stay on one TLS/MMS association. Use **Load SCD/CID…** to open a local IEC 61850 SCL file (`.scd`, `.cid`, `.icd`, or XML) as a hierarchy of IEDs, logical devices, logical nodes, data objects and functional-constraint-qualified attributes. Selecting an SCL item fills its MMS reference and FC; use Read or another action to query the live server. Successful reads also fill the Value field. **Browse MMS** builds and expands the live model from the connected server. State-changing writes and controls use a standard Yes/No confirmation dialog.

The Configuration and Certificates tab shows the active endpoint, AP-title/AE qualifier, authentication mode, CA and TLS/application certificate and key paths. After connection it displays each certificate's subject, issuer, role token, validity window, serial and SHA-256 fingerprint. Paths and connection settings are saved to the ignored `gui.local.json`; private-key bytes and the key passphrase are never stored. The key passphrase is cleared from the interface after the session connects.

The desktop shell requires Windows Python with Tkinter and WSL with Python 3, `cryptography`, and the built `ccli-mms-client`. Build the backend in WSL with `build.sh`. Select `client` or `aggregator` to load an existing ignored local profile; edit fields directly to change the session configuration. Press Connect again to replace an active session with the edited settings.

Build in WSL from the repository root:

```sh
bash apps/ccli/tools/mms-client/build.sh
```

On this bench, `run.ps1` loads the existing credential paths from Git-ignored `client.local.json`. From PowerShell at the merged repository root:

```powershell
& .\apps\ccli\tools\mms-client\run.cmd
& .\apps\ccli\tools\mms-client\run.cmd check
& .\apps\ccli\tools\mms-client\run.cmd browse CCI016_01LD_Plant
& .\apps\ccli\tools\mms-client\run.cmd watch CCI016_01LD_Plant/PdCMMXU1.TotW.t --count 10
```

The optional local profile stores paths only, not keys. On another PC, supply the identity options explicitly or create your own profile with `identity_cert` and `identity_key` Linux/WSL paths. Explicit command-line options override the profile. Use `--verbose` for TLS/session diagnostics.

`run.cmd --profile aggregator` (or `run-aggregator.cmd`) loads ignored
`aggregator.local.json`, with `identity_cert`, `identity_key`, `tls_cert`,
`tls_key` and `auth_mode`. This bench profile selects the commissioned
AGGREGATOR_OPERATOR certificate. The control menu includes Aggregator P/Q modes
and targets. Keep the menu open while controlling; use mode 1 for on and 5 for off.

r61 implements `WSaDAGC1.Mod` / `WSptPct` and `VArSaDVAR1.Mod` /
`VArTgtSptPct` in the handbuilt model. P accepts 0..100 percent of Smax as an
export ceiling and cannot relax a stricter DSO/protection cap. Negative P is
rejected because this PF2 path cannot track import. Q accepts -100..100 percent
and is clamped to the plant Q envelope. DSO direct Q and DSO PFSP take priority;
disabling them resumes an enabled Aggregator Q target. Enhanced termination and
status readback confirm the model request; verify plant measurements separately.

`verify_aggregator.py` checks controls, range/timing rejection, core regulation
effects, DSO/Viewer scope separation and communications-loss fallback on the
loopback-only no-I/O harness. `verify_aggregator_live.py --yes` is an explicit
proof for this fixed 10 kW COM8 bench; it requires `CCLI_TESPRO_PASSWORD` in the
environment, restores modes/defaults/mock state, and refuses an unhealthy or
already-controlled plant. It is not a generic production test script.

Use an existing commissioned application identity. Private keys stay at the paths you supply; the client does not install or copy them. TLS transport and ACSE application identities are separate. An encrypted application key prompts locally. The TLS key must be readable by libIEC61850 without a passphrase. Keep this PC's clock synchronized for the signed authentication proof.

For a commissioned role-bearing TLS certificate, use `--auth-mode tls` and supply that same certificate/key with both `--tls-cert`/`--tls-key` and `--identity-cert`/`--identity-key`. The default `--auth-mode certificate` uses a signed ACSE application certificate bound to a separate commissioned TLS transport. Both modes keep mutual TLS and chain validation enabled. A certificate's policy binding does not automatically add it to the server's TLS certificate allowlist.

```sh
python3 apps/ccli/tools/mms-client/client.py check \
  --identity-cert /protected/dual-certificate/dso-mms.pem \
  --identity-key /protected/dual-certificate/dso-mms.key \
  --report lab/evidence/mms-client/check.json
```

Default endpoint: `192.168.0.1:3782`. CA and TLS client credentials default to `apps/ccli/config/tls/`; override with `--ca`, `--tls-cert`, `--tls-key`. Application AP/AE defaults are `1.1.1.999.1` / `12`, matching the current bench commissioning policy.

Commands (append the same identity options to each):

```sh
# Logical devices, then nodes, then objects
python3 apps/ccli/tools/mms-client/client.py browse ...
python3 apps/ccli/tools/mms-client/client.py browse CCI016_01LD_Plant ...
python3 apps/ccli/tools/mms-client/client.py browse CCI016_01LD_Plant/PdCMMXU1 ...
python3 apps/ccli/tools/mms-client/client.py datasets CCI016_01LD_Plant/LLN0 ...
python3 apps/ccli/tools/mms-client/client.py read CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f --fc MX ...
python3 apps/ccli/tools/mms-client/client.py watch CCI016_01LD_Plant/PdCMMXU1.TotW.t --count 10 ...
```

`check` associates, browses logical devices and reads active/reactive power, measurement quality and timestamp. UTC timestamps include the `clockNotSynchronized` bit. A successful read proves access, not that a stale meter value is valid or that the clock meets ±100 ms. Watch is polling, not report subscription. JSON reports include observed PC UTC times and operation errors; exit 0 means all requested operations succeeded, 1 means connection/read failure, 2 means invalid setup.

With no command, the client opens a numbered menu. Browse the model by choosing logical devices, nodes, objects, and attributes; use `b` for back, `r` to read, `w` to write, `o` to operate, and `q` to return to the main menu. The identity screen decodes the role in the actual application certificate. Choosing an identity does not grant permissions: the server still checks object scope and service access.

The menu opens **one persistent TLS/MMS association** and keeps it open while you browse, wait at prompts, read, write and operate. `CONNECTED` appears in the menu header. Return from the browser with `q` without disconnecting. Option `0` exits and disconnects; option `12` explicitly reconnects; changing the endpoint also closes the old association before connecting to the new endpoint. A lost connection is reported and commands are never automatically retried, especially writes/operates. Restart an already running older menu to load this version.

CCI's 15-second no-client fallback remains enabled. Holding the menu open prevents a client-induced disconnect after each action; after Exit, a broken connection, or closing the terminal, the server can still apply its configured fallback. Single-command CLI invocations (`check`, `read`, `operate`, etc.) still close when that command completes: use the menu for a sustained control session.

Operate now reads the status back **before releasing the connection** and prints `readback_match` when the value matches the request: `Mod.stVal` with ST for modes, or `mxVal.f` with MX for analog controls. Accepted command, termination and readback are shown separately. Readback failure is reported without automatically repeating the control.

The menu includes quick reads, watch, typed configuration writes, controls, datasets and reports. Before changes it displays endpoint, operation, object and value, and asks you to type `APPLY`. CLI automation can use `--yes` for the explicitly requested change. Write/operate can affect plant behavior. Operation success is a protocol acknowledgement, not proof of physical actuator movement.

Additional CLI examples (same launcher/profile as above):

```powershell
# Configuration write with readback; the value is int32 and the FC is CF
.\apps\ccli\tools\mms-client\run.cmd write CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct.ctlModel --fc CF --type int32 --value 3
# Read the model/type before operating the control object
.\apps\ccli\tools\mms-client\run.cmd control-info CCI016_01LD_Plant/WlimDWMX1.Mod
.\apps\ccli\tools\mms-client\run.cmd operate CCI016_01LD_Plant/WlimDWMX1.Mod --type int32 --value 1
# Static datasets and reports
.\apps\ccli\tools\mms-client\run.cmd dataset-members CCI016_01LD_Plant/LLN0.DS_R_PdC_Mis4sec
.\apps\ccli\tools\mms-client\run.cmd dataset-read CCI016_01LD_Plant/LLN0.DS_R_PdC_Mis4sec
.\apps\ccli\tools\mms-client\run.cmd reports CCI016_01LD_Plant/LLN0.RP.urcb_PdC_Mis4sec --count 10
```

Value types: `bool`, `int32`, `int64`, `uint32`, `float`, `double`, `string`. Values must match the server attribute/control type. Raw writes are for configuration/setpoints (CF/SP/etc.); use the control service for CO. Operate detects direct/SBO and normal/enhanced control models. Selection, operation and immediate readback use the same association, and enhanced operations wait up to 10 seconds for command termination. A termination timeout means the action may have executed: check server state before retrying. `select-cancel` tests selection/release on an object that actually implements SBO; merely changing `ctlModel` cannot add missing SBO/SBOw attributes.

`reports` temporarily enables an unused RCB and receives change/integrity reports, then disables it. Already enabled blocks and reservations not acquired by this association are left alone. A block reserved by a successful subscription on this same connection can be used again; the server still enforces reservation ownership. `reports-gi` additionally requests general interrogation; the current DSO commissioning policy denies GI, so the normal subscribe command is preferable. Report directory listing can be restricted even when a full RCB reference is readable. No report is a failure result, distinct from a received report containing a zero value. Ctrl+C requests cleanup before returning to the menu.

`dataset-create REFERENCE --members 'LD/LN.item[MX]' ...` and `dataset-delete REFERENCE` are exposed for servers that support and authorize them. Current CCI Profile A explicitly disables dynamic dataset creation/deletion, file service and log service; this client does not change that policy. Static dataset members/values remain readable. No commissioning-policy or certificate-role editing is performed.

This is a commissioning console, not Test Suite Pro's formal conformance suite or graphical SCL editor. Live-device validation used reads only. Configuration writes, enhanced operation/termination, viewer denials, and report delivery/cleanup were tested with an isolated localhost harness with no plant I/O. No firmware installation or server restart is required.

`verify_session.py` is an explicit isolated integration regression requiring the no-I/O host harness, an isolated config and existing credential paths. It verifies one association across browse/read/operate and a 20-second idle, immediate control readback, error/cancellation recovery, and fallback after Exit. The harness now polls its normal communications-loss fallback timer so the reset behavior is actually exercised. Test reports are in `lab/evidence/mms-persistent-session-2026-10-19/` for this bench run.

`verify_roles.py --pki /protected/rbac --output /evidence/roles --server /path/to/mms-session-test-server --config /path/to/isolated.yaml` checks all 19 commissioned identities, revision 0/1, the Viewer+Engineer union, six invalid identities and an unbound transport/application pair through this client. Each isolated server explicitly pins the tested transport certificate and uses the existing signed policy without plant I/O. Tests require real access-denied responses for forbidden reads/writes/controls, verify enhanced control readback and report delivery/cleanup, and distinguish globally disabled dynamic datasets from role denials. Raw MX/ST write attempts deliberately bypass the menu guard to check server enforcement. `--live` instead sends only discovery and reads to port 3782 and reports certificates blocked before association. Full write/control tests refuse live endpoints. Evidence and deployment limitations are recorded in `lab/evidence/mms-role-boundaries-2026-10-19/`.
## TSP connection profile (20 October 2026)

The standard `build.sh` now builds the client with the observed Test Suite Pro
connection profile. Open `gui.cmd` as before; close and reopen an existing GUI window
to use the new backend/executable. The Configuration tab shows the profile,
local TLS/MMS identities, and verified MMS server identity after connection.
MMS server CA can be set separately; blank uses the TLS CA. CLI equivalent:
`--mms-ca PATH`. Existing read, browse, watch, write, operate, dataset and report
commands continue using the persistent connection.

The profile uses TSP's presentation contexts, COTP fragment size, ACSE invocation
fields and MMS negotiation parameters. It reassembles fragmented AARE responses
and validates their server certificate chain/signature/time before sending
operations. Fresh client proof timestamps retain microseconds to support rapid
reconnects. Details and exact observed differences are in
`TSP_INTEROPERABILITY_NOTES.md`.

This is connection/service interoperability, not a replacement for TSP's full
conformance-test catalog. For local authentication regressions, run in WSL:

```sh
python3 apps/ccli/tools/mms-client/tests/test_tsp_profile.py
```


## Native E2E field/limit regression

Run `bash scripts/check_e2e_host.sh` from the repository root in WSL. It builds
and runs the isolated native fixture, including `e2e_native_field_campaign`.
To rerun the campaign after building:

```sh
ctest --test-dir apps/ccli/build-e2e-crypto -R '^e2e_native_field_campaign$' --output-on-failure
```

The fixture exercises exact size boundaries, scoped C++ allocation failures,
field mutations and an independent Python contract on emitted messages. The
large-response callback and failure commands exist only in the test executable.
Results include JSON cases, TX assertions and a 695-row execution ledger in
`build-e2e-crypto/e2e-field-fixtures/run-*`. Disposable credentials in those
folders are private test inputs; copy only public result logs/JSON/CSV to evidence.
See `lab/evidence/e2e-field-campaign-2026-10-23/RESULTS.md` for executed coverage
and remaining per-field variants. The normal commissioning GUI remains separate.

# Tespro Profile A component test results

**Later status:** Profile A was subsequently enabled in the main CCLI service
on port 3782 and basic verification passed. See
`CCI_RBAC_TESPRO_ACTIVATION_2026-10-16.md`. The unchanged-production statements
below describe the earlier isolated test only.

Date: 2026-10-16, Europe/Rome. Target: confirmed Tespro at `192.168.0.1`.

**Result: first on-device component test pass completed successfully.**
Profile A ran in an isolated MMS adapter harness on port **3783**. The normal
CCLI controller continued on port **3782** and was not restarted or reconfigured.
The harness was stopped after testing. This is not a full firmware/TSP
conformance result or production Profile A activation.

## Target, backup and approved clock correction

- Verified installed CCLI: **0.1.0-r58 / TLS-ALLOWLIST-REVOCATION-FIX**.
- Original controller PID: **5019**, preserved through the tests.
- Active configuration: `/etc/ccli/lab.yaml`; UCI selection unchanged.
- Backup: `/root/ccli-before-rbac-test-20261016/config-backup.tar.gz`, in a
  root-protected directory. Includes existing CCLI configuration/TLS and UCI.
- Before changing the clock, the original DSO credential completed TLS/MMS
  association and a measurement read.
- Device initially reported **2026-10-14 18:30:42 UTC**. The user explicitly
  approved synchronization to the PC; set **2026-10-16 08:11:59 UTC**. The
  timezone configuration was not changed. New certificates would otherwise
  have failed as not yet valid. Rechecked the original TLS/MMS read afterward.

Preserved SHA256 values, checked again after cleanup:

| File | SHA256 |
|---|---|
| `/etc/ccli/lab.yaml` | `114149038f25cfd5eff5d1cfe283972caa56547f3acd562eea6f5e3bc4f6c859` |
| `/etc/config/ccli` | `011062951870aee5d25344071f87db1ff38eadc00943d4407ba90eb1945b2c88` |
| `/usr/sbin/ccli` | `12dae230d5d45257de88ae9585dcc15744e53ed1f54f0a6dd0c48c881f580786` |

Every file in the captured existing `/etc/ccli/tls` hash manifest also matched
at the end. The protected TLS/ACSE source-baseline checker passed.

## Test architecture and scope

The normal CCLI main initializes physical outputs at startup. To begin testing
without disturbing the running controller, cross-built a dedicated harness
from the repository's real `MmsAdapter`, RBAC/XACML/security, libIEC61850 and
mbedTLS code using the OpenWrt 25.12.5 AArch64 SDK, GCC 14.3.0.

- Source: `apps/ccli/tests/integration/mms_rbac_device_server.cpp`.
- Build script: `lab/tg544-openwrt/build-rbac-device-test.sh`.
- Optional CMake flag: `CCLI_BUILD_MMS_DEVICE_TEST=ON`, default OFF. The harness
  is not an installed APK target and was not substituted for `/usr/sbin/ccli`.
- Executable: `/tmp/mms-rbac-device-server`, size 9,700,392 bytes.
- Executable SHA256:
  `2af9d7f01648d26ea084634fc92d10eeb73deb179f850ed78cdc8a2f05ed7247`.
- Bounded lifetime: 600 seconds per launch; SIGTERM cleanup performed explicitly.
- The harness never initializes controller services, GPIO or Modbus. Successful
  controls/configuration writes affect only its own in-memory model.
- Same handbuilt MVP model as the active controller, with synthetic measurements.
  No switch to a different CID or claim of real plant measurement/control testing.
- Reused the existing server TLS and MMS/ACSE certificate/key paths. GNSS/chrony
  polling and clock discipline were disabled in the harness.
- Separate combined public CA and CRL PEM bundles retained the existing trust
  and added `CCLI-RBAC-Lab-CA`. Original trust files were not edited.
- Explicit initial pins: DSO, Viewer revision 1, Engineer revision 0 and the
  deliberately revoked certificate. A later isolated configuration pinned the
  no-role negative fixture for a TLS-success/MMS-denial test.

The component harness is not the installed full CCLI executable. Full main-service
activation, actual TSP and physical plant integration remain separate gates.

## Staged files and private-key handling

Root-protected directory: `/etc/ccli/rbac-test-20261016/`.

Contains public client/issuer/owner certificates, signed CRLs, signed positive
policy, isolated test configurations, persistent audit logs and test activation
state. All 34 initial public/config content files passed their transferred SHA256
manifest. No new client, owner or CA private key was transferred to the device.
Only existing server keys were read from their original paths.

The normal positive commissioning candidate was used, not `verification-only`.
The unsigned-tampered and signed sequence-test copies are explicit isolated
test fixtures and must not be used as production configuration.

## Executed checks

| Check | Observed result |
|---|---|
| Valid owner signature and initial sequence 1 | Policy activation succeeded on Tespro |
| New role clients with the existing server certificate | TLS/MMS connections succeeded; both CA roots/CRLs loaded |
| Viewer revision 1 measurement read | Allowed |
| Viewer RW configuration write | Denied |
| Viewer control operation | Denied |
| Viewer write to measurement-only value | Denied |
| Engineer revision 0 read and permitted configuration write | Allowed |
| DSO read and designated RW configuration write | Allowed |
| DSO control | Allowed in isolated memory only; `Mod=5` |
| DSO write to measurement-only value | Denied |
| Static dataset directory/member authorization | Allowed for `DS_R_PdC_Mis4sec`; server audited all members authorized |
| Report read, RptEna enable and delivery | Authorized report received; disabled afterward |
| Pinned revoked RBAC certificate | Rejected during TLS: alert **44 / certificate revoked** |
| Trusted but unpinned legacy client on test endpoint | Rejected during TLS: alert **49 / access denied** |
| CA-valid, explicitly pinned no-role certificate | TLS succeeded; MMS association rejected: `identity certificate not individually authorized` |
| Modified file in copied signed package | Activation rejected: `signed package digest mismatch`; state unchanged |
| Incorrect owner trust | Activation rejected; state unchanged |
| Different signing-capable leaf as owner | Activation rejected: `owner commissioning signature invalid`; state unchanged |
| Valid signed sequence 2 | Activated; persistent test counter advanced to 2 |
| Signed sequence 1 after sequence 2 | Rejected: `commissioning rollback rejected`; state unchanged |
| Different owner-signed bytes at sequence 2 | Rejected: `same sequence cannot activate different content`; state unchanged |
| Identical sequence-2 package after process restart | Accepted; state unchanged; Viewer read still succeeded |
| Structured audit | Authentication success/failure, role metadata, access success/denial, policy failures and reporting events persisted |
| Original service after cleanup | Existing DSO and Viewer TLS/MMS measurement reads passed on port 3782 |
| Original revoked credential after cleanup | Still rejected at TLS on port 3782 |

The negative no-role certificate was **not** owner-bound. The device result proves
that successful TLS and a transport pin alone do not grant MMS access. It does
not isolate missing-extension parsing from missing-binding rejection. Exact
wrong-role/AoR/definition/revision rejection paths were previously verified on
the host; they are not claimed as separately exercised on the device here.

The client probe initially expected uncommissioned clients to associate and
then receive service denial. The adapter correctly rejected the MMS association
instead. The test expectation was corrected after an independent successful TLS
handshake and the server's `ASSOCIATION_AUTH_FAIL` audit confirmed the cause;
no product authorization code was changed.

## Cleanup and retained state

- Test listener 3783 closed and its executable process stopped.
- Original controller PID 5019 still runs the original configuration on 3782.
- Production Profile A remains disabled; its trust, server keys, client pins,
  configuration, model and installed binary retain their saved hashes.
- Approved system-clock synchronization is the intentional system-wide change.
- Test material/audit evidence remains in `/etc/ccli/rbac-test-20261016/`.
- Test activation state is retained on JFFS2-backed `/etc`, mode 0600 in a
  root-only directory. It is now **sequence 2**. Do not delete it or expect the
  original sequence-1 package to activate against that same state.
- Final test state:
  `2 f4f13dacbe36b631d86ca99e3e396f143c417a2881a7239d96d122ffec3da45a`.

This verifies process-restart persistence on persistent storage. It does not
claim a full device reboot/power-loss test; no device reboot was performed.

## Evidence and remaining gates

Protected local copy of device logs/public fixtures:
`lab/ejbca-staging/rbac/device-test-evidence/rbac-test-20261016/`.
Includes `events.jsonl`, `server.log`, activation failures, state, public packages
and `final-baseline-check.log`. No device private-key backup was downloaded.

PC client results/logs: `tmp/tespro-rbac-test-results.json`,
`tmp/tespro-rbac-viewer-r1.log`, `tmp/tespro-rbac-engineer-r0.log`,
`tmp/tespro-rbac-dso.log`, `tmp/tespro-rbac-tls-negative-results.json`,
`tmp/tespro-rbac-uncommissioned-tls.log`,
`tmp/tespro-rbac-uncommissioned-mms.log`,
`tmp/tespro-rbac-after-sequence2-restart.log`, and
`tmp/tespro-rbac-final-legacy-{dso,viewer,revoked}.log`.

Next gates: controlled activation of the normal CCLI service, actual TSP identity
route/interoperability, full role/revision/service matrix on the device, live
session/report revocation/expiry, device reboot persistence, and complete
model-appropriate evidence. Aggregator functions absent from the current model
cannot be positive-tested merely by installing an Aggregator certificate.

See `CCI_RBAC_TESPRO_COMMISSIONING.md` and `CCI_RBAC_COMPLETE_TEST_INPUTS.md`.

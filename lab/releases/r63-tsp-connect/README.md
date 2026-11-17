# CCLI 0.1.0-r63 — Test Suite Pro connection checkpoint

On 20 October 2026 the operator confirmed that the Test Suite Pro connection to
Tespro succeeded after applying r63. This records connection interoperability;
it is not a claim that every read/control test or standards test has passed.

r62 removed multi-second association delays caused by rewriting the full audit
log on each event. r63 corrects the authentication presentation abstract-syntax
OID expected from TSP (`1.0.840.0.1.1.4.1`) and preserves an explicit legacy alias
for the existing commissioning client. Certificate verification, transport
binding, freshness/replay protection and RBAC checks remain enabled.

Before the operator's confirmation, the live device accepted a probe with the
captured TSP context order and 1,021/565-byte COTP fragmentation, using a freshly
signed timestamp. Accepted AARE and MMS Initiate response arrived in 91.6 ms;
the server proof signature verified. The unchanged commissioning client also
authenticated and read successfully. A stale captured proof remained rejected.
Seven parser/encoder regression cases passed.

TSP wire behavior for a later client implementation is documented in
`apps/ccli/tools/mms-client/TSP_INTEROPERABILITY_NOTES.md`. The client executable
was not rebuilt during the server repair.

Build/sign the APK only with `apps/firmware-updater/scripts/build-ccli.sh`, its
local manufacturer keys and verifier. Local release artifacts and metadata go
in `apps/firmware-updater/artifacts/` and remain Git-ignored. Installation must
preserve the existing Tespro configuration, certificates, keys and audit log.
No reboot, factory reset or trust reprovisioning is required for this release.

Real lab private keys remain local and ignored. Removing legacy keys from the
current Git index does not erase them from previously published Git history.

## Signed APK and installation

Source build commit: `31f4b2abee3d0de5d34c3546ca83ebaf169579af`.
Signed artifact: `apps/firmware-updater/artifacts/ccli-0.1.0-r63.apk` (local,
ignored; 1,076,013 bytes). APK SHA-256:
`74df622f8b6bf584cefa020b4714a340409bec7e75b5ae239003ed02d04b148a`.
Payload executable SHA-256:
`f37a783e5936baceed72f8265c08f42e40c4226cab10c7132035f1b5b13cf418`.

The signature verified with the canonical local manufacturer public key. On
Tespro it also verified with the manufacturer key read from TPM NV 0x1500019;
no trust reprovisioning or untrusted installation flag was used. All 37 payload
files were inspected: no credential directories/private-key material, and the
init script is executable with LF endings.

Installed over SSH at the operator's request, without a reboot. APK database
reports `ccli` version `0.1.0-r63`. PID 26349 owned the live port 3782 listener
at verification and matched the payload hash on three consecutive checks.
Package hooks preserved every pre-existing configuration/credential/audit file;
the upgrade hold was released only after checksum verification. Configuration
backup: `/etc/ccli-upgrade.hMLCKL`. No pending/hold marker remained.

Post-install tests: accepted TSP-shaped signed AARE/MMS Initiate response in
95.8 ms, server-proof signature verified, and an unchanged-client MMS read
succeeded. Host audit-store, rotation, aggregator arbitration and repeated TLS
allowlist configuration tests all passed (four tests). Live Modbus health is a
separate check; a successful MMS read alone does not establish it.

The SDK sets an older reproducible SOURCE_DATE_EPOCH in the executable. Actual
build provenance is the source commit and package/executable hashes above.
The APK is retained locally rather than committed, following toolkit policy.

# New TLS code tested on Tespro — 26 October 2026

Result: **all final targeted device checks passed**. The new code was cross-built
for ARM64 and executed in isolated component processes on the real Tespro at
192.168.0.1. It was **not installed over the running CCLI service**.

| Check | Observed result |
|---|---|
| Shared TLS HAL security campaign | 23/23 passed on ARM64 hardware |
| IEC 104 TLS configuration/factory campaign | 23/23 passed on ARM64 hardware |
| Real MMS adapter, commissioned identities | 7/7 dual-certificate cases passed |
| Real IEC 104 adapter | Mutual TLS 1.2, STARTDT, general interrogation with a synthetic 12 kW value, STOPDT and graceful TLS shutdown passed |
| Audit events | MMS negative-case reasons and IEC 104 TLS session events retrieved from the device and verified |
| Existing service preservation | Binary, configuration and active TLS CRL bundle hashes unchanged; original PID 15480 remained running |
| Cleanup | Temporary test listeners on 3783, 3793 and 2405 removed; generated `/tmp/ccli-tls-*` and isolated `tls-*` state directories removed |

The TLS campaigns verified all four required suites, rejected missing/expired/
revoked/wrong-EKU/unpinned peers, rejected weak/expired/mismatched local identities,
rejected invalid timer bounds, rejected stale/future/bad-signature startup CRLs,
enforced CBC encrypt-then-MAC, retained established sessions on CRL refresh failure,
disconnected a peer revoked by an updated CRL, and enforced the renegotiation
completion deadline using a **test-only accelerated monotonic clock on Tespro**.

MMS cases: DSO positive, replay rejection, viewer positive with CONFIG/CONTROL
denied, bad-signature rejection, stale-proof rejection, wrong application reference
rejection, and transport/application certificate mismatch denial. DSO CONFIG/
CONTROL operations exercised only the isolated in-memory component; that harness
has no plant I/O. No control operation was sent to the installed service.

IEC 104 used ECDHE-RSA-AES128-GCM-SHA256. Its interrogation returned the synthetic
measurement and the expected activation confirmation/termination. The client
acknowledged the final in-flight periodic measurement before STOPDT confirmation
and TLS close_notify; the final device log records a graceful close.

## Evidence

- `results.json`: machine-readable summary and binary hashes.
- `hal-campaign.log`, `hal-events.log`: shared backend cases and device events.
- `iec104-campaign.log`, `iec104-events.log`: IEC 104 factory cases and events.
- `adapter-campaign.log`: final real-adapter campaign output.
- `adapters/device-server.log`, `adapters/security-events.jsonl`: device audit evidence.
- `adapters/mms/device-test/`: seven MMS probe results and public wire captures.
- `adapters/iec104-startdt.json`: negotiated version/cipher and monitoring APDUs.
- `adapters/installed-before.txt`, `adapters/installed-after.txt`,
  `final-device-state.txt`: preservation and cleanup evidence.
- `cross-build.log`: SDK build output. SDK: OpenWrt 25.12.5 mediatek/filogic,
  GCC 14.3.0 musl, aarch64_cortex-a53.

Temporary test credentials were freshly generated disposable PKI and removed
after each campaign. Existing device private keys were used in place for the
commissioned-identity checks; they were not exported. The fresh candidate public
CRL bundle was used only by the temporary adapters. Installed trust was unchanged.

## Limits and deployment prerequisites

These checks do not establish that every firmware function or IEC requirement is
complete. The full controller's GPIO/relay/plant behavior, actual long-duration
12h/24h intervals, independent conformance tests and production installation were
not exercised. OCSP stapling/download support and safe session resumption remain
open as described in `lab/conformance/IEC62351_TLS_CLOSURE.md`.

The installed r65 service still uses a TLS CRL bundle containing the RBAC CA CRL
that expired on **23 October 2026**. The temporary new-build tests used the verified
candidate bundle to avoid that stale entry. The candidate's earliest nextUpdate
is **29 October 2026 19:36:10 UTC**. Update the installed trust bundle before
deploying/restarting the new strict build; renew the legacy CRL before that date.

The ARM64 `ccli` binary here is a build artifact, not a manufacturer-signed APK
or USB update bundle. Production packaging must use the canonical
`apps/firmware-updater` workflow.

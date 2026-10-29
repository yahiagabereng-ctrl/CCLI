# CCLI 0.1.0-r58 — TLS allowlist revocation correction

- Date: 2026-10-15; target root@192.168.0.1, confirmed TesproOS 32-2.1.0,
  mediatek/filogic, aarch64_cortex-a53.
- Owner authorized correcting the discovered TLS validation bypass; use the
  established controlled SSH lab installation and temporary untrusted APK path.
- SSH host fingerprint previously confirmed:
  `SHA256:fy0pZiLnyHHY6hKpcR7ukqBAb22qfNxU72Qpv/hseJM`.
- Artifact: `lab/tg544-openwrt/ccli-0.1.0-r58.apk`, 1,071,001 bytes.
- APK SHA256:
  `336ed6229b80922620eea1e247d3eb4192f5fbbfa5069bbb568132b7dad33e2b`.
- Payload binary SHA256:
  `12dae230d5d45257de88ae9585dcc15744e53ed1f54f0a6dd0c48c881f580786`.
- Previous r57 binary SHA256:
  `d7b69c916f2cc79dd0fdfe077e6c68ce1552ddba2d7fc301b04158cb43334593`.
- Root-only rollback archive: `/root/ccli-before-r58/system-backup.tar.gz`.

## Narrow correction

`apps/ccli/cmake/tls_allowlist_validation.cmake` generates corrected TLS verifier
source during both native and SDK builds, leaving vendor sources intact.
An allowlist match no longer clears validation errors. With chain validation
enabled all certificate/CRL errors are retained. Explicit pin-only mode can
waive only the NOT_TRUSTED flag; it retains leaf expiry/revocation errors.
The latter option remains disabled on this device (chain validation is true).

TLS 1.2 version settings, credentials, CA/CRL paths, allowlist membership,
ACSE/session/presentation sources and RBAC activation settings are unchanged.
The source-baseline JSON retains all previous hashes/settings and adds only
the approved validation overlay hash. Normal APK tooling still requires signing;
the separate local lab script makes this session's exception explicit.

## Evidence before installation

- Native `ccli` build and clean Tespro SDK cross-build: PASS.
- CTest fixture + TLS/MMS integration + source baseline: 3/3 PASS, 41.39 s.
- Ten direct TLS cases inside the integration test: valid client accepted;
  revoked, expired, wrong issuer, non-allowlisted client, stale CRL and
  invalid-signature CRL rejected; explicit pin-only valid certificate accepted
  while revoked/expired clients are still rejected.
- Existing positive legacy TLS/MMS, Profile A role/scopes, reporting and
  live RBAC identity revocation tests continue to pass in the same executable.
- Payload inspected: correct version/target, preserved upgrade hooks,
  root-ownership fix and no live TLS credential files/private keys in APK.
- Device r57 identity verified and a fresh configuration/UCI/TLS snapshot saved.

## Device results

- Upgraded r57 to r58 using `apk add --allow-untrusted --no-network`; no trust
  store or dependency changes. Upgrade snapshot `/etc/ccli-upgrade.FKdBaI`.
- Upgrade scripts held automatic startup. All pre-existing configuration, UCI
  and TLS SHA256 checks passed before controlled restart. No rollback required.
- Installed APK and executable identify r58; binary hash matches above.
- One running instance, PID 14258 at verification, serving the existing model
  on `192.168.0.1:3782`. Active YAML hash remains
  `114149038f25cfd5eff5d1cfe283972caa56547f3acd562eea6f5e3bc4f6c859`.
- Existing valid TLS client: TLSv1.2, AES128-SHA256, chain verification OK.
- Existing revoked TLS client: handshake rejected with TLS alert 44,
  `certificate revoked`. The rejection occurs before MMS authentication.
- Existing DSO and Viewer clients: both pass live TLS/MMS association and
  measurement read with the same fingerprint-matched credentials as before.
- Profile A stays disabled. No CA, certificate/key, model, firmware-updater or
  system-clock modification. Version sidecar updated separately after checks.
- Native build, real SDK cross-build and TLS/MMS regression suite all pass.
- Device logs: `/root/ccli-before-r58/install-r58.log`,
  `tls-valid-after.log`, `tls-revoked-after.log`. PC logs:
  `tmp/r58-native-build.log`, `tmp/r58-apk-build.log`, `tmp/r58-tls-tests.log`,
  `tmp/r58-apk-dump.txt`, `tmp/r58-live-mms.log`.

## Complete RBAC testing inputs

See `lab/conformance/CCI_RBAC_COMPLETE_TEST_INPUTS.md`. Real EJBCA role identities,
owner signer/package, actual plant model/allocation, TSP mode and persistent
activation/audit storage are still required. Profile A is kept disabled.

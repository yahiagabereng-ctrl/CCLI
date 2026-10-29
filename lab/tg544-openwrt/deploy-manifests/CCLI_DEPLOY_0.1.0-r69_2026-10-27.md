# CCLI 0.1.0-r69 — Tespro TLS recovery

Deployed over user-authorized SSH on 27 October 2026 to Tespro at
192.168.0.1. The confirmed Ed25519 fingerprint was
`SHA256:7E7+Gd+Jvz8jzjVM99u3VPQrrUQ6a/crr/dKQfrXryg`.

## Artifact and corrections

- Canonical build/signing entry point: `apps/firmware-updater/scripts/build-ccli.sh`.
- Manufacturer-verified artifact: `apps/firmware-updater/artifacts/ccli-0.1.0-r69.apk`.
- APK SHA-256: `1017cb2f36a709adf6ed4ee5a58e0b5506d21496d273a7faed31fee289c078e1`.
- Source base: `5122558dd9fb309de428eb99320e99165b50a7e6` plus uncommitted workspace changes.
- Restore the registered DHE-RSA AES128-GCM suite (`0x009E`) and mandatory
  RSA AES128-CBC SHA256 suite (`0x003C`). Use named Mbed TLS constants; retain
  encrypt-then-MAC enforcement and omit the conditional NULL suite.
- Report individual TLS policy rejection stages instead of one generic alarm.
- Fix the MMS authorization test's common configuration to supply its required
  CRL, so missing-identity tests exercise identity loading rather than failing
  earlier on missing CRL configuration.
- Update the reviewed TLS extension fingerprint and explicitly account for the
  required IEC 104 CRL setting in the source baseline checker.

## Actual startup blocker and CRL replacement

The installed TLS bundle contained two issuing-CA CRLs. Its CCLI RBAC Lab CA
entry expired on 23 October 2026 at 08:59:49 UTC. The separate RBAC identity
file already contained the user's new CRL, but the TLS bundle had not been
updated. The ARM64 diagnostic rejected the old bundle and reached READY with
the replacement; certificate and key replacement was unnecessary.

User-supplied `C:/Users/Administrator/Downloads/CCLIRBACLabCA.crl`:

- SHA-256: `05c51835aa6ce45dd2a9ce260317be63e8f505babc8c1a16530bf6a5f25ea9cf`.
- Issuer: CN=CCLI RBAC Lab CA, O=HiTEKS, C=IT.
- CRL number: 7.
- thisUpdate: 26 October 2026, 09:04:58 UTC.
- nextUpdate: 2 November 2026, 09:04:57 UTC.
- Signature verified on Tespro against its configured TLS CA bundle.

Replaced the expired entry in
`/etc/ccli/rbac-dual/trust/tls-crl-bundle.pem` using a temporary file and rename.
The resulting bundle SHA-256 is
`790a018ac443a026b640bfeb676f51fcead6ed123047e8680e851cf408aa942b`.
`/etc/ccli/rbac-dual/trust/rbac_crl.pem` already matches the supplied file exactly.
The other, still-valid CCLI Lab Root CA CRL remains in the bundle and expires
**29 October 2026 at 19:36:10 UTC**. It will require its own signed renewal.
The prior TLS bundle is retained at `/root/ccli-tls-crl-before-r69.pem`.

## Installation and preservation

- Upgraded r68 to r69; newest on-device pre-upgrade snapshot:
  `/etc/ccli-upgrade.DiAoaP`.
- Checked 228 pre-install file hashes. All non-event-log files, including
  credentials, active YAML, UCI and the refreshed CRL bundle, were preserved.
  The runtime event log changed during shutdown and was excluded from the
  static-file preservation gate.
- Cleared the completed upgrade hold only after preservation checks and
  explicitly started CCLI. No pending or hold marker remains.
- Older FGinal and fnjdeE recovery directories were preserved in an archive
  before moving them to tmpfs to free installation space. Verified local copy:
  `tmp/tespro-recovery-r69/ccli-r69-old-backups.tar.gz`, SHA-256
  `f3c2e2d44cab84d1cdc2e398a935d1736cb525e4e113d79b50ed11a7e800e79c`.
  This ignored recovery directory has a restricted local ACL and must never be
  committed or included in release APKs/bundles. The tmpfs copies are temporary;
  the verified workstation archive preserves them across a device reboot.

## Verification results

- Five focused host CTests passed: fixture generation, MMS TLS authorization,
  MMS TLS policy, IEC 104 TLS policy and TLS source baseline (43.21 seconds).
  Policy tests force all four mandatory suites and exercise negative cases.
- The broad host build encountered unrelated missing includes in GPIO and
  service-supervision tests; the focused TLS targets built and passed. This
  record does not claim that the entire repository test suite passed.
- Installed binary reports `ccli 0.1.0-r69`; process 8663 was running and
  listening on `192.168.0.1:3782` at the final check.
- Startup reports TLS, ACSE authentication, RBAC and CRL enabled.
- OpenSSL valid-client handshake: TLSv1.2,
  ECDHE-RSA-AES128-GCM-SHA256, server verification OK.
- An unlisted legacy revoked fixture was rejected by the individual-certificate
  allowlist; that check alone was not used as revocation evidence.
- The actual allowlisted RBAC `negative-revoked` fixture, serial
  `224C5FAB9930AA8CD11E0CEE5B1767398F3DCC1F`, was rejected with TLS alert 44
  (certificate revoked). The server recorded certificate-verification and
  revocation-session-termination alarms, including event code 1004.
- Main-service read-only MMS campaign passed: associated=1, readable=1,
  client_error=0. Independently verified the AARE server certificate, issuer,
  AP/AE, signature, mechanism, timestamp and session PGI 194.
  Local evidence: `tmp/tespro-recovery-r69/mms-read-test/main-test/`.
- IEC 104 is disabled in the active device configuration; its host TLS policy
  tests passed, but this deployment does not claim a live IEC 104 endpoint test.

OCSP remains outside the agreed scope. These results establish the tested
TLS/MMS behavior and do not constitute complete IEC conformance certification.

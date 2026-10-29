# CCLI 0.1.0-r70 — CRL expiry and listener availability

Installed on the user's Tespro at 192.168.0.1 over authorized SSH on
27 October 2026. Pinned Ed25519 fingerprint:
`SHA256:7E7+Gd+Jvz8jzjVM99u3VPQrrUQ6a/crr/dKQfrXryg`.

## Artifact

- Built and signed through `apps/firmware-updater/scripts/build-ccli.sh`.
- Manufacturer verification passed through the toolkit's local APK verifier.
- APK: `apps/firmware-updater/artifacts/ccli-0.1.0-r70.apk`.
- SHA-256: `cc5a813d01763e00b87408596b0c48648f59668302810a5cc8af7dc99fcd26b6`.
- The uploaded APK hash matched before installation.
- Source is the existing uncommitted workspace based on
  `5122558dd9fb309de428eb99320e99165b50a7e6`.

## Behavior

A correctly signed, authorized CRL whose nextUpdate has passed raises a warning
without preventing MMS or IEC 104 TLS listener startup. Startup ignores only the
specific expired-CRL verification flag; revoked or expired endpoint certificates,
invalid signatures, weak keys, untrusted issuers and other validation failures
still reject startup. Future-dated, malformed or missing-nextUpdate CRLs remain
invalid. Missing CRL configuration remains a startup error.

Established TLS sessions retain known revocations and continue when their CRL
expires, with an explicit warning. Initial peer-certificate verification continues
to enforce applicable issuer-CRL freshness. A fresh authenticated CRL published
at the configured path restores authentication on the next connection without a
listener restart. Session resumption remains disabled and OCSP remains outside
the agreed scope.

This implements the availability distinction in CEI 57-142:2026-07, section 5.1,
page 49, and its TLS_CRL_EXP warning in section 5.6.2.2, page 61. It does not
claim complete CEI/IEC conformance or availability of new authenticated sessions
when the applicable peer revocation data is expired.

## Verification

- All five focused host CTests passed in 68.08 seconds: certificate fixtures,
  MMS TLS authorization, MMS TLS policy, IEC 104 TLS policy and source baseline.
- New regressions cover startup with expired CRLs, strict new-handshake rejection,
  fresh-CRL recovery without restart, active-session continuity across real CRL
  expiry, and rejection of expired CRLs combined with bad signatures, revoked
  local certificates or expired local certificates.
- The echo probe drives I/O policy ticks across the whole-second X.509 expiry
  boundary before checking its asynchronous warning. It does not substitute for
  the actual protocol idle loops.
- ARM64 diagnostic on Tespro, port 3793, with the actual old pre-r69 CRL bundle:
  `EVENT 1 1001 warning: CRL expired; listener available, new TLS handshakes require current revocation data`,
  followed by `READY`. The production service and its fresh CRL were unchanged
  during that isolated check. The probe subsequently exited.
- Host evidence: `output/iec62351-tls/r70/host-tests.log`.
- After upgrading r69 to r70, all saved non-event-log configuration and credential
  hashes passed. Event logs were excluded from the static preservation gate.
- Latest on-device pre-upgrade snapshot: `/etc/ccli-upgrade.lnDbBF`.
- Hold cleared after preservation checks; CCLI explicitly started. Version reports
  r70. Final process 12220 listens on `192.168.0.1:3782`.
- Main-service read-only MMS test passed: associated=1, readable=1, client_error=0.
  Independently verified signed AARE certificate, issuer, AP/AE, signature,
  mechanism, timestamp and session PGI 194.
- Read-only evidence: `tmp/tespro-recovery-r70/mms-read-test/main-test/`.
- Active IEC 104 remains disabled in the device configuration; its host TLS
  policy campaign passed.

## Preserved CRLs and recovery data

The fresh RBAC CRL supplied by the user remains installed in the TLS bundle and
the RBAC identity trust file. CRL number 7 expires 2 November 2026 at 09:04:57 UTC.
The `/etc/ccli/rbac-dual/trust/rbac_crl.pem` SHA-256 remains
`05c51835aa6ce45dd2a9ce260317be63e8f505babc8c1a16530bf6a5f25ea9cf`,
matching `C:/Users/Administrator/Downloads/CCLIRBACLabCA.crl`.

The TLS bundle's separate CCLI Lab Root CA CRL remains valid until
29 October 2026 at 19:36:10 UTC and still requires its own signed renewal.
Both current TLS and ACSE server certificates are issued by this root CA.
Verified public PEM/DER CRL copies and the CA certificate for Test Suite Pro are
under `output/iec62351-tls/r70/tsp-public/`. No private keys are included there.
Test Suite Pro configuration was not changed.

The older `/etc/ccli-upgrade.fodLEe` recovery directory was archived and then
moved into tmpfs to free installation space. A verified, ignored workstation
copy is retained at `tmp/tespro-recovery-r70/ccli-r70-older-backup.tar.gz`, hash
`dad4e1a7e8974ccd8ba390de272f7c2167041c4f170165045fb6cad8baf4e393`.
Its local directory has a restricted ACL. It contains recovery credentials and
must never be committed or included in an APK, public evidence or update bundle.

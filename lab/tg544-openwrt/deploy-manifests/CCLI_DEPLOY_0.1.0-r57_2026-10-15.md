# CCLI 0.1.0-r57 — controlled RBAC-ready APK installation

- Date: 2026-10-15 (operator PC date; device clock differs).
- Target: confirmed TesproOS 32-2.1.0, mediatek/filogic,
  aarch64_cortex-a53, root@192.168.0.1.
- SSH fingerprint explicitly confirmed by owner:
  `SHA256:fy0pZiLnyHHY6hKpcR7ukqBAb22qfNxU72Qpv/hseJM`.
- Authorization: owner requested safeguards, build and SSH APK installation
  using `--allow-untrusted` for this release. No trust-store change requested.
- Artifact: `lab/tg544-openwrt/ccli-0.1.0-r57.apk`, 1,070,956 bytes.
- APK SHA256: `e3f4d175f7007d2957632ff58a2d49b866d31f7b97f513dc22ec40750b3cf3fb`.
- Payload `/usr/sbin/ccli` SHA256:
  `d7b69c916f2cc79dd0fdfe077e6c68ce1552ddba2d7fc301b04158cb43334593`.
- SDK: OpenWrt 25.12.5 mediatek/filogic GCC 14.3.0 musl; clean cross-build.
- Before: APK database r55; running binary identifies as r53
  (`P3-07-AARE-SIGN-RAW-GT`). Runtime rollback uses the actual saved binary.
- Previous binary SHA256:
  `e14fff720c8692181a2394a2290654dd4db9e79066a11efda85c94ecfd0f5feb`.
- Active YAML SHA256:
  `114149038f25cfd5eff5d1cfe283972caa56547f3acd562eea6f5e3bc4f6c859`.
- Root-only rollback archive: `/root/ccli-before-r57/system-backup.tar.gz`.

## Changes and safeguards

- Profile A implementation included; activation remains disabled.
- No TLS credentials or private keys in APK payload.
- Pre-upgrade snapshot protects files formerly owned by the older APK.
- Pending/hold markers inhibit framework-triggered starts until restoration
  and verification complete. Controlled installer explicitly releases the hold.
- Post-upgrade makes executable/init and installed policy templates root-owned.
- Existing config, UCI and TLS fingerprints must match before restart.
- No firmware-updater, CA, certificate, trust-store or system-clock change.
- Existing upstream ACSE mode confirmed in the original device log; no
  missing historical custom bridge is introduced as part of this release.
- IEC 104 is compiled as a stub in this checkout; no IEC 104 listener was
  present in the original device baseline. No new IEC 104 capability claimed.

## Evidence before installation

- Actual package preservation-hook simulation: PASS, including old TLS payload
  removal, byte/mode restoration, private backup and held automatic start.
- Protected TLS/ACSE source baseline: PASS.
- Final APK metadata inspected: canonical upgrade scripts present, correct
  target/version, no `/etc/ccli/tls` payload or private keys.
- Device mutual TLS 1.2: PASS, AES128-SHA256, chain verification OK.
- Read-only live client: mutual TLS, MMS association and
  `CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f` read PASS using fingerprint-matched
  existing client credentials. This is not a Test Suite Pro E2E test.

## Installation and final verification

- Installed via SSH with `apk add --allow-untrusted --no-network`.
- Real pre-upgrade backup retained at `/etc/ccli-upgrade.GpLgOL`.
- Credential/config restoration and held automatic startup: PASS on device.
- First verification encountered missing BusyBox `stat`; installer automatically
  restored the original runtime before starting r57. Package registration
  remained r57. Corrected ownership check to `ls -ln`/`awk`.
- Completed the same registered APK by extracting its verified payload with
  `apk extract --allow-untrusted --no-chown` and restoring only its executable
  and guarded init script. No package removal or dependency changes were used.
- Final package database and running executable both identify `0.1.0-r57`.
- Installed binary hash matches the exact payload hash above.
- All recorded pre-existing `/etc/ccli` files, UCI settings and TLS material
  match their pre-upgrade SHA256 fingerprints. No CA, credential or trust-key
  replacement. Added the r57 version sidecar separately after verification.
- Executable and init script root:root, mode 0755; installed policy template
  directory root-owned. Upgrade hold and pending markers released.
- Exactly one running CCLI process, PID 16110 at verification, listening on
  `192.168.0.1:3782` with the existing `mvp_handbuilt` model/configuration.
- Post-install mutual TLS 1.2: PASS; same AES128-SHA256 suite and verification OK.
- Post-install live MMS association and measurement read: PASS, same probe and
  existing credentials as the baseline. Not a physical TSP E2E/security test.
- Profile A remains off: active YAML does not enable it; default is false and
  authentication logs use the legacy TLS identity path. Existing startup
  `rbac=on` means the legacy certificate-based control gate, not Profile A.
- Device evidence: `/root/ccli-before-r57/install-r57-retry.log`,
  `tls-before.log`, `tls-after.log`, configuration/UCI/TLS fingerprint lists.
- PC evidence: `tmp/r57-final-build.log`, `tmp/rbac-r57-apk-dump.txt`,
  `tmp/r57-mms-after.log`. Sensitive backups stay root-only on the device.

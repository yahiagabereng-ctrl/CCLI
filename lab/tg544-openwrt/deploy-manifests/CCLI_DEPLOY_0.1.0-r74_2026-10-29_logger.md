# TesPro development deployment — 0.1.0-r74

Date: 2026-10-29 (Europe/Rome). Target: 192.168.0.1.
Coordinated with “Check simulator compatibility with CCI”; this chat owns the
deployment and that chat owns the separate inverter investigation.

## Exact artifacts

- Unsigned development APK: `output/logger-fix-2026-10-29/ccli-0.1.0-r74.apk`.
- APK SHA-256: `54f6ea5cbedc93618aa0b7ebda949a3ca64dcd5fd5132898c905be83dd29ef69`.
- Extracted/installed executable SHA-256: `d336618c3e6eddc1c2661b8a2715fdce775c9e9138d1c5684e1ae5be17d81616`.
- Built with existing ARM64 SDK, libmodbus enabled, current shared r73 RTU
  source plus logger fixes; no forced dependency clean/download.
- Installed with `apk --no-network add --upgrade --allow-untrusted`.
- Package registration and runtime both report r74.

## Corrections

- Log export loads a read-only EventStore: no additional monitor, forwarding,
  append, directory creation or permission mutation.
- Wrap/reload test uses isolated producers; both temporary segments are cleaned.
- Auth pipe reads are bounded by remaining queue capacity, removing unsigned
  subtraction underflow.
- Collector gets a parent-death signal and closes inherited descriptors before
  exec; partial input is reset on collector restart.
- Removed only verified orphan logread processes from older builds.

## Preservation

- Recoverable backup: `/mnt/mmcblk0/ccli-r74-logger-20261029T112504Z`.
- Full `/etc/ccli` and `/etc/config/ccli` backup plus before/after file hashes.
- Live configuration and credentials passed hash checks after package restoration.
- Kept inverter `/dev/rs485_2_uart`, unit 1, 9600 8N1, 1000 ms; TCP meter remains
  independent. Serial-service configuration was not changed by this task.
- Archived three historical lab backup directories onto SD, leaving symlinks at
  their original paths to avoid copying obsolete data into limited flash during
  upgrade. Before/after path-based hash checks passed.
- Upgrade preservation pending marker absent; newly created hold cleared only
  after restoration/hash checks and isolated wrap test passed.
- Free overlay after upgrade: approximately 10.8 MiB.

## Verification

- Host `event_store_o14`: PASS (wrap/reload, read-only append rejection,
  producer suppression and output-stream error handling).
- Host `platform_auth_account_events`: PASS after child-cleanup changes.
- Installed ARM64 `--event-wrap-test`: PASS (2048 retained, oldest dropped).
- Log read generated `CYBSECEV_LOG_READ_SUCCESS` while collector count stayed one.
- Controlled stop/start: old collector exited with parent; new collector count
  one and parent PID matched CCI.
- No CCI socket inode was inherited by new logread process.
- MMS listener `192.168.0.1:3782` running after restart; event sequence continued.
- Detailed evidence/scripts: `output/logger-fix-2026-10-29/`.

## Scope

PKI lifecycle logging is deferred by user. Remote Syslog remains disabled;
CEI T.3.3.4.6.1 recommends it and does not mandate an active collector under that
clause. Full CEI/IEEE1686 certification, external tamper anchoring, power-loss
qualification and independent inverter freshness issues are not claimed closed.

An earlier r72 direct-executable hotfix was briefly installed and then superseded
by the coordinated r73 deployment. This manifest records the final r74 APK,
which supersedes both and updates package metadata correctly.

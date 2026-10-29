# Tespro system logger installation — 2026-10-29

Scope: implemented system-audit changes described in
`lab/conformance/ANNEX_T_SYSTEM_LOG_COVERAGE_2026-10-28.md`.

Device: 192.168.0.1. Direct SSH binary installation; no APK signing or USB bundle.
Application version remains 0.1.0-r72; this is a local working-tree build,
not a separately numbered release. ARM64/musl, SDK 25.12.5, real libmodbus,
IEC 61850 and IEC 60870 libraries enabled.

Installed `/usr/sbin/ccli` SHA-256:
`874a435212026b6de8cfb763c799283508de22d5bceb968ff5a12f2453687877`.

Previous SHA-256:
`37e00e14af4297b86a90094eb7fb24c13dbcb7b4b5301186b50572f0402b7305`.

Rollback binary and restricted configuration/log snapshot:
`/mnt/mmcblk0/ccli-before-system-logger-20261029-111936/` on Tespro.
The snapshot stays on the device and includes sensitive configuration material.

Verified after installation:
- CCLI service running, single CCLI process; platform `logread -f` monitor active.
- `/etc/ccli/lab.yaml` hash unchanged; remote syslog remains disabled.
- SSH LOGIN_SUCCESS (IEC 1.1) and ACCESS_LOGOUT (IEC 1.8) in the shared store.
- Actual CLI read produced CYBSECEV_LOG_READ_SUCCESS (IEC 9.1), sequence 10097.
- Persistent updater audit import continues; observed actual UPDATE_DEFERRED / TPM unavailable.
- Existing event history retained and sequence continues above pre-install values.
- MMS listener 192.168.0.1:3782 active. Mutual TLS 1.2 probes with RSA,
  DHE-RSA and ECDHE-ECDSA suites all verified server chain/IP successfully;
  combined offer selected the first RSA suite as before.
- Existing TCP Modbus meter polls continue with live voltage data.

Concurrency: no Modbus RTU source or configuration files were edited by this
installation task. Build directory was `/home/adminavz/.cache/ccli-tls-device-build`;
the other chat uses `/tmp/ccli-inverter-poller-check` and reported no device work.
The installed full-application build includes shared RTU source changes already
present in the checkout. Device configuration was preserved; this task did not
enable or validate the new inverter RTU poller.

Limits: `logread -f` produced no existing backlog in a one-second device check.
Pre-ingestion RAM log recovery remains open. No destructive account/password,
clock-loss, PKI or power-loss acceptance stimuli were performed. Coverage remains
Partial as documented; remote collector interoperability and remaining gap rows
are not closed. A host CTest attempt used an obsolete cache whose test executable
was absent; it did not run. Acceptance evidence here is the live checks above.

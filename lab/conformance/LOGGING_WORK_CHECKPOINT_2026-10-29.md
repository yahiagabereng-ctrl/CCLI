# Logging work checkpoint — 2026-10-29

Workspace: CCLI Merged. Work remains local and uncommitted; preserve all existing changes.
No TesPro install or APK signing was performed.

Implemented:
- Firmware updater persistent audit import, IEC outcome mappings and durable cursor.
- GNSS/chrony state events with acknowledgement only after persistence success.
- Complete program-bridge logger configuration copy.
- Periodic account reconciliation and notification-overflow recovery.
- Removal of unsupported success classifications for version change/TLS file loading.
- Durable log-view success/failure requests imported by the shared EventStore.
- Restricted source ownership/permissions, symlink rejection and process UID parameter.
- Durable normalized SSH/web authentication sidecar with importer replay; raw syslog text is discarded.
- Product CLI `--event-dump` now audits reads through the same sidecar as the legacy CLI.
- Import cursors reject unsafe/symlink cursor files, use restrictive atomic replacement, and compact consumed sidecars after 64 KiB.
- Alternate MMS credentials and configured CRL, allow-list, and RBAC paths are watched for configuration changes.
- Platform auth spool failures retry through a bounded queue; `logread` draining pauses while it is saturated.

Verification: MMS-enabled OpenWrt-stub `ccli_program` built successfully. The
`platform_auth_account_events` and `tls_allowlist_repeated_entries` tests both
passed, including normalized auth replay, peer-field preservation, no raw-log
leakage, symlink cursor rejection, compaction and configured watched-path coverage.

Remaining work:
- Validate `logread -f` backlog replay and monitor saturation behavior on TesPro.
- Remove at-least-once duplicates after a crash between EventStore append and cursor commit.
- Authoritative account/permission operation hooks, failed operations and human attribution.
- PKI provisioning lifecycle hooks (install/remove/export/CRL transfer).
- Log tamper evidence with a trusted anchor outside writable log storage.
- Final TesPro acceptance, service permissions and power-loss scenarios.

Important limits: audit sidecars are at-least-once; a crash between EventStore
append and cursor commit can duplicate a row. Fully consumed read/auth sidecars
compact at 64 KiB. A disk failure before first spooling still depends on the
platform log ring and bounded in-memory retry. Imported historical events receive
current append sequence numbers, not original global occurrence ordering.
Shared root UID is not an individual administrator identity.

No TesPro deployment or installation was performed.

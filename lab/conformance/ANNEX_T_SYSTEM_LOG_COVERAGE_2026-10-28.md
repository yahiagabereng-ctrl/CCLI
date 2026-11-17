# CCI system logging — Annex T.3.3.4.6.2.1

Source checked: `Architecture/_extracted_reg_analysis/cei_0-16_consolidated_allegato_t_it.txt`,
lines 3105 onward (CEI 0-16:2025-12 repository copy). The parent subsection
T.3.3.4.6.2 explicitly says **[informativo]**; “Log di sistema” follows at
line 3140. The list concerns the **CCI device**, including its application,
platform administrative services, time services, PKI operations and updater.
It is not a list confined to the MMS client or TLS connections.

The normative logging requirements and the applicable protocol/security
requirements still apply. An informative catalogue is not an unconditional
requirement to add every listed feature. Conversely, a catalogue definition in
the source code is not evidence that an existing CCI feature emits that event.

## Actual source integration

Device follow-up 2026-10-29: r74 APK installed and verified. The installed
2048-event wrap/reload test now passes. Log export starts no extra producer;
read-audit import was observed with exactly one collector. Controlled CCI
stop/start reaped the collector, and its replacement inherited no CCI sockets.
See `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r74_2026-10-29_logger.md`.
PKI lifecycle events remain deferred by user. Remote Syslog is optional under
the cited CEI clause and remains disabled; no collector transport test is claimed.

| CEI system event family/examples | Current evidence | Status / remaining work |
|---|---|---|
| CCI MMS login/authentication: LOGIN_OK, LOGIN_FAIL_WRONG_CR, COMM_PEER_AUTHENTICATION_FAIL | MMS ACSE/Profile A audit callback enters EventStore; classifier supplies standard IEC IDs and available actor/peer/role fields | Connected for application events; verify complete applicable negative cases and final device |
| TLS session/certificate outcomes: TLS_CONN_OK, TLS_CONN_FAIL_CERT, X509_CERT_EXPIRED/REVOKED | MMS and IEC104 TLS event queues/callbacks enter EventStore and map to IEC62351-3/-9 definitions | Connected for existing TLS validation paths; OCSP excluded by user |
| CCI SSH/HTTPS administrative login/logout and failed-login threshold | OpenWrt `logread -f` bridge classifies Dropbear success/failure/logout and platform-reported attempt thresholds; it also recognizes explicit LuCI/rpcd accepted/failed-login records. Recognized records enter a restricted, fsynced normalized sidecar and are imported by cursor. | Source integrated; host tests cover durable replay, identity/peer fields and omission of raw log text. Verify actual TesPro formats and `logread -f` backlog behavior. Web logout, failed-login lockout and session-limit events are reported only if the platform emits an explicit event; the bridge does not create those controls. At-least-once cursoring can duplicate an event after a crash between append and cursor update. |
| OS account creation/deletion/enable/disable and password changes | OpenWrt watcher compares `/etc/passwd`, `/etc/shadow` and `/etc/group`; it emits account create/delete, credential change, lock/enable and account/group metadata events to the same EventStore | Source connected and host-tested. A short in-memory change fingerprint is used (SHA-256 on the TLS-enabled product build); raw shadow hashes and raw log lines are not retained in the account snapshot/audit. Successful state changes are detected; failed operations depend on explicit platform failure logs. Actor attribution and target-device acceptance remain open |
| Role/permission operations | Profile A access/authentication and policy activation/rejection are audited | Partial: individual administrative role/permission create/delete/add/remove/change operations are not all wired |
| SW_UPDATE_OK / SW_UPDATE_FAIL | OpenWrt platform monitor imports the updater's fsynced `epoch|EVENT|detail` file from `event_log.platform_updater_audit_path`; cursor is stored beside the CCI JSONL log. Update success/failure, verification failure and authorization rejection have explicit IEC 62351-14 mappings. | Source integrated. Configure updater and CCI to the same path and confirm permissions/storage on TesPro. At-least-once cursoring can duplicate a row after a crash; it does not skip the source event. |
| FILE_HASH_CHECK_FAIL / FILE_DS_CHECK_FAIL | TLS/E2E verification failures are audited; updater has independent authorization/verification events | Partial: universal platform file-integrity detection and precise updater-to-IEC event mapping are not demonstrated |
| VIEW_SEC_EV_LIST_OK / VIEW_SEC_EV_LIST_FAIL | Both legacy and product CLI `--event-dump` paths record success/failure in a permission-restricted, fsynced sidecar; the shared EventStore imports IDs 9.1/9.2 and compacts fully imported data after 64 KiB. | Source implemented with advisory locking and symlink-resistant atomic cursor writes. Device permissions, installed-entry-point coverage and power-loss behavior remain for TesPro acceptance. |
| TIME_SYNC_SRC_OK / TIME_SYNC_SRC_FAIL | MMS time-quality polling emits IEC 62351-14 8.1/8.2/8.3 into the shared EventStore on initial result, recovery/loss and synchronized source change; repeated identical states are suppressed. | Source integrated. Exercise GNSS fix/no-fix and chrony lock/loss transitions on TesPro; polling must be enabled. |
| Certificate/trust-anchor install/remove/transfer/read and CRL transfer | Configured MMS (including alternate identities, allow-list and CRL), IEC104, Syslog and RBAC policy/owner-certificate paths are watched for create/replace/remove and emit IEC 62351-14 CONFIG_CHANGE with basename and state | Partial: configured-file changes are observed, but initiating actor and semantic install/export/CRL-transfer/read operations are not known; explicit PKI management hooks remain required. |
| PKI_CERT_EXP_NEAR | Actual certificate expiration is checked during TLS validation | Gap: no complete certificate-expiry warning scheduler/source was found for this event |
| TCP_COMM_LOG_SUBS_FAIL / TCP_COMM_LOG_SRV_FAIL | New RFC5425 sender records COMM_LOG_SUBSCRIBER_FAIL locally on transport failure/overflow without recursive logging | Source implemented and host-tested; real collector configuration/device acceptance outstanding |
| LOG_DATA_HASH_FAIL | Bounded application-append-only JSONL, permissions and rejected user clear; no log hash monitor | No tamper-detection/hash-check feature demonstrated; log immutability/IEEE1686 qualification remains separate |
| AV_VIRUS_FOUND | No malware detection event source demonstrated | Platform feature/supplier applicability pending; no antivirus subsystem is introduced by the logger work |
| NEW_CERT_GEN_OK / PKI_CSR_OK/FAIL | Existing external EJBCA provisioning supplies credentials | Device enrolment/generation is not introduced by this sender implementation; qualify events if a device enrolment service is selected |
| TLS_CONN_FAIL_IKE | No IKE/GDOI protocol feature selected by this CCI logger | Not a requirement to introduce that feature solely to emit its events |
| TLS_CERT_CHECK_DIS_OK | Strict reviewed TLS policy requires certificate validation | Validation-disable operation is not permitted by this profile |
| TCP_COMM_LOG_PUBL_FAIL / UNKNOWN_SYSLOG_EV | CCI is implemented as a log generation entity, not a log collection server | Receiver/publisher-collection events are outside the selected entity role |

## Names and severities

The CEI informative list and the repository's IEC62351-14 CDV 2025 extract use
different names and sometimes severities for equivalent events. For example,
CEI LOGIN_OK corresponds to the IEC catalogue's LOGIN_SUCCESS; SW_UPDATE_OK to
FW_SW_UPDATE_SUCCESS; VIEW_SEC_EV_LIST_OK to CYBSECEV_LOG_READ_SUCCESS. Do not
invent IEC numeric IDs or silently override the selected IEC/TLS profile's
severity rules to make an older mnemonic list look identical.

The new sender implements the required IEC fields/format/security for connected
events. Platform authentication/account, updater and time-quality sources now
feed the same EventStore, but target-device log-format checks, failed
account-operation logging, actor attribution and the remaining partial/gap rows
remain open. **This does not close the whole CCI system-event coverage
requirement.**
The system-wide status remains **Partial** until the relevant platform/updater/
time/provisioning sources above are integrated and exercised on the final CCI.

## Code evidence

- `apps/ccli/services/ccli_main.cpp`: CCI EventStore, boot/update observations,
  MMS audit registration and IEC104 audit polling.
- `apps/ccli/core/event/iec62351_event.cpp`: standard field preparation,
  known-stimulus mapping and RFC5424 encoding.
- `apps/ccli/adapters/security/syslog_sender.cpp`: RFC5425 TLS sender and
  collector authorization.
- `apps/firmware-updater/src/firmware_updater.cpp`: `audit()` and
  `security_event()` use an independent persistent audit path.
- `apps/ccli/core/event/platform_event_monitor.cpp`: imports updater audit rows
  with a durable cursor and classifies supported updater outcomes.
- `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp`: `refresh_time_quality()`
  updates time quality and emits transition records through the registered
  EventStore callback.
- `apps/ccli/core/event/event_store.cpp`: JSONL readout and persistence.

## Follow-up reliability corrections

The program bridge now copies the complete EventLogConfig, including updater path,
collector identity and TLS settings. Time-sync transitions are acknowledged only
when EventStore append succeeds and are retried on a later poll after failure.
Account snapshots are reconciled periodically and after inotify overflow; their
baseline advances only after event persistence succeeds. Oversized updater rows
are scanned through their delimiter instead of blocking subsequent audit rows.
Version-change observations and loading existing TLS material no longer claim
successful firmware update or certificate installation.

Remaining platform integration: authoritative administrative operation hooks
(including failed operations and actor attribution), validation that OpenWrt
`logread` replays its in-memory backlog after a CCI restart, duplicate handling
across a crash between EventStore append and sidecar cursor commit, PKI lifecycle
auditing and privileged log tamper protection. These remain open and are not
closed by the reliability corrections above.


## Durable log-view audit integration

`--event-dump` records success/failure in a restricted, fsynced `.read-audit`
sidecar. The OpenWrt monitor imports completed requests through the shared
EventStore with a persistent cursor and IEC 62351-14 IDs 9.1/9.2. This avoids
opening a second EventStore writer and preserves requests across CCI downtime.
The recorded UID is the local process UID; shared root sessions still need
authenticated platform hooks for individual human attribution. Import is
at-least-once. Source files must be owned by the service UID, not writable by
group/others, and may not be symlinks.

Recognized SSH/web authentication records now enter a separate fsynced
`.platform-auth-audit` sidecar. It stores normalized event ID, service, account
and peer fields only; raw syslog text is discarded. Its importer uses bounded
compaction and symlink-resistant atomic cursor handling. This prevents loss
after the monitor has accepted a log line, but does not make OpenWrt's
pre-ingestion RAM log ring durable.

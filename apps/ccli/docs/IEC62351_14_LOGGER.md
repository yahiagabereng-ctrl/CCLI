# IEC 62351-14 field-device logger

Implemented scope: CCI as a **log generation entity**, using the mandatory
Syslog binding. Requirement source is the repository's
`knowledge-base/08-engineering/CCI_62351-14_Extract.md`, revision 1.6, marked
**CDV 2025**. An approved final-edition comparison and final-device acceptance
remain necessary; this engineering implementation is not a certification claim.

## Required behavior

| Extract requirement | Implementation |
|---|---|
| Tables 1/2, event definition and record | Catalogue ID, mnemonic, severity and text; LogType `IEC62351-14`, IECVersion `1`; UTC timestamp, sequence, device source and application name |
| Sequence | One serialized uint32 sequence per EventStore; starts at 1, wraps from 4294967295 to 1; recovered from the latest durable record after restart |
| Conditional identity fields | PeerInfo, UsrID and Role when supplied by the existing MMS/RBAC audit; first asserted role triplet is recorded; full identity/assertion stays in the original local detail |
| Conditional parameters | Unique `P.*` name/value pairs; quoted SD values escape `"`, `\` and `]`; invalid UTF-8, control characters, invalid headers and duplicate names are rejected |
| Section 6.1 | RFC 5424 VERSION=1, UTC timestamp, HOSTNAME, APP-NAME, PROCID=NIL, MSGID=`IEC62351-14:1` |
| Section 6.2 | SD-ID `62351-14@41912`; ID, MNEMONIC, Text and SqNum; applicable conditional fields |
| Section 6.4 | Facility 13; alarm=105, error=107, warning=108, notice=109 |
| Section 6.5 | TCP/TLS only; default port 6514; RFC 5425 octet counting; no UDP or plaintext fallback |
| TLS profile | Existing reviewed IEC 62351-3 TLS 1.2 backend: mutual certificates, chain/EKU/size/cryptographic checks, current CRL at handshake, revocation refresh and configured session timers |
| Receiver authorization | SHA-256 fingerprint of the collector's DER end-entity certificate, checked before application data and again after peer renegotiation; chain validation is additionally required |
| Annex A 9.5 | A latched local `COMM_LOG_SUBSCRIBER_FAIL` alarm on transport failure or bounded queue overflow, without recursive forwarding |

The catalogue contains standardized definitions for generic events and relevant
TLS, certificate, access-token and MMS event families. Its summaries are
human-readable event descriptions. It does not implement the underlying
optional protocol features merely by listing their event definitions.

Existing CCI TLS events, MMS association authentication/denial, policy
activation, firmware-update/reboot and communication state stimuli are
classified into the catalogue. Explicit producers can set `EventRecord.iec_id`
or use a catalogue mnemonic as their event type. Unknown IEC IDs are rejected.
Unrelated O.14 measurement/control records remain local; they are not assigned
invented IEC event IDs.

When running on OpenWrt, the CCI EventStore starts a bounded platform-event
monitor if `event_log.platform_audit_enabled` is true (default). It recognizes
explicit Dropbear SSH and LuCI/rpcd authentication
records and watches `/etc/passwd`, `/etc/shadow` and `/etc/group` for account
creation/deletion, password changes, account lock/enable changes and account
or group-membership metadata changes. It emits only classified fields into the
same EventStore and sequence; it never stores raw platform log lines or
password hashes in its account snapshot or audit records. It keeps only an
in-memory change fingerprint (SHA-256 in the TLS-enabled product build) to
detect credential changes. Failed account/password operations are recorded
only when the platform service emits an explicit failure record.
The platform log formats, target permissions and event timing still require
acceptance testing on the final CCI image; this is not proof that every admin
action is logged or attributed to the human initiating it.

The same monitor tails the firmware updater's persistent `epoch|EVENT|detail`
audit file (`event_log.platform_updater_audit_path`, defaulting to
`/mnt/mmcblk0/firmware-updater/security-audit.log`). A cursor beside the CCI
JSONL store is fsynced and atomically replaced after each complete imported
line, so records written while CCI is stopped are collected after restart.
This is at-least-once across a crash between EventStore append and cursor
replacement; a repeated final record is possible, while the source audit is not
skipped. Update success/failure, verification failure and authorization
rejection map to their IEC 62351-14 catalogue IDs. Other updater lifecycle
records remain local O.14 entries where no applicable standard event mapping
has been established. Configure the updater and CCI to use the same audit path.

MMS time-quality polling now sends structured EventStore entries on initial
source failure, synchronization recovery/loss and a synchronized source switch
(IEC 62351-14 IDs 8.1–8.3). Repeated polls in the same state do not generate
duplicates. Each record includes source, offset, GNSS-fix and synchronized
state. This depends on the MMS time-quality poll being enabled; target chrony/
GNSS event behavior still requires acceptance testing.

The device-wide Annex T system-event assessment is in
`lab/conformance/ANNEX_T_SYSTEM_LOG_COVERAGE_2026-10-28.md`. It tracks which
platform/admin events are now bridged and which log-read and provisioning
feeds still need integration or target acceptance.

Optional XML binding, collector functions, timeQuality, SNMP, remote OCSP and
new protocol features are outside this implementation.

## Configure a collector

Merge `config/examples/syslog_tls.yaml` into the selected device profile. Supply
a numeric IPv4 collector address and its authorized certificate fingerprint,
plus the provisioned CCI TLS client identity, CA chain and issuer CRL bundle.
The client certificate needs clientAuth EKU; the collector needs serverAuth EKU.
Keep private keys outside Git and restrict filesystem access. Existing EJBCA
provisioning supplies the certificate/key pairs.

Display the local sender or collector fingerprint through the certificate
management tooling:

```sh
openssl x509 -in certificate.pem -noout -sha256 -fingerprint
```

`syslog_tls_peer_sha256` accepts `SHA-256:AA:BB:...`, colon-separated SHA-256
hex, or 64 hexadecimal digits. Obtain the collector fingerprint from a trusted
provisioning channel. An enabled sender with incomplete or invalid settings
fails configuration rather than selecting a weaker transport.

Legacy profiles that enabled UDP loopback forwarding now leave remote logging
disabled until TLS credentials are commissioned. Tespro's 192.168.0.1 profile
already has remote forwarding disabled. This source change does not deploy or
enable a collector on Tespro.

## Persistence and transport limits

The latest 2048 records remain in the existing FIFO and rotating JSONL files.
IEC fields and parameters survive reload and are included in JSON readout;
legacy JSONL records remain readable. New records are fsynced before append
success. Files are application-append-only with mode 0640; this is not hardware
write-once storage or protection from privileged filesystem modification.

The TLS worker isolates network waits from append/control threads. Its queue
is bounded by 2048 messages and 2 MiB, with an 8192-byte maximum message. When
full, new forwarding attempts are dropped with a local alarm; local event
retention continues. Connect, handshake, write and TLS close have bounded waits.
Reconnect retries retain pending messages during the process lifetime.
Shutdown or process failure can lose pending remote deliveries. RFC 5425 has no
application acknowledgement, so reconnects after partial delivery may duplicate
records; collectors can correlate Source/App-Name/SqNum. `syslog_ok()` latches
false after a forwarding fault until the EventStore is restarted. The sender
is not a durable SIEM delivery service.

The filesystem and its power-loss behavior, clock accuracy (including the
one-second requirement), collector interoperability and all relevant platform
stimuli still require final-device acceptance. A device whose audit storage is
disabled cannot promise sequence continuation across restart.

## Verification

`iec62351_logger` checks mandatory fields, severity, escaping, conditional
identities, invalid inputs, persistence, restart, sequence rollover, concurrent
ordering and configuration. Existing O.14 and rotation tests cover retention.

`iec62351_syslog_tls` uses ephemeral local certificates and an actual Python
TLS collector. It checks mutual TLS, exact octet framing, collector pin and CA
rejection, expired CRLs, revoked collectors, bounded stalled handshakes and the
local subscriber-failure alarm. No production credentials or device are used.

Normative transport reference: https://www.rfc-editor.org/rfc/rfc5425.html

## Recorded host verification — 2026-10-28

The complete CCI application and TLS/logger test executables built in WSL
Ubuntu, Debug, using the pinned Mbed TLS provider. CTest passed **7/7** selected
tests in **83.93 seconds**: `iec62351_syslog_tls`,
`tls_allowlist_repeated_entries`, `event_store_o14`, `iec62351_logger`,
`event_store_rotation`, `iec62351_tls_policy`,
`iec62351_iec104_tls_policy`. The Syslog TLS test includes six live collector
scenarios. The TLS echo fixture's obsolete six-hour resumption request was
updated to one hour to match the existing strict two-hour policy maximum.

These are host results. No APK was built/signed, no firmware was installed and
no Tespro or production collector was used in this work.

The platform-auth/account bridge was subsequently built with the complete CCLI
executable in the host OpenWrt-stub configuration. `platform_auth_account_events`,
`iec62351_logger` and `event_store_o14` passed (3/3). The platform test covers
Dropbear outcomes, explicit LuCI/rpcd login messages, threshold alerts, account
create/delete, password/lock changes, group metadata and input redaction. It
does not replace acceptance checks against the exact Tespro image and its
installed logging packages.

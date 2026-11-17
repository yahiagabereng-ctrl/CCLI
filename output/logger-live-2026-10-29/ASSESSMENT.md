# TesPro logger live assessment — 2026-10-29

Result: operational, CEI compliance PARTIAL; full conformity is not established.

Device: 192.168.0.1, CCLI r72 built 2026-10-29T10:17:41Z, process /usr/sbin/ccli --config /etc/ccli/lab.yaml.
Evidence: events.jsonl saved alongside this report. 3346 JSON records parsed, zero non-increasing sequence numbers. Previous segment contains 2048 records. Current segment was growing during inspection.

Confirmed live: SSH login/logout records, UTC event timestamps, sequence numbering, security-log read success IEC62351-14:9.1 following an actual --event-dump, time-source success/loss (8.1/8.3), TLS handshake success, updater import. Auth spool cursor reached its source byte size. Directory mode 0700, event files 0640, audit/cursor files 0600. Storage is persistent JFFS2 overlay with approximately 7.5 MiB free (83% used). No event_store/platform_event_monitor error messages were returned by logread queries.

Findings:
1. --event-wrap-test returns FAIL on the installed build. Source run_wrap_test constructs EventStore with platform_audit_enabled default true; OpenWrt therefore starts live producers in the synthetic test. This is a test-isolation defect; rotation itself is observed but the required controlled retention test has not passed.
2. dump_to_stream likewise constructs EventStore with the platform monitor enabled by default. A read-only CLI operation can start an additional producer and writer. This needs correction before accepting concurrency behavior.
3. syslog_enabled is false. Remote RFC5424/RFC5425 transport, collector identity, delivery failure alarms and recovery were not verified. CEI recommends Syslog; disabled transport alone must not be described as automatic failure of every mandatory requirement.
4. A stale doubly suffixed auth sidecar exists (events.jsonl.platform-auth-audit.platform-auth-audit), dated before the current service. Current main auth spool/cursor were advancing; no evidence it is the active source.
5. Individual administrator attribution remains root UID/account only. Failed account/permission operations and semantic PKI lifecycle operations need authoritative platform hooks. Certificate-near-expiry warning and tamper detection with an external trusted anchor remain source-level gaps.
6. Reboot/power-loss durability, CCI-down backlog replay, spool saturation/compaction under load, negative authentication, web events, account lifecycle and firmware-update outcome paths were not exercised. No production account/certificate files were changed and CCI was not restarted during this inspection.
7. Frequent meter communication failure/recovery records and updater TPM-unavailable records consume retention capacity; the count requirement alone does not ensure a long security-history duration.

CEI references: repository extracted Annex T T.3.3.4.6.1 (storage/transmission, at least 2048 events, protected sequential circular history); T.3.3.4.6.2 is informative, including system-event examples in T.3.3.4.6.2.1. IEEE1686 qualification and full IEC62351-14 conformance require further evidence beyond this live check.

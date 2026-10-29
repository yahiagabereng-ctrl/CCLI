# Tespro TLS enrollment — 19 October 2026

**Completed:** all 19 commissioned identities now connect to `192.168.0.1:3782`.
Added the 15 previously missing public TLS certificates. The updated service passed
**143 live read/discovery/authentication checks** and **298 isolated role-boundary
checks**, with no unresolved failures.

The nine roles, both standard-role revisions, the combined Viewer+Engineer identity
and both dual-certificate identities connected successfully. Reads, dataset access,
report-block access and private-role object scope remained restricted by the signed
policy. Six invalid identities remained rejected. A valid application certificate
paired with the wrong TLS certificate could not obtain application access. Live
verification sent no configuration writes, control operations or report enables.

The previous fixed certificate slots could not enroll every peer simultaneously.
Merged source now supports repeatable `mms.tls_allowed_client_cert` entries alongside
those slots. Each public leaf certificate is explicitly pinned; malformed/missing
files prevent MMS startup. Chain validation, TLS 1.2, CRL checks and commissioned
identity/transport binding remain active. The five signed policy files are unchanged.

Built the merged sources with the existing Tespro SDK, staged the ARM64 binary and
checked its runtime dependencies and hashes on the device. Backed up the original
binary/configuration, installed the pin entries and restarted CCI. The final service
PID was 21119 and it owns the MMS listener. After restart, RS485 measurements remained
10 kW / 1 kvar with good quality and DO OFF.

The native parser test passed. The isolated matrix used the new host server with
all added pins configured together, including successful operations/readback for
authorized roles and server rejection of unauthorized configuration/control and
raw measurement/status writes. A fixture initially omitted the existing Engineer r0
pin; its corrected 15-check rerun is included in the final combined evidence.

Only public certificates were uploaded. Client private keys remain on the PC.
Tespro's existing server key was left at its configured PEM path; TPM migration was
not part of this enrollment. TPM-backed signing would require separate support in
the server's TLS/key interface.

Aggregator scoped access is now usable. Its control operation remains unimplemented
in the current model, as identified in the earlier role review.

Evidence:

- [Enrollment summary and hashes](summary.json)
- [All 19 identities and 143 live checks](live/results.json)
- [298 final isolated checks](isolated-final.json)
- [Public certificate manifest](enrollment-manifest.json)
- [Deployment receipt](deployment-receipt.txt)
- [Final listener, policy hashes and RS485 health](post-check.txt)
- [Configuration before enrollment](lab.before.yaml) and [enrolled configuration](lab.enrolled.yaml)

Recovery copies remain on Tespro at `/etc/ccli/tls-enrollment-backup-20261019/ccli`
and `/etc/ccli/tls-enrollment-backup-20261019/lab.yaml`. The original and updated
versions share the r60 version label; use the recorded SHA-256 hashes to distinguish
the deployed allowlist build. Existing MMS menus must reconnect after the restart.

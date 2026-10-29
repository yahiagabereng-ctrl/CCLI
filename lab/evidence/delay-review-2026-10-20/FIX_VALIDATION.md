# r62 audit-log delay fix — applied and validated

Date: 2026-10-20. Endpoint: 192.168.0.1:3782.

The live CCI service is running `0.1.0-r62 (AUDIT-LOG-ROTATION)`.
Its binary SHA-256 is
`8a7963740d88f79e26adf162075300f2afe08ff6f5a7b4619c1111713c22449c`.

## Change

`EventStore` appends a single JSONL record per event. When the current segment
reaches 2,048 lines, it is renamed to `events.jsonl.1` and a new segment starts.
No retained records are rewritten. Both segments are bounded to 2,048 lines;
the in-memory ring and CLI expose exactly the latest 2,048 valid events across
the segments. Audit persistence stays synchronous, rather than adding a queue
that could lose pending audit events when the process exits.

Existing single-file logs migrate by rename. A missing current segment after
an interrupted rotation is recovered from `.1`; an interrupted final record is
separated from subsequent appends. File permissions remain 0640. Persistence
errors are reported and returned to callers; Profile A startup now also checks
the event store's persistence status. Remote UDP syslog uses nonblocking sends.

The JSONL reader now preserves escaped quotes, backslashes, control characters
and Unicode. This prevents new RBAC audit details being truncated on reload;
it cannot reconstruct details already damaged by earlier software.

The application and package recipe release numbers are both 62. The TLS,
ACSE, RBAC authorization rules, certificate bindings and CID were not changed.

## Validation

- Native tests: `event_store_o14` and `event_store_rotation` pass.
- The rotation regression covers legacy full-log migration, repeated rotation,
  retained order after restart, an interrupted rename, a partial final record,
  escaped audit details, Unicode, explicit rotation failure, and concurrent
  writers. On POSIX it verifies that rotation preserves the original inode.
- The same regression executable passes on Tespro's actual aarch64/musl runtime.
- An isolated MMS harness on port 3783 used separate commissioning state and
  logs under `/etc/ccli/rbac-test-20261016/logfix-r62-20261020`. Its log was seeded
  with 2,047 records; POLICY_ACTIVATED filled record 2,048, and the first TLS
  connection rotated the full segment on the actual flash filesystem.
- All five isolated COTP probes passed, with responses in 4.86–11.45 ms.
- Signed ACSE authentication and an isolated MMS read passed. The reported
  13 kW was a synthetic harness value, with no plant I/O.
- After deployment and verified restart, all ten live COTP probes passed:
  **5.11–12.61 ms**, compared with the original **4.13–5.18 seconds**.
- Live TLS connection times were 28.13–75.33 ms.
- Signed ACSE authentication and an MMS read of
  `CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f` passed on the live port 3782. The live
  server returned 0.000000; this test establishes the MMS read path, not inverter
  measurement availability.
- A read-only CLI export contains exactly 2,048 retained events. All 26 fresh
  RBAC audit records in that export decode correctly as nested JSON.
- Built source hashes match the current workspace for the recorded build inputs.

The protocol probe sends the COTP request bytes from the supplied TSP capture,
using TLS 1.2 / AES128-SHA256 and a two-second socket timeout. Actual Test Suite
Pro on the other PC has not been rerun here; the former COTP delay has been
removed and the existing MMS client passes authentication and reading.

## Deployment and preservation

Only the CCI binary was replaced, followed by a CCI service restart. Tespro was
not rebooted. The initial service restart exposed an existing listener lifetime
issue: the old listening socket was still present while the new instance tried
to bind, so that first instance logged a bind failure. The deployment helper's
readiness check was too broad and initially accepted that old listener.

The restart helper now waits for the old listener to disappear and verifies
that the new CCI process itself holds the listening socket inode. A second
service restart passed this check (PID 13080), followed by the successful live
protocol tests above. This helper change does not alter the library's existing
descriptor inheritance behavior.

Protected configuration, credentials, trust and RBAC state hashes are unchanged.
The original full audit segment is preserved byte-for-byte as the `.1` segment.
No keys were copied into the build or evidence outputs.

Device rollback backup: `/etc/ccli/logfix-r62-backup-20261020` (mode 0700).
It contains the original r61 binary, main YAML, UCI configuration, the settled
original audit log and the protected-file checksum inventory.

Original r61 binary SHA-256:
`861ec666a0aced3b9c97ac0d210c2bffe4c14233d7cd5ef8e3ef66ad4fbca4d0`.

The isolated test server was stopped. The replacement is a bench binary build
and deployment; no APK signing or USB update was performed.

## Evidence files

- `isolated-test-status.txt`, `isolated-cotp-after.json`, `mms-isolated-read.json`.
- `deployment-receipt-r62.txt`, `verified-restart-receipt-r62.txt`.
- `live-cotp-after-verified-restart.json`,
  `mms-live-verified-restart-read.json`, `validation-summary-r62.json`.
- `live-retained-events-r62.json`: read-only retained event export.
- `live-cotp-after.json` and `mms-live-read.json`: the initial unsuccessful
  post-restart checks, retained to distinguish them from the verified rerun.
- `build-r62.log`, `build-sha256.txt`, `built-source-sha256.txt`.

The original diagnosis and packet timings remain in `FINDINGS.md`.

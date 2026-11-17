# Independent investigation of the Test Suite Pro connection delay

Date: 2026-10-20. Device: Tespro 192.168.0.1, CCI PID 5136, r61.

## Finding

CCI blocks its connection-processing thread while persisting audit events to SPI
NOR flash. Its security event ring has reached the 2,048-record limit. At that
limit, every new event truncates and rewrites the entire retained JSONL file.
Two such synchronous writes occur before the first COTP connection response:
one for TLS session establishment, followed by one for `client_connect`.

This explains the measured 4–5 second response delay and TSP timing out first.
It is supported by both supplied captures, direct reproduction using only
TLS/COTP, and live observation of the CCI server thread writing the event file
and waiting inside JFFS2/SPI NOR write functions. An experiment changing or
disabling the active logger was not performed.

## Original captures, independently decrypted and rechecked

The supplied run1 and run2 folders were copied here unchanged. TLS was decoded
locally with the existing server key; no private key was copied into this
evidence directory. `analyze_captures.py` regenerates the packet timelines and
`packet-timings.json`.

| Capture | Stream | Request frame | Reply frame | COTP reply delay | Client TLS close after request |
|---|---:|---:|---:|---:|---:|
| run1 | 0 | 21 | 30 | 4.872526 s | 2.002285 s |
| run1 | 1 | 53 | 72 | 4.382283 s | 2.002880 s |
| run1 | 2 | 85 | 94 | 4.128007 s | 1.002397 s |
| run2 | 0 | 21 | 38 | 5.035303 s | 2.002780 s |
| run2 | 1 | 53 | 62 | 4.361939 s | 2.002624 s |
| run2 | 2 | 85 | 94 | 4.556837 s | 2.010339 s |
| run2 | 3 | 117 | 126 | 4.209296 s | 2.002384 s |
| run2 | 4 | 152 | 161 | 5.179627 s | 1.002426 s |

All eight connections complete TLS and send COTP CR. The COTP CC arrives after
the client has closed; the client then resets the connection. Neither decrypted
capture contains ACSE or MMS messages. The supplied DUT logs show certificate
verification flags of zero, followed by CONNECT and DISCONNECT. Clearing the
TSP CRL field in run2 did not remove this same delay.

The kernel acknowledges the incoming COTP-containing TCP record promptly. The
multi-second wait is for the application reply. Some outbound packets appear
twice within microseconds in the captures; those duplicate observations are not
counted as additional connection attempts.

Relative packet times are used throughout, avoiding assumptions about the PC
and device clocks. The capture metadata stub says r53, but both DUT log pulls
identify the actual running executable as r61, built 2026-10-19T10:41:59Z.

Capture SHA-256:

- run1: `aecea7590ff25ad19dfbcc5fc9232c867b28cc2bcdb32f77051f6bb76588a09b`
- run2: `8f084f30c7a4c947217f731e2c6d69805510c5207bc563d28a84196e17865576`

## Live reproduction and direct observations

At the start of the live review:

- Active config: `/etc/ccli/lab.yaml`.
- Event log: `/etc/ccli/security-events/events.jsonl`.
- Event count: exactly 2,048; file size: 342,728 bytes.
- Event persistence enabled; remote syslog disabled.
- Overlay filesystem: 43% used, approximately 24.5 MB available.
- Load average approximately 0.14. No evidence of general CPU saturation.

`probe_cotp_delay.py` uses the existing TLS transport certificate and key,
TLS 1.2 / AES128-SHA256, and exactly the COTP CR bytes from run2 stream 0.
It sends no ACSE, MMS reads, writes or controls. It leaves the connection open
long enough to receive the delayed COTP CC.

| Probe | TLS connection time | COTP reply time |
|---|---:|---:|
| 1 | 0.090890 s | 4.361943 s |
| 2 | 0.046017 s | 5.509488 s |

Probe 1's fractional `sleep` was unsupported by the device shell. Its client
timing remains valid, but the observer loop finished early. Probe 2 corrected
sampling to one-second intervals and is the reliable complete thread trace.
See `live-probe-2.json` and `live-thread-observations-2.txt`.

During probe 2, server thread 5990 kept a writable descriptor to the actual
event log. The trace contains these observations:

| Device uptime | Server thread wait | Event file bytes |
|---:|---|---:|
| 19961.73 | do_sys_poll | 343310 |
| 19962.74 | spi_nor_prep_and_lock_pe | 95192 |
| 19963.76 | runnable (`0`) | 224115 |
| 19964.78 | runnable (`0`) | 295435 |
| 19965.80 | runnable (`0`) | 20637 |
| 19966.81 | runnable (`0`) | 112258 |
| 19967.83 | jffs2_write_begin | 15274 |

The first shrinking/growing sequence is the TLS security-event rewrite. The
next shrinking/growing sequence is the connect-event rewrite. The subsequent
sequence occurs after the client receives CC and closes. Main thread 5136 also
waits on a futex during the connect audit, consistent with the adapter mutex
being held through synchronous persistence.

The device's event timestamps are taken before persistence. For probe 2:

- TLS_SECURITY_EVENT: 1792500015796 ms.
- client_connect: 1792500018601 ms, 2.805 seconds later.
- The next TLS event, after the client receives CC and closes:
  1792500021309 ms, another 2.708 seconds later.

That 5.513-second span closely matches the independent client measurement of
5.509 seconds. Absolute PC/device timestamps were not equated.

## Source path accounting for the stall

1. `TLSSocket_create` completes the TLS handshake and invokes the registered
   SESSION_ESTABLISHED event handler before returning.
2. `MmsAdapter::Impl::tls_event_tramp` invokes `security_audit` for that event.
3. The audit lambda in `ccli_main.cpp` calls `EventStore::append` synchronously.
4. At 2,048 records, `EventStore::append` calls `rewrite_file_from_ring` under
   its mutex. That function opens the file with `std::ios::trunc` and serializes
   every retained record.
5. After TLS initialization returns, `iso_server.c` invokes the OPENED callback.
   `conn_tramp` takes the adapter mutex and emits `client_connect`, causing the
   second full rewrite.
6. Only after that callback returns does `IsoConnection_start` start processing
   COTP. TSP has already exhausted its observed 1–2 second wait.

Relevant current working-tree locations:

- `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:518` — TLS observer.
- `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:546` — TLS security audit.
- `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:828` — connection callback.
- `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:863` — connect audit.
- `apps/ccli/services/ccli_main.cpp:738` — synchronous audit lambda.
- `apps/ccli/core/event/event_store.cpp:186` — append and full-ring branch.
- `apps/ccli/core/event/event_store.cpp:258` — full-file rewrite.
- `apps/ccli/third_party/libiec61850/src/mms/iso_server/iso_server.c:515` —
  callback before connection-thread start at line 519.
- `apps/ccli/third_party/libiec61850/hal/tls/mbedtls/tls_mbedtls.c:964` —
  session-established event before returning from TLS socket creation.

## Comparison with ffb0f19 and the earlier investigation

The independent findings and live measurements above were established before
reading the earlier investigation's final responses for comparison.

The earlier work correctly localized the wait to pre-MMS connection handling
and identified synchronous auditing as a candidate. It had not checked the
full event ring or observed the running thread in flash writes. Those are the
new observations establishing the concrete blocking mechanism.

Against ffb0f19f104efc8358e7b82e79ba996bbde5247b:

- The generic connection callback and synchronous `client_connect` audit were
  already present.
- The full-ring rewrite algorithm was already present. The only diff in
  `event_store.cpp` changes remote-syslog severity selection; remote syslog is
  disabled on this device.
- The current adapter adds the Profile A TLS event observer and therefore an
  additional synchronous audit before the existing connection audit.
- The transport-binding comparison requires an ACSE-issued token. Both original
  captures and the new reproduction stop before ACSE. That comparison cannot
  account for this reproduced delay. TLS acceptance and its security audit do
  run, and must be distinguished from MMS role authorization.

A prior successful connection is compatible with this failure: before the ring
reaches its retention limit, each event appends a line instead of rewriting
all retained records. The exact ring occupancy during an earlier successful
test is not available, so this explains the possible transition rather than
proving the historical occupancy.

## Correction indicated by the evidence

Remove full-log compaction from each event append and keep slow flash
persistence out of protocol-critical callbacks, while preserving security
auditing, retention limits and explicit handling of persistence failures.
Review lock scope as part of that change: the existing connect audit holds the
adapter mutex for the whole persistence operation.

Validation should fill the retention ring deliberately, reconnect repeatedly,
and measure COTP/ACSE/MMS latency with the same client timeouts. It must also
verify log retention, recovery after interrupted writes and reporting of
persistence failures. Increasing TSP's timeout could mask the stall but would
leave this device-side defect in place.

No firmware, configuration, certificates or existing audit files were replaced
or cleared during this investigation. Diagnostic connections generated normal
audit entries. The source comparison used the current r61 working tree; the
device reports Git revision `unknown`, so exact binary/source identity cannot
be proven from the version string alone.

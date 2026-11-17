# TSP wire behavior and implemented client profile

This records the real TSP requests in the 2026-10-20 run 3 capture and the
subsequent client implementation. It covers the observed connection profile;
it does not imply that every Test Suite Pro feature or test case is implemented.

Evidence: `lab/evidence/delay-review-2026-10-20/run3/FINDINGS.md`,
`associate-request-frame27.txt`, `captured-session.bin`, and
`captured-presentation.bin`. Capture SHA-256:
`01426025f507f75d96d6b7b80e51006766e03482f98ada721358e2834bf33eb0`.

## Observed connection and association

- TCP to 192.168.0.1:3782, mutual TLS 1.2, then COTP and session/presentation/ACSE.
- Presentation contexts in this order: MMS ID 3 (`1.0.9506.2.1`), ACSE ID 1
  (`2.2.1.0.1`), authentication ID 5 (`1.0.840.0.1.1.4.1`). All use BER `2.1.1`.
- Authentication abstract-syntax OID bytes: `28 86 48 00 01 01 04 01`.
  Earlier custom clients emit an extra zero (`1.0.840.0.0.1.1.4.1`). Server r63
  accepts that specific legacy alias as well as TSP's encoding. This does not
  skip certificate authentication, proof validation, or RBAC checks.
- Presentation selector `00000001`; session selectors `0001`.
- ACSE application-context OID `1.0.9506.2.3`; called and calling AP title
  `1.1.1.999.1`, AE qualifier 12; calling invocation IDs 10.
- ACSE authentication mechanism `1.0.840.0.1.0.1.1`; authentication EXTERNAL
  indirect reference 5; MMS user-information indirect reference 3.
- MMS Initiate request is embedded in the initial AARQ.
  Its local-detail-calling is 65,000, maximum outstanding calling/called is
  10/10, and proposed data-structure nesting level is 5. Preserve the captured
  parameter/service-support bit strings when building a later fidelity test.
- The 1,586-byte session CONNECT uses extended lengths and user-data parameter
  C2; its presentation CP is 1,558 bytes. It arrives as two COTP DT fragments
  (1,021 and 565 session bytes). Reassembly must precede parsing.
- All four captured attempts complete TLS and COTP. r62 closes about 3 ms
  after receiving the complete AARQ, before its authentication callback.
  This capture contains no accepted association or read to imitate.

## Implemented client profile

`build.sh` enables `CCLI_MMS_TSP_CLIENT_PROFILE` for the client executable.
`scripts/generate_mms_tsp_client.py` generates client-only overlays over the
reviewed certificate-authentication sources. Vendor files and the installed
Tespro APK remain unchanged. Regular server/package builds leave this flag off.

The profile reproduces the captured COTP request (source reference 4, selectors
0001, TPDU 1024), presentation context order 3/1/5 and calling selector, ACSE
invocation IDs and BER direct reference, and MMS Initiate parameters including
the captured service-support bit string. Association-response reassembly now
waits for all COTP fragments before parsing the signed AARE.

The client verifies the returned server certificate chain, signed timestamp and
RSA/SHA-256 proof before the native connection is allowed to issue operations.
The GUI displays the verified server identity/fingerprint and the active profile.
`--mms-ca`, or the GUI's MMS server CA field, can override the TLS CA for that
verification. Blank uses the TLS CA. TLS peer-identity mode remains available;
the captured three-context/signed-AARE profile is certificate-authentication mode.

All six MMS write request paths in the TSP client overlay now check the ASN.1
encoder result against the actual transmit-buffer size. Failed or truncated
encodings release the reserved buffer and return a local resource error before
`sendAsyncRequest`, so partial BER cannot be sent as a complete request. This
protects the transport from encoder/buffer failures; it does not validate an
independently constructed BER payload or replace live peer interoperability checks.

`tests/verify_tsp_wire.py` observed the actual client's bytes through a local
authenticated TLS relay to Tespro, while completing read/browse/control-info/read
on one association. Its presentation request matched the captured TSP request
after normalizing **only** the fresh authentication timestamp and signature.
The COTP request matched byte-for-byte. The timestamp deliberately retains
microseconds and UTC `Z` to avoid identical signed proofs on rapid reconnects;
this adds seven bytes, giving request fragments 1021/572 instead of 1021/565.
The server reply arrived as fragments 1021/247 and was accepted and verified.

Nine server-authentication regression tests pass. The isolated synthetic server
also passed DSO write/operate/readback and Viewer write/operate denial checks.
The GUI backend passed connect, verified-peer display data, read, control-info,
and disconnect checks. No live controls were sent by these tests.

## Historical server repair / future coverage

Further work can compare additional TSP service sequences, reports and failure
handling against fresh captures. Do not infer those unobserved details from the
initial association capture. Keep captured-proof freshness/replay protection.

The custom client was left unchanged during the initial server repair, then
rebuilt separately after the operator explicitly requested this client work.

Server r63 validation: a disposable probe using this captured request shape,
with only a fresh timestamp/signature, received an accepted AARE and MMS Initiate
response on the live Tespro port in 91.6 ms. The server response signature
verified. The unchanged client also completed a signed association and read.
This does not substitute for reconnecting the actual TSP application.

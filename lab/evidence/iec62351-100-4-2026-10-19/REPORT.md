# Tespro IEC TS 62351-100-4 gap audit — 19 October 2026

## Result

The current Tespro firmware demonstrates mutual TLS and useful MMS role enforcement, but does **not** demonstrate native IEC 62351-4 end-to-end (E2E) conformance. The missing E2E implementation prevents the mandatory OSI server test families below from being closed. This audit identifies gaps; it is not a completed formal conformance test campaign.

The supplied standard was inspected visually, including its mandatory/optional summary and detailed tables. Its text layer contains mostly website navigation, so text searching alone is unreliable. Rendered pages in this folder provide the inspected source views. This PDF is an export of a document viewer: use the printed standard page numbers below, not its toolbar counter. PDF page = printed page + 2 for the inspected tables.

Source: `C:\Users\Administrator\Desktop\CCLI Alireza\CCLI\Normativa\IEC TS 62351-100-4 2023.pdf`.
SHA256: `93c73072bc18d08020489c40657691db5010dc3d19344729e642def7d96e7fa3`.

## Applicability

This audit treats CCI on Tespro as an IEC 61850-8-1 MMS **server in an OSI environment**. Table 3, printed pp.19–20 / PDF pp.21–22, explicitly marks the relevant association, resiliency, clear transfer, release and rekey families mandatory. Client-only DUT cases are excluded from this server audit; the PC tool is the test equipment. XMPP/61850-8-2 and ICCP are not demonstrated product interfaces here.

These are mandatory **within that E2E test scope**. A final PICS/PIXIT and the referencing product specification determine the formal claim and conditional cases. Selecting an A-profile-only claim would not make its TLS or ACSE results evidence of native E2E compliance. Older project planning documents disagree about whether E2E is deferred or mandatory for the intended national profile; this audit does not resolve that external product-scope decision.

## Missing mandatory test coverage

| Standard reference | Required behavior for the server scope | Current status and missing evidence |
|---|---|---|
| Table 15, rows 1–2; printed p.27 / PDF p.29 | Accept a valid EnvPDU/SecPDU association and process its E2E release | **Implementation gap.** Captured associations use legacy application context `1.0.9506.2.3`, not a demonstrated native E2E handshake. Signed ACSE authentication alone is insufficient. Native request/accept and ReleaseReq/ReleaseRsp evidence is missing. |
| Table 17, rows 1–3; printed p.29 / PDF p.31 | Reject the wrong application context and invalid OSI association reference; execute Table 50's SecPDU resiliency cases | **Untested formal cases plus E2E implementation gap.** The revoked TLS fixture is not a substitute for malformed E2E association tests and specified diagnostics. |
| Table 50; starts printed p.55 / PDF p.57 | Server handshake resilience: protected-protocol mismatch, signature/clearToken problems, invalid certificate and identity bindings, and remaining malformed handshake cases | **Not covered by current client.** Inspected rows 1–5 and 20 include protocol mismatch, missing signature/clearToken, invalid public certificate, sender-address mismatch, and a handshake timestamp over ten minutes away. The full table needs a dedicated E2E fault injector. Our ACSE proof tests have a different encoding and validation path. |
| Table 18, server direction row 2; printed p.30 / PDF p.32 | Receive and validate native clear transfer after association | **Implementation gap.** Need SecPDU ClearTransfer encoding/decoding, integrity validation and a successful response carried in the specified protection. Ordinary MMS reads inside TLS do not close this case. |
| Table 20, rows 1–3; printed p.31 / PDF p.33 | Reject invalid/missing presentation contexts and execute Table 51's server transfer-resilience cases | **Not tested; E2E prerequisite missing.** Need the specified aborts and diagnostics, not just generic disconnects. |
| Table 51; starts printed p.59 / PDF p.61 | Server-side protected transfer resilience, including mandatory clearToken and version checks, plus the rest of the table | **Implementation/test gap.** Inspected rows 1–2 require missing-clearToken and unexpected-version diagnostic checks. Native integrity, sequence/replay and time-related negative cases require the complete table campaign after the E2E engine exists. |
| Table 24; printed p.35 / PDF p.37 | Process client-initiated E2E rekey and protected traffic with the changed key state | **Implementation/test gap.** No application DH/HKDF rekey state machine or old/new key transition evidence. Test specifies continuous traffic and a 15-minute trigger. The present capture cannot establish this result. |
| Table 25; printed p.36 / PDF p.38 | Initiate server E2E rekey and continue protected traffic across the transition | **Implementation/test gap.** Need server request, client response and new-key traffic evidence; TLS reconnection is not E2E rekey. |
| Tables 5–13; printed pp.23–25 / PDF pp.25–27 | Declare base profile, association/release or abort, EnvPDU/SecPDU support, crypto and security ASN.1 identifiers | **Documentation and implementation evidence gap.** Table 7 requires at least abort or release. Table 12 marks RSA, EC curves and RSA/ECDSA SHA256 signatures mandatory; integrity/encryption requirements have explicit conditions. Existing RSA TLS and ACSE success does not prove all these E2E capabilities. |

Do not count the rows above as a fixed number of failed test cases: some are families with many individual cases, and unimplemented prerequisites prevented execution. `mandatory-gaps.csv` is a machine-readable family/row map.

### Optional or excluded cases

- Table 3 identifies encrypted application data transfer and its resilience (Tables 21–23 / 53–54) as **optional**. TLS encryption is a separate layer. Do not present missing EncrTransfer as an unconditional mandatory failure.
- Tables 14, 16 and 19/52 are client-only DUT cases in this summary; not part of the Tespro server claim. Table 24 is still relevant because a server must respond to a client's rekey request.
- XMPP cases need a separate interface claim and are not exercised on this OSI MMS endpoint.

## What the packet capture actually proves

Capture: `tespro-mms.pcapng`, Wireshark/dumpcap 4.2.14, Ethernet 2, filter `host 192.168.0.1 and tcp port 3782`. Capture ran for 180 seconds; matching packets occurred in a 1.4465-second burst. **145 packets, zero reported capture drops, four TCP streams.** See `capture-info.txt` for hashes and timestamps.

Decryption used the already-existing local laboratory `apps/ccli/config/tls/server.key` directly in Wireshark/tshark. No private key or session secret was copied into this evidence folder or embedded in the capture. RSA key exchange permits this local analysis. `packet-summary.tsv` contains decoded application summaries; `association-details.txt` contains selected decoded association fields. Wireshark was opened on this capture with the same decode settings.

| Stream / frames | Observation | Interpretation |
|---|---|---|
| 0 / ServerHello 6 | TLS 1.2, cipher `0x003c` (`TLS_RSA_WITH_AES_128_CBC_SHA256`) | Negotiated transport for Aggregator; not an exhaustive TLS conformance result. |
| 0 / 25–26 | Association request and accepted response | Role-bearing TLS identity association works. |
| 0 / 27–30 | Plant measurement and DSO mode reads receive object-access-denied | Aggregator's DSO-scope read restrictions are enforced. |
| 0 / 31–32 | Aggregator mode read returns 5 | Own-scope read succeeds, mode is off. No control sent. |
| 0 / 33–44 | Discovery works; DSO dataset and report-block access denied | Observed policy boundaries for this identity. |
| 1 / ServerHello 54, association 72–73 | Viewer connects; allowed reads/RCB read succeed | Matches the read-only driver results. Reading a value does not establish measurement quality. |
| 2 / ServerHello 103, AARQ 119, closes 121–125 | Signed dual-certificate DSO completes TLS but connection closes after its authentication request; no accepted AARE | **Observed live failure** at association, not a TLS connection failure. Exact rejection reason remains unconfirmed. |
| 3 / 139 | Server sends fatal TLS `Certificate Revoked` alert | This revoked fixture is rejected at TLS, before MMS. |

Read-only driver result: **16 checks passed, 1 failed** (`live-reads/results.json`). These are representative functional checks, not 16 passed IEC test cases. Existing broader role tests in other evidence folders have different timestamps, software conditions and scope; they are not silently counted as present-run results.

## Current bench prerequisites

`device-status.txt` and `serial-ownership.txt` preserve the observed state:

- GNSS fix false; chrony reports stratum 0 and `Not synchronised`. Tespro was about 38 minutes behind the PC. The signed ACSE verifier in `mms_rbac_security.cpp:157` accepts only +/-120 seconds, so this is a strong candidate cause for the failed signed DSO association. The packet trace and available generic log do not independently identify its exact rejection reason. Correct synchronization and rerun before treating it as an authentication implementation defect; keep freshness validation enabled.
- CCI reports Modbus RTU `/dev/rs485_2_uart`, 9600, unit 1, link down, `read_fail`, quality stale, P/Q zero. COM8 simulator logs contain a successful FC03 @60000 read, but do not establish continuing healthy reads at capture time.
- Serial-sharing service remains enabled for RS485-2, and seriald PID 4691 has fd12 open on `/dev/ttyUSB1`, the RS485-2 target. This is a confirmed competing owner and a diagnostic lead, not proof it causes the failure. No service setting was changed during this audit.
- Wireshark captures Ethernet, not COM8 wire bytes. Healthy RTU and physical control behavior need simulator logs and fresh CCI readback, or a proper serial tap. No live writes/operations were sent in this run.

## Closure order

1. Restore verified time synchronization; rerun the signed dual-certificate association with capture and explicit authentication diagnostics.
2. Restore fresh RTU readings, resolving port ownership/wiring as evidence requires, then complete the outstanding Aggregator physical-control proof separately.
3. Freeze the server PICS/PIXIT, supported algorithms, identity bindings, error diagnostics, timing and referencing product profile.
4. Implement the native E2E engine in both DUT and test equipment: handshake, SecPDU integrity/ClearTransfer, sequence/replay validation, release/abort and both rekey directions.
5. Execute Tables 15/17/18/20/24/25 and all applicable Tables 50/51 subcases with exact expected diagnostics and decoded traces. Run rekey tests long enough for the specified triggers. Maintain separate IEC 62351-100-3 transport evidence.

This task changed no firmware, policy, allowlist, time setting or serial-service configuration.

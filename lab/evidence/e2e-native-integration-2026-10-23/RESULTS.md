# Native E2E integration evidence — 23 October 2026

> Historical integration/timed-renewal snapshot. The current source, binary and additional abort/resilience results are recorded in [the subsequent extension](../e2e-abort-resilience-2026-10-23/RESULTS.md). Binary hashes and outstanding-case statements below describe this earlier phase.

This records host engineering verification and an ARM64 compilation. It is not device runtime evidence or formal IEC conformance. No native E2E APK was packaged or installed.

## Verified results

| Check | Result |
|---|---|
| Full fast E2E CTest campaign after final OSI diagnostic correction | **6/6 PASS**, 19.08 seconds |
| MMS compatibility, signed RBAC, TLS authorization and preserved source baseline after final relink | **5/5 PASS**, 28.53 seconds |
| ARM64 full-protocol compile | **PASS**, MMS/TLS + GOOSE + real IEC104/TLS + SDK libmodbus |
| Source whitespace check | **PASS**, line-ending notices only |
| Real 900-second server-requested renewal | **PASS**, reqRekey at 840.1s, new keys acknowledged at 850.2s, protected reads beyond the original epoch and signed release |

The fast native campaign verifies real localhost mutual-TLS/COTP/session/presentation/MMS traffic. It uses the C++/Mbed TLS native server and a separately implemented Python/OpenSSL development peer. The project peer is not accepted external TEQ.

### Positive traffic and authorization

- RSA/group 14 and ECDSA/group 23, each with HMAC-SHA256, AES-128-GMAC and AES-256-GMAC.
- Native signature/identity/CRL validation, actual MMS initialization and Health value 7 readback.
- Protected asynchronous reports, repeated reads, client DH renewal, old-key changedKey acknowledgement, new-key traffic and signed release.
- A 16-byte GMAC nonce case confirms that the recommended 12-byte size was not incorrectly imposed as a universal mandatory size.
- Actual product `MmsAdapter` with owner-signed policy: authorized DSO measurement reads and renewal/release; denial on an existing out-of-scope `LLN0.Mod.stVal`; uncommissioned native certificate denied; expected audit events present.
- Strict native configuration cases and same server TLS/application certificate configuration rejection.

### Negative traffic

- Wrong application identity, forbidden native ACSE authentication fields, replayed handshake and invalid request signature produce verified signed rejection.
- Invalid ICV, replayed transfer, wrong association, expired timestamp, nonzero initial sequence, client-sent server-only reqRekey/changedKey, unexpected IV, missing GMAC nonce, reused nonce and altered ICV algorithm produce verified signed security abort in the implemented path.
- Final review corrected context/reference failures to their required OSI layer: wrong transfer PCI produces service-user ABRT with OsiDiagnostics invalid-pci (3); wrong association indirect-reference produces invalid-indirect-reference (2). These carry OSI user-information rather than a signed security response. Raw mapping checks precede handshake security semantics. The source was checked against IEC 62351-4 clauses 15.6.2–15.6.3; both RSA and EC wire cases passed in the final 6/6 rerun.
- The nonce-reuse case uses a fresh valid timestamp/sequence and recalculated ICV, so it independently exercises nonce tracking. The replay test separately repeats a packet.
- Per raw RSA/EC campaign, exactly 14 authorized read-access callbacks occur: three report dataset checks, nine positive reads, one initial replay-test read and one initial nonce-reuse-test read. Invalid requests add none.
- CRL fixtures prove matching critical scope is accepted and wrong URI/type, partial reasons and malformed scope are rejected. Default Mbed TLS/TLS CRL parsing remains strict.

Other provider-versus-user OSI mapping and source-dependent diagnostics are not fully qualified by these cases. Those broader requirements remain open.

## Real timed renewal

The separate `--timed-rekey` campaign uses a real 900-second epoch and continuous protected GMAC-256 reads. It observed server reqRekey at **840.1 seconds** and acknowledged renewed client/server keys at **850.2 seconds**. Protected reads continued beyond the original 900-second deadline (the test requires at least 905 seconds), followed by verified signed release. The process exited successfully after its exact application-dispatch count assertion. This run preceded the final context/reference error-response correction; the positive renewal path was unchanged. This is host server-triggered renewal evidence for the tested profile. A peer ignoring renewal, overdue-key behavior, all other algorithms and actual Tespro timing still require qualification.

## Build and dependency record

Base repository checkpoint: `bdbac16` plus the uncommitted integration/readiness changes listed in `source-hashes.json`. The prior 22 October ARM64 hash described a build with an IEC104 stub; it is superseded by this full-protocol binary.

- SDK: OpenWrt 25.12.5 mediatek/filogic, GCC 14.3, aarch64 cortex-a53 musl.
- Binary: `apps/ccli/build-e2e-arm64/ccli` (ELF64 ARM aarch64; interpreter `/lib/ld-musl-aarch64.so.1`).
- Final binary SHA-256 after the OSI diagnostic correction: `15234b8a02202c8388698bc7826b9188b2e3d834b929118a753d06bb96e84f7f`.
- Restored dependency: repository-pinned lib60870 commit [`5b678d995a289c03d233b1ae2c6712d7b6abbed6`](https://github.com/mz-automation/lib60870/commit/5b678d995a289c03d233b1ae2c6712d7b6abbed6), fetched from the official repository. The previously found SDK cache was not substituted for the pin.
- `.gitmodules` now supplies that missing dependency URL. The commit pointer was not changed.
- `CCLI_REQUIRE_PROTOCOL_DEPENDENCIES=ON` in target readiness/package configuration prevents silent MMS/IEC104 stub fallback. CMake 4 compatibility for this older pinned dependency is explicit (`CMAKE_POLICY_VERSION_MINIMUM=3.5`).

The original TLS baseline and dual-certificate fingerprint manifest remain intact. `CCI_RBAC_NATIVE_E2E_EXTENSION.json` separately pins reviewed native integration sources and narrowly records the earlier TSP syntax correction, repeated configured TLS client pins and native failure cleanup. Original protected vendor/TLS sources, configuration calls and lab trust settings still match after those explicit extensions. A passing fingerprint check detects subsequent source drift; it does not certify TLS/E2E behavior.

## Reproduction

Run from the repository root inside WSL:

```sh
bash scripts/check_e2e_host.sh
ctest --test-dir apps/ccli/build-e2e-crypto -R mms_ --output-on-failure
python3 -u apps/ccli/tests/unit/e2e_native_stack_test.py \
  --server apps/ccli/build-e2e-crypto/OPTIONAL/e2e_native_server_test \
  --fixtures apps/ccli/build-e2e-crypto/e2e-timed-fixtures \
  --probe apps/ccli/tools/mms-client/e2e_probe.py --timed-rekey
bash apps/firmware-updater/scripts/check-ccli-target-build.sh
```

The fast CTest transcript is `e2e-ctest.log`; the final compatibility transcript is `mms-ctest.log`. The aggregate host build now also builds the compatibility executables, so the documented commands work on a clean host build. Private fixture material stays in ignored build directories and is not copied into this evidence folder.

## Remaining work

The repository's local `config/tls/server.pem` and `client.pem` public certificate subjects both encode attribute `2.5.4.106` as UTF8String. Those specific compatibility certificates cannot be reused as native Annex G application certificates. Native issuance preserving the OBJECT IDENTIFIER value and commissioning the new identity/TLS fingerprint bindings is a concrete target-test prerequisite. No private keys or live device credentials were changed.

Exact OSI/security/application error mapping; MMS-originated ApplAbort; complete initialization-reject/client-abort and malformed-field/BER/size campaigns; ignored/overdue renewal; concurrency and resource cases; intermediate PKI/lifecycle; target credentials and device runtime; external S02/S13 interpretation, PICS/PIXIT and qualified TEQ. Application encryption, attribute certificates and intermediate routing are not claimed. See the current [implementation plan](../../conformance/e2e/IMPLEMENTATION_PLAN.md) and [Tespro runbook](../../conformance/e2e/TESPRO_LAB_READINESS.md).

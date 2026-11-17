# Native E2E abort and resilience — 23 October 2026

Engineering evidence for the requested abort/error handling and resilience extension. This revision has not been packaged as an APK or installed on Tespro. Independent interoperability and formal conformance remain open.

## Implementation

- Classify malformed/non-kernel ACSE as provider ABRT without user-information; outer title/context/version failures use AARE rejection without user-information and with the appropriate user/provider source diagnostic.
- Distinguish unsigned OSI diagnostic aborts from signed security and application aborts. Validate native presentation PDVs before the legacy parser can truncate their PCI.
- Verify signatures and transfer ICVs before signed/authenticated token semantics. Omit security diagnostics on alarm paths, including bad signatures/ICVs and replay. Preserve the known client-only association ID on handshake rejection.
- Connect synchronous MMS callbacks to queued signed ApplAbort, retaining the optional encoded MMS diagnostic. Ordinary MMS service errors keep their normal service response; services explicitly request an association abort when appropriate.
- Validate signed incoming client aborts and provided certificate paths. Close on valid peer provider/OSI abort without replying. Preserve signed release behavior.
- Bound native MMS negotiation/serialization and authenticated replay history. Default replay history: at most 65,536 pairs per association, pruned against the ten-minute freshness window; exhaustion closes that association.
- Reap timer-stopped connections without waiting for new socket input. This fixes the observed expiry path that sent an abort but left an idle socket open.

## Verification

| Check | Result |
|---|---|
| Full E2E CTest campaign | **7/7 PASS**, 76.43 seconds |
| Expanded `e2e_native_resilience` within that campaign | **PASS**, 55.03 seconds |
| MMS compatibility, signed RBAC, TLS authorization and source baseline | **5/5 PASS**, 34.62 seconds |
| Source whitespace check | **PASS**, line-ending notices only |

The expanded campaign executes 64 additional case groups for each raw RSA/EC server profile, including provider/OSI/security/application termination, signed initialization rejection, malformed envelopes, authentication precedence, replay and sequence rules, PDU negotiation, four concurrent associations, abrupt report disconnects, three ICV epoch profiles and replay-cache exhaustion. Each profile asserts exactly **117 authorized MMS read callbacks**; rejected transfers add none. Signed aborts must verify, carry the expected diagnostic or omit it on alarm paths, and be followed by socket EOF. Valid incoming peer aborts must close without a response. The product authorization campaign also passes inside the E2E tests.

The interrupted build left invalid output files and a stale host object. Those were explicitly relinked/recompiled before this passing run. ARM64 contains the same final source correction. Test fixtures and private keys remain in ignored build directories; saved evidence logs were scanned for private-key PEM headers.

ARM64 compile: **PASS**, canonical `check-ccli-target-build.sh`, actual MMS/TLS, GOOSE, IEC104/TLS and SDK libmodbus enabled. Output is ELF64 ARM aarch64, musl interpreter `/lib/ld-musl-aarch64.so.1`.

Binary: `apps/ccli/build-e2e-arm64/ccli`.
SHA-256: `0c937e6b1fa8e51424179993d7cb1244173cbc23bf3d2d285a20cad6d66e5a07`.

Saved logs: [E2E CTest](e2e-ctest.txt), [MMS CTest](mms-ctest.txt). Source and binary fingerprints: [source-hashes.json](source-hashes.json).

## Harness and practical limits

`e2e_native_resilience` runs real localhost mutual TLS/COTP/session/presentation/MMS with the C++/Mbed TLS server and the project Python/OpenSSL peer. It runs RSA/group 14 and ECDSA/group 23. Epoch cases exercise HMAC-SHA256, AES-128-GMAC and AES-256-GMAC using an injected C++ monotonic clock; production uses the actual steady clock and the 900-second minimum is unchanged. Replay-cache exhaustion uses a 64-entry harness profile, followed by a healthy reconnect. Wall-clock timestamp validation remains real.

The earlier real 900-second GMAC256 renewal is historical evidence in [the preceding integration results](../e2e-native-integration-2026-10-23/RESULTS.md). It has not been repeated with real elapsed time on this abort/resilience revision.

Remaining work includes the complete applicable field/size/lifecycle matrix, constructed-string BER variants, target memory/load and real-time runs, native credential reissuance/commissioning, Tespro installation, independent peer qualification and the open S02/S13 interpretations. Application encryption, attribute certificates and intermediate routing are not claimed. The 108 tracker groups retain formal OPEN status.

Reproduction and commissioning: [Tespro runbook](../../conformance/e2e/TESPRO_LAB_READINESS.md). Current next steps: [implementation plan](../../conformance/e2e/IMPLEMENTATION_PLAN.md).

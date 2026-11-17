# Native E2E implementation and closure plan

> **22 October 2026 continuation update:** native TLS/OSI/MMS integration, protected reports, live rekey and product authorization have now been implemented, and an ARM64 binary built. Development was paused at the user's usage-limit request before the final expanded campaign passed. Read [the current handoff](../../../HANDOFF_E2E_2026-10-22.md) first. The snapshot and planned-only descriptions below predate that integration and require reconciliation; their six-partial count is not current code progress. No new APK or Tespro E2E installation is claimed.

Date: 2026-10-22. Prepared for CCI on Tespro. Status: standalone E2E building blocks are present; native MMS/OSI integration, profile freeze, independent-peer evidence and formal evidence remain open.

## Decision and scope

Implement IEC 62351-4 native security at the MMS/OSI boundary, beneath the MMS service dispatch and around all outgoing MMS traffic. Keep the E2E engine separate from compatibility ACSE timestamp authentication. The existing TLS/RBAC foundation is reusable, but native certificate/identity validation, protocol state, wire mapping and key epochs require explicit implementation.

Proposed product baseline: IEC 61850-8-1 MMS **server**, OSI environment, native E2E **with TLS**, integrity-protected ClearTransfer, both secure abort and orderly release, both application rekey trigger directions. Non-AE baseline activates **HMAC-SHA256 and AES-128/256-GMAC integrity support**; one HMAC happy path is not the entire mandatory crypto capability. RSA and EC certificate/signature support, including both declared public-key curves, also requires implementation evidence. Brainpool public-key capability and optional ECDH group 28 are distinct declarations.

Application encryption remains a conditional capability. Retain its requirements in the matrix and make the product decision with the DSO/lab. If AES-GCM encryption is selected, implement both required AES key sizes and all applicable encrypted-positive/resilience tests. Missing source pages and inconsistent test wording must be resolved before that formal claim.

Current scope excludes a client DUT claim, XMPP, ICCP and operation as an intermediate application router. Incoming client messages and server obligations toward client-originated rekey remain included. CEI edition/amendments and the formal assessment procedure must be frozen with the product/lab owner; the national E2E support requirement is not waived by choosing compatibility-only documentation.

## Audit snapshot — 2026-10-22

This is a source review plus a fresh host build/test run on 2026-10-22. The repository contains useful standalone code, but no native E2E handshake or protected transfer has been accepted by an independent peer or enforced in the MMS stack. All 108 rows in `requirements.csv` remain open; six record partial code, and 102 remain at planning status. Do not describe the product as E2E-capable or any mandatory requirement as closed.

| Area | Present in the repository | Still open |
|---|---|---|
| BER/wire codec | Bounded BER reader; structural decoders and definite-length encoders for association, transfer, signed messages and presentation wrappers; original TBS/TBP slices retained. | Freeze amendment-specific wire interpretation; independent byte vectors; constructed-string support and remaining parser/profile rules; bind the codec to libiec61850 ACSE/presentation. |
| Crypto and key schedule | Mbed TLS provider for SHA-256, HMAC-SHA-256, HKDF, RSA/ECDSA SHA-256, AES-128/256 GMAC and MODP/ECDH groups; initial/rekey transcript and directional-key schedule. | Resolve the `h0=0` and MODP shared-secret byte conventions with an independent peer/lab; map negotiated OIDs/profile; implement live signature/ICV/nonce use; prove ARM64 provider capabilities. |
| Identity, CRL and association preflight | Annex G certificate/path validator; CRL distribution-point/issuing-point name, issuer, certificate-type and reason-scope checks; standalone HandshakeReq preflight verifies signature over preserved TBS bytes and checks time/protocol/certificate path. | Add and run wrong-scope CRL cases; derive expected identity from ACSE AP-title/AE-qualifier; load commissioned anchors/CRLs; connect validation to session identity/RBAC; generate and validate handshake responses and failure paths. |
| Time and replay | GeneralizedTime parser/UTC formatter and a per-window timestamp/sequence prepare/commit API. | Call only after authentication on live traffic; define profile limits/nonce generation; protect all MMS request, response, report and control traffic; implement both rekey directions and epoch transitions. |
| Product stack and evidence | E2E sources are listed in `apps/ccli/CMakeLists.txt`; a fresh Ubuntu WSL build compiled `ccli_adapters`, and all five registered `e2e_` CTest cases passed on 2026-10-22. | No native ACSE/AARE/abort/release bridge, per-connection E2E state, MMS dispatch gate, protected send path, audit integration, Tespro E2E run, independent native-peer interoperability or accredited 100-4 assessment. Existing identity tests cover missing CRL coverage and revoked peers, but do not cover wrong-scope or malformed-scope CRLs. |

The TSP client support/entitlement question is still unconfirmed in the repository record. TSP should not be a coding prerequisite: use it only if its exact installed version is proven to implement the required native E2E profile. Otherwise arrange a separate native E2E peer or qualified test equipment; the in-repository harness can support development but cannot replace independent formal assessment.

## Deliverables and traceability

- [Requirements map](REQUIREMENTS.md): grouped implementation obligations with exact clause/table/row references, applicability, code workstream and verification.
- [Requirements CSV](requirements.csv): editable tracker; status starts open for native implementation/evidence.
- [Abstract test cases](test-cases.csv): every Table 50 server-handshake row and Table 51 clear-transfer row, normal OSI cases, rekey adaptations and conditional encrypted families.
- [Every-table disposition](table-coverage.csv): all 79 tables accounted for, including exclusions and conditions.
- [Source/interpretation issues](SOURCE_ISSUES.md): concrete page omissions, incompatible tags/enums, time checks and test-role inconsistencies.
- [Source manifest](source-manifest.json): exact PDFs and hashes. Source page references use printed standard numbers; do not use viewer toolbar numbers.
- [Repository input inventory](REPOSITORY_INPUT_INVENTORY.md): confirmed local standards, recorded TSP 4.7.4.5037/help, unanswered lab questionnaire and the narrowed external-input list.

The test specification does not enumerate every requirement in the implementation standard. For example, its Table 51 has only four rows. The plan adds mandatory ICV, nonce, timestamp, replay, key-direction and renewal checks from IEC 62351-4 even when the short server test table does not list a dedicated case.

## Code findings that determine the implementation

| Existing location | Current behavior / gap | Planned action |
|---|---|---|
| `apps/ccli/third_party/libiec61850/CMakeLists.txt` | Vendored stack identifies itself as 1.6.0; no native SecPDU/ClearToken/rekey engine was found in the reviewed MMS sources. | Pin vendor revision/hash; decide maintained overlay/fork versus an E2E-capable stack integration. |
| `apps/ccli/cmake/mms_certificate_auth.cmake` and `scripts/generate_mms_certificate_auth.py` | Build-time overlays alter ACSE/session/presentation/connection source for compatibility certificate authentication. | Add a separate deterministic native integration layer and baseline checks. Keep all compilation units consistent with private-header changes. |
| `third_party/libiec61850/hal/tls/mbedtls3/mbedtls_config.h` | The prior `#error "MBEDTLS"` build stop is removed. The selected bundled Mbed TLS 3.6.0 configuration enables DHM, ECDH, ECDSA, ECP with secp256r1 and brainpoolP256r1, HKDF, SHA-256 and AES/GCM. Host WSL build and E2E provider/identity tests now exercise the selected configuration. | Record target feature manifest and verify ARM64 build/runtime; TLS availability alone does not prove E2E crypto support. |
| `src/mms/iso_server/iso_connection.c` | Association and incoming MMS dispatch at roughly lines 228-294 and 321-372; asynchronous `IsoConnection_sendMessage` starts at line 649 in the reviewed snapshot. | Validate/unwrap security before MMS callbacks; wrap response and asynchronous/report send paths; integrate tick/disconnect cleanup. Line numbers must be rechecked at implementation time. |
| `src/mms/iso_presentation/iso_presentation.c` | Existing context-list and User-data encoding expect current stack paths. | Negotiate and retain native E2E context; validate correct wrapper before security decode; audit all BER lengths and context selections. |
| `apps/ccli/adapters/iec61850_mms/mms_rbac_security.cpp` | Compatibility proof verifies a signed timestamp, enforces +/-120 seconds and uses a short proof cache; its certificate AP/AE check accepts a textual OID representation. | Standalone strict Annex G validation now exists in `e2e_identity.cpp/.hpp`; native association code must still derive the expected AP-title/AE-qualifier OID and pass its validated identity into RBAC. Keep compatibility time/cache semantics separate from native E2E. |
| `mms_aare_auth.cpp/.hpp` | Compatibility AARE authentication builder and fixed responder identity. | Native AARE omits those authentication fields and carries signed HandshakeAcc in user-information. Move native identity/configuration into session/profile inputs. |
| `mms_adapter.cpp`, `mms_rbac.cpp`, `mms_xacml.cpp` | Existing role enforcement and policy hooks are useful. | Preserve authorization semantics; only a validated native application identity may establish native privileges. Audit direct TLS/application binding versus any future routed deployment. |

These findings come from source inspection and historical evidence. No new device state was sampled. Later enrollment evidence records working dual-certificate identities, so the earlier audit's failed DSO association is not carried as a confirmed unresolved implementation defect.

## Proposed module layout

This is the target module layout under `apps/ccli/adapters/iec61850_mms/e2e/`. The generic BER reader, receive sequence gate, structural BER codec, crypto provider/key schedule, standalone identity validator, timestamp parser and association-request preflight are now started. Live association/transfer wiring, negotiated profile enforcement, rekey, stack bridge and audit integration remain open.

| Module | Responsibility |
|---|---|
| `e2e_types.hpp`, `e2e_profile.hpp` | Explicit modes, supported algorithms/OIDs, immutable peer/profile configuration, symbolic diagnostics and resource limits. |
| `e2e_codec.cpp/.hpp` | Bounded BER reader and structural decoders cover OSI association request/response/abort/release wrappers, presentation User-data, DataTransfer, clear transfer, HandshakeReq/ClearToken1, ClearToken3, Signature, Authenticator and signed response/reject/abort/release bodies. Definite-length encoders now cover Signature, HandshakeReq, signed response/error/release messages, OSI association/abort/release wrappers, ClearToken2, Authenticator, TBP, ClearTransfer, clear DataTransfer choice and presentation User-data. Continue with libiec61850 ACSE binding, supported BER constructed-string forms, profile validation and a reviewed independent byte corpus. See `WIRE_MAPPING.md`. |
| `e2e_crypto.cpp/.hpp` | A generic Mbed TLS provider supplies CSPRNG, SHA-256, HMAC-SHA-256 generation/constant-time verification, RFC 5869 HKDF-SHA-256, AES-128/256 GMAC, explicit RSA PKCS#1 v1.5/ECDSA SHA-256 signing and verification, and per-association ephemeral MODP group 14/ECDH group 23 plus optional group 28 (Brainpool P-256 per IEC 62351-4 §13.3.1.2) key agreement. It keeps private values in Mbed TLS contexts and validates peer public values. It does not interpret negotiated algorithm OIDs or normalize the DH secret for the IEC key schedule. |
| `e2e_key_schedule.cpp/.hpp` | Derives the initial and rekey transcript salts from the exact preserved TBS/TBP content slices, then derives two or four directional keys with SHA-256 HKDF, absent info, and the IEC client/server key ordering. It accepts profile-selected key lengths but does not map negotiated OIDs or normalize the provider's MODP secret representation. |
| `e2e_identity.cpp/.hpp` | Standalone Annex G leaf/profile, association subject, chain/time and CRL verification is implemented. CRL distribution-point/issuing-point scope is checked in code; dedicated scope fixtures still need execution. Native handshake ingestion, exact AP-title/AE-qualifier OID derivation, validated identity object for RBAC and signing-provider integration remain open. |
| `e2e_time.cpp/.hpp` | Bounded parser for the companion XSD GeneralizedTime profile, calendar validation, fractional precision and explicit UTC/numeric offsets; rejects ambiguous local time and emits canonical UTC `Z`. The transfer sequence window can take encoded wire time. Live ACSE/MMS use remains open. |
| `e2e_association.cpp/.hpp` | Incoming OSI association-request preflight now checks protected-protocol name, SHA-256 signature identifiers, token/signature agreement, timestamp, Annex G certificate path/CRLs and the exact signed TBS. The caller still supplies the AP-title/AE-derived identity OID and commissioned trust/CRL data. Per-session negotiation/state, signed responses/errors and lifecycle remain open. |
| `transfer_sequence.cpp/.hpp` | Per-direction timestamp/sequence receive gate is started. Add authenticated ClearTransfer ICV/nonce processing, no unverified dispatch and conditional EncrTransfer. |
| `e2e_rekey.cpp/.hpp` | Retained DH state, transcript salt, current/pending key epochs, reqRekey/changedKey and bounded transition handling. |
| `e2e_osi_bridge.cpp/.h` | Narrow C ABI to stack overlay for native AARQ/AARE/ABRT/RLRQ/RLRE/User-data plus lifecycle/tick/report callbacks. |
| `e2e_audit.cpp/.hpp` | Structured event and test-status integration with existing audit storage; no private/session key material in logs. |

Session state must be owned by each ISO connection, not by a single global AARE builder. Keep peer certificate, selected algorithm, transcript, sequence and key epoch associated with that session. Incoming data is bounded structurally before expensive operations, cryptographically validated before semantic effects, and passed to MMS only after all required checks pass.

Proposed integration contract: `create(profile, transport_identity)`, `receive_association(envelope)`, `receive_transfer(envelope)`, `protect_outgoing(mms_bytes, message_kind)`, `request_release()`, `tick(monotonic_time, utc_time)` and `close(reason)`. Results distinguish accept, protocol-layer response, security reject/abort, application rejection and terminal failure. Names are design proposals; freeze a typed interface after W0/W1 review.

## Work packages and exit criteria

### W0 - Authoritative wire/profile baseline and test readiness

1. Pin and reconcile the companion code component now stored at `apps/ccli/adapters/iec61850_mms/e2e/spec/iec62351-4-ed1.1/`. Its manifest identifies the normative full module as IEC 62351-4:2018 (2018-04-19), while the downloaded SEK archive is for the consolidated Ed. 1.1 publication. Confirm amendment applicability and any errata, then record chosen tags, identifiers and diagnostics with source/hash plus an independently decoded vector. The observed body/Annex A disagreement cannot be solved by copying one at random.
2. Freeze OSI server PICS/PIXIT, required algorithms, transfer mode, native enforcement policy, AP/AE identities, certificate/revocation policy, clock behavior, buffer limits and rekey interval. Propose 15 minutes for the explicit test configuration and the production policy chosen within 15 minutes to 24 hours.
3. Agree conditional/server-role test adaptations and expected diagnostics in SOURCE_ISSUES.md. Obtain complete missing pages where applicable.
4. Verify independent test peer capability and access. The repository already records TSP **4.7.4.5037** and includes its offline help; native E2E support/entitlement remains unconfirmed. The recorded 2026-10-01 trial expiry is historical, not proof of the current license state. A vendor client/library with native E2E enabled or the test body's qualified client is a viable route, subject to actual version/license/capability confirmation.
5. Evaluate stack route with an actual native exchange demonstration, maintenance/source access, build target, licensing, event/control/report callbacks and crypto feature support. Record the decision before large implementation investment.

Exit: reviewed baseline/profile, usable TEQ path, implementation-route decision and no unresolved wire-constant blocker. Requirements design, crypto/provider checks and harness scaffolding can begin while external inputs are obtained.

### W1 - Codec, OSI contexts and byte-level fixtures

Continue bounded types and BER codecs from the pinned components. Structural decoding now covers OSI association request/response/abort/release user-information, presentation User-data, DataTransfer choice, ClearTransfer, ClearToken2, Authenticator, HandshakeReq/ClearToken1, ClearToken3, Signature and signed response/error/release bodies; the implementation preserves exact TBP/TBS and application-data slices and derives ICV/signature inputs without the outer tag/length while retaining indefinite-form EOC. Definite-length encoders cover signed handshake/error/release messages, their OSI wrappers, the protected clear-transfer data path and presentation wrapper. The parser validates NMTOKEN UTF-8/ranges, rejects non-minimal non-negative BER INTEGERs, and bounds BER inputs to 1 MiB by default with configurable depth/element limits. Set a tighter profile-specific byte limit to match the delivered stack/PDU cap. Remaining W1 work includes libiec61850 ACSE binding, supported constructed-string BER forms, forbidden-ACSE-field checks, profile-independent field refinements and an independently decoded byte corpus. Then cover malformed nested lengths, absent/duplicate fields, integers/bitstrings/OIDs, explicit/implicit tags and maximum session payloads. Preserve exact tbs/tbp source slices for signature/ICV verification rather than decode/re-encode received data.

A host-only CTest target `e2e_codec_test` is registered as `e2e_codec`. It exercises BER fixtures for ClearToken2 and Authenticator, protected TBP byte preservation, transfer/presentation wrappers, signed release encoding, OSI release wrapping, BER size limits and transfer replay state. It was built and passed on Ubuntu WSL 2 with GCC 15.2 on 2026-10-21. The reproducible build commands from the repository root are:

```sh
cmake -S apps/ccli -B apps/ccli/build-e2e-host \
  -DCCLI_BUILD_TESTS=ON -DCCLI_WITH_LIBIEC61850=OFF \
  -DCCLI_WITH_LIB60870=OFF -DCCLI_WITH_GOOSE=OFF
cmake --build apps/ccli/build-e2e-host --target e2e_codec_test --parallel
ctest --test-dir apps/ccli/build-e2e-host -R '^e2e_codec$' --output-on-failure
```

Exit: positive/negative byte corpus decoded by an independent decoder; exact protected-byte slices verified; context/size boundary tests pass. No application dispatch occurs from an invalid envelope.

### W2 - Required crypto provider and known-answer vectors

Prove actual host and target crypto configurations support mandatory RSA/EC signatures and public-key curves, MODP group 14 and ECDH group 23, SHA-256/HKDF, and all activated ICV algorithms. Test DH public-value validation, key lengths, signature encoding, transcript salt, absent HKDF info and directional key ordering. Verify RFC 5869 primitive vectors separately from IEC-specific derivation vectors. Define a unique nonce strategy for each direction/key epoch and concurrent send path. Conditional CBC/AE functions are implemented only for selected supported profiles, with their full required algorithm sets.

The provider implementation wraps the bundled Mbed TLS operations for randomness, SHA-256, HMAC-SHA-256, HKDF-SHA-256, AES-128/256 GMAC, explicit RSA PKCS#1 v1.5/ECDSA SHA-256 operations, and ephemeral MODP group 14/ECDH group 23 plus optional group 28 key agreement. IEC 62351-4 maps group 28 to Brainpool P-256; the TLS NamedCurve registry must not be used to reinterpret this IEC group ID. The optional ECDH group use is distinct from the curve's public-key/signature support declaration. Private ephemeral values stay within Mbed TLS contexts. A new key-schedule module implements the printed §13.3.1.3–4 transcript salt formulas and two/four-key output ordering. Its current `h0=0` encoding is one zero octet; this and Mbed TLS's minimal-width MODP shared-secret encoding require independent peer/lab confirmation. The `e2e_crypto_test` target passed on Ubuntu WSL 2 with GCC 15.2 on 2026-10-21, covering provider primitives, RSA-2048/SHA-256, secp256r1 and Brainpool P-256 ECDSA/SHA-256 sign/verify, weak-key rejection, MODP/ECDH two-peer agreement, transcript salts, key splits and invalid peer values. Its temporary key/certificate fixtures are generated only under the ignored build directory. Validation exposed and fixed Mbed TLS's TLS ECPoint length-prefix conversion at the provider boundary; the IEC API continues to expose raw 65-byte SEC1 points. The key-schedule fixture now supplies a 32-byte transcript digest. Host results do not establish ARM64 capability. Negotiated algorithm OIDs, certificate path validation, peer interoperability and target provider capabilities remain open.

With the bundled Mbed TLS present, configure a fresh build from WSL/Linux at the repository root and run the crypto vector target:

```sh
cmake -S apps/ccli -B apps/ccli/build-e2e-crypto \
  -DCCLI_BUILD_TESTS=ON -DCCLI_WITH_LIBIEC61850=ON \
  -DCCLI_WITH_LIB60870=OFF -DCCLI_WITH_GOOSE=OFF
cmake --build apps/ccli/build-e2e-crypto --target e2e_crypto_test --parallel
ctest --test-dir apps/ccli/build-e2e-crypto -R '^e2e_crypto_provider_checks$' --output-on-failure
```

Exit: independently computed crypto vectors pass on host and ARM64; required provider features exist; no new cryptographic primitive is written from scratch.

### W3 - Native handshake, PKI, signed errors and termination

Implement per-session association state, peer addressing and native context enforcement. A standalone Annex G certificate validator checks X.509 v3, a non-negative DER serial, absent issuer/subject unique IDs, a single subject attribute of type 2.5.4.106 whose OBJECT IDENTIFIER value equals the caller-supplied association identity, mandatory `digitalSignature` and `keyAgreement`, RSA >=2048 or EC >=256 leaf keys, SHA-256-or-stronger certificate signatures, trusted path/time, CRL distribution-point coverage, issuer-matched CRLs and revocation status. The validator parses certificate CRL Distribution Points and CRL Issuing Distribution Point scope, checks names/issuer/certificate type/reasons, and fails closed for unsupported or incomplete scope. The bundled Mbed TLS 3.6 name parser accepts the standards-valid OID-valued subject attribute; the CRL distribution point is detected through the unsupported-extension callback because this Mbed TLS build does not otherwise expose it. The 2026-10-22 host build compiled this code, and current identity tests pass for missing CRL coverage and revoked peers. Add wrong-scope and malformed-scope fixtures: those cases are not present in the existing suite, so scope enforcement is not yet verified by a negative test.

`validate_handshake_request` now connects the structural OSI association decoder, strict profile signature selection, GeneralizedTime parsing, certificate-path extraction, Annex G certificate/CRL validation and exact-TBS signature verification. Next, derive the expected identity OID from the ACSE AP-title/AE-qualifier, load commissioned trust anchors/current CRLs into product configuration, and consume this preflight in per-session handshake/RBAC state. Negotiation, signed acceptance/application rejection/security rejection, incoming abort/release validation, signed abort/release responses, and retained session key state remain open. The validator currently uses the CRL path; OCSP retrieval/validation is not implemented.

Host fixtures exercise valid RSA-2048 and P-256 leaves, strict OID-valued subjects, wrong identity, text-valued subject rejection, weak RSA rejection, missing keyAgreement/CRL distribution point, missing CRL coverage and revoked peer rejection. Provision product fixtures for RSA and EC with separate TLS/application certificates and commissioned path/revocation data. Use a signing-provider abstraction so file-key and any required future TPM integration are explicit choices. Do not assume existing textual compatibility certificate binding is sufficient.

Exit: independent native client accepts normal handshake and signed release; all applicable Table 50 rows and supplementary signature/certificate/path failures produce settled exact responses/events; no unauthorized identity reaches MMS/RBAC.

### W4 - Protected traffic and rekey

Protect every ClearTransfer request/response/report using the selected ICV and key direction. Enforce negotiated ID/version, timestamp window, nondecreasing receive time, timestamp/sequence replay checks, sequence reset on timestamp change, and nonce rules. Keep receive checks and state commit atomic: rejected packets cannot advance state. Retain replay entries for the full period in which their signed timestamp could still be accepted; do not reuse the current 240-second compatibility cache. Define reboot/clock-step behavior and resource limits explicitly.

Implement both client-triggered and server-requested application rekey. Client carries fresh DH in a message protected by old keys; server changedKey acknowledgement is also under old keys; subsequent traffic switches at the prescribed directional boundary. Retain the server DH material as required, update transcript salt correctly and prevent overlapping unacknowledged renewals. Serialize rekey transitions with asynchronous report sends. Include disconnect, timeout, duplicate flags and invalid new public key paths.

The first timestamp prerequisite is now implemented in `e2e_time.cpp`: bounded parsing of the companion XSD's GeneralizedTime forms, calendar validation, fractional values, explicit UTC/numeric offsets and canonical UTC `Z` formatting. Ambiguous unzoned local times fail closed. `TransferSequenceWindow` accepts the encoded timestamp and applies its existing skew/replay gate. This API is not yet invoked on live MMS traffic, and precision/time behavior still needs a dedicated protocol fixture and independent peer confirmation.

Exit: integrity/nonce/replay/time/flag negatives pass; an independent peer exchanges uninterrupted traffic through both rekey triggers for actual configured 15-minute runs; tests prove old/new key epoch use, not merely continued TCP connectivity. Conditional EncrTransfer adds its complete positive and negative campaign.

### W5 - Product integration on every path

Connect the validated native application identity to the existing RBAC checks and audit events. In a native-required configuration, reject attempts to acquire application access through compatibility/TLS-only fallback. Define deliberate compatibility use in separate declared configurations. Check all MMS data paths: initialization, read/write, controls, datasets, report enable/reservations, unsolicited reports, errors, aborts and release. Verify outstanding message handling during rekey and teardown.

Cross-build with pinned SDK/sysroot/crypto configuration; record host/target hashes and dependencies. Verify time readiness, entropy, memory, maximum associations/PDU limits and CPU/latency on Tespro. Repeat existing identity/role boundaries through native sessions. Physical control, measurement correctness, firmware installation and broader security certification remain separate required product evidence where claimed.

Exit: no unprotected outgoing or unverified incoming MMS path; authorization boundaries and declared functional/performance behavior hold on target; recovery/clock/connection lifecycle tests have evidence.

### W6 - Independent verification and formal closure

Build `lab/e2e/` test equipment (proposed) with separate codec/vector fixtures, explicit malformed-message construction and deterministic scenario definitions keyed by the matrix IDs. Test construction must allow invalid messages; a normal client library may reject bad inputs before transmitting, so successful peer connection alone is insufficient. Use independent cryptographic/decoding implementations for expected values where feasible. Packet mutation occurs before protection is finalized when a valid outer signature/ICV is needed to isolate a deeper field check.

Run known-answer/unit/state-machine tests first, then host integration, parser fuzzing/memory-safety checks, independent interoperability and target campaigns. Capture expected versus observed diagnostic, required event, state effect and whether MMS application logic was reached. For tampering/replay tests verify that controls/reports/policy cannot be modified by rejected packets. Retest TLS and existing MMS/RBAC behavior appropriate to the integration changes.

Formal 100-4 conformance needs a test body with the technical/quality accreditation described in 5.2.1 and a qualified/homologated peer described in 5.3.3. Our own harness supports development and repeatability; it is not automatically accepted formal TEQ. Complete transport evidence before E2E assessment, and retain justified conditional exclusions rather than calling unexecuted cases passes.

Exit: every applicable matrix row maps to reviewed code and passing reproducible evidence; all source interpretations affecting tests are settled; PICS/PIXIT match the delivered build; independent assessment supports the exact claim.

## Ordered coding milestones

| Milestone | Concrete deliverable | Current audit state | Required evidence to proceed |
|---|---|---|---|
| M0 | Frozen profile/source/TEQ and route decision | OPEN — amendment applicability, wire ambiguities, final profile, peer access and stack route are not recorded as resolved. | W0 record, wire constants resolved, test access confirmed. |
| M1 | Host codec and crypto modules | PARTIAL — substantial source modules and historical host unit results exist; independent wire/key-schedule confirmation, remaining BER cases and fresh scope validation are open. | W1/W2 fixtures, independent vectors and memory limits. |
| M2 | Native server handshake plus protected read | NOT STARTED — request preflight is standalone; no live handshake or MMS dispatch integration. | Independent client accepts native handshake/read; changed byte rejected with required signed response/event. This is a milestone, not complete E2E closure. |
| M3 | Complete mandatory server engine | NOT STARTED — live transfer integrity, signed errors/release and both rekey directions are absent. | Signed errors/release, all activated ICVs, identity checks and both rekey directions; complete host negative suite. |
| M4 | Tespro native integration | NOT STARTED — no native E2E target evidence. | Target build/role/report/control/time/resource/recovery evidence, real rekey-duration runs. |
| M5 | Assessed release | NOT STARTED. | Applicable 100-3/100-4 evidence, final PID, installation verification and exact release hashes. |

No reliable calendar estimate is given before M0. The critical external dependencies are authoritative wire source, independent native peer and the agreed lab profile. Engineering can start on preparation and isolated modules without waiting for TSP support to be confirmed.

## Evidence schema and definition of closed

Each executed case records matrix ID, declared profile/condition, hardware and software/dependency hashes, configuration and SCL hashes, public certificate identifiers, clock state, algorithm/key lengths, sent/received bytes or controlled decoded trace, expected/actual response, audit event, application side effect, result and reproducible command/tool version. Keep private keys/session secrets out of shared evidence and release bundles.

Closure requires native association, protected traffic and real application rekey to work with an independent peer; mandatory negatives to give correct signed/OSI diagnostics and events; every activated crypto and certificate capability to have evidence; product behavior to survive integration; and the agreed independent assessment to cover the exact released software/hardware. A compatibility Connect, TLS packet capture or self-client/self-server success alone does not meet these criteria.

Any APK signing/USB bundle follows `apps/firmware-updater/docs/SIGNING_AND_USB_UPDATES.md`, local canonical keys/tools and manufacturer signature verification. A requested USB installation test is performed by USB. This document authorizes no key disclosure or substitute installation method.

## What has been completed and what happens next

Preparation is complete enough to continue coding: the standards/test requirements and companion code component are inventoried, test tables are dispositioned, and the main integration points and source questions are documented. The repository now has bounded BER and typed E2E codecs, crypto/key-schedule primitives, time parsing, sequence-window logic, Annex G certificate/CRL validation, and an association-request preflight. CMake includes these sources, but this confirms compilation-unit registration only; it does not show that the product uses E2E.

Next, close W0 decisions that affect interoperability while resolving Mbed TLS DH/key-schedule representation with an independent implementation. Add/run the outstanding parser, crypto and CRL-scope cases. Then build a per-connection native association bridge in libiec61850: derive peer identity from AP-title/AE-qualifier, load commissioned trust and revocation data, validate the signed request, emit the correct signed response/reject, and gate MMS dispatch on success. After handshake integration, implement protected transfer in every inbound/outbound MMS path, then both rekey directions and signed termination. Finish with independent-peer, host-negative, target Tespro and qualified 100-4 evidence.

Historical notes report codec/crypto/identity host tests passing on Ubuntu WSL 2 with GCC 15.2 on 2026-10-21. On 2026-10-22, CMake configured the host build, `ccli_adapters` and the three E2E test executables built, and all five registered `e2e_` CTest cases passed. The first build exposed a missing `e2e_time.cpp` source in the standalone codec-test target; that CMake link was fixed before the passing run. The identity suite does not yet cover wrong-scope or malformed-scope CRLs. These are host unit results only. ARM64 builds, native peer interoperability, Tespro E2E operation and conformance remain unverified. No claim of E2E conformance or closure is made.

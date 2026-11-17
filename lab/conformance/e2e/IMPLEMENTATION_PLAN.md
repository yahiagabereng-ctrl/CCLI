# Native E2E implementation and closure plan

**CEI applicability correction, 23 October:** application encryption capability is mandatory under CEI 57-142:2026-07, 5.1, pp. 47-48, even when an operating policy disables encryption. Implement and qualify AES-128/256-CBC and AES-128/256-GCM encrypted transfer; the current integrity-only baseline does not close this requirement. Separate attribute certificates remain conditional on RBAC Profile B because 5.3.2 permits Profile A or B. See [source review](APPLICABILITY_REVIEW.md).

Updated: 23 October 2026. Scope: CCI IEC 61850-8-1 OSI MMS server on Tespro, native integrity protection over mutual TLS. This is engineering status, not a conformance certificate.

> **Field/limit continuation, 23 October:** added an executable native field campaign for both RSA and EC, independent outgoing-message assertions, actual scoped C++ allocation failures and exact CONNECT/MMS/BER boundaries. It found and fixed the complete CONNECT payload limit. The [execution report](../../evidence/e2e-field-campaign-2026-10-23/RESULTS.md) and [695-row evidence ledger](../../evidence/e2e-field-campaign-2026-10-23/complete-test-evidence-ledger.csv) link every mapped row to evidence or a conditional disposition. There are 587 partially evidenced rows and 108 conditional dispositions; none are marked fully complete. Host and target verification are recorded there.

## What works now

Native security is connected through the actual TLS, OSI, presentation and MMS stack. The product `MmsAdapter` establishes native identities through the commissioned Profile A policy and the actual TLS certificate binding. This supersedes the earlier account of six disconnected partial components.

The host campaign covers codec, crypto, identity and actual native stack behavior. It exercises RSA/group 14 and ECDSA/group 23 with HMAC-SHA256, AES-128-GMAC and AES-256-GMAC; actual reads; protected asynchronous reports; client-driven renewal with old/new key transitions; signed release; signed failure responses; and commissioned product authorization. The new abort/resilience campaign adds outer ACSE/provider and OSI classification, signed application rejection/abort, incoming client abort, authentication precedence, replay and sequence boundaries, concurrent connections, report disconnects, resource exhaustion and ignored renewal. Invalid requests must not add application read-access calls. See the separate abort/resilience evidence for executed results and limits.

| Workstream | Implemented | Still required |
|---|---|---|
| W0: profile/source | Downloaded normative ASN.1/XSD; source hashes, requirements and table map; chosen integrity-only TLS server profile. | Lab-confirmed applicability, PICS/PIXIT, S02 diagnostics/tags, S13 seed/MODP conventions and accepted external TEQ. |
| W1: wire/OSI | Bounded codec; exact received signed/integrity slices; actual MMS open types; native contexts; extended session lengths; deterministic stack overlay; provider/user abort and AARE rejection classification; native PDV validation before legacy PCI truncation; MMS size negotiation bounded by native buffers; constructed-string BER support and live constructed handshake/transfer/rekey cases. | Remaining per-row BER/native variants, intermediate-chain/conditional TX presence, combined envelope/resource limits and external confirmation of response mapping. Exact CONNECT, 64,000-byte PDU boundaries and independent emitted-profile TX assertions are covered by the field campaign. |
| W2: crypto | Mbed TLS RSA/ECDSA, HMAC/HKDF, GMAC both key sizes; groups 14/23 and optional 28; Brainpool primitive capability; secrets cleared. | External key-schedule agreement, remaining parameter/curve live cases and target runtime evidence. |
| W3: association/identity | Signature-first preflight; strict OID-valued AP/AE subject; chain/time/CRL scope; signed accept/reject and invalid-initialization ApplicationReject; incoming signed/client provider/OSI abort handling; commissioned role/TLS binding. | Intermediate-chain/lifecycle campaigns, complete applicable field matrix and real target credentials. |
| W4: transfer/rekey | All native MMS including reports; ICV before token semantics; bounded timestamp/sequence replay history; retained server DH; directional epochs; ignored-renewal expiry cleanup; concurrent association and report-disconnect campaigns. Earlier real 900s GMAC256 renewal evidence retained. | Remaining race/resource and per-field variants beyond the recorded allocation sweep, real-time renewal for other declared profiles and actual device runtime. |
| W5: product/build | Explicit native configuration and fail-closed prerequisites; security/audit callbacks; signed release/security abort; synchronous MMS callback API for signed ApplAbort with diagnostic; host and ARM64 helpers; pinned IEC104 and dependency guard. | Full service coverage, application-specific abort policy, Tespro installation/runtime and independent diagnostic qualification. |
| W6: evidence | Reproducible repository peer and tests, separate crypto implementation, real socket exchanges, denied product operations. | Qualified external peer, transport qualification, full applicable 100-4 matrix and evidence for released hardware/software. |

See [readiness/runbook](TESPRO_LAB_READINESS.md), [current abort/resilience evidence](../../evidence/e2e-abort-resilience-2026-10-23/RESULTS.md), [earlier integration and real-time renewal evidence](../../evidence/e2e-native-integration-2026-10-23/RESULTS.md), [requirements](REQUIREMENTS.md), [tracker](requirements.csv), [test families](test-cases.csv) and [source issues](SOURCE_ISSUES.md).

The BER implementation's current host evidence is in [BER resume results](../../evidence/e2e-ber-mapping-2026-10-23/RESUME_RESULTS.md). The single-octet sanitizer corpus and passing integration runs prove the recorded finite cases only; they do not close every mapping row or formal qualification requirement.

## Architecture and implementation ownership

`TLS → native OSI bridge → NativeServer → authenticated MMS → existing dispatcher/RBAC`

The outgoing path protects synchronous responses and asynchronous reports. Each ISO connection owns its peer identity, DH, transcript, replay state and directional key epochs. The installed bridge belongs to one configured server and is removed after connections stop.

- `e2e_codec`, `ber_codec`, `e2e_wire`: bounded BER, native mapping and exact protected bytes.
- `e2e_association`, `e2e_identity`: raw signature-first request validation, strict Annex G certificate and applicable CRL checks.
- `e2e_crypto`, `e2e_key_schedule`: primitives, retained-server renewal and directional HKDF keys.
- `e2e_time`, `transfer_sequence`: protocol UTC syntax/format and authenticated timestamp/sequence state. GNSS/chrony remain the system time source.
- `e2e_server`, `e2e_osi_bridge.h`: negotiation, per-connection state, transfer, rekey, signed control and C interface.
- `scripts/generate_mms_e2e.py`: maintained libiec61850 overlay for receive/send/tick/cleanup and presentation/session mapping.
- `mms_adapter.cpp`, `core/config`: configuration, policy, actual TLS/application identity binding and audit integration.
- `tools/mms-client/e2e_probe.py`: Python/OpenSSL development peer. It is project-written, not qualified external TEQ; the existing GUI client remains a compatibility client.

## Next implementation steps

**Credential continuation paused, 26 October:** the user confirmed EJBCA and deferred issuance. No new certificates, CA/profile changes or device configuration were made. The [EJBCA native credential handoff](EJBCA_NATIVE_E2E_HANDOFF.md) contains the exact findings and resume sequence: restore the existing CA service, confirm identities/trust, build strict OID-valued CSRs, issue through a dedicated profile, validate CRL/identity coverage, then prepare owner-signed bindings and a live Tespro probe run.

### 1. Qualify the implemented error and resilience behavior

The requested implementation connects outer ACSE/presentation/provider responses separately from signed security rejection and abort, omits security diagnostics on alarm paths, and preserves authentication-before-semantics. MMS services can request signed ApplAbort through `CcliE2e_requestApplicationAbort` during their synchronous callback; the API queues the result until the callback returns and preserves the optional MMS diagnostic. It does not automatically convert ordinary MMS service errors into association aborts.

`e2e_native_resilience` covers signed handshake/control faults, actual invalid MMS initialization, client aborts, bounded malformed presentation/transfer messages, replay/sequence behavior, concurrent rekey, report disconnects, reduced-limit replay-cache exhaustion and accelerated monotonic epoch expiry. The trusted C++ test clock leaves production wall-clock checks and the 900-second minimum unchanged. The replay cache defaults to 65,536 authenticated pairs per association within the ten-minute freshness window; exhaustion closes the association. The harness selects 64 to reach that boundary quickly. Extend the remaining applicable Table 50/51 and FIELD matrix, constructed-string families and maximum-size corpus, then qualify externally. Wire interpretation issues remain explicitly open.

### 2. Complete real-time renewal evidence

The earlier separate `--timed-rekey` campaign passed on 23 October with a real 900-second epoch: reqRekey at 840.1s, new keys acknowledged at 850.2s, protected reads beyond the original epoch and signed release. The resilience campaign additionally uses an injected monotonic clock to exercise renewal and ignored-request expiry for HMAC and both GMAC sizes. Repeat real-time campaigns on the current candidate, Tespro and other declared timed profiles. Production minimums remain unchanged.

### 3. Prepare the Tespro candidate

Use the canonical compile-only helper with actual Modbus, MMS/GOOSE and IEC104 dependencies. Candidate builds now reject missing requested protocol dependencies. Before packaging, complete the source-baseline review, preserve current device configuration, commission separate native application credentials/CRLs and owner-signed role/TLS bindings, and select a new coordinated release number.

Use only `apps/firmware-updater` for APK signing and USB bundling, with local keys/tools and manufacturer signature verification. Enter the operator passphrase only at its visible local prompt. The current native revision has not been installed; the prior r64 installation is separate historical evidence.

### 4. Device and independent qualification

On a controlled Tespro bench, verify all ICVs, real measurements/reports, safe authorized control/readback, denied operations, tamper/replay, signed release/reconnect, real timed renewal, CRL/time failures and reboot. Capture firmware/configuration/public-certificate hashes, logs and packets without private/session keys.

Confirm an external vendor peer or lab TEQ, S02/S13, the applicable CEI/IEC editions, PICS/PIXIT and TLS profile. Run transport qualification before the agreed E2E procedures. Application encryption is unimplemented and is a mandatory CEI capability gap; add both CBC and GCM key sizes with negotiation, encrypted transfer and relevant tests. Separate attribute certificates are unsupported and may be excluded only through the allowed Profile A RBAC choice; intermediate routing is unsupported. Native E2E support in the installed TSP version remains unconfirmed.

## How to read progress

The 108 groups are formal evidence obligations, not 108 source files or a percentage complete. The tracker keeps formal OPEN until complete applicable evidence is accepted, while recording integrated implementation and host coverage separately. A passing project peer does not close an external interoperability or accredited assessment requirement.

The [22 October handoff](../../../HANDOFF_E2E_2026-10-22.md) and [archived earlier plan](IMPLEMENTATION_PLAN_PRE_INTEGRATION_2026-10-22.md) preserve history. Their pending-test and missing-IEC104 facts are superseded only where the current evidence records a verified result.


Follow-up parsing fixes and current target build evidence: [review results](../../evidence/e2e-ber-mapping-2026-10-23/REVIEW_FIX_RESULTS.md).

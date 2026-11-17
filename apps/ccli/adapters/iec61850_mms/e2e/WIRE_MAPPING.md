# IEC 62351-4 E2E BER wire mapping (draft)

> **Updated 23 October 2026:** native E2E is wired through TLS/OSI/MMS and commissioned RBAC; `ApplData` accepts actual MMS tagged open types. The BER sanitizer corpus passes 465,468 checks, native E2E CTests pass 8/8 and MMS regressions pass 5/5. Constructed/indefinite BER now has live handshake, transfer and rekey cases. Complete row-by-row coverage, device runtime and external qualification remain open; see [BER_FIELD_HANDOFF.md](../../../../../lab/conformance/e2e/BER_FIELD_HANDOFF.md).

## Source and status

This map is transcribed from the IEC code component in [`spec/iec62351-4-ed1.1/IEC_62351-4_E2E-security.asn`](spec/iec62351-4-ed1.1/IEC_62351-4_E2E-security.asn) and the companion [`IEC_62351-4_OSI-Env.asn`](spec/iec62351-4-ed1.1/IEC_62351-4_OSI-Env.asn). Their hashes and the original archive hash are recorded in [`source-manifest.json`](../../../../../lab/conformance/e2e/source-manifest.json).

The manifest describes this as a normative full code component for IEC 62351-4:2018, while the SEK listing supplied it with the consolidated Ed. 1.1 publication. Confirm Amendment 1 applicability and independently decode representative vectors before treating this as the final product wire map. The module's `IMPLICIT TAGS` setting determines the context-specific tags below.

## OSI presentation User-data

| ASN.1 field | BER tag / form | Notes |
|---|---|---|
| `User-data` | APPLICATION 1, constructed (`61`) | IMPLICIT `SEQUENCE SIZE (1)` |
| User-data list entry | UNIVERSAL 16, constructed (`30`) | Exactly one entry |
| `presentation-context-identifier` | UNIVERSAL 2, primitive (`02`) | INTEGER; positive value |
| `single-ASN1-type` | context [0], constructed (`A0`) | EXPLICIT `DataTransfer` |
| `clearTransfer` choice | context [0], constructed (`A0`) | IMPLICIT tag replaces the ClearTransfer SEQUENCE tag; fields follow directly, with no nested `30` |
| `encrTransfer` choice | context [1], constructed (`A1`) | IMPLICIT tag replaces the EncrTransfer SEQUENCE tag; fields follow directly, with no nested `30` |

## Association user-information

`OSI-AssoReq` and `OSI-AssoRsp` are `SEQUENCE SIZE (1) OF` an EXTERNAL-shaped value. Its ASN.1 tag is UNIVERSAL 8, constructed (`28`), followed by `indirect-reference` (INTEGER) and `single-ASN1-type` (context [0], EXPLICIT, `A0`). The request's explicit value is a universal-sequence `HandshakeReq`. The response's explicit value is the `HandshakeRsp` CHOICE: context [1] `HandshakeAcc`, [2] `ApplicationReject`, or [3] `HandshakeSecReject`. Those choice tags replace the respective SEQUENCE tags under IMPLICIT TAGS; the signed fields follow directly inside the context-tagged value.

`OSI-AssoAbr`, `OSI-AssoRelReq`, and `OSI-AssoRelRsp` use the same outer EXTERNAL wrapper. The abort value is a CHOICE: context [0] `HandshakeSecAbort`, [1] `DtSecAbort`, [2] `ApplAbort`, or primitive context [3] `OsiDiagnostics`. Release request and response carry universal-sequence `ReleaseReq` and `ReleaseRsp` values.

## ClearTransfer and ClearToken2

`ClearTransfer` is a universal constructed SEQUENCE containing `tbp` (SEQUENCE) followed by `auth` (SEQUENCE). `tbp` contains `token2` (SEQUENCE), then `applData` (the actual tagged MMS open type; no synthetic SEQUENCE wrapper), followed by any extension additions.

| `ClearToken2` field | BER tag / form | ASN.1 behavior |
|---|---|---|
| `version` | UNIVERSAL 3, primitive (`03`) or constructed (`23`) | BIT STRING default `{v1}`; may be omitted; constructed leaves use universal BIT STRING tag `03` |
| `assoID` | UNIVERSAL 12, primitive (`0C`) or constructed (`2C`) | UTF8String; 1–3 decimal digits, optionally followed by `:` and 1–3 digits; constructed leaves are universal OCTET STRINGs |
| `time` | UNIVERSAL 24, primitive (`18`) or constructed (`38`) | GeneralizedTime; constructed leaves are universal OCTET STRINGs |
| `seq` | UNIVERSAL 2, primitive (`02`) | non-negative SequenceNumber in this implementation |
| `iv` | context [0], primitive (`80`) or constructed (`A0`) | IMPLICIT OCTET STRING, exactly 16 logical bytes; conditional encryption field; constructed leaves use universal tag `04` |
| `rekey` | context [1], primitive (`81`) or constructed (`A1`) | IMPLICIT OCTET STRING public key; constructed leaves use universal tag `04` |
| `reqRekey` | context [2], primitive (`82`) | IMPLICIT BOOLEAN, default FALSE; may be omitted |
| `changedKey` | context [3], primitive (`83`) | IMPLICIT BOOLEAN, default FALSE; may be omitted |

Optional root fields remain in ASN.1 order. Unknown extension additions after the root fields are structurally skipped; they remain covered by the preserved original BER slices. A constructed form is accepted where BER allows constructed encoding of the underlying restricted string; the implicit context tag has its constructed bit set while each fragment uses the universal OCTET STRING tag.

## HandshakeReq and ClearToken1

`HandshakeReq` is a universal constructed SEQUENCE: `pp` (UTF8String/NMTOKEN), `tbs` (SEQUENCE), then `sign` (Signature SEQUENCE). The TBS contains `token1` (ClearToken1 SEQUENCE), optional `applData`, then extension additions. `Signature` contains `algo` (AlgorithmIdentifier SEQUENCE) and `sign` (OCTET STRING primitive `04` or constructed `24`, with universal OCTET STRING leaves). Signature input is the exact TBS contents without its start tag/length; an indefinite-length TBS retains its EOC bytes.

| `ClearToken1` field | BER tag / form | Notes |
|---|---|---|
| `sigAlg` | UNIVERSAL 16, constructed (`30`) | AlgorithmIdentifier |
| `pkCert` | UNIVERSAL 4, primitive (`04`) or constructed (`24`) | OCTET STRING containing certificate DER; constructed segments are concatenated before strict DER certificate parsing |
| `certPath` | UNIVERSAL 16, constructed (`30`), optional | SEQUENCE OF certificate OCTET STRINGs |
| `version` | UNIVERSAL 3, primitive (`03`) or constructed (`23`), optional | BIT STRING default `{v1}`; constructed segments use universal BIT STRING tag `03` |
| `assoID` | UNIVERSAL 12, primitive (`0C`) or constructed (`2C`) | UTF8String/NMTOKEN; constructed segments use universal OCTET STRING tag `04` |
| `dHKey` | UNIVERSAL 16, constructed (`30`) | SEQUENCE of group INTEGER and public-key OCTET STRING; the public key may be constructed (`24`) |
| `hmac` | UNIVERSAL 16, constructed (`30`) | AlgorithmIdentifier |
| `time` | UNIVERSAL 24, primitive (`18`) or constructed (`38`) | GeneralizedTime; constructed segments use universal OCTET STRING tag `04` |
| `encr-mode` AEA alternative | UNIVERSAL 16, constructed (`30`) | SEQUENCE OF authenticated-encryption AlgorithmIdentifier |
| `encr-mode` non-AEA alternative | context [0], constructed (`A0`) | IMPLICIT SEQUENCE; optional encryption list is context [0] (`A0`), required ICV SET OF is context [1] (`A1`) |
| `confParams` | UNIVERSAL 3, primitive (`03`) or constructed (`23`) | BIT STRING: `tlsProtected` bit 0, `applProtected` bit 1, `vpn` bit 2; constructed segments use universal BIT STRING tag `03` |
| `attCert` | UNIVERSAL 4, primitive (`04`) or constructed (`24`), optional | OCTET STRING containing attribute certificate DER |

The current decoder structurally parses this token and retains its exact encoded view. Certificate path validation, DH group policy, profile algorithm selection, timestamp policy, and certificate semantics remain separate.

`ClearToken3` used in signed errors and release messages is a SEQUENCE containing `sigAlg`, optional defaulted `version`, optional `assoID`, `time`, `pkCert`, optional `certPath`, and extensions. The decoder preserves `assoID` verbatim because rejection cases may carry partial IDs. Signed response/reject/abort/release bodies all have a `tbs` SEQUENCE followed by `Signature`; their body-specific `ApplData` or ENUMERATED diagnostic is selected with `SignedMessageKind`.

## Authenticator and protected bytes

`Authenticator` is a universal constructed SEQUENCE with optional `nonce` (UNIVERSAL 4 OCTET STRING), required `algo` (UNIVERSAL 16 `AlgorithmIdentifier` SEQUENCE), and required `icv` (UNIVERSAL 4 OCTET STRING), then optional extension additions.

For ClearTransfer integrity verification, preserve both the full encoded `tbp` and its ICV input. IEC 62351-4 §13.2.2 excludes the `tbp` start tag and length from BER ICV input; when `tbp` uses indefinite length, its terminal end-of-contents octets remain in that input. `BerTlvView::header_size` and `ClearTransferView::icv_input` retain this distinction. Never decode and re-encode `tbp` before authentication.

## Implementation state — 23 October 2026

> Current BER representation changes pass the sanitizer corpus and native host integration: 8/8 E2E CTests and 5/5 MMS regressions. The map remains larger than the executed subset; the current-source ARM64 build passes using OPENWRT_SDK_HOME=/home/adminavz. Exact evidence and remaining cases are in [BER_FIELD_COVERAGE.md](../../../../../lab/conformance/e2e/BER_FIELD_COVERAGE.md) and [BER_FIELD_HANDOFF.md](../../../../../lab/conformance/e2e/BER_FIELD_HANDOFF.md).

The codec is now used by `e2e_server.cpp` through `e2e_osi_bridge.h` and the maintained `scripts/generate_mms_e2e.py` overlay. Actual mutual-TLS/OSI associations negotiate the native contexts, verify exact request signatures, enforce Annex G identity/CRL policy, authorize the application/TLS identity binding, generate signed AARE responses and gate MMS dispatch. All native MMS responses and asynchronous reports pass through the protection hook. Live HMAC-SHA256 and AES-128/256-GMAC, nonce/time/sequence checks, retained-server-DH rekey and signed release are exercised by the Python/OpenSSL development peer.

`ApplData` carries the actual MMS tagged PDU. Both initial and renewal key schedules are invoked on real messages. The server retains a per-association DH key for client renewal through explicit provider use; default repeated initial derivation remains rejected. Exact received TBS/TBP bytes are authenticated before application dispatch. Raw handshake and control signatures are checked before signed semantic validation.

The Annex G validator consumes the actual AP-title/AE-qualifier identity and commissioned trust/CRLs. Matching critical IDP scope, wrong URI, wrong certificate type, partial reasons and malformed scope fixtures have executed. A narrowly exposed Mbed TLS CRL parser permits native scope inspection without relaxing default/TLS CRL parsing; signatures are verified over unmodified bytes.

The abort/resilience implementation classifies malformed/non-kernel ACSE as provider ABRT without user-information, and outer title/context/version rejection as AARE without user-information and with the appropriate source diagnostic. OSI mapping failures use service-user ABRT with unsigned `OsiDiagnostics`; security failures use signed rejection/abort, with diagnostics omitted on alarm paths. Native presentation PDVs are validated before the legacy parser can truncate their PCI. ClearTransfer ICV is verified over the original TBP before typed token semantics. Rejected handshakes use the known client association ID without appending a server suffix.

`CcliE2e_requestApplicationAbort` permits a synchronous native MMS service callback to queue a signed ApplAbort with an optional encoded MMS diagnostic. It requires the current callback's security token and sends after callback return. Valid signed peer aborts and provider/OSI aborts close without an answer. MMS negotiated size is capped at the native buffer limit. Timer-stopped connections are reaped even without new socket input.

The host campaign proves project development interoperability, including protected reports, authenticated negative transfers, abort/resilience behavior and the recorded constructed/indefinite BER cases. It does not qualify an external TEQ or establish target runtime/conformance. Row-by-row malformed-field/size boundaries and independent qualification of the complete applicable response matrix remain open. Source S02 still requires external applicability/diagnostic review. S13 still requires external confirmation of the one-zero-octet seed and minimal-width MODP shared secret. Application encryption, attribute certificates and intermediate routing are not claimed.

Current plan: [IMPLEMENTATION_PLAN.md](../../../../../lab/conformance/e2e/IMPLEMENTATION_PLAN.md). Reproducible commands and limitations: [TESPRO_LAB_READINESS.md](../../../../../lab/conformance/e2e/TESPRO_LAB_READINESS.md).


Follow-up parsing fixes and current target build evidence: lab/evidence/e2e-ber-mapping-2026-10-23/REVIEW_FIX_RESULTS.md.

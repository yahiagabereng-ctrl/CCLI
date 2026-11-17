# BER and field coverage map

**CEI applicability overlay:** generic IEC conditional classifications do not waive the mandatory switchable encryption capability in CEI 57-142:2026-07, 5.1. Preserve encryption rows as capability gaps; Profile B-specific positive cases may be excluded under an explicit Profile A declaration. See [source review](APPLICABILITY_REVIEW.md).

Updated: 23 October 2026. Latest execution: [native field/limit campaign and per-row ledger](../../evidence/e2e-field-campaign-2026-10-23/RESULTS.md). **Mapping complete for the declared native OSI/MMS server profile; BER representation support has a passing host integration subset, while full implementation and qualification remain open.**

## 1. Scope and sources

This is the finite acceptance map for all E2E fields, containers, BER representations and implementation bounds in the selected profile. “Exhaustive” means every mapped field/family and boundary has a disposition; it does not mean enumeration of every possible message or proof against every future ASN.1 extension.

Sources:

- Pinned `spec/iec62351-4-ed1.1/IEC_62351-4_E2E-security.asn` and `IEC_62351-4_OSI-Env.asn`, relative to `apps/ccli/adapters/iec61850_mms/e2e/`. Their exact type/cardinality/default/extension declarations govern the schema map.
- IEC 62351-4:2018+AMD1:2020: clauses 10, 13, 14, 15 and Annex G. Local source inventory and hashes are in `source-manifest.json`.
- IEC TS 62351-100-4:2023, Tables 50–51 and 55–75; all 28 `E2E-FIELD-*` groups in `requirements.csv`.
- [ITU-T X.690 (2021)](https://www.itu.int/rec/T-REC-X.690-202102-I/en), especially 8.1, 8.3, 8.6–8.7, 8.9–8.10, 8.14–8.15, 8.19, 8.23 and 8.25. BER and DER are separate policies. Inner certificates remain DER even when their containing OCTET STRING is fragmented BER.

Selected: server endpoint, mutual TLS, integrity-only ClearTransfer, HMAC-SHA256 and AES-128/256-GMAC, signed abort and release, RSA/group 14 and EC/group 23 campaigns. Declared additional algorithm/curve capabilities need their own vectors. Conditional application encryption and attribute certificates are mapped below without claiming implementation. XMPP/XER, router behavior and the full MMS application ASN.1/service suite are outside this BER workstream; their exclusions are explicit, not silently passed.

The latest [combined test-evidence ledger](../../evidence/e2e-field-campaign-2026-10-23/complete-test-evidence-ledger.csv) links all 695 rows to test evidence or a conditional profile disposition. It records 587 partially evidenced rows, 108 conditional dispositions and no completed rows; applicable variants and qualification remain open.

Machine-readable companion: [ber-field-map.csv](ber-field-map.csv). Each row identifies its table/requirement, schema path, direction, presence, tests, expected disposition, owner and pending status. [BER_FIELD_HANDOFF.md](BER_FIELD_HANDOFF.md) records current implementation evidence and remaining work.

## 2. Case rules applied to every field

| Family | Required variants | Expected assertion |
|---|---|---|
| F01 presence | Present; missing required; omitted optional/default; explicit default; mandatory conditional present/absent under both conditions | Required absence rejects. Optional omission and explicit BER defaults work where allowed; profile requirements can strengthen ASN.1 OPTIONAL. |
| F02 tag/type | Correct class/number/construction; wrong class; wrong tag; primitive/constructed alternatives; high-tag form where legal; illegal high-tag alias | Accept legal alternatives for the ASN.1 type; reject invalid type/tag before using its value. |
| F03 order/count | Every adjacent root-field exchange; duplicated recognized field; extra closed-container field; empty/one/two list entries; list order; nested duplicate | SEQUENCE order and non-extensible cardinality enforced. SET OF order is not treated as SEQUENCE order. |
| F04 value/range | Empty, minimum, maximum, below/above; signed/minimal INTEGER; UTF-8/alphabet; named bits/padding; OID termination/minimal subidentifiers; algorithm parameters | Separate BER validity, ASN.1 constraints and negotiated-profile rules. No integer truncation or wrong diagnostic due to evaluation order. |
| F05 encoding | Definite short/long; permitted nonminimal length; indefinite constructed; mixed nested forms; fragmented strings; zero-length fragments; nested fragments | Preserve exact TBS/TBP bytes. Logical string values may be flattened only for interpretation/crypto APIs. |
| F06 framing | Truncate at every byte; trailing bytes; length under/overrun; missing/extra EOC; nested malformed value; extra outer element | Reject complete-frame violations; no out-of-bounds access, hang or dispatch. |
| F07 extensions | Unknown distinguishable additions; several additions; nested additions; root optional after extension; exact authenticated extension bytes; unknown CHOICE | Follow each container's actual extension marker. Unsupported security choices fail closed. An unknown extension is not automatically a duplicate merely because its tag resembles an earlier required component. |
| F08 authenticity | Valid authentication; bad signature/ICV alone; bad authentication plus semantic fault; equivalent re-encoding after signing; signature-algorithm mismatch | Signature/ICV checked over original bytes before protected semantics. Authentication failure takes precedence and follows the alarm/no-diagnostic path. |
| F09 resources/lifetime | Depth, element, byte and list bounds at N−1/N/N+1; failed decode/reuse; copy/move/reset result; input/flattened-buffer lifetime; allocation failure | Enforce documented bounds before dispatch. Constructed-value ownership survives copies and return from preflight. Primitive borrowed slices still require live input. |
| F10 state/direction | Wrong association/state/direction; before accept; after abort/release; old/new key; pending renewal; duplicate or concurrent terminal messages | No unauthorized application dispatch, stale token use, double release, key reuse or reopened association. |

Apply applicable F01–F10 variants to every row. A variant that cannot distinguish an optional open type from a legal extension gets a schema-specific assertion, not an invented universal rejection rule. Mutations that remain legal messages may be accepted. A mutation campaign must count accepted and rejected cases and check decoder error consistency; its purpose is not to reject every changed byte.

## 3. Complete SecPDU/container inventory

| Container | Fields and cardinality | Direction / table | Closed versus extensible |
|---|---|---|---|
| HandshakeReq | `pp`, `tbs`, `sign`; `tbs.token1`; optional `tbs.applData` strengthened to required MMS InitiateRequest | RX, 55 | Outer closed; TBS extensible |
| HandshakeAcc | `tbs`, `sign`; `tbs.token1`; optional `tbs.applData` strengthened to required MMS InitiateResponse | TX, 56 | Outer closed; TBS extensible |
| ApplicationReject | `tbs`, `sign`; `tbs.token3`, required MMS rejection diagnostic `tbs.applData` | TX, 57 | Outer closed; TBS extensible |
| HandshakeSecReject | `tbs`, `sign`; `tbs.token3`, optional `tbs.diag` | TX, 58 | Outer closed; TBS extensible |
| HandshakeSecAbort | `tbs`, `sign`; `tbs.token3`, optional `tbs.diag` | RX, 59; pending/accepted lifecycle must be dispositioned | Outer closed; TBS extensible; no server-generated capability claim |
| DtSecAbort | `tbs`, `sign`; `tbs.token3`, optional `tbs.diag` | RX/TX, 60 | Outer closed; TBS extensible |
| ApplAbort | `tbs`, `sign`; `tbs.token3`, optional MMS diagnostic `tbs.applData` | RX/TX, 61 | Outer closed; TBS extensible |
| ReleaseReq | `tbs`, `sign`; `tbs.token3` | RX, 66 | Outer closed; TBS extensible |
| ReleaseRsp | `tbs`, `sign`; `tbs.token3` | TX, 67 | Outer closed; TBS extensible |
| ClearTransfer | `tbp`, required `auth`; `tbp.token2`, required MMS `tbp.applData` | RX/TX, 62–63 | **Outer closed**, TBP extensible |
| EncrTransfer | `tbp`, conditional `auth`; `tbp.token2`, required `Encrypted-ApplData` | Conditional encryption, 64–65 | Outer closed, TBP extensible; unsupported selected-profile RX choice must reject |
| Signature | required `algo`, `sign` | All signed messages | Extensible |
| Authenticator | optional `nonce`, required `algo`, `icv` | ClearTransfer; conditional EncrTransfer | Extensible; nonce strengthened to required for GMAC, absent for selected HMAC profile |
| AlgorithmIdentifier | required `algorithm` OID; conditional/optional `parameters` | Every signature/HMAC/ICV/encryption algorithm occurrence | Extensible; parameter policy is algorithm-specific |
| DiffieHellmanSet | required INTEGER `groupId`, nonempty OCTET STRING `dhPublicKey` | ClearToken1 | Extensible |
| CertPath | SEQUENCE SIZE(1..MAX) OF `pkCert` | ClearToken1 and ClearToken3 when applicable | List, not arbitrary extra components |
| Encryption mode | CHOICE `aea`, `non-aea`, future alternatives; `aea` SEQUENCE SIZE(1..MAX) OF algorithms | ClearToken1 | CHOICE extensible; unsupported modes reject according to negotiated policy |
| non-aea | optional `[0] encr` SEQUENCE OF algorithms; required `[1] icvAlgID` SET OF algorithms, both nonempty when present | ClearToken1 | Sequence itself has no extension marker |

### Shared token fields — expand at every container occurrence

| Field | ASN.1/tag and constraints | Complete value/state cases |
|---|---|---|
| ClearToken1.sigAlg | AlgorithmIdentifier SEQUENCE | Supported/unsupported OID; RSA NULL/absent policy; ECDSA parameter policy; BER-equivalent identifiers; token/outer mismatch; group/key/signature consistency. |
| ClearToken1.pkCert | OCTET STRING containing DER certificate | Primitive/fragmented wrapper; empty/truncated/malformed DER; TLS certificate reused; wrong issuer/identity/usage/key size/validity; revoked/no covering CRL/wrong scope; unsupported critical extension; DER remains strict. |
| ClearToken1.certPath | Optional CertPath before version | Omit for direct trust; required intermediate absent; correct order; reversed/unrelated/duplicate/cyclic/truncated/over-limit paths; primitive/fragmented certificate wrappers; empty path invalid. |
| ClearToken1.version | BIT STRING DEFAULT `{v1}` | Omitted/explicit; primitive/constructed; equivalent trailing zero named bits; BER unused-padding values; unknown bit/version; invalid unused count; non-final partial fragment. |
| ClearToken1.assoID | AssoID/NMTOKEN, 1–3 decimal digits, optionally colon plus 1–3 digits in the shared ASN.1 type | Client request must use client portion only; server acceptance must compose matching client/server portions. Empty, separators, overlength, non-digits, malformed UTF-8 and cross-association substitutions. |
| ClearToken1.dHKey.groupId | INTEGER interpreted as supported group | Supported mandatory and declared groups; negative/nonminimal/overflow/unsupported; mismatched key length or group. |
| ClearToken1.dHKey.dhPublicKey | Nonempty OCTET STRING | Primitive/nested fragments; empty; finite-field 0/1/p−1/p/outside subgroup or range; wrong width/leading octets under settled S13 convention; invalid EC point/infinity/curve/format. |
| ClearToken1.hmac | AlgorithmIdentifier | HMAC-SHA256 valid; wrong/missing OID; malformed or unsupported parameters; unsupported offered derivation algorithm. |
| ClearToken1.time | GeneralizedTime | Syntax/calendar/offset/fraction/precision and ±600-second boundaries; missing timezone; malformed length; current request/response time relationship; replayed timestamp+ID. Keep S03 time-fixture interpretation explicit. |
| ClearToken1.encr-mode | CHOICE described above | Every supported mode; AE rejected when unsupported; application encryption prohibited/required; empty lists; no common algorithm; first supported offer/selection membership; SET permutations; duplicate offers; single selected algorithm in responses; illegal extra alternative. |
| ClearToken1.confParams | Named BIT STRING: TLS, application, VPN bits | Actual transport agrees with TLS flag; application flag agrees with mode/policy; VPN unsupported; contradictory flags; primitive/constructed/default padding; unknown named bits require explicit policy. |
| ClearToken1.attCert | Optional OCTET STRING, unsupported selected feature | Absence succeeds; presence gets explicit unsupported-feature rejection. Positive attribute-certificate validation remains conditional, not silently passed. |
| ClearToken2.version | BIT STRING DEFAULT `{v1}` | Same BER representation matrix as token1; negotiated-version semantics. |
| ClearToken2.assoID | AssoID | Full accepted association ID required; missing/partial/wrong/oversized/malformed ID; alarm and no cross-association dispatch. |
| ClearToken2.time | GeneralizedTime | Exact freshness endpoints; just outside each endpoint; same/regressed/new timestamp; equivalent encodings of the same instant; UTC jumps and precision/rounding. |
| ClearToken2.seq | INTEGER mapped to bounded nonnegative counter | 0, 1, 127, 128, 2^63−1, 2^63, 2^64−1; negative/nonminimal/overflow; first-zero; same-time exact +1; new-time reset; wrap/replay/gap. Counter width is an implementation bound, not an invented ASN.1 SIZE. |
| ClearToken2.iv | Optional `[0]` IMPLICIT OCTET STRING SIZE(16) | Primitive/constructed root with universal OCTET STRING segments; 0/15/16/17 bytes; absent in clear and non-CBC modes; mandatory/unique for selected CBC. |
| ClearToken2.rekey | Optional `[1]` IMPLICIT nonempty OCTET STRING | Omitted/valid/empty/malformed; retained server DH; expected curve/group; overlap; old/new directional epoch transitions; implicit fragmentation. |
| ClearToken2.reqRekey | `[2]` BOOLEAN DEFAULT FALSE | Omitted/explicit FALSE; every nonzero one-octet TRUE; wrong length/constructed/indefinite primitive; server-only direction; request/ignore/deadline behavior. |
| ClearToken2.changedKey | `[3]` BOOLEAN DEFAULT FALSE | Same BER boolean cases; server-only direction; old-key authenticated acknowledgement; unexpected/repeated/missing transition flag. |
| ClearToken3.sigAlg | AlgorithmIdentifier | Supported algorithm and equality to outer Signature.algo; verify raw signature before protected semantics. |
| ClearToken3.version | BIT STRING DEFAULT `{v1}` | All token1 version encoding variants; negotiated version. |
| ClearToken3.assoID | Optional AssoID | Error context permits known partial/client-only ID or omission when unknown; established control requires exact full ID; invalid UTF-8 versus deliberately preserved offending ID must be separately dispositioned. |
| ClearToken3.time | GeneralizedTime | Same parser/freshness matrix; certificate validity/revocation rechecked; signed termination with stale/future/malformed time. |
| ClearToken3.pkCert | OCTET STRING containing DER | Must identify the established peer for control; malformed/replaced/wrong-identity/revoked/time-expired certificate; fragmented wrapper ownership. |
| ClearToken3.certPath | Optional CertPath | Supplied path validated; cached path used only under the permitted omission rule; malformed/unrelated/reordered/expired/revoked intermediate, and path changes during association. |
| Signature.algo / Signature.sign | AlgorithmIdentifier / nonempty OCTET STRING | Missing/duplicate/order faults; valid RSA/ECDSA and declared curves; malformed signature; wrong key; bit flips; OCTET STRING fragmentation; BER-equivalent wrapper changes without changing signature value; all message kinds. |
| Authenticator.nonce | Optional OCTET STRING | HMAC absence; GMAC required; missing/empty/1/8/12/16/128/256/257 byte values against documented provider/profile bounds; valid non-12 sizes; same-key reuse; different-key reuse; fragmentation; direction/epoch isolation. Do not mistake a peer library's nonce restriction for a protocol rule. |
| Authenticator.algo | AlgorithmIdentifier | Negotiated algorithm/key/tag size; unknown OID; swapped HMAC/GMAC/key size; missing/extra/malformed/default parameters; parameter-bearing BER variants and provider capability. |
| Authenticator.icv | OCTET STRING | Exact HMAC/GMAC negotiated length; empty/short/long; tamper; wrong key/direction/epoch; fragmented logical value; original TBP MAC input preserved. |
| applData / Encrypted-ApplData | Protected application open type / encrypted OCTET STRING | Exactly the actual tagged MMS PDU; no synthetic wrapper. Correct initiation/error/read/write/report/control/release-related diagnostics; malformed or wrong PDU type, duplicate application root, absent required data, negotiated/native size boundary. Conditional ciphertext requires correct mode, padding/AAD/tag before MMS dispatch. |
| HandshakeDiagnostics / DtDiagnostics | Extensible ENUMERATED | Required symbolic condition, canonical signed INTEGER representation, alarm-driven omission versus present zero, unknown future value/negative/overflow policy; retain S02/S04/S05 gates. |

Certificate DER/path/CRL tests are reused from identity coverage; they are still dependencies of each certificate-bearing field. Full unrelated certificate standards are not relabelled as BER-only tests.

## 4. Complete OSI/ACSE/presentation mapping

| Layer/type | Fields / alternatives | Mandatory coverage and result class |
|---|---|---|
| OSI-AssoReq/Rsp/Abr/RelReq/RelRsp | One outer SEQUENCE OF; one UNIVERSAL 8 constructed entry; INTEGER `indirect-reference`; `[0] EXPLICIT single-ASN1-type` | Empty/two entries; wrong/duplicate/order/class/tag; missing/wide/nonminimal/negative reference; zero/two explicit values; negotiated E2E reference. Wrong reference → OSI invalid-indirect-reference; malformed mapping → OSI protocol-error. |
| HandshakeRsp | `[1] assoAccept`, `[2] applReject`, `[3] secReject`, extension | Correct implicit tag replacing inner SEQUENCE; wrong/double/universal-sequence wrapper; accepted result matches selected signed payload; application diagnostic required on ApplicationReject; unsupported future choice closed. |
| AssoAbort | `[0] handsSecAbort`, `[1] dtSecAbort`, `[2] applAbort`, `[3] osiDiag`, extension | Correct implicit signed choice versus primitive OSI enum; source/layer combination; peer abort closes without echo; no server-generated HandshakeSecAbort claim; malformed/future alternatives reject safely. |
| DataTransfer | `[0] clearTransfer`, `[1] encrTransfer`, extension | Correct implicit root; wrong nested universal SEQUENCE; illegal primitive/context/class; unsupported encrypted/future choice rejected before dispatch. |
| Presentation User-data | APPLICATION 1; one SEQUENCE entry; INTEGER PCI; `[0] EXPLICIT DataTransfer` | Missing/duplicate/wide/negative/nonminimal PCI; 261 must not truncate to 5; one value exactly; wrong negotiated context; raw validation before legacy parser; context/permitted state/type match. |
| Presentation CONNECT/CPA | Mode, selectors, context list, abstract/transfer syntaxes, user-data | Distinct odd identifiers; positive permutation of allowed IDs with consistently adapted peer; duplicate/zero/even/out-of-range/missing IDs; duplicate/missing/wrong ACSE/MMS/E2E abstract syntax and BER transfer syntax; unknown/extra context; CPA acceptance/results consistent with actual negotiation. Selector/mode limits remain transport-layer qualified. |
| Session CONNECT/ACCEPT | Version, duplex requirements, negotiated presentation user-data and length encodings | Extended user data; 254/255/256 transition; CONNECT total including presentation/cert/init at 10239/10240/10241 bytes; version/duplex mismatch; overflow/segmentation policy; inconsistent declared length. Distinguish session versus inner AARQ length. |
| Session data/finish/disconnect/abort | Session wrapper, length and presentation payload | Short/extended lengths, missing payload, malformed/trailing lengths, abort transport selector, fragmented COTP transport; full signed control payload must fit and transmit or fail closed. |
| Native AARQ | Type; optional/default protocol version; application context; called AP/AE; calling AP/AE; invocation IDs; implementation info; user-information | Correct order/cardinality/type; malformed BER/kernel → provider ABRT without user-info; supported/default version; well-formed wrong context/title/version → appropriate AARE reject/no user-info; calling identity bound to certificate. AP OID and AE integer bounds enforced without truncation. Omitted versus inherited titles require a recorded profile disposition. |
| Native AARQ forbidden components | Sender ACSE requirements; mechanism; calling authentication; application-context-name-list | Every forbidden component separately, plus combinations: no compatibility authentication acceptance on native context; provider failure before inappropriate security payload. |
| Native AARE | Type/version/context; result; source diagnostic and its user/provider CHOICE; responding AP/AE; optional invocation/implementation fields; user-information | Independently decode every emitted accept, security reject and application reject; verify result/source/payload consistency; AP/AE match application certificate; native forbidden responder authentication fields absent. Validate generated fields, not just accepted RX messages. |
| Native ABRT | Type; abort-source; optional abort-diagnostic; optional user-information | Provider source with no inappropriate user-info; user source plus exactly one correct OSI/security/application payload; abort-diagnostic omitted where required; bad source/duplicates/order; peer provider/OSI abort closes with no response. |
| RLRQ/RLRE | Type; optional reason; user-information; OSI signed ReleaseReq/ReleaseRsp | Permitted reason/presence per Tables 74/75 and native mapping; wrong/missing/duplicate payload; correct reference; signature/token validation; terminal socket closure; any queued report remains protected. |
| Optional invocation IDs / implementation information | AARQ/AARE AP/AE invocation IDs and implementation info | Explicit support/PID disposition. Safe bounded parse or standards-permitted rejection; omission accepted. “Not required” is not an assumption that every present component is malformed. Existing implementation-info handling needs review. |

The companion module does not redefine the entire presentation/session/ACSE ASN.1. Use the applicable native profile and inherited protocol specifications for those outer fields. Do not manufacture numeric tags or mandatory presence from OCR/table shorthand. Generated-wire capture plus an independent decoder is required for every TX subclass.

## 5. BER representation and bound inventory

| ID | Dimensions / exact boundaries | Owner / acceptance |
|---|---|---|
| B01 tags | All classes; low numbers; high-tag 31/127/128/16383/2^32−1; truncated/unterminated/leading-zero/high-tag alias/overflow; max-tag-octet limit −1/at/+1 | `ber_codec`; structural failure, not integer truncation. |
| B02 lengths | 0/1/2/126/127/128/129/254/255/256/1023/1024/64000/65535/65536; all representable long-length widths; leading zero length octets | BER accepts legal nonminimal length forms within encoded-byte cap. Reserved 0xff, overflow, truncated length/value reject. DER-only inner objects retain DER rules. |
| B03 indefinite/EOC | Every constructed container and string; mixed nesting; empty; EOC split/truncated/missing/extra; EOC in definite contents; indefinite primitive | Original encoded spans include enclosing EOC; logical value excludes EOC; signature/ICV convention explicitly adds/retains the correct EOC. |
| B04 string fragments | OCTET STRING, BIT STRING, UTF8String, GeneralizedTime; definite/indefinite; empty fragments; nested depth; implicit roots | Octet/character string fragments use universal OCTET STRING tags; bit fragments use universal BIT STRING. Wrong segment type rejects. Do not wrongly require UTF8String child tags. |
| B05 bit semantics | Unused count 0..7 and 8/255 invalid; empty; zero bits; final padding; non-final partial segment; named trailing zero equivalence | Keep BER padding semantics separate from DER canonical padding. Interpretation must not modify authenticated bytes. |
| B06 scalar types | INTEGER/ENUMERATED signed/minimal/overflow; BOOLEAN single octet and every nonzero TRUE; NULL empty primitive; OID minimal terminated base-128; UTF-8 bounds | Test decoder and raw OSI helper equally, including optional/default and implicit tags. Generic BER reader is not a complete ASN.1 type checker. |
| B07 depth/elements/bytes | Defaults depth 32, elements 4096, encoded bytes 1 MiB, tag octets 6; typed handshake depth 24/elements 2048; signature-algorithm preflight 1024 bytes; certPath preflight 32 certificates/256 elements; raw wire helper 128 fields per container; zero limits; exact global versus child counts | Root counts toward limits; no reset-based bypass via child readers; custom public `BerLimits` honored. Record actual selected limits in each test, rather than claiming one global limit. |
| B08 profile buffers | Native MMS 64000; dispatcher backing/compile cap 65535; native transfer envelope allowance +4096; I/O send/receive allocation; MMS proposed max below/equal/above | No negotiated max above actual serializer/protection capacity; valid largest response transmits; larger response/request rejects or aborts before unsafe copy; combined signatures/cert chains count too. |
| B09 association limits | CONNECT complete native user-data 10240; certificate/path counts and encoded byte bounds; active ID space; handshake replay table | Test N−1/N/N+1 including headers/chain/init. Do not equate the inner AARQ byte bound with complete session CONNECT bound. |
| B10 lifecycle/resources | 65536 default authenticated-pair cache and reduced 64 harness profile; GMAC nonce entries/size; pending keys; copied/moved decoded results | Cache exhaustion rejects before dispatch and reconnect recovers. Test actual memory envelope and allocation failure separately from count-only bounds. |
| B11 mutation corpus | Every single-byte substitution 0..255 at every byte of each seed; each truncation; appended bytes; deterministic multibyte/length/nesting mutations | Reproducible seeds, source hash, accepted/rejected counts, no unexpected errors, ASan/UBSan and timeout. Add coverage-guided fuzzing separately; finite corpus is not universal proof. |
| B12 compatibility | Native versus compatibility contexts; explicit mixed-mode setting; original TLS/RBAC sources and actual operations | Full MMS compatibility/TLS/RBAC regressions and source-baseline review after integration. BER changes must not alter default certificate DER validation or enable unsupported native context fallback. |

Startup/profile bounds also need N−1/N/N+1 mapping: rekey interval 900..86400 seconds; native maximum PDU 1024..64000; authenticated-pair limit 1..65536; per-peer composed server-ID space 1000; ten-minute handshake replay retention and the actual replay-table bound; per-file credential input 1 MiB. Read connection/send/receive limits from the selected build configuration and record them in PIXIT before generating those boundary cases.

### Algorithm-specific and diagnostic value coverage

For every AlgorithmIdentifier occurrence, test absent versus NULL versus structured parameters, extra/missing/wrong parameter components, definite/indefinite container forms, unsupported OIDs, and an unsupported offer before/after a supported offer. Handshake selection and transfer authenticator validation are different contexts. HMAC uses a 32-octet ICV. Candidate GMAC tag policy is 12..16 octets with default 12; cover 11/12/13/14/15/16/17, AES-128 versus AES-256, handshake parameters versus per-message nonce, and conflicting parameter encodings. Keep any parameter-format interpretation under S02 review. CBC/GCM parameters and AAD/padding are conditional on selecting application encryption.

RSA cases include every declared modulus size, wrong signature length, PKCS#1 v1.5 padding/DigestInfo/hash mismatch, and an otherwise valid signature from another key. ECDSA cases include both declared signature curves, DER r/s encoding, 0/1/order−1/order bounds, wrong curve/key and malformed signature integers. DH/EC public-key and initial/rekey shared-secret encodings remain subject to S13. Crypto capability tests do not replace field/negotiation tests.

The following is the **pinned component's candidate enum inventory**, not a resolution of conflicting publication values. Every value has an emitted-condition/received-value disposition; conditional positive cases stay conditional. Handshake errors use signed security rejection (or received client handshake abort); transfer errors use signed DtSecAbort; source/alarm precedence may require diag omission instead of the present value.

| Handshake value | Symbolic condition | Handshake value | Symbolic condition |
|---|---|---|---|
| 0 | no-reason-given; distinct from absence | 14 | encrypt-algorithms-not-supported |
| 1 | protocol-error | 15 | aea-algorithms-not-supported |
| 2 | protected-protocol-not-supported | 16 | invalid-attribute-certificate |
| 3 | invalid-signatureAlgorithm | 17 | encr-mode-aea-not-supported |
| 4 | unexpected-version | 18 | ae-is-required |
| 5 | hmac-algorithm-not-supported | 19 | altSig-mismatch |
| 6 | encryption-not-required | 20 | aea-select-but-encryp-not-supp |
| 7 | encryption-required | 21 | invalid-ae-algorithm |
| 8 | invalid-public-key-certificate | 22 | single-ae-algorithm-required |
| 9 | invalid-certification-path | 23 | ae-not-used |
| 10 | invalid-time-value | 24 | invalid-encryption-algorithm |
| 11 | dhGroup-not-supported | 25 | single-encrypt-algo-required |
| 12 | illegal-dhGroup-selected | 26 | single-icv-algo-required |
| 13 | icv-algorithms-not-supported | extension | Unknown future diagnostic: parse safely, retain original, close by message/state policy |

Transfer inventory: 0 no-reason-given; 1 protocol-error; 2 unexpected-version; 3 encryption-not-selected; 4 encryption-required; **5 unused/reserved**; 6 gmac-nonce-required; 7 invalid-time-value; 8 invalid-sequence-number; 9 aes-init-vector-required; 10 unexpected-dhPublicKey; 11 unexpected-changedCypher; extensible future values. Test signed/nonminimal/negative/overflow encodings and do not assign reserved 5 a meaning.

OSI inventory: 0 no-reason-given, 1 protocol-error, 2 invalid-indirect-reference, 3 invalid-pci, future extension. ACSE source/cause values are validated against the inherited ACSE specification and selected native mapping; security enum values must never be substituted for ACSE/OSI values.

## 6. Response/assertion contract

1. Outer malformed ACSE/kernel errors: required provider abort; no inappropriate security signature/diagnostic. Supported outer structures with unsupported context/title/version: specified AARE source/cause, no security user-information.
2. OSI mapping errors: user ABRT with `OsiDiagnostics` protocol-error, invalid-indirect-reference or invalid-pci, as applicable, without security signature or inappropriate ACSE abort-diagnostic.
3. Authenticated handshake semantics: signed HandshakeSecReject; bad MMS initialization: signed ApplicationReject with actual MMS diagnostic. Known client ID remains client-only until acceptance.
4. Authenticated established transfer/control semantics: signed security abort or application-requested ApplAbort as appropriate. Bad authentication, address mismatch and replay alarm paths omit security diag. Source conflicts S02/S04/S05 remain explicit.
5. Every terminal case: no unauthorized read/write/control dispatch; signature/ICV checked independently; exact expected source/choice/diagnostic and identity; socket closure; one token/key cleanup; healthy reconnect. A disconnect alone is not a passing negative test.
6. Generated TX cases: verify full structure and required/forbidden fields in addition to cryptographic verification. Test invalid TX using an independent decoder/assertion layer; do not feed malformed server output back into the server as a substitute for TX coverage.
7. Inject one fault at a time for exact diagnostics. Paired fault cases explicitly test precedence; they must not accidentally pass solely because signature/ICV was invalid.

## 7. Status, remaining gates and completion criteria

Mapping coverage includes every component of both pinned ASN.1 modules, all Tables 55–75, all 28 FIELD groups, and explicit conditional exclusions. The CSV expands shared structures at each message/OSI occurrence and records active negative tests even where positive conditional capability is unselected. The completeness generator confirms 695 expanded rows, 28/28 FIELD groups and all 21 subclass tables. Rows remain mapped obligations; this does not claim they have all been executed or formally passed.

### Executed BER subset (23 October 2026)

- `e2e_ber_fields`: passed under ASan/UBSan. 465,536 field/BER assertions, 27 seeds, 459,520 exhaustive single-octet substitutions; 206,314 accepted legal/ambiguous mutations, 253,206 rejected, zero failures. Legal extension-tag collisions are deliberately accepted when the selected grammar cannot distinguish them from unknown additions.
- `e2e_native_stack` and `e2e_native_resilience`: both pass. The positive native BER exchange exercises constructed handshake strings and signatures, indefinite protected containers, constructed transfer authenticator strings, and constructed rekey data end-to-end; malformed constructed-string and unsupported constructed-IV cases receive signed failure handling. The independent Python peer validates nested constructed BER children and EOC framing.
- Earlier full host E2E CTest: 8/8 passed; the field campaign adds a ninth test. Current verification is in the field/limit report. MMS regressions: 5/5 passed after refreshing the reviewed native E2E source fingerprint. Evidence: [resume results](../../evidence/e2e-ber-mapping-2026-10-23/RESUME_RESULTS.md).
- ARM64 target compile passes using OPENWRT_SDK_HOME=/home/adminavz. The SDK was present at its recorded location; the earlier absence claim was incorrect. No APK/device verification is claimed.

Before calling this workstream implemented/verified:

- Complete row-by-row native wire coverage for every active applicable obligation, both RX and independently decoded TX; extend constructed BER cases to every certificate/path/nonce/ICV/implicit-string family, combined envelope limits and remaining ACSE/session/presentation bounds (exact complete CONNECT and PDU limits are now exercised) and exact response assertions.
- Expand beyond single-byte mutations to lengths, nested structures, invalid context negotiation, remaining allocation/connection/resource paths and stateful sequences. The current campaign covers all six context-list orders at three identifier sets, scoped allocation failure positions and subsequent reconnect. Run sanitizers and keep reproducible corpus evidence.
- Finish the full-source consumer/lifetime review; refresh reviewed baseline fingerprints only after source review; keep the current host E2E/MMS pass separate from the passing ARM64 compile and still-missing target runtime evidence.
- Record independent vector/peer verification and resolve S02/S03/S04/S05/S06/S08/S11/S13 where relevant. Keep formal conformance and device runtime open until their own evidence is accepted.

An item is complete only with a concrete case ID, applicable preconditions, input/output evidence, asserted layer/diagnostic, implementation/source identity and result. A case excluded by feature/role has an explicit reason and is reactivated when that declaration changes.


Follow-up parsing fixes and current target build evidence: [review results](../../evidence/e2e-ber-mapping-2026-10-23/REVIEW_FIX_RESULTS.md).

# Native E2E requirements and test mapping

Date: 2026-10-21. Scope: CCI IEC 61850-8-1 OSI MMS server. This is a paraphrased engineering map, not a replacement for the source standards or a conformance claim.

See [implementation plan](../E2E_GAP_CLOSURE_PLAN.md), [source issues](SOURCE_ISSUES.md), [requirements CSV](requirements.csv), [test cases CSV](test-cases.csv), [all-table disposition CSV](table-coverage.csv) and [source hashes](source-manifest.json).

M = applicable mandatory obligation; C-* = required when its stated condition holds; proposed integrity-only baseline activates C-NONAE and C-GMAC. EXCLUDED means an explicit role/environment exclusion, not waived E2E support. Source m labels and profile applicability are kept separate. FIELD rows group repeated container/field checks and provide explicit source row ranges. Core requirements add normative obligations not exhausted by the short test tables.

108 mapped requirement groups; 44 abstract test entries; all 79 tables dispositioned. Counts are tracking units, not a count of IEC test passes. The original mapping was a source review. For implementation and executed host evidence as of 23 October 2026, see [the current plan](IMPLEMENTATION_PLAN.md), [tracker](requirements.csv) and [test results](../../evidence/e2e-native-integration-2026-10-23/RESULTS.md). Formal OPEN status is recorded separately from integrated code and host coverage.

## Test readiness and evidence

| ID | Source / printed pages | Applicability | Implementation requirement | Work / verification |
|---|---|---|---|---|
| E2E-PRE-001 | 1;5.2.1; pp. 9,14 | M | Perform transport-profile testing before E2E testing; use production-equivalent software and parameters. | W0: Separate 100-3 evidence and immutable tested build/configuration manifest. |
| E2E-PRE-002 | 5.1;5.3.3; pp. 14-16 | M | Provide a conforming/homologated test peer capable of valid and deliberately invalid messages; document any split across test tools. | W0/W6: Lab acceptance of TEQ capabilities; reproducible positive and negative exchanges. |
| E2E-PRE-003 | 5.2.2; pp. 14 | M | Run normal and resilience procedures at least once with a mandatory algorithm and mandatory key length; this sampling rule does not remove other mandatory implementation capabilities. | W6: Per-case algorithm/key-size record plus capability tests for every mandatory algorithm. |
| E2E-PRE-004 | 5.3.1-5.3.2; pp. 15 | M | Test within the actual protected application and provide the configured DUT, PID/PICS/PIXIT, instructions and ICD/IID. | W0: Readiness review using real MMS data points and SCL. |
| E2E-PRE-005 | 5.3.2 g-h; pp. 15 | M | Provide representative functional data points and human-readable verification of values/status changes. | W5/W6: MMS read, report and authorized control/readback; separate physical application tests. |
| E2E-PRE-006 | 5.3.3; pp. 15-16 | M | Customize and communicate the plan from PID; run ordered steps; record pass/fail or justified not-performed for each case; bind results to hardware/software versions. | W6: Signed-off applicability matrix and evidence manifest. |
| E2E-PRE-007 | 5.3.4;5.5;Tables 1-2; pp. 16-18 | M | Expose association, transfer, release, rekey, certificate-validation and security-failure outcomes; document observation/trigger methods and primary time source. | W5: Structured logs/status and filled PIXIT with observable outcomes. |
| E2E-PRE-008 | 5.4;6.1; pp. 16,22-25 | M | Prepare application-level PICS including OSI/native profile, base protocol, algorithms and supported subclasses. | W0: Dedicated 62351-4 PICS aligned with applicable 62351-6 declarations. |

## Profile and cryptographic declarations

| ID | Source / printed pages | Applicability | Implementation requirement | Work / verification |
|---|---|---|---|---|
| E2E-DECL-001 | Tables 5-8; pp. 23 | M | Declare IEC 61850-8-1 server and transport support; association conformance and OSI EnvPDU support. | W0/W1/W2: PICS base and transport evidence. |
| E2E-DECL-002 | Table 7 rows 2-3; pp. 23 | C-TERM | At least abort or release must be supported; proposed product supports both, including tested signed orderly release. | W0/W1/W2: Table 15.2 plus abort cases. |
| E2E-DECL-003 | Tables 9-10; pp. 23-24 | M | Declare each OSI and SecPDU subclass by actual direction/capability; blank declaration is not proof of optionality. | W0/W1/W2: Complete direction-aware support matrix. |
| E2E-DECL-004 | Table 11; pp. 24 | M | Declare non-AE for integrity-only ClearTransfer; declare AE only when application encryption is implemented. | W0/W1/W2: Native handshake negotiation and profile-policy checks. |
| E2E-DECL-005 | Table 12 rows 1-4; pp. 24-25 | M | Support RSA and EC public keys (secp256r1 and brainpoolP256r1) plus RSA/SHA-256 and ECDSA/SHA-256 signatures. | W0/W1/W2: Known-answer vectors and peer tests for both signature families and curves. |
| E2E-DECL-006 | Table 12 rows 5-6 C1; pp. 25 | C-ENC-NONAE | AES-128-CBC and AES-256-CBC become mandatory when encryption without AE is declared. | W0/W1/W2: Both algorithms, padding/IV handling and encrypted transfers. |
| E2E-DECL-007 | Table 12 rows 7-8 C2; pp. 25 | C-ENC-AE | AES-128-GCM and AES-256-GCM become mandatory when encryption with AE is declared. | W0/W1/W2: Both algorithms, nonce/AAD/tag verification. |
| E2E-DECL-008 | Table 12 rows 9-11 C3; pp. 25 | C-NONAE (true in proposed baseline) | Implement HMAC-SHA256 and AES-128/256-GCM as integrity algorithms (GMAC) when non-AE is declared, including integrity-only mode. | W0/W1/W2: All three ICV capabilities and negotiated on-wire transfers. |
| E2E-DECL-009 | Table 13; pp. 25 | M | Use the correct E2E module/object identifiers; keep module identifier, abstract syntax and MMS application context distinct. | W0/W1/W2: OID codec vectors and independent decoder. |

## Implementation obligations

| ID | Source / printed pages | Applicability | Implementation requirement | Work / verification |
|---|---|---|---|---|
| E2E-CORE-001 | 6;17 Tables 6-11; pp. 22-27,75-78 | M | Retain the declared TLS transport profile and its mandatory cipher capabilities; E2E cannot bypass transport authentication or revocation. See S12. | W0/W5: 100-3 regression and native TLS PICS review. |
| E2E-CORE-002 | 8.3-8.5;Annex G.4.3,G.4.7; pp. 30-31,103-105 | M | RSA keys at least 2048 bits, EC at least 256 bits; support required curves/signatures; SHA-256 or stronger certificate signature hash. | W2/W3: Small/invalid key and unsupported algorithm fixtures; crypto vectors. |
| E2E-CORE-003 | 8.6; pp. 31-32 (redline PDF 35-36) | C-ENC-NONAE | CBC IV is 16 octets, generated with a CSPRNG, different for each encrypted message and transmitted with it. See S01. | W2/W4: IV generation/length/absence/duplicate tests. |
| E2E-CORE-004 | 8.7;13.4.2; pp. 32,57 | C-GMAC (true in baseline) or C-ENC-AE | AES-GCM/GMAC nonce must differ for every use of a given key; honor nonce and tag-length parameters; 12-octet nonce is recommended, not an invented mandatory length. | W2/W4: Nonce uniqueness across concurrent traffic and key epochs; tag vectors. |
| E2E-CORE-005 | 10.1,10.3;G.3; pp. 33-34,103 | M | Bound CONNECT extended session data including presentation, ACSE, certificates and initialization to 10240 octets; support extended user data, session v2, duplex, no data overflow/segmentation and larger abort payloads. | W1/W5: Boundary/over-limit PDUs, extended-length/session vectors. |
| E2E-CORE-006 | 10.4.1-10.4.3;15.2.1; pp. 34-35,64 | M | Negotiate ACSE, protected MMS and E2E presentation contexts with distinct odd identifiers; BER transfer syntax; use fully encoded single-ASN1-type data and the negotiated E2E context. | W1/W5: Context permutations, wrong/missing/duplicate context and valid transfers. |
| E2E-CORE-007 | 10.5.2-10.5.3; pp. 36-37 | M | Use ACSE version 1 and form-2 OID AP titles/integer AE qualifiers; called/calling values identify peers; response titles if omitted inherit called values. | W1/W3: Address/OID/default-value vectors and independent association. |
| E2E-CORE-008 | 10.5.4;15.3; pp. 37,64-67 | M | Encode EXTERNAL and context references correctly; wrap native security in ACSE user-information with specified EXPLICIT single-ASN1-type mapping. | W1/W5: Byte vectors and peer-decoded request/response/reject/abort/release. |
| E2E-CORE-009 | 13.1.2; pp. 44 | M | Emit pure UTC GeneralizedTime with Z and no T separator; parse permitted precision and fractions rather than reusing compatibility timestamp formatting. See S03. | W1/W3: Calendar/timezone/fraction and malformed-time vectors. |
| E2E-CORE-010 | 13.1.3;13.1.4; pp. 44-46 | M | Validate HandshakeReq and generate HandshakeAcc with ClearToken1; protected protocol is IEC 61850-8-1; embed MMS Initiate-Request/Response. | W3/W5: Tables 15,55,56,68,69; no MMS dispatch before validation. |
| E2E-CORE-011 | 13.1.3 c;13.1.4 b;13.4.1; pp. 45-46,57 | M | Sign and verify the exact received tbs content octets excluding enclosing tag/length; preserve permitted indefinite-length trailing octets; match signature identifiers. See S02. | W1/W2/W3: Independent signature vectors and signed-byte mutation tests. |
| E2E-CORE-012 | 13.1.5; pp. 46 | M-MMS | Reject invalid MMS initialization with signed ApplicationReject and the relevant MMS diagnostic PrPDU. | W3/W5: Table 57 and AARE application-rejection case. |
| E2E-CORE-013 | 13.1.6-13.1.8;14.3.1;14.4.1; pp. 46-47,60,63 | M | Send signed HandshakeSecReject or DtSecAbort with ClearToken3 as appropriate; omit diag when an alarm event is raised; otherwise include the precise diagnostic. See S04. | W3/W4/W5: Tables 50,51,58,60,71 plus alarm/no-diag assertions. |
| E2E-CORE-014 | 13.1.7; pp. 47 | M-RX | Accept and validate the client HandshakeSecAbort mapping and terminate pending association; do not implement a server-generated client-only abort as a claimed capability. | W3/W5: Tables 59,70 received-client abort and cleanup. |
| E2E-CORE-015 | 13.1.9; pp. 47-48 | M | Wrap an MMS-requested abort as signed ApplAbort; preserve applicable application diagnostic and ClearToken3. | W3/W5: Table 61 and server OSI abort mapping. |
| E2E-CORE-016 | 13.1.10-13.1.11; pp. 48 | C-TERM (both selected) | Validate/generate signed ReleaseReq and ReleaseRsp; orderly release closes and cleans the matching association. | W3/W5: Tables 15.2,66,67,74,75; both initiators as declared. |
| E2E-CORE-017 | 13.2.2; pp. 49 | M | Protect ClearToken2 plus MMS application payload with negotiated ICV in both directions; verify before application action; include auth. See S02. | W4/W5: Tables 18,62,63,72; reads, responses, reports and controls. |
| E2E-CORE-018 | 13.2.2 b; pp. 49 | M | Compute ICV over exact tbp content excluding enclosing tag/length, with specified indefinite-length treatment. | W1/W2/W4: Independent MAC vectors and bit-level alterations. |
| E2E-CORE-019 | 13.2.3; pp. 49-50 | C-ENC | Protect encrypted application transfer with the required encryption boundaries and ICV rules; never dispatch failed authentication plaintext. See S01/S10. | W2/W4: Tables 21,23,54,64,65,73 after missing source recovery. |
| E2E-CORE-020 | 13.2.3 a-b; pp. 49-50 | C-ENC-AE | For AE, use encoded ClearToken2 including its BER tag as AAD; external auth is absent because tag is carried with ciphertext. | W2/W4: AAD mutation, tag failure and optional-field presence vectors. |
| E2E-CORE-021 | 13.2.3 b; pp. 50 | C-ENC-NONAE | For separate encryption/integrity, auth is present and ICV covers the post-encryption tbp value. | W2/W4: Encrypt-then-integrity vectors and field-presence checks. |
| E2E-CORE-022 | 13.3.1.1 a-c; pp. 50-51 | M | Carry supported signature algorithm, DER end-entity certificate and required ordered certification path; omit supplied/direct-trust path when specified. | W1/W3: Tables 55/56 cert/path and signature fields; chain omission/inclusion vectors. |
| E2E-CORE-023 | 13.3.1.1 d; pp. 51 | M | Support v1; server chooses exactly one supported offered version; do not accept unsolicited versions. | W3: Handshake version negotiation and Table 50.7. |
| E2E-CORE-024 | 13.3.1.1 e; pp. 51 | M | Client ID is 1-3 numeric characters; server appends its unique 1-3 numeric ID separated by colon; maintain unique live peer-pair association IDs. | W3: Grammar, collision/concurrency and full-ID propagation tests. |
| E2E-CORE-025 | 13.3.1.1 f;14.3.4 f-g; pp. 51,61 | M | Check handshake UTC against normative 10-minute limit and reject duplicate time/association-ID pairs with replay alarm. See S03/S04. | W3: Boundary/future/past/replay tests including acceptance-lifetime replay retention. |
| E2E-CORE-026 | 13.3.1.1 g-h;13.3.1.2; pp. 51-53 | M | Support RSA-associated DH group 14 and ECDH group 23 (secp256r1); optional ECDH group 28 uses brainpoolP256r1, while Brainpool public-key/signature capability remains a separate mandatory declaration. Preserve the selected group and validate public values. | W2/W3: DH/ECDH vectors, invalid points/subgroups and same-group checks. |
| E2E-CORE-027 | 13.3.1.2 b; pp. 53 | M | Generate fresh DH key pair for each association; retain server DH key pair/public peer state needed for renewal; isolate associations. | W2/W3: Freshness/concurrency and rekey vectors with no cross-session state. |
| E2E-CORE-028 | 13.3.1.1 i;13.3.1.4; pp. 51-54 | M | Negotiate non-AE/AE according to declared policy, choose first supported offered algorithm and exactly one result; set tlsProtected/applProtected consistently; forbid AE when application encryption is off. See S05. | W3: Ordered-preference and empty/multiple/unsupported selection tests. |
| E2E-CORE-029 | 13.3.1.3; pp. 53-54 | M | Derive directional keys with HKDF using agreed HMAC, DH secret and transcript-hash salt; info absent; split OKM in mandated client/server ICV and optional encryption order. See S02. | W2: Externally computed extract/expand/transcript/key-order vectors. |
| E2E-CORE-030 | 13.3.1.3; pp. 53-54 | M | Retain initial transcript salt and update it with the rekey-carrying tbp content; derive new directional keys from new client DH material. | W2/W4: Initial and multiple successive rekey transcript vectors. |
| E2E-CORE-031 | 13.3.1.1 k;14.3.4 aa; pp. 52,63 | C-ATTR | When attribute certificates are accepted, validate them and their role privileges; invalid attribute certificate must not establish authorization. | W3/W5: Attribute-cert validity and policy tests; otherwise explicit unsupported declaration. |
| E2E-CORE-032 | 13.3.2 a-c; pp. 55 | M | ClearToken2 uses negotiated version, full association ID and UTC generation time. | W4: Tables 62/63 field checks and cross-association substitutions. |
| E2E-CORE-033 | 13.3.2 d; pp. 55 | M | Sequence starts at zero for each timestamp value, increments by one for repeats of that value, and resets to zero when timestamp changes. See S06. | W4: Multiple same-time sends, next timestamp reset and invalid sequence tests. |
| E2E-CORE-034 | 13.3.2 e; pp. 55 | M/C-ENC | CBC IV only when encrypted CBC is selected; it is absent in ClearTransfer. | W4: Clear field absence and conditional CBC field checks. |
| E2E-CORE-035 | 13.3.2 f; pp. 55-56 | M | Application key refresh maximum interval is configurable from 15 minutes to 24 hours; enforce selected policy with continuous traffic. See S07. | W4: Real 15-minute triggered test and configuration-boundary checks. |
| E2E-CORE-036 | 13.3.2 f; pp. 55-56 | M | Only client supplies rekey DH public value; server retains DH private key; client replaces its DH key pair and both derive same new keys. | W2/W4: Client-driven rekey with independent client and invalid rekey public values. |
| E2E-CORE-037 | 13.3.2 f-h; pp. 55-56 | M | Rekey-carrying client message uses old keys; server changedKey response is protected using old keys; subsequent directions transition according to changedKey rules. See S07. | W4: Old/new epoch assertions, queued reports, in-flight traffic and rejection of stale-key traffic. |
| E2E-CORE-038 | 13.3.2 f-h; pp. 55-56 | M | Client must not send another rekey before changedKey acknowledgement; reqRekey and changedKey are server-only; server can request client renewal. | W4: Both trigger directions, duplicate/out-of-order flags and disconnect mid-rekey. |
| E2E-CORE-039 | 13.3.3; pp. 56 | M | ClearToken3 carries own signature algorithm, negotiated version, known/partial association ID, current UTC and DER certificate/path with specified conditional presence. | W3/W4: Tables 57-61,66-67 and partial-ID reject cases. |
| E2E-CORE-040 | 13.4.2; pp. 57 | M | Authenticator algorithm must equal negotiated ICV; nonce required for GMAC and absent otherwise; emit/verify actual ICV. See S02. | W2/W4: All negotiated ICVs, missing nonce and altered algorithm/MAC cases. |
| E2E-CORE-041 | 14.3.2; pp. 60-61 | M | Verify signature before semantic validation of signed data; mismatched algorithm identifiers or invalid signature raise alarm. See S04. | W1/W3: Parser-bound safety tests and signature-first rejection assertions. |
| E2E-CORE-042 | 14.3.3-14.3.4; pp. 61-63 | M | Validate protected protocol, certificate/path, sender AP/AE binding, version, time/replay, DH, HMAC, algorithm alternatives and confidentiality policy with specified failure handling. See S04/S05. | W3: Table 50 plus requirements and negative fixtures not enumerated there. |
| E2E-CORE-043 | 14.4.1-14.4.2; pp. 63 | M | Reject protocol/encryption-mode mismatch; altered ICV algorithm or invalid ICV raises alarm; missing GMAC nonce returns specified diagnostic. See S02/S04. | W4: Tables 51/54 plus explicit tamper, nonce and algorithm mismatch cases. |
| E2E-CORE-044 | 14.4.3 a-d; pp. 63 | M | Reject out-of-window or decreasing time, detect duplicate time/sequence as replay alarm, and enforce exact sequence/time reset rules. See S02/S03/S06. | W4: Boundary/replay/time reversal tests beyond Table 51 four rows. |
| E2E-CORE-045 | 14.4.3 e-i; pp. 63 | M/C-ENC | Reject missing CBC IV when required and invalid direction/repetition of rekey flags; server rejects changedKey from client and repeated unacknowledged rekey. See S02. | W4: Flag-direction and CBC-conditional fault cases. |
| E2E-CORE-046 | 15.2.2; pp. 64 | M | Native ACSE uses kernel only: omit mechanism-name, authentication-value and sender/responder ACSE requirements; carry security in user-information. | W1/W5: Native AARQ/AARE field absence and compatibility separation tests. |
| E2E-CORE-047 | 15.2.1-15.2.2; pp. 64 | M | Use native MMS application context e2eMMSAC and E2E abstract syntax; do not retain legacy basic MMS context on native path. | W1/W5: OID vectors and wrong-context rejection. |
| E2E-CORE-048 | 15.3.2-15.3.3; pp. 65-66 | M | Map handshake into AARQ/AARE; accepted, application-rejected and security-rejected outcomes have distinct payload choices and ACSE results. See S08. | W5: Tables 15,17,68,69 and each handshake outcome. |
| E2E-CORE-049 | 15.3.4;15.6; pp. 66-67,70-71 | M | Use ABRT mapping with proper user/provider source, optional user-information and diagnostics; distinguish OSI errors from security errors and application aborts. | W5: Tables 20,60,61,70,71; provider versus user abort vectors. |
| E2E-CORE-050 | 15.3.5-15.3.6; pp. 67 | C-TERM (both selected) | Wrap ReleaseReq/ReleaseRsp in RLRQ/RLRE with negotiated context and signed security payload; normal release response reason. | W5: Tables 74/75 plus signed release fields. |
| E2E-CORE-051 | 15.4.1-15.4.2; pp. 67-68 | M | Route every MMS transfer through native OSI presentation wrapper and security context, including asynchronous reports and error responses. | W5: Table 72; all synchronous/asynchronous send paths captured. |
| E2E-CORE-052 | 15.4.3; pp. 68 | C-ENC | Wrap EncrTransfer with the negotiated E2E presentation context. See S01. | W5: Table 73 and encrypted send/receive paths. |
| E2E-CORE-053 | 15.6.1; pp. 70 | M | Check ACSE structure/titles/kernel first; malformed outer ACSE uses specified provider abort or association rejection without inappropriate embedded security response. | W1/W5: Wrong context/title/forbidden field/outer malformed BER tests. |
| E2E-CORE-054 | 15.6.2-15.6.3; pp. 70-71 | M | Reject wrong association indirect-reference and wrong/missing transfer presentation context using correct OSI diagnostics and wrapper. | W5: Table 17.2;Table 20.1-2. |
| E2E-CORE-055 | 4.6;14.3;14.4; pp. 21,60-63 | M | Log required security violations and replay/signature/identity/ICV alarms in protected audit storage; expose test status without logging keys. | W5: Cause-specific event assertions plus PIXIT observation procedures. |
| E2E-CORE-056 | G.2; pp. 103 | M | Use separate TLS and application end-entity certificates; audit distinct key handling in provisioning. | W3/W5: TLS/application certificate fingerprints and provisioning checks. |
| E2E-CORE-057 | G.4.1-G.4.4; pp. 103-104 | M | Use v3 certificate, nonnegative serial, matching inner/outer issuer-signature algorithm and issuer DN unique within PKI; recommendations such as random serial are not converted into mandates. | W3: Annex G fixture audit and invalid certificate rejection. |
| E2E-CORE-058 | G.4.5-G.4.8; pp. 104-105 | M | Validate lifetime/subject/public-key profile and allowed key families/sizes; issuerUniqueID and subjectUniqueID absent; do not invent a fixed mandatory certificate lifetime. | W3: Valid/expired/not-yet-valid/wrong-curve/forbidden-ID fixtures. |
| E2E-CORE-059 | G.5.2; pp. 105 | M | Application certificate includes digitalSignature and keyAgreement key-usage bits. | W3: Missing/wrong keyUsage rejects at native handshake. |
| E2E-CORE-060 | G.5.3; pp. 105 | C-PKI | CRL distribution points required when using CRLs; AIA required when using OCSP; apply distribution-point issuer/reason conditions and actual revocation validation. | W3/W5: Revoked/intermediate-invalid/CRL-or-OCSP policy cases; lifecycle test. |
| E2E-CORE-061 | G.5.4; pp. 106 | C-ROLE | User-role extension only when privilege information is included; validate supported role representation against authorization policy. | W3/W5: Existing role matrix via validated native application identity. |
| E2E-CORE-062 | G.6.2;10.5.3; pp. 106,36-37 | M | OSI certificate subject is one objectIdentifier name component identifying AP-title plus AE-qualifier; parse proper ASN.1 OID value and enforce address binding. | W3/W5: Strict DER OID and wrong-address tests; no reliance on current textual-OID compatibility handler. |
| E2E-CORE-063 | 15.5; pp. 68-70 | C-ROUTER (excluded) | If the product acts as an intermediate router, preserve EnvPDUs and maintain routing/association state; current proposed scope is endpoint only. | W0: Explicit PID exclusion; add router requirements before claiming that role. |

## SecPDU and OSI field coverage

| ID | Source / printed pages | Applicability | Implementation requirement | Work / verification |
|---|---|---|---|---|
| E2E-FIELD-001 | Tables 55.1-3;56.1-2; pp. 69-71 | M | Parse/generate handshake containers, pp and ClearToken1 with required structure. | W1/W3: 15.1; valid request/accept. |
| E2E-FIELD-002 | Tables 55.4-6;56.3-5; pp. 69-72 | M/C-PATH | Validate signature algorithm, DER identity certificate and conditional certPath. | W2/W3: Certificate/signature/path variants. |
| E2E-FIELD-003 | Tables 55.7-12;56.6-11; pp. 69,72 | M | Check v1, association-ID grammar/composition, DH group/public value and HMAC; preserve required handshake agreement. | W2/W3: Version/ID/group/HMAC faults and valid key agreement. |
| E2E-FIELD-004 | Tables 55.13;56.12; pp. 70,72 | M | Check/generate UTC handshake time using settled test interpretation. See S03/S06. | W3: Boundary/format and request/response time relationship. |
| E2E-FIELD-005 | Tables 55.14-19;56.13-18; pp. 70,72-73 | M/C-MODE | Enforce selected encryption alternative, conditional encr list, ICV selection and confidentiality flags. | W3: All declared negotiation combinations and unsupported alternatives. |
| E2E-FIELD-006 | Tables 55.20;56.19; pp. 70,73 | C-ATTR | Check conditional attribute certificate. | W3: Valid/invalid attribute certificate or justified unsupported. |
| E2E-FIELD-007 | Tables 55.21;56.20; pp. 70,73 | M-MMS | Carry and validate the required MMS initialization PrPDU. | W3/W5: MMS initiation success/application reject. |
| E2E-FIELD-008 | Tables 55.22-24;56.21-23; pp. 70-73 | M | Produce/verify matching signature algorithm and exact signed octets. | W2/W3: Independent signature checking, algorithm mismatch and mutation. |
| E2E-FIELD-009 | Tables 57.1-8;58.1-8;59.1-8;60.1-8;61.1-8; pp. 75-84 | M;59 is RX client abort | Signed error/abort containers use ClearToken3, appropriate identity, ID, version, UTC and conditional cert path. | W3/W4: Reject/abort decode, signature and identity checks. |
| E2E-FIELD-010 | Tables 57.9;61.9; pp. 75,84 | M-MMS/C-APP | Protected-protocol rejection/abort carries required application diagnostic with conditional presence. | W3/W5: Invalid MMS initiation or MMS abort cause. |
| E2E-FIELD-011 | Tables 58.9;59.9;60.9; pp. 77-82 | M | Use expected diagnostic unless an alarm requires absent diag. | W3/W4: Table 50/51 plus signed no-diag error vectors. |
| E2E-FIELD-012 | Tables 57.10-12;58.10-12;59.10-12;60.10-12;61.10-12; pp. 75-84 | M | Verify and generate signatures on rejection/security abort/application abort, not only normal handshake. | W2/W3/W4: Signature checks on every emitted/received error type. |
| E2E-FIELD-013 | Tables 62.1-6;63.1-6; pp. 85-87 | M | ClearTransfer contains integrity-protected token2 and negotiated version/ID, valid UTC and timestamp-aware sequence. See S03/S06. | W1/W4: Valid transfer and time/ID/sequence mutations. |
| E2E-FIELD-014 | Tables 62.7;63.7; pp. 85,87 | M | IV is absent for ClearTransfer. | W4: Field-absence tests. |
| E2E-FIELD-015 | Tables 62.8;63.8-9; pp. 85,87 | M/C-REKEY | Validate client rekey public key and server reqRekey/changedKey flags with direction and transition rules. | W4: Rekey flows and illegal flags. |
| E2E-FIELD-016 | Tables 62.9;63.10; pp. 85,87 | M | Carry valid unencrypted MMS PrPDU inside the integrity envelope. | W4/W5: Read/response/report/control decoding. |
| E2E-FIELD-017 | Tables 62.10-13;63.11-14; pp. 85-88 | M/C-GMAC | Require auth, conditional nonce, negotiated ICV algorithm and verified ICV. See S03/S06. | W2/W4: All ICVs plus missing nonce, wrong algorithm and tampering. |
| E2E-FIELD-018 | Tables 64.1-12;65.1-14; pp. 89-92 | C-ENC | Apply encrypted counterparts for token2, ciphertext, conditional IV/auth/nonce and rekey fields. Do not copy erroneous plaintext/signature wording. | W2/W4: Conditional encrypted counterpart matrix; see source issues. |
| E2E-FIELD-019 | Tables 66.1-8;67.1-8; pp. 93-95 | C-TERM (both selected) | Release request/response uses signed token3 with agreed identity/version/ID/time/cert-path. | W3/W5: Both directions and invalid release signatures/IDs. |
| E2E-FIELD-020 | Tables 66.9-11;67.9-11; pp. 93-95 | C-TERM (both selected) | Check release signature algorithm and signature value. | W2/W3: Independently verify signed release. |
| E2E-FIELD-021 | Tables 68.1-5,8-9;69.1-7; pp. 96-99 | M | Validate/emit native ACSE type/version/context, AP/AE titles and association result/source diagnostic. | W1/W5: AARQ/AARE decode; inherited titles; accept/reject variants. |
| E2E-FIELD-022 | Tables 68.6-7,10-11,16;69.8-9,14; pp. 96-100 | N/R declaration | Invocation IDs and implementation information are marked not required; do not invent mandatory support. | W0: PID support declaration and safe parser handling. |
| E2E-FIELD-023 | Tables 68.12-15;69.10-13; pp. 97,100 | M | Native ACSE authentication/requirements/mechanism and context-name-list fields are absent. | W1/W5: Explicit forbidden-field tests. |
| E2E-FIELD-024 | Tables 68.17-19;69.15-17; pp. 97,100 | M | User-information carries proper OSI mapping, E2E indirect reference and correctly selected handshake response. | W1/W5: Table 17 errors and accepted/application/security rejected AARE. |
| E2E-FIELD-025 | Tables 70.1-9;71.1-9; pp. 101-102 | M;Table 70 RX/TEQ only | Abort source and optional user-information obey error layer; wrap correct security/application/OSI diagnostic choice; handSecAbort is not server-originated. | W1/W5: Provider abort, OSI abort, DtSecAbort and ApplAbort. |
| E2E-FIELD-026 | Tables 72.1-3; pp. 103 | M | OSI clear transfer has correct wrapper/context and embedded ClearTransfer. | W1/W5: Valid transfer and wrong/missing context tests. |
| E2E-FIELD-027 | Tables 73.1-3; pp. 103 | C-ENC | OSI encrypted transfer has correct wrapper/context and embedded EncrTransfer. | W1/W5: Encrypted context and payload tests. |
| E2E-FIELD-028 | Tables 74.1-5;75.1-5; pp. 104-105 | C-TERM (both selected) | RLRQ/RLRE carry permitted reason, user-information, negotiated context and signed ReleaseReq/ReleaseRsp. | W1/W5: Orderly release and malformed wrapper cases. |

## Abstract test case map

Server-role adaptations are explicit. Table 50 has 20 rows and Table 51 has 4; the latter does not exhaust normative data-integrity/time/replay/rekey checks. Refer to CORE rows for those additional obligations.

| Case | Printed pages | Applicability | Stimulus | Expected result / issue |
|---|---|---|---|---|
| 15.1 | 27 | M | TEQ sends valid native handshake | DUT validates request and sends correctly signed native acceptance; valid OSI and SecPDU fields.  |
| 15.2 | 27 | M/C-TERM | TEQ sends signed native release request | DUT validates and returns signed native release response; matching session closes.  |
| 17.1 | 29 | M | Wrong ACSE application context | AARE rejected-permanent; acse-service-user application-context-name-not-supported.  |
| 17.2 | 29 | M | Wrong association indirect-reference | AARE rejected-permanent/no-reason-given and OSI invalid-indirect-reference as specified; validate response structure.  |
| 17.3 | 29 | M | Execute Table 50 subcases | Each applicable handshake fault has its exact response and event.  |
| 18.1-2 | 30 | M; adapt DUT/TEQ roles for server | Establish native association then exchange protected clear MMS data | Both client/server ClearTransfer subclasses and OSI wrappers validate. Printed steps assume client DUT; agree server-role adaptation. S08 |
| 20.1 | 31 | M | Clear transfer uses wrong presentation context | ABRT with OSI invalid-pci; valid server abort wrapper.  |
| 20.2 | 31 | M | Clear transfer omits presentation context | ABRT with OSI protocol-error; valid server abort wrapper.  |
| 20.3 | 31 | M | Execute Table 51 subcases | All four table rows plus additional implementation-standard obligations.  |
| 24.1-3b | 35 | TEQ/client-role reference; server response obligation is M | Client supplies new DH material after configured 15-minute interval during traffic | Server acknowledges changedKey with old-key protection, subsequent traffic uses proper new keys; agree clear-mode adaptation. S07/S08 |
| 25.1-3c | 36 | M | Server requests renewal at configured 15-minute interval during traffic | Client supplies new DH; server signals changedKey; continuous traffic verifies each key epoch. S07/S08 |
| 50.1 | 55 | M | Different protected-protocol identity | Signed HandshakeSecReject: protected-protocol-not-supported; validate Table 58 fields.  |
| 50.2 | 55 | M | ClearToken1 signature algorithm differs from Signature.algo | Signed HandshakeSecReject: Table says no-reason-given; normative alarm path omits diag; validate Table 58 fields. S04 |
| 50.3 | 55 | M | Missing mandatory ClearToken1 field | Signed HandshakeSecReject: protocol-error; validate Table 58 fields.  |
| 50.4 | 55 | M | Invalid application public-key certificate | Signed HandshakeSecReject: invalid-public-key-certificate, or specified alarm path; validate Table 58 fields.  |
| 50.5 | 55 | M | Certificate identity differs from sender AP/AE | Signed HandshakeSecReject: Table says no-reason-given; normative address-mismatch alarm omits diag; validate Table 58 fields. S04 |
| 50.6 | 56 | M | Unsupported signature algorithm | Signed HandshakeSecReject: invalid-signatureAlgorithm; validate Table 58 fields.  |
| 50.7 | 56 | M | Unsupported E2E version | Signed HandshakeSecReject: unexpected-version; validate Table 58 fields. S08 |
| 50.8 | 56 | M | Handshake time more than ten minutes from local UTC | Signed HandshakeSecReject: invalid-time-value; validate Table 58 fields. S03 |
| 50.9 | 56 | M | Reuse handshake timestamp and association ID | Signed HandshakeSecReject: Table says no-reason-given; normative replay alarm omits diag; validate Table 58 fields. S04 |
| 50.10 | 56 | M | Unsupported DH group | Signed HandshakeSecReject: dhGroup-not-supported; validate Table 58 fields.  |
| 50.11 | 57 | M | Unsupported HMAC for derivation | Signed HandshakeSecReject: hmac-algorithm-not-supported; validate Table 58 fields.  |
| 50.12 | 57 | C-no-AE (true in baseline) | Offer AE where server does not support/accept it | Signed HandshakeSecReject: Table says aea-algorithms-not-supported; implementation standard has distinct unsupported-mode case; validate Table 58 fields. S05 |
| 50.13 | 57 | C-no-encryption (true in baseline) | Offer AE where server prohibits encryption | Signed HandshakeSecReject: aea-select-but-encryp-not-supp; validate Table 58 fields. S05 |
| 50.14 | 57 | C-AE negotiation | Offer only unsupported AE algorithms | Signed HandshakeSecReject: aea-algorithms-not-supported; validate Table 58 fields. S05 |
| 50.15 | 57 | C-AE-required | Offer non-AE where server requires AE | Signed HandshakeSecReject: ae-is-required; validate Table 58 fields. S05 |
| 50.16 | 58 | C-NONAE (true in baseline) | Offer only unsupported non-AE ICV algorithms | Signed HandshakeSecReject: icv-algorithms-not-supported; validate Table 58 fields.  |
| 50.17 | 58 | C-no-encryption (true in baseline) | Request application encryption when server prohibits it | Signed HandshakeSecReject: encryption-not-required; validate Table 58 fields.  |
| 50.18 | 58 | C-encryption-required | Omit application encryption when server requires it | Signed HandshakeSecReject: encryption-required; validate Table 58 fields.  |
| 50.19 | 58 | C-ENC-NONAE | Offer multiple supported non-AE encryption algorithms | Signed HandshakeSecReject: Valid HandshakeAcc selects first supported offered algorithm; validate Table 58 fields.  |
| 50.20 | 59 | M | Handshake time more than ten minutes off current UTC | Signed HandshakeSecReject: invalid-time-value; retain separate row even though similar to row 8; validate Table 58 fields. S03 |
| 51.1 | 59 | M | ClearToken2 mandatory field missing | Signed DtSecAbort: protocol-error; validate Table 60 and server OSI abort.  |
| 51.2 | 59 | M | Transfer version differs from accepted version | Signed DtSecAbort: unexpected-version; validate Table 60 and server OSI abort.  |
| 51.3 | 60 | M | Send EncrTransfer on clear-only association | Signed DtSecAbort: encryption-not-selected; validate Table 60 and server OSI abort.  |
| 51.4 | 60 | M | Invalid transfer sequence number | Signed DtSecAbort: invalid-sequence-number; validate Table 60 and server OSI abort. S02 |
| 21 | 32 | C-ENC; source unavailable | Encrypted positive exchange | Cannot reconstruct exact test steps from supplied unloaded page; recover authoritative page before formal claim. S01 |
| 23.1-3 | 34 | C-ENC | Wrong/missing encrypted transfer presentation context; Table 54 | Correct OSI abort diagnostics and all applicable Table 54 cases. S10 |
| 54.1 | 66 | C-ENC | Missing token2 field | Signed DtSecAbort: protocol-error. S02/S04/S10 |
| 54.2 | 66 | C-ENC | Wrong negotiated version | Signed DtSecAbort: unexpected-version. S02/S04/S10 |
| 54.3 | 67 | C-ENC | Clear transfer on encryption-required association | Signed DtSecAbort: encryption-required. S02/S04/S10 |
| 54.4 | 67 | C-ENC | Wrong sequence | Signed DtSecAbort: invalid-sequence-number. S02/S04/S10 |
| 54.5 | 67 | C-ENC | Transfer time outside ten-minute limit | Signed DtSecAbort: invalid-time-value. S02/S04/S10 |
| 54.6 | 68 | C-ENC | Repeated transfer timestamp and sequence | Signed DtSecAbort: replay alarm; diag presence discrepancy. S02/S04/S10 |
| 54.7 | 68 | C-ENC | Unexpected key-change field | Signed DtSecAbort: Source server/client direction conflicts; agree valid server test stimulus. S02/S04/S10 |

## Disposition of every table

| Table | Printed pages | Applicability | Subject / mapping |
|---|---|---|---|
| 1 | 17 | M | PIXIT base; PRE |
| 2 | 18 | M | PIXIT security; PRE |
| 3 | 19-20 | M | OSI applicability summary; CASE |
| 4 | 21-22 | EXCLUDED-XMPP | XMPP applicability summary; scope |
| 5 | 23 | M | Base profile; DECL |
| 6 | 23 | M | Handshake declarations; DECL |
| 7 | 23 | M/C-TERM | Association declarations; DECL |
| 8 | 23 | M | OSI EnvPDU declaration; DECL |
| 9 | 23-24 | M | OSI subclasses; DECL/FIELD |
| 10 | 24 | M | SecPDU subclasses; DECL/FIELD |
| 11 | 24 | M | Encryption mode; DECL |
| 12 | 24-25 | M/C-crypto | Crypto declarations; DECL/CORE |
| 13 | 25 | M | ASN.1 object identifier; DECL |
| 14 | 26 | EXCLUDED-DUT-CLIENT | Client handshake/release; TEQ reference |
| 15 | 27 | M | Server handshake/release; CASE |
| 16 | 28 | EXCLUDED-DUT-CLIENT | Client association resilience; TEQ reference |
| 17 | 29 | M | Server association resilience; CASE |
| 18 | 30 | M-adapt-roles | Clear data exchange; CASE/S08 |
| 19 | 30 | EXCLUDED-DUT-CLIENT | Client clear-transfer resilience; TEQ reference |
| 20 | 31 | M | Server clear-transfer resilience; CASE |
| 21 | 32 | C-ENC; source missing | Encrypted exchange; CASE/S01 |
| 22 | 33 | EXCLUDED-DUT-CLIENT | Client encrypted context errors; TEQ reference |
| 23 | 34 | C-ENC | Server encrypted context errors; CASE |
| 24 | 35 | TEQ-reference; server response M | Client-originated rekey; CASE/S07/S08 |
| 25 | 36 | M | Server-originated rekey; CASE/S07 |
| 26 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 27 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 28 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 29 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 30 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 31 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 32 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 33 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 34 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 35 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 36 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 37 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 38 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 39 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 40 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 41 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 42 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 43 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 44 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 45 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 46 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 47 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 48 | 38-48 | EXCLUDED-XMPP | XMPP declarations/procedures; scope |
| 49 | 50-54 | EXCLUDED-DUT-CLIENT | Client handshake resilience; TEQ reference |
| 50 | 55-59 | M/trigger conditions | Server handshake resilience; CASE |
| 51 | 59-60 | M | Server clear resilience; CASE/CORE |
| 52 | 61-63 | EXCLUDED-DUT-CLIENT | Client clear resilience; TEQ reference |
| 53 | 64-66 | EXCLUDED-DUT-CLIENT | Client encrypted resilience; TEQ reference |
| 54 | 66-68 | C-ENC | Server encrypted resilience; CASE/S10 |
| 55 | 69-71 | M-RX | HandshakeReq; FIELD/CORE |
| 56 | 71-74 | M-TX | HandshakeAcc; FIELD/CORE |
| 57 | 75-76 | M-TX | ApplicationReject; FIELD/CORE |
| 58 | 77-78 | M-TX | HandshakeSecReject; FIELD/CORE |
| 59 | 79-80 | M-RX; no server TX | Client handshake abort; FIELD/CORE |
| 60 | 81-82 | M-RX/TX | Security transfer abort; FIELD/CORE |
| 61 | 83-84 | M-RX/TX | Application abort; FIELD/CORE |
| 62 | 85-86 | M-RX | Client ClearTransfer; FIELD/CORE |
| 63 | 87-88 | M-TX | Server ClearTransfer; FIELD/CORE |
| 64 | 89-90 | C-ENC-RX | Client EncrTransfer; FIELD/CORE |
| 65 | 90-92 | C-ENC-TX | Server EncrTransfer; FIELD/CORE |
| 66 | 93-94 | C-TERM (selected) | ReleaseReq; FIELD/CORE |
| 67 | 95-96 | C-TERM (selected) | ReleaseRsp; FIELD/CORE |
| 68 | 96-97 | M-RX | AARQ native wrapper; FIELD/CORE |
| 69 | 99-100 | M-TX | AARE native wrapper; FIELD/CORE |
| 70 | 101 | M-RX/TEQ-reference | Client ABRT; FIELD/CORE |
| 71 | 102 | M-TX | Server ABRT; FIELD/CORE |
| 72 | 103 | M-RX/TX | Clear presentation wrapper; FIELD/CORE |
| 73 | 103 | C-ENC | Encrypted presentation wrapper; FIELD/CORE |
| 74 | 104 | C-TERM (selected) | RLRQ wrapper; FIELD/CORE |
| 75 | 105 | C-TERM (selected) | RLRE wrapper; FIELD/CORE |
| 76 | 106-109 | EXCLUDED-XMPP | XMPP stanza subclass; scope; Table 77 source missing |
| 77 | 106-109 | EXCLUDED-XMPP | XMPP stanza subclass; scope; Table 77 source missing |
| 78 | 106-109 | EXCLUDED-XMPP | XMPP stanza subclass; scope; Table 77 source missing |
| 79 | 106-109 | EXCLUDED-XMPP | XMPP stanza subclass; scope; Table 77 source missing |

## Limits of completeness

All visible applicable server requirement families and subclass fields in the supplied test document are mapped, together with the relevant implementation clauses in the companion standard. Source defects and interpretation conflicts are retained in SOURCE_ISSUES.md. Final numeric wire constants, conditional applicability and formal test steps require the authoritative source/code baseline and lab agreement. This map does not claim complete CEI certification, full TLS/PKI/RBAC certification, XMPP/client/router support, or tests of all application logic.

# E2E field test evidence - 23 October 2026

**Applicability correction after source review:** the 108 conditional evidence dispositions record current behavior; they are not approved CEI exclusions. CEI 57-142:2026-07 requires implemented, switchable application encryption (AES-128/256-CBC and AES-128/256-GCM). Separate attribute certificates are conditional on selecting RBAC Profile B; Profile A is an allowed alternative. See [applicability review](../../conformance/e2e/APPLICABILITY_REVIEW.md).

## Result and scope

The evidence package now joins executable test records to every row in the 695-row BER field map. The ledger has **695/695 rows linked to evidence or an explicit conditional feature disposition; 0 rows are unlinked**. It is a complete traceability record for the current selected native profile, not a declaration that IEC conformance is closed.

### Row status

- **565** active-profile rows have partial direct evidence.
- **22** conditional-map rows are on the exercised ClearTransfer path and have partial direct evidence.
- **108** conditional feature rows record an outgoing profile absence or an active unsupported-feature receive gate.
- **0** rows are marked complete. The direct rows still need the applicable remaining F01-F10 and B01-B12 variants and any target/external qualification. Conditional positive behaviors remain unimplemented or unselected.

The machine-readable [695-row evidence ledger](complete-test-evidence-ledger.csv) lists, per row, outgoing contract assertions, profile dispositions, native receive case IDs, standalone decoder field assertions, complete-container mutation seeds, unsupported-feature gates, status and remaining work. The native-only [execution ledger](field-execution-ledger.csv) is a narrower view: it records 515 rows with direct native campaign assertions and 180 without a direct assertion from that campaign. The combined ledger additionally links independent TX, standalone codec and whole-message mutation evidence.

## Verification

- **Full E2E host suite:** 9/9 tests passed. See [host CTest log](host-ctest.log).
- **Latest native field campaign:** 1/1 passed in 204.76 seconds after adding explicit TX profile-disposition assertions. See [field campaign CTest log](field-campaign-ctest.log).
- **MMS regressions:** 5/5 passed. See [MMS CTest log](mms-ctest.log).
- **ASan/UBSan BER field corpus:** passed, 465,536 checks across 27 seeds and 459,520 exhaustive single-octet mutations (206,314 accepted, 253,206 rejected, zero failures). See [sanitizer CTest log](sanitizer-ctest.log) and [field assertion TSV](ber-field-assertions.tsv).
- **Per-profile native campaign:** [RSA](field-campaign-rsa.json) and [EC](field-campaign-ec.json) each contain 2,111 PASS records across 421 unique case IDs.
- **Independent outgoing contract:** 2,189 received frames, 29 emitted structural shapes, and 3,215 local negative controls. It links TX assertions to 334 mapped rows and receive cases to 229 rows. See [TX contract results](tx-contract.json).
- [Source and evidence SHA-256 manifest](source-sha256.json) identifies the test inputs and captured outputs. Generated private credentials are not included in this evidence folder.

## Cases covered by this campaign

| Area | Evidence |
|---|---|
| Complete CONNECT size | Actual complete session CONNECT user-data lengths 10,239 and 10,240 accepted; 10,241 rejected before presentation parsing or application dispatch. |
| Protected MMS RX/TX size | 63,999/64,000 accepted and 64,001 rejected or signed-aborted as specified. TX responses are checked independently byte-for-byte; oversized callback results produce the expected signed abort. |
| Profile limits | Rekey, PDU size and replay-pair boundaries, including values immediately below, at and beyond configured limits. |
| Context negotiation | Six offered context orders across three context-ID ranges, followed by protected read and signed release. |
| Association and handshake | AARQ and ClearToken1 required-field missing/duplicate/tag/order cases; selected optional presence/absence; unsupported algorithm and malformed parameter cases; invalid DH group/public key; signed diagnostics and no-dispatch checks. |
| Transfer and controls | ClearToken2, authenticator and application-data structural cases; signed release and terminal control-message checks; valid authenticated unsupported-choice rejection. |
| Resource handling | Scoped native C++ allocation-failure sweeps for RSA/EC receive and recovery paths; decoder allocation failures; BER byte, nesting-depth and element-count limits. The campaign checks bounded dispatch, process survival and healthy reconnect. |
| Outgoing messages | Independent parsing and validation of emitted field presence/absence, order, cardinality, tags, parameters, time/integer syntax, key widths, presentation contexts and payload correspondence. Local malformed-output controls validate the TX checker. |

The large-response callback is a test-only synthetic MMS application. It tests native protection, serialization and framing limits; it does not show that the legacy object model can create an application object of that size. Allocation injection covers ordinary C++ `new` on the selected native event path; it does not inject C/Mbed TLS allocation failures, kernel/socket failures or every event path. If memory exhaustion prevents encoding an abort, the resource test accepts controlled EOF/reset while requiring bounded dispatch, process survival and a healthy reconnect.

## CRL scope evidence

The earlier CRL concern was that matching the CRL issuer alone is not enough: the certificate's CRL Distribution Point must be covered by the CRL's Issuing Distribution Point, with unsupported reason or certificate-type scopes rejected. The current identity validator parses and compares that scope and fails closed when it cannot establish coverage. The identity test covers a matching scope, wrong scope, CA-only scope, partial reasons, malformed scope, missing distribution point, missing CRL coverage and revoked certificates. These are separate identity/revocation tests, not rows in the 695-entry BER map; the identity cases passed in the full host suite. See `apps/ccli/tests/unit/e2e_identity_test.cpp` and the identity fixture generator.

## Defect found and fixed

The campaign found that an exact complete CONNECT of 10,241 bytes was accepted because the prior bound applied to the smaller inner AARQ and did not include presentation context/header overhead. The maintained `scripts/generate_mms_e2e.py` overlay now checks complete session CONNECT user-data against the native 10,240-byte budget before `IsoPresentation_parseConnect`. This is scoped to the configured native adapter owner. Existing TLS/RBAC and dual-certificate source baselines remain pinned, with the reviewed generator fingerprint updated for the fix.

## Remaining closure work

- Execute applicable remaining F01-F10 field/value/state variants and B01-B12 BER representation/resource cases for the 587 partially evidenced rows.
- Implement/select positive encrypted-transfer, encryption-algorithm and attribute-certificate behavior before positive vectors for those features can apply. The ledger records the current profile disposition or active rejection gate separately from positive coverage.
- Exercise intermediate-chain transmission and positive `certPath` on the native stack, plus fragmentation/order/lifecycle and combined certificate/signature/envelope bounds.
- Extend fault injection to remaining receive/control/handshake/send paths, C-provider allocations, socket failures, connection/global replay exhaustion, races and target memory limits.
- Complete Tespro runtime, qualified external peer/TEQ, transport qualification and applicable formal conformance procedures. No APK build/signing, device install or Tespro runtime result is claimed here.

## Reproduction

From the repository root in WSL:

```sh
ctest --test-dir apps/ccli/build-e2e-crypto -R '^e2e_native_field_campaign$' --output-on-failure
bash scripts/check_e2e_host.sh
bash scripts/check_e2e_ber_sanitizers.sh
ctest --test-dir apps/ccli/build-e2e-crypto -R 'mms_' --output-on-failure
python3 lab/conformance/e2e/build_field_evidence_ledger.py
```

The campaign creates a unique fixture folder with disposable private credentials. Copy only the JSON/CSV/log outputs when preserving evidence; do not copy its private keys.

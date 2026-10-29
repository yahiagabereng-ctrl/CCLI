# BER/field work — current status

**CEI applicability correction:** the 108 conditional evidence dispositions are not approved CEI exclusions. CEI requires switchable application encryption with AES-128/256-CBC and AES-128/256-GCM. Profile B attribute-certificate positive capability can be excluded when Profile A is declared. See [source review](APPLICABILITY_REVIEW.md).

23 October 2026. Latest execution and remaining per-row variants: [field/limit campaign](../../evidence/e2e-field-campaign-2026-10-23/RESULTS.md). The mapping is complete for the selected native OSI/MMS server profile. BER implementation has passed the current standalone sanitizer corpus and native host integration checks. Full per-row field coverage, target runtime, external TEQ interoperability and formal qualification remain open. The combined [695-row test-evidence ledger](../../evidence/e2e-field-campaign-2026-10-23/complete-test-evidence-ledger.csv) now has no unlinked rows: 565 active rows and 22 exercised conditional-path rows have partial direct evidence; 108 unselected features have an explicit disposition or rejection gate; none are closed as fully conformance-complete.

## Mapping deliverables

- [BER_FIELD_COVERAGE.md](BER_FIELD_COVERAGE.md): complete selected-profile acceptance map, shared field constraints, SecPDU/OSI/ACSE containers, F01–F10 variants, B01–B12 representations/resources, diagnostics, source gates and closure criteria.
- [ber-field-map.csv](ber-field-map.csv): **695 expanded entries**, covering **28/28 FIELD groups** and **21/21 Tables 55–75**. It includes conditional features and active unsupported-feature negatives.
- [ber-field-map-inventory.json](ber-field-map-inventory.json): both pinned ASN.1 type inventories, aliases/metadata/exclusions, source hashes and counts. generate_ber_field_map.py regenerates and checks mapping completeness; it does not test implementation.
- [BER mapping resume evidence](../../evidence/e2e-ber-mapping-2026-10-23/RESUME_RESULTS.md).

All 695 rows remain mapping obligations. The passing corpus is a subset and does not mark the formal requirement tracker complete.

## Current source work completed

- ber_codec.cpp/.hpp: bounded constructed OCTET/BIT/UTF8/time strings with retained storage; malformed fragment/tag/depth/byte checks; original BER spans remain available for authentication.
- e2e_codec.cpp/.hpp: constructed string support in tokens/signatures/authenticators, BER-equivalent named bits, algorithm parameter policy and extensible AlgorithmIdentifier parsing, schema-aware extensibility and error preservation. Closed outer ClearTransfer cardinality remains enforced.
- e2e_association.cpp/.hpp and e2e_server.cpp: retained constructed-value ownership through preflight, control, and association processing; constructed ACSE/version/identifier/signature/path parsing.
- e2e_probe.py: bounded BER parser validates children inside definite constructed values, high tags, indefinite EOC and authenticated transcript contents.
- e2e_ber_field_test.cpp: 27 seed corpus with field matrices and exhaustive single-octet mutations.
- e2e_native_stack_test.py: live native constructed-handshake/transfer/rekey and malformed-value cases.
- Reviewed native E2E fingerprints include the BER engine fixes and the maintained generator's complete CONNECT bound. The TLS/RBAC and dual-certificate baselines remain pinned. Their guard passes with the current source.

## Verification on current source

Commands were run from the repo root through WSL:

```powershell
wsl.exe -d Ubuntu --cd '/mnt/c/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged' -- bash scripts/check_e2e_ber_sanitizers.sh
wsl.exe -d Ubuntu --cd '/mnt/c/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged' -- bash scripts/check_e2e_host.sh
wsl.exe -d Ubuntu --cd '/mnt/c/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged' -- ctest --test-dir apps/ccli/build-e2e-crypto -R mms_ --output-on-failure
```

Results:

- ASan/UBSan BER corpus: **pass**, 465,536 checks, 27 seeds, 459,520 exhaustive single-byte substitutions; 206,314 accepted, 253,206 rejected, zero failures.
- Host E2E CTest: **9/9 pass**, including the new field campaign, including e2e_native_stack and e2e_native_resilience.
- MMS regression CTest: **5/5 pass**, including the TLS/source-baseline guard.
- Native field campaign: **319 cases/profile**, 2186 independently validated outgoing frames and 3215 local TX negative controls. The 695-row execution ledger records **455 partially covered rows and 240 without direct assertions in this campaign**. It does not close all F01-F10 variants.
- Python syntax checks: pass.
- Mapping completeness generator: **695 rows; 28/28 FIELD groups; 21/21 subclass tables; both ASN.1 inventories dispositioned**.
- The first full host run found a stale E2E source fingerprint; after source review, the e2e_server.cpp BER fingerprint was updated and all five MMS checks passed.
- The canonical ARM64 helper passes with OPENWRT_SDK_HOME=/home/adminavz. Current binary SHA-256: 5d42af83f7548dcb116cd7a8ded046bfb83566d81e6f1a4e53d620469d1892fa. The earlier SDK absence conclusion used the wrong location.
- No APK was built, signed, bundled or installed. No Tespro runtime claim is made.

## Limits and remaining work

1. The corpus is finite. It does not constitute exhaustive execution of each of the 695 mapping rows, every generated TX subclass, every list/SET ordering, all allocations or every resource boundary.
2. Exact complete CONNECT 10239/10240/10241 and MMS PDU 63999/64000/64001 RX/TX limits are now executed, including all ICV modes. The new independent Python TX contract checks actual emitted fields and runs local malformed-output controls. Continue certificate/path fragmentation, unexercised implicit strings and combined envelope limits; consult the 695-row execution ledger for partial and untouched rows.
3. Scoped ordinary C++ allocation failures are injected through decoder parsing and native receive/dispatch/protection serialization, with subsequent reconnect checks. This does not inject Mbed TLS C allocations, socket/system-call failures or every event path. Remaining multibyte/nesting/state/resource variants remain open; the sanitizer corpus includes exact default byte/element limits.
4. Complete the source/lifetime review for all decoded-view consumers. Keep ASN.1 extension ambiguity and known source conflicts explicitly documented.
5. Use OPENWRT_SDK_HOME=/home/adminavz with apps/firmware-updater/scripts/check-ccli-target-build.sh for subsequent target builds; current build passes.
6. Obtain a qualified external TEQ/TSP answer, resolve applicable S02/S03/S04/S05/S06/S08/S11/S13 gates, and complete device commissioning/runtime evidence before any conformance claim.
7. Only if the user requests a candidate, use the canonical apps/firmware-updater signing/USB workflow. Device runtime remains unverified.

The previous pause is historical. Resume from the remaining work above; preserve other uncommitted Tespro/Aggregator changes.


Follow-up parsing fixes and current target build evidence: [review results](../../evidence/e2e-ber-mapping-2026-10-23/REVIEW_FIX_RESULTS.md).

Target build: use the exact command in the linked review results, with OPENWRT_SDK_HOME=/home/adminavz.

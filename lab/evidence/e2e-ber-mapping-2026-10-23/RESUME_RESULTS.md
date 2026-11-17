# BER mapping and integration resume results

Date: 23 October 2026
Repository: CCLI Merged working tree (uncommitted)
Purpose: record current BER implementation checks without claiming formal conformance.

## Mapping completeness

The reproducible mapper completed successfully: 695 expanded map rows; 28/28 E2E-FIELD groups; all 21 Tables 55–75; both pinned ASN.1 module type inventories dispositioned. This is mapping completeness, not execution of all rows. Conditional application encryption and attribute certificates remain mapped as conditional positives with active unsupported-feature negatives.

## Current-source verification

| Check | Result |
|---|---|
| Standalone ASan/UBSan BER corpus | PASS: 465,462 checks; 27 seeds; 459,520 exhaustive single-octet substitutions; 206,299 accepted legal/ambiguous mutations; 253,221 rejected; 0 failures |
| Native host E2E CTest | PASS: 8/8, including codec, BER field matrix, crypto, identity, native stack and native resilience |
| Native socket BER additions | PASS: constructed handshake certificate/version/association-ID/DH/time/confidentiality/signature values; indefinite signed containers; constructed transfer authentication and GMAC rekey; authenticated malformed constructed-string and unsupported constructed-IV abort cases |
| MMS regression CTest | PASS: 5/5, including TLS/source-baseline guard |
| Python peer/test syntax | PASS |
| Mapping generator | PASS: 695 rows / 28 groups / 21 tables / both ASN.1 inventories |
| git diff whitespace check | PASS; only expected LF-to-CRLF notices |

The MMS baseline check initially flagged an outdated native E2E source fingerprint. Review found the mismatch was limited to e2e_server.cpp, which contains the BER representation changes. The native E2E source fingerprint was refreshed; no TLS/RBAC or dual-certificate baseline pin was changed. The guard then passed as part of the 5/5 MMS run.

## Boundaries

The finite sanitizer corpus and live wire cases do not execute every one of the 695 mapped rows, all generated TX subclasses, all maximum-size boundaries, every algorithm parameter/curve, or every allocation/resource failure. The development peer is not a qualified external TEQ. External profile conflicts S02/S03/S04/S05/S06/S08/S11/S13 are not closed by host tests.

Correction from follow-up review: the SDK exists under /home/adminavz. The corrected code now passes the canonical ARM64 compile using OPENWRT_SDK_HOME=/home/adminavz. See REVIEW_FIX_RESULTS.md for newer source/test/build evidence; the fingerprints below describe the earlier resume run. No APK, USB bundle or Tespro runtime test is claimed.

## Source and binary fingerprints

Reviewed LF-normalized source fingerprint used by the native E2E extension guard:
- e2e_server.cpp: 86486208f3b791933864312c0df4780c25ec25fd0bcfb6f7cccd9fbdf2aee640

Relevant Windows-file SHA-256 values:
- ber_codec.cpp: 3286c966333da1675c10039b150d2939683a3ea3db5fdc2d313d2bf40f9d1984
- ber_codec.hpp: 6b1d1a24104a7bab459c45667a3b35cd87395d3279cba92b086dab058fae7d6c
- e2e_codec.cpp: 971d7a4c22cbf18c266adb179292d4826097bad12c7a0f5e2aec7876cd1dc302
- e2e_association.cpp: a5af19e44bc9cc955a80a161a63bf45217f40abb41c3bd399bffd71ff2ad2fc4
- e2e_server.cpp: a22e10feb7816b8e0dbcda0bdd9c00cef0f714ade15e8f74a6b1c351d3bcec46
- e2e_probe.py: a6d579ab2ba224d419faa8c922271669f47b7eb376191a1052253f12d8e85d9f
- e2e_ber_field_test.cpp: 2a73c4f303571cfd0cbecd126a89c6680c43a9a26499fb5935234601639d339f
- e2e_native_stack_test.py: bb705a94530d8641b723a4912826548bc7ba6d3d109ab0295545a9058802c4e8
- sanitizer corpus executable: 768aadcf0c1069b6fd63c428d79ebc3aaa6e3d0c2a87ef83e5a0ad2bd5b4cd93
- native server test executable: a68580a8bcc4c7c3e35d398505a25d8725e74b564edcabdcf287ebd763b2f01e

## Next work

1. Complete row-by-row RX and independently decoded TX evidence for active requirements, including certificate/path and implicit-string families, full-size transfers, and ACSE/session/presentation boundaries.
2. Extend multibyte, length, nesting, stateful, concurrency, resource and allocation-failure coverage.
3. Obtain the configured OpenWrt SDK and run the canonical target compile; record fresh source and binary hashes.
4. Resolve external profile/source gates and obtain a qualified peer before formal qualification or device claims.
5. Only create a Tespro candidate after target build and commissioning readiness; use the canonical firmware-updater toolkit for any requested APK/USB process.

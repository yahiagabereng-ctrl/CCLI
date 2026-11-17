# E2E review fixes and verification

23 October 2026. Current uncommitted working tree. This record supersedes earlier SDK-absence claims and the preceding BER corpus counts.

## Fixed

- Structural AlgorithmIdentifier decoding now consumes valid extension additions, as required by the pinned extensible ASN.1 declaration. Algorithm parameter semantics remain enforced by consumers.
- A shared bounded signature AlgorithmIdentifier helper is used for handshake and control validation. RSA permits absent/NULL parameters; ECDSA requires absent parameters. Distinguishable context-specific additions start the opaque extension tail; universal values at the optional-parameter position must satisfy the known parameter policy.
- GMAC parsing permits extension additions while retaining its parameter/tag-length checks.
- Regression tests include generic extensibility and parameter policies, live RSA/EC handshake extensions, GMAC authenticator extensions, signed-control extensions, RSA NULL acceptance and ECDSA NULL rejection. Negative control inputs are signed correctly, so rejection does not merely result from a broken signature.
- The SDK was present at /home/adminavz; the default home lookup was insufficient. Documentation now records the explicit SDK parent override and passing target build. The original TLS and dual-certificate pins were preserved; the reviewed native server fingerprint was refreshed for the actual code change.

## Executed checks

| Check | Result |
|---|---|
| ASan/UBSan BER corpus | PASS: 465468 checks, 27 seeds, 459520 single-octet substitutions, 206314 accepted, 253206 rejected, zero failures |
| Host build and E2E CTests | PASS: 8/8; includes native stack and resilience, RSA/group14 and EC/group23 |
| MMS/TLS/RBAC regression CTests | PASS: 5/5, including reviewed-source guard |
| Python syntax | PASS |
| Git diff whitespace check | PASS; checkout line-ending notices only |
| Canonical ARM64 target compile | PASS: MMS/TLS, GOOSE, IEC104/TLS and actual SDK Modbus enabled |

Target command, from PowerShell at the repo root:

    wsl.exe -d Ubuntu --cd '/mnt/c/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged' -- env OPENWRT_SDK_HOME=/home/adminavz bash apps/firmware-updater/scripts/check-ccli-target-build.sh

ARM64 ELF SHA-256: 5812ec9ddedb7c872332862cd39fd7eac118f8b3b70cb1d49b285c1ef938f5ab

Raw and LF-normalized source hashes plus executable hashes are in [review-fix-source-hashes.json](review-fix-source-hashes.json). Actual CTest logs: [sanitizer](review-fix-sanitizer-ctest.txt), [E2E](review-fix-e2e-ctest.txt), [MMS](review-fix-mms-ctest.txt).

## Review scope and limits

Reviewed the changed structural decoder, shared algorithm helper, handshake preflight and control/GMAC consumers, error containment and BER test-peer mutations. No further confirmed defects were found in those changed paths. This is not an exhaustive review of every E2E source file. The full 695-row execution matrix, remaining maximum-size/resource/TX cases, external source/profile gates and qualified-peer evidence remain open. No APK or USB bundle was created, and no device installation/runtime test was performed.

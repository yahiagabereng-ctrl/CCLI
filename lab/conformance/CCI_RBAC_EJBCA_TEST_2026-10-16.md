# EJBCA Profile A provisioning and host verification receipt

Date: 2026-10-16, Europe/Rome. Firmware/TLS deployment: none.

## Artifacts

| Artifact | Result |
|---|---|
| EJBCA | Existing `ccli-ejbca-ce`, version 9.6.3, https://localhost:9443/ejbca/adminweb/ |
| CA | `CCLI-RBAC-Lab-CA`, RSA 3072 / SHA256WithRSA |
| Current certificates | 24, with local keys, PEM/DER certificates, CSR and encrypted PKCS#12 |
| Standard role matrix | IDs 0..6, each at revisions 0 and 1 (14 certificates) |
| Private roles | DSO -1 and Aggregator -2, revision 1 |
| Other identities | Multi-role 0+2, six negative fixtures, owner signer |
| Signed CRL | Five revoked serials: four retired initial certificates and `negative-revoked` |
| Recovery | 27 role/retired certificate DB records imported with strict issuer checks; original serials/keys retained |
| Candidate | Sequence 1, 17 positive identity/transport bindings, four signed package files |

Protected local material: `lab/ejbca-staging/rbac/`. `public-manifest.json` records
all current fingerprints, serials, roles, revisions, expiry and revocation status.
`secrets.json` contains local secrets; it must stay ignored and protected. The
owner private key stays on this PC and the CA private key remains in EJBCA.

- CA SHA256: `c71e4b50ea27be45d334d7b1f3f65f7004d6aaa5fb0c4417215d396509f34470`.
- Owner signer SHA256: `4897c6b465ec61f4261f70846209e98cecd809de416c95c40f255ab21f3548e3`.
- Candidate manifest SHA256: `a7c68a7bd04f900cb52d1371e3f1bd72704518059c8c6245dd4bd7f424e98391`.
- CRL thisUpdate: 2026-10-16 09:36:05 Europe/Rome.
- CRL nextUpdate: 2026-10-17 09:36:04 Europe/Rome. Refresh before later tests.

## Checks actually executed

1. Verified every current certificate's issuer signature, lifetime, key pair,
   explicit non-CA constraints, digitalSignature-only usage and exact noncritical
   IECUserRoles DER. Role clients have clientAuth; owner signer has no role
   extension/TLS EKU. Checked certificate size and all PKCS#12 key/cert/chain exports.
2. Verified CA self-signature, CA signing usages, CRL signature and expected current
   revocation status; separately confirmed all four retired initial serials occur.
3. Verified the candidate's four file hashes and owner signature. Negative fixture
   bindings exist only in the separate `verification-only` host test package.
4. Built the optional native product-verifier checker with the existing host
   libIEC61850/mbedTLS build. Rebuilt its source with the generated CMake
   compile/link settings after correcting its DSO CONFIG test expectation.
5. Ran the checker against the real EJBCA bundle and signed test package using
   a fresh Linux host activation-state file. Results:

```text
Rejected negative-unknown-role: asserted role not authorized for this identity
Rejected negative-wrong-aor: asserted role not authorized for this identity
Rejected negative-wrong-definition: asserted role not authorized for this identity
Rejected negative-wrong-revision: asserted role not authorized for this identity
Rejected negative-revoked: identity chain, lifetime or revocation verification failed
Rejected negative-no-roles: invalid or missing Profile A role extension
Rejected owner-signer: identity certificate not individually authorized
PASS: product verifier accepted 17 positive identities and rejected 7 negative/non-user identities; signed package, CA/CRL and access checks passed
```

6. Protected TLS/ACSE baseline check passed; the saved working PC TLS baseline
   contains 30 files and all hashes remain unchanged. Checked JSF checkbox
   handling with a checked input missing an explicit value and an unchecked input.

Source checker: `apps/ccli/tests/integration/mms_rbac_commissioned_pki_check.cpp`.
It takes `BUNDLE NEW_STATE_PATH`; the state parent must already exist. The
optional CMake target is `mms_rbac_commissioned_pki_check`. It reads public
certificates and a signed package and makes no network connections.
Local logs: `tmp/ejbca-rbac-check-build.log`, `tmp/ejbca-rbac-native-check.log`.

## Corrections and limits

The provisioning form initially submitted an empty value for checked inputs
without a value, disabling profile flags. The script now uses browser default
`on`. Required extensions and certificate storage were explicitly repaired.
The first four bad certificates were superseded. Later cryptographically valid
role certificates had not been stored for revocation; strict import recovered
their original signed public records and final CRL checks prove revocation.
See [EJBCA certificate storage fields](https://docs.keyfactor.com/ejbca/9.6/certificate-profile-fields).

This proves host verification of actual issued certificates and signed policy;
it does not prove live TLS/MMS/TSP interoperability, plant controls or full CEI
conformance. No Tespro file, TLS setting, model, firmware or activation state was
changed. Profile A remains off at the last verified device configuration.
Next work is controlled trust/pin/client commissioning and the remaining live
tests in `CCI_RBAC_COMPLETE_TEST_INPUTS.md`. The current model lacks complete
Aggregator functions, so host acceptance of that identity is not a positive
live test of absent objects. Never deploy `verification-only/`.

# Local owner-branch merge — 16 October 2026

## Location and inputs

- Destination: `C:/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged`.
- Personal input: `main` / `b185266` from `alirezavzand/CCLI`.
- Owner input: `upstream/main` / `ffb0f19` from `yahiagabereng-ctrl/CCLI`.
- Integration branch: `codex/merge-owner-main`; backup: `backup/before-owner-merge`.
- Original checkout and its main branch are preserved. No device deployment,
  Git push, PR creation, or signing/bundling action is part of this merge.

## User-approved important choices

1. Combine the owner's protocol behavior with the existing Profile A RBAC.
2. Use version **0.1.0-r60**, codename `OWNER-PROTOCOL-RBAC-MERGE`.
3. Keep the legacy DER CRL only in
   `archive/owner-upstream-ffb0f19/lab_crl.ejbca.der`; do not select it for active trust.
4. The owner dependency commit `d826f94` is unavailable from the configured
   official library repository. With explicit user approval, reproduce the
   documented fixes on the tested upstream **519b0208** baseline. This is a
   behavior port, not a claim to possess or exactly reproduce that missing fork.

## Protocol integration

| Owner requirement | Combined implementation |
| --- | --- |
| Extended X.225 length indicators | Existing bounded session length encoder/parser retained |
| PGI 193/194 inbound data | Existing parser accepts both, including extended lengths |
| PGI 194 for large outgoing CONNECT/ACCEPT | Added to generated session source at payload >=255 bytes |
| Extra mmsAuthenticationAS context | Existing explicit OID recognition and three-context CPA retained |
| Certificate mechanism and signed AARE AP/AE | Existing verified explicit CERTIFICATE route and signed responder retained |
| Session CONNECT/error diagnostics | Added through a generated iso_connection.c overlay |
| Legacy ACSE bridge cleanup | Owner's empty legacy bridge file retained; typed generated bridge remains active |

Original vendor files are preserved. The generator validates reviewed inputs;
the extension manifest records the intentional additions. TLS chain/CRL checks,
proof freshness/signature/replay checks, calling AP/AE matching, exact signed
transport pairing and MMS permission enforcement remain mandatory. Unknown
explicit authentication mechanisms do not fall back to TLS identity. The
existing bounded support for the authentication context is retained; arbitrary
unknown contexts are not silently accepted.

The root source changes use generated files. Both CONNECT and ACCEPT frames
are captured by the lab probe and checked for actual lengths and correct PGI;
positive responses also require certificate/signature/AP/AE verification.

## Conflict resolutions and local material

- VERSION and PKG_RELEASE synchronized to 60 by explicit choice.
- Both branches' changelog history retained, with owner branch labels distinguishing
  overlapping r57 versions. Root ignore rules and documentation were combined.
- Deleted legacy DER export stays deleted at its former active path; owner's
  bytes are preserved in the approved reference archive.
- No ignored engineering EJBCA bundle, firmware signing keys, operator passphrase,
  build output or device backup was copied into the tracked merge. Historical
  Git-tracked lab certificate fixtures remain present as in the source histories.
- Native validation reads the original protected EJBCA bundle without copying its
  keys into this checkout, and uses a separate evidence directory.
- Source integrity checks accept reviewed LF/CRLF checkout variants. The
  previously mixed-ending AARE source has an explicit canonical LF hash in the
  extension manifest. Both checkout styles produce identical generated code;
  actual source edits are rejected by the checker and generator.

## Verification

**Final native validation passed.**

- Native Linux `ccli`, synthetic server, dual-certificate probe and RBAC test
  targets built successfully as **0.1.0-r60**.
- **5/5 CTest checks passed:** ACSE/role/policy tests, certificate fixture
  generation, signed commissioning/security, TLS authorization/revocation, and
  protected source baseline. Total CTest time: 14.19 seconds.
- **7/7 network cases passed:** DSO CONFIG/CONTROL allowed in synthetic memory,
  Viewer CONFIG/CONTROL denied, replay, bad signature, stale proof, wrong AP and
  wrong TLS/application pairing. The last case can associate but all access is
  denied; association success alone is not counted as authorization.
- Positive CONNECT and ACCEPT captures independently confirm extended lengths
  and **PGI 194**. AARE certificate, issuer, mechanism, AP/AE, timestamp and
  signature verification passed. Session diagnostics were present.
- LF and CRLF checkouts both pass source checks and produce identical generated
  protocol output. Actual source edits are rejected by the checker and generator.
- Final portable generator output is byte-identical to the protocol source
  actually compiled/tested. Modified integration files pass whitespace checks;
  no unresolved merge entries remain.

Native test binary SHA-256:
`339b219fe0f971bfc05a6dbc4ef3372020524c603e7dbfc8712f9066806e14d2`.
This is a host test executable, not a flashable APK. Logs and wire captures are
local-only in ignored `lab/merge-validation/`; persistent Linux build sources
are under `/home/adminavz/ccli-owner-merge-r60-20261016/`. No lab PKI private keys
were copied into that build source or the merge commit.
Actual Test Suite Pro is on a separate unavailable PC and is not a verification
claim for this local merge. No merged build is installed on Tespro.

## Build scope and inherited dependency layout

The inherited repository records dependency Gitlinks but has no root
`.gitmodules`. This local copy contains the recorded libIEC61850 519b0208
checkout and the existing mbedTLS 3.6.0 source required for validation. Do not
assume a fresh clone automatically initializes dependencies. The missing owner
fork was not substituted silently; the documented port was explicitly approved.

Validation is a native Linux build with GOOSE and lib60870 disabled, matching the
MMS component test scope. It is not an OpenWrt APK build, full device/plant test
or proof of the owner r57 freeze's TSP compatibility. Owner freeze/example
profiles are reference assets; do not replace active RBAC/TLS configuration with
their older CRL-off lab settings. No such profile was activated by this merge.

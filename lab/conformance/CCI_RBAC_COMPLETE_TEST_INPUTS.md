# Inputs for a complete CCLI MMS/Profile A test

**Latest update, 16 October:** r59 is installed and signed sequence 3 is active.
Separate TLS/MMS key authentication passed seven host cases, seven isolated
Tespro cases and a main-service read-only test. Retained clients and revoked
certificate rejection passed. The seven-day CRL is deployed. Actual TSP remains
pending because its separate PC is unavailable. See
`CCI_RBAC_TSP_DUAL_CERTIFICATE_INTEGRATION.md`; earlier checkpoints follow.

**On-device update 2026-10-16:** Initial isolated MMS adapter checks on Tespro
passed for Viewer, Engineer revision 0, DSO, datasets/reports, TLS revocation,
uncommissioned-client denial, signed package guards and persistent process
restarts. See `CCI_RBAC_TESPRO_TEST_2026-10-16.md`. Subsequently Profile A was
enabled in the main CCLI service on port 3782, with signed activation, three
role-client reads, Viewer write denial and revoked/unauthorized TLS rejection
verified. See `CCI_RBAC_TESPRO_ACTIVATION_2026-10-16.md`. Full main-service
reporting/TSP, remaining matrix, live revocation/expiry and reboot evidence
remain open.

Date: 2026-10-15. Scope: IEC 61850/MMS; firmware updater excluded.
**Later role-matrix update, 2026-10-16:** all 17 positive certificates passed
selected on-device TLS/MMS checks on the isolated harness: standard revisions
0/1, DSO, partial Aggregator and combined Viewer/Engineer. Includes discovery,
read, dataset, report delivery and allowed/denied CF/control decisions.
Main-service configuration/TLS/state preserved; final main-service Viewer read
passed. Full Aggregator functions, full service/negative/live-expiry matrix,
TSP and reboot checks remain open. See `CCI_RBAC_ROLE_MATRIX_TEST_2026-10-16.md`.

The current CA and working TLS baseline can be retained. Existing certificates
without IECUserRoles cover legacy/negative tests, not positive Profile A grants.

**Update 2026-10-16:** Dedicated EJBCA lab issuance is complete: all 24 test
certificates/PKCS#12 exports and the five-entry signed CRL are verified. A local
owner-signed candidate has 17 positive bindings. The actual host product verifier
accepts 17 positives and rejects seven negative/non-user identities. These inputs
are available in protected `lab/ejbca-staging/rbac/`; they do not need to be supplied
again. See `CCI_RBAC_EJBCA_TEST_2026-10-16.md`. Refresh the CRL before later tests.
Live device/client commissioning, test trust/pins, clock, persistent state/logging
and model-appropriate service/reporting evidence remain necessary. No current
device TLS configuration or Profile A activation was changed by this work.

## 1. EJBCA-issued role identities

Provide exported public certificates and their issuer chains. Keep client
private keys on the test computer; provide their local file paths or a client
keystore accessible to the test tool. Do not paste private keys into chat.

| Identity | Role ID | AoR in current package | roleDefinition | Revision |
|---|---:|---|---|---|
| VIEWER | 0 | CCI | IEC62351-8 | 0 and 1 |
| OPERATOR | 1 | CCI | IEC62351-8 | 0 and 1 |
| ENGINEER | 2 | CCI | IEC62351-8 | 0 and 1 |
| INSTALLER | 3 | CCI | IEC62351-8 | 0 and 1 |
| SECADM | 4 | CCI | IEC62351-8 | 0 and 1 |
| SECAUD | 5 | CCI | IEC62351-8 | 0 and 1 |
| RBACMNT | 6 | CCI | IEC62351-8 | 0 and 1 |
| DSO_OPERATOR | -1 | DSO | IEC 62351-8-CEI016:2021 | 1 |
| AGGREGATOR_OPERATOR | -2 | AGGREGATOR | IEC 62351-8-CEI016:2021 | 1 |

For the full role/revision matrix this means 14 standard-role certificates and
two private-role certificates, or enrollment capability to issue them during
testing. A smaller initial smoke test needs DSO, VIEWER and ENGINEER identities.
Different AoRs/definitions require an agreed signed policy change first.

Certificate profile requirements for this implementation:

- End-entity certificate, valid lifetime, digitalSignature key usage, and an
  appropriate client-authentication profile if the same certificate is used
  for TLS. Issuing CA/chain must be trusted for the selected identity route.
- Noncritical `IECUserRoles` extension OID `1.0.62351.8.1`, containing actual
  ASN.1 DER, not a text string or raw XML. Each record supplies role IDs, AoR,
  revision and roleDefinition. One record per AoR/roleDefinition pair.
- Omit administrative Operation/status-sequence fields from ordinary client
  access certificates. Package sequence is a separate commissioning counter.
- Export PEM certificates; PKCS#12 is usable for client import when required.

### Select the client identity route

1. **Roles carried in the TLS client certificate:** supported by the existing
   stack and local integration test. Add the test certificate to the explicit
   TLS allowlist and bind its identity/transport fingerprints in the signed
   package. Existing working certificates need not be overwritten; use separate
   filenames and retain the original configuration for regression/rollback.
2. **Separate MMS/ACSE identity certificate:** provide both certificates, key
   pairing information and the client's signed authentication/proof format.
   Same-key identity/transport pairing and separate-key proof verification are
   implemented, including explicit AARQ handling and signed responder AARE.
   Separate-key network tests now pass on host and Tespro. The actual TSP ACSE
   exchange still must be demonstrated. This is not a native E2E security claim.

Do not broaden TLS trust to all CA-issued clients to avoid configuring pins.
Role tests can be run in controlled batches using the existing allowlist slots.

## 2. Owner policy signer

- Owner's public signing certificate, valid and permitting digitalSignature.
- Protected local path to the matching RSA >=2048-bit or ECDSA >=256-bit key,
  or an owner-generated signature/package using `scripts/commission_rbac.py`.
  The current tool accepts an unencrypted PEM key kept on the protected
  engineering host. Never install that private signing key on Tespro.
- Confirm the approved signer fingerprint and initial/next package sequence.
  This signer is independent of the SECADM/RBACMNT client role certificates;
  it does not need an IECUserRoles extension to sign the commissioning package.

The generated package contains the three policy/scope XML files, signed
identity bindings and the manifest. Device owner approval of the role/object
allocations is required before activation; the signing script supplies the
integrity envelope, not that approval.

## 3. Trust and negative-test material

- Identity issuer CA chain(s) and current signed CRL for every identity issuer.
- A certificate revoked in the supplied CRL, plus its client key for the
  handshake test. The current revoked lab credential is already available.
- Expired/not-yet-valid, untrusted issuer, no-role-extension, unknown-role,
  wrong-AoR/revision/definition, and unauthorized actor/administrative-role
  cases. Include a valid multi-role identity to test combined grants.
- Stale and invalid-signature CRLs for an isolated test setup; do not replace
  production trust material with deliberately invalid fixtures.

Many negative fixtures already exist in the automated suite. EJBCA enrollment
access or exported equivalents are needed only to reproduce the full real-CA
interoperability matrix; sharing the issuing CA private key is unnecessary.

## 4. Actual model and expected plant allocation

- Existing repository CID/SCL files are already available; no replacement file
  is required merely to enable RBAC. Align the test plan with the actual model:
  the deployed configuration currently uses `mvp_handbuilt`, whereas
  `apps/ccli/config/icd/lab_tg544_eth_a.cid` and its `.cfg` describe a larger
  model. The separate product CID contains only five logical nodes. This is a
  model-selection/test-coverage issue, not evidence that the CID is invalid.
- Actual DSO/Aggregator allocated objects, static datasets and RCB references.
  Permissions use the agreed CEI Tables 22/23; only plant allocation needs
  confirmation, not an invented replacement permission table.
- The full lab CID has no `WSaDAGC1`, `VArSaDVAR1` or `PdCiMMXU1`. The running
  handbuilt model includes only an inactive `WSaDAGC1.Mod` stub for that group,
  not the complete Aggregator model. If those functions are in the product's
  required scope, implement their objects and allocate datasets/RCBs before
  claiming positive coverage. A policy grant does not create model objects.

## 5. Client, device and operating conditions

- TSP version, IED/connection configuration and intended TLS/MMS security mode;
  accessible certificate/key or PKCS#12 paths on its PC. Another approved MMS
  client can supplement automated tests, but is not evidence for TSP itself.
- Test network connectivity, maintenance access and an agreed safe test window
  for write/control operations, so tests do not unintentionally operate a plant.
- Device clock synchronization suitable for certificate/proof lifetime tests.
  The connected device clock differs from the PC; it was not changed during
  the deployment.
- Verified persistent activation-state path. On this Tespro `/var/lib` is
  tmpfs: use an owner-protected persistent location, for example
  `/etc/ccli/rbac-state/activation.state`, and preserve it through upgrades.
- Appropriate persistent security-log storage, protected permissions, sufficient
  space and optional syslog receiver details if remote logging is to be tested.

## 6. Execution once the inputs are available

1. Preserve r58/working TLS credentials and take a fresh rollback snapshot.
2. Verify EJBCA profiles/DER, chains, CRLs and certificate/key pairings.
3. Review exact scopes, create identity/transport bindings and owner-sign the
   commissioning package; install trust/state/logging inputs while Profile A
   stays off.
4. Prove normal TLS clients still work and revoked/invalid clients fail at TLS.
5. Enable Profile A in a controlled test configuration. Run positive and
   negative role/revision/actor/scope tests, discovery, writes/controls, static
   dataset membership and reporting.
6. Test revocation/expiry during live sessions/reporting, signed-proof replay
   where applicable, malformed/tampered packages, same-sequence changes and
   rollback rejection.
7. Verify audit records, reboot/persistent state and signed package upgrades.
8. Repeat the actual TSP workflow and capture evidence/remaining exclusions.

The selected implementation supports the supplied XACML template subset.
File, journal and dynamic dataset services are outside this MMS profile.
This plan does not claim every IEC 62351-8 feature or every device function.

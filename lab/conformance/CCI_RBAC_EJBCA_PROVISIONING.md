# Dedicated EJBCA RBAC lab PKI

Created: 2026-10-15. Updated: 2026-10-16. Scope: IEC 61850/MMS Profile A testing only.

## Latest CRL update — 2026-10-16 08:59 UTC

The Python provisioning tool now sets a seven-day CRL lifetime for the new or
existing dedicated RBAC CA. Applied successfully with `finish`: all 24 current
certificates and PKCS#12 exports verified, signed CRL contains the five expected
revocations, nextUpdate **2026-10-23 10:59:49 Europe/Rome**. This local export
has not replaced the one-day CRL currently loaded on Tespro. Earlier expiry
timestamps below are historical. Main-service Profile A activation has separately
completed; see `CCI_RBAC_TESPRO_ACTIVATION_2026-10-16.md`. Automatic distribution
and reload are described in `CCI_RBAC_CRL_AUTOMATION.md`, not yet enabled.

## Completed provisioning and host verification — 2026-10-16

This section supersedes the interim and paused checkpoints below. EJBCA
provisioning and host verification are complete; live Tespro/TSP commissioning
and conformance evidence remain pending.

- All 24 current certificates and encrypted PKCS#12 exports verified: 14 standard
  role/revision identities, DSO, Aggregator, multi-role, six negative fixtures,
  and a separate owner policy signer. PEM and DER public certificates are present.
- Dedicated role and owner profiles explicitly enable certificate storage and
  certificate-data storage; their v3 configuration markers are present.
- Found and fixed a JSF form-handling bug: checked inputs without a `value`
  must submit `on`, not an empty string. Empty values had cleared profile flags.
  A direct parser check now verifies checked/unchecked handling. Required
  extensions were explicitly configured and verified on the issued certificates.
- CRL verification initially failed because role-profile certificate storage was
  disabled. Recovered the original 23 role certificates and four retired initial
  certificates with EJBCA's strict `ca importcert`, updating only this task's lab
  enrollments. Existing keys, serials and fingerprints were retained. The owner
  certificate already had a database metadata record. Future issuance uses
  storage-enabled profiles. The earlier enrollment revocations alone were not
  adequate evidence of certificate revocation; the exported CRL now proves it.
- Final signed CRL contains five entries: all four initial certificates plus
  `negative-revoked`. Its signature and each current certificate's expected
  revocation status pass. This CRL expires **2026-10-17 09:36:04 Europe/Rome**;
  refresh with the `finish` stage before later tests.
- Owner-signed local candidate: `lab/ejbca-staging/rbac/commissioning-candidate/`,
  sequence 1, four package file hashes verified, 17 positive identity bindings.
  This retains the repository scopes; absent model functions still need separate
  implementation/testing. The owner signing key stays on this PC.
- Separate `verification-only/` package binds six negatives to reach role/CRL
  rejection checks. **Never deploy this verification package.**
- Native product verifier: **17 positive identities accepted, 7 negative/non-user
  identities rejected**. Standard revisions 0/1, private actors, multi-role,
  Viewer read/control denial and DSO RW/measurement-RO decisions pass. No network
  MMS operation or plant control was performed by this checker.
- All 30 saved working PC TLS files retain their hashes. Protected TLS/ACSE source
  baseline check passes. No Tespro configuration, trust, keys, firmware, model or
  activation state changed; Profile A remains disabled on the device.

CA SHA256: `c71e4b50ea27be45d334d7b1f3f65f7004d6aaa5fb0c4417215d396509f34470`.
Owner signer SHA256: `4897c6b465ec61f4261f70846209e98cecd809de416c95c40f255ab21f3548e3`.
Candidate manifest SHA256: `a7c68a7bd04f900cb52d1371e3f1bd72704518059c8c6245dd4bd7f424e98391`.
Full receipt: `CCI_RBAC_EJBCA_TEST_2026-10-16.md`.

For the existing instance, `profiles` recognizes v3 markers; `issue` verifies
and resumes existing identities; `finish` refreshes the CRL/exports and verifies
the bundle; `package` prepares the positive candidate. `register-issued` is the
recorded one-time recovery stage and skips matching saved recovery fingerprints.
`verification-package` is for host testing only. Do not reconfigure issuer/trust
from stale markers if the container/database has been replaced.

## Historical resume checkpoint — 2026-10-16 (superseded)

Docker Desktop and the existing persistent EJBCA container were started. EJBCA
9.6.3 startup logs confirm the RBAC CA was retained. The corrective owner
profile pass completed; both role/owner profiles now have their v2 markers.
The corrected Viewer, Operator, Engineer and Installer revision-0 certificates
passed post-issuance checks. Remaining identities, CRL and signed candidate
are in progress; final results will supersede this interim status below.
The saved working PC TLS baseline (30 files) matched before resuming.

An on-demand host checker, `mms_rbac_commissioned_pki_check`, was added to the
optional native test build. It uses the actual product verifier against the
EJBCA bundle, with a fresh local activation state and a deliberately separate
verification-only package. It neither contacts Tespro nor changes TLS settings.
Negative fixture fingerprints are bound only in that verification package so
rejection reaches certificate/role checks, rather than merely missing pins.

The owner selected a dedicated RBAC lab CA after inspection found only
ManagementCA and CCI-Firmware-Operator-CA in the current local EJBCA instance.
The issuer of the device's working TLS certificates is not present there.

## Configuration

- Administration: https://localhost:9443/ejbca/adminweb/
- Container: `ccli-ejbca-ce`.
- Dedicated CA: `CCLI-RBAC-Lab-CA`; RSA 3072, SHA256WithRSA, 1825-day validity.
- Role certificate profile: `CCLI-RBAC-ProfileA-TLS`; clientAuth, digitalSignature,
  RSA 2048/3072, 365-day validity; CSR extension override restricted to
  `1.0.62351.8.1`. Role certificates are requested by the trusted lab operator,
  not by an unrestricted self-service role-selection endpoint.
- Owner profile: `CCLI-RBAC-Owner-Signer`; digitalSignature, no TLS EKU and no
  CSR extension override. This is the lab device-owner policy signer, separate
  from the SECADM/RBACMNT access roles.
- Enrollment profile: `CCLI-RBAC-Lab-EE`; only the dedicated CA, the two new
  certificate profiles, and user-generated keys.

Role extensions follow the agreed policy: standard IDs 0 through 6, revisions
0 and 1, AoR `CCI`, definition `IEC62351-8`; DSO -1/AoR `DSO` and Aggregator
-2/AoR `AGGREGATOR`, revision 1, definition `IEC 62351-8-CEI016:2021`.
The IECUserRoles extension is noncritical and contains DER, not text.

## Artifacts and safeguards

Provisioner: `scripts/provision_ejbca_rbac.py` (stages init, profiles, issue,
finish). It resumes saved identities and never deletes existing end entities.

Generated material is in ignored `lab/ejbca-staging/rbac/`, protected by a
Windows ACL granting only the current account and SYSTEM. Each identity has a
PEM certificate, local PEM private key, CSR and password-protected PKCS#12.
`secrets.json` holds local enrollment/token/PKCS#12 passwords; never commit it
or paste its contents into chat. The CA private key is not exported.

The script uses the fixed localhost HTTPS administration endpoint with the
local quickstart self-signed server certificate exception. This is a lab setup.
EJBCA's extension override accepts externally supplied extension content;
the trusted provisioning script and post-issuance verification control it here.
Reference: [EJBCA certificate profile fields](https://docs.keyfactor.com/ejbca/9.6/certificate-profile-fields).

## Historical verification checkpoint — 2026-10-15 (superseded)

**Paused at the user's request on 2026-10-15; resume next session.**

- CA creation completed; CA public certificate exported. Persistent container
  volume confirmed: `docker_ejbca-data` mounted at `/mnt/persistent`.
- Role profile's extension override was saved and read back as enabled with
  allowlist `[1.0.62351.8.1]` and empty denylist. The CLI cannot set this Set-valued
  field, so the script uses the administration form and checks the saved values.
  Automatic approval review rejected temporarily enabling unrestricted override;
  that rejected action was not executed. The form solution saves the OID
  restriction and enabled setting together.
- Initial issuance exposed missing KeyUsage (the first certificates contained
  only IECUserRoles). No such certificate was deployed or accepted as ready.
  Four initial identities `viewer-r0`, `operator-r0`, `engineer-r0`,
  `installer-r0` were revoked for supersession and their public certificates
  archived in `retired-initial/`. `retired-initial.json` records them. Replacement
  enrollments use `-v2` usernames; their local keys/CSRs can be reused.
- Corrective role profile configuration completed: explicitly enables KeyUsage,
  BasicConstraints, SKI, AKI and clientAuth, and restricts use to digitalSignature.
  Local marker `CCLI-RBAC-ProfileA-TLS.v2-configured` records this stage. Corrected
  certificates have **not** yet been issued or validated.
- Owner profile and enrollment profile exist. Corrective owner profile pass was
  interrupted after `availableCAs` was configured. It must finish before issuance.
  The provisioning controller was stopped; no background certificate batch is
  intentionally left running. EJBCA itself remains available.

## Historical resume instructions (completed; use current status above)

1. Read this file and `CCI_RBAC_ROLE_IMPLEMENTATION.md`. Inspect current profile
   markers and EJBCA state; do not assume certificate provisioning completed.
2. Using bundled Python, run `scripts/provision_ejbca_rbac.py profiles` to finish
   owner profile settings. The completed role profile is skipped by its v2
   marker. Preserve the verified extension OID restriction.
3. Run `... issue`. It resumes local artifacts, uses new enrollment names for
   the four retired certificates, and verifies key pairs, signatures, validity,
   explicit non-CA constraints, digitalSignature and clientAuth, plus exact DER
   IECUserRoles bytes. The 24 intended identities include the 16-role/revision
   matrix, multi-role identity, six negative cases, and separate owner signer.
4. Run `... finish` only after all 24 pass. It revokes the dedicated
   `negative-revoked` identity, publishes/exports the CRL, checks its signature
   and all current serials, verifies PKCS#12 exports, writes a public manifest,
   and verifies the working PC TLS files against the saved hash baseline.
5. Optionally run `... package` to prepare a local commissioning candidate using
   the repository policy/scopes, 17 positive identity bindings and an owner
   signature at sequence 1. Verify the signature/hash checks pass; this candidate
   is not proof of a live test or approval to alter the current model.
6. Update this record with verified certificate count, CA fingerprint and final
   results. Then plan controlled device/client testing; no automatic activation.

The staging directory, including `secrets.json`, remains ignored by Git and
ACL-protected for `MINIPC-2022-B\\Administrator` and SYSTEM. Do not print secrets.
The last issuance attempts failed validation and were retired; do not describe
the certificates, final CRL or signed package as complete yet.

No files are deployed to Tespro by this provisioner. Before activation, add
the new issuer as the appropriate RBAC identity trust, configure explicit test
transport pins/trust if using these certificates for TLS, bind actors/roles,
and owner-sign the commissioning package. Preserve the working TLS identities
and validate client interoperability. Certificate issuance alone does not
prove complete RBAC or CEI conformance, or complete the absent Aggregator model.

## 16 October: deployed separate MMS identity update

EJBCA now also has the dedicated `CCLI-RBAC-MMS-ACSE` certificate profile and
`CCLI-RBAC-MMS-EE` end-entity profile for separate application keys. DSO and
Viewer MMS credentials are in `lab/ejbca-staging/rbac/dual-certificate/`.
Reproduce with `scripts/provision_ejbca_rbac_dual.py`; original TLS files remain
unchanged. The signed sequence-3 package adds their explicit TLS bindings and
retains all 17 previous positive bindings. r59, sequence 3 and the seven-day
CRL are now deployed; host/isolated Tespro/main read-only checks passed. Actual
TSP is pending. See `CCI_RBAC_TSP_DUAL_CERTIFICATE_INTEGRATION.md`.

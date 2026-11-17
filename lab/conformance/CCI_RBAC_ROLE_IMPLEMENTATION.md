# CCI IEC 61850/MMS RBAC implementation and standards review

## Annex T C3 source closure — 2026-10-28

Aggregator CONFIG now has an explicit scope for the four allocated control
values and executes through the existing validated control path. A hard bound
keeps control-model attributes, measurements and status protected even under a
broader signed scope. Standard-role CONFIG does not imply CONTROL. Expanded
Table 101 service checks, both standard-role revisions, signed commissioning
checks and CONFIG-only TLS/MMS checks on both model paths passed in an isolated
repository-baseline-plus-patch build. See
`ANNEX_T_RBAC_GAP_CLOSURE_2026-10-28.md` for exact scope, evidence and remaining
deployment/interoperability checks. No appliance policy or firmware was changed.

## Aggregator source update — 2026-10-21

The merged r63 repository now contains the newer CEI 57-142:2026-07 Aggregator model/interface, signed requests, separate P/Q measurements, datasets and buffered/unbuffered reports. Both model paths passed isolated TLS/MMS tests. Plant outputs remain disabled; full physical signed-power tracking remains incomplete. See CCI_AGGREGATOR_INTERFACE_2026-10-21.md for changes, test evidence, inherited TLS baseline warning and commissioning steps. Historical results below describe earlier builds; no new firmware was installed for this update.


**Status: Profile A enabled in the main Tespro CCLI 0.1.0-r58 service on
192.168.0.1:3782; initial main-service checks passed. Full conformance evidence
remains pending.**
**Reviewed: 2026-10-16.** This record supersedes the earlier claim that the
commissioning RBAC implementation was complete. Passing the host unit test
does not establish Annex T or IEC 62351-8 conformity.

## On-device role/revision matrix passed — 2026-10-16

**Subsequent correction:** the user requires the existing dual-certificate TSP
workflow. The unified-certificate matrix below does not validate that route.
User explicitly chose retaining existing TLS client credentials and issuing a
separate MMS/ACSE role certificate with its own key. Consultation is required
before important design choices. Current code gaps and integration work are
recorded in `CCI_RBAC_TSP_DUAL_CERTIFICATE_INTEGRATION.md`.

Tested all 17 positive role identities over actual TLS/MMS on the isolated
Tespro harness with Profile A enabled: seven standard roles at revisions 0 and
1, DSO, Aggregator and combined Viewer/Engineer. The selected read, discovery,
dataset, reporting, CF write and control permission checks passed against the
commissioned policy. Allowed reporting includes actual delivery and disable.
Accepted writes/controls affected isolated memory only; main CCLI stayed on
3782 as PID 3374 with Profile A enabled and its configuration/TLS/state intact.
Final main-service Viewer read passed; isolated listener 3783 is stopped.

Aggregator authenticated, read/discovered its existing WSaDAGC1.Mod stub and
was denied DSO process/config/dataset/report scope. Its full positive functional
control/setpoint coverage remains blocked by absent model functions. One probe
assertion was corrected to recognize MMS per-value access-denied encoding;
only Aggregator needed a rerun, which passed. Product code/policy unchanged.

Full matrix, retained initial failure evidence, safeguards and remaining limits:
`CCI_RBAC_ROLE_MATRIX_TEST_2026-10-16.md`. This tests selected MMS permissions,
not remote security administration, MMS SECAUD log access or complete conformance.

## Seven-day CRL configuration — 2026-10-16 08:59 UTC

At the user's request, `scripts/provision_ejbca_rbac.py` now explicitly sets
the dedicated RBAC CA's CRLPeriod to 604800000 milliseconds (seven days).
Both `init` and `finish` update existing CAs; `crl-settings` applies only that
setting. `finish` checks the actual signed CRL lifetime as well as signature
and revocation. Applied to the existing lab EJBCA CA and generated a replacement:
thisUpdate 2026-10-16 08:59:50 UTC, nextUpdate 2026-10-23 08:59:49 UTC
(10:59:49 Europe/Rome). All 24 certificate/PKCS#12 checks passed again.

This updated EJBCA and the local export only. Tespro still has the CRL deployed
at activation, expiring 2026-10-17 08:14:02 UTC. No device restart or trust/policy
change was made during this lifetime update. Automatic generation, distribution
and safe security-material reload remain to be implemented/configured; see
`CCI_RBAC_CRL_AUTOMATION.md`. A routine full controller restart would reinitialize
hardware I/O and is not an appropriate unattended production refresh method.

## Main-service activation completed — 2026-10-16 08:49 UTC

At the user's request, backed up the active configuration and enabled Profile A
in `/etc/ccli/lab.yaml`, then restarted the installed CCLI service. PID 3374
loaded the owner-signed policy on port 3782, persisted commissioning sequence 1
in a new production state directory and recorded policy/authentication/access
events. Added combined old/new public CA and CRL bundles and explicit DSO,
Engineer-r0 and Viewer-r1 client pins. Original server TLS/ACSE keys and
certificates, all existing TLS files, UCI and installed executable retained their
hashes. I/O, model and operating parameters were retained; restart reinitialized
hardware I/O. No control command was sent during verification.

Passed all five main-service cases: Viewer measurement/configuration reads and
denied same-value CF write; Engineer and DSO measurement reads; pinned revoked
certificate TLS rejection; legacy uncommissioned certificate TLS rejection.
Profile A remains enabled. The isolated listener on 3783 remains stopped and its
sequence-2 state is separate. TSP interoperability, full matrix, live revocation/
expiry, reporting on the main service and reboot checks remain open.

Full activation evidence, configuration hashes, client paths, CRL lifetime and
rollback procedure: `CCI_RBAC_TESPRO_ACTIVATION_2026-10-16.md`.

## Earlier isolated on-device Profile A tests passed — 2026-10-16

Connected to the confirmed Tespro and verified r58, active configuration and
working TLS/MMS. Made a protected device backup. The user approved synchronizing
the device clock from 14 October to the PC's 16 October time so the new
certificates are valid. No timezone configuration change.

Built and ran an isolated, bounded AArch64 MMS adapter harness on port 3783,
using the real RBAC/XACML/TLS adapter implementation, existing server credentials,
same MVP model and synthetic measurements. It initializes no plant I/O. The
normal controller on 3782 was not restarted or reconfigured.

Passed Viewer revision-1 read/write/control decisions, Engineer revision-0
read/configuration access, DSO RW/control versus measurement-RO bounds, static
dataset authorization and report delivery. Pinned revoked certificate rejected
at TLS with alert 44; trusted/unpinned client rejected with alert 49. A valid
pinned but uncommissioned no-role certificate passed TLS and was rejected at
MMS association with an explicit identity-binding audit failure.

Tampered package and wrong owner/signing key rejected without changing test
state. Signed sequence 2 activated; sequence-1 rollback and different content
at sequence 2 rejected; identical sequence-2 restart and subsequent Viewer read
passed. Structured security audit persisted on `/etc` storage.

Stopped the test harness and confirmed port 3783 closed. Original PID 5019,
active YAML/UCI, installed binary and every captured existing TLS-file hash
remain unchanged. Original DSO/Viewer measurement reads and revoked-client TLS
rejection pass on port 3782. Production Profile A remains disabled. The approved
clock correction and retained isolated test fixtures/state are intentional device
changes; this was not a production firmware/RBAC deployment.

Full results and evidence: `CCI_RBAC_TESPRO_TEST_2026-10-16.md`. Retained isolated
state `/etc/ccli/rbac-test-20261016/state/activation.state` is at sequence 2;
do not delete it to reuse sequence 1. Full main-service/TSP, remaining on-device
matrix, live revocation/expiry and reboot tests remain pending. This component
test is not complete CEI/IEC conformance evidence.

Tespro activation procedure: `CCI_RBAC_TESPRO_COMMISSIONING.md` records backup,
signed-package/public-trust installation, additive TLS trust and explicit pins,
persistent state/logging, the exact Profile A YAML fields, controlled tests and
rollback. It is a commissioning runbook; documentation alone makes no device
change or Profile A activation.

## EJBCA provisioning completed; actual product verifier passes — 2026-10-16

Reproducible script usage: `scripts/provision_ejbca_rbac.py --plan` previews
the identity matrix offline; `... all` runs init/profiles/issue/finish/package.
The script now protects output permissions before handling secrets. Recovery
and negative-package stages remain explicit. Full instructions are in
`scripts/README_EJBCA_RBAC.md`. The new command flow was checked offline; it
was not rerun against EJBCA merely to document the existing issued bundle.

Resumed the saved checkpoint and completed the dedicated RBAC lab PKI. All
24 current certificates and PKCS#12 exports pass signature, key-pair, lifetime,
KeyUsage, role DER and revocation checks. The 24 comprise 16 standard/private
role-matrix identities, one multi-role identity, six negative fixtures and the
separate owner policy signer. Standard IDs 0..6 cover revisions 0 and 1.

Fixed the provisioning script's checked-checkbox default (`on`), which had
cleared JSF profile flags. Explicitly enabled certificate/metadata storage on
the new profiles. Used EJBCA strict certificate import to recover 23 original
role records plus four retired initial records with unchanged keys, serials
and fingerprints. The final CA-signed CRL proves all four initial certificates
and the negative revocation fixture are revoked. Earlier enrollment status
alone did not prove these certificates were represented in a CRL.

Prepared an owner-signed **local candidate**, sequence 1, with 17 positive
identity/transport bindings; verified all four package hashes and signature.
Created a separate never-deploy verification package to test negative identities.
The optional native `mms_rbac_commissioned_pki_check` uses the actual product
security/policy implementation: **17 positive identities accepted, 7 negative
or non-user identities rejected**, including revoked certificate, unknown role,
wrong AoR/definition/revision, missing roles, and the owner signer as an MMS user.
The checker also verifies Viewer control denial and the agreed DSO RW/RO scope.
Its initial blanket denial expectation for DSO CONFIG was corrected: CONFIG
includes permitted MMS writes on CEI RW objects; measurement-only objects remain
read-only. The product permission policy was not changed.

Protected TLS/ACSE baseline passes; all 30 saved working PC TLS files retain
their hashes. **No device deployment/activation or TLS change was made today.**
Tespro remains at the last verified r58 configuration, with Profile A disabled.
Live Tespro/TSP interoperability, client route/trust/pins, persistent state and
model-appropriate access/report tests remain pending. Aggregator identity
acceptance does not supply the missing Aggregator model functions.

Details: `CCI_RBAC_EJBCA_PROVISIONING.md` and
`CCI_RBAC_EJBCA_TEST_2026-10-16.md`. Private material remains ACL-protected and
ignored in `lab/ejbca-staging/rbac/`; the CA private key stays in EJBCA.

## Historical EJBCA provisioning checkpoint — 2026-10-15 (superseded above)

The owner authorized creating a dedicated `CCLI-RBAC-Lab-CA` in the existing
local EJBCA at https://localhost:9443/ejbca/adminweb/. The instance contains
ManagementCA and the separate firmware-operator CA, but not the issuer of the
working Tespro TLS credentials. The new CA has been created with RSA 3072 and
SHA256WithRSA. Role/owner profile setup and issuance are in progress; see
`lab/conformance/CCI_RBAC_EJBCA_PROVISIONING.md` for the configuration and final
verification status. Private material is kept in ACL-protected ignored staging;
the CA key remains in EJBCA. No Tespro trust/configuration change or Profile A
activation is performed by the provisioner.

**Session paused by the owner on 2026-10-15 to continue tomorrow.** CA creation,
restricted IECUserRoles override, enrollment profile and corrective role-profile
configuration are complete. Four initial certificates failed KeyUsage checks;
all four were revoked and archived, and none was deployed. Corrective owner
profile configuration was stopped after `availableCAs`; resume the `profiles`
stage, then issue/verify the 24 intended certificates and publish the test CRL.
No corrected certificate batch or signed candidate package is complete yet.
Detailed exact resume steps are in `CCI_RBAC_EJBCA_PROVISIONING.md`.
Tespro remains r58 with its working TLS configuration and Profile A disabled.

## Latest fix and deployment: r58 — 2026-10-15

Model/test-input clarification: the existing repository CID/SCL files are
available and do not need to be supplied again. No CID validity defect was
established by this inspection. The deployed configuration uses `mvp_handbuilt`,
not the larger `lab_tg544_eth_a.cfg` model. The full lab CID lacks
`WSaDAGC1`, `VArSaDVAR1` and `PdCiMMXU1`; the handbuilt model has only an
inactive `WSaDAGC1.Mod` stub for that group. RBAC tests must use actual object,
dataset and report references; full Aggregator positive coverage depends on
implementing the applicable missing functions. No live model/configuration
change was made during this clarification. See the corrected test-input list.

The owner authorized fixing the TLS allowlist validation bypass. Built and
installed **0.1.0-r58 / TLS-ALLOWLIST-REVOCATION-FIX** on the confirmed Tespro
at **192.168.0.1**, using the existing controlled SSH lab deployment workflow.

- Added `apps/ccli/cmake/tls_allowlist_validation.cmake`, which generates a
  corrected verifier during CCLI native/SDK builds. Vendor source files remain
  untouched; the HAL targets compile the generated correction. A pin no longer
  clears certificate/CRL errors. With chain validation enabled, all errors are
  preserved. Explicit pin-only mode waives only NOT_TRUSTED, retaining leaf
  expiry/revocation errors; this device continues to require chain validation.
- Original protected TLS/ACSE/session/presentation source hashes, configured
  TLS calls, allowlist membership and TLS settings remain unchanged. The
  baseline JSON adds only the reviewed correction-overlay hash. This is an
  intentional validation behavior fix, not a claim that the faulty verifier
  remains the effective implementation.
- Direct handshake regression cases pass for valid clients, revoked/expired
  pinned clients, wrong issuer, unknown client, stale CRL and bad-signature
  CRL, plus explicit pin-only behavior. Existing legacy TLS/MMS and Profile A
  authorization/report-revocation integration checks also pass. CTest fixture,
  integration and baseline checks: **3/3 passed, 41.39 s**.
- Native `ccli` and clean OpenWrt Tespro SDK build pass. APK SHA256:
  `336ed6229b80922620eea1e247d3eb4192f5fbbfa5069bbb568132b7dad33e2b`.
  Installed binary SHA256:
  `12dae230d5d45257de88ae9585dcc15744e53ed1f54f0a6dd0c48c881f580786`.
- Real device test now rejects the existing revoked certificate **at TLS**
  with alert **44 / certificate revoked**. Existing valid DSO and Viewer
  credentials still complete TLS 1.2, MMS association and measurement reads.
- All pre-existing active configuration, UCI and TLS files retain their hashes.
  No certificate/key, CA/CRL path, TLS version, model or system-clock replacement.
  Controlled startup and root ownership verified; one process serves the same
  endpoint/model. No rollback was required for r58.
- Rollback archive: `/root/ccli-before-r58/system-backup.tar.gz`.
  Upgrade snapshot: `/etc/ccli-upgrade.FKdBaI`.
- **Profile A remains disabled.** Positive real-CA RBAC tests still require role
  certificates and owner commissioning; existing credentials lack IECUserRoles.
  The complete input list and execution plan are now in
  `lab/conformance/CCI_RBAC_COMPLETE_TEST_INPUTS.md`.

Deployment receipt:
`lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r58_2026-10-15.md`.
The following r57 findings and deployments are historical; the TLS bypass
described there is resolved by the tested r58 correction.

## Historical r57 certificate tests — 2026-10-15

Tested the existing installed credentials without replacing certificates,
changing configuration, restarting CCLI or activating Profile A. PEM file hashes
on the PC match the Tespro copies for all inspected identities and the CRL.

| Test | Actual result |
|---|---|
| Server/MMS, server/TLS, client/MMS, client/TLS and viewer certificates | Valid at test time; expire 2029-01-01. OpenSSL on Tespro validates all six against the installed CA and CRL. |
| Server and client/viewer key pairing | All inspected configured certificate/key pairs match. Client MMS identity and TLS transport share a public key. |
| Current CRL | Current at test time. Existing revoked test certificate is listed. OpenSSL rejects it with verification error 23. |
| IECUserRoles extension | Absent from every current client/viewer/revoked certificate checked, under both recognized OIDs. These credentials cannot provide positive Profile A role tests. |
| Revoked certificate against live TLS | **FAIL:** TLS 1.2 completes despite CRL revocation. Device verifier logs flag `00000002`, but the allowlisted-certificate path clears verification flags. |
| Revoked certificate against live MMS | Association rejected; live read-only client reports connection failure. This does not make the preceding TLS revocation test pass. |

The unchanged upstream callback in
`apps/ccli/third_party/libiec61850/hal/tls/mbedtls3/tls_mbedtls.c`
sets `*flags = 0` for an individually allowlisted leaf. This bypass is a concrete
remaining TLS validation issue, not a missing Profile A extension or a failed
CRL file. No TLS source/settings fix or new APK was applied during these tests.
It needs a targeted correction and successful-client regression before a
TLS revocation conformance claim. Independent RBAC identity revocation checks
do not substitute for TLS handshake validation.

The current credentials remain useful for normal TLS/MMS and negative tests.
Positive RBAC tests require newly issued MMS identity certificates containing
the Profile A role extension, matching owner-signed role/transport bindings and
a signed commissioning package. The existing TLS certificates and client key
pair can remain the baseline; reusing a public key in a new identity certificate
does not modify the old TLS certificate. Owner policy-signing trust must still
be commissioned separately. Profile A remains disabled.

## Historical r57 deployment — 2026-10-15

The owner explicitly authorized upgrade safeguards, a new APK build and SSH
installation with `--allow-untrusted` this time. After confirming the changed
SSH host key, installed **0.1.0-r57 / RBAC-PROFILE-A-SAFE-UPGRADE** on the
connected Tespro at **192.168.0.1** (TesproOS 32-2.1.0, aarch64_cortex-a53).

- Clean OpenWrt 25.12.5 mediatek/filogic SDK cross-build: PASS. APK is
  `lab/tg544-openwrt/ccli-0.1.0-r57.apk`; SHA256
  `e3f4d175f7007d2957632ff58a2d49b866d31f7b97f513dc22ec40750b3cf3fb`.
- Removed bundled live TLS files/private keys from APK payload and from source
  staging. Package pre-upgrade preserves existing `/etc/ccli` and UCI settings
  before old package-owned files are removed. Post-upgrade restores them and
  retains a root-only persistent backup.
- Init pending/hold guards prevent framework-triggered automatic starts.
  Explicit controlled start follows checksum, executable identity and ownership
  checks. Executable/init and policy templates are root-owned.
- Hook simulation: PASS. Actual device migration: PASS. Initial verification
  used unavailable `stat`; automatic rollback restored the original runtime.
  After replacing that check with `ls -ln`/`awk`, repaired only executable/init
  from the same verified, already-registered r57 APK and completed startup.
- Final installed package and executable both identify r57. Binary SHA256 is
  `d7b69c916f2cc79dd0fdfe077e6c68ce1552ddba2d7fc301b04158cb43334593`.
- Every recorded pre-existing CCLI configuration, UCI and TLS file retains its
  original checksum. No TLS credential, CA, trust-key or system-clock change.
  Mutual TLS 1.2 plus live MMS association/measurement read passed both before
  and after installation, using the existing fingerprint-matched credentials.
- One running instance serves the existing `mvp_handbuilt` model on
  `192.168.0.1:3782`. No model replacement or firmware-updater change.
- **Profile A remains disabled.** Existing legacy startup `rbac=on` refers to
  the prior certificate-based control gate, not activation of the new Profile A
  package. Real owner/role provisioning and physical TSP E2E validation remain
  required before enabling Profile A.
- `/var/lib` is confirmed tmpfs on this device. Future anti-rollback state must
  be placed on verified persistent storage and retained across upgrades;
  persistent audit storage must also be provisioned appropriately.
- Persistent rollback archive: `/root/ccli-before-r57/system-backup.tar.gz`.
  Migration snapshot: `/etc/ccli-upgrade.GpLgOL`. No secrets recorded here.

Full receipt and limits:
`lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r57_2026-10-15.md`.
Earlier no-deployment/cross-build statements below are historical snapshots.

## Historical pre-deployment handoff — 2026-10-15

The pause to save the Markdown checkpoint ended; implementation and local
verification resumed and finished. The authoritative current results are in
**Verification after resuming the saved checkpoint** below. Statements marked
historical describe earlier stages, including compile failures that were
subsequently fixed; they are not the current implementation status.

- **Integrated:** Profile A roles, the supplied IEC XACML hierarchy, owner-signed
  commissioning and rollback protection, actor/identity/transport bindings,
  certificate and CRL checks, MMS scope enforcement and structured audit.
- **Verified locally:** native Linux executable and adapter build; Windows
  focused RBAC test; Linux policy, signed-security and audit tests; real
  loopback TLS/MMS permissions and report revocation; XACML schema and protected
  TLS source checks. Detailed test names and evidence are recorded below.
- **Runtime state:** the repository lab YAML still has `rbac_profile_a: false`.
  No APK was deployed, no device service was restarted, and installed TLS
  credentials were not replaced. The firmware updater remains outside scope.
- **Next commissioning work:** prepare the real EJBCA Profile A identities,
  owner signing certificate and signed package; provision identity trust/CRLs
  and durable activation state; verify the existing physical device/TSP TLS
  and explicit ACSE communication before enabling RBAC.
- **Scope limits:** no physical-device/TSP or OpenWrt cross-build verification
  is claimed. Missing Aggregator model objects and allocations are not created
  by an RBAC grant. Full IEC/Annex T conformance is not established by these
  local tests.

Commissioning instructions: `apps/ccli/config/rbac/README.md`.
Owner tooling: `scripts/commission_rbac.py`.
TLS preservation check: `scripts/check_rbac_tls_baseline.py`.

## Active implementation update — 2026-10-15

The user authorized completing the remaining applicable RBAC parts while
preserving the working TLS/ACSE communication. No device deployment, restart,
TLS certificate replacement or production enablement has been performed.

Current source work adds mms_rbac_security.cpp/.hpp for owner-signed package
verification, durable sequence/hash anti-rollback state, individually pinned
actor/role identities, certificate chain/lifetime/CRL validation, explicit
transport-to-MMS identity pairing, and structured security events. The original
TLS known-certificate gate has been restored; a comparison against pre-RBAC
HEAD confirmed the TLS configuration calls and allowlist expression match,
apart from a passive event observer enabled only for Profile A. Protected
TLS/ACSE sources are recorded in CCI_RBAC_TLS_BASELINE.json and checked by
scripts/check_rbac_tls_baseline.py. This is a repository source baseline, not
a fresh live DUT test. The user confirmed this repository is the working-source
baseline; existing handoff notes also describe custom wire support, while the
checked-in libIEC61850 gitlink currently points to upstream 519b0208.

Authentication keeps both previously accepted TLS and CERTIFICATE mechanisms.
RBAC identities must match owner-signed certificate fingerprints and their
actual authenticated transport certificate. Separate identity and transport
certificates sharing a key can use TLS proof of possession; separate keys
require a valid fresh signed MMS authentication proof. Existing repository
client.pem/client_tls.pem public keys were compared and match. No private key
material was printed or modified. Role grants remain unusable until transport
binding succeeds, and certificate/CRL checks are repeated on access.

Additional work covers ordinary writes, READVALUES-filtered discovery, every
static dataset member, immutable report dataset selection, and report-delivery
revalidation. cmake/rbac_reporting.cmake generates a guarded copy of reporting.c
in the build directory; TLS/ACSE/session/presentation source files are untouched.
File, journal and dynamic-dataset services are disabled only under the selected
RBAC MMS profile, matching the existing first-pass PICS exclusions. Exact
Table 22/23 RO/RW bounds are enforced in code independently of scope XML.

The supplied knowledge-base/08-engineering/CCI_62351-8_Extract.md is historical
2011 material. Its clause numbers, VIEW terminology and incomplete token
description must not override the supplied 2020 PDF and current CEI text.
IEC 2020 10.4.4.2 confirms the standard roleDefinition is IEC62351-8 (the
existing value is correct, not provisional). Section 10.4.4.3 identifies
Operation fields as administrative updates, not ordinary subject access:
the parser recognizes these fields, but the MMS session path must reject them
as the wrong token purpose rather than silently applying them as grants.

### Verification after resuming the saved checkpoint

The implementation resumed after the documentation checkpoint. The following
checks now pass on the current source:

| Check | Result and scope |
|---|---|
| Native Linux build | `ccli`, the actual MMS adapter, signed security module and loopback TLS test compile/link with libIEC61850 and mbedTLS 3.6 in WSL. GOOSE is disabled and IEC 104 is a stub in this verification build. No OpenWrt/TG544 cross-build claim. |
| Windows focused parser/policy CTest | `mms_acse_p307_acse_auth`: 1/1 passed. Includes all seven standard roles at revisions 0/1, administrative token rejection for sessions, uint32 sequence bounds, Unicode character limits, reserved RO bounds and SECAUD DATASET denial. |
| Linux parser/policy CTest | `mms_acse_p307_acse_auth`: passed. |
| Signed commissioning/security CTest | `mms_rbac_signed_security`: passed with generated real RSA certificates and CRLs. Covers signatures, digest tampering, rollback/same-sequence changes, actor escalation, issuer/lifetime/revocation, revision 0, proof signature/freshness/replay, BER re-encoding replay and audit metadata. |
| Local TLS/MMS CTest | `mms_rbac_tls_authorization`: passed (10.91 s, plus isolated fixture generation). Checks legacy TLS access without a role extension; Profile A DSO/VIEWER/ENGINEER access; writable/read-only objects; revision 0; actual static dataset/report membership; report reception and stopped delivery after live identity CRL revocation. |
| Persistent audit CTest | `event_store_o14`: passed, including the existing 2048-event wrap check and new RBAC security category/syslog alarm, warning and notice mappings. |
| TLS source preservation | `mms_rbac_tls_source_baseline` and the Windows baseline script: passed. Protected wire sources, TLS calls, allowlist and lab TLS settings match the saved baseline. Nested libIEC61850 checkout remains clean. |
| XACML schema | Both updated XACML documents validate against the official OASIS XACML 3.0 XSD with its import resolved locally. |

The five Linux parser/security/audit/source checks passed together (5/5,
47.37 s). The final loopback test and its fresh fixture passed together (2/2,
28.37 s). Fixture generation is shared between those runs, not an additional
independent security assertion.

Integration testing exposed upstream callback ordering: connection indication
precedes ACSE authentication. Identity binding is now established on the first
authenticated service using that actual connection, before granting access.
The test also checks `MMS_DATA_ACCESS_ERROR` values rather than treating a
successful transport response as successful authorization. Report tests use
the client API's required `.RP.` reference syntax.

Replay detection is keyed to certificate identity and signed timestamp, so
changing the BER wrapper cannot bypass it. Unbound observer entries left by
failed ACSE attempts can be reclaimed without evicting active associations.
Signed-time calendar normalization is rejected. UTF-8 character limits now
apply in both token parsing and scope compilation; NFC normalization remains
a commissioning recommendation rather than an implemented transformation.

The package README and YAML examples now describe owner signing, identity
bindings, CA/CRL inputs and durable activation state. The OpenWrt package
includes the inactive RBAC templates under `/usr/share/ccli/config/rbac`.
The temporary scope migration helper has been removed. No release/version
bump, APK deployment, service restart or production enablement was performed.

The optional Windows build of the full event-store dependency tree failed on
existing Linux-only code (`S_ISDIR`/`mkdir`, `unistd.h`, and a missing standard
header in plant-envelope code). The focused Windows RBAC target passes; the
audit-store verification above is Linux evidence. Unrelated Windows platform
code was not changed to manufacture a full Windows appliance build.

**Remaining deployment gates:** provision the real owner's public signing
certificate and signed package, EJBCA Profile A identities with exact permitted
roles/AoR/revisions, transport pairings, identity CA and current applicable CRLs,
and persistent protected activation storage. Then validate the actual physical
device and existing TSP TLS + explicit ACSE flow before enabling Profile A.
The loopback client uses TLS certificate authentication; separate signed MMS
identity proof is tested in the crypto test, not through the physical TSP.
The upstream checkout's missing optional custom AARE bridge remains an existing
baseline limitation and was not patched as part of RBAC.

The compiler still implements the supplied IEC XACML template subset, not an
arbitrary XACML PDP. File/journal/dynamic-dataset services remain excluded from
the selected MMS profile. Aggregator grants do not create missing plant LNs or
dataset/RCB allocations. These limits and the required physical-device evidence
prevent a claim of complete IEC 62351-8 or Annex T appliance conformance.

### Historical saved implementation checkpoint before context compaction

This checkpoint records source changes, not a claim that the new code has
passed testing. Sections 4–8 below retain the earlier review and its historical
test evidence; where their status conflicts with this checkpoint, this
checkpoint describes the newer source. Earlier passing builds do not validate
the security and adapter changes listed here.

#### Owner commissioning and authenticated identity

- Added `mms_rbac_security.hpp/.cpp`: verification of an owner-signed manifest,
  SHA-256 digests of four fixed package files, a pinned owner certificate, and
  durable sequence/package-hash state. Lower sequences and a changed package
  at the same sequence are rejected; identical packages can survive restart.
  POSIX activation uses a temporary file, fsync and rename. The state directory
  must already exist. Policy compilation, identity bindings and trust inputs
  must succeed before activation state is committed.
- The signed package contains `commissioning-policy.xml`,
  `permission-library.xml`, `commissioning-scopes.xml` and
  `identity-bindings.xml`; `commissioning-manifest.xml` carries its signature
  and sequence. `scripts/commission_rbac.py` provides `bind` and `sign` commands
  using Python cryptography. It does not replace existing TLS credentials.
- Signed bindings authorize individual identity and transport certificate
  fingerprints, actor and role IDs. DSO bindings allow only -1/0, Aggregator
  only -2/0, and OWNER standard roles 0–6. Arbitrary CA acceptance alone cannot
  authorize an administrative role.
- Identity validation checks the configured CA chain, certificate lifetime,
  digitalSignature key usage, minimum signing-key size and an applicable CRL.
  A separate RBAC identity CRL can be configured without changing TLS CRL
  settings. Long-lived associations revalidate identity access; the adapter
  caches that result for at most the current UTC second.
- Transport binding requires the actual TLS peer fingerprint and either the
  same public key as the identity certificate or a verified fresh signed MMS
  authentication proof. Proof signatures, time bounds and replay checks are
  implemented. Both raw and embedded certificate authentication data are
  recognized. This has not yet been demonstrated with the physical TSP client.

#### Role, scope and MMS enforcement changes

- Standard role policy sets and commissioning scopes now include revisions
  **0 and 1**. Private DSO/Aggregator roles retain revision 1. The standard
  roleDefinition is `IEC62351-8`, confirmed by the 2020 text.
- Profile A parsing recognizes optional administrative Operation and uint32
  sequence fields. Such tokens are rejected for ordinary MMS session access;
  these fields do not authorize MMS operations. DER UTF-8 limits are counted
  as characters. Compiler-side Unicode limits still need review.
- Updated the scope data to CEI 57-142 Tables 22/23 and the current SCL model.
  Added an independent private-role RO/RW guard in code, so a broader owner
  scope cannot turn a reserved read-only DO into a writable one. DSO and
  Aggregator object allocation remains separate. Aggregator dataset/report
  grants await actual Aggregator model allocations.
- Corrected SECAUD: LIST, READVALUES and REPORTING are present; DATASET is not
  granted by that standard role's permission group.
- Added general data-attribute write enforcement and control checks across the
  model. Discovery intersects LIST with READVALUES, including ancestor and
  bulk-read handling. Dataset access checks every static member against the
  session's read scope.
- Report access checks commissioned dataset membership. Private roles can
  write only RptEna; DatSet and whole-RCB writes are blocked to preserve that
  membership. RCB pointers are captured before server creation because the
  runtime repurposes their sibling links.
- `cmake/rbac_reporting.cmake` generates a guarded reporting source in the
  build directory, rechecking authorization before queued report delivery.
  Its source anchor must match or CMake fails. The libIEC61850 checkout and
  protected TLS/ACSE/session/presentation files are not edited by this overlay.

#### Configuration, audit and TLS baseline

- Added configuration fields `rbac_owner_cert`, `rbac_activation_state`,
  `rbac_identity_ca` and `rbac_identity_crl`. Profile A also requires TLS chain
  validation and the enabled persistent event log. The template
  `identity-bindings.xml` is intentionally empty; no production identity or
  owner signature is invented. `rbac_profile_a` remains **false**.
- Added structured JSON security events with subject, issuer, serial,
  certificate fingerprints, transport binding, actor, roles, policy sequence,
  service, object and decision reason. Events use the existing security event
  category and persistent EventStore; syslog severity mapping was extended.
  This does not add an MMS security journal service.
- Recorded protected source hashes, TLS configuration calls, allowlist gate
  and lab TLS settings in `lab/conformance/CCI_RBAC_TLS_BASELINE.json`.
  `scripts/check_rbac_tls_baseline.py` checks this repository baseline. The
  original allowlist gate was restored and compared against pre-RBAC HEAD.
  The only added TLS configuration call is a passive Profile A event observer.
  The user confirmed **this repository is the working baseline**; no alternate
  source checkout is being requested. A live TLS regression remains required.

#### Tests written and verification still pending

- Added `tests/unit/mms_rbac_security_test.cpp` for signed activation,
  rollback/tamper rejection, role/actor restrictions, certificate trust,
  lifetime, revocation, signed proof replay and audit metadata.
- Added `tests/integration/rbac_fixture.py` to generate isolated temporary
  CAs, certificates, CRLs, signed packages and a deliberately unsafe dataset.
  These fixtures do not modify installed credentials.
- Added `tests/integration/mms_rbac_tls_test.cpp` for real local TLS/MMS
  connections: legacy access, DSO/VIEWER/ENGINEER permissions, revision 0,
  read-only enforcement, dataset/report membership and revocation during
  report delivery. This source has not yet been compiled or run, and its
  CMake target still needs wiring. The security fixture/test registration is
  present; the tests have not yet passed.
- The latest native compile stopped on a mbedTLS 3.6 private CA-field access.
  The code now uses the public accessor, but the build has not been rerun to
  completion. Existing parser tests also need updates for the newly recognized
  administrative fields. Earlier Windows/Linux passes apply only to the
  preceding implementation.
- Still required: finish test wiring and fixes; build the current adapter and
  executable; run focused parser, signed-security and actual local TLS/MMS
  tests; check audit behavior; revalidate changed XACML files against the XSD;
  run the protected TLS source check; update package README/config examples
  and this record with actual results. Remove the temporary, non-idempotent
  scope migration helper rather than rerunning it.
- Deployment gates remain real EJBCA owner/identity provisioning and the
  existing physical device/client TLS + explicit ACSE regression. No target
  deployment, production enablement, complete IEC conformity claim or live
  TSP compatibility claim has been made.

## 1. Confirmed product decisions

| Item | Agreed decision |
|---|---|
| Protocol scope | IEC 61850/MMS only. |
| Token profile | IEC 62351-8 Profile A: identity certificate with IECUserRoles extension. Profile B is not required when A is correctly supported. |
| Roles | Seven standard roles (IDs 0–6), DSO_OPERATOR (-1), AGGREGATOR_OPERATOR (-2). |
| Permissions and objects | Use the actual Annex T data tables, preserving RO/RW restrictions and DSO/Aggregator separation. |
| Policy authority | Device owner; explicitly authorized technician delegation may be configured later. |
| Policy delivery | Owner-authorized commissioning configuration, outside MMS. No MMS policy-editing service. |
| Certificate system | EJBCA. CA/profile creation can follow source development, but actual certificates are required for integration tests. |
| TLS integration direction | Preserve the previously working TLS setup and ACSE authentication method; add RBAC using the authenticated identity. Source changes are in progress as recorded above; live regression is pending. |
| Firmware | Separate updater APK, entirely outside this RBAC change. No firmware authorization is introduced. |

TLS/E2E certificates “A” and “B” in IEC 62351-4 must not be confused with
RBAC token Profiles A and B in IEC 62351-8.

### Latest agreement: preserve working TLS and add RBAC

The user confirmed that TLS communication worked before this RBAC work and
agreed that the existing working setup should be retained when RBAC is added.
The user initially requested discussion only, then authorized completing the
applicable RBAC implementation while preserving TLS. The checkpoint above
records the subsequent source work; installed credentials remain unchanged.

The intended integration is:

1. Keep the existing working TLS trust settings and certificate validation.
2. Preserve the working ACSE authentication method, including its use of
   separate identity certificates where applicable.
3. Read the Profile A IECUserRoles extension from the authenticated identity
   certificate and bind the resulting roles explicitly to that identity and
   its MMS association. Do not assume that a TLS transport certificate and an
   ACSE identity certificate are interchangeable.
4. Check subsequent MMS operations against the role policy and commissioned
   AoR/object scopes after authentication succeeds.
5. Reissue relevant EJBCA identity certificates if they lack the required
   noncritical role extension; updating the policy alone cannot add roles to
   an existing certificate. Retain the working certificate/trust configuration
   as the regression baseline when preparing replacements.

The identity-binding source must pass verification for the existing TLS/ACSE
combination before enabling Profile A. This checkpoint request changes only
the documentation.

## 2. Applicable edition and corrected table numbering

The local Normativa/cei-0.16.pdf is CEI 0-16:2026-07. Its printed/PDF page
546 replaces the Annex T text with a reference to **CEI 57-142**. The official
CEI 57-142:2026-07 document has been downloaded to
Normativa/CEI 57-142-2026-07.pdf from
https://static.ceinorme.it/strumenti-online/doc/26353.pdf.
Its current references are **4.3.1.4, Table 22 (DSO), Table 23 (Aggregator),
5.3 (RBAC), 5.3.1 (permissions), 5.3.2.1 (Profile A), 5.9.4 (configuration)**.
These are the applicable references for the current local edition.

The older repository extraction of CEI 0-16:2025-12 Annex T contains inconsistent
cross-references:

- T.3.3.1.4, printed pp. 569–570 (PDF pages 576–577 in the consolidated
  extraction), heads the DSO data table **Table 97**.
- Printed p. 571 (PDF page 578) heads the Aggregator data table **Table 98**.
- The following **Table 99** is the ACSI services table, not an Aggregator
  data-object allocation.
- T.3.3.4.3.1, printed p. 579, nevertheless refers to “Table 98” for DSO and
  “Table 99” for Aggregator.

The earlier statement “Table 98 is DSO and Table 99 is Aggregator” repeated the
later cross-reference without checking the actual table headings. It was
incorrect as a description of the supplied document. Object allocation must
follow the data-table contents and actor labels. Object allocations now live
in the separate commissioning-scopes.xml, keyed by role ID and token identity,
without obsolete table numbers in executable identifiers.
The source edition and this inconsistency must be recorded in the conformance
assessment.

The current CEI 57-142 5.3.1 correctly references its Tables 22 and 23. It
retains the private-role IDs, role definitions and dataset/report restrictions
used below. The older Annex T review remains recorded for traceability.

## 3. Requirements verified directly

### CEI Annex T

- **T.3.3.1.4:** identify/authenticate each actor before granting its roles.
  A DSO may present only DSO_OPERATOR or VIEWER. An Aggregator may present
  only AGGREGATOR_OPERATOR (or the specified equivalent) or VIEWER.
  Other authenticated actors receive privileges consistent with assigned roles.
- **T.3.3.4.3:** configure and verify who can assume each enabled role, including
  individual certificates or issuer CAs authorized to request that role.
- **T.3.3.4.3.1:** LISTOBJECTS, READVALUES, CONTROL, CONFIG, DATASET, REPORTING.
  Discovery may include only objects for which the subject has READVALUES.
  Private-role DATASET access is read-only; REPORTING writes are limited to
  report enable; CONTROL/CONFIG are limited to the corresponding reserved
  Data Objects, preserving RO/RW distinctions.
- **Table 100:** dataset services use DATASET; GetBRCBValues/GetURCBValues and
  reporting use REPORTING. Treating every control-block write as REPORTING
  would incorrectly include unrelated SGCB, logging, GOOSE and sampled values.
- **XACML role definition:** must contain RoleID, unique-ID in the PDP domain,
  RoleName, roleDefinition, revision, PermissionGroup, Permission.
- **Private roles:** IDs -1/-2; revision 1.0; roleDefinition
  IEC 62351-8-CEI016:2021.
- **T.3.3.4.3.2:** Profile A or B is required; C/D are optional.
- **T.3.3.4.9.4:** Administrative Domain CA identities represent the owner/delegate.
  DSO trust is separately configured. Configuration requires cryptographic
  authenticity and integrity.

CEI does not name a mandatory XACML XML signer identity or prescribe a single
delivery protocol. Device-owner approval and commissioning delivery are product
choices. A digest stored beside a mutable policy does not by itself authenticate
the owner's approval; the commissioning channel/package must supply that trust.

### IEC 62351-8:2020

The supplied PDF is image based. The following complete pages were inspected:

- **Table 3, printed p. 23 / PDF p. 25:** standard role rights; standard-role
  revisions **1 and 0** must be supported for backward compatibility.
  SECAUD includes REPORTING.
- **10.5.1.3, printed p. 48 / PDF p. 50:** Profile A extension is noncritical.
  Current OID is 1.0.62351.8.1; validators must also accept legacy
  1.2.840.10070.8.1.
- IECUserRoles is a SEQUENCE OF UserRoleInfo. Each entry contains one or more
  RoleIDs (-32768..32767), AoR UTF8String (1..64), revision INTEGER (0..255),
  roleDefinition UTF8String (0..23), optional Operation
  {Add(1), Delete(2), Change(3)}, optional statusChangeSequenceNumber.
  There must be only one UserRoleInfo per AoR/roleDefinition combination.
  Unicode NFC normalization is recommended.
- **13.3–13.4, printed p. 61 / PDF p. 63:** at least one profile is required;
  mandatory token components and standard-role revisions 0 and 1 must be handled.

Profile A is defined in **10.5.1**, not “Clause 12” as previously stated.

## 4. Historical implementation found by the earlier review

**Historical snapshot:** the active checkpoint above supersedes these status
entries. They describe the gaps that prompted the current implementation work.

| Component | Actual status |
|---|---|
| Role IDs | All nine are defined. This alone does not implement their required services. |
| Profile A DER extraction | Implemented and tested at parser level; it is not a certificate signature verifier. |
| TLS binding | ACSE_AUTH_TLS obtains the TLS peer certificate in the bundled libIEC61850 acse.c. Profile A currently accepts only that mechanism. |
| Issuer authorization | **Missing:** TLS CA acceptance is not a per-actor/per-role issuer restriction. |
| Policy | Follows the supplied IEC Role PolicySet → PermissionGroup → Permission template. Nine role definitions use typed IEC attributes, UUID identifiers, Version 1.0 and references into a reusable local library. PolicySets use permit-overrides; Permission Policy/Rules use the template's algorithm and independent resource/action targets. |
| Commissioning scopes | AoR and plant object allocation are separate from role semantics. The compiler intersects owner commissioning scopes with the referenced role permissions; scopes cannot invent an additional action. |
| Policy integrity | **Missing:** owner/package signature or authenticated commissioning verification, pinned digest, anti-rollback and atomic activation. |
| MMS checks | Callbacks exist for reads, listing, datasets, control blocks and registered controls. Completeness is not established. |
| CONFIG | **Missing:** general write-access enforcement; a CONFIG rule does not install write handlers. |
| Discovery | **Incomplete:** ancestor discovery and the READVALUES intersection need enforcement. |
| Datasets/reports | Named dataset and RCB checks do not yet verify all member objects against READVALUES or prevent cross-scope data exposure. |
| Object map | Partial. Corrected current-control targets and measurement/report references; full reserved-table allocation remains to be reconciled with SCL/CID. |
| Standard roles | Template supports only revision 1 and uses a provisional roleDefinition; revision 0 compatibility is missing. |
| Audit | Generic success/reject/deny strings only; subject, certificate serial/issuer, roles, policy revision, object/service and reason are incomplete. |
| Enablement | Existing lab YAML has rbac_profile_a: false. Do not enable for production before the blockers below are resolved. |

The original Aggregator prefix LD_Plant/Aggregator was an invented placeholder,
not an Annex T model path. The prototype now uses WSaDAGC1, VArSaDVAR1 and
PdCiMMXU1 references; Aggregator dataset/RCB grants are omitted until actual
plant allocations exist. This does not claim those services are implemented.

### TLS and ACSE changes currently present in the prototype

A source diff review after the template changes found:

| Item | Current code behavior |
|---|---|
| TLS protocol version | TLS 1.2 minimum and maximum remain unchanged. |
| Server credentials and trust loading | Existing server key/certificate paths, CA loading and CRL loading remain unchanged. |
| Profile A disabled | The prior TLS certificate allowlist and authentication branch are preserved. The lab YAML explicitly sets rbac_profile_a: false. This is a source-level observation, not a live regression test. |
| Profile A enabled | The added condition disables the TLS known-certificate-only restriction and uses configured CA-chain validation. This differs from the working allowlist setup and must be reconciled with the latest agreement above. |
| Profile A association authentication | issue_profile_a_token currently accepts only the TLS authentication mechanism and reads the TLS peer certificate. An explicit ACSE certificate mechanism is rejected by this branch, even if that mechanism previously worked. This limitation must be corrected for the agreed integration. |
| Startup requirements | Profile A currently requires TLS enabled, chain validation enabled and all policy-package files successfully loaded. |
| Live communication evidence | The previously working device/client connection has not been retested after RBAC changes. Successful builds and parser/policy tests do not establish that connection still works. |

The preceding TLS discussion and this Markdown update made no runtime changes.
Preserving the working TLS/ACSE path is now a required follow-up, rather than
accepting the prototype's changed trust/authentication behavior as final.

## 5. Corrections made during this recheck

1. Fixed invalid C++ static_cast operations between unrelated libIEC61850
   C structs in the control callback.
2. Bound object-prefix matching to object boundaries, so WlimDWMX1 does not
   grant access to WlimDWMX10.
3. Applied DATASET permissions to dataset operations; private-role mutation is
   rejected even if the role also has CONFIG.
4. Limited private-role report writes to RptEna; whole-RCB and DatSet writes are
   rejected. Report reads use REPORTING. Non-report control blocks no longer
   receive write authorization through REPORTING.
5. Normalized named dataset references from IED-prefixed MMS syntax to the
   policy's LD/LN notation.
6. Hardened DER lengths and signed integers, RoleID bounds, UTF-8, nonempty AoR,
   duplicate AoR/definition entries, duplicate/critical role extensions and
   malformed certificate envelopes. Added legacy OID recognition.
7. Unsupported optional update operations are rejected instead of silently
   being interpreted as grants. Supporting their semantics remains future work.
8. Rejects an association when any claimed role fails the active role/AoR/
   definition/revision policy check.
9. Failed policy reloads clear previous grants; partial parses do not activate
   rules. Numeric suffixes are rejected and XML comments cannot grant access.
10. Restricted existing DSO control/config examples to explicit current control
    DOs, added PdC measurement read references and actual LLN0 dataset/RCB
    references, removed the invented Aggregator object prefix, and marked the
    template as a prototype. Restored SECAUD REPORTING from IEC Table 3 and
    replaced numeric scope labels with actor-based names.
11. Replaced the original custom Rule dialect, and then the incorrect flat
    XACML replacement, with the supplied IEC PolicySet template. The earlier
    flat deny-overrides policy did not follow this template and is superseded.
    commissioning-policy.xml now contains only the root and nine role sets
    (203 lines instead of about 2,340). permission-library.xml (240 lines)
    defines reusable groups and six permissions; commissioning-scopes.xml
    (168 lines) holds AoR and plant allocation separately. RoleID/revision use
    integer-equal and XSD integer; roleDefinition uses string-equal and XSD
    string, with the IEC attribute URNs and MustBePresent=false. Resource and
    action matches follow the supplied template, including the access-subject
    category for action-id. UUIDs provide unique definition identifiers.
12. Added a bounded compiler that follows PolicySetIdReference links only in
    the local package, preserves Target AnyOf/AllOf semantics, and rejects
    duplicate IDs, unresolved references, cycles, wrong types/categories,
    DTDs and unsupported constructs. Only the role root is evaluated; the
    permission library cannot grant access by being present in the package.
    All three files must be loaded successfully before grants activate.

## 6. Historical release-blocker list from the earlier review

Implementation progress against these blockers is recorded in the active
checkpoint. A source change does not close its verification or deployment gate.

| Priority | Required work |
|---|---|
| P1 | Implement the agreed integration with the existing working TLS trust settings and ACSE authentication method. Remove the prototype's unnecessary coupling to ACSE_AUTH_TLS, bind roles to the actual authenticated identity, and verify the existing device/client connection before enabling RBAC. |
| P1 | Bind certificate/issuer trust to actor identity and allowed roles; reject DSO/aggregator attempts to assume standard administrative roles. |
| P2 | The runtime supports the supplied IEC template, not arbitrary XACML. Conditions, obligations, Deny effects, policy-version selectors and other algorithms are rejected. Expand only if required by an agreed interoperating policy producer. |
| P1 | Authenticate owner commissioning updates; verify integrity and expected revision; prevent rollback; fail closed on malformed configuration. |
| P1 | Complete exact per-DO RO/RW scope from the reserved data tables and the actual generated model. |
| P1 | Cover general writes, discovery, dataset members and report payloads; verify no service bypasses authorization. |
| P2 | Standard role revision 0 compatibility and issuer/profile-specific roleDefinition values. |
| P2 | Complete token optional-field handling, Unicode character limits/NFC policy, certificate lifecycle and long-lived session revalidation. |
| P2 | Structured Annex T audit events and real certificate/issuer/revocation negative tests. |
| Gate | Compile actual MMS adapter and appliance; exercise TLS and E2E/Profile A combinations with EJBCA certificates on target. |

## 7. EJBCA commissioning inputs still needed

- Administrative Domain CA for device-owner/delegate commissioning identities.
- DSO issuer trust and distinct Aggregator/administrative issuer or individual
  certificate authorizations.
- End-entity profile emitting noncritical IECUserRoles DER with agreed AoR,
  roleDefinition and revision.
- Identify which certificate supplies the authenticated identity in the working
  TLS/ACSE configuration. Reissue that identity certificate with the Profile A
  extension if needed, and verify its binding to the association while keeping
  the working TLS transport settings as the baseline.
- DSO, Aggregator, Viewer and standard-role test certificates; wrong-issuer,
  expired, revoked, missing-extension, unknown-role and wrong-revision cases.
- An owner policy-signing certificate/key profile for the implemented signed
  manifest. Provision only the public signer certificate on the device; the
  private signing key stays on the owner's engineering host. The signing tool,
  four-file manifest and anti-rollback activation are implemented and tested.

## 8. Historical validation evidence and limits

These results predate the latest security and MMS enforcement changes. Current
verification status is recorded in the active checkpoint; do not reuse these
passes as evidence for the untested changes.

The focused host CMake target compiles mms_rbac.cpp, mms_xacml.cpp,
vendored pugixml and mms_acse_auth.cpp
independently of the appliance. It covers legacy authentication, valid/malformed
Profile A DER, policy loading, object-prefix collision, custom-role revision
mismatch, invalid UTF-8, RoleID range, unsupported Delete operation,
dataset mutation denial, RptEna-only reporting writes, clearing stale grants,
typed IEC targets, separate AoR binding, missing/cyclic references, duplicate
identifiers, scope escalation, independent resource/action targets, malformed
XML, unsupported Conditions/algorithms and DTD rejection.
Both generated XACML files validate against the official OASIS XACML 3.0
`xacml-core-v3-schema-wd-17.xsd` (with its W3C xml.xsd import resolved
locally). This confirms XML schema structure, not full runtime PDP semantics.
It passes after these corrections on both Windows/MSVC and Linux/GCC (CTest
mms_acse_p307_acse_auth: 1/1 passed on each). No live client, target device or
EJBCA-issued Profile A certificate was used in these parser/policy tests.

The earlier successful test did **not compile mms_adapter.cpp** because the test
was explicitly detached from ccli_adapters. Linux compilation and live MMS
integration must therefore be reported separately; host test success alone
cannot support the earlier completeness claim. A Linux syntax check of the
corrected mms_adapter.cpp against the real libIEC61850 headers has now passed.
The complete native Linux ccli executable also builds successfully in WSL
Ubuntu, build-rbac-linux, with libIEC61850 and mbedTLS enabled (GOOSE disabled,
IEC 60870-5-104 unavailable/stub in this checkout). This is a native Linux
compile/link check, not an OpenWrt/TG544 cross-build or a live certificate test.
After replacing the flat policy with the supplied IEC PolicySet hierarchy,
the focused CTest passes again on Windows and Linux. The complete native Linux
`ccli` target links with the new mms_xacml.cpp compiler, pugixml and the real
MMS/TLS adapter. The packaged-policy tests also verify Aggregator/DSO scope
separation, shared permission references and that an unreferenced library
CONTROL permission cannot give VIEWER control access.

Required integration evidence still includes a live regression of the existing
working TLS/ACSE connection, followed by authentication and MMS permission tests
using the intended EJBCA Profile A identity certificates. That work is planned;
it was not performed during the documentation update.

## Sources

- Normativa/cei-0.16.pdf (2026-07), page 546, and the official
  Normativa/CEI 57-142-2026-07.pdf (4.3.1.4 and 5.3). Older Annex T text at
  Architecture/_extracted_reg_analysis/cei_0-16_consolidated_allegato_t_it.txt.
- Normativa/IEC 62351-8-2020.pdf, Table 3, 10.5.1.3 and 13.3–13.4.
- lab/EJBCA_LAB_SETUP.md.
- apps/ccli/adapters/iec61850_mms/mms_rbac.*, mms_xacml.* and mms_adapter.cpp.
- User-supplied IEC role, PermissionGroup and Permission PolicySet template
  screenshots; package layout documented in apps/ccli/config/rbac/README.md.
- Bundled libIEC61850 acse.c, reporting.c, mms_mapping.c and server API headers.
- OASIS XACML 3.0 core specification and official XML schema at
  https://docs.oasis-open.org/xacml/3.0/ .
- Vendored pugixml 1.16 source and MIT license in apps/ccli/third_party/pugixml/.

## 2026-10-16: separate MMS certificate correction and approved additional TLS pin

The user chose to retain the original TSP TLS certificate/key and issue a
separate MMS/ACSE role certificate with its own key. EJBCA has issued the DSO and
Viewer application certificates; owner-signed commissioning sequence 3 retains
17 earlier bindings and adds the two explicit pairs. Explicit ACSE certificate
authentication and signed responder AARE integration are implemented with
bounded generated vendor overlays. The old unified-identity tests alone did
not prove this TSP route.

The user explicitly approved an additional TLS client entry while preserving
all current entries. The optional `mms.tls_additional_client_cert` now adds the
original TSP transport leaf without replacing the revoked negative fixture or
working role clients. TLS admission still does not assign application roles;
chain/CRL checks, application proof and signed pairing remain required.

See `CCI_RBAC_TSP_DUAL_CERTIFICATE_INTEGRATION.md` for the current verification
checkpoint, exact files and known interoperability limits. The main Tespro
service was still r58, PID 3374, on port 3782 before isolated testing; no claim
of actual TSP success or new APK installation is made at this checkpoint.

### User-requested pause, 16 October around 12:44

Stopped the in-progress host/device builds at the user's request. The approved
additional TLS client setting is implemented and documented. Its updated network
checks, isolated Tespro test and final APK rebuild are still pending. No new APK
was installed; main Tespro r58 and its TLS files/configuration remain unchanged.
Resume checklist and exact artifacts are in
`CCI_RBAC_TSP_DUAL_CERTIFICATE_INTEGRATION.md`, under PAUSED.

## 2026-10-16 final r59 deployment and dual-certificate verification

Resumed at the user's request. **CCLI 0.1.0-r59 is installed on Tespro
192.168.0.1, Profile A enabled on port 3782**, with the user-approved additional
TSP TLS client entry. Original TLS server/client files and UCI settings retain
their pre-update hashes. The owner-signed sequence 3 package is active under
`/etc/ccli/rbac-dual/package/`; production anti-rollback state advanced to 3 and
was never erased. The refreshed seven-day RBAC CRL is deployed and expires on
23 October 2026 at 10:59:49 Europe/Rome. No automated CRL reload was introduced.

Verification completed: four TLS/security CTest checks, seven host and seven
isolated Tespro separate-certificate cases, independent signed AARE checks,
and actual main-service read-only authentication using the old TSP TLS key
plus the new DSO MMS key. Retained DSO/Engineer/Viewer clients still read; TLS
without an MMS role is denied; the allowlisted revoked certificate is rejected
at TLS. Synthetic CONFIG/CONTROL permission tests operated only on the isolated
harness, which is now stopped. The main service was restarted once and its
normal communications-loss fallback remains active. No main-service control
commands were sent by the post-install tests.

Recovery backup remains on the device at `/root/ccli-before-r59/`; all new
private client keys remain on the engineering PC. Actual TSP interoperability
is still pending: the user confirmed TSP is on the separate LAN1 PC and that
PC is currently unavailable. The separate Viewer certificate pair was tested
on the isolated harness; the main additional TLS pin is the approved original
TSP DSO transport identity. Existing unified Viewer remains supported.

Detailed hashes, certificate mapping, evidence, reproduction and remaining
conformance/context/E2E exclusions are recorded in
`CCI_RBAC_TSP_DUAL_CERTIFICATE_INTEGRATION.md`, Final verified status. APK:
`lab/tg544-openwrt/ccli-0.1.0-r59.apk`, SHA256
`ecbcdeacf0dcc3925bcf25ee7df5d5df9bd15954bdb87ed3652459bdfa44d875`.

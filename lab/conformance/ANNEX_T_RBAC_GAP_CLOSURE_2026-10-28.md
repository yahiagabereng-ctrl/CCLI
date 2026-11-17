# Annex T MMS role management source closure — 28 October 2026

Scope: the supplied CEI Annex T T.3.3.4.3, T.3.3.4.3.1 and T.3.3.4.3.2 excerpts,
Tables 101–104, and the Profile A ASN.1 definition. The local current allocation
used for C3 is CEI 57-142:2026-07 Tables 22/23, as already selected for this
product. This record concerns source and isolated host verification. It does
not claim complete Annex T certification or a new Tespro deployment.

## Implemented correction

The AGGREGATOR_OPERATOR XACML PermissionGroup already referenced CONFIG, but
its commissioned object scope did not contain CONFIG. This left the C3 right
without an executable grant.

The scope now grants CONFIG only on these control values:

- `LD_Plant/WSaDAGC1.Mod.Oper.ctlVal`
- `LD_Plant/WSaDAGC1.WSptPct.Oper.ctlVal`
- `LD_Plant/VArSaDVAR1.Mod.Oper.ctlVal`
- `LD_Plant/VArSaDVAR1.VArTgtSptPct.Oper.ctlVal`

`MmsCommissioningPolicy::permits_control_roles` accepts CONTROL or an applicable
private-role CONFIG grant on the exact `Oper.ctlVal` payload. The MMS perform
check invokes it after identity/session verification. Both paths run the
existing control-value checks and command handler: mode values -1/-5, finite
signed setpoints in [-100,+100], command completion and spacing validation.
The raw write handler continues to deny FC_CO cache writes.

A hard Table 23 bound rejects Aggregator CONFIG on control-model attributes,
status, measurements and every unallocated object, even if the owner signs a
broader scope. Standard-role CONFIG never implies CONTROL. DSO/Aggregator
separation, read-only private datasets and RptEna-only report writes remain
enforced. Physical Aggregator outputs stay behind the existing disabled backend.

## Requested requirements

| Requirement | Source implementation and verification scope |
|---|---|
| Seven standard roles and the two private roles | Nine role definitions; standard revisions 0 and 1; private IDs -1/-2 and revision 1. Both standard revisions are checked against all six supported permission classes. |
| Configure who may assume each enabled role | Owner-signed identity/transport certificate bindings, actor-specific role restrictions, certificate chain/date/CRL verification and signed-policy rollback protection. |
| XACML fields | Stable unique PolicySet IDs, readable role names, RoleID, roleDefinition, revision, PermissionGroup and Permission references; scoped object/AoR intersection. |
| LISTOBJECTS and READVALUES | Directory, object listing, type definition, single-object and bulk-read authorization; discovery intersects readable scope. |
| CONTROL and CONFIG C3 | Both private roles have scoped rights; Aggregator CONFIG now resolves only the four allocated control values through checked control services. |
| DATASET C1 | Directory/value reads check every member; private dataset modification is denied. |
| REPORTING C2 | Buffered/unbuffered reports, payload-member checks, live validity checks and RptEna-only private writes. |
| Table 101 association services | Authorized Release and Abort are exercised over actual TLS/MMS. |
| Tables 103/104 token fields | Profile A role payload carries RoleID/AoR/revision/roleDefinition; verified X.509 certificate supplies holder, issuer, serial, validity, signature algorithm and signature. |
| Profile A ASN.1 / OID | Existing bounded DER parser and certificate verifier; role acceptance rejects administrative operation/sequence tokens for ordinary user sessions. |
| Profile A or B | Profile A is selected and implemented. The excerpt requires A or B; Profile B is not a missing requirement for this selected profile. |

FILE, SETTING GROUP and SECURITY columns are blank for the two custom roles in
the supplied table. Adding those services is not necessary to satisfy these
custom-role excerpts. The selected MMS profile explicitly disables file,
journal and dynamic dataset modification services. This does not assert support
for remote security administration or every possible IEC 62351-8 service.

## Verification

All network checks use loopback TLS/MMS, disposable test certificates and
signed fixture packages generated in the build directory. They do not contact
EJBCA or Tespro and do not use production private keys.

The shared working tree was concurrently receiving event/syslog changes. A
direct shared-tree build stopped on an event-store declaration/definition
mismatch. To avoid changing or testing partially written unrelated work, the
final build used an isolated native WSL snapshot: repository commit `15b0a9c`
plus the eight RBAC source/policy/test files recorded in
[`verification-source.json`](../../output/annex-t-rbac-20261028/verification-source.json).
The pinned libIEC61850 commit and bundled mbedTLS 3.6 sources were used. Other
agents' pending event/syslog and TLS working-tree changes are excluded from
this verification; this is not an integrated validation of their work.

Configure/build:

```text
cmake -S /tmp/ccli-annext-rbac-source-20261028/apps/ccli -B /tmp/ccli-annext-rbac-verified-20261028 -DCCLI_BUILD_TESTS=ON -DCCLI_WITH_LIBIEC61850=ON -DCCLI_WITH_LIB60870=OFF -DCCLI_WITH_GOOSE=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build /tmp/ccli-annext-rbac-verified-20261028 --target mms_acse_auth_test mms_rbac_security_test mms_rbac_tls_test -j 6
ctest --test-dir /tmp/ccli-annext-rbac-verified-20261028 -R '^(mms_acse_p307_acse_auth|mms_rbac_signed_security|mms_rbac_tls_authorization)$' --output-on-failure
```

Result: **all three test suites passed, plus certificate-fixture setup (4/4
CTest entries, 22.06 seconds)**:

| Check | Result |
|---|---|
| `mms_acse_p307_acse_auth` | PASS; all standard roles/revisions across six privilege classes, private C3 CONFIG scope, broad-scope/prefix/actor denials and parser/policy regressions. |
| `mms_rbac_generate_certificates` | PASS; disposable fixture generation, including a separately signed package with Aggregator CONTROL removed and CONFIG retained. This is setup, not a separate conformance test. |
| `mms_rbac_signed_security` | PASS; signed policy, actor bindings, certificate/CRL validation, revisions, proof replay and rollback cases. |
| `mms_rbac_tls_authorization` | PASS (17.90 s); Table 101 directory/definition/read/dataset/association paths, CONFIG-only controls and audit, value/scope denials on both models, existing TLS/credential/revocation and buffered/unbuffered reporting cases. |

Evidence: [`ctest.log`](../../output/annex-t-rbac-20261028/ctest.log) and
[`ctest-results.xml`](../../output/annex-t-rbac-20261028/ctest-results.xml).
The initial directory assertion expected only an MMS error, while the server
correctly returned an empty directory. It was corrected to accept either error
or an empty result and still reject any disclosed entries. That first failure
is retained as `initial-directory-check.log`/`.xml` in the same evidence folder;
no product change was made to satisfy that assertion.

## Device activation and remaining evidence

The changed scope must be delivered together with the updated executable and a
complete owner-signed commissioning package at a higher activation sequence.
Existing signed packages cannot be edited in place. No running device policy,
activation sequence or firmware was changed by this source task.

Full TSP interoperability against the selected production CID, all deployed
role/object combinations, reboot persistence, and accredited conformance
evidence remain release checks. Physical signed P/Q regulation is separate
from this role-management implementation and remains outside this closure.

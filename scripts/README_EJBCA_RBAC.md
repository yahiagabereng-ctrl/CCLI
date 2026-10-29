# Create the CCLI EJBCA RBAC lab certificates

The executable workflow is `provision_ejbca_rbac.py`. It reproduces the CA,
profiles, role certificates, CRL and owner-signed local commissioning candidate
used for the October 2026 Profile A tests. It does not install on Tespro.

## Requirements

- Python 3.10+ with `cryptography>=42` and `lxml>=5`.
- Docker Desktop running, with the existing `ccli-ejbca-ce` container ready.
- Local lab EJBCA administration at `https://localhost:9443/ejbca/adminweb/`,
  configured for the existing simple lab administrator access.
- Keep the database/container volume and the protected output directory together
  when resuming. Local completion markers are for this EJBCA instance; do not
  reuse them against a fresh or replaced database.

Install Python dependencies if necessary:

```powershell
python -m pip install "cryptography>=42" "lxml>=5"
```

Run from the repository root. First inspect the offline plan:

```powershell
python scripts/provision_ejbca_rbac.py --plan
```

Run the complete normal workflow:

```powershell
python scripts/provision_ejbca_rbac.py all
```

The script protects the output folder before handling secrets: current Windows
user and SYSTEM on Windows, mode 0700 on Linux. New client keys are generated
locally. The CA key stays inside EJBCA. Existing local keys/certificates are
reused and verified; the normal workflow does not delete enrollments.

## Individual steps

| Command suffix | What it does |
|---|---|
| `init` | Create/reuse `CCLI-RBAC-Lab-CA`, RSA 3072/SHA256, set seven-day CRL validity, export public CA certificate |
| `crl-settings` | Set existing RBAC CA CRL validity to seven days; does not issue a CRL or update a device |
| `profiles` | Create/configure role and owner certificate profiles plus restricted enrollment profile; enable certificate storage |
| `issue` | Issue/resume 24 identities, verify key pairs/signatures/role DER/usages, export encrypted PKCS#12 |
| `finish` | Ensure seven-day CRL validity, revoke the deliberate negative fixture if needed, publish/export and verify CRL lifetime/signature, verify certificates/PKCS#12, export DER and public manifest, check saved TLS-file hashes |
| `package` | Bind 17 positive test identities and create/verify owner-signed candidate sequence 1 |

For example, refresh the CRL and recheck an existing bundle:

```powershell
python scripts/provision_ejbca_rbac.py finish
```

Both `init` and `finish` apply the lifetime to the existing dedicated RBAC CA,
using EJBCA `ca editca --field CRLPeriod --value 604800000` (milliseconds).
`finish` checks the actual signed thisUpdate/nextUpdate interval is seven days
(five-second tolerance for EJBCA timestamp backdating). Certificate lifetimes,
CA keys, role bindings and owner package sequence are not changed by this setting.
An already deployed CRL retains its original expiry until replaced on the device.
See `../lab/conformance/CCI_RBAC_CRL_AUTOMATION.md` for the automation design;
this script does not schedule jobs, distribute CRLs or restart CCLI.

`all` performs these five steps sequentially and stops on the first failure.
Do not use it merely to view status: it can create certificates, revoke the
designated negative-test identity and generate a signed package. Use `--plan`
for an offline preview. Existing unrelated CAs/profiles and working TLS files
are not replaced. The script uses a local self-signed HTTPS exception for its
fixed localhost administrator endpoint; it is intended for this lab setup.

## Identities and profiles

- Standard roles VIEWER=0, OPERATOR=1, ENGINEER=2, INSTALLER=3, SECADM=4,
  SECAUD=5, RBACMNT=6; each issued at revisions 0 and 1, AoR `CCI`, definition
  `IEC62351-8` (14 certificates).
- DSO=-1/AoR `DSO` and Aggregator=-2/AoR `AGGREGATOR`, revision 1, definition
  `IEC 62351-8-CEI016:2021` (two certificates).
- One multi-role identity (0+2), six deliberate negative fixtures and one
  separate owner policy-signing identity (eight certificates).
- Role clients: digitalSignature, clientAuth, noncritical IECUserRoles OID
  `1.0.62351.8.1` containing DER. Administrator-controlled CSR override is
  restricted to that OID. The owner signer has no role extension or TLS EKU.

This instantiates the agreed roles in certificates and signed policy bindings;
it does not create new standardized role definitions or missing MMS objects.

## Output

`lab/ejbca-staging/rbac/` is ignored by Git:

- `rbac_ca.pem`, `rbac_crl.pem` — public issuer and signed revocation list.
- Each identity directory — `.pem`, `.der`, `.key`, `.csr.pem`, `.p12`.
- `secrets.json` — local enrollment/token/PKCS#12 passwords; never publish it.
- `public-manifest.json` — fingerprints, serials, expected roles and CRL validity.
- `commissioning-candidate/` — positive-only owner-signed lab candidate.

Client PKCS#12 import uses `pkcs12_password` from the local secrets file. Keep
private keys on the engineering/test PC. New CA trust and explicit transport
pins still need controlled device/client commissioning before Profile A use.
Sequence 1 is a lab starting point, not an automatic device upgrade sequence.

## Historical recovery and optional host verification

These commands are deliberately excluded from `all`:

- `retire-initial`: historical retirement of the first four certificates missing
  KeyUsage. Do not use as a routine issuance step.
- `register-issued`: one-time strict import recovery for certificates issued
  before storage was enabled. It can overwrite this tool's lab enrollment records,
  retains original certificates, and skips matching saved recovery fingerprints.
- `verification-package`: creates a separate signed package with negative test
  bindings to exercise verifier rejection paths. **Never deploy this folder.**
- `inspect` / `restrict`: profile-form diagnostics and IECUserRoles restriction
  maintenance; normal provisioning uses `profiles`.

The optional native CMake target `mms_rbac_commissioned_pki_check` reads the
bundle and `verification-only` package with a fresh host activation state.
It needs public certificates, not private keys, and makes no network connections.
See `lab/conformance/CCI_RBAC_EJBCA_TEST_2026-10-16.md` for the actual completed
17-positive/7-negative result and `CCI_RBAC_EJBCA_PROVISIONING.md` for history.

## Separate MMS certificate workflow (16 October correction)

The approved setup retains the existing TSP TLS certificate/key and creates a
separate MMS/ACSE role certificate with its own key. Reproduce the EJBCA work with:

```powershell
python scripts/provision_ejbca_rbac_dual.py --plan
python scripts/provision_ejbca_rbac_dual.py all
```

Check the script's `--help` for supported arguments before running. It uses the
existing protected lab bundle and owner key; it does not install firmware or
change the device. Output is under `lab/ejbca-staging/rbac/dual-certificate/`.
The owner-signed sequence 3 package retains the earlier bindings and adds the
explicit MMS/TLS pairs. Never lower or erase device activation state to import it.

See `lab/conformance/CCI_RBAC_TSP_DUAL_CERTIFICATE_INTEGRATION.md` for the TSP
field mapping, verification evidence and remaining interoperability gate.

## Network test targets

On the native Linux build, compile `mms-rbac-dual-host-server` and
`mms-rbac-dual-probe`. Then run `python3 scripts/test_mms_dual_certificate.py`
from WSL for the seven synthetic-server cases. `--device` targets an already
started guarded harness on port 3783. `--main-read-only` sends only association
and measurement-read requests to the installed service on port 3782 and verifies
its signed AARE. These explicit lab tests need the protected certificate bundle;
they are not automatic CTest dependencies. Public on-device test staging is
prepared by `scripts/stage_mms_dual_certificate_test.py`. Preserve every prior
activation state when reusing a test directory.

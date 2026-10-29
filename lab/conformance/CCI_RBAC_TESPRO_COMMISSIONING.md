# Enable Profile A RBAC on Tespro

Date: 2026-10-16. Scope: IEC 61850/MMS only; firmware updater excluded.

## Current status

**Latest verified update, 16 October at 14:33 Europe/Rome:** r59 is installed,
Profile A is active on port 3782 with owner-signed sequence 3. Original TLS files
are unchanged; an additional TSP TLS peer pin is configured, and the seven-day
RBAC CRL is deployed. Separate TLS/MMS keys passed host, isolated Tespro and
main-service read-only checks. Actual TSP is pending (separate PC unavailable).
See `CCI_RBAC_TSP_DUAL_CERTIFICATE_INTEGRATION.md` for exact files and evidence.
The earlier r58 activation history below is retained as background.

- All 17 positive role certificates have subsequently passed selected TLS/MMS
  role/revision tests on the isolated on-device harness, including the remaining
  standard roles, both revisions and combined Viewer/Engineer. Aggregator scope
  isolation and its existing stub read passed; full functions are absent.
  Main-service settings and Profile A activation were preserved. See
  `CCI_RBAC_ROLE_MATRIX_TEST_2026-10-16.md` for exact scope and remaining limits.
- The earlier Tespro installation was CCLI **0.1.0-r58**, with Profile A and
  the TLS revocation fix. It has been superseded by the verified r59 update above.
- EJBCA certificates, signed CRL and owner-signed commissioning candidate have
  been prepared and verified on the PC. The host product verifier accepted 17
  positive identities and rejected seven negative/non-user identities.
- **Main CCLI Profile A is now enabled on 192.168.0.1:3782**, following explicit
  user authorization on 16 October. Signed policy activation, three role-client
  reads, Viewer write denial and revoked/unauthorized TLS rejection passed.
  Original server credentials and executable hashes were preserved. See
  `CCI_RBAC_TESPRO_ACTIVATION_2026-10-16.md` for backup, state and evidence.
- Before main-service activation, an isolated on-device MMS
  adapter harness was enabled on port 3783 for the initial component checks,
  then stopped. Viewer/Engineer/DSO permissions, reports, revoked TLS clients,
  signed policy guards and process-restart state passed; the normal service
  and TLS configuration were preserved during that earlier test. See
  `CCI_RBAC_TESPRO_TEST_2026-10-16.md`.
- With explicit user approval, the device clock was synchronized to the PC.
  Isolated test state remains under `/etc/ccli/rbac-test-20261016/` at sequence 2;
  keep it separate from production commissioning state and do not erase it to
  reuse sequence 1. No full device reboot/TSP test has been performed.
- The existing server TLS certificate/key, current communication settings and
  MMS/ACSE baseline are the starting point for commissioning.

## 1. Preserve the working baseline

Before making device changes:

1. Confirm the intended Tespro, installed build, active service configuration
   and endpoint. The last verified active configuration was `/etc/ccli/lab.yaml`.
   Edit the actual active file, not merely a repository example YAML.
2. Back up the active configuration, `/etc/ccli/tls`, owner/policy trust,
   existing RBAC activation state if present, and service/UCI settings. Record
   hashes and the working client settings; keep the backup protected.
3. Verify a normal TLS/MMS connection and measurement read before changes.
4. Check device time and certificate/CRL validity. Arrange safe conditions for
   any write/control tests; begin with reads and denied operations.

Keep a recoverable legacy configuration while preparing a separate controlled
RBAC test configuration. Retain current server keys, protocol/cipher settings,
MMS/ACSE setup and model selection during the initial test.

## 2. Prepare the PC material

Existing generated bundle:

`C:\Users\Administrator\Desktop\CCLI Alireza\CCLI\lab\ejbca-staging\rbac\`

| Source | Purpose |
|---|---|
| `rbac_ca.pem` | Public certificate of the new RBAC identity issuer |
| `rbac_crl.pem` | Current CA-signed RBAC revocation list |
| `owner-signer/owner-signer.pem` | Public certificate pinned for policy signatures |
| `commissioning-candidate/` | Owner-signed package with 17 positive test identities |
| Selected identity `.pem` / `.p12` | Role-bearing test client credential |
| `public-manifest.json` | Expected roles, fingerprints, serials and CRL validity |

Refresh the CRL and verify the existing bundle before use:

```powershell
python scripts/provision_ejbca_rbac.py finish
```

For the approved TSP setup, retain `apps/ccli/config/tls/client_tls.pem` and
`client_tsp.key` in its TLS panel. Use `dual-certificate/dso-mms.pem` and
`dso-mms.key` in its MMS panel. The existing unified role-certificate clients
remain supported, but the user selected separate keys for TSP.
Its private key stays on the client PC. The owner signing key also stays on the
PC; the CA private key stays in EJBCA. Copy only public trust/client certificates,
CRLs and signed policy material to Tespro.

**Never deploy `verification-only/`.** It deliberately binds negative fixtures
for host rejection tests. Do not use certificates from `retired-initial/`.

## 3. Install the signed package and RBAC public trust

Suggested device layout:

```text
/etc/ccli/rbac/
  commissioning-policy.xml
  permission-library.xml
  commissioning-scopes.xml
  identity-bindings.xml
  commissioning-manifest.xml
/etc/ccli/rbac-trust/
  owner.pem
  rbac_ca.pem
  rbac_crl.pem
/etc/ccli/rbac-state/
  activation.state      # created by successful activation
```

Copy `owner-signer.pem` as `owner.pem` and install the complete candidate's five
XML files together. Protect the directories and files with root ownership and
administrative write access; the activation-state directory should be mode 0700.

The manifest signs hashes of the four package content files. Any edit to their
contents requires a new owner signature. Sequence 1 is the prepared lab starting
point; inspect any existing activation state before selecting a sequence. A
higher sequence updates policy; the same sequence requires identical content.
Never delete activation state to bypass rollback protection.

Use persistent storage for state: **`/etc/ccli/rbac-state/activation.state`**.
On this Tespro `/var/lib` is tmpfs and is unsuitable for persistent anti-rollback
state unless a separate persistent mount has been explicitly configured.

## 4. Configure TLS trust and client allowlisting

The existing server certificate and the new client certificates have different
issuing CAs. They do not need a common issuer or matching keys, but both peers
must validate the other peer:

- The client must continue trusting the CA of Tespro's existing server certificate.
- Tespro must trust the new RBAC CA for the selected new client certificates.
- Tespro must explicitly allow the selected client certificate; CA trust alone
  does not authorize every certificate issued by that CA.
- The owner-signed identity bindings must match the actual client identity and
  transport certificate fingerprints and permitted actor/roles.

The current MMS implementation reads one configured TLS CA-file path and one
TLS CRL-file path. Prepare **separate combined PEM bundles** containing the
existing trust material plus the new RBAC CA/CRL, then point only the controlled
test configuration at these bundles. Validate multi-CA/multi-CRL loading and
handshakes on the actual build before activation. Preserve the original files
and paths in the rollback configuration.

The `rbac_identity_ca` and `rbac_identity_crl` settings below add identity
verification; setting them alone does **not** add TLS transport trust or pins.
The implemented allowlist uses explicit client-certificate configuration fields
with limited slots. Run role tests in batches where needed; do not disable
allowlisting or broaden access to all CA-issued clients.

The prepared candidate binds each new role certificate as both identity and
transport. Confirm the actual client follows that route. A separate MMS/ACSE
identity route needs the corresponding binding and client interoperability
evidence. Preserve the established MMS/ACSE settings until that exchange is
verified; do not guess replacements for the working authentication setup.

## 5. Configure Profile A, persistent state and logging

Provision persistent security event logging with protected storage and sufficient
capacity. Profile A requires security logging. Preserve existing log history and
verify the configured destination is persistent on the actual device.

Once package/trust/pins and client preparation are complete, merge these fields
under the active configuration's existing `mms` mapping:

```yaml
mms:
  # Merge into the existing mapping; retain its other required settings.
  rbac_profile_a: true
  rbac_policy: /etc/ccli/rbac/commissioning-policy.xml
  rbac_owner_cert: /etc/ccli/rbac-trust/owner.pem
  rbac_activation_state: /etc/ccli/rbac-state/activation.state
  rbac_identity_ca: /etc/ccli/rbac-trust/rbac_ca.pem
  rbac_identity_crl: /etc/ccli/rbac-trust/rbac_crl.pem
```

This is the RBAC portion, **not a complete replacement configuration**. TLS must
already be enabled with chain validation. Configure the test TLS CA/CRL bundles
and selected client pins described above separately. Keep Profile A false during
material preparation; set it true only for the controlled activation test.

## 6. Restart and verify

Restart the configured CCLI service after installation and configuration checks.
Check startup logs for successful signed policy/trust loading and Profile A
activation; a running process or legacy `rbac=on` log alone is not proof that
Profile A is enabled.

Test in order:

1. Existing server TLS authentication still succeeds with the intended client.
2. Selected valid role-bearing clients pass TLS and MMS association.
3. Viewer measurement reads succeed; controls/writes are denied.
4. DSO access follows the agreed CEI RW/RO allocation. MMS CONFIG is a write
   permission and is permitted on designated RW objects; measurement-only
   objects remain read-only.
5. Revoked certificates fail at TLS when used as transport identities; missing
   roles, wrong AoR/definition/revision and unauthorized identities fail closed.
6. Static dataset and reporting checks enforce every member's permitted scope;
   role/identity revocation or expiry is enforced during active operation.
7. Security events are recorded and activation state survives a controlled reboot.
8. Complete the real TSP workflow and retain evidence and configuration hashes.

The current model lacks full Aggregator functionality. Do not claim positive
live access tests for objects, datasets or reports absent from that model. Host
acceptance of the Aggregator certificate does not create those functions.

If regression checks fail, stop the controlled test and restore the verified
legacy service/client configuration. Preserve any successfully advanced RBAC
activation state; rollback must not erase its anti-rollback counter. Investigate
the failure before retrying activation.

## Related records

- `CCI_RBAC_ROLE_IMPLEMENTATION.md` — implementation status and history.
- `CCI_RBAC_EJBCA_PROVISIONING.md` — CA/profile provisioning and recovery.
- `CCI_RBAC_EJBCA_TEST_2026-10-16.md` — actual host verification evidence.
- `CCI_RBAC_COMPLETE_TEST_INPUTS.md` — remaining complete-test requirements.
- `../../scripts/README_EJBCA_RBAC.md` — reproducible Python provisioning steps.

This runbook describes commissioning. The actual main-service deployment is
recorded separately in `CCI_RBAC_TESPRO_ACTIVATION_2026-10-16.md` and is not a
complete conformance claim.

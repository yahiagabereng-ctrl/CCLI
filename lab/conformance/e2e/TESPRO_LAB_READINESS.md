# Tespro native E2E lab readiness

Updated: 26 October 2026. The current integrity-only native E2E implementation is ready for preparation of a controlled Tespro bench candidate. No current native E2E APK installation or Tespro runtime result is recorded. The prior r64 deployment is separate historical evidence.

**EJBCA credential work paused, 26 October:** no new credentials were issued or commissioned. Resume from the [native E2E EJBCA handoff](EJBCA_NATIVE_E2E_HANDOFF.md), which records the unavailable Docker service, existing CSR subject-encoding problem, trust-anchor selection, strict issuance/validation steps and device commissioning prerequisites. Existing compatibility/TLS credentials do not establish native E2E readiness.

Latest saved host evidence: 9/9 E2E CTests, 5/5 MMS regressions, the BER ASan/UBSan corpus and a final passing native field campaign. See the [field test report and exact coverage limits](../../evidence/e2e-field-campaign-2026-10-23/RESULTS.md). This supersedes the earlier paused-BER warning and 7-test status.

The saved `apps/ccli/build-e2e-arm64/ccli` was checked on 26 October as a 64-bit AArch64 ELF binary, SHA-256 `5d42af83f7548dcb116cd7a8ded046bfb83566d81e6f1a4e53d620469d1892fa`. It is compilation evidence; rebuild the chosen source candidate and create/verify its APK before installation.

Before connecting a native peer, prepare distinct TLS/application credentials, correct Annex G application identity encoding, applicable current CRLs, owner-signed identity/TLS bindings and explicit native configuration. The repository certificate finding in section 2 is still a provisioning issue; no live-device credential audit is claimed.

The current bench campaign covers integrity-only E2E. Mandatory CEI switchable CBC/GCM application encryption remains an implementation gap; see [applicability review](APPLICABILITY_REVIEW.md). The gap does not prevent testing the implemented integrity-only behavior.

## 1. Reproduce the host check

From PowerShell at the merged repository root:

```powershell
wsl.exe -d Ubuntu --cd '/mnt/c/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged' -- bash scripts/check_e2e_host.sh
wsl.exe -d Ubuntu --cd '/mnt/c/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged' -- ctest --test-dir apps/ccli/build-e2e-crypto -R mms_ --output-on-failure
```

The fast campaign generates disposable PKI and owner-signed policy under the ignored build directory, starts actual localhost TLS/MMS servers, validates protected reads/reports/rekey/release and checks rejected requests caused no unauthorized reads. It also exercises the product adapter and its role/object policy. The separate `e2e_native_resilience` CTest adds provider/OSI/security/application termination, malformed messages, authentication precedence, concurrent rekey, abrupt report disconnects and replay-cache exhaustion. Never put those disposable private keys into a release or a USB bundle.

Resilience epoch cases inject a monotonic clock into the C++ harness for HMAC and both GMAC sizes; production uses the actual steady clock and retains the 900-second minimum. They check that an ignored renewal request produces a signed abort and socket closure. The harness reduces the authenticated-pair replay cache to 64 entries; production defaults to 65,536 within the ten-minute freshness window. Exhaustion closes that association, and the test requires a healthy reconnect. These bounded host checks are not target memory/load or real-time timing qualification.

For the real 15-minute server-requested renewal campaign, run separately from the fast CTest suite:

```sh
python3 -u apps/ccli/tests/unit/e2e_native_stack_test.py \
  --server apps/ccli/build-e2e-crypto/OPTIONAL/e2e_native_server_test \
  --fixtures apps/ccli/build-e2e-crypto/e2e-timed-fixtures \
  --probe apps/ccli/tools/mms-client/e2e_probe.py --timed-rekey
```

This uses a real 900-second epoch with continuous GMAC-256 reads, waits for server reqRekey, verifies renewal acknowledgement and new keys, then verifies traffic beyond the original epoch and signed release. It takes about 15 minutes. Ignored renewal is covered separately by the injected-clock host campaign; repeat it with real elapsed time on the target.

## 2. Prepare target credentials and policy

Preserve the current device configuration, owner policy sequence, public certificate fingerprints and rollback package. Record these before installing a candidate. Use the existing commissioning process for changes.

- Separate TLS and native application certificates/private keys for server and peer. Native startup and peer association enforce distinct leaf certificates. Audit distinct key generation during provisioning too.
- The native application certificate must meet Annex G: one OID-valued subject attribute of type `2.5.4.106`, with its value equal to AP-title followed by AE-qualifier. A CN containing an OID string is not equivalent. Required key usage includes digitalSignature and keyAgreement; RSA at least 2048 bits or supported EC at least 256 bits.
- Trusted application issuing chain and current applicable CRLs, including certificate distribution-point/CRL scope coverage. The selected policy is CRL based; OCSP is not implemented. The current probe supports direct-root application issuance.
- Native application fingerprint, actual peer TLS fingerprint, actor and roles in owner-signed `identity-bindings.xml` and the commissioned policy. Authorization remains defined by the policy XML. Native E2E does not grant extra roles.
- Working system UTC disciplined by GNSS/chrony. E2E timestamp parsing does not replace clock discipline. Record clock state and time quality during device tests.

Existing deployed credentials have not been audited or replaced for this native revision. Confirm them before enabling native-only mode.

**Repository credential finding:** the local `apps/ccli/config/tls/server.pem` and `client.pem` public certificates were inspected on 23 October. Both use subject attribute `2.5.4.106` with a **UTF8String** value. Those specific certificates do not meet the native OID-valued subject requirement. Reissue native application certificates using an issuance process that preserves the ASN.1 OBJECT IDENTIFIER value, then commission their new fingerprints and TLS bindings. Existing compatibility credentials and device files were not changed; this is not a live-device certificate audit.

## 3. Merge explicit native settings

Merge the following into the existing `mms:` block, substituting real commissioned values and preserving the rest of the device configuration. This is a fragment, not a replacement configuration. Do not invent peer identities or repurpose package-signing keys as application keys.

```yaml
mms:
  tls_enabled: true
  tls_chain_validation: true
  rbac_profile_a: true
  # Existing rbac_policy, rbac_owner_cert, rbac_activation_state,
  # TLS certificates/keys/CA/pins and rbac_identity_ca/crl stay commissioned.
  e2e_enabled: true
  e2e_allow_compatibility: false
  e2e_certificate: /etc/ccli/e2e/server-app.der
  e2e_private_key: /etc/ccli/e2e/server-app.key
  e2e_trust_anchors: /etc/ccli/e2e/app-roots.pem
  e2e_crls: /etc/ccli/e2e/app-crls.pem
  e2e_ap_title: COMMISSIONED_SERVER_AP_OID
  e2e_ae_qualifier: 1
  e2e_signature_algorithm: rsa-sha256
  e2e_rekey_seconds: 900
```

Use the actual commissioned AE integer, not an assumed `1`. `ecdsa-sha256` is also accepted with corresponding application credentials. Repeat `e2e_certificate_path` for intermediate certificate files when needed. `e2e_crls` accepts DER or a PEM CRL bundle. Unknown/blank E2E keys or invalid booleans/numbers reject startup. Renewal interval must be 900–86400 seconds.

`e2e_allow_compatibility: false` requires native E2E. The existing GUI/commissioning client uses compatibility ACSE and will not connect in that mode. A deliberate mixed-mode lab needs an explicitly reviewed compatibility setting; it does not make that client native.

## 4. Compile with production protocol dependencies

The tracked `.gitmodules` entry restores the already-pinned lib60870 commit. From a fresh checkout initialize that dependency before the deployment build:

```powershell
git submodule update --init --depth 1 -- apps/ccli/third_party/lib60870
wsl.exe -d Ubuntu --cd '/mnt/c/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged' -- bash apps/firmware-updater/scripts/check-ccli-target-build.sh
```

The helper uses the local OpenWrt 25.12.5 ARM64 SDK and actual libmodbus. MMS/TLS, GOOSE and IEC104/TLS are requested; `CCLI_REQUIRE_PROTOCOL_DEPENDENCIES=ON` prevents silently stubbing missing MMS/IEC104. This is compilation only. Confirm target runtime and complete feature parity before packaging.

## 5. APK and installation

Read `apps/firmware-updater/docs/SIGNING_AND_USB_UPDATES.md` first. Select the next free candidate release and update VERSION, package release and CHANGELOG together; do not overwrite an earlier r64 artifact with changed code. Then use the canonical toolkit:

```sh
bash apps/firmware-updater/scripts/build-ccli.sh
```

Review the generated APK metadata and use the toolkit's manufacturer verification. Signing uses its ignored local `keys/`, `tools/bin/apk` and `artifacts/`. Create the USB bundle with its `scripts/new-bundle.sh` when installation is scheduled. Enter the encrypted operator-key passphrase only at the visible local prompt. Copy only the generated update directory to USB, never private keys or the whole toolkit. Perform a requested USB installation through USB.

## 6. Connect a native test peer

The repository Python peer performs read/rekey/read/signed-release and verifies server identity/signatures/ICVs. Its CLI is read-only. Replace placeholders below with actual commissioned identities and a readable MMS object:

```sh
python3 apps/ccli/tools/mms-client/e2e_probe.py \
  --host 192.168.0.1 --port 3782 \
  --tls-ca /secure/tls-ca.pem --tls-certificate /secure/peer-tls.pem --tls-key /secure/peer-tls.key \
  --app-ca /secure/app-root.pem --crl /secure/app-root.crl.der \
  --certificate /secure/peer-app.der --key /secure/peer-app.key \
  --ap-title PEER_AP_OID --ae-qualifier PEER_AE_INTEGER \
  --server-ap-title SERVER_AP_OID --server-ae-qualifier SERVER_AE_INTEGER \
  --icv hmac --domain REAL_DOMAIN --item 'REAL_LN$ST$REAL_DO$stVal'
```

Repeat with `gmac128` and `gmac256`. Group selection defaults to 14 for RSA and 23 for EC; `--group 28` exercises the additional Brainpool group required by the CEI ECDH capability profile; its complete native/target qualification remains to be recorded. Python and Mbed TLS are separate crypto implementations, but both peers were developed in this project. Arrange an external native peer/accepted TEQ for interoperability and formal assessment; TSP capability must be confirmed for the actual installed version/license.

## Device acceptance sequence

1. Record running version, package/config/SCL/public-certificate hashes and clock state.
2. Verify native handshake, protected values, all ICVs and signed release/reconnect.
3. Verify protected URCB/BRCB reports; safe authorized control/readback in the controlled plant setup; role/object denial; keep outputs in the approved safe state.
4. Verify signature/ICV tampering, replay, wrong identity/role/context, missing/current/wrong-scope CRLs and time failures.
5. Run real server-requested renewal, ignored/overdue renewal, reboot/reconnect and connection/report concurrency cases.
6. Preserve sanitized logs and packet captures with exact expected/actual outcomes. Keep private/session keys out of evidence.

Remaining formal source/profile and protocol-response gaps are in [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) and [SOURCE_ISSUES.md](SOURCE_ISSUES.md). This runbook does not declare all 108 groups closed.

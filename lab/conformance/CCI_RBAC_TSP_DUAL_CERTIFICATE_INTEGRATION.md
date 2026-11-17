# TSP dual-certificate integration checkpoint

Date: 2026-10-16. Latest status: **r59 installed; Profile A active; host and
Tespro automated dual-certificate tests passed. Actual TSP interoperability is
pending because its separate PC is unavailable.** See the final verified status
at the end; earlier checkpoints below retain the implementation history.

## User decisions and required consultation

- User requires consultation before important design decisions; do not silently
  substitute a combined TLS/application identity or change the working profile.
- Explicit answer on 16 October: **keep the existing TLS client certificate and
  key; issue a separate MMS/ACSE role certificate with its own key**.
- Retain Tespro server TLS and MMS/ACSE credentials and existing transport
  settings. Certificate issuance uses the dedicated EJBCA RBAC lab CA.
- Separate keys require verified application signature proof and an owner-signed
  binding to the authenticated existing TLS certificate. A subject-name match or
  certificate embedded in an unverified payload is insufficient.

## Initial findings (addressed by the extension described below)

- Existing `client.pem` and `client_tls.pem` are distinct certificates with the
  same SPKI. This historical shared-key arrangement is not the new user-approved
  separate-key arrangement.
- Actual existing client MMS subject: `2.5.4.106=1.1.1.999.1.12`; preserve its
  AP/AE addressing as the initial compatibility baseline and verify against TSP.
- Upstream libIEC61850 `acse.c` recognizes only the password mechanism and falls
  back to the TLS peer certificate for an unrecognized mechanism. Explicit
  certificate authentication is therefore not delivered as a separate identity
  to the RBAC verifier in the installed build.
- The application has an AARE signing builder, but registration is a logged
  no-op in this checkout. A signed responder-authentication AARE is not currently
  produced by that bridge.
- Session CONNECT/ACCEPT encoding and decoding currently use one-byte lengths;
  separate certificate-authentication payloads require extended-length support.
- Existing `MmsRbacSecurity` validates certificate/time/signature proof and replay
  bounds when present, and binds it to an exact owner-authorized TLS fingerprint.
  Network coverage of the explicit ACSE route must be added; prior unified-role
  certificate tests do not demonstrate it.
- The server-side private credentials must stay on the device. New MMS client
  private keys and owner signing keys stay on the protected engineering PC.

## Planned verification gates

1. Verify the existing profile, certificate AP/AE and exact application encoding.
2. Add narrowly scoped generated vendor overlays, preserving original vendor
   files and TLS configuration/validation implementation. Verify legacy TLS
   behavior and malformed/unknown mechanism rejection alongside the new route.
3. Issue separate application role credentials using the existing RBAC CA; verify
   subject, key usage, roles, signature and separate SPKI.
4. Sign a higher-sequence commissioning package binding the new MMS identity to
   the retained TLS identity; retain all previous activation state.
5. Prove dual-certificate association, responder signature, roles/permissions and
   invalid signature/replay/pairing rejection in host and isolated Tespro tests.
6. Prepare the main-service change and exact TSP field mapping. Actual TSP
   connection must be observed before declaring the integration complete.

Offline CRL delivery remains the separate documented requirement in
`CCI_RBAC_CRL_AUTOMATION.md`; it must not be silently enabled as a daily full
controller restart. Native IEC 62351-4 E2E security must not be claimed from
compatibility/A-profile certificate-authentication testing.

## Implementation and evidence checkpoint — 16 October, dual certificates

The implementation now recognizes explicit IEC certificate authentication in
AARQ, verifies the separate application certificate/signature, checks its calling
AP/AE, and binds it to the authenticated TLS certificate through the signed
commissioning policy. It produces a signed AARE using the existing device MMS
credential. Generated session/presentation overlays support the larger payload
and third authentication presentation context. Vendored originals remain intact.

EJBCA issued independent DSO and Viewer MMS keys/certificates under the dedicated
RBAC CA. Their public details are in the protected bundle's
`dual-certificate/public-manifest.json`. The new MMS profile has no TLS EKU;
the subject retains AP/AE OID 2.5.4.106 and EJBCA also includes a CN. The sequence 3
owner-signed commissioning package contains the previous 17 bindings plus the
new two pairs. No key from the device was exported for this work.

The user also explicitly approved preserving all existing TLS pins and adding
an optional `mms.tls_additional_client_cert` for the original TSP TLS leaf. It
adds only a peer pin: chain/CRL validation remains active, and the pin grants no
MMS role. An empty setting preserves the previous configuration behavior.

The original TLS baseline JSON is retained. A separate
`CCI_RBAC_DUAL_CERTIFICATE_EXTENSION.json` lists the intentional AARE and
allowlist additions and hashes the overlay inputs. The baseline checker verifies
both, including unchanged original TLS validation source and configured lab TLS
settings. This is change detection, not a standards-conformance certificate.

Before the additional-pin change, the host network test passed seven cases:
DSO positive (including CONFIG/CONTROL in the synthetic model), Viewer positive
(with CONFIG/CONTROL denied), replay, invalid signature, stale proof, wrong AP,
and wrong TLS/application pairing. Both positive AARE signatures, mechanism,
AP/AE and certificates were independently verified with Python cryptography.
Three legacy RBAC/TLS regression tests also passed. These checks are being rerun
with the approved additional setting and on the isolated Tespro harness; their
final results must be recorded below before treating the new APK as ready.

### TSP field mapping for the approved DSO pair

| TSP field | File under repository / value |
| --- | --- |
| IED address / MMS port | 192.168.0.1 / 3782 for the main service |
| Local address | PC's connected 192.168.0.x interface |
| TLS public certificate | apps/ccli/config/tls/client_tls.pem |
| TLS private key | apps/ccli/config/tls/client_tsp.key |
| MMS public certificate | lab/ejbca-staging/rbac/dual-certificate/dso-mms.pem |
| MMS private key | lab/ejbca-staging/rbac/dual-certificate/dso-mms.key |
| CA in both panels | apps/ccli/config/tls/root_CA.pem (existing server issuer) |
| CRL in both panels | apps/ccli/config/tls/lab_crl.pem (existing server issuer) |
| Calling AP / AE | 1.1.1.999.1 / 12 |

Keep the existing TLS 1.2/cipher/selector settings. The new RBAC CA/CRL validates
the new client identity on Tespro; it is not a replacement for the old server CA
in TSP. New client private keys stay on the engineering PC.

### Explicit remaining limits

- Actual Test Suite Pro association must still be observed. A C API probe does
  not establish TSP interoperability.
- The tested authentication presentation context is ID 5, matching the project
  convention. Arbitrary/omitted authentication-context interoperability has not
  been proven; the responder builder currently uses reference 5.
- Native IEC 62351-4 E2E security and full IEC/CEI conformance are not established
  by these tests. Full Annex G subject-encoding compliance is not claimed.
- CRL distribution/reload automation remains the separately documented offline
  commissioning requirement. No periodic controller restart was introduced.

## PAUSED at user request — 2026-10-16, approximately 12:44 Europe/Rome

User requested stopping now to continue in about an hour. Host and isolated
Tespro cross-build processes were terminated; their Hangup/termination messages
are intentional, not test results. No new test server was started on Tespro.
No r59 APK was installed and no main-service commissioning files were changed.

### Saved work

- Additional entry explicitly approved by user: keep every current TLS pin and
  add the original TSP TLS certificate. Implemented optional
  `tls_additional_client_cert` in MmsTlsConfig, YAML parser and MMS TLS setup.
  It fails startup if a configured additional certificate cannot load.
- Host and device staging test configurations now exercise the additional entry
  with the old TSP TLS leaf and new separate DSO MMS identity. The existing
  revoked fixture is retained in the tests.
- `scripts/check_rbac_tls_baseline.py` passes using the original baseline plus
  `CCI_RBAC_DUAL_CERTIFICATE_EXTENSION.json`; original baseline not overwritten.
- EJBCA dual-certificate issuance, signed sequence-3 package and prior seven-case
  host evidence remain saved under the protected ignored
  `lab/ejbca-staging/rbac/dual-certificate/` folder.
- Current r59 APK is a PREVIOUS candidate, built BEFORE the additional-pin change:
  SHA256 697b4089ec61fed24d2b530859b698e33d2e75d49044cb0eb9b8514ab2e003f1.
  It must be rebuilt before installation. Likewise, the local device harness is
  stale until its interrupted build completes.

### Last device verification

At 10:37 UTC: main ccli PID 3374, port 3782; installed binary SHA256
12dae230d5d45257de88ae9585dcc15744e53ed1f54f0a6dd0c48c881f580786;
active YAML SHA256 8b2854bd844eb7e219b3a7a33257d29ef2ca78a2854ba3c81a25fd4dbc80be15;
UCI config SHA256 011062951870aee5d25344071f87db1ff38eadc00943d4407ba90eb1945b2c88.
All files in the original TLS hash manifest matched. Production activation state
remained sequence 1; old isolated state sequence 2. Port 3783 was unused.

### Resume from here

1. Resume native host build target `mms-rbac-dual-host-server` in
   `apps/ccli/build-rbac-linux`; run `python3 tmp/test_dual_certificate_host.py`
   in WSL. The additional-pin version has NOT yet run its network tests.
2. Run the three legacy regression tests and confirm tests actually execute
   (CTest root points into `OPTIONAL`; prior evidence is in
   `tmp/dual-cert-regression.log`). Do not accept an empty CTest selection as a pass.
3. Resume `wsl bash lab/tg544-openwrt/build-rbac-device-test.sh`.
4. Prepared but NOT RUN: `tmp/run_dual_device_test.ps1` (expects password in
   CCLI_TG544_PW). Review it before running. It uploads only public staging files
   and a guarded synthetic MMS harness to a new isolated directory, runs seven
   cases on port 3783, retrieves audit evidence, stops its own process and checks
   main-service hashes/state. It uses `tmp/stage_dual_device.py` and
   `tmp/test_dual_certificate_host.py --device`. No plant I/O in the harness.
5. Rebuild the r59 APK after successful checks; stage main-service update with
   backup/recovery and preserve working server/TLS credentials and activation
   state. Important new choices must be put to the user first.
6. Observe an actual TSP connection with the separate certificate panel mapping
   above before declaring interoperability. Native GUI control is unavailable;
   coordinate the user's Connect action and inspect device logs.

No automatic reminder or background continuation was scheduled.

## Final verified status — 16 October 2026, 14:33 Europe/Rome

### Installed and commissioned

- Tespro 192.168.0.1: CCLI **0.1.0-r59**, Profile A enabled on port **3782**,
  last verified PID **3418**. APK installed with the previously authorized
  `--allow-untrusted --no-network` lab exception. Installation guards prevented
  implicit start while restoring credentials; one controlled restart completed.
- All original `/etc/ccli/tls` files, including server/client keys, match the
  pre-update hash manifest. `/etc/config/ccli` is unchanged. TLS 1.2 still works
  with the original TSP transport certificate/key and existing server identity.
- Existing TLS peer entries retained; added
  `tls_additional_client_cert: /etc/ccli/tls/client_tls.pem`.
- Active package: `/etc/ccli/rbac-dual/package/`; owner pin and CA trust unchanged.
  Persistent `/etc/ccli/rbac-state/activation.state` advanced from sequence 1 to
  **3**, digest `0a0712723b81d5bd1a0f4796d595c18162e523da6d92c87dc6ed74eeb606d5b7`.
  Old isolated state remains sequence 2; the new isolated state remains sequence
  3 in its own directory. No state was erased or rolled back.
- The refreshed RBAC CRL is active at `/etc/ccli/rbac-dual/trust/rbac_crl.pem`;
  `tls-crl-bundle.pem` in that directory retains the old server/TLS issuer CRL.
  RBAC CRL nextUpdate: **23 October 2026, 10:59:49 Europe/Rome**.
- Protected recovery backup stays on Tespro at `/root/ccli-before-r59/`;
  package-hook preservation backup `/etc/ccli-upgrade.IlaCmk/` is retained.
  Device private files were not downloaded. Upgrade hold/pending markers are
  absent. The recovery branch was not needed or exercised.

| Artifact | SHA-256 |
| --- | --- |
| `lab/tg544-openwrt/ccli-0.1.0-r59.apk` | `ecbcdeacf0dcc3925bcf25ee7df5d5df9bd15954bdb87ed3652459bdfa44d875` |
| Installed `/usr/sbin/ccli` | `42af88a5a555c11a77ababd827445815f1d741dc1438f385088ca0cec824f64f` |
| Active `/etc/ccli/lab.yaml` | `3d8498ba8efdd468a7d262ccb678f381e4c28e1f545fcf7228df2ef01baa3b7d` |
| Unchanged `/etc/config/ccli` | `011062951870aee5d25344071f87db1ff38eadc00943d4407ba90eb1945b2c88` |
| Isolated test harness | `2cf2e65fb75c35c572b5ad394f4de5c488207139b590d00bf34f601feab393c1` |

### Completed verification

- **4/4 CTest checks passed:** certificate fixtures, signed RBAC security,
  TLS authorization/revocation regression, and source baseline/extension checks.
- **7/7 host and 7/7 isolated Tespro cases passed**, using distinct TLS and MMS
  keys: DSO success, Viewer success, replay, invalid signature, stale proof,
  incorrect calling AP and wrong transport/identity pairing. Server audit reasons
  were checked, not just client connection failures.
- DSO CONFIG/CONTROL allowed and Viewer CONFIG/CONTROL denied only in the
  synthetic isolated model. No plant I/O is initialized by that harness.
- Both positive AAREs were independently verified for the existing server leaf,
  issuer signature, responder AP/AE, certificate mechanism, timestamp and proof.
- On the actual main service: original TSP TLS plus new separate DSO MMS identity
  associated and read a measurement; signed responder proof passed independent
  verification. The existing DSO, Engineer revision 0 and Viewer revision 1
  unified clients still associated/read. Original TLS without an MMS role was
  denied at MMS authentication. The pinned revoked fixture was rejected at TLS
  with the certificate-revoked alert.
- The main-service checks sent no configuration writes or control commands.
  Normal configured controller behavior remains active: after the last client
  disconnected, the audit recorded the existing communications-loss fallback
  (`wlim_off` / PF setpoint cleared). Thus these tests do not claim that plant
  outputs were frozen throughout a controller restart or client disconnect.
- Port 3783 is closed and the isolated harness is stopped. No full device reboot
  or actual Test Suite Pro connection was performed.

The first isolated launcher attempt stopped before startup because the image
has no `nohup`. It was replaced with a shell HUP trap; the successful run above
then completed. Initial backup preparation also stopped on remote `find -exec`
quoting; the archive/configuration were verified and hashing was completed with
a bounded file loop before installation. Neither preparation stop changed the
active main service.

### Reproduction and evidence

Maintained scripts: `scripts/provision_ejbca_rbac_dual.py` (EJBCA),
`scripts/stage_mms_dual_certificate_test.py` (public test staging),
`scripts/test_mms_dual_certificate.py` (network cases), and
`lab/tg544-openwrt/install-r59-preserve.sh` (this guarded lab upgrade record).
The installer expects the pre-upgrade state; do not rerun it on the commissioned
r59 device. Its recovery logic retains a later accepted activation sequence.

Native probe build target: `mms-rbac-dual-probe` in `apps/ccli/build-rbac-linux`.
It permits CONFIG/CONTROL tests only on isolated ports; port 3782 requires the
explicit `main-readonly` label, which does not enter the write/control branch.
Run the maintained runner in WSL:

```sh
python3 scripts/test_mms_dual_certificate.py                 # host synthetic server
python3 scripts/test_mms_dual_certificate.py --device        # prestarted isolated 3783 harness
python3 scripts/test_mms_dual_certificate.py --main-read-only # actual service, reads only
```

Private lab material/evidence stays in the protected ignored
`lab/ejbca-staging/rbac/dual-certificate/`: `host-test/`, `device-test/`,
`main-test/`, `main-compatibility/`, `r59-install.log`, `main-config.diff`,
`final-device-status.txt`. No keys or enrollment secrets are included here.

**Remaining:** actual TSP connection on the separate LAN1 PC, which the user
confirmed is unavailable. Use the distinct TLS/MMS fields above when it returns.
The current main-service separate-certificate pair is DSO. The separate Viewer
pair was exercised on the isolated harness with its own old Viewer TLS pin;
that old Viewer TLS leaf was not added to the main allowlist. Existing unified
Viewer remains working. Other previously noted conformance/context/E2E and
service-matrix exclusions remain; these results are not a full IEC/CEI claim.

# IEC 62351-3:2023 TLS corrections

Updated: 27 October 2026. Scope: merged CCLI source, MMS and IEC 104 TLS listeners.
These changes are **not a complete conformance claim**. Authorized Tespro
deployments are recorded under `lab/tg544-openwrt/deploy-manifests/`. An isolated ARM64 component test on Tespro
passed both 23-case TLS campaigns, seven MMS cases and IEC 104 monitoring/clean
shutdown checks; see `output/iec62351-tls/device/README.md` and its evidence.
Native IEC 62351-4 E2E remains a separate security layer.

## Implemented

| Area | Behavior and relevant clauses |
|---|---|
| Local identity | Validate chain, validity, signature/key strength, server EKU when present, and certificate/private-key pairing before starting either listener (6.4.3, 6.4.4). |
| Trust anchors | Load CA bundles containing multiple roots; a host regression now covers authentication using a client chain rooted at the fifth CA (6.4.1). |
| Peer identity | Mutual TLS requires a certificate from both endpoints. Validate CA trust, chain, validity, usage, SHA-256 certificate signatures by default, RSA >=2048 or P-256 EC. SHA-1 emits the required deprecation warning and is rejected. RSA <2048 emits the required key-length alarm and ends the handshake. Optional SHA-384/SHA-512 certificate signatures can be enabled or disabled by policy. Configured exact-certificate allowlists are checked in addition to CA-chain validation (6.4.3, 6.4.4, 7.5.4). |
| Certificate size | Support each TLS certificate up to **8,192 DER octets**. Reject a larger local identity before opening the listener; reject an oversized peer certificate during handshake, raise the event alarm: TLS certificate size exceeded, and terminate establishment (6.4.2). This limit applies to every certificate in the TLS chain. |
| Versions and suites | TLS 1.2 only. Offer exactly the four mandatory suites: 0xc02b ECDHE-ECDSA/AES128-GCM/SHA256, 0xc02f ECDHE-RSA/AES128-GCM/SHA256, 0x009e DHE-RSA/AES128-GCM/SHA256 and 0x003c RSA/AES128-CBC/SHA256. The conditional NULL suite stays disabled and the optional AES-256 suites are not enabled. Reject CBC without negotiated RFC 7366 encrypt-then-MAC before application data is accepted (Table 1, 7.2, 7.5.4). |
| Version stability | Store the initial negotiated protocol version and compare it after each renegotiation; a change raises an alarm and aborts the session. The TLS 1.2-only min/max policy also prevents a version transition in the current profile (6.2). |
| Handshake signature policy | Require ClientHello signature_algorithms. A certificate-selection callback checks the pinned mbedTLS 3.6 parser's signature list after ClientHello processing and before any server flight. Its legacy SHA-1 fallback raises the deprecation warning and a missing-extension alarm, sends a fatal handshake_failure alert, and aborts. The callback applies to initial handshakes and renegotiations; it is registered only for the strict CCLI policy (7.5.4). |
| CRL processing | Validate every configured CRL's signature, issuer authorization, thisUpdate and nextUpdate. A correctly signed expired CRL warns without blocking listener startup; new handshakes still enforce freshness for the certificate issuer. Each connection owns a private parsed snapshot; reloading never frees data another connection is using. Reject removal of an existing issuer and older thisUpdate values in a live snapshot (6.4.4.4.2). |
| CRL refresh | Re-read the configured local file periodically, and check expiry while idle. Accept authenticated replacements atomically, revalidate active peer/local certificates, disconnect revoked/invalid certificates, and request a full renegotiation for other valid changes. Missing, stale, future-dated or bad-signature refreshes warn and preserve the last valid snapshot; refresh failure alone does not close an established session. New handshakes remain subject to CRL expiry verification. |
| Renegotiation | Configurable 600..86400 seconds, default 43200 (12h). Use a monotonic clock. Both protocol I/O loops call a TLS tick while idle. Enforce RFC 5746 secure renegotiation; an unanswered/incomplete request fails after 30 seconds, with an alarm. Allow up to 16 in-flight application records during the request, within the deadline (7.4.5, 7.5.2). |
| Resumption containment | Disable session resumption in the strict server policy. The old cache can skip peer-certificate verification/observer identity restoration. Always perform a full handshake until that is corrected and tested. This mitigates the bypass but does **not** close resumption feature requirements in 7.4.4. |
| Security events | Record MMS TLS events independently of Profile A; carry severity, code and peer address. IEC 104 bridges TLS events into its own bounded queue, then into the application's persistent event store. Queue overflow is counted and reported as an alarm; all events are also written to stderr. Revocation now emits separate certificate-revoked and session-terminated alarms. Local TLS policy/identity setup failures raise alarms. |

The shared cryptographic feature configuration is now tracked under
`apps/ccli/adapters/security/mbedtls_config.h`. HAL, adapters and tests use the
same configuration. The original vendored TLS/ACSE/session/presentation sources
are preserved; count-checked build overlays generate the corrected transport and
idle-loop sources. `IEC62351_TLS_EXTENSION.json` extends the existing source
baseline with explicit fingerprints and keeps the original baseline history.

## Configuration

Both `mms:` and `iec104:` accept these fields:

```yaml
  tls_crl: /etc/ccli/trust/tls-crl-bundle.pem
  tls_revocation_check_s: 3600
  tls_renegotiation_s: 43200
  tls_resumption_s: 21600
  # Optional certificate-signature hashes; SHA-256 is always enabled.
  tls_allow_sha384: false
  tls_allow_sha512: false
  # Optional peer pins; repeat this key for each individual certificate.
  tls_allowed_client_cert: /etc/ccli/tls/peer-operator.pem
```

Revocation interval: 1..86400 seconds. Renegotiation: 600..86400. Resumption
lifetime: 1..86400, validated and retained for future safe resumption support;
**resumption is currently disabled**. Empty, malformed and out-of-range timer
values make the TLS configuration invalid. A TLS-enabled listener now requires
`tls_crl`; missing, malformed, future-dated or cryptographically invalid CRLs prevent listener startup. Expiry alone raises a warning and leaves the listener available.
The product YAMLs point to `/etc/ccli/tls/lab_crl.pem`, which must be provisioned
with a current, CA-signed bundle before deployment. CRL distribution and renewal
remain operator responsibilities.

SHA-384 and SHA-512 are optional certificate and TLS signature hashes. Both
are disabled by default. Set either `tls_allow_sha384` or `tls_allow_sha512` to
`true` under `mms:` or `iec104:` to permit that hash; SHA-256 remains first in
the TLS `signature_algorithms` offer and is always accepted. SHA-1 is never
enabled by policy and its use raises `warning: deprecated hash algorithm used`
before strict validation rejects the certificate. Omitting the ClientHello
signature_algorithms extension also raises that warning and
`alarm: signature_algorithms extension unavailable; SHA-1 fallback prohibited`.
The server aborts before sending ServerHello, certificates or a signed key
exchange; configuring a signature hash list alone does not prevent mbedTLS
3.6's legacy fallback. The strict server callback uses the pinned provider's
internal signature-list layout, so a provider upgrade requires review.
RSA public keys below 2,048
bits are disabled; their detection raises the clause-specific alarm and ends
TLS establishment. RSA 3,072-bit keys are accepted, and P-256 is the required
ECDSA curve. The conditional NULL cipher suite is not configurable on this
endpoint.

A provisioning service/operator must write a complete CRL bundle to a temporary
file on the same filesystem and rename it over `tls_crl`. Include every issuing
CA's CRL and its CA certificate in the configured trust bundle. Distribution
credentials, authenticated downloads and scheduling are not implemented by this
local reload mechanism. Do not overwrite the active file incrementally.

From r70, startup permits correctly signed expired CRLs with a warning; it
continues to reject invalid CRLs and revoked or invalid endpoint certificates.
Initial TLS handshakes enforce freshness for the applicable issuing CA. Publishing
a fresh signed CRL restores authentication on the next connection without a
listener restart. During an established connection,
refresh failure retains the last accepted snapshot and warns. Previously known
revocation entries continue to apply; expired CRL availability alone does not
terminate that established connection. Snapshot rollback protection is local to
that connection: persistent CRL-number anti-rollback across process restarts is
still needed for a stronger provisioning workflow.

## Remaining work — required before closing the IEC 62351-3 profile

1. **OCSP stapling (6.4.4.6, 7.5.5/Table 12).** The bundled mbedTLS 3.6 TLS HAL
   does not expose TLS 1.2 `status_request` processing/stapling. No OCSP response
   has been invented, ignored under a success label, or replaced by a CRL claim.
   A supported TLS provider or a reviewed, maintained TLS extension integration
   is needed, including responder authorization, signatures, certificate IDs,
   freshness/nextUpdate, cached-response limits and events. Direct OCSP retrieval
   is separate from mandatory stapled-response handling. The source document's
   RFC 6066/status_request_v2 wording discrepancy should be resolved in the PIXIT;
   RFC 6066 `status_request` and RFC 6961 multi-stapling are different extensions.
2. **Safe resumption (7.4.4).** Restore and revalidate the cached peer certificate
   and application binding; enforce an absolute <=24h lifetime, CRL/policy change
   invalidation, unchanged TLS version and expired-session/full-handshake notice.
   Re-enable only after interoperability and revoked-session tests pass.
3. **CRL distribution.** Connect the local bundle publisher to the actual EJBCA
   distribution endpoint, authenticated transport and operator policy. No endpoint
   was inferred from an untrusted certificate URI or guessed from lab paths.
4. **Remaining extension/event conformance details.** Independently test the
   complete mandatory security-event map. Missing signature_algorithms now
   fails before the server flight, with warning and alarm events, for all four
   mandatory suites in both host campaigns. Exact-certificate allowlist rejection,
   mutual-certificate absence, key-length and SHA-1 warnings, and the
   8,192/8,193-octet certificate boundary are covered by the TLS campaigns.
5. **Device and independent conformance tests.** Exercise real 12h/24h timers,
   idle concurrent MMS/IEC 104 clients, renewed CA/CRL bundles, controlled restarts,
   external clients and cipher captures. Host loopback tests alone do not establish
   IEC certification or target scheduling/resource behavior.

## Tespro baseline captured before disconnection

Read-only capture: `output/iec62351-tls/tespro-before-disconnect/`. Public
certificates/CRLs were exported; private keys were not copied. No daemon restart,
installation, clock/configuration change or output actuation was performed.

- TesproOS 32-2.1.0 r601-af70fcaaa2; aarch64 Linux 6.12.68.
- Running CCLI 0.1.0-r65; `/etc/ccli/lab.yaml`; MMS listening on
  `192.168.0.1:3782`. No CCLI IEC 104 listener appeared in the captured listener
  list; this capture is not an IEC 104 runtime test.
- TLS CA bundle: `/etc/ccli/rbac-trust/tls-ca-bundle.pem`.
- TLS CRL bundle: `/etc/ccli/rbac-dual/trust/tls-crl-bundle.pem`.
- That bundle contains a legacy lab CA CRL valid until **2026-10-29 19:36:10 UTC**
  and an RBAC CA CRL that expired **2026-10-23 08:59:49 UTC**. Renew the latter
  and rebuild the complete TLS bundle before deployment. A newer application
  RBAC CRL at a separate path does not refresh this TLS bundle.
- An **offline candidate**, `output/iec62351-tls/candidate-tls-crl-bundle.pem`,
  replaces the stale entry with that newer full CRL. Both CRL signatures verify
  against the captured CA bundle, no delta/issuing-distribution-point scope was
  present, and all five previously revoked RBAC serials remain revoked. This
  candidate was not installed. Its earliest nextUpdate is still **29 October
  2026 19:36:10 UTC**, so the legacy CA CRL also needs renewal before that time.
- Captured server TLS certificate expires 2029-01-01; the inspected certificate
  did not advertise AIA/CRL distribution extensions. Logs report GNSS no fix /
  clockNotSynchronized. A secure, monitored time source remains necessary for
  certificate/CRL validity evaluation; monotonic interval timers do not replace it.

## Host verification

The new `iec62351_tls_policy` and `iec62351_iec104_tls_policy` campaigns generate
disposable RSA/P-256 PKI and test real TLS 1.2 handshakes through the compiled HAL
and IEC 104 factory. Cases cover all four suites, missing/expired/revoked/wrong-EKU
and unpinned peers, invalid local identity/key/timers, certificate sizes of 8,192
and 8,193 DER octets, stale/future/bad-signature CRLs, live CRL replacement and
retrieval failure, CBC without EtM, RSA 3,072-bit acceptance, 1,024-bit RSA
rejection with the required alarm, SHA-1 rejection with its warning,
missing signature_algorithms rejection before any server flight for each of
the four mandatory suites, and an
ignored idle renegotiation request using a test-only accelerated monotonic clock.
Production timer bounds are not weakened for these tests.

```sh
cmake -S apps/ccli -B build/host-tls -G Ninja \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCCLI_BUILD_TESTS=ON \
  -DCCLI_WITH_LIBIEC61850=ON -DCCLI_WITH_LIB60870=ON -DCCLI_WITH_GOOSE=OFF
cmake --build build/host-tls --target e2e_host_checks_build \
  iec62351_tls_probe tls_allowlist_config_test
ctest --test-dir build/host-tls --output-on-failure \
  -R '^mms_rbac|^e2e_|^iec62351|^tls_allowlist' \
  --exclude-regex e2e_native_field_campaign
```

Verification output is archived under `output/iec62351-tls/`; see the final
regression log for observed results. No tests depend on Tespro or its private keys.

Observed results:

- Full host `ccli` application compiled and linked, including the IEC 104 service
  event forwarding path.
- Final TLS/MMS run: **5/5 passed**, including disposable fixture generation,
  existing MMS TLS authorization, both new TLS campaigns and source baseline.
- The broader targeted run also passed the timer parser, signed RBAC policy,
  E2E codec/BER/crypto/identity/native-stack/resilience checks. Its initial stale
  CRL startup assertion failed because startup now rejects stale CRLs; the test
  was updated to assert rejection and subsequently passed. The retained initial
  regression log is diagnostic history, not a claim of a passing whole run.
- The latest focused rerun passed **6/6 CTest targets**: the TLS policy parser,
  disposable certificate fixtures, MMS TLS authorization, both TLS campaigns,
  and the protected source baseline. The campaigns passed **35 MMS** and
  **38 IEC 104** cases, adding RSA 3,072-bit acceptance, RSA 1,024-bit rejection
  with its required alarm, SHA-1 warning/rejection, and SHA-384 policy disabled
  and enabled checks to the earlier 8,192/8,193-octet boundary, identity, pin,
  cipher, CRL and renegotiation coverage. Four additional raw ClientHello
  cases per campaign assert a fatal alert, required events, and no server
  handshake messages when signature_algorithms is omitted. This regression
  failed against the previous binary before the callback correction.
  The full host application and probe rebuilt successfully. The final log is
  `output/iec62351-tls/sha1-fallback-fix.log`.
- The long E2E field campaign and real-duration/device timer tests were not run
  for this change. No target APK was built or deployed.

Subsequent device verification cross-built the full ARM64 application and isolated
test harnesses, then executed the new TLS backend and both real protocol adapters
on Tespro without changing r65. Both TLS campaigns passed 23/23; all seven MMS
dual-certificate cases passed; IEC 104 mutual TLS, STARTDT, interrogation, STOPDT,
and graceful TLS shutdown passed. Renegotiation deadlines used an accelerated
test clock; real 12h/24h timing and full controller/plant behavior remain untested.
All temporary services and disposable credentials were cleaned up. See the device
report for hashes, audit logs, preservation checks and remaining deployment needs.

The later certificate-size and event-wording corrections are included in source
and host campaigns; they have not been cross-built or run on Tespro yet. The
installed r65 service is unchanged.

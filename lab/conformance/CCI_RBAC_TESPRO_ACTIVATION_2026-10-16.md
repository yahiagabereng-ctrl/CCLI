# Tespro main CCLI Profile A activation

Date: 2026-10-16. User explicitly requested enabling Profile A now.

## Active state

- Target: confirmed Tespro `192.168.0.1`, pinned SSH host key unchanged.
- Installed CCLI **0.1.0-r58**, unchanged executable; no new APK required.
- Main service restarted at 08:49:26 UTC; signed policy loaded at
  **08:49:29 UTC (10:49:29 Europe/Rome)**. Last verified PID **3374** at
  08:51:10 UTC, command `/usr/sbin/ccli --config /etc/ccli/lab.yaml`.
- **Profile A enabled on 192.168.0.1:3782**. No listener on 3783.
- Same `mvp_handbuilt` model, I/O, Modbus, plant and fallback settings.
  Restart reinitialized hardware I/O. No MMS control command was sent.
- Owner-signed five-file package in `/etc/ccli/rbac/`, public trust under
  `/etc/ccli/rbac-trust/`, public client pins under `/etc/ccli/rbac-clients/`.
  New client/owner/CA private keys were not copied to Tespro.
- Added combined old/new CA and CRL PEM bundles. Server TLS/ACSE certificate/key
  paths and content unchanged. Client pins now select DSO, Engineer revision 0,
  Viewer revision 1, plus a revoked negative fixture to verify revocation.
  CA trust alone does not authorize additional clients.
- New production state `/etc/ccli/rbac-state/activation.state`, mode 0600 in
  directory 0700, on persistent `/etc`. Exact content:
  `1 85f1f4679c9ad84382f76aaab564a32d47c8a20c031412f23f2f9c91ad2b57e7`.
  Isolated-test sequence-2 state is untouched and separate.
- Security events: `/etc/ccli/security-events/events.jsonl`, mode 0640 inside
  root-only 0700 directory. POLICY_ACTIVATED, ASSOCIATION_AUTH_OK and
  MMS_ACCESS_DENIED events verified.

## Verification on the main endpoint

| Case | Result |
|---|---|
| Viewer-r1 | TLS/MMS association, measurement and CF configuration reads pass |
| Viewer CF write | Writing the identical current ctlModel value denied with IED_ERROR_ACCESS_DENIED; audit confirms CONFIG permission denial |
| Engineer-r0 | TLS/MMS association and measurement read pass |
| DSO | TLS/MMS association and measurement read pass |
| Pinned revoked role certificate | TLS rejects with certificate-revoked alert |
| Legacy uncommissioned client | TLS rejects with access-denied alert |
| Original TLS files | Every recursive pre-activation file hash matches |
| CCLI executable and UCI configuration | Saved hashes match |
| Repository TLS/ACSE baseline checker | PASS; this check describes repository code, not unchanged live trust/pins |

Legacy DSO/Viewer reads and legacy revoked-client rejection passed immediately
before activation. Legacy role-less certificates are now rejected as intended;
configure clients with the new authorized role identities. The operating model
already reported Modbus read failures and GNSS no-fix before activation; these
continued afterward. Successful MMS reads do not establish valid live metering.

No successful control or configuration write was attempted on the main service.
Successful write/control and reporting checks from the isolated harness are
separate evidence. Actual TSP interoperability, main-service reporting, full
role/revision/negative matrix, live session revocation/expiry and full reboot
persistence remain unverified. This is not complete CEI/IEC conformance evidence.

## Client credentials and CRL lifetime

On the PC, authorized client certificates/keys and PKCS#12 files are under
`lab/ejbca-staging/rbac/{dso,engineer-r0,viewer-r1}/`. Import the desired identity
into the client. Retain trust in the existing server CA
`apps/ccli/config/tls/root_CA.pem`. PKCS#12 password is in protected
`lab/ejbca-staging/rbac/secrets.json`; do not copy secrets into this document.

The deployed RBAC CRL nextUpdate is **2026-10-17 08:14:02 UTC
(10:14:02 Europe/Rome)**. Refresh and deploy it before expiry. Run
`scripts/provision_ejbca_rbac.py finish` against the existing EJBCA lab, verify
the signed CRL, update `/etc/ccli/rbac-trust/rbac_crl.pem` and rebuild the combined
`tls-crl-bundle.pem` retaining the original TLS CRL. A controlled service restart
loads the refreshed material. No automatic CRL distribution has been configured.

Subsequent update at 08:59 UTC: the provisioning script and existing EJBCA CA
were changed to seven-day CRL validity. A verified replacement now exists on
the PC, nextUpdate 2026-10-23 08:59:49 UTC. It has not been deployed to Tespro;
the deployed expiry above still applies. See `CCI_RBAC_CRL_AUTOMATION.md`.

## Backup and recovery

Protected device backup:
`/root/ccli-before-profile-a-20261016/config-backup.tar.gz`, with original
`lab.yaml`, recursive `tls.sha256`, `unchanged.sha256` and commissioned public
material hashes. Backup includes existing device private files and stays on
Tespro; it was not downloaded.

Preparation initially stopped because a TLS directory was passed to sha256sum.
The archive was verified, active YAML was still unchanged, and recursive regular
file hashing was used before continuing. No failed restart or partial activation
occurred from that preparation stop.

To recover the previous legacy service configuration if necessary:

```sh
/etc/init.d/ccli stop
cp /root/ccli-before-profile-a-20261016/lab.yaml /etc/ccli/lab.yaml
/etc/init.d/ccli start
```

This disables Profile A by restoring the prior configuration and old client
pins; use only as an explicit recovery action. Retain production and test
activation state, trust and security evidence. Never delete state to bypass
anti-rollback. This recovery was documented, not executed.

## Hashes and retained evidence

| Artifact | SHA-256 |
|---|---|
| Active `/etc/ccli/lab.yaml` | `8b2854bd844eb7e219b3a7a33257d29ef2ca78a2854ba3c81a25fd4dbc80be15` |
| Installed `/usr/sbin/ccli` (unchanged) | `12dae230d5d45257de88ae9585dcc15744e53ed1f54f0a6dd0c48c881f580786` |
| `/etc/config/ccli` (unchanged) | `011062951870aee5d25344071f87db1ff38eadc00943d4407ba90eb1945b2c88` |
| Original active YAML, in backup | `114149038f25cfd5eff5d1cfe283972caa56547f3acd562eea6f5e3bc4f6c859` |

Protected local evidence:
`lab/ejbca-staging/rbac/production-activation-20261016/` contains five client
case logs, results JSON, activated YAML and final device status/audit/hash log.
Reproduction scripts are in ignored `tmp/`: `prepare_production_profile_a.py`,
`enable_tespro_production_profile_a.sh`, `verify_production_profile_a.py`.

## Subsequent r59 update — 16 October, 14:33 Europe/Rome

The historical r58 activation above is superseded by **r59**, installed with
Profile A still enabled on 192.168.0.1:3782 and signed commissioning sequence 3.
Old TLS credentials and UCI configuration hashes are unchanged. The original
TSP TLS certificate is an additional allowed peer; its separate DSO MMS key
and role certificate passed an actual main-service read-only test. The seven-day
RBAC CRL is deployed (nextUpdate 23 October, 10:59:49 Europe/Rome). Recovery
backup: `/root/ccli-before-r59/`. No state was reset or rolled back. Actual TSP
on the unavailable separate PC is pending. See
`CCI_RBAC_TSP_DUAL_CERTIFICATE_INTEGRATION.md` for current hashes and evidence.

The old recovery example in this document selects a pre-RBAC configuration.
After accepting sequence 3, retain that activation state and use a compatible
signed sequence-3-or-later package; never restore old state to bypass rollback
protection. The r59 recovery script explicitly preserves a later accepted state.

# r75 device CRL pull deployment

Follow-up: the initial RTU startup sample below did not prove continued
polling. The user reported the stalled poller; r76 fixes a public-class layout
mismatch that could stop its worker after connecting. See the r76 RTU layout
deployment record for the root cause and repeated-read verification.

Date: 2026-10-29. Target: 192.168.0.1, live board tespro,tr500,
TesproOS 32-2.1.0 r601-af70fcaaa2, Linux 6.12.68.

## Artifact and installation

- APK: `lab/tg544-openwrt/ccli-0.1.0-r75.apk`.
- SHA-256: `5cbed0b8fa88268bee2eea3f579918377d8c30a172d7e2b148ba068503454e28`.
- Installed with SSH and `apk --no-network add --upgrade --allow-untrusted`.
- Installed executable SHA-256: `1b34a6ac552fbf89355eea31f6371d947cfe5fc91efa72ab71d288ec07c4e612`.
- Version reports 0.1.0-r75. Existing build metadata reports git unknown and
  a fixed historical build timestamp; artifact hashes identify this specimen.
- Configuration was byte-identical across the APK upgrade before commissioning:
  SHA-256 `50ccf7a9d8fb7200fc9cfad6a9b38afcbdbf4e4ca977acaea123d7c6a7632912`.
- Backups: `/tmp/ccli-r75-pre-upgrade.tgz` (volatile),
  `/etc/ccli-upgrade.kcCPem` (package-preservation backup), and
  `/etc/ccli-crl-migration.FIcHJb` (pre-migration configuration).
- No pre-existing upgrade hold/pending marker. The new hold was cleared only
  after package preservation and the configuration hash were checked.

## Approved decisions and commissioned behavior

The user selected device pull, prompt disconnection upon verified revocation,
EJBCA migration, and retirement of the old dual-viewer test fixture while
retaining EJBCA Viewer credentials. The user requested CEI-based timing.
Hourly fetching and 60-second local TLS checks are engineering defaults;
one-minute retries apply after failed fetches, not normal hourly polling.

The procd `ccli-crl` worker is running and enabled for reboot. It fetches the
public EJBCA distribution endpoint at `http://192.168.0.137:9080`, validating
against two pinned local CA certificates. TLS and RBAC lists are published
together through `/etc/ccli/crl-pull/current`. The CA's signing keys and
administrative enrollment credentials are not installed for this service.

The active MMS configuration uses the new CA bundle and snapshot CRL paths,
with `tls_revocation_check_s: 60`. The legacy root is absent from the new
transport trust bundle. Only the legacy dual-viewer TLS allowlist entry was
retired; the EJBCA Viewer entries remain. Legacy files are retained for rollback.

## Verification

- Host signed-fixture HTTP tests passed: valid initial/new/identical CRLs;
  rejection of bad signatures, rollback, reused numbers with changed content,
  expiry, future issue time and issue-time rollback; seven-day signed lifetime
  accepted; complete publication and failed-second-issuer preservation.
- Actual BusyBox/OpenSSL fetch and snapshot publication passed on TesPro.
  TLS CA CRL number 8 expires 2026-10-30 13:07:27 UTC; RBAC CRL number 12
  expires 2026-11-04 14:48:35 UTC at this check.
- All 21 remaining configured transport/association server/client certificates
  passed OpenSSL chain and CRL validation against the EJBCA-only bundle.
- `scripts/tests/check_dut_crl_tls.py` passed against TesPro: the commissioned
  EJBCA client negotiated TLS 1.2 / ECDHE-ECDSA-AES128-GCM-SHA256; the retired
  legacy client received TLSV1_ALERT_ACCESS_DENIED.
- Service/listener checked after commissioning: MMS on 192.168.0.1:3782,
  CCLI running, CRL worker running and enabled.
- RTU log after restart: /dev/rs485_2_uart, unit 1, poll_ms=1000,
  P=74.7467 kW, Q=0 kvar, quality=good. Independent TCP meter logs also continue.
- Fixed commissioning startup sequencing after a check revealed an asynchronous
  stop/start port race. The final process logs a successful listener startup;
  subsequent TLS probes passed. The repository commissioning script now waits
  for the old listener to release the port.

## Scope and remaining work

CRL fetching/publication is installed. This is not full Annex T/PKI closure.
No live operational identity was revoked as part of this installation.
Prompt disconnection of an idle session on separately revoked ACSE identity
still needs implementation/qualification; current service access revalidates
that identity, while transport certificate refresh exists in the TLS backend.
TLS handshakes do not establish MMS association/report/control conformance.

Remaining updater hardening: bounded generation retention, crash/stale-lock
recovery, endpoint availability/expiry monitoring in the shared security logger,
power-loss qualification and operational endpoint stability. The EJBCA host is
still the lab PC; device pull does not remove that CA availability dependency.

Supplier TG500-20261021 documents were reviewed as platform inputs. All 11 have
the same body text as the existing certification Tespro copies. Draft ABI,
IDevID and secure-boot claims do not establish those features on this TR500.
No firmware, bootloader, fuse or TPM provisioning changes were performed.

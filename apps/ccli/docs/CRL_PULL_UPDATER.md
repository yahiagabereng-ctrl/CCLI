# TesPro CRL pull updater — implementation checkpoint

Date: 2026-10-29.

User decisions: TesPro downloads public signed CRLs directly. A newly revoked
connected client must be disconnected promptly after verified revocation is
learned. The user requested CEI-based timing rather than selecting one of the
proposed one/five/fifteen-minute targets.

CEI 57-142:2026-07 section 5.1 recommends CRL updates at least every 24 hours;
it does not prescribe a one/five/fifteen-minute revocation detection deadline.
Section 5.9.8 gives hourly or within-24-hour CRL retrieval as examples for a
proxy. Use hourly fetching as the proposed engineering default, configurable
for the operator; active-session checking is separate from downloading and
TLS renegotiation. A recommended 60-second local revocation check after a
validated download is an implementation choice, not a CEI numeric mandate.
Network/CA outages prevent a guaranteed remote revocation detection deadline.

TLS renegotiation must not occur more frequently than every ten minutes;
the document relates its default timing to the CRL update period. Session
resumption has its own configurable timing and must occur at least every two
hours and sooner than renegotiation. Do not confuse those timers with local
CRL checking. Existing sessions must not be terminated solely because CRL or
OCSP freshness limits are exceeded; a verified revocation is a different event.

The 24-hour update recommendation does not impose a 24-hour maximum signed
CRL lifetime. The validator therefore checks current validity without rejecting
a valid longer-lived CRL solely on that basis.

Source: https://static.ceinorme.it/strumenti-online/doc/26353.pdf

Development lifetime update, 29 October: the user explicitly requested a
one-month CRL lifetime during development. Both development EJBCA CAs are
configured for 30 days (2592000000 ms); hourly device fetching and 60-second
TLS revalidation remain unchanged. This signed lifetime is a development
policy, not a production conformity claim. The reproducible issuance tool is
`scripts/set_development_crl_lifetime.py`; it checks pinned signatures, signed
lifetime, increasing CRL numbers and preservation of existing revocations.
The RBAC provisioner also uses 30 days so later development provisioning does
not silently revert this CA to seven days.

## Supplier platform inputs

The user's TG500-20261021 deliverables are included in the design review.
They describe TesproOS 32-2.x/OpenWrt 25.12, gcc 14.3/musl and procd service
coexistence. The live specimen identifies as TR500, board tespro,tr500,
TesproOS 32-2.1.0 r601-af70fcaaa2, kernel 6.12.68. The exact TG500-to-TR500
applicability and vendor same-revision SDK remain acceptance items; matching
toolchain families alone is not evidence of a vendor ABI guarantee.

The updater uses a separate procd service and does not take ownership of the
IEC 61850 ports, RS485 or vendor serial services. It uses the existing package
manager and leaves the canonical signed-APK/USB workflow intact. The supplier
IDevID and secure-boot procedures are draft inputs; they do not authorize
fuse programming or establish enabled secure boot. No supplier provisioning,
key-generation, bootloader or fuse commands are executed by this updater.

Read-only device tests on 29 October successfully fetched and validated the
new EJBCA TLS CA CRL (number 8) and RBAC CA CRL (number 12), then published a
two-issuer snapshot under /tmp. BusyBox lacks the stat command on this image;
the ownership check was adjusted to use its existing ls/awk tools.

The user selected migration of legacy credentials to EJBCA. The active profile
already references EJBCA-issued TSP TLS and application identity certificates;
legacy certificate files and a legacy CA/CRL still exist. Remove legacy active
trust only after verifying all currently configured server/client identities
against the EJBCA-only replacement bundle and preserving a rollback copy.

## Implemented

`/usr/sbin/ccli-crl-fetch URL PINNED_CA CACHE_DIRECTORY` accepts DER or PEM,
verifies the signature and issuer against a locally provisioned CA, checks
thisUpdate/nextUpdate, requires CRL
numbers and rejects rollback or different content under a reused number.
It rejects delta and restricted-scope CRLs. Failed updates preserve the cache.
HTTP transport is allowed because authenticity comes from the pinned CA's
signature; HTTPS additionally validates the endpoint through curl's trust store.
No CA keys, user keys or CA administrative credentials are needed on TesPro.

`/usr/sbin/ccli-crl-refresh ISSUER_CONFIG STATE_DIRECTORY` fetches every
configured issuer before publishing a complete pair through one atomic
`current` symlink replacement. Existing published files survive any failed
issuer fetch. Unchanged snapshots do not create another published generation.
Issuer caches also persist the highest accepted CRL number across process
restarts. Storage rollback by a privileged attacker is outside this protection.

Issuer configuration contains four whitespace-separated fields per line:

```
# id      use       URL                                   pinned CA
tls-root tls       https://ca.example/tls.crl              /etc/ccli/pki/tls-ca.pem
rbac     both      https://ca.example/identity.crl         /etc/ccli/pki/rbac-ca.pem
```

These are examples, not commissioned endpoints. Paths cannot contain spaces.
Use `both` when an issuer is trusted for both TLS and association identities.
The pinned CA must be one certificate, not an arbitrary downloaded CA bundle.
Run under a dedicated protected state directory owned by the updater account.
The tools require shell utilities, curl and OpenSSL CLI; package dependencies
include curl and openssl-util. No Python is needed on the target.

Output paths are `STATE_DIRECTORY/current/tls-crl-bundle.pem` and
`STATE_DIRECTORY/current/rbac_crl.pem`. Published generations are retained;
garbage collection and final power-loss qualification remain to be implemented.
Interrupted execution can leave a lock directory. It deliberately requires
inspection rather than deleting a potentially live lock automatically.

## Validation and outstanding integration

`python3 scripts/tests/test_crl_fetch.py` passed in WSL on 29 October using
real signed CRLs and a local HTTP server. It exercises initial fetch,
idempotency, newer CRL acceptance, older number rejection, reused-number
rejection, wrong signature, expiry, future issue time, valid seven-day lifetime,
issue-time rollback, snapshot publication, and a failed second issuer download.
Rejected updates preserve the last accepted cache and published snapshot.

The existing TLS backend already refreshes connection-local CRLs and rejects
known revoked peers, avoiding a whole-controller restart. This does not yet
establish prompt termination for a separately revoked ACSE/Profile A identity;
that path needs integration and device tests as well.

Deployment update: r75 is installed on TesPro; endpoints, hourly procd fetching,
active TLS/identity CRL paths and the EJBCA-only transport trust migration are
commissioned. Device compatibility and positive/negative TLS probes passed.
See `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r75_2026-10-29_CRL_PULL.md`.

Still open: active application-identity session termination acceptance and
integration, bounded generation retention, crash/stale-lock recovery, shared
security-event integration and power-loss behavior. These scripts do not close
the complete PKI gap or certify Annex T conformity.

# Development CRLs valid for 30 days

On 29 October 2026 the user requested one-month CRL validity during development,
before committing the new code to the personal repository.

Both EJBCA development CAs now have CRLPeriod=2592000000 milliseconds. Fresh
CRLs were issued and verified against their existing pinned CA certificates:

| CA | CRL number | thisUpdate UTC | nextUpdate UTC | Revoked entries |
|---|---|---|---|---|
| CCLI-Lab-CA | 9 | 2026-10-29 16:24:22 | 2026-11-28 16:24:21 | 0 |
| CCLI-RBAC-Lab-CA | 13 | 2026-10-29 16:25:00 | 2026-11-28 16:24:59 | 5 |

Expiry in Europe/Rome is 28 November 2026 at 17:24:21 and 17:24:59 CET.
Signatures, current validity, signed lifetime, increased CRL numbers and
preservation of all previous revocations passed verification.

TesPro pulled both CRLs into its active atomic snapshot through
`ccli-crl-refresh`. No CCLI restart, certificate replacement or CA key change
was needed. Hourly fetching and 60-second local TLS revalidation remain in
place. MMS remains listening on port 3782. Positive EJBCA TLS and negative
retired-legacy-client probes both passed after the refresh.

Public exports are in `output/crl-pull-20261029/development-30day/`; the
combined `development-tls-crl-bundle.pem` can be transferred to Test Suite Pro
on its PC. This work did not change that external PC's selected CRL file.

`scripts/set_development_crl_lifetime.py` reproduces issuance and verification
for these two lab CAs. The RBAC provisioner now retains the 30-day development
policy. This does not declare the production PKI policy or complete Annex T
conformity; refresh frequency and signed validity period are separate settings.

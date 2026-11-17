# Public development CRLs

Issued on 29 October 2026 at the user's request, with a 30-day signed lifetime.
TesPro has fetched these same CRLs into its active trust snapshot.

- CCLI-Lab-CA: number 9, expires 28 November 2026 16:24:21 UTC (17:24:21 CET).
- CCLI-RBAC-Lab-CA: number 13, expires 28 November 2026 16:24:59 UTC (17:24:59 CET).
- `development-tls-crl-bundle.pem` contains both issuer CRLs for lab clients.
- `verification.json` records signed dates, numbers and revoked-entry counts.

These files contain public revocation information only. No private keys or
administrative credentials are included. Consumers must verify each CRL using
their existing pinned issuer certificate. This directory is a dated snapshot;
the live EJBCA endpoint and device pull updater provide subsequent versions.
Do not replace another installation's trust or production PKI policy with this
lab snapshot. A Test Suite Pro PC must select the refreshed CRL independently.

# Certificate Profiles — EJBCA doc snapshot

**Source:** https://docs.keyfactor.com/ejbca/latest/certificate-profiles-overview (Keyfactor, ingested 2026-10-06)

## Summary

A **certificate profile** is the EJBCA template that controls:

- Key algorithms and sizes (RSA ≥2048 for CCLI)
- **keyUsage** and **extendedKeyUsage**
- Custom extensions (needed for 62351-4 G.6.2 subject via DN / custom OID)
- Which CAs may use the profile

## CCLI profiles to create

| Profile | keyUsage | EKU | Subject |
|---------|----------|-----|---------|
| **CCLI-Cert-A-TLS** | digitalSignature, keyEncipherment (RSA server) | serverAuth + clientAuth | CN, O, serialNumber + SAN IP |
| **CCLI-Cert-B-E2E-MMS** | digitalSignature, keyAgreement | (none or minimal) | 2.5.4.106 = AP+AE OID |

See `CCI_EJBCA_PKI_Extract.md` for full field mapping from K6.3.

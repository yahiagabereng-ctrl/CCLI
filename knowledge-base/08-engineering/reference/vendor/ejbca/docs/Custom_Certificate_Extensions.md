# Custom Certificate Extensions — EJBCA doc snapshot

**Source:** https://docs.keyfactor.com/ejbca/latest/custom-certificate-extensions (Keyfactor, ingested 2026-10-06)

## Relevance to CCLI

62351-4 **G.6.2** requires subject RDN **2.5.4.106** (`id-at-objectIdentifier`) with MMS AP+AE binding.

EJBCA may encode this via:

1. **Subject DN fields** in End Entity Profile (preferred if OID-as-DN supported), or
2. **Custom certificate extension** / DN component override in certificate profile

Lab reference: `gen_lab_pki.py` uses UTF8String encoding of OID text for TSP/OpenSSL compatibility — verify EJBCA issued cert with:

```powershell
openssl x509 -in server.pem -noout -subject -text
```

Match behaviour of lab certs before TSP Connect.

## BasicCertificateExtension encodings

| Encoding | Use for CCLI |
|----------|--------------|
| DEROBJECT | Hex DER for complex ASN.1 |
| DERUTF8STRING | Possible G.6.2 OID text path |
| DERIA5STRING | SAN hostname |

Configure under **System Configuration → Custom Certificate Extensions**, then attach to certificate profile.

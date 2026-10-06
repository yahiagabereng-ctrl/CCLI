# PKI reference corpus (RFCs + enrolment norms)

**Cluster:** `pki-ejbca` · **Parent:** `CCI_EJBCA_PKI_Capture_Plan.md`

| File | Source | Role |
|------|--------|------|
| `RFC7030_EST.txt` | IETF RFC 7030 | Annex T preferred enrolment |
| `RFC8894_SCEP.txt` | IETF RFC 8894 | Annex T alternative enrolment |
| `RFC5280_X509.txt` | IETF RFC 5280 | X.509, CRL, extensions |
| `RFC6960_OCSP.txt` | IETF RFC 6960 | Revocation (62351-9) |

Normative extracts (structured): `../CCI_62351-9_Extract.md`, `../CCI_62351-4_Extract.md`, `../CCI_K6.3_Key_Ceremony_SOP.md`.

Regenerate RFC files:

```powershell
powershell -File scripts\sync-pki-rfc-reference.ps1
```

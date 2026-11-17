# Annex-aligned CCLI lab PKI (EJBCA CLI)

Issued by `scripts/issue_ejbca_cli_pki.py` via addendentity + createcert.

| Artefact | Profile |
|----------|---------|
| `root_CA.pem` | CCLI-Lab-CA |
| `server_tls.pem` + `server.key` | CCLI-Cert-A-TLS |
| `server.pem` + `server_acse.key` | CCLI-Cert-B-E2E-MMS |
| `client.pem` / `client_tls.pem` | operator dual |

Verify: `python scripts/verify_annex_pki.py --dir C:\Yahia\projects\CCLI\apps\ccli\config\tls\ejbca`

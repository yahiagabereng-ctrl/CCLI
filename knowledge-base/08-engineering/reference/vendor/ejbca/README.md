# EJBCA Community Edition — lab PKI

**Edition:** Community (LGPL v2.1) — **not for production**  
**Vendor:** Keyfactor · https://www.ejbca.org/  
**Docs:** https://docs.keyfactor.com/ejbca/latest/

## Software on this machine

| Artefact | Location / command |
|----------|-------------------|
| Docker image | `keyfactor/ejbca-ce:latest` (pulled to local Docker) |
| Image digest | see `docker/image-info.txt` after pull |
| Doc snapshots | `docs/*.md` (Keyfactor docs, for offline RAG) |

## Quick start (lab VM / Docker host)

```powershell
docker run -d --name ejbca-ce -p 8080:8080 -p 8443:8443 -h ejbca keyfactor/ejbca-ce:latest
```

- Admin UI: https://localhost:8443/ejbca/adminweb/ (initial setup wizard on first boot)
- EST base URL pattern: `https://<host>/.well-known/est/<alias>/simpleenroll`

See `../../../../CCI_EJBCA_PKI_Extract.md` for **Cert A / Cert B** profile mapping to CCLI.

## CCLI certificate profiles to create in EJBCA

| EJBCA profile name (suggested) | K6.3 / 62351 role |
|--------------------------------|-------------------|
| `CCLI-Cert-A-TLS` | 62351-3 transport TLS `:3782` |
| `CCLI-Cert-B-E2E-MMS` | 62351-4 Annex G MMS auth |
| End entity `TG544-<serial>` | Server — two enrolments (A then B) |
| End entity `TSP-DSO-OPERATOR` | Test Suite Pro client certs |

## Related repo paths

- Lab PEM staging (legacy): `apps/ccli/config/tls/gen_lab_pki.py`
- TSP stage script: `scripts/stage-tsp-tls.ps1`
- Ceremony SOP: `knowledge-base/08-engineering/CCI_K6.3_Key_Ceremony_SOP.md`

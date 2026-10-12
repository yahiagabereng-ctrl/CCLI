# EJBCA lab access handoff — @alirezavzand

**Repo:** https://github.com/yahiagabereng-ctrl/CCLI (private)  
**Date:** 2026-10-12  
**Full runbook:** `lab/EJBCA_LAB_SETUP.md`  
**PEMs (already on `main`):** `apps/ccli/config/tls/ejbca/` · commit `db81c16`

---

## 1. Pull

```powershell
cd <your-clone>\CCLI
git pull origin main
```

---

## 2. Admin account (lab)

There is **no username/password** for Admin Web in this lab.

| Item | Value |
|------|--------|
| Mode | `TLS_SETUP_ENABLED=simple` (see `knowledge-base/08-engineering/reference/vendor/ejbca/docker/docker-compose.yml`) |
| Admin URL | https://localhost:8443/ejbca/adminweb/ |
| Login | Open browser → accept TLS warning → Admin UI (no SuperAdmin P12, no password) |
| Public UI | https://localhost:8443/ejbca/ |

Start container:

```powershell
powershell -File scripts\start-ejbca-lab.ps1
powershell -File scripts\start-ejbca-lab.ps1 -Status
curl.exe -k https://localhost:8443/ejbca/publicweb/healthcheck/ejbcahealth
```

---

## 3. Passwords that are *not* Admin login

| Purpose | Default | Where |
|---------|---------|--------|
| End-entity enroll (CLI issue) | `ccli-lab-ee` | `scripts/issue_ejbca_cli_pki.py` |
| CA staging P12 | `ccli-lab-p12` | `lab/ejbca-staging/` (gitignored) |
| SuperAdmin P12 (REST only) | often `foo123` | Only if you create `superadmin.p12` for REST |

Leaf rebuild (no Admin UI account needed):

```powershell
python scripts\issue_ejbca_cli_pki.py --verify-after
```

Annex gate:

```powershell
python scripts\verify_annex_pki.py --dir apps\ccli\config\tls\ejbca
```

---

## 4. Deploy PEMs (DUT / TSP)

| Role | Files |
|------|--------|
| DUT TLS (Cert A) | `server_tls.pem` + `server.key` |
| DUT ACSE (Cert B) | `server.pem` + `server_acse.key` |
| Trust | `root_CA.pem` |
| TSP operator | `client_tls.pem` / `client.pem` + keys · staged `C:\CCLI_product_tls\` |

Do not mix old serials from earlier Connect captures with this rebuild.

---

## 5. Related

- TLS/Wireshark Step 7 handoff: `lab/evidence/TLS_WIRESHARK_HANDOFF_ALIREZA.md`
- Master log residual (TSP dual-cert): `lab/CCLI_PHASE_LAB_MASTER_LOG.md` §3.7

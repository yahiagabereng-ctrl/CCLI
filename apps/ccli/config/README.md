# CCLI configuration files

| File | Role |
|------|------|
| **`ccli_application.yaml`** | **Canonical application config** → DUT `/etc/ccli/ccli_application.yaml` (UCI `ccli.main.config_file`) |
| `product_tr400.yaml` | Ship profile (EJBCA + CRL on) — alternate deploy, not the default bench single file |
| `product_tr400_p3_07_connect_5ln.yaml` | Legacy alias; content merged into `ccli_application.yaml` |
| `lab_tr400_*.yaml` | Lab variants (31-LN, RTU, cleartext forbidden on product binary) |
| `icd/product/product_tr400_eth_a_5ln.*` | 5-LN certificate model (separate from yaml) |
| `tls/` | MMS/104 PKI material (deploy to `/etc/ccli/tls/`, not embedded in yaml) |

Deploy: `scripts/push-ccli-application-config.ps1` · Admin: `lab/CCLI_DUT_ADMIN_HARDENING.md`

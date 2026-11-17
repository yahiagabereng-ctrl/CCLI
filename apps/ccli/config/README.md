# CCLI configuration files

For the personal Tespro bench, use **`lab_tespro_192_168_0_1_23ln.yaml`**:
MMS TLS at **192.168.0.1:3782**, the 23-node `product_tr400_eth_a_all_mandatory.cfg`,
and the existing commissioned TLS/RBAC paths. The package installs this profile
as `/etc/ccli/lab.yaml`; the configuration push script also selects this source
and defaults to host `192.168.0.1`. The profiles listed below retain the owner's
historical bench settings and are alternatives, not this bench's defaults.

The personal profile reads the EMT432 over Modbus TCP at `192.168.178.10:502`
(unit 1), using the verified 1-phase P/Q/current/voltage register map and scales.
Tespro also needs the persistent `meter_lan3` static interface (`192.168.178.1/24`
on `br-lan`) so it can reach the meter subnet while MMS remains at
`192.168.0.1:3782`. Do not treat the added 23-node model as implementation of
every new node's runtime behavior.

| File | Role |
|------|------|
| **`ccli_application.yaml`** | **Canonical application config** → DUT `/etc/ccli/ccli_application.yaml` (UCI `ccli.main.config_file`) |
| `product_tr400.yaml` | Ship profile (EJBCA + CRL on) — alternate deploy, not the default bench single file |
| `product_tr400_p3_07_connect_5ln.yaml` | Legacy alias; content merged into `ccli_application.yaml` |
| `lab_tr400_*.yaml` | Lab variants (31-LN, RTU, cleartext forbidden on product binary) |
| `icd/product/product_tr400_eth_a_5ln.*` | 5-LN certificate model (separate from yaml) |
| `tls/` | MMS/104 PKI material (deploy to `/etc/ccli/tls/`, not embedded in yaml) |

Deploy: `scripts/push-ccli-application-config.ps1` · Admin: `lab/CCLI_DUT_ADMIN_HARDENING.md`

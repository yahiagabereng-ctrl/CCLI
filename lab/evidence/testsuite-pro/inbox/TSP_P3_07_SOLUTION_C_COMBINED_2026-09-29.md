# TSP P3-07 — Solution C: IOP COMBINED server cert (2026-09-29)

## Prior results

| Test | Server TLS | AARE embedded | TSP |
|------|------------|---------------|-----|
| G.2 dual | `server_tls.pem` | `server.pem` | Signature fail (crypto OK offline) |
| Solution B | `server_tls.pem` | `server_tls.pem` (CN) | Signature fail (crypto OK offline) |

## Solution C

**UCA IOP `_COMBINED` pattern** — one end-entity cert/key for **both** Transport TLS and AARE §11.2.2:

| Property | Value |
|----------|-------|
| Files | `server_combined.pem` / `server_combined.key` |
| Subject | G.6.2 `2.5.4.106 = 1.1.1.999.1.12` (UTF8String, TSP-parseable) |
| keyUsage | `digitalSignature` + `keyEncipherment` + `keyAgreement` |
| AP/AE in AARE | `1.1.1.999.1` / `12` (unchanged) |

Config: `apps/ccli/config/lab_tr400_phase1_regulation_p3_07_combined.yaml`

Regenerate PKI: `python apps/ccli/config/tls/gen_lab_pki.py`

## Deploy

```powershell
$env:CCLI_TG544_PW = "<password>"
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -LabYaml "apps\ccli\config\lab_tr400_phase1_regulation_p3_07_combined.yaml"
```

## TSP (unchanged client side)

- `C:\CCLI_tls\`: `root_CA.pem`, `client.pem`, `client_tsp.key`, `lab_crl.pem`
- Port **3782**, TLS v1.3 **OFF**, Directory to CA **empty**

## Live test result (2026-09-29 ~196 s capture)

| Check | Result |
|-------|--------|
| TSP | **FAIL** — same `Unable to verify signature (+)` |
| Wire | TLS OK → AARE 1147+363 B → Encrypted Alert **~5 ms** (frames 3678–3712) |
| DUT | `auth ready (cert=780 octets)`, `TX_ACCEPT len=1269`, `inner=1065` |
| Offline | **PASS** — `lab/tmp_tx_solution_c_20260929.hex` + `server_combined.pem` |

**Conclusion:** COMBINED cert profile does **not** fix TSP. All three server cert strategies fail with valid §11.2.2 crypto.

## Pass criteria (historical)

| Result | Meaning |
|--------|---------|
| **Connected** | TSP needs G.6.2 binding + TLS keyUsage in one cert |
| **Signature fail** | Issue is not cert profile — investigate signed payload / TSP bug ← **observed** |

## Revert

```powershell
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1
```

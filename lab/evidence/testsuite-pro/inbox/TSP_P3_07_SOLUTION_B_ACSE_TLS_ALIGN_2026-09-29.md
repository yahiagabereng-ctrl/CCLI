# TSP P3-07 — Solution B: AARE cert aligned with Transport TLS (2026-09-29)

## Problem (baseline)

Dual-cert G.2 lab config fails at TSP with **Unable to verify signature (+)** after full TLS + AARE, although offline verify passes with `server.pem` + `server_acse.key`.

## Solution B (this test)

Keep Transport TLS as **`server_tls.pem` / `server.key`**. Point AARE auth at the **same** cert/key:

| Layer | Certificate | Private key |
|-------|-------------|-------------|
| Transport TLS | `server_tls.pem` | `server.key` |
| AARE §11.2.2 | `server_tls.pem` | `server.key` |

Config file: `apps/ccli/config/lab_tr400_phase1_regulation_p3_07_acse_tls_align.yaml`

**Not normative** for product (Annex G.2 requires separate APP vs TLS end-entity certs). This is an **isolation test** only.

## Hypothesis

If **Connect succeeds**, TSP validates the AARE signature against the **Transport TLS peer certificate** (or requires embedded cert == TLS cert), not the separate G.2 application certificate.

## Deploy

```powershell
$env:CCLI_TG544_PW = "<root-password>"
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -LabYaml "apps\ccli\config\lab_tr400_phase1_regulation_p3_07_acse_tls_align.yaml"
```

## TSP (unchanged)

- `192.168.10.1:3782`
- `C:\CCLI_tls\`: `root_CA.pem`, `client.pem`, `client_tsp.key`, `lab_crl.pem`
- MMS Security + Transport TLS: same paths; TLS v1.3 **OFF**

## Pass criteria

| Result | Meaning |
|--------|---------|
| **Connected** | Confirms TSP/TLS-alignment issue; escalate to Triangle; product keeps G.2 dual cert |
| **Same signature error** | Not TLS-peer binding; try Solution C (IOP COMBINED cert) or G.6.2 OID subject |

## Revert

Redeploy default yaml:

```powershell
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1
```

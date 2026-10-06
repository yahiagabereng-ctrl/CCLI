# CCLI deploy manifest — 0.1.0-r34

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-05 19:50:00 UTC |
| **Version** | **0.1.0-r34** |
| **Codename** | P7-ZONE-DASHBOARD |
| **SHA256** | PENDING_BUILD |
| **Revert to** | **0.1.0-r33** — tag `a8e2a81`, manifest `CCLI_DEPLOY_0.1.0-r33_2026-10-05_214500.*` |

## Deploy

**Binary (zones block):** rebuild `ccli-bin` from r34, deploy via manifest flow.

**Web only (no yaml/binary):**

```powershell
python scripts/plant-map-generate.py
$env:CCLI_TG544_PW = "..."
.\lab\tg544-openwrt\deploy-zone-dashboard.ps1
```

Browser: `http://<dut>/ccli/zone_dashboard.html`

## Rollback to r33

1. `git checkout 0.1.0-r33`
2. Rebuild/redeploy r33 `ccli-bin`
3. Optional: restore `/www/ccli/index.html` without dashboard redirect
4. `ccli --version` → **0.1.0-r33**

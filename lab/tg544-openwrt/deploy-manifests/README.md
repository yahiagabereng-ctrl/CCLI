# CCLI deploy manifests

Every **`deploy-ccli-session-fix.ps1`** run must produce:

1. **`CCLI_DEPLOY_0.1.0-rN_YYYY-MM-DD_HHMMSS.md`** — human record (changes, SHA256, yaml, TLS, ICD)
2. **`CCLI_DEPLOY_0.1.0-rN_YYYY-MM-DD_HHMMSS.json`** — machine sidecar
3. **`CCLI_DEPLOY_LATEST.md` / `.json`** — rolling pointer (overwritten; cite this in evidence)
4. **`../CCLI_SW_VERSION_AND_BINARIES.md`** — CURRENT block updated

Writer: **`../write-deploy-manifest.ps1`** · Refresher: **`../Update-CcliDeployRecord.ps1`**

Do not delete manifests for uploads that back **TSP / unified cert evidence** — they are the SHA audit trail.

# Lab revision pins

Machine-readable **deploy bundles**: firmware release, git source pin, yaml/TLS/ICD, network role, verification, and optional **reference SHA256** from a known-good DUT upload.

| Profile ID | Purpose |
|------------|---------|
| [`R51-P3-07-SOURCE.json`](R51-P3-07-SOURCE.json) | **Pre–5LN / pre–cert-model** — rebuild **0.1.0-r51** from git **`4c73add`** (Oct-08 TSP MMS pass lineage) |
| [`P3-07-TSP-CONNECT.json`](P3-07-TSP-CONNECT.json) | **Connect repro on current tree** — **r55** (or rebuilt r51) + `p3_07_connect` PKI + `crl=off` |
| [`UNIFIED-5LN-CERT.json`](UNIFIED-5LN-CERT.json) | **Unified cert matrix** — **r55** + 5-LN product model + EJBCA TLS + `crl=on` |

**Recap (human):** [../DEPLOY_REVISION_RECAP.md](../DEPLOY_REVISION_RECAP.md)  
**SSH + TSP TLS (both PCs):** [../LAB_ACCESS_FREEZE.md](../LAB_ACCESS_FREEZE.md)

## Apply a pin

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\deploy-from-revision-pin.ps1 -Profile R51-P3-07-SOURCE -WhatIf
powershell -File scripts\deploy-from-revision-pin.ps1 -Profile P3-07-TSP-CONNECT
```

`-WhatIf` prints checks and the deploy command without uploading. Use `-ApplyVersion` only when intentionally checking out a **source pin** and syncing `apps/ccli/VERSION` + `package/ccli/Makefile` before build.

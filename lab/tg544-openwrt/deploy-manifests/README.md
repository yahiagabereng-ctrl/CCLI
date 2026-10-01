# CCLI deploy manifests

One file per DUT upload: **`CCLI_DEPLOY_<semver>-rN>_<timestamp>.md`** (+ `.json` sidecar).

Created by `write-deploy-manifest.ps1` (called automatically from `deploy-ccli-session-fix.ps1`).

On the DUT after deploy:

- `/etc/ccli/VERSION` — first line of `ccli --version`
- `/etc/ccli/VERSION.json` — `ccli --version-json`
- `/etc/ccli/DEPLOY_MANIFEST.md` — copy of the manifest uploaded with that binary

**Before every deploy:**

1. Edit `apps/ccli/VERSION` (bump release line)
2. Run `scripts/sync-ccli-version.ps1`
3. Add entry to `apps/ccli/CHANGELOG.md`
4. Build (`wsl-build-ccli.sh`)
5. Deploy with `-Changes` describing this upload

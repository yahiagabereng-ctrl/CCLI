# CCLI deploy manifest — 0.1.0-r32

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-05 21:06:00 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.10.1 |
| **Version** | **0.1.0-r32** |
| **Codename** | P7-SR62-WATCHDOG |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | *(fill after `wsl-build-ccli.sh` — see post-build step below)* |
| **Revert to** | **0.1.0-r31** — `CCLI_DEPLOY_0.1.0-r31_2026-10-05_161342.*` |
| **Lab yaml** | `lab_tr400_cleartext_tsp_ttyS1.yaml` |

## Changes in this deploy

- P7-SR62 service supervision (62443 SR 6.2 partial)
- Fatal signal → crash tombstone + best-effort safe DO
- Startup `security/crash_recovered` / `unclean_restart` O.14 events
- procd `watchdog 90` + ubus ping every 30 s (hang detection)
- `security/service_monitor:started|shutdown` lifecycle events

## Post-build (before upload)

```powershell
cd C:\Yahia\projects\CCLI
.\scripts\sync-ccli-version.ps1
# WSL: lab/tg544-openwrt/wsl-build-ccli.sh
Get-FileHash lab\tg544-openwrt\ccli-bin -Algorithm SHA256
# Update SHA256 in this manifest + .json sidecar
```

## Post-deploy checks (DUT)

```bash
/usr/sbin/ccli --version
grep watchdog /etc/init.d/ccli
ccli --config /etc/ccli/lab.yaml --event-dump --count 10 | grep security
tail -20 /tmp/ccli.log
```

## Rollback to r31

1. Copy r31 `ccli-bin` (SHA256 `e3659ed074a355e100b5e5a0450c3c7174229547f262298e0ae36dda4fa8a38f`) to DUT `/tmp/ccli.new`
2. Run `sh /tmp/deploy-ccli-dut.sh`
3. Restore r31 `ccli.init` (remove `procd_set_param watchdog 90`) or redeploy r31 `.apk`
4. `/etc/init.d/ccli restart`
5. Confirm `ccli --version` → **0.1.0-r31**

Git revert: `git checkout 0.1.0-r31` (tag on r32 commit parent chain).

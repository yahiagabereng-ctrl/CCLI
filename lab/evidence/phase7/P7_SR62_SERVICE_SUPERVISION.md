# P7-SR62 — Service supervision (62443 SR 6.2 partial)

**Date:** 2026-10-05  
**Build:** **0.1.0-r32** (P7-SR62-WATCHDOG)  
**Revert baseline:** **0.1.0-r31** (P5-R02-PFSP)  
**Gate:** P7-SR62 · IEC 62443-3-3 SR 6.2 (service health) + CR 3.6 (safe DO on fault)

---

## Scope

| Item | r32 behaviour |
|------|----------------|
| Fatal crash | Tombstone `/var/lib/ccli/crash.tombstone` + best-effort safe DO |
| Unclean restart | Detect stale `/var/run/ccli-running` |
| Hang | procd `watchdog 90` + ubus ping every 30 s |
| O.14 | `security/crash_recovered`, `unclean_restart`, `service_monitor:*` |

**Not in scope:** HW WDT→DO (DRB), SIEM/IDS (full SR 6.2).

---

## Files (this revision only)

| Path | Role |
|------|------|
| `apps/ccli/core/service/service_supervision.{hpp,cpp}` | Crash/hang supervision |
| `apps/ccli/services/ccli_main.cpp` | Hooks + O.14 events |
| `package/ccli/files/ccli.init` | `watchdog 90` |
| `apps/ccli/VERSION` | `32` / `P7-SR62-WATCHDOG` |

---

## Lab verification (after deploy)

```bash
/usr/sbin/ccli --version                    # 0.1.0-r32
ccli --event-dump --count 10 | grep security
# expect: service_monitor:started

# Lab crash test (optional)
kill -SEGV $(pidof ccli); sleep 5
ccli --event-dump --count 10 | grep crash_recovered
```

---

## Revert to r31

| Step | Action |
|------|--------|
| 1 | Git: `git checkout 0.1.0-r31` or revert r32 commit |
| 2 | DUT: deploy r31 binary per `CCLI_DEPLOY_0.1.0-r31_2026-10-05_161342.md` |
| 3 | DUT: `/etc/init.d/ccli` without `watchdog 90` line |
| 4 | Verify: `ccli --version` → **0.1.0-r31** |

Manifest: [`../tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r31_2026-10-05_161342.md`](../../tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r31_2026-10-05_161342.md)

---

## Verdict

**PENDING** — deploy r32 binary + run verification above.

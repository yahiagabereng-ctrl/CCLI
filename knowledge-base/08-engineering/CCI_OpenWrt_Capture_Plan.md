# OpenWrt Procedural Docs — Capture Plan (CCLI / TG-524)

**Document ID:** CCLI-OS-002  
**Revision:** 1.0  
**Date:** 2026-09-07  
**RAG source_id:** `ccli-openwrt-capture-plan` → `ccli-openwrt-procedural-extract`  
**Normative basis:** OpenWrt 24.10.5 (frozen product baseline)  
**Parent:** `ccli-openwrt-freeze` · `ccli-tg500-knowledge-tree` (K2.3–K2.7)  
**Programme link:** K2.4 (procd/.ipk) · K2.5 (UCI) · K2.7 (sysupgrade)

---

## Summary

Procedural OpenWrt documentation was **missing** from the CCLI corpus (K2.3–K2.7 gaps). Direct download from `openwrt.org` is **blocked** (Anubis bot challenge on automated fetch). This capture uses:

| Source tier | URL pattern | Status |
|-------------|-------------|--------|
| **P0** | `oldwiki.archive.openwrt.org/_export/raw/...` | **HAVE** — UCI, config scripting, procd, packages, sysupgrade |
| **P0** | `raw.githubusercontent.com/openwrt/openwrt/...` | **HAVE** — `procd.sh`, `rc.common`, `functions.sh`, `opkg.conf` |
| **P0** | `raw.githubusercontent.com/openwrt/packages/.../libgpiod/Makefile` | **HAVE** — libgpiod **2.1.3** feed recipe |
| **P1** | `openwrt.org/docs/guide-user/...` | **BLOCKED** — bot challenge HTML only |
| **P1** | `git.kernel.org/.../libgpiod/.../examples/` | **BLOCKED** — bot challenge |
| **P2** | Live TG-524 / TesPro SDK docs | **MISSING** — K2.2 |

Raw files on disk: `knowledge-base/08-engineering/openwrt-sources/`  
Download script: `scripts/download-openwrt-wiki.sh`

---

## Capture batches

### Batch A — UCI & config scripting — **COMPLETE**

| Topic | Raw file | Extract section |
|-------|----------|-----------------|
| UCI system, syntax, CLI | `raw_uci.txt` | § UCI |
| Shell `config_*` API | `raw_config-scripting.txt` | § UCI scripting |

**Closes:** K2.5 (partial — model + persistence policy TBD for CCI)

### Batch B — procd init — **COMPLETE**

| Topic | Raw file | Extract section |
|-------|----------|-----------------|
| Init script how-to | `raw_procd-init-scripts.txt` | § procd |
| API reference | `github_openwrt_procd.sh` | § procd API |
| rc.common wrapper | `github_openwrt_rc.common` | § procd |

**Closes:** K2.4 (partial — CCI `ccli` init script not yet written)

### Batch C — Package build (.ipk) — **COMPLETE**

| Topic | Raw file | Extract section |
|-------|----------|-----------------|
| Creating packages | `raw_packages.txt` | § Packages |
| libgpiod feed | `github_openwrt-packages_libgpiod_Makefile` | § libgpiod |

**Closes:** K2.3 (partial), K2.4 (partial), K5 GPIO on OpenWrt

### Batch D — opkg & sysupgrade — **PARTIAL**

| Topic | Raw file | Extract section |
|-------|----------|-----------------|
| sysupgrade procedure | `raw_generic_sysupgrade.txt` | § opkg · § sysupgrade |
| opkg.conf defaults | `github_openwrt_opkg.conf` | § opkg |
| Dedicated opkg how-to | — | **MISSING** (oldwiki page absent) |

**Closes:** K2.7 (partial — signed A/B path still MISSING)

### Batch E — Network / firewall wiki — **NOT CAPTURED**

| Topic | Status |
|-------|--------|
| Network start | **MISSING** — openwrt.org blocked; defer to K3 when SDK arrives |
| Firewall start | **MISSING** — same |

### Batch F — Build system · SDK · Pi 3 — **COMPLETE**

| Topic | Raw file | Extract |
|-------|----------|---------|
| Install prerequisites (live wiki) | `raw_install-buildsystem.txt` | `ccli-openwrt-buildsystem-extract` §1 |
| Buildroot installation | `raw_buildroot-exigence.txt` | §2 |
| Build usage (menuconfig, make) | `raw_build-usage.txt` | §3 |
| Feeds | `raw_feeds.txt` | §2 |
| SDK usage | `raw_obtain-firmware-sdk.txt` | §4 |
| Raspberry Pi target | `raw_raspberry_pi.txt` | §5 |

**Closes:** K2.3 (PART+), Pi3 optional OpenWrt path documented

### Batch G — Package dependencies — **COMPLETE**

| Topic | Raw file | Extract |
|-------|----------|---------|
| DEPENDS syntax | `raw_dependencies.txt` | `ccli-openwrt-dependencies-extract` |

**Closes:** K2.4 (Makefile dep rules for `ccli` `.ipk`)

---

## CCLI mapping template

| OpenWrt concept | CCI use | Knowledge node |
|-----------------|---------|----------------|
| `/etc/config/ccli` UCI file | Replace `lab_pi.yaml` on TG-524 | K2.5 |
| `config_load` / `config_get` in init | Parse PF2 thresholds at start | K2.5 |
| procd init `/etc/init.d/ccli` | Run `ccli` daemon foreground | K2.4 |
| `procd_add_reload_trigger "ccli"` | Hot-reload on UCI commit | K2.4 |
| OpenWrt package Makefile | Build `.ipk` for `apps/ccli/` | K2.4 |
| `libgpiod` 2.1.3 + `CONFIG_GPIO_CDEV` | TG-524 DI/DO HAL (`hal_gpio_ll.c`) | K5 |
| `opkg install` / sysupgrade preserve | Field update of CCI package | K2.7 |

---

## Verification

| Item | Pass criteria |
|------|---------------|
| Corpus ingest | `ccli-openwrt-procedural-extract` in RAG; K2.3–K2.5 move PARTIAL |
| Download script | `bash scripts/download-openwrt-wiki.sh` exits 0; no bot-challenge files |
| libgpiod version | Extract cites **2.1.3** matching OpenWrt 24.10 feed |
| CCI init (future) | `/etc/init.d/ccli` passes `PROCD_DEBUG=1` smoke on TG-524 |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-07 | Initial capture from archive wiki + GitHub; openwrt.org blocked |

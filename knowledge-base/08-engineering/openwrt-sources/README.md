# OpenWrt source captures

Raw downloads for `CCI_OpenWrt_Procedural_Extract.md`.  
Regenerate: `bash scripts/download-openwrt-wiki.sh` (from repo root, WSL/Linux).

**Do not ingest bot-challenge HTML** from `openwrt.org` — files under 5 KB containing "Testing to determine if you are a bot" are invalid.

## Files

| File | Source | Notes |
|------|--------|-------|
| `raw_uci.txt` | oldwiki `_export/raw/doc/uci` | UCI system |
| `raw_config-scripting.txt` | oldwiki `doc/devel/config-scripting` | Shell config API |
| `raw_procd-init-scripts.txt` | oldwiki `inbox/procd-init-scripts` | Init how-to |
| `raw_generic_sysupgrade.txt` | oldwiki `doc/howto/generic.sysupgrade` | Upgrade + opkg restore |
| `raw_install-buildsystem.txt` | openwrt.org (user capture) | Build prerequisites — live wiki |
| `raw_buildroot-exigence.txt` | oldwiki `doc/howto/buildroot.exigence` | Install + disk/RAM needs |
| `raw_build-usage.txt` | oldwiki `doc/howto/build` | menuconfig, make, clean |
| `raw_feeds.txt` | oldwiki `doc/devel/feeds` | feeds.conf, scripts/feeds |
| `raw_obtain-firmware-sdk.txt` | oldwiki `doc/howto/obtain.firmware.sdk` | SDK cross-compile |
| `raw_raspberry_pi.txt` | oldwiki `toh/.../raspberry_pi` | Pi 1/2/3 targets (legacy names) |
| `raw_dependencies.txt` | oldwiki `doc/devel/dependencies` | DEPENDS / opkg syntax |
| `github_openwrt_procd.sh` | openwrt/openwrt master | procd API |
| `github_openwrt_rc.common` | openwrt/openwrt master | Init wrapper |
| `github_openwrt_functions.sh` | openwrt/openwrt master | UCI shell helpers |
| `github_openwrt-packages_libgpiod_Makefile` | openwrt/packages master | libgpiod 2.1.3 |
| `github_openwrt_opkg.conf` | openwrt/openwrt master | Default opkg paths |
| `oldwiki_*.html` | oldwiki HTML exports | Legacy; prefer `raw_*.txt` |

## HTML archives (optional)

Larger HTML exports (`oldwiki_uci.html`, etc.) remain for offline browsing; extracts use `raw_*.txt`.

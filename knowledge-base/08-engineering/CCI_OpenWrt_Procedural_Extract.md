# OpenWrt Procedural Extract — UCI · procd · packages · opkg · libgpiod

**Document ID:** CCLI-OS-003  
**Revision:** 1.0  
**Date:** 2026-09-07  
**RAG source_id:** `ccli-openwrt-procedural-extract`  
**Capture plan:** `ccli-openwrt-capture-plan`  
**Parents:** `ccli-openwrt-freeze` · `ccli-tg500-knowledge-tree`  
**Status:** **HAVE (P0 procedural)** — archive wiki + GitHub upstream  
**Product baseline:** OpenWrt **24.10.5** on TesPro TG-524 (MT798X)

---

## Requirements

| ID | Requirement | Source | Status |
|----|-------------|--------|--------|
| REQ-OW-001 | CCI services run under **procd** on TG-524 | K2.4 | **DOCUMENTED** — init pattern below |
| REQ-OW-002 | Product config via **UCI** (`/etc/config/ccli`) | K2.5 | **DOCUMENTED** — model below |
| REQ-OW-003 | Deploy as **`.ipk`** built from OpenWrt package Makefile | K2.4 | **DOCUMENTED** |
| REQ-OW-004 | GPIO via **libgpiod 2.x** character device | K5 · feed Makefile | **DOCUMENTED** — v2.1.3 in 24.10 feed |
| REQ-OW-005 | Field update preserves UCI; user packages reinstalled after sysupgrade | K2.7 | **PARTIAL** |
| REQ-OW-006 | Pi dev path unchanged (`platform_pi`, YAML) | `ccli-openwrt-freeze` | **FROZEN** |

---

## Architecture

### Layer model (TG-524 product)

```
/etc/config/ccli          ← UCI (persistent)
       ↓ config_load / uci get
/etc/init.d/ccli          ← procd init (USE_PROCD=1)
       ↓ procd_set_param command
/usr/sbin/ccli              ← cross-compiled apps/ccli/ (foreground)
       ↓ hal_gpio_ll.c
libgpiod 2.1.3              ← Linux GPIO character device (/dev/gpiochip*)
```

### Pi vs TG-524

| Concern | Pi (`platform_pi`) | TG-524 (`platform_tg500`) |
|---------|-------------------|---------------------------|
| Config | `config/lab_pi.yaml` | UCI `/etc/config/ccli` |
| Init | systemd / manual | procd `/etc/init.d/ccli` |
| GPIO HAL | libgpiod (Pi OS) or mock | libgpiod 2.1.3 from OpenWrt feed |
| Deploy | `cmake && ./ccli` | `opkg install ccli_*.ipk` |

---

## 1. UCI (Unified Configuration Interface)

**Corpus:** `openwrt-sources/raw_uci.txt`

UCI centralises OpenWrt configuration under `/etc/config/`. LuCI, shell scripts, and the `uci` CLI all read/write the same files.

### Key paths for CCI

| File | CCI relevance |
|------|---------------|
| `/etc/config/network` | Interface binding (Eth_A/B, VLAN) |
| `/etc/config/firewall` | Zone policy — aligns with `net_policy` |
| `/etc/config/system` | Hostname, NTP, logging |
| `/etc/config/ccli` | **TBD** — PF2 thresholds, Modbus map, DI/DO polarity |

### File syntax

```
package 'ccli'

config pf2 'main'
        option stale_data_s '10'
        option curtailment_delay_s '5'

config io 'plant'
        list di_pin '17'
        option do_safe '0'
```

Rules:
- Identifiers: `a-z`, `0-9`, `_` only (no hyphens).
- Boolean true: `1`, `yes`, `on`, `true`, `enabled`.
- Lists: repeated `list name value` lines merge into one list.
- After editing files directly, restart via **init.d** (not raw binary restart) so UCI-aware init regenerates app configs.

### CLI workflow

```sh
uci set ccli.main.stale_data_s=10
uci commit ccli
/etc/init.d/ccli reload    # or reload_config if triggers registered
```

Important commands: `uci show`, `uci get`, `uci set`, `uci commit`, `uci revert`, `uci export`.

Anonymous sections use `@type[index]` notation, e.g. `network.@interface[0]`.

### uci-defaults (first boot)

Scripts in `/etc/uci-defaults/` run once at first boot (exit 0 → deleted). Use for factory IP/SSID defaults; CCI factory image may seed `/etc/config/ccli` defaults here.

---

## 2. UCI scripting (`/lib/functions.sh`)

**Corpus:** `openwrt-sources/raw_config-scripting.txt` · `github_openwrt_functions.sh`

Init scripts include shell helpers:

```sh
. /lib/functions.sh
config_load ccli
config_get stale ccli.main stale_data_s 10
config_foreach handle_di io di
```

| Function | Purpose |
|----------|---------|
| `config_load <name>` | Load `/etc/config/<name>` |
| `config_get <var> <section> <option> [default]` | Read option |
| `config_get_bool <var> <section> <option> [default]` | Read as 0/1 |
| `config_set <section> <option> <value>` | In-memory only |
| `config_foreach <func> <type> [args...]` | Iterate sections |
| `config_list_foreach <section> <list> <func>` | Iterate list items |

Use `config_foreach` for structured CCI config; use callbacks (`config_cb` / `option_cb`) when generating arbitrary output files from all options in a section.

---

## 3. procd init scripts

**Corpus:** `raw_procd-init-scripts.txt` · `github_openwrt_procd.sh` · `github_openwrt_rc.common`

### Minimal CCI template

```sh
#!/bin/sh /etc/rc.common

START=99
STOP=10
USE_PROCD=1

start_service() {
        procd_open_instance
        procd_set_param command /usr/sbin/ccli --config /etc/config/ccli
        procd_set_param respawn 3600 5 5
        procd_set_param file /etc/config/ccli
        procd_set_param stdout 1
        procd_set_param stderr 1
        procd_close_instance
}

service_triggers() {
        procd_add_reload_trigger "ccli"
}

reload_service() {
        procd_send_signal ccli
}
```

### Rules

1. Shebang: `#!/bin/sh /etc/rc.common` — **not** plain `#!/bin/sh`.
2. Set `USE_PROCD=1`.
3. Daemon runs in **foreground**; procd supervises (no `-d` fork).
4. `START`/`STOP` control boot order symlinks in `/etc/rc.d/`.
5. Enable: `/etc/init.d/ccli enable` → symlinks `S99ccli` / `K10ccli`.

### procd_set_param types (from upstream `procd.sh`)

| Type | Use |
|------|-----|
| `command` | argv array — one arg per `procd_set_param` / `procd_append_param` |
| `respawn` | `<fail_threshold> <restart_timeout> <max_fail>` |
| `env` | Environment variable |
| `file` | Config file MD5 watch → reload on change |
| `netdev` | Reload when interface ifindex changes |
| `data` | Arbitrary name/value change detection |
| `user` / `group` | Drop privileges |
| `stdout` / `stderr` | Forward to logd (1 = yes) |
| `pidfile` | Write PID file |
| `limits` | ulimit, e.g. `core="unlimited"` |

### Reload model

- `procd_add_reload_trigger "ccli"` + `reload_config` → calls `/etc/init.d/ccli reload` when `/etc/config/ccli` MD5 changes.
- Default reload = stop/start unless command line unchanged.
- Prefer `procd_send_signal` (SIGHUP) for config reload without full restart.

### Debugging

```sh
PROCD_DEBUG=1 /etc/init.d/ccli start
```

### rc.common commands

`start` · `stop` · `restart` · `reload` · `enable` · `disable` · `enabled` · `running` · `status`

---

## 4. OpenWrt packages (.ipk)

**Corpus:** `raw_packages.txt`

### Directory layout

```
package/ccli/
├── Makefile          # Build recipe
├── patches/          # optional
└── files/
    ├── /etc/init.d/ccli
    └── /etc/config/ccli
```

### Skeleton Makefile (external / feed package)

```makefile
include $(TOPDIR)/rules.mk

PKG_NAME:=ccli
PKG_VERSION:=0.1.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/ccli
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=CCLI Central Plant Controller
  DEPENDS:=+libstdcpp +libgpiod
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) -r /path/to/apps/ccli/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" \
		CMAKE_FLAGS="-DBUILD_PLATFORM=tg500"
endef

define Package/ccli/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ccli $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ccli.init $(1)/etc/init.d/ccli
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/ccli.config $(1)/etc/config/ccli
endef

$(eval $(call BuildPackage,ccli))
```

Key variables: `PKG_NAME`, `PKG_VERSION`, `PKG_RELEASE`, `DEPENDS`, `Build/Compile`, `Package/<name>/install`, `$(eval $(call BuildPackage,<name>))`.

Build on SDK tree: `make package/ccli/compile V=s` → `.ipk` in `bin/packages/`.

---

## 5. opkg

**Corpus:** `raw_generic_sysupgrade.txt` (opkg sections) · `github_openwrt_opkg.conf`

Dedicated opkg wiki page **MISSING** from archive. Operational commands (from sysupgrade doc + standard OpenWrt):

```sh
opkg update
opkg install ccli
opkg list-installed
opkg list-changed-conffiles
```

Default `opkg.conf`:

```
dest root /
dest ram /tmp
lists_dir ext /var/opkg-lists
option overlay_root /overlay
```

### User-installed package tracking

After sysupgrade, manually installed packages are **not** preserved. Before upgrade, list them:

```sh
awk '/^Package:/{PKG=$2} /^Status: .*user installed/{print PKG}' /usr/lib/opkg/status
```

Reinstall after upgrade: `opkg update && opkg install <pkg> ...`

Conflicting conffiles saved as `*-opkg` during install; merge with `diff` before removing.

---

## 6. sysupgrade

**Corpus:** `raw_generic_sysupgrade.txt`

- Replaces **entire** rootfs (kernel + SquashFS + JFFS2 overlay).
- Preserves listed config under `/etc/config/` (and other documented paths).
- **Does not** preserve manually `opkg install` packages — reinstall required.
- CLI: `sysupgrade -v /tmp/openwrt-*.bin`
- LuCI: *System → Backup/Flash Firmware*

Pre-upgrade checklist for CCI:
1. Export `/etc/config/ccli` (backup).
2. List user-installed `.ipk` packages (include `ccli`, `libgpiod`, protocol libs).
3. Document overlay customisations outside `/etc/config/`.

Signed A/B update path: **MISSING** from this capture — requires TesPro SDK / product image policy (K2.7).

---

## 7. libgpiod (OpenWrt 24.10 feed)

**Corpus:** `github_openwrt-packages_libgpiod_Makefile`

| Field | Value |
|-------|-------|
| **PKG_VERSION** | **2.1.3** |
| **PKG_RELEASE** | 2 |
| **Kconfig** | `CONFIG_GPIO_CDEV=y` |
| **Packages** | `libgpiod`, `libgpiodcxx`, `gpiod-tools`, `python3-gpiod` |
| **Headers (dev)** | `gpiod.h`, `libgpiod.pc` |

TG-524 HAL (`platform/platform_tg500/hal_gpio_ll.c`) should target **libgpiod v2 API** (`gpiod_line_request_*`, chip path `/dev/gpiochipN`). Pi HAL (`platform_pi`) should use the same API for parity.

Install on device: `opkg install libgpiod gpiod-tools`

Upstream example C files blocked from automated fetch; refer to libgpiod 2.x documentation for `gpiod_chip_open`, `gpiod_line_request_new`, `gpiod_line_request_set_value`.

---

## Interface matrix

| Interface | Source | Destination | Protocol / API |
|-----------|--------|-------------|----------------|
| UCI files | LuCI / `uci` CLI | procd init scripts | `config_*` shell API |
| procd | init.d scripts | ubus | JSON over ubus |
| `.ipk` | SDK `make package/` | opkg on device | ipkg format |
| GPIO | `ccli` HAL | Linux kernel | libgpiod → `/dev/gpiochip*` |
| Config reload | `reload_config` | `/etc/init.d/ccli reload` | procd trigger |

---

## BOM matrix

| Block | Function | Candidate | Status |
|-------|----------|-----------|--------|
| OS | Product runtime | OpenWrt 24.10.5 | **FROZEN** |
| Init | Service supervisor | procd (in base system) | **HAVE** |
| Config | Persistent settings | UCI | **HAVE (docs)** |
| Packager | Field deploy | opkg + `.ipk` | **HAVE (docs)** |
| GPIO lib | DI/DO on TG-524 | libgpiod 2.1.3 | **HAVE (feed Makefile)** |
| SDK | Cross-compile | TesPro OpenWrt SDK | **MISSING** — K2.2 |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| openwrt.org live wiki | Bot-blocked | Network/firewall guides | **MISSING** — K3 |
| libgpiod examples | Not on disk | Reference toggle/read C | **MISSING** — low risk |
| TesPro SDK | None | Cross-build + feed integration | **MISSING** — K2.2 |
| CCI UCI schema | None | `/etc/config/ccli` spec | **MISSING** — K2.5 |
| Signed sysupgrade | None | Secure field update | **MISSING** — K2.7 |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-OW-001 | Archive wiki may lag 24.10 | Stale API detail | GitHub `procd.sh` / `functions.sh` as authority |
| R-OW-002 | Pi YAML vs UCI divergence | Config bugs on port | Define UCI schema; shared semantic model in `core/` |
| R-OW-003 | libgpiod v1 vs v2 API mix | HAL compile fail | Pin 2.1.3; v2-only in both HALs |
| R-OW-004 | sysupgrade drops `.ipk` | Field regression | Document reinstall list; consider image-integrated ccli |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-OW-001 | `PROCD_DEBUG=1 /etc/init.d/ccli start` on TG-524 | Process supervised; respawn on kill |
| REQ-OW-002 | `uci set` + `reload_config` | CCI picks up new threshold without full flash |
| REQ-OW-003 | `make package/ccli/compile` with SDK | Valid `.ipk` installs via opkg |
| REQ-OW-004 | `gpiodetect` + HAL unit test on TG-524 | DI/DO lines toggle |
| REQ-OW-005 | sysupgrade + reinstall script | UCI preserved; ccli restored |

---

## Keywords

OpenWrt, UCI, procd, opkg, ipk, sysupgrade, libgpiod 2.1.3, TG-524, K2.3, K2.4, K2.5, K2.7, platform_tg500

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-07 | Initial ingest from archive wiki + GitHub |

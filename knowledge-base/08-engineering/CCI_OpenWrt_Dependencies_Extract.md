# OpenWrt Package Dependencies Extract

**Document ID:** CCLI-OS-005  
**Revision:** 1.0  
**Date:** 2026-09-07  
**RAG source_id:** `ccli-openwrt-dependencies-extract`  
**Capture plan:** `ccli-openwrt-capture-plan` (Batch G)  
**Parents:** `ccli-openwrt-procedural-extract` · `ccli-openwrt-buildsystem-extract`  
**Status:** **HAVE (P0)**  
**Source:** [OpenWrt Wiki — Using Dependencies](https://openwrt.org/docs/guide-developer/dependencies#using_dependencies) · `openwrt-sources/raw_dependencies.txt`

---

## Requirements

| ID | Requirement | Status |
|----|-------------|--------|
| REQ-DEP-001 | CCI `.ipk` declares correct **runtime** deps for opkg | **DOCUMENTED** |
| REQ-DEP-002 | Build-time libs (headers) use `PKG_BUILD_DEPENDS` | **DOCUMENTED** |
| REQ-DEP-003 | GPIO packages gated on `@GPIO_SUPPORT` | **DOCUMENTED** — from libgpiod feed |
| REQ-DEP-004 | No circular deps in CCI feed | **RULE** |
| REQ-DEP-005 | Field `opkg install` must not rely on `+@CONFIG_*` only | **DOCUMENTED** |

---

## Architecture

OpenWrt resolves dependencies at **three layers**:

```
menuconfig (image build)     DEPENDS / @SYMBOL / Package/*/config select
        ↓
compile order              PKG_BUILD_DEPENDS (+ DEPENDS for staging)
        ↓
opkg on device             +package entries in DEPENDS only (not +@SYMBOL)
```

For CCLI: the **`ccli`** package Makefile must use `DEPENDS:=+…` for everything `opkg` must pull on TG-524 or Pi3-OpenWrt.

---

## DEPENDS syntax reference

| Form | Meaning | opkg enforced? |
|------|---------|----------------|
| `libpcap` | Hide unless `libpcap` already selected | N/A (menuconfig gate) |
| `+libpcap` | Auto-select `libpcap` when this package selected | **Yes** |
| `+PACKAGE_a:b` | Select `b` only if package `a` enabled | **Yes** (conditional) |
| `+!BUSYBOX_CONFIG_X:pkg` | Select `pkg` if busybox lacks applet X | **Yes** |
| `@GPIO_SUPPORT` | Package invisible unless Kconfig symbol set | Image build only |
| `+@KERNEL_DEBUG_FS` | Selecting package sets kernel symbol | **No** on running box |

### Boolean operators (in `+SYMBOL:pkg` forms)

- `!` — negates whole condition
- `&&` — higher precedence than `||`
- Parentheses in `+(A&&B):pkg` are **ignored** for grouping — use `+A||(B&&C):pkg` for readability only

---

## PKG_BUILD_DEPENDS vs DEPENDS

| Variable | When | Example (CCLI) |
|----------|------|----------------|
| `PKG_BUILD_DEPENDS` | Compile on build host; headers/tools | `cmake/host`, `ninja/host` if wrapping cmake |
| `DEPENDS` | Runtime on target; **opkg metadata** | `+libstdcpp +libgpiod +libmodbus` |

From upstream **libgpiod** feed Makefile:

```makefile
define Package/libgpiod
  DEPENDS:=@GPIO_SUPPORT
endef

define Package/gpiod-tools
  DEPENDS:=+libgpiod
endef
```

`@GPIO_SUPPORT` hides the package unless kernel GPIO cdev is enabled — correct for menuconfig. **`ccli`** should use `DEPENDS:=+libgpiod` (with `+`), not `@GPIO_SUPPORT` alone, so opkg installs libgpiod on a running gateway.

---

## Recommended CCI `ccli` Makefile deps (draft)

```makefile
define Package/ccli
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=CCLI Central Plant Controller
  DEPENDS:=+libstdcpp +libgpiod +zlib
  # Optional protocol libs when packaged separately:
  # DEPENDS:=+libstdcpp +libgpiod +libmodbus +libopenssl
endef
```

| Dependency | Syntax | Why |
|------------|--------|-----|
| C++ runtime | `+libstdcpp` | `apps/ccli/` is C/C++ |
| GPIO HAL | `+libgpiod` | TG-524 / Pi OpenWrt DI/DO |
| TLS (C1) | `+libopenssl` | When IEC 62351-3 stack linked |
| Modbus | `+libmodbus` | Plant interface adapter |

Use **`+`** prefix on every runtime library so `opkg install ccli` pulls them on TG-524 without a custom image.

---

## Package/config — when to use

`define Package/ccli/config` directives can `select libgpiod`, but that:

- Does **not** add opkg runtime dependency
- Does **not** guarantee build order

**Always duplicate runtime needs in `DEPENDS:=+…`.** Use `Package/ccli/config` only for optional Kconfig toggles (e.g. enable MMS vs 104-only build).

---

## Anti-patterns (CCLI)

| Pattern | Problem | Fix |
|---------|---------|-----|
| `DEPENDS:=+@BUSYBOX_CONFIG_FOO` | opkg cannot resolve CONFIG symbol | Use `+real-package` |
| `DEPENDS:=libgpiod` (no `+`) | User must manually enable libgpiod first | Use `+libgpiod` |
| Circular `ccli` ↔ custom lib | menuconfig loops / odd errors | One-way deps only |
| `select libgpiod` in config only | Missing on `opkg install` | Add `DEPENDS:=+libgpiod` |

---

## Interface matrix

| Consumer | Depends on | Mechanism |
|----------|------------|-----------|
| `ccli` binary | libgpiod, libstdc++ | `DEPENDS:=+…` |
| `ccli` build | SDK staging headers | OpenWrt `STAGING_DIR` + `PKG_BUILD_DEPENDS` |
| Image integrator | kernel GPIO cdev | `@GPIO_SUPPORT` in libgpiod; enable in `make menuconfig` |
| Field update | opkg | `+package` entries only |

---

## Knowledge gaps

| Area | Gap |
|------|-----|
| Exact CCI protocol `.ipk` split | Single `ccli` vs split `ccli-modbus` packages TBD |
| TesPro prebuilt image packages | Which symbols already `y` on TG-524 image unknown until SDK |

---

## Verification

| Test | Pass criteria |
|------|---------------|
| `make package/ccli/menuconfig` | No circular dependency warning |
| Fresh OpenWrt image without ccli | `opkg install ccli_*.ipk` pulls libgpiod + libstdcpp |
| `opkg info ccli` | Depends: lists `libgpiod`, `libstdcpp` |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-07 | Ingest from openwrt.org dependencies wiki |

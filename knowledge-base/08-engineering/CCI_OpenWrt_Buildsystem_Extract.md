# OpenWrt Build System Extract — install · build · SDK · Pi 3

**Document ID:** CCLI-OS-004  
**Revision:** 1.0  
**Date:** 2026-09-07  
**RAG source_id:** `ccli-openwrt-buildsystem-extract`  
**Capture plan:** `ccli-openwrt-capture-plan` (Batch F)  
**Parents:** `ccli-openwrt-freeze` · `ccli-openwrt-procedural-extract` · `ccli-app-structure`  
**Status:** **HAVE (P0)** — user-provided live wiki + archive wiki  
**Product baseline:** OpenWrt **24.10.5** (TesPro); Pi 3 optional target **bcm27xx/bcm2710**

---

## Requirements

| ID | Requirement | Pi 3 relevance | Status |
|----|-------------|----------------|--------|
| REQ-BS-001 | Document host prerequisites for OpenWrt build | WSL/x86 host for cross-build | **HAVE** |
| REQ-BS-002 | Document clone → feeds → menuconfig → make flow | Same flow for any target | **HAVE** |
| REQ-BS-003 | Pi 3 L1 app test via `platform_pi` (native) | **Primary path — frozen** | **ACTIVE** |
| REQ-BS-004 | Optional OpenWrt-on-Pi3 for procd/UCI L2 | Flash **24.10.x** `bcm2710` image | **DOCUMENTED** |
| REQ-BS-005 | Cross-SDK for TG-524 from TesPro | **Not Pi3** — K2.2 | **MISSING** |

---

## Pi 3 — three paths (read this first)

CCLI uses Raspberry Pi 3 in **different roles**. Do not conflate them.

| Path | Role | OS | Build command | When to use |
|------|------|-----|---------------|-------------|
| **A — Native app (P0)** | Run `apps/ccli/` unit tests + L1/L2 | **Raspberry Pi OS** | `cmake -DBUILD_PLATFORM=pi` | **Now** — frozen in `ccli-openwrt-freeze` |
| **B — OpenWrt on Pi3 (P2)** | Test procd, UCI, `.ipk`, libgpiod on real OpenWrt | **OpenWrt 24.10.x** on SD | Cross-build `.ipk` on x86; flash image | Before TG-524; optional L2 |
| **C — Build host (P1)** | Compile OpenWrt / SDK / packages | **WSL Ubuntu 24.04** (x86_64) | Full buildroot on PC | Cross-compile for Pi3 or TG-524 |

**Pi 3 is NOT a recommended OpenWrt build host** (1 GB RAM, slow storage, ARM). Use WSL per Path C.

---

## Architecture

### Path A — `platform_pi` on Raspberry Pi OS (primary)

```
Pi 3 (Raspberry Pi OS, aarch32 or aarch64)
  ├── cmake -DBUILD_PLATFORM=pi
  ├── libgpiod-dev (optional; mock HAL until wired)
  ├── config/lab_pi.yaml
  └── ./ccli  — no OpenWrt buildsystem required
```

**Native deps on Pi 3** (minimal for `apps/ccli/`):

```bash
sudo apt update
sudo apt install build-essential cmake git libgpiod-dev pkg-config
```

This is **not** the OpenWrt prerequisite list below — that list is for building OpenWrt itself.

### Path B — OpenWrt 24.10 on Pi 3 / Pi 4

| Model | Target | Subtarget | Factory image |
|-------|--------|-----------|---------------|
| Pi 3 / 3 B+ | `bcm27xx` | `bcm2710` | `rpi-3-squashfs-factory.img.gz` |
| **Pi 4 / 4 B** | `bcm27xx` | **`bcm2711`** | **`rpi-4-squashfs-factory.img.gz`** |

| Field | Pi 4 value |
|-------|------------|
| Download | [24.10.5 bcm2711](https://downloads.openwrt.org/releases/24.10.5/targets/bcm27xx/bcm2711/) |
| Default LAN | `192.168.1.1` (adjust for lab `192.168.10.0/24`) |

After flash: install `libgpiod`, build `ccli` `.ipk` cross-compiled from x86 SDK, test procd init from `ccli-openwrt-procedural-extract`.

**GPIO on Pi 3 under OpenWrt:** enable in `/boot/config.txt` on FAT partition if needed; use libgpiod 2.x (`CONFIG_GPIO_CDEV`).

### Path C — x86 build host (WSL)

```
WSL Ubuntu 24.04 (x86_64)
  ├── git clone openwrt/openwrt -b openwrt-24.10
  ├── ./scripts/feeds update -a && ./scripts/feeds install -a
  ├── make menuconfig → Target: bcm27xx → bcm2710 (Pi3) OR MediaTek (TG-524)
  ├── make download && make -j$(nproc)
  └── bin/…/packages/*.ipk
```

Disk: **≥ 4 GB** free; RAM: **≥ 4 GB** for full build (per archive wiki).

---

## 1. Build system setup (prerequisites)

**Corpus:** `openwrt-sources/raw_install-buildsystem.txt` (live wiki, user capture 2026-09-07)

### Steps (summary)

1. Use GNU/Linux (WSL acceptable; Cygwin **not** supported).
2. Install `git` + distro build-tools metapackage.
3. Install prerequisite packages from table below.
4. **Unset** `SED` and `GREP_OPTIONS`; avoid aliased `which`.

### Ubuntu 24.04 (OpenWrt main / master — matches WSL lab)

```bash
sudo apt update
sudo apt install build-essential clang flex bison g++ gawk \
  gcc-multilib g++-multilib gettext git libncurses5-dev libssl-dev \
  python3-setuptools rsync swig unzip zlib1g-dev file wget
```

Note: **python2 removed** from OpenWrt 22.03+; use **python3** only.

### Ubuntu 22.04 (add distutils)

```bash
sudo apt install build-essential clang flex bison g++ gawk \
  gcc-multilib g++-multilib gettext git libncurses-dev libssl-dev \
  python3-distutils python3-setuptools rsync swig unzip zlib1g-dev file wget
```

### Environment pitfalls

| Variable | Action |
|----------|--------|
| `SED` | `unset SED` before build (Ticket 10612) |
| `GREP_OPTIONS` | Must not include `--initial-tab` |
| Build path | No spaces in path; non-root user |

---

## 2. Obtain source and feeds

**Corpus:** `raw_buildroot-exigence.txt` · `raw_feeds.txt`

```bash
git clone https://github.com/openwrt/openwrt.git
cd openwrt
git checkout openwrt-24.10    # align with frozen 24.10.5 baseline

./scripts/feeds update -a
./scripts/feeds install -a      # or install only needed packages

make menuconfig                 # first run checks prerequisites
```

### Feeds (default `feeds.conf.default`)

| Feed | Source |
|------|--------|
| packages | `https://github.com/openwrt/packages.git` |
| luci | LuCI web UI |
| routing | routing packages |

Custom CCI feed (future): `src-link ccli /path/to/ccli-package` + `./scripts/feeds update ccli`.

---

## 3. Build procedure

**Corpus:** `raw_build-usage.txt`

```bash
make menuconfig    # Target System → bcm27xx → bcm2710 for Pi 3
make defconfig     # expand diffconfig if used
make download      # prefetch sources (required before -j)
make -j$(nproc) V=s
```

### menuconfig symbols

| Key | Meaning |
|-----|---------|
| `y` `<*>` | Built into firmware image |
| `m` `<M>` | Built as `.ipk` module only |
| `n` `< >` | Excluded |

### Single package (CCI `.ipk` dev loop)

```bash
make package/ccli/compile V=s
make package/ccli/{clean,compile} V=s
```

### Custom files in image

Place factory defaults under `openwrt/files/` e.g. `files/etc/config/ccli`.

### Clean targets

| Command | Effect |
|---------|--------|
| `make clean` | Clears `bin/`, `build_dir/` |
| `make dirclean` | + toolchain, staging_dir |
| `make distclean` | **Erases `.config`** — use with care |

---

## 4. SDK (cross-compile packages without full image)

**Corpus:** `raw_obtain-firmware-sdk.txt`

Download prebuilt SDK (x86_64 host → ARM target):

```
openwrt-sdk-24.10.x-bcm27xx-bcm2710_gcc-*.Linux-x86_64.tar.zst
```

From: `https://downloads.openwrt.org/releases/24.10.5/targets/bcm27xx/bcm2710/`

Usage:

```bash
tar xf openwrt-sdk-*.tar.zst
cd openwrt-sdk-*
./scripts/feeds update -a
./scripts/feeds install libgpiod
make package/ccli/compile V=s
# → bin/packages/*/ccli_*.ipk
```

**TG-524:** use TesPro-supplied SDK when available (K2.2) — not the Pi3 SDK.

---

## 5. Raspberry Pi hardware notes (archive)

**Corpus:** `raw_raspberry_pi.txt`

| Model | SoC | OpenWrt subtarget |
|-------|-----|-------------------|
| Pi 1 | BCM2835 | bcm2708 |
| Pi 2 | BCM2836 | bcm2709 |
| **Pi 3** | BCM2837 | **bcm2710** |
| Pi 3 B+ | BCM2837B0 | bcm2710 |
| **Pi 4 / 4 B** | BCM2711 | **bcm2711** |
| Pi 5 | BCM2712 | bcm2712 |

- **Ethernet:** USB hub chip (LAN9514) — 100 Mbit; adequate for lab PF2/Modbus.
- **Serial console:** GPIO 8/10, 115200 8N1, 3.3 V.
- **I2C/SPI:** `dtparam=i2c1=on`, `dtparam=spi=on` in `/boot/config.txt`.
- **Power:** ≥ 1 A recommended (800 mA minimum quoted).

---

## Interface matrix

| Activity | Host | Target | Tool |
|----------|------|--------|------|
| L1 unit tests | Pi 3 | Pi 3 (native) | cmake / ctest |
| L2 HIL | Pi 3 | Pi 3 (native) | `./ccli` + RS485 USB |
| OpenWrt L2 | x86 WSL | Pi 3 (OpenWrt) | SDK → `.ipk` → opkg |
| Product L3 | x86 + TesPro SDK | TG-524 | cross cmake / `.ipk` |

---

## BOM matrix

| Block | Pi 3 lab | Status |
|-------|----------|--------|
| Pi 3 B/B+ | Test DUT @ 192.168.10.20 | **ACTIVE** |
| SD card | Raspberry Pi OS (Path A) or OpenWrt image (Path B) | **PARTIAL** |
| WSL Ubuntu 24.04 | OpenWrt build host | **AVAILABLE** |
| TesPro SDK | TG-524 cross-compile | **MISSING** |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| Pi3 OpenWrt lab runbook | None | Flash + network + deploy ccli ipk | **MISSING** |
| Pi3 native build runbook | Scaffold | Documented cmake deps above | **PARTIAL** |
| TesPro SDK | None | MT798X toolchain | **MISSING** — K2.2 |
| libgpiod on Pi3 HAL | Mock | Real `hal_gpio_ll.c` | **PARTIAL** |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-BS-001 | Full OpenWrt build on Pi3 | OOM / days of compile | Build on WSL only |
| R-BS-002 | Pi OS vs OpenWrt-on-Pi divergence | procd/UCI untested | Optional Path B before TG-524 |
| R-BS-003 | brcm2708 vs bcm27xx naming | Wrong image | Use **24.10.5 bcm27xx/bcm2710** URLs |
| R-BS-004 | SDK Pi3 ≠ SDK TG-524 | Wrong binary ABI | Separate SDK per target |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-BS-001 | `make menuconfig` on WSL after apt install | No missing prereq errors |
| REQ-BS-003 | `cmake -DBUILD_PLATFORM=pi && ctest` on Pi3 | `io_safe_state` passes |
| REQ-BS-004 | Flash 24.10.5 Pi3 image; SSH | OpenWrt banner; `opkg list-installed` |
| REQ-BS-004 | `opkg install ccli_*.ipk` on Pi3 OpenWrt | procd starts `/usr/sbin/ccli` |

---

## Source files

| File | Origin |
|------|--------|
| `raw_install-buildsystem.txt` | openwrt.org (user capture) |
| `raw_buildroot-exigence.txt` | oldwiki archive |
| `raw_build-usage.txt` | oldwiki archive |
| `raw_feeds.txt` | oldwiki archive |
| `raw_obtain-firmware-sdk.txt` | oldwiki archive |
| `raw_raspberry_pi.txt` | oldwiki archive |

Regenerate: `bash scripts/download-openwrt-wiki.sh`

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-07 | Buildsystem + Pi3 path matrix; live wiki ingest |

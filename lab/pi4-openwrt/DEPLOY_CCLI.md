# ARCHIVED — see `ARCHIVED.md`. Active deploy: `../tg544-openwrt/DEPLOY_CCLI.md`

# Deploy CCLI `.ipk` on Pi 4 OpenWrt (historical)

Prerequisites: OpenWrt **24.10.5** flashed (`FLASH_PI4_OPENWRT.md`), SSH to `root@192.168.1.1`.

## 1. Download SDK (build host — WSL Ubuntu 24.04)

```bash
cd ~
wget https://downloads.openwrt.org/releases/24.10.5/targets/bcm27xx/bcm2711/openwrt-sdk-24.10.5-bcm27xx-bcm2711_gcc-13.3.0_musl.Linux-x86_64.tar.zst
tar --zstd -xf openwrt-sdk-24.10.5-bcm27xx-bcm2711_*.tar.zst
cd openwrt-sdk-*
```

Install build deps if missing: `sudo apt install build-essential clang flex bison g++ gawk gcc-multilib git gettext libncurses-dev libssl-dev python3-distutils rsync unzip zlib1g-dev file wget`

## 2. Link CCLI feed and build

```bash
REPO=/mnt/c/Yahia/projects/CCLI   # adjust path

echo "src-link ccli ${REPO}/package" >> feeds.conf
./scripts/feeds update ccli
./scripts/feeds install ccli
make defconfig
echo 'CONFIG_PACKAGE_ccli=y' >> .config
make oldconfig
make package/ccli/compile V=s
make package/index
```

## 3. Install on Pi

```bash
IPK=$(ls bin/packages/*/base/ccli_*.ipk | head -1)
scp "$IPK" root@192.168.1.1:/tmp/
ssh root@192.168.1.1 'opkg install /tmp/ccli_*.ipk && /etc/init.d/ccli start'
```

Verify:

```bash
ssh root@192.168.1.1 'pgrep -a ccli; logread | grep ccli | tail -5'
```

Browser: **http://192.168.1.1** → **System → Startup** → `ccli` enabled.

## 4. Lab network (optional)

If not done yet:

```bash
ssh root@192.168.1.1 'sh -s' < openwrt-first-boot.sh
```

Reconnect at `192.168.10.20` (PC on `192.168.10.10`).

## Troubleshooting

| Issue | Fix |
|-------|-----|
| `CCLI_SRC_DIR` not found | Build from full repo; or `make ... CCLI_SRC_DIR=/path/to/apps/ccli` |
| `libgpiod` missing | `opkg install libgpiod` before ccli |
| Service exits immediately | `PROCD_DEBUG=1 /etc/init.d/ccli start`; check `logread` |
| GPIO no relay | Check `gpiodetect`; `logread` for libgpiod errors; lines 5/27 free |

Package source: `package/ccli/` in this repo.

#!/bin/bash
set -euo pipefail

REPO="/mnt/c/Yahia/projects/CCLI"
SDK="$HOME/openwrt-sdk-24.10.5-bcm27xx-bcm2711_gcc-13.3.0_musl.Linux-x86_64"

cd "$SDK"
mkdir -p staging_dir/host
touch staging_dir/host/.prereq-build

if ! grep -q 'src-link ccli' feeds.conf 2>/dev/null; then
    echo "src-link ccli ${REPO}/package" >> feeds.conf
fi

./scripts/feeds update ccli
./scripts/feeds update packages
./scripts/feeds install -p ccli ccli
./scripts/feeds install -p packages libgpiod

make defconfig
grep -q 'CONFIG_PACKAGE_ccli=y' .config || echo 'CONFIG_PACKAGE_ccli=y' >> .config
grep -q 'CONFIG_PACKAGE_libgpiod=y' .config || echo 'CONFIG_PACKAGE_libgpiod=y' >> .config
make oldconfig

echo "=== Building libgpiod (headers for ccli) ==="
make package/feeds/packages/libgpiod/compile V=s

echo "=== Building ccli ==="
make package/ccli/compile V=s
make package/index

OUT_DIR="${REPO}/lab/pi4-openwrt"
mkdir -p "$OUT_DIR"
IPK=$(ls -1 bin/packages/*/base/ccli_*.ipk | head -1)
cp -f "$IPK" "$OUT_DIR/"
echo "BUILT ${OUT_DIR}/$(basename "$IPK")"

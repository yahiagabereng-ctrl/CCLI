#!/bin/bash
set -euo pipefail

REPO="/mnt/c/Yahia/projects/CCLI"
SDK_URL="https://downloads.openwrt.org/releases/24.10.5/targets/bcm27xx/bcm2711/openwrt-sdk-24.10.5-bcm27xx-bcm2711_gcc-13.3.0_musl.Linux-x86_64.tar.zst"
SDK_DIR="$HOME/openwrt-sdk-24.10.5-bcm27xx-bcm2711_gcc-13.3.0_musl.Linux-x86_64"
SDK_TAR_ZST="$HOME/openwrt-sdk.tar.zst"
SDK_TAR="$HOME/openwrt-sdk.tar"

cd "$HOME"
if [ ! -d "$SDK_DIR" ]; then
    if [ ! -f "$SDK_TAR" ] && [ -f "$SDK_TAR_ZST" ]; then
        echo "=== Decompressing SDK (needs zstd or Python on host) ==="
        if command -v zstd >/dev/null 2>&1; then
            zstd -d -f "$SDK_TAR_ZST" -o "$SDK_TAR"
        elif command -v python3 >/dev/null 2>&1; then
            python3 - <<'PY'
import zstandard as z
with open("/home/yahia/openwrt-sdk.tar.zst","rb") as fi, open("/home/yahia/openwrt-sdk.tar","wb") as fo:
    z.ZstdDecompressor().copy_stream(fi, fo)
PY
        else
            echo "Install zstd or: pip install zstandard" >&2
            exit 1
        fi
    fi
    if [ ! -f "$SDK_TAR" ]; then
        echo "=== Downloading OpenWrt SDK (~220 MB) ==="
        wget -O "$SDK_TAR_ZST" "$SDK_URL"
        if command -v zstd >/dev/null 2>&1; then
            zstd -d -f "$SDK_TAR_ZST" -o "$SDK_TAR"
        else
            python3 - <<'PY'
import zstandard as z
with open("/home/yahia/openwrt-sdk.tar.zst","rb") as fi, open("/home/yahia/openwrt-sdk.tar","wb") as fo:
    z.ZstdDecompressor().copy_stream(fi, fo)
PY
        fi
    fi
    tar -xf "$SDK_TAR"
fi

cd "$SDK_DIR"

if ! grep -q 'src-link ccli' feeds.conf 2>/dev/null; then
    echo "src-link ccli ${REPO}/package" >> feeds.conf
fi

./scripts/feeds update ccli
./scripts/feeds install ccli

make defconfig
if ! grep -q 'CONFIG_PACKAGE_ccli=y' .config; then
    echo 'CONFIG_PACKAGE_ccli=y' >> .config
fi
if ! grep -q 'CONFIG_PACKAGE_libgpiod=y' .config; then
    echo 'CONFIG_PACKAGE_libgpiod=y' >> .config
fi
make oldconfig

echo "=== Building ccli package ==="
make package/ccli/compile V=s
make package/index

OUT_DIR="${REPO}/lab/pi4-openwrt"
mkdir -p "$OUT_DIR"
IPK=$(ls -1 bin/packages/*/base/ccli_*.ipk | head -1)
cp -f "$IPK" "$OUT_DIR/"
echo "=== Built: $OUT_DIR/$(basename "$IPK") ==="
ls -lh "$OUT_DIR"/*.ipk

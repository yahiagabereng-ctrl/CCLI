#!/usr/bin/env bash
# Build ccli.ipk for TG544 using OpenWrt 25.12.5 mediatek/filogic SDK.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SCRIPT_DIR/../.." && pwd)"

RELEASE="25.12.5"
SDK_NAME="openwrt-sdk-${RELEASE}-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64"
SDK="${OPENWRT_SDK_HOME:-$HOME}/$SDK_NAME"

if [[ ! -d "$SDK" ]]; then
  echo "SDK not found. Run: $SCRIPT_DIR/download-sdk.sh" >&2
  exit 1
fi

cd "$SDK"

mkdir -p staging_dir/host tmp/info
touch staging_dir/host/.prereq-build

if [[ ! -f feeds.conf ]] || ! grep -q 'src-git packages' feeds.conf 2>/dev/null; then
  cp -f feeds.conf.default feeds.conf
fi
if ! grep -q 'src-link ccli' feeds.conf 2>/dev/null; then
  echo "src-link ccli ${REPO}/package" >> feeds.conf
fi

./scripts/feeds update -a
./scripts/feeds install -p ccli ccli
./scripts/feeds install -p packages libmodbus

make defconfig
grep -q 'CONFIG_PACKAGE_ccli=y' .config || echo 'CONFIG_PACKAGE_ccli=y' >> .config
grep -q 'CONFIG_PACKAGE_libmodbus=y' .config || echo 'CONFIG_PACKAGE_libmodbus=y' >> .config
make oldconfig

echo "=== Building libmodbus ==="
make package/feeds/packages/libmodbus/compile V=s

echo "=== Building ccli (platform_tg500) ==="
make package/feeds/ccli/ccli/clean V=s 2>/dev/null || true
make package/feeds/ccli/ccli/compile V=s
make package/index

OUT_DIR="$SCRIPT_DIR"
mkdir -p "$OUT_DIR"
APK=$(ls -1 bin/packages/*/ccli/ccli-*.apk | head -1)
cp -f "$APK" "$OUT_DIR/"
echo "BUILT ${OUT_DIR}/$(basename "$APK")"

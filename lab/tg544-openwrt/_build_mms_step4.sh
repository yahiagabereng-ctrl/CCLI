#!/usr/bin/env bash
set -eu
REPO="/mnt/c/Yahia/projects/CCLI"
SDK="${HOME}/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64"
OUT="/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt"
cd "$SDK"
mkdir -p staging_dir/host tmp/info
touch staging_dir/host/.prereq-build
if ! grep -q 'src-link ccli' feeds.conf 2>/dev/null; then
  echo "src-link ccli ${REPO}/package" >> feeds.conf
fi
./scripts/feeds update ccli
./scripts/feeds install -p ccli ccli
./scripts/feeds install -p packages libmodbus || true
make package/feeds/ccli/ccli/clean V=s 2>/dev/null || true
make package/feeds/ccli/ccli/compile V=s -j"$(nproc)"
make package/index || true
APK=$(ls -1 bin/packages/*/ccli/ccli-*.apk 2>/dev/null | head -1)
if [ -z "$APK" ]; then
  APK=$(find bin -name 'ccli-*.apk' 2>/dev/null | head -1)
fi
cp -f "$APK" "$OUT/"
echo "BUILT $OUT/$(basename "$APK")"
TMP=$(mktemp -d)
tar -xzf "$APK" -C "$TMP" 2>/dev/null || (cd "$TMP" && ar x "$APK" && tar xf data.tar.gz)
BIN=$(find "$TMP" -name ccli -type f 2>/dev/null | head -1)
if [ -n "$BIN" ]; then
  cp -f "$BIN" "$OUT/ccli-bin"
  chmod +x "$OUT/ccli-bin"
  echo "EXTRACTED $OUT/ccli-bin"
fi

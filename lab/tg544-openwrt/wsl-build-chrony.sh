#!/usr/bin/env bash
# Build chrony (OpenWrt packages feed) for TG544 aarch64.
set -eu
REPO="/mnt/c/Yahia/projects/CCLI"
SDK="${HOME}/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64"
OUT="$REPO/lab/tg544-openwrt"
cd "$SDK"
mkdir -p staging_dir/host tmp/info
touch staging_dir/host/.prereq-build
./scripts/feeds update packages 2>/dev/null || true
./scripts/feeds install -p packages chrony libcap
# libcap often needed
./scripts/feeds install -p packages libcap 2>/dev/null || true
make package/feeds/packages/libcap/compile V=s -j"$(nproc)" || true
make package/feeds/packages/chrony/clean V=s 2>/dev/null || true
make package/feeds/packages/chrony/compile V=s -j"$(nproc)"
make package/index || true
mkdir -p "$OUT/chrony-pkg"
# Collect apks
find bin/packages -name 'chrony*.apk' -o -name 'libcap*.apk' 2>/dev/null | while read -r f; do
  cp -f "$f" "$OUT/chrony-pkg/"
  echo "COPIED $f"
done
# Extract chronyd/chronyc
PKGDIR=$(find build_dir -path '*/.pkgdir/chrony/usr/sbin/chronyd' 2>/dev/null | head -1)
if [ -n "$PKGDIR" ]; then
  ROOT=$(dirname "$(dirname "$(dirname "$PKGDIR")")")
  mkdir -p "$OUT/chrony-root"
  cp -a "$ROOT"/. "$OUT/chrony-root/" 2>/dev/null || {
    cp -f "$PKGDIR" "$OUT/chronyd"
    cp -f "$(dirname "$PKGDIR")/../bin/chronyc" "$OUT/chronyc" 2>/dev/null || \
      cp -f "$(find build_dir -path '*/.pkgdir/chrony/usr/bin/chronyc' | head -1)" "$OUT/chronyc"
  }
  echo "EXTRACTED chrony from .pkgdir"
fi
find "$OUT" -name 'chronyd' -o -name 'chronyc' 2>/dev/null | head
file "$OUT/chronyd" 2>/dev/null || file "$(find "$OUT/chrony-root" -name chronyd | head -1)"

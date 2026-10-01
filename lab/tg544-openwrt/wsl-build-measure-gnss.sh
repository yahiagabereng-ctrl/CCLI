#!/usr/bin/env bash
# Cross-compile measure_gnss_offset for TG544 (aarch64 musl).
set -eu
SDK="${HOME}/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64"
REPO="/mnt/c/Yahia/projects/CCLI"
OUT="$REPO/lab/tg544-openwrt/measure_gnss_offset"
CC=$(find "$SDK/staging_dir" -path '*/bin/aarch64-openwrt-linux-musl-gcc' | head -1)
if [ -z "$CC" ]; then
  echo "ERROR: aarch64 cross-gcc not found under $SDK" >&2
  exit 1
fi
"$CC" -O2 -Wall -static "$REPO/lab/measure_gnss_offset.c" -o "$OUT"
chmod +x "$OUT"
echo "BUILT $OUT"
file "$OUT"

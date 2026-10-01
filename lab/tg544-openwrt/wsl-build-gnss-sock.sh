#!/usr/bin/env bash
# Cross-compile gnss_chrony_sock for TG544.
set -eu
SDK="${HOME}/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64"
REPO="/mnt/c/Yahia/projects/CCLI"
OUT="$REPO/lab/tg544-openwrt/gnss_chrony_sock"
CC=$(find "$SDK/staging_dir" -path '*/bin/aarch64-openwrt-linux-musl-gcc' | head -1)
"$CC" -O2 -Wall -static "$REPO/lab/gnss_chrony_sock.c" -o "$OUT" -lm
chmod +x "$OUT"
echo "BUILT $OUT"
file "$OUT"

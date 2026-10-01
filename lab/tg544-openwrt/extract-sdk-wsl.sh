#!/usr/bin/env bash
# Extract OpenWrt SDK on WSL ext4 ($HOME). Required: Windows DrvFS breaks kernel-tree symlinks.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ARCHIVE="$SCRIPT_DIR/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64.tar.zst"
SDK_NAME="openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64"
OUT="${OPENWRT_SDK_HOME:-$HOME}/$SDK_NAME"

if [[ ! -f "$ARCHIVE" ]]; then
  echo "Missing archive. Run download-sdk.sh first." >&2
  exit 1
fi

if [[ -f "$OUT/Makefile" ]]; then
  echo "SDK already at $OUT"
  exit 0
fi

ZSTD_BIN=""
if command -v zstd >/dev/null 2>&1; then
  ZSTD_BIN=zstd
else
  TMP="/tmp/zstd-bootstrap-$$"
  mkdir -p "$TMP"
  echo "Bootstrapping zstd (no system zstd) ..."
  wget -q -O "$TMP/zstd-src.tar.gz" https://github.com/facebook/zstd/releases/download/v1.5.7/zstd-1.5.7.tar.gz
  tar xzf "$TMP/zstd-src.tar.gz" -C "$TMP"
  make -C "$TMP/zstd-1.5.7/lib" >/dev/null
  make -C "$TMP/zstd-1.5.7/programs" zstd >/dev/null
  ZSTD_BIN="$TMP/zstd-1.5.7/programs/zstd"
fi

echo "Extracting to $OUT (WSL Linux filesystem) ..."
rm -rf "$OUT"
mkdir -p "$OUT"
"$ZSTD_BIN" -d -c "$ARCHIVE" | tar -xf - -C "$OUT" --strip-components=1

if [[ ! -f "$OUT/Makefile" ]]; then
  echo "Extract failed — Makefile missing" >&2
  exit 1
fi

echo "SDK ready: $OUT"
echo "Export: export OPENWRT_SDK_HOME=\"$(dirname "$OUT")\""

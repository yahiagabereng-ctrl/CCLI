#!/usr/bin/env bash
# Download and extract OpenWrt 25.12.5 SDK for mediatek/filogic (MT798X).
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

RELEASE="25.12.5"
TARGET="mediatek/filogic"
SDK_NAME="openwrt-sdk-${RELEASE}-${TARGET//\//-}_gcc-14.3.0_musl.Linux-x86_64"
BASE_URL="https://downloads.openwrt.org/releases/${RELEASE}/targets/${TARGET}"
ARCHIVE="${SDK_NAME}.tar.zst"
URL="${BASE_URL}/${ARCHIVE}"
SHA256="ff4a38a397caa2cfe1c39e18f84ddede14878221b3593c3f2c4cfe24e3ec4c25"

WSL_OUT="${OPENWRT_SDK_HOME:-$HOME}/$SDK_NAME"
if [[ -f "$WSL_OUT/Makefile" ]]; then
  echo "SDK already extracted: $WSL_OUT"
  exit 0
fi

need() {
  command -v "$1" >/dev/null 2>&1 || {
    echo "Missing $1 — install: sudo apt install wget zstd" >&2
    exit 1
  }
}
need wget

if [[ ! -f "$ARCHIVE" ]]; then
  echo "Downloading $URL ..."
  wget -c -O "$ARCHIVE" "$URL"
fi

echo "Verifying SHA256 ..."
echo "${SHA256}  ${ARCHIVE}" | sha256sum -c -

echo "Extracting to WSL Linux filesystem (not /mnt/c — symlinks break on DrvFS) ..."
exec "$SCRIPT_DIR/extract-sdk-wsl.sh"

#!/usr/bin/env bash
set -euo pipefail
repo=$(cd "$(dirname "$0")/.." && pwd)
sdk=/home/adminavz/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64
build=/home/adminavz/.cache/ccli-tls-device-build
export STAGING_DIR="$sdk/staging_dir"
cmake -S "$repo/apps/ccli" -B "$build" \
  -DCCLI_MODBUS_LIBRARY="$sdk/staging_dir/target-aarch64_cortex-a53_musl/usr/lib/libmodbus.so" \
  -DCCLI_MODBUS_INCLUDE_DIR="$sdk/staging_dir/target-aarch64_cortex-a53_musl/usr/include"
cmake --build "$build" --target ccli -j6
mkdir -p "$repo/output/system-logger-install-20261029"
cp "$build/ccli" "$repo/output/system-logger-install-20261029/ccli"
"$sdk/staging_dir/toolchain-aarch64_cortex-a53_gcc-14.3.0_musl/bin/aarch64-openwrt-linux-musl-strip" "$repo/output/system-logger-install-20261029/ccli"
sha256sum "$repo/output/system-logger-install-20261029/ccli"

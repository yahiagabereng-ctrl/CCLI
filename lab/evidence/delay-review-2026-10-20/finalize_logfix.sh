#!/usr/bin/env bash
set -euo pipefail
out="$(cd "$(dirname "$0")" && pwd)"
strip=/home/adminavz/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64/staging_dir/toolchain-aarch64_cortex-a53_gcc-14.3.0_musl/bin/aarch64-openwrt-linux-musl-strip
"$strip" --strip-unneeded "$out/ccli-r62" "$out/mms-rbac-device-server-r62" "$out/event-store-rotation-test"
file "$out/ccli-r62"
sha256sum "$out/ccli-r62" "$out/mms-rbac-device-server-r62" "$out/event-store-rotation-test" | tee "$out/build-sha256.txt"

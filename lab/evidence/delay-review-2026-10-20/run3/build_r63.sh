#!/usr/bin/env bash
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
repo="$(cd "$here/../../../.." && pwd)"
sdk=/home/adminavz/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64
export STAGING_DIR="$sdk/staging_dir"
# Reuse the r62 SDK build, updating every current server source and the overlay
# generator. No client binary is built by these targets.
work=$(cat "$here/../build-workspace.txt")
test -d "$work/source/apps/ccli"
tar -C "$repo/apps/ccli" --exclude='./build*' --exclude='./config' --exclude='*.local.json' --exclude='__pycache__' -cf - . | tar -C "$work/source/apps/ccli" -xf -
cp "$repo/scripts/generate_mms_certificate_auth.py" "$work/source/scripts/"
cmake -S "$work/source/apps/ccli" -B "$work/target-build" -DCCLI_BUILD_TESTS=OFF -DCCLI_BUILD_MMS_DEVICE_TEST=ON
cmake --build "$work/target-build" --target ccli mms-rbac-device-server -j4
cp "$work/target-build/ccli" "$here/ccli-r63"
cp "$work/target-build/mms-rbac-device-server" "$here/mms-rbac-device-server-r63"
strip="$sdk/staging_dir/toolchain-aarch64_cortex-a53_gcc-14.3.0_musl/bin/aarch64-openwrt-linux-musl-strip"
"$strip" --strip-unneeded "$here/ccli-r63" "$here/mms-rbac-device-server-r63"
sha256sum "$here/ccli-r63" "$here/mms-rbac-device-server-r63" | tee "$here/r63-build-sha256.txt"
sha256sum "$work/source/scripts/generate_mms_certificate_auth.py" "$work/source/apps/ccli/VERSION" "$work/source/apps/ccli/adapters/iec61850_mms/mms_adapter.cpp" > "$here/r63-built-source-sha256.txt"

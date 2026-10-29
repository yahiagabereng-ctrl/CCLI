#!/usr/bin/env bash
set -euo pipefail
repo="$(cd "$(dirname "$0")/../../.." && pwd)"
out="$repo/lab/evidence/delay-review-2026-10-20"
sdk=/home/adminavz/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64
export STAGING_DIR="$sdk/staging_dir"
toolchain=/home/adminavz/ccli-tls-allowlist-20261019/tespro-toolchain.cmake
test -f "$toolchain"
cat "$toolchain"
work=$(mktemp -d /home/adminavz/ccli-logfix-20261020.XXXXXX)
printf '%s\n' "$work" > "$out/build-workspace.txt"
mkdir -p "$work/source/apps/ccli" "$work/source/scripts" "$work/source/lab/conformance"
# Build from current sources without copying credentials or earlier outputs.
tar -C "$repo/apps/ccli" --exclude='./build*' --exclude='./config' \
    --exclude='*.local.json' --exclude='__pycache__' -cf - . |
    tar -C "$work/source/apps/ccli" -xf -
cp "$repo/scripts/generate_mms_certificate_auth.py" "$work/source/scripts/"
cp "$repo/lab/conformance/CCI_RBAC_TLS_BASELINE.json" "$work/source/lab/conformance/"
mkdir -p "$work/source/apps/ccli/config"
cmake -S "$work/source/apps/ccli" -B "$work/target-build" \
    -DCMAKE_TOOLCHAIN_FILE="$toolchain" -DCMAKE_BUILD_TYPE=Release \
    -DCCLI_BUILD_TESTS=OFF -DCCLI_BUILD_MMS_DEVICE_TEST=ON \
    -DCCLI_MODBUS_INCLUDE_DIR="$sdk/staging_dir/target-aarch64_cortex-a53_musl/usr/include" \
    -DCCLI_MODBUS_LIBRARY="$sdk/staging_dir/target-aarch64_cortex-a53_musl/usr/lib/libmodbus.so"
cmake --build "$work/target-build" --target ccli mms-rbac-device-server -j4
cross="$sdk/staging_dir/toolchain-aarch64_cortex-a53_gcc-14.3.0_musl/bin/aarch64-openwrt-linux-musl-g++"
export STAGING_DIR="$sdk/staging_dir"
"$cross" -std=c++17 -O2 -static-libstdc++ -pthread \
    -I "$work/source/apps/ccli/core" \
    "$work/source/apps/ccli/core/event/event_store.cpp" \
    "$work/source/apps/ccli/core/event/event_ring.cpp" \
    "$work/source/apps/ccli/tests/unit/event_store_rotation_test.cpp" \
    -o "$work/event-store-rotation-test"
cp "$work/target-build/ccli" "$out/ccli-r62"
cp "$work/target-build/mms-rbac-device-server" "$out/mms-rbac-device-server-r62"
cp "$work/event-store-rotation-test" "$out/event-store-rotation-test"
bash "$out/finalize_logfix.sh"
cd "$work/source"
sha256sum apps/ccli/VERSION apps/ccli/CMakeLists.txt apps/ccli/core/event/event_store.cpp \
    apps/ccli/core/event/event_store.hpp apps/ccli/core/event/event_ring.cpp \
    apps/ccli/services/ccli_main.cpp apps/ccli/adapters/iec61850_mms/mms_adapter.cpp \
    apps/ccli/tests/unit/event_store_rotation_test.cpp > "$out/built-source-sha256.txt"

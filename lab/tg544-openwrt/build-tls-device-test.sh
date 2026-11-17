#!/usr/bin/env bash
set -euo pipefail
tls_repo=$(cd "$(dirname "$0")/../.." && pwd)
tls_sdk=${OPENWRT_SDK:-$HOME/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64}
tls_build=${CCLI_TLS_BUILD_DIR:-$HOME/.cache/ccli-tls-device-build}
tls_toolchain="$tls_sdk/staging_dir/toolchain-aarch64_cortex-a53_gcc-14.3.0_musl"
test -x "$tls_toolchain/bin/aarch64-openwrt-linux-musl-g++"
mkdir -p "$tls_build" "$tls_repo/output/iec62351-tls/device"
cat > "$tls_build/toolchain.cmake" <<EOF
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER "$tls_toolchain/bin/aarch64-openwrt-linux-musl-gcc")
set(CMAKE_CXX_COMPILER "$tls_toolchain/bin/aarch64-openwrt-linux-musl-g++")
set(CMAKE_FIND_ROOT_PATH "$tls_toolchain" "$tls_sdk/staging_dir/target-aarch64_cortex-a53_musl")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
EOF
export STAGING_DIR="$tls_sdk/staging_dir"
cmake -S "$tls_repo/apps/ccli" -B "$tls_build" -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="$tls_build/toolchain.cmake" -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DCMAKE_BUILD_TYPE=Release -DCCLI_BUILD_TESTS=OFF \
    -DCCLI_BUILD_TLS_DEVICE_TEST=ON -DCCLI_BUILD_MMS_DEVICE_TEST=ON \
    -DCCLI_WITH_LIBIEC61850=ON -DCCLI_WITH_LIB60870=ON -DCCLI_WITH_GOOSE=OFF \
    -DCCLI_FORCE_LIBMODBUS=OFF -DCCLI_FORCE_LIBGPIOD=OFF
cmake --build "$tls_build" --target iec62351-tls-device-probe mms-rbac-device-server ccli -j6
for tls_binary in iec62351-tls-device-probe mms-rbac-device-server ccli; do
    cp "$tls_build/$tls_binary" "$tls_repo/output/iec62351-tls/device/$tls_binary"
done
sha256sum "$tls_repo"/output/iec62351-tls/device/{iec62351-tls-device-probe,mms-rbac-device-server,ccli}

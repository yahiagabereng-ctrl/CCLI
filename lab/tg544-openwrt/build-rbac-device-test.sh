#!/usr/bin/env bash
set -euo pipefail
rbac_repo="$(cd "$(dirname "$0")/../.." && pwd)"
rbac_sdk=/home/adminavz/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64
rbac_build=/tmp/ccli-rbac-device-test-build
rbac_toolchain="$rbac_sdk/staging_dir/toolchain-aarch64_cortex-a53_gcc-14.3.0_musl"
test -x "$rbac_toolchain/bin/aarch64-openwrt-linux-musl-g++"
mkdir -p "$rbac_build"
cat > "$rbac_build/toolchain.cmake" <<EOF
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER "$rbac_toolchain/bin/aarch64-openwrt-linux-musl-gcc")
set(CMAKE_CXX_COMPILER "$rbac_toolchain/bin/aarch64-openwrt-linux-musl-g++")
set(CMAKE_FIND_ROOT_PATH "$rbac_toolchain" "$rbac_sdk/staging_dir/target-aarch64_cortex-a53_musl")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
EOF
export STAGING_DIR="$rbac_sdk/staging_dir"
cmake -S "$rbac_repo/apps/ccli" -B "$rbac_build" \
    -DCMAKE_TOOLCHAIN_FILE="$rbac_build/toolchain.cmake" -DCMAKE_BUILD_TYPE=Release \
    -DCCLI_BUILD_TESTS=OFF -DCCLI_BUILD_MMS_DEVICE_TEST=ON \
    -DCCLI_WITH_GOOSE=OFF -DCCLI_WITH_LIB60870=OFF \
    -DCCLI_FORCE_LIBMODBUS=OFF -DCCLI_FORCE_LIBGPIOD=OFF
cmake --build "$rbac_build" --target mms-rbac-device-server -j4
cp "$rbac_build/mms-rbac-device-server" "$rbac_repo/tmp/mms-rbac-device-server"
sha256sum "$rbac_repo/tmp/mms-rbac-device-server"

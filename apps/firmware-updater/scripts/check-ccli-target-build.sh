#!/usr/bin/env bash
# Compile-only readiness check. APK creation/signing uses build-ccli.sh.
set -euo pipefail
base=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
repo=$(cd "$base/../.." && pwd)
sdk_name=openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64
sdk="${OPENWRT_SDK_HOME:-$HOME}/$sdk_name"
toolchain="$sdk/staging_dir/toolchain-aarch64_cortex-a53_gcc-14.3.0_musl"
target="$sdk/staging_dir/target-aarch64_cortex-a53_musl/usr"
compiler="$toolchain/bin/aarch64-openwrt-linux-musl"
build=${1:-$repo/apps/ccli/build-e2e-arm64}
test -x "$compiler-gcc"
test -f "$target/lib/libmodbus.so"
export STAGING_DIR="$sdk/staging_dir"
cmake -S "$repo/apps/ccli" -B "$build" \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
    -DCMAKE_C_COMPILER="$compiler-gcc" -DCMAKE_CXX_COMPILER="$compiler-g++" \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DCMAKE_C_FLAGS="-I$target/include" -DCMAKE_CXX_FLAGS="-I$target/include" \
    -DCMAKE_EXE_LINKER_FLAGS="-L$target/lib -Wl,-rpath-link,$target/lib" \
    -DCMAKE_FIND_ROOT_PATH="$target;$toolchain" \
    -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
    -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
    -DBUILD_PLATFORM=tg500 -DCCLI_BUILD_TESTS=OFF -DCCLI_FORCE_LIBGPIOD=OFF \
    -DCCLI_WITH_LIBIEC61850=ON -DCCLI_WITH_LIB60870=ON -DCCLI_WITH_GOOSE=ON \
    -DCCLI_REQUIRE_PROTOCOL_DEPENDENCIES=ON \
    -DCCLI_FORCE_LIBMODBUS=ON -DCCLI_MODBUS_LIBRARY="$target/lib/libmodbus.so" \
    -DCCLI_MODBUS_INCLUDE_DIR="$target/include/modbus"
cmake --build "$build" --target ccli --parallel "${CCLI_BUILD_JOBS:-4}"
file "$build/ccli"
sha256sum "$build/ccli"

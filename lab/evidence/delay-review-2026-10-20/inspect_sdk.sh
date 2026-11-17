#!/usr/bin/env bash
set -eu
sdk=/home/adminavz/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64
ls "$sdk/staging_dir"
ls "$sdk/staging_dir/toolchain-aarch64_cortex-a53_gcc-14.3.0_musl/bin/"*g++
ls "$sdk/staging_dir/target-aarch64_cortex-a53_musl/usr/lib/"*modbus*
rg --files --hidden -g CMakeCache.txt "$sdk/build_dir" /home/adminavz 2>/dev/null | head -30 || true
for f in "$sdk"/build_dir/target-*/ccli-*/CMakeCache.txt; do
    test -f "$f" || continue
    echo "$f"
    rg 'CMAKE_CXX_COMPILER:|CMAKE_C_COMPILER:|CMAKE_SYSTEM|CMAKE_CXX_FLAGS:|CMAKE_C_FLAGS:|CMAKE_EXE_LINKER_FLAGS:|CMAKE_FIND_ROOT_PATH:|CCLI_MODBUS|CMAKE_TOOLCHAIN_FILE:' "$f"
done
rg 'CMAKE_CXX_COMPILER:|CMAKE_C_COMPILER:|CMAKE_CXX_FLAGS:|CMAKE_C_FLAGS:|CMAKE_EXE_LINKER_FLAGS:|CMAKE_FIND_ROOT_PATH:|CCLI_MODBUS|CMAKE_TOOLCHAIN_FILE:|CMAKE_HOME_DIRECTORY:' /home/adminavz/ccli-tls-allowlist-20261019/target-build/CMakeCache.txt
ls /home/adminavz/ccli-tls-allowlist-20261019

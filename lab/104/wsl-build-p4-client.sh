#!/usr/bin/env bash
# Build Phase 4 CS104 lab client (x86_64 WSL) using vendored lib60870.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
OUT="${SCRIPT_DIR}/p4_cs104_client"
MBED_SRC="${REPO}/apps/ccli/third_party/libiec61850/third_party/mbedtls/mbedtls-3.6.0"
MBED_DST="${REPO}/apps/ccli/third_party/lib60870/lib60870-C/dependencies/mbedtls-3.6"

if [[ -d "${MBED_SRC}" ]]; then
  mkdir -p "$(dirname "${MBED_DST}")"
  ln -sfn "${MBED_SRC}" "${MBED_DST}"
  echo "mbedtls linked for 104 TLS: ${MBED_DST}"
fi

rm -rf "${BUILD_DIR}"
cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_TESTS=OFF
cmake --build "${BUILD_DIR}" -j"$(nproc)"

cp -f "${BUILD_DIR}/p4_cs104_client" "${OUT}"
echo "Built: ${OUT}"
file "${OUT}"

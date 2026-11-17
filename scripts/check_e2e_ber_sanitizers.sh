#!/usr/bin/env bash
set -euo pipefail
repo=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$repo"
build=${1:-apps/ccli/build-e2e-ber-sanitizers}
cmake -S apps/ccli -B "$build" -DCCLI_BUILD_TESTS=ON \
    -DCCLI_WITH_LIBIEC61850=OFF -DCCLI_WITH_LIB60870=OFF -DCCLI_WITH_GOOSE=OFF \
    -DCCLI_REQUIRE_PROTOCOL_DEPENDENCIES=OFF -DCCLI_E2E_BER_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build "$build" --target e2e_ber_field_test --parallel "${CCLI_BUILD_JOBS:-4}"
ctest --test-dir "$build" -R '^e2e_ber_fields$' --output-on-failure

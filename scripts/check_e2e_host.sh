#!/usr/bin/env bash
set -euo pipefail
repo=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$repo"
build=${1:-apps/ccli/build-e2e-crypto}
cmake -S apps/ccli -B "$build" -DCCLI_BUILD_TESTS=ON \
    -DCCLI_WITH_LIBIEC61850=ON -DCCLI_WITH_LIB60870=OFF -DCCLI_WITH_GOOSE=OFF
cmake --build "$build" --target e2e_host_checks_build --parallel "${CCLI_BUILD_JOBS:-4}"
ctest --test-dir "$build" -R '^e2e_' --output-on-failure

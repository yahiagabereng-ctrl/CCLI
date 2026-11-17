#!/usr/bin/env bash
set -euo pipefail
repo="$(cd "$(dirname "$0")/../../.." && pwd)"
build="$repo/apps/ccli/build-event-log-fix"
cmake -S "$repo/apps/ccli" -B "$build" -DCCLI_BUILD_TESTS=ON \
    -DCCLI_WITH_LIBIEC61850=OFF -DCCLI_WITH_LIB60870=OFF -DCMAKE_BUILD_TYPE=Debug
cmake --build "$build" --target event_store_test event_store_rotation_test -j4
ctest --test-dir "$build" -R '^event_store_' --output-on-failure

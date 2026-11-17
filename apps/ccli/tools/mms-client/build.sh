#!/usr/bin/env bash
set -euo pipefail
HERE=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
REPO=$(cd "$HERE/../../../.." && pwd)
cmake -S "$REPO/apps/ccli" -B "$REPO/apps/ccli/build-mms-client" \
    -DBUILD_PLATFORM=tg500 -DCCLI_BUILD_TESTS=OFF -DCCLI_WITH_LIB60870=OFF \
    -DCCLI_WITH_GOOSE=OFF -DCCLI_BUILD_MMS_CLIENT=ON -DCCLI_MMS_TSP_CLIENT_PROFILE=ON
cmake --build "$REPO/apps/ccli/build-mms-client" --target ccli-mms-client -j 4

#!/usr/bin/env bash
# One local commissioning build. Unsigned APK explicitly authorized by owner.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
REPO="$(cd "$HERE/../.." && pwd)"
SDK=/home/adminavz/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64
LINK="$SDK/.ccli-repo"
test "$(readlink -f "$LINK")" = "$REPO"
cd "$SDK"
make package/feeds/ccli/ccli/clean V=s CCLI_SRC_DIR="$LINK/apps/ccli" CCLI_REPO_DIR="$LINK"
make package/feeds/ccli/ccli/compile V=s -j4 \
    CCLI_SRC_DIR="$LINK/apps/ccli" CCLI_REPO_DIR="$LINK"
APK="$SDK/bin/packages/aarch64_cortex-a53/ccli/ccli-0.1.0-r57.apk"
test -f "$APK"
cp "$APK" "$HERE/ccli-0.1.0-r57.apk"
sha256sum "$HERE/ccli-0.1.0-r57.apk"

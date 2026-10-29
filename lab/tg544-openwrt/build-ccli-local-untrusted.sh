#!/usr/bin/env bash
# Explicit one-session lab exception; the normal build still requires signing.
set -euo pipefail
test "${1:-}" = --untrusted-lab-build || { echo 'Explicit --untrusted-lab-build required' >&2; exit 1; }
HERE="$(cd "$(dirname "$0")" && pwd)"
REPO="$(cd "$HERE/../.." && pwd)"
SDK=${OPENWRT_SDK_HOME:-/home/adminavz/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64}
LINK="$SDK/.ccli-repo"
test "$(readlink -f "$LINK")" = "$REPO"
version="$(sed -n '1p' "$REPO/apps/ccli/VERSION" | tr -d '\r')"
release="$(sed -n '2p' "$REPO/apps/ccli/VERSION" | tr -d '\r')"
cd "$SDK"
if [[ "${2:-}" == --clean ]]; then
    make package/feeds/ccli/ccli/clean V=s CCLI_SRC_DIR="$LINK/apps/ccli" CCLI_REPO_DIR="$LINK"
elif [[ -n "${2:-}" ]]; then
    echo 'Unknown option (optional second argument: --clean)' >&2
    exit 1
fi
make package/feeds/ccli/ccli/compile V=s -j4 CCLI_SRC_DIR="$LINK/apps/ccli" CCLI_REPO_DIR="$LINK"
APK="$SDK/bin/packages/aarch64_cortex-a53/ccli/ccli-${version}-r${release}.apk"
test -f "$APK"
cp "$APK" "$HERE/$(basename "$APK")"
sha256sum "$HERE/$(basename "$APK")"

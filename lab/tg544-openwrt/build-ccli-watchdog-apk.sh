#!/usr/bin/env bash
# Build and manufacturer-sign the independent CCLI watchdog APK for TesPro.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SCRIPT_DIR/../.." && pwd)"
SDK_NAME="openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64"
SDK="${OPENWRT_SDK_HOME:-$HOME/$SDK_NAME}"
SIGN_KEY="${CCLI_APK_SIGN_KEY:-}"
SIGN_PUB="${CCLI_APK_SIGN_PUB:-${SIGN_KEY%.pem}.pub.pem}"

if [[ -z "$SIGN_KEY" || ! -f "$SIGN_KEY" || ! -f "$SIGN_PUB" ]]; then
  echo "Missing CCLI_APK_SIGN_KEY / CCLI_APK_SIGN_PUB; refusing unsigned watchdog APK." >&2
  exit 1
fi
[[ -d "$SDK" ]] || { echo "SDK not found: $SDK" >&2; exit 1; }

cd "$SDK"
REPO_LINK="$SDK/.ccli-repo"
ln -sfn "$REPO" "$REPO_LINK"
mkdir -p staging_dir/host tmp/info
touch staging_dir/host/.prereq-build
[[ -f feeds.conf ]] || cp feeds.conf.default feeds.conf
sed -i '\|^src-link ccli[[:space:]]|d' feeds.conf
echo "src-link ccli $REPO_LINK/package" >> feeds.conf
./scripts/feeds update ccli
./scripts/feeds install -p ccli ccli-watchdog

make package/feeds/ccli/ccli-watchdog/clean V=s 2>/dev/null || true
make package/feeds/ccli/ccli-watchdog/compile V=s \
  CCLI_SRC_DIR="$REPO_LINK/apps/ccli" \
  BUILD_KEY_APK_SEC="$SIGN_KEY" BUILD_KEY_APK_PUB="$SIGN_PUB"

APK=$(ls -1 bin/packages/*/ccli/ccli-watchdog-[0-9]*.apk | head -1)
WORK="$SDK/tmp/$(basename "$APK").signed"
OUT="$SCRIPT_DIR/$(basename "$APK")"
cp -f "$APK" "$WORK"
"$SDK/staging_dir/host/bin/apk" adbsign --allow-untrusted --reset-signatures \
  --sign-key "$SIGN_KEY" "$WORK"
cp -f "$WORK" "$OUT"
echo "SIGNED $OUT"
sha256sum "$OUT"

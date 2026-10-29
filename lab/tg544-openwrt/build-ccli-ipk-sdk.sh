#!/usr/bin/env bash
# Build ccli.ipk for TG544 using OpenWrt 25.12.5 mediatek/filogic SDK.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SCRIPT_DIR/../.." && pwd)"

RELEASE="25.12.5"
SDK_NAME="openwrt-sdk-${RELEASE}-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64"
SDK="${OPENWRT_SDK_HOME:-$HOME}/$SDK_NAME"
SIGN_KEY="${CCLI_APK_SIGN_KEY:-}"
SIGN_PUB="${CCLI_APK_SIGN_PUB:-${SIGN_KEY%.pem}.pub.pem}"

if [[ -z "$SIGN_KEY" || ! -f "$SIGN_KEY" || ! -f "$SIGN_PUB" ]]; then
  echo "Missing CCLI_APK_SIGN_KEY. Refusing to produce an unsigned APK." >&2
  echo "Set CCLI_APK_SIGN_KEY and CCLI_APK_SIGN_PUB to the protected APK v3 key pair." >&2
  exit 1
fi

if [[ ! -d "$SDK" ]]; then
  echo "SDK not found. Run: $SCRIPT_DIR/download-sdk.sh" >&2
  exit 1
fi

cd "$SDK"

# OpenWrt feed paths cannot contain spaces.  Point the SDK at a stable,
# space-free symlink when the Windows checkout lives in a path such as
# "CCLI Alireza".
REPO_LINK="$SDK/.ccli-repo"
ln -sfn "$REPO" "$REPO_LINK"

# These variables also become GNU make target names. Shell quoting alone does
# not protect spaces there; use SDK-local symlinks to the canonical local keys.
# Keep the original key files in place and never generate replacement keys.
SIGN_KEY_MAKE="$SDK/.ccli-apk-signing-key.pem"
SIGN_PUB_MAKE="$SDK/.ccli-apk-signing-key.pub.pem"
ln -sfn "$SIGN_KEY" "$SIGN_KEY_MAKE"
ln -sfn "$SIGN_PUB" "$SIGN_PUB_MAKE"
KEY_ARGS=("BUILD_KEY_APK_SEC=$SIGN_KEY_MAKE" "BUILD_KEY_APK_PUB=$SIGN_PUB_MAKE")

mkdir -p staging_dir/host tmp/info
touch staging_dir/host/.prereq-build

if [[ ! -f feeds.conf ]] || ! grep -q 'src-git packages' feeds.conf 2>/dev/null; then
  cp -f feeds.conf.default feeds.conf
fi
sed -i '\|^src-link ccli[[:space:]]|d' feeds.conf
echo "src-link ccli ${REPO_LINK}/package" >> feeds.conf

./scripts/feeds update -a
./scripts/feeds install -p ccli ccli
./scripts/feeds install -p packages libmodbus

make defconfig
grep -q 'CONFIG_PACKAGE_ccli=y' .config || echo 'CONFIG_PACKAGE_ccli=y' >> .config
grep -q 'CONFIG_PACKAGE_libmodbus=y' .config || echo 'CONFIG_PACKAGE_libmodbus=y' >> .config
make oldconfig

echo "=== Building libmodbus ==="
make package/feeds/packages/libmodbus/compile V=s

echo "=== Building ccli (platform_tg500) ==="
make package/feeds/ccli/ccli/clean V=s CCLI_SRC_DIR="$REPO_LINK/apps/ccli" CCLI_REPO_DIR="$REPO_LINK" 2>/dev/null || true
make package/feeds/ccli/ccli/compile V=s CCLI_SRC_DIR="$REPO_LINK/apps/ccli" CCLI_REPO_DIR="$REPO_LINK" "${KEY_ARGS[@]}"
make package/index "${KEY_ARGS[@]}"

OUT_DIR="$SCRIPT_DIR"
mkdir -p "$OUT_DIR"
APK=$(ls -1 bin/packages/*/ccli/ccli-[0-9]*.apk | head -1)
SIGNED_APK="$OUT_DIR/$(basename "$APK")"
SIGNED_WORK="$SDK/tmp/$(basename "$APK").signed"
cp -f "$APK" "$SIGNED_WORK"
"$SDK/staging_dir/host/bin/apk" adbsign --allow-untrusted --reset-signatures --sign-key "$SIGN_KEY" "$SIGNED_WORK"
cp -f "$SIGNED_WORK" "$SIGNED_APK"
echo "SIGNED $SIGNED_APK"

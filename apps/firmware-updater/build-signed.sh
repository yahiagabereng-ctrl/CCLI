#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SCRIPT_DIR/../.." && pwd)"
SDK_NAME="openwrt-sdk-25.12.2-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64"
SDK="${OPENWRT_SDK:-$HOME/$SDK_NAME}"
SIGN_KEY="${CCLI_APK_SIGN_KEY:-$SCRIPT_DIR/keys/manufacturer/ccli-release.pem}"
SIGN_PUB="${CCLI_APK_SIGN_PUB:-$SCRIPT_DIR/keys/manufacturer/ccli-release.pub.pem}"

[[ -d "$SDK" ]] || { echo "OpenWrt SDK not found: $SDK" >&2; exit 1; }
[[ -f "$SIGN_KEY" && -f "$SIGN_PUB" ]] || {
  echo "Protected CCLI APK signing key pair is missing." >&2
  exit 1
}

private_fingerprint=$(openssl ec -in "$SIGN_KEY" -pubout -outform DER 2>/dev/null | sha256sum | cut -d' ' -f1)
public_fingerprint=$(openssl pkey -pubin -in "$SIGN_PUB" -outform DER 2>/dev/null | sha256sum | cut -d' ' -f1)
[[ "$private_fingerprint" == "$public_fingerprint" ]] || {
  echo "APK signing private/public keys do not match." >&2
  exit 1
}

ln -sfn "$SCRIPT_DIR" "$SDK/package/firmware-updater"
grep -q '^CONFIG_PACKAGE_firmware-updater=m$' "$SDK/.config" 2>/dev/null || \
  echo 'CONFIG_PACKAGE_firmware-updater=m' >> "$SDK/.config"

make -C "$SDK" package/firmware-updater/clean >/dev/null
make -C "$SDK" package/firmware-updater/compile V=s -j2

unsigned_apk=$(find "$SDK/bin/packages" -type f -name 'firmware-updater-1.1.0-r8.apk' | head -1)
[[ -n "$unsigned_apk" ]] || { echo "Built APK not found." >&2; exit 1; }

work="$SDK/tmp/firmware-updater-1.1.0-r8.signed.apk"
trust=$(mktemp -d)
trap 'rm -rf "$trust"' EXIT
cp "$unsigned_apk" "$work"
"$SCRIPT_DIR/tools/bin/apk" adbsign --allow-untrusted --reset-signatures \
  --sign-key "$SIGN_KEY" "$work"
cp "$SIGN_PUB" "$trust/ccli-release.pem"
bash "$SCRIPT_DIR/scripts/verify-apk.sh" "$work"

mkdir -p "$SCRIPT_DIR/dist"
cp "$work" "$SCRIPT_DIR/dist/firmware-updater-1.1.0-r8.apk"
sha256sum "$SCRIPT_DIR/dist/firmware-updater-1.1.0-r8.apk"

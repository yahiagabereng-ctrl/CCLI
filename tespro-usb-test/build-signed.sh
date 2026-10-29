#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SDK="${OPENWRT_SDK:-/home/adminavz/openwrt-sdk-25.12.2-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64}"
SIGN_KEY="${CCLI_APK_SIGN_KEY:-/mnt/c/Users/Administrator/.ccli-signing/ccli-release.pem}"
SIGN_PUB="${CCLI_APK_SIGN_PUB:-/mnt/c/Users/Administrator/.ccli-signing/ccli-release.pub.pem}"
PACKAGE="usb-test-1.0.2-r1.apk"

[[ -d "$SDK" && -f "$SIGN_KEY" && -f "$SIGN_PUB" ]] || {
  echo "SDK or release signing key pair is missing." >&2
  exit 1
}

private_fingerprint=$(openssl ec -in "$SIGN_KEY" -pubout -outform DER 2>/dev/null | sha256sum | cut -d' ' -f1)
public_fingerprint=$(openssl pkey -pubin -in "$SIGN_PUB" -outform DER 2>/dev/null | sha256sum | cut -d' ' -f1)
[[ "$private_fingerprint" == "$public_fingerprint" ]] || {
  echo "Release private and public keys do not match." >&2
  exit 1
}

ln -sfn "$SCRIPT_DIR" "$SDK/package/usb-test"
grep -q '^CONFIG_PACKAGE_usb-test=m$' "$SDK/.config" 2>/dev/null ||
  echo 'CONFIG_PACKAGE_usb-test=m' >> "$SDK/.config"
make -C "$SDK" package/usb-test/clean >/dev/null
make -C "$SDK" package/usb-test/compile -j2 >/dev/null

unsigned=$(find "$SDK/bin/packages" -type f -name "$PACKAGE" -print -quit)
[[ -n "$unsigned" ]] || { echo "Built package not found." >&2; exit 1; }
work="$SDK/tmp/$PACKAGE"
trust=$(mktemp -d)
trap 'rm -rf "$trust"' EXIT
cp "$unsigned" "$work"
"$SDK/staging_dir/host/bin/apk" adbsign --allow-untrusted --reset-signatures \
  --sign-key "$SIGN_KEY" "$work"
cp "$SIGN_PUB" "$trust/ccli-release.pem"
"$SDK/staging_dir/host/bin/apk" --keys-dir "$trust" verify "$work"

mkdir -p "$SCRIPT_DIR/dist"
cp "$work" "$SCRIPT_DIR/dist/$PACKAGE"
sha256sum "$SCRIPT_DIR/dist/$PACKAGE"

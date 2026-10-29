#!/usr/bin/env bash
set -euo pipefail
repo=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
exec bash "$repo/apps/firmware-updater/scripts/verify-apk.sh" "$@"
sdk=${OPENWRT_SDK:-/home/adminavz/openwrt-sdk-25.12.2-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64}
public_key=${CCLI_APK_SIGN_PUB:-/mnt/c/Users/Administrator/.ccli-signing/ccli-release.pub.pem}
trust=$(mktemp -d)
trap 'rm -f "$trust/release.pem"; rmdir "$trust"' EXIT
cp "$public_key" "$trust/release.pem"
"$sdk/staging_dir/host/bin/apk" --keys-dir "$trust" verify "$1"

#!/usr/bin/env bash
set -euo pipefail
base=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
repo=$(cd "$base/../.." && pwd)
export CCLI_APK_SIGN_KEY=${CCLI_APK_SIGN_KEY:-$base/keys/manufacturer/ccli-release.pem}
export CCLI_APK_SIGN_PUB=${CCLI_APK_SIGN_PUB:-$base/keys/manufacturer/ccli-release.pub.pem}
bash "$repo/lab/tg544-openwrt/build-ccli-ipk-sdk.sh"
mkdir -p "$base/artifacts"
for apk in "$repo"/lab/tg544-openwrt/ccli-[0-9]*.apk; do
    bash "$base/scripts/verify-apk.sh" "$apk"
    cp "$apk" "$base/artifacts/"
done

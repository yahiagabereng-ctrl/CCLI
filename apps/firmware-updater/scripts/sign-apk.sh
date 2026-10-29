#!/usr/bin/env bash
# Sign an existing APK into artifacts without modifying the input file.
set -euo pipefail
[[ $# == 1 ]] || { echo 'Usage: sign-apk.sh BUILT_APK' >&2; exit 64; }
base=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
apk_tool="$base/tools/bin/apk"
key=${CCLI_APK_SIGN_KEY:-$base/keys/manufacturer/ccli-release.pem}
name=$(basename "$1")
[[ $name =~ ^[A-Za-z0-9_.:-]+\.apk$ ]] || exit 65
mkdir -p "$base/artifacts"
destination="$base/artifacts/$name"
[[ ! -e "$destination" ]] || { echo "Output already exists: $destination" >&2; exit 1; }
work=$(mktemp -d)
trap 'rm -f "$work/package.apk"; rmdir "$work"' EXIT
cp "$1" "$work/package.apk"
"$apk_tool" adbsign --allow-untrusted --reset-signatures --sign-key "$key" "$work/package.apk"
bash "$base/scripts/verify-apk.sh" "$work/package.apk"
cp "$work/package.apk" "$destination"
sha256sum "$destination"

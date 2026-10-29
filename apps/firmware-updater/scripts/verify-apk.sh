#!/usr/bin/env bash
set -euo pipefail
base=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
apk_tool="$base/tools/bin/apk"
[[ -x "$apk_tool" ]] || { echo "Missing local APK tool: $apk_tool" >&2; exit 1; }
public_key=${CCLI_APK_SIGN_PUB:-$base/keys/manufacturer/ccli-release.pub.pem}
trust=$(mktemp -d)
trap 'rm -f "$trust/release.pem"; rmdir "$trust"' EXIT
cp "$public_key" "$trust/release.pem"
"$apk_tool" --keys-dir "$trust" --no-network verify "$@"

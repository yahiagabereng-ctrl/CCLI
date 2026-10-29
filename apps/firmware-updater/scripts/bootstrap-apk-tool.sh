#!/usr/bin/env bash
# Copy OpenWrt's host APK executable into this toolkit for local use.
set -euo pipefail
base=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
sdk=${OPENWRT_SDK:-${HOME}/openwrt-sdk-25.12.2-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64}
source_apk_tool="$sdk/staging_dir/host/bin/.apk.bin"
destination="$base/tools/bin/apk"
[[ -x "$source_apk_tool" ]] || {
    echo "OpenWrt APK binary not found at $source_apk_tool" >&2
    echo 'Set OPENWRT_SDK to an extracted OpenWrt 25.12.x SDK containing staging_dir/host/bin/.apk.bin.' >&2
    exit 1
}
[[ ! -e "$destination" ]] || {
    echo "Local APK tool already exists: $destination (remove it explicitly to replace it)." >&2
    exit 1
}
mkdir -p "$base/tools/bin"
install -m 0755 "$source_apk_tool" "$destination"
"$destination" --version
sha256sum "$destination"

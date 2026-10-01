#!/bin/bash
# Download OpenWrt reference material for CCLI corpus ingest.
# Strategy: oldwiki raw exports + GitHub upstream (openwrt.org blocks bot curl).
set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/knowledge-base/08-engineering/openwrt-sources"
mkdir -p "$OUT"
cd "$OUT"

UA="Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36"

fetch() {
  local url="$1"
  local out="$2"
  echo "FETCH $out"
  if curl -fsSL -A "$UA" -L "$url" -o "$out"; then
    local sz
    sz=$(wc -c < "$out")
    if [ "$sz" -lt 5000 ] && grep -q "Testing to determine if you are a bot" "$out" 2>/dev/null; then
      echo "  FAIL bot-challenge ($sz bytes)"
      rm -f "$out"
      return 1
    fi
    echo "  OK $sz bytes"
  else
    echo "  FAIL $url"
    return 1
  fi
}

# Archived OpenWrt wiki — raw text (stable; maps to 24.10 concepts)
fetch "https://oldwiki.archive.openwrt.org/_export/raw/doc/uci" "raw_uci.txt"
fetch "https://oldwiki.archive.openwrt.org/_export/raw/doc/devel/config-scripting" "raw_config-scripting.txt"
fetch "https://oldwiki.archive.openwrt.org/_export/raw/inbox/procd-init-scripts" "raw_procd-init-scripts.txt"
fetch "https://oldwiki.archive.openwrt.org/_export/raw/doc/howto/generic.sysupgrade" "raw_generic_sysupgrade.txt"
fetch "https://oldwiki.archive.openwrt.org/_export/raw/doc/devel/packages" "raw_packages.txt"
fetch "https://oldwiki.archive.openwrt.org/_export/raw/doc/howto/build" "raw_build-usage.txt"
fetch "https://oldwiki.archive.openwrt.org/_export/raw/doc/howto/buildroot.exigence" "raw_buildroot-exigence.txt"
fetch "https://oldwiki.archive.openwrt.org/_export/raw/doc/devel/feeds" "raw_feeds.txt"
fetch "https://oldwiki.archive.openwrt.org/_export/raw/doc/howto/obtain.firmware.sdk" "raw_obtain-firmware-sdk.txt"
fetch "https://oldwiki.archive.openwrt.org/_export/raw/toh/raspberry_pi_foundation/raspberry_pi" "raw_raspberry_pi.txt"
fetch "https://oldwiki.archive.openwrt.org/_export/raw/doc/devel/dependencies" "raw_dependencies.txt"

# Upstream OpenWrt / libgpiod (GitHub raw — authoritative for 24.10 feed)
fetch "https://raw.githubusercontent.com/openwrt/openwrt/master/package/system/procd/files/procd.sh" "github_openwrt_procd.sh"
fetch "https://raw.githubusercontent.com/openwrt/openwrt/master/package/base-files/files/etc/rc.common" "github_openwrt_rc.common"
fetch "https://raw.githubusercontent.com/openwrt/openwrt/master/package/base-files/files/lib/functions.sh" "github_openwrt_functions.sh"
fetch "https://raw.githubusercontent.com/openwrt/packages/master/libs/libgpiod/Makefile" "github_openwrt-packages_libgpiod_Makefile"
fetch "https://raw.githubusercontent.com/openwrt/openwrt/master/package/system/opkg/files/opkg.conf" "github_openwrt_opkg.conf"

# libgpiod example (optional — may 404 on some tags)
fetch "https://raw.githubusercontent.com/brgl/libgpiod/v2.1.3/examples/toggle_line_value.c" "github_libgpiod_toggle_line_value.c" || true

echo "DONE"

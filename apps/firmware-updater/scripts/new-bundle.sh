#!/usr/bin/env bash
# Usage: new-bundle.sh SIGNED_APK [DEVICE_ID] [VALID_MINUTES]
set -euo pipefail
base=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
[[ $# -ge 1 && $# -le 3 ]] || { echo 'Usage: new-bundle.sh SIGNED_APK [DEVICE_ID] [VALID_MINUTES]' >&2; exit 64; }
exec bash "$base/scripts/new-firmware-update-bundle.sh" "$1" \
  "$base/keys/operator/operator-certificate.pem" \
  "$base/keys/operator/operator-private-key.pem" \
  "${2:-CCI-0008CCFD041F}" "$base/bundles" "${3:-60}"

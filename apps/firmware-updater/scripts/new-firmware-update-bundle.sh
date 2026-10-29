#!/usr/bin/env bash
# Create a signed, one-time TesPro USB update bundle from WSL.
# The encrypted operator private key is used locally and is never copied out.
set -euo pipefail

if [[ $# -ne 6 ]]; then
  echo "Usage: $0 <signed-apk> <operator-cert.pem> <operator-private-key.pem> <device-id> <output-dir> <valid-minutes>" >&2
  exit 64
fi

package=$(realpath "$1")
script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
bash "$script_dir/verify-apk.sh" "$package"
certificate=$(realpath "$2")
private_key=$(realpath "$3")
device_id=$4
output_dir=$(realpath -m "$5")
valid_minutes=$6

[[ -f "$package" && -f "$certificate" && -f "$private_key" ]] || {
  echo "The APK, certificate, and private-key paths must be files." >&2
  exit 66
}
[[ $(basename "$package") =~ ^[A-Za-z0-9_.:-]+\.apk$ ]] || {
  echo "Unsafe APK filename." >&2
  exit 65
}
[[ "$device_id" =~ ^[A-Za-z0-9_.:-]{3,96}$ ]] || {
  echo "Unsafe device ID." >&2
  exit 65
}
[[ "$valid_minutes" =~ ^[0-9]+$ && "$valid_minutes" -ge 1 && "$valid_minutes" -le 1440 ]] || {
  echo "Validity must be between 1 and 1440 minutes." >&2
  exit 65
}

constraints=$(openssl x509 -in "$certificate" -noout -ext basicConstraints)
usage=$(openssl x509 -in "$certificate" -noout -ext keyUsage)
subject=$(openssl x509 -in "$certificate" -noout -subject -nameopt sep_multiline,sname)
grep -q 'CA:FALSE' <<<"$constraints" && grep -q 'Digital Signature' <<<"$usage" &&
  grep -q 'OU=Firmware Update Operators' <<<"$subject" || {
  echo "Operator certificate must be a non-CA digital-signature certificate with OU=Firmware Update Operators." >&2
  exit 65
}
openssl x509 -in "$certificate" -checkend "$((valid_minutes * 60))" -noout >/dev/null || {
  echo "Operator certificate expires before the requested bundle validity." >&2
  exit 65
}

transaction_id=$(cat /proc/sys/kernel/random/uuid)
bundle="$output_dir/update-$transaction_id"
mkdir -p "$bundle"
completed=0
cleanup() {
  if [[ $completed -ne 1 ]]; then rm -rf -- "$bundle"; fi
}
trap cleanup EXIT

package_name=$(basename "$package")
cp -- "$package" "$bundle/$package_name"
cp -- "$certificate" "$bundle/operator-certificate.pem"
sha256=$(sha256sum "$package" | awk '{print $1}')
now=$(date -u +%s)
expires=$((now + valid_minutes * 60))

PACKAGE_NAME="$package_name" PACKAGE_SHA256="$sha256" TRANSACTION_ID="$transaction_id" \
DEVICE_ID="$device_id" NOT_BEFORE="$now" EXPIRES="$expires" \
  python3 - <<'PY' > "$bundle/update-manifest.json"
import json
import os

print(json.dumps({
    "schema_version": 1,
    "transaction_id": os.environ["TRANSACTION_ID"],
    "device_id": os.environ["DEVICE_ID"],
    "operator_role": "firmware-update",
    "not_before_epoch": int(os.environ["NOT_BEFORE"]),
    "expires_epoch": int(os.environ["EXPIRES"]),
    "packages": [{"file": os.environ["PACKAGE_NAME"], "sha256": os.environ["PACKAGE_SHA256"]}],
}, indent=2))
PY

echo "Enter the operator private-key passphrase at the OpenSSL prompt."
openssl dgst -sha256 -sign "$private_key" -out "$bundle/update-manifest.json.sig" "$bundle/update-manifest.json"
openssl x509 -in "$certificate" -pubkey -noout > "$bundle/.operator-public-key.pem"
openssl dgst -sha256 -verify "$bundle/.operator-public-key.pem" \
  -signature "$bundle/update-manifest.json.sig" "$bundle/update-manifest.json"
rm -f -- "$bundle/.operator-public-key.pem"

completed=1
echo "USB update bundle created: $bundle"
echo "Transaction: $transaction_id"
echo "Device:      $device_id"
echo "Expires UTC: $(date -u -d "@$expires" '+%Y-%m-%d %H:%M:%SZ')"

#!/usr/bin/env bash
# Read-only preflight. Does not unlock the private key or create a bundle.
set -euo pipefail
base=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
[[ $# == 1 ]] || { echo 'Usage: check-bundle-inputs.sh SIGNED_APK' >&2; exit 64; }
for tool in openssl python3 realpath sha256sum date; do command -v "$tool" >/dev/null; done
for file in keys/operator/operator-private-key.pem keys/operator/operator-certificate.pem keys/manufacturer/ccli-release.pub.pem; do
    [[ -s "$base/$file" ]] || { echo "Missing $file" >&2; exit 1; }
done
bash "$base/scripts/verify-apk.sh" "$1"
openssl x509 -in "$base/keys/operator/operator-certificate.pem" -noout -checkend 3600
openssl x509 -in "$base/keys/operator/operator-certificate.pem" -noout -ext basicConstraints | grep -q 'CA:FALSE'
openssl x509 -in "$base/keys/operator/operator-certificate.pem" -noout -ext keyUsage | grep -q 'Digital Signature'
openssl x509 -in "$base/keys/operator/operator-certificate.pem" -noout -subject -nameopt sep_multiline,sname | grep -q 'OU=Firmware Update Operators'
echo 'PASS: local bundle inputs and runtime available; signing still requires the operator key passphrase.'

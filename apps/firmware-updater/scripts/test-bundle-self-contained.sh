#!/usr/bin/env bash
# Integration test for local APK verification and signed-bundle creation.
set -euo pipefail
base=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
source_apk=${1:-$base/artifacts/ccli-0.1.0-r60.apk}
manufacturer_key="$base/keys/manufacturer/ccli-release.pem"
[[ -s "$source_apk" && -s "$manufacturer_key" ]] || {
    echo 'Missing source APK or local manufacturer private key.' >&2
    exit 1
}
work=$(mktemp -d)
trap 'rm -rf -- "$work"' EXIT
mkdir -p "$work/output"

# Re-sign a disposable copy with the local manufacturer key so the bundle
# verification path is exercised independently of the source APK's signature.
cp "$source_apk" "$work/test-ccli.apk"
"$base/tools/bin/apk" adbsign --allow-untrusted --reset-signatures \
    --sign-key "$manufacturer_key" "$work/test-ccli.apk"
bash "$base/scripts/verify-apk.sh" "$work/test-ccli.apk"

openssl req -x509 -newkey rsa:2048 -nodes -days 2 \
    -subj '/C=IT/O=Toolkit Integration Test/OU=Firmware Update Operators/CN=Temporary Test Operator' \
    -keyout "$work/operator-key.pem" -out "$work/operator-cert.pem" \
    -addext 'basicConstraints=critical,CA:FALSE' \
    -addext 'keyUsage=critical,digitalSignature' >/dev/null 2>&1

bash "$base/scripts/new-firmware-update-bundle.sh" \
    "$work/test-ccli.apk" "$work/operator-cert.pem" "$work/operator-key.pem" \
    CCI-0008CCFD041F "$work/output" 5

bundle=$(find "$work/output" -mindepth 1 -maxdepth 1 -type d -name 'update-*' -print -quit)
[[ -n "$bundle" ]]
[[ $(find "$bundle" -maxdepth 1 -type f | wc -l) -eq 4 ]]
[[ ! -e "$bundle/operator-key.pem" && ! -e "$bundle/ccli-release.pem" ]]
python3 - "$bundle/update-manifest.json" "$work/test-ccli.apk" <<'PY'
import hashlib, json, pathlib, sys
manifest = json.loads(pathlib.Path(sys.argv[1]).read_text())
apk = pathlib.Path(sys.argv[2])
assert len(manifest["packages"]) == 1
assert manifest["packages"][0]["file"] == apk.name
assert manifest["packages"][0]["sha256"] == hashlib.sha256(apk.read_bytes()).hexdigest()
assert manifest["device_id"] == "CCI-0008CCFD041F"
PY
echo 'PASS: self-contained bundle integration test; private keys absent from bundle.'

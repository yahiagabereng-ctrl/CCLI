#!/usr/bin/env bash
# Deploy CCLI Phase 1 to TG544 (USB Modbus /dev/ttyUSB0 + ubus DIO)
set -euo pipefail

HOST="${CCLI_TG544_HOST:-192.168.1.130}"
USER="${CCLI_TG544_USER:-root}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SCRIPT_DIR/../.." && pwd)"
APK="$SCRIPT_DIR/ccli-0.1.0-r1.apk"
YAML="$REPO/apps/ccli/config/lab_tr400.yaml"

[[ -f "$APK" ]] || { echo "Run ./build-ccli-ipk-sdk.sh first" >&2; exit 1; }

echo "=== Deploy to $USER@$HOST ==="
scp "$APK" "$USER@$HOST:/tmp/ccli.apk"
ssh "$USER@$HOST" "mkdir -p /etc/ccli"
scp "$YAML" "$USER@$HOST:/etc/ccli/lab.yaml"

ssh "$USER@$HOST" <<'REMOTE'
set -e
apk add --allow-untrusted /tmp/ccli.apk 2>/dev/null || true
echo "=== USB ==="
ls -l /dev/ttyUSB* 2>/dev/null || echo "NO ttyUSB — plug dongle and check dmesg"
dmesg | tail -10
echo "=== DIO ==="
uci set didoservice_v2.channel_2.mode=di 2>/dev/null || true
uci commit didoservice_v2 2>/dev/null || true
/etc/init.d/didoservice_v2 restart 2>/dev/null || true
ubus call dido_v2 set_mode '{"channel":2,"mode":"DI"}' 2>/dev/null || true
ubus call dido_v2 gd32.set_mode '{"channel":2,"mode":"DI"}' 2>/dev/null || true
/etc/init.d/ccli stop 2>/dev/null || true
killall ccli 2>/dev/null || true
echo "=== ccli (120s foreground log) ==="
timeout 120 /usr/sbin/ccli --config /etc/ccli/lab.yaml 2>&1 | tee /tmp/ccli-phase1.log || true
REMOTE

echo "Log on device: /tmp/ccli-phase1.log"

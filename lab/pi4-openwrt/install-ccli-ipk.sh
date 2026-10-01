#!/bin/bash
# Run from WSL after build-ccli-ipk.sh, or with IPK path as $1
set -euo pipefail

PI="${PI_HOST:-192.168.1.1}"
IPK="${1:-$(ls -1 /mnt/c/Yahia/projects/CCLI/lab/pi4-openwrt/ccli_*.ipk 2>/dev/null | head -1)}"

if [ -z "$IPK" ] || [ ! -f "$IPK" ]; then
    echo "No .ipk found. Run build-ccli-ipk.sh first." >&2
    exit 1
fi

echo "=== Installing $(basename "$IPK") on root@${PI} ==="
scp -o StrictHostKeyChecking=accept-new "$IPK" "root@${PI}:/tmp/"
ssh -o StrictHostKeyChecking=accept-new "root@${PI}" \
    "opkg update && opkg install /tmp/$(basename "$IPK") && /etc/init.d/ccli enable && /etc/init.d/ccli restart && sleep 1; pgrep -a ccli || true; logread | tail -15"

echo "=== Done ==="

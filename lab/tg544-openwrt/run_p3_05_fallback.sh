#!/usr/bin/env bash
# P3-05: DSO write Wlim, disconnect, wait fallback_s, check DUT log.
set -u
CERT=/mnt/c/Yahia/projects/CCLI/apps/ccli/config/tls
CLI=/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt
EV=/mnt/c/Yahia/projects/CCLI/lab/evidence/O13_1_isolation_2026-09-25
HOST=192.168.10.1
PORT=3782

echo "=== P3-05 apply Wlim then disconnect ==="
"$CLI/mms_tls_wlim_client" "$HOST" "$PORT" "$CERT" dso 25 write
echo "waiting 20 s for Operating Rule fallback (fallback_s=15)..."
sleep 20
echo "DONE wait"

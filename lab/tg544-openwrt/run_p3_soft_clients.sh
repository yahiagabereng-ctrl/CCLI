#!/usr/bin/env bash
set -u
CERT=/mnt/c/Yahia/projects/CCLI/apps/ccli/config/tls
CLI=/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt
EV=/mnt/c/Yahia/projects/CCLI/lab/evidence/O13_1_isolation_2026-09-25
HOST=192.168.10.1
PORT=3782
OUT="$EV/P3_08_09_RBAC_PKI.txt"
{
  echo "=== P3-08 DSO write expect OK ==="
  "$CLI/mms_tls_wlim_client" "$HOST" "$PORT" "$CERT" dso 12 write
  echo
  echo "=== P3-08 VIEWER write expect DENY ==="
  "$CLI/mms_tls_wlim_client" "$HOST" "$PORT" "$CERT" viewer 12 write
  echo
  echo "=== P3-08 VIEWER read expect OK ==="
  "$CLI/mms_tls_wlim_client" "$HOST" "$PORT" "$CERT" viewer 0 read
  echo
  echo "=== P3-09 revoked expect CONNECT FAIL ==="
  "$CLI/mms_tls_wlim_client" "$HOST" "$PORT" "$CERT" revoked 12 write || true
} | tee "$OUT"
echo "WROTE $OUT"

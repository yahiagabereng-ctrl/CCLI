#!/bin/sh
# Ensure ccli (not OEM service) owns 192.168.10.1:3782 TLS MMS.
set -u

echo "=== listeners 3782/102 ==="
netstat -tlnp 2>/dev/null | grep -E '3782|102' || true

echo "=== ccli processes ==="
ps w | grep -v grep | grep ccli || true

echo "=== mms config ==="
grep -E 'enabled|tcp_port|tls_enabled|bind_address' /etc/ccli/lab.yaml | head -8

echo "=== stop ccli + OEM grabbers ==="
/etc/init.d/ccli stop 2>/dev/null || true
killall ccli 2>/dev/null || true
# TesPro LuCI IEC61850 gateway (if installed) may bind :102/:3782
/etc/init.d/iec61850 2>/dev/null && /etc/init.d/iec61850 stop || true
/etc/init.d/iec61850server 2>/dev/null && /etc/init.d/iec61850server stop || true
sleep 1

echo "=== start ccli with lab yaml ==="
/usr/sbin/ccli --config /etc/ccli/lab.yaml >/tmp/ccli-mms.log 2>&1 &
sleep 3

echo "=== after start ==="
ps w | grep -v grep | grep ccli || echo "FAIL: no ccli"
netstat -tlnp 2>/dev/null | grep -E '3782|102' || true
tail -15 /tmp/ccli-mms.log 2>/dev/null || true
logread 2>/dev/null | grep -i mms | tail -5 || true

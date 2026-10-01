#!/bin/sh
# Start / verify ccli MMS on Eth_A (192.168.10.1:3782 TLS). Run on DUT via SSH.
set -u

echo "=== lab.yaml mms ==="
grep -E 'tcp_port|tls_enabled|bind_address' /etc/ccli/lab.yaml 2>/dev/null || echo "WARN: /etc/ccli/lab.yaml missing"

echo "=== tls dir ==="
ls -la /etc/ccli/tls/server.pem /etc/ccli/tls/root_CA.pem /etc/ccli/tls/client.pem 2>/dev/null || {
	echo "FAIL: TLS PEMs missing under /etc/ccli/tls/"
	exit 1
}

echo "=== stop old ==="
/etc/init.d/ccli stop 2>/dev/null || true
killall ccli 2>/dev/null || true
sleep 1

echo "=== start procd ==="
/etc/init.d/ccli start 2>/dev/null || true
sleep 3

if ! ps w | grep -v grep | grep -q '[c]cli'; then
	echo "procd start failed — trying foreground fork"
	/usr/sbin/ccli --config /etc/ccli/lab.yaml >/tmp/ccli-mms.log 2>&1 &
	sleep 2
fi

echo "=== process ==="
ps w | grep -v grep | grep ccli || echo "FAIL: ccli not running"

echo "=== mms log ==="
logread 2>/dev/null | grep -i mms | tail -8
[ -f /tmp/ccli-mms.log ] && tail -8 /tmp/ccli-mms.log

echo "=== listen ==="
netstat -tlnp 2>/dev/null | grep -E '3782|102' || grep EC6 /proc/net/tcp 2>/dev/null || echo "WARN: no MMS port seen"

echo "=== done ==="

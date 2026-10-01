#!/bin/sh
# Install uploaded /tmp/ccli.new and record deploy version on DUT.
set -eu

killall ccli 2>/dev/null || true
sleep 1
mv /tmp/ccli.new /usr/sbin/ccli
chmod +x /usr/sbin/ccli

mkdir -p /etc/ccli/tls
if [ -d /tmp/ccli.tls ]; then
    cp -f /tmp/ccli.tls/* /etc/ccli/tls/
    echo "=== tls deployed ==="
    ls -la /etc/ccli/tls/
    if command -v openssl >/dev/null 2>&1; then
        openssl x509 -in /etc/ccli/tls/server.pem -noout -subject -ext keyUsage 2>/dev/null || true
    fi
fi

if [ -f /tmp/ccli.deploy.manifest ]; then
    cp -f /tmp/ccli.deploy.manifest /etc/ccli/DEPLOY_MANIFEST.md
    echo "=== deploy manifest ==="
    head -20 /etc/ccli/DEPLOY_MANIFEST.md
fi

echo "=== ccli version (binary) ==="
/usr/sbin/ccli --version || true
/usr/sbin/ccli --version-json > /etc/ccli/VERSION.json 2>/dev/null || true
/usr/sbin/ccli --version 2>/dev/null | head -1 > /etc/ccli/VERSION || true

echo "=== sha256 /usr/sbin/ccli ==="
if command -v sha256sum >/dev/null 2>&1; then
    sha256sum /usr/sbin/ccli
elif command -v openssl >/dev/null 2>&1; then
    openssl dgst -sha256 /usr/sbin/ccli
fi

killall ccli 2>/dev/null || true
sleep 1
/usr/sbin/ccli --config /etc/ccli/lab.yaml >>/tmp/ccli.log 2>&1 &
sleep 3
echo "=== process ==="
ps w | grep -v grep | grep ccli || echo "FAIL no ccli"
echo "=== listen (3782 Eth_A, 2404 Eth_B) ==="
netstat -tlnp 2>/dev/null | grep -E '3782|2404' || grep -E '3782|2404' /proc/net/tcp || echo "WARN no 3782/2404"
echo "=== log tail ==="
tail -20 /tmp/ccli.log 2>/dev/null || true

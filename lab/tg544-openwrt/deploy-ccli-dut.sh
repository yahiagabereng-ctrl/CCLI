#!/bin/sh
# Install uploaded /tmp/ccli.new and record deploy version on DUT.
set -eu

mkdir -p /var/lib/ccli

uci set iec61850service.global.enabled='0' 2>/dev/null || true
uci commit iec61850service 2>/dev/null || true
/etc/init.d/iec61850service disable 2>/dev/null || true
/etc/init.d/iec61850service stop 2>/dev/null || true
/etc/init.d/iec61850-mmsd disable 2>/dev/null || true
/etc/init.d/iec61850-mmsd stop 2>/dev/null || true
killall iec61850d 2>/dev/null || true
killall -9 iec61850d 2>/dev/null || true
killall iec61850-mmsd 2>/dev/null || true
killall -9 iec61850-mmsd 2>/dev/null || true
killall ccli 2>/dev/null || true
sleep 1
mv /tmp/ccli.new /usr/sbin/ccli
chmod +x /usr/sbin/ccli

mkdir -p /etc/ccli/tls
if [ -d /tmp/ccli.tls ]; then
    # Replace session PEMs/keys entirely (avoid stale EJBCA/client_acse from merged copy).
    find /etc/ccli/tls -maxdepth 1 -type f \( -name '*.pem' -o -name '*.key' \) -delete 2>/dev/null || true
    rm -f /etc/ccli/tls/client_acse.key /etc/ccli/tls/revoked_acse.key 2>/dev/null || true
    for f in /tmp/ccli.tls/*; do
        [ -f "$f" ] || continue
        cp -f "$f" /etc/ccli/tls/
    done
    echo "=== tls deployed (full replace) ==="
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

if [ -x /tmp/enable-dut-gpio.sh ]; then
    echo "=== enable DIO (GD32) before ccli start ==="
    sh /tmp/enable-dut-gpio.sh
elif [ -f /usr/share/ccli/enable-dut-gpio.sh ]; then
    sh /usr/share/ccli/enable-dut-gpio.sh
else
    /etc/init.d/didoservice_v2 enable 2>/dev/null || true
    /etc/init.d/didoservice_v2 restart 2>/dev/null || true
fi

killall ccli 2>/dev/null || true
sleep 1
/etc/init.d/ccli enable 2>/dev/null || true
/etc/init.d/ccli restart 2>/dev/null || {
	/usr/sbin/ccli --config /etc/ccli/lab.yaml >>/tmp/ccli.log 2>&1 &
	sleep 3
}
sleep 2
echo "=== process ==="
ps w | grep -v grep | grep ccli || echo "FAIL no ccli"
echo "=== listen (3782 Eth_A, 2404 Eth_B) ==="
netstat -tlnp 2>/dev/null | grep -E '3782|2404' || grep -E '3782|2404' /proc/net/tcp || echo "WARN no 3782/2404"
echo "=== log tail ==="
tail -20 /tmp/ccli.log 2>/dev/null || true

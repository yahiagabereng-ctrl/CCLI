#!/bin/sh
# Install OpenWrt procd service for CCLI without a full APK (binary-only Phase 1).
# Run on TG544 as root, or from WSL:
#   scp lab/tg544-openwrt/install-ccli-procd.sh package/ccli/files/ccli.init \
#       package/ccli/files/ccli.config root@192.168.1.130:/tmp/
#   ssh root@192.168.1.130 'sh /tmp/install-ccli-procd.sh /tmp/ccli.init /tmp/ccli.config'
#
# LuCI: System -> Startup -> "ccli" should appear after enable.

set -eu

INIT_SRC="${1:-/tmp/ccli.init}"
UCI_SRC="${2:-/tmp/ccli.config}"

if [ ! -x /usr/sbin/ccli ]; then
	echo "ERROR: /usr/sbin/ccli missing — deploy ccli-bin first." >&2
	exit 1
fi

if [ ! -f "$INIT_SRC" ]; then
	echo "ERROR: init script not found: $INIT_SRC" >&2
	exit 1
fi

mkdir -p /etc/ccli
sed 's/\r$//' "$INIT_SRC" > /etc/init.d/ccli
chmod +x /etc/init.d/ccli

if [ -f "$UCI_SRC" ]; then
	sed 's/\r$//' "$UCI_SRC" > /etc/config/ccli
fi

if [ ! -f /etc/ccli/lab.yaml ]; then
	echo "WARN: /etc/ccli/lab.yaml missing - copy regulation yaml before start." >&2
fi

/etc/init.d/ccli enable
/etc/init.d/ccli restart

echo "OK: procd service ccli enabled (START=99)."
echo "Config: $(uci -q get ccli.main.config_file 2>/dev/null || echo /etc/ccli/lab.yaml)"
pgrep -a ccli || echo "(not running — check logread)"

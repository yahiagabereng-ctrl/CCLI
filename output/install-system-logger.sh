#!/bin/sh
set -eu
expected="$1"
candidate=/tmp/ccli-system-logger-new
test "$(sha256sum "$candidate" | cut -d' ' -f1)" = "$expected"
test ! -e /etc/ccli-upgrade.pending
test ! -e /etc/ccli-upgrade.hold
backup=/mnt/mmcblk0/ccli-before-system-logger-$(date +%Y%m%d-%H%M%S)
mkdir -m 700 "$backup"
cp -p /usr/sbin/ccli "$backup/ccli"
sha256sum /etc/ccli/lab.yaml > "$backup/config.sha256"
tar -czf "$backup/ccli-config-and-events.tgz" -C / etc/ccli etc/config/ccli
chmod 600 "$backup/ccli-config-and-events.tgz"
echo "ROLLBACK=$backup"
/etc/init.d/ccli stop
sleep 2
if pidof ccli >/dev/null; then echo 'CCI did not stop; refusing replacement'; exit 1; fi
cp "$candidate" /usr/sbin/ccli.logger-new
chmod 755 /usr/sbin/ccli.logger-new
chown root:root /usr/sbin/ccli.logger-new
mv /usr/sbin/ccli.logger-new /usr/sbin/ccli
sync
if ! /usr/sbin/ccli --version; then
  cp "$backup/ccli" /usr/sbin/ccli
  /etc/init.d/ccli start
  exit 1
fi
/etc/init.d/ccli start
sleep 12
if ! pidof ccli >/dev/null || ! netstat -lnt | grep -q '192.168.0.1:3782'; then
  /etc/init.d/ccli stop
  sleep 2
  cp "$backup/ccli" /usr/sbin/ccli
  /etc/init.d/ccli start
  echo 'Startup failed; restored previous binary'
  exit 1
fi
sha256sum -c "$backup/config.sha256"
sha256sum /usr/sbin/ccli
/etc/init.d/ccli status
echo SYSTEM_LOGGER_INSTALLED

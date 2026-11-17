#!/bin/sh
set -eu
apk_file=/tmp/ccli-0.1.0-r74.apk
test "$(sha256sum "$apk_file" | cut -d ' ' -f1)" = 54f6ea5cbedc93618aa0b7ebda949a3ca64dcd5fd5132898c905be83dd29ef69
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f1)" = 1f2f34ab0322bae7b6a54c81f17ebc1b22a60ba392ae708b3e59ed354ccb5e08
test ! -e /etc/ccli-upgrade.pending
test ! -e /etc/ccli-upgrade.hold
backup="/mnt/mmcblk0/ccli-r74-logger-$(date -u +%Y%m%dT%H%M%SZ)"
mkdir -m 700 "$backup"
cp -p /usr/sbin/ccli "$backup/ccli.before"
tar -C / -czf "$backup/config-before.tgz" etc/ccli etc/config/ccli
find /etc/ccli -type f ! -path '/etc/ccli/security-events/*' -exec sha256sum '{}' ';' > "$backup/config-before.sha256"
sha256sum /etc/config/ccli >> "$backup/config-before.sha256"
# Retain historical backups on the SD card and preserve their old paths.
# Avoid duplicating many megabytes of obsolete lab data in upgrade preinst.
mkdir -m 700 "$backup/legacy"
for name in logfix-r62-backup-20261020 tsp-r63-backup-20261020 tls-enrollment-backup-20261019; do
    if [ -d "/etc/ccli/$name" ] && [ ! -L "/etc/ccli/$name" ]; then
        mv "/etc/ccli/$name" "$backup/legacy/$name"
        ln -s "$backup/legacy/$name" "/etc/ccli/$name"
    fi
done
sha256sum -c "$backup/config-before.sha256" > "$backup/config-after-archive.check"
/etc/init.d/ccli stop
attempt=0
while pidof ccli >/dev/null && [ "$attempt" -lt 20 ]; do sleep 1; attempt=$((attempt + 1)); done
if pidof ccli >/dev/null; then /etc/init.d/ccli start; exit 1; fi
if ! apk --no-network add --upgrade --allow-untrusted "$apk_file"; then
    echo "APK install failed; configuration backup: $backup"
    /etc/init.d/ccli start
    exit 1
fi
test ! -e /etc/ccli-upgrade.pending
sha256sum -c "$backup/config-before.sha256" > "$backup/config-after-install.check"
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f1)" = d336618c3e6eddc1c2661b8a2715fdce775c9e9138d1c5684e1ae5be17d81616
/usr/sbin/ccli --version
/usr/sbin/ccli --event-wrap-test
# This hold was created by this successful upgrade, after the preflight check.
rm -f /etc/ccli-upgrade.hold
/etc/init.d/ccli start
sleep 3
pidof ccli
echo "BACKUP=$backup"
apk info ccli
sha256sum /usr/sbin/ccli

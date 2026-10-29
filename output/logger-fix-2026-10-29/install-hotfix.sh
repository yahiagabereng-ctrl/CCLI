#!/bin/sh
set -eu
expected_new="$1"
expected_old="$2"
new=/tmp/ccli-logger-fixed
test "$(sha256sum "$new" | cut -d ' ' -f1)" = "$expected_new"
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f1)" = "$expected_old"
chmod 755 "$new"
"$new" --version
"$new" --event-wrap-test
backup="/mnt/mmcblk0/ccli-logger-fix-$(date -u +%Y%m%dT%H%M%SZ)"
mkdir -m 700 "$backup"
cp -p /usr/sbin/ccli "$backup/ccli.before"
cp -p /etc/ccli/lab.yaml "$backup/lab.yaml"
cp "$new" /usr/sbin/ccli.logger-new
chmod 755 /usr/sbin/ccli.logger-new
/etc/init.d/ccli stop
attempt=0
while pidof ccli >/dev/null && [ "$attempt" -lt 20 ]; do
    sleep 1
    attempt=$((attempt + 1))
done
if pidof ccli >/dev/null; then
    /etc/init.d/ccli start
    echo 'CCI did not stop; installation aborted'
    exit 1
fi
cp -rp /etc/ccli/security-events "$backup/events.before"
mv /usr/sbin/ccli.logger-new /usr/sbin/ccli
stale=/etc/ccli/security-events/events.jsonl.platform-auth-audit.platform-auth-audit
if [ -f "$stale" ]; then mv "$stale" "$backup/stale-auth-sidecar"; fi
if ! /etc/init.d/ccli start; then
    cp "$backup/ccli.before" /usr/sbin/ccli.rollback
    chmod 755 /usr/sbin/ccli.rollback
    mv /usr/sbin/ccli.rollback /usr/sbin/ccli
    /etc/init.d/ccli start
    echo 'New service failed to start; restored previous binary'
    exit 1
fi
sleep 3
if ! pidof ccli >/dev/null; then
    /etc/init.d/ccli stop
    cp "$backup/ccli.before" /usr/sbin/ccli.rollback
    chmod 755 /usr/sbin/ccli.rollback
    mv /usr/sbin/ccli.rollback /usr/sbin/ccli
    /etc/init.d/ccli start
    echo 'New process missing; restored previous binary'
    exit 1
fi
sync
echo "BACKUP=$backup"
sha256sum /usr/sbin/ccli
ps w | grep '[c]cli --config'

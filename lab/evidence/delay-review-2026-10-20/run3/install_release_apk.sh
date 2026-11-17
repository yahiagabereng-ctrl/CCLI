#!/bin/sh
set -eu
umask 077
stage=/tmp/ccli-apk-r63-20261020
apkfile=$stage/ccli-0.1.0-r63.apk
expected_apk=74df622f8b6bf584cefa020b4714a340409bec7e75b5ae239003ed02d04b148a
expected_binary=f37a783e5936baceed72f8265c08f42e40c4226cab10c7132035f1b5b13cf418
test ! -e /etc/ccli-upgrade.pending
test ! -e /etc/ccli-upgrade.hold
test "$(sha256sum "$apkfile" | cut -d ' ' -f 1)" = "$expected_apk"
test "$(openssl pkey -pubin -in "$stage/trust/manufacturer.pem" -outform DER | sha256sum | cut -d ' ' -f 1)" = 97ccaf550b53aaa155eb74c1001715019488aa2f901daeb8e5d4eee0edcc4486
apk --keys-dir "$stage/trust" --no-network verify "$apkfile"
apk --keys-dir "$stage/trust" --no-network --simulate add "$apkfile"

owns_listener() {
    inodes=$(awk '$2 ~ /:0EC6$/ && $4 == "0A" {print $10}' /proc/net/tcp)
    for fd in /proc/$1/fd/*; do
        link=$(readlink "$fd" 2>/dev/null || true)
        for inode in $inodes; do [ "$link" = "socket:[$inode]" ] && return 0; done
    done
    return 1
}
old_pids=$(pidof ccli || true)
/etc/init.d/ccli stop
ready=0
for attempt in $(seq 1 30); do
    alive=0
    for pid in $old_pids; do if kill -0 "$pid" 2>/dev/null; then alive=1; fi; done
    if [ "$alive" -eq 0 ] && ! netstat -lnt 2>/dev/null | grep '192.168.0.1:3782 .*LISTEN' >/dev/null; then ready=1; break; fi
    sleep 1
done
test "$ready" -eq 1
cp -p /usr/sbin/ccli "$stage/ccli.before-apk"
cp -p /etc/init.d/ccli "$stage/ccli.init.before-apk"
{
    sha256sum /etc/config/ccli
    find /etc/ccli -type f -exec sha256sum '{}' ';'
} > "$stage/all-config-before.sha256"
cp -p /etc/ccli/security-events/events.jsonl "$stage/events.before.jsonl"
cp -p /etc/ccli/security-events/events.jsonl.1 "$stage/events.before.jsonl.1"
historical_link=$(readlink /etc/ccli/aggregator-backup-20261019)

apk --keys-dir "$stage/trust" --no-network add "$apkfile"
test ! -e /etc/ccli-upgrade.pending
test -e /etc/ccli-upgrade.hold
sha256sum -c "$stage/all-config-before.sha256" > "$stage/all-config-after-check.txt"
test "$(readlink /etc/ccli/aggregator-backup-20261019)" = "$historical_link"
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f 1)" = "$expected_binary"
apk query --installed --fields name,version --format json ccli | grep '0.1.0-r63' >/dev/null
echo ALL_EXISTING_CONFIGURATION_CREDENTIALS_AND_AUDIT_FILES_PRESERVED_BEFORE_START
rm -f /etc/ccli-upgrade.hold
/etc/init.d/ccli start
stable=0
for attempt in $(seq 1 30); do
    found=0
    for pid in $(pidof ccli || true); do
        if [ "$(sha256sum /proc/$pid/exe 2>/dev/null | cut -d ' ' -f 1)" = "$expected_binary" ] && owns_listener "$pid"; then
            found=1
            listener_pid=$pid
        fi
    done
    if [ "$found" -eq 1 ]; then stable=$((stable + 1)); else stable=0; fi
    [ "$stable" -ge 3 ] && break
    sleep 1
done
test "$stable" -ge 3
test ! -e /etc/ccli-upgrade.pending && test ! -e /etc/ccli-upgrade.hold
# Logs may gain startup events; every pre-install byte must remain intact.
if ! cmp -s "$stage/events.before.jsonl" /etc/ccli/security-events/events.jsonl.1; then
    bytes=$(wc -c < "$stage/events.before.jsonl")
    head -c "$bytes" /etc/ccli/security-events/events.jsonl | cmp -s - "$stage/events.before.jsonl"
    cmp -s "$stage/events.before.jsonl.1" /etc/ccli/security-events/events.jsonl.1
fi
echo "APK_R63_RUNNING_AND_OWNS_PORT_3782 pid=$listener_pid stable_checks=$stable"
/usr/sbin/ccli --version
apk query --installed --fields name,version --format json ccli
sha256sum /usr/sbin/ccli /etc/ccli/lab.yaml
df -h /overlay
echo APK_INSTALLATION_VERIFIED_NO_DEVICE_REBOOT

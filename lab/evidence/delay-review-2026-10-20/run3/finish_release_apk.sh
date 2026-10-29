#!/bin/sh
set -eu
stage=/tmp/ccli-apk-r63-20261020
expected_binary=f37a783e5936baceed72f8265c08f42e40c4226cab10c7132035f1b5b13cf418
test ! -e /etc/ccli-upgrade.pending
test -e /etc/ccli-upgrade.hold
sha256sum -c "$stage/all-config-before.sha256" > "$stage/all-config-after-check.txt"
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f 1)" = "$expected_binary"
apk query --installed --fields name,version --format json ccli | grep '0.1.0-r63' >/dev/null
echo ALL_EXISTING_CONFIGURATION_CREDENTIALS_AND_AUDIT_FILES_PRESERVED_BEFORE_START
rm -f /etc/ccli-upgrade.hold
/etc/init.d/ccli start
owns_listener() {
    inodes=$(awk '$2 ~ /:0EC6$/ && $4 == "0A" {print $10}' /proc/net/tcp)
    for fd in /proc/$1/fd/*; do
        link=$(readlink "$fd" 2>/dev/null || true)
        for inode in $inodes; do [ "$link" = "socket:[$inode]" ] && return 0; done
    done
    return 1
}
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
bytes=$(wc -c < "$stage/events.before.jsonl")
head -c "$bytes" /etc/ccli/security-events/events.jsonl | cmp -s - "$stage/events.before.jsonl"
cmp -s "$stage/events.before.jsonl.1" /etc/ccli/security-events/events.jsonl.1
echo "APK_R63_RUNNING_AND_OWNS_PORT_3782 pid=$listener_pid stable_checks=$stable"
/usr/sbin/ccli --version
apk query --installed --fields name,version --format json ccli
sha256sum /usr/sbin/ccli /etc/ccli/lab.yaml
df -h /overlay
echo APK_INSTALLATION_VERIFIED_NO_DEVICE_REBOOT

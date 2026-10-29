#!/bin/sh
set -eu
umask 077
stage=/tmp/ccli-tsp-r63-20261020
backup=/etc/ccli/tsp-r63-backup-20261020
expected=cd1b233b41c8d5360846ec447458fb1fbb599d9eaf8e57c8b162a1c2cdfda2de
original=8a7963740d88f79e26adf162075300f2afe08ff6f5a7b4619c1111713c22449c
test ! -e "$backup"
test ! -e /usr/sbin/ccli.r63.new
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f 1)" = "$original"
test "$(sha256sum /etc/ccli/lab.yaml | cut -d ' ' -f 1)" = 346e0b653d224231c542e664cf50ada443835e44436e3fe43705b5825a203c5c
test "$(sha256sum "$stage/ccli-r63" | cut -d ' ' -f 1)" = "$expected"
mkdir -m 700 "$backup"
cp -p /usr/sbin/ccli "$backup/ccli-r62"
cp -p /etc/ccli/lab.yaml "$backup/lab.yaml"
cp -p /etc/config/ccli "$backup/uci-ccli"
{
    sha256sum /etc/ccli/lab.yaml /etc/config/ccli
    find /etc/ccli/tls /etc/ccli/rbac-dual /etc/ccli/rbac-state -type f -exec sha256sum '{}' ';'
} > "$backup/protected-files.sha256"
cp "$stage/ccli-r63" /usr/sbin/ccli.r63.new
chmod 755 /usr/sbin/ccli.r63.new
test "$(sha256sum /usr/sbin/ccli.r63.new | cut -d ' ' -f 1)" = "$expected"

port_clear() {
    for attempt in $(seq 1 30); do
        if ! netstat -lnt 2>/dev/null | grep '192.168.0.1:3782 .*LISTEN' >/dev/null; then return 0; fi
        sleep 1
    done
    return 1
}
owns_listener() {
    inodes=$(awk '$2 ~ /:0EC6$/ && $4 == "0A" {print $10}' /proc/net/tcp)
    for fd in /proc/$1/fd/*; do
        link=$(readlink "$fd" 2>/dev/null || true)
        for inode in $inodes; do [ "$link" = "socket:[$inode]" ] && return 0; done
    done
    return 1
}
restore_needed=0
rollback() {
    code=$?
    trap - EXIT
    if [ "$restore_needed" -eq 1 ]; then
        /etc/init.d/ccli stop || true
        port_clear || true
        cp "$backup/ccli-r62" /usr/sbin/ccli.rollback.new
        chmod 755 /usr/sbin/ccli.rollback.new
        mv /usr/sbin/ccli.rollback.new /usr/sbin/ccli
        /etc/init.d/ccli start || true
        echo "r63 verification failed; r62 binary restored. Backup: $backup" >&2
    fi
    exit "$code"
}
trap rollback EXIT
old_pids=$(pidof ccli || true)
restore_needed=1
echo RESTARTING_CCI_SERVICE_ONLY_NO_DEVICE_REBOOT
/etc/init.d/ccli stop
for attempt in $(seq 1 30); do
    alive=0
    for pid in $old_pids; do if kill -0 "$pid" 2>/dev/null; then alive=1; fi; done
    [ "$alive" -eq 0 ] && break
    sleep 1
done
test "$alive" -eq 0
port_clear
cp -p /etc/ccli/security-events/events.jsonl "$backup/events.before.jsonl"
cp -p /etc/ccli/security-events/events.jsonl.1 "$backup/events.before.jsonl.1"
mv /usr/sbin/ccli.r63.new /usr/sbin/ccli
/etc/init.d/ccli start
ready=0
for attempt in $(seq 1 30); do
    for pid in $(pidof ccli || true); do
        if [ "$(sha256sum /proc/$pid/exe 2>/dev/null | cut -d ' ' -f 1)" = "$expected" ] && owns_listener "$pid"; then
            ready=1
            echo "VERIFIED_R63_LISTENER_OWNER pid=$pid"
        fi
    done
    [ "$ready" -eq 1 ] && break
    sleep 1
done
test "$ready" -eq 1
sha256sum -c "$backup/protected-files.sha256" > "$backup/protected-files-check.txt"
if cmp -s "$backup/events.before.jsonl" /etc/ccli/security-events/events.jsonl.1; then
    echo AUDIT_SEGMENT_ROTATED_INTACT
else
    bytes=$(wc -c < "$backup/events.before.jsonl")
    head -c "$bytes" /etc/ccli/security-events/events.jsonl | cmp -s - "$backup/events.before.jsonl"
    cmp -s "$backup/events.before.jsonl.1" /etc/ccli/security-events/events.jsonl.1
    echo BOTH_EXISTING_AUDIT_SEGMENTS_PRESERVED
fi
restore_needed=0
trap - EXIT
/usr/sbin/ccli --version
sha256sum /usr/sbin/ccli /etc/ccli/lab.yaml
wc -l /etc/ccli/security-events/events.jsonl*
echo PROTECTED_CONFIGURATION_AND_CREDENTIAL_HASHES_UNCHANGED
echo "R63_APPLIED backup=$backup"

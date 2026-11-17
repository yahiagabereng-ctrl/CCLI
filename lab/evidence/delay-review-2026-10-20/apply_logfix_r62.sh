#!/bin/sh
set -eu
umask 077
stage=/tmp/ccli-logfix-r62-20261020
backup=/etc/ccli/logfix-r62-backup-20261020
expected=8a7963740d88f79e26adf162075300f2afe08ff6f5a7b4619c1111713c22449c
original=861ec666a0aced3b9c97ac0d210c2bffe4c14233d7cd5ef8e3ef66ad4fbca4d0

test ! -e "$backup"
test ! -e /usr/sbin/ccli.r62.new
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f 1)" = "$original"
test "$(sha256sum /etc/ccli/lab.yaml | cut -d ' ' -f 1)" = 346e0b653d224231c542e664cf50ada443835e44436e3fe43705b5825a203c5c
test "$(sha256sum "$stage/ccli-r62" | cut -d ' ' -f 1)" = "$expected"
mkdir -m 700 "$backup"
cp -p /usr/sbin/ccli "$backup/ccli-r61"
cp -p /etc/ccli/lab.yaml "$backup/lab.yaml"
cp -p /etc/config/ccli "$backup/uci-ccli"
{
    sha256sum /etc/ccli/lab.yaml /etc/config/ccli
    find /etc/ccli/tls /etc/ccli/rbac-dual /etc/ccli/rbac-state -type f -exec sha256sum '{}' ';'
} > "$backup/protected-files.sha256"
cp "$stage/ccli-r62" /usr/sbin/ccli.r62.new
chmod 755 /usr/sbin/ccli.r62.new
test "$(sha256sum /usr/sbin/ccli.r62.new | cut -d ' ' -f 1)" = "$expected"

restore_needed=0
rollback() {
    code=$?
    trap - EXIT
    if [ "$restore_needed" -eq 1 ]; then
        /etc/init.d/ccli stop || true
        cp "$backup/ccli-r61" /usr/sbin/ccli.rollback.new
        chmod 755 /usr/sbin/ccli.rollback.new
        mv /usr/sbin/ccli.rollback.new /usr/sbin/ccli
        /etc/init.d/ccli start || true
        echo "r62 verification failed; original CCI restored. Backup: $backup" >&2
    fi
    exit "$code"
}
trap rollback EXIT
old_pids=$(pidof ccli || true)
restore_needed=1
echo STOPPING_CCI_SERVICE_ONLY
/etc/init.d/ccli stop
alive=0
for attempt in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30; do
    alive=0
    for pid in $old_pids; do if kill -0 "$pid" 2>/dev/null; then alive=1; fi; done
    [ "$alive" -eq 0 ] && break
    sleep 1
done
test "$alive" -eq 0
# Forked helpers from the old daemon can briefly inherit its listening fd.
# Do not start the replacement until those copies have closed as well.
port_busy=0
for attempt in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30; do
    port_busy=0
    if netstat -lnt 2>/dev/null | grep '192.168.0.1:3782 .*LISTEN' >/dev/null; then port_busy=1; fi
    [ "$port_busy" -eq 0 ] && break
    sleep 1
done
test "$port_busy" -eq 0
# Save the settled audit files without clearing them. The new reader migrates
# the existing full segment by rename on its next append.
cp -p /etc/ccli/security-events/events.jsonl "$backup/events.before.jsonl"
if [ -f /etc/ccli/security-events/events.jsonl.1 ]; then
    cp -p /etc/ccli/security-events/events.jsonl.1 "$backup/events.before.jsonl.1"
fi
mv /usr/sbin/ccli.r62.new /usr/sbin/ccli
/etc/init.d/ccli start
ready=0
owns_listener() {
    inodes=$(awk '$2 ~ /:0EC6$/ && $4 == "0A" {print $10}' /proc/net/tcp)
    for fd in /proc/$1/fd/*; do
        link=$(readlink "$fd" 2>/dev/null || true)
        for inode in $inodes; do [ "$link" = "socket:[$inode]" ] && return 0; done
    done
    return 1
}
for attempt in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30; do
    for pid in $(pidof ccli || true); do
        if [ "$(sha256sum /proc/$pid/exe 2>/dev/null | cut -d ' ' -f 1)" = "$expected" ] &&
           owns_listener "$pid"; then ready=1; fi
    done
    [ "$ready" -eq 1 ] && break
    sleep 1
done
test "$ready" -eq 1
sha256sum -c "$backup/protected-files.sha256" > "$backup/protected-files-check.txt"
test "$(sha256sum "$backup/events.before.jsonl" | cut -d ' ' -f 1)" = \
     "$(sha256sum /etc/ccli/security-events/events.jsonl.1 | cut -d ' ' -f 1)"
restore_needed=0
trap - EXIT
/usr/sbin/ccli --version
pidof ccli
sha256sum /usr/sbin/ccli /etc/ccli/lab.yaml
wc -l /etc/ccli/security-events/events.jsonl /etc/ccli/security-events/events.jsonl.1
echo PROTECTED_CONFIGURATION_AND_CREDENTIAL_HASHES_UNCHANGED
echo ORIGINAL_FULL_AUDIT_SEGMENT_PRESERVED_BYTE_FOR_BYTE
echo "LOGFIX_R62_APPLIED backup=$backup"

#!/bin/sh
set -eu
backup=/etc/ccli/logfix-r62-backup-20261020
expected=8a7963740d88f79e26adf162075300f2afe08ff6f5a7b4619c1111713c22449c
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f 1)" = "$expected"
sha256sum -c "$backup/protected-files.sha256" > "$backup/protected-files-check.txt"
/etc/init.d/ccli stop
ready=0
for attempt in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30; do
    if ! netstat -lnt 2>/dev/null | grep '192.168.0.1:3782 .*LISTEN' >/dev/null; then ready=1; break; fi
    sleep 1
done
test "$ready" -eq 1
/etc/init.d/ccli start
owns_listener() {
    inodes=$(awk '$2 ~ /:0EC6$/ && $4 == "0A" {print $10}' /proc/net/tcp)
    for fd in /proc/$1/fd/*; do
        link=$(readlink "$fd" 2>/dev/null || true)
        for inode in $inodes; do [ "$link" = "socket:[$inode]" ] && return 0; done
    done
    return 1
}
ready=0
for attempt in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30; do
    for pid in $(pidof ccli || true); do
        if [ "$(sha256sum /proc/$pid/exe 2>/dev/null | cut -d ' ' -f 1)" = "$expected" ] && owns_listener "$pid"; then
            ready=1
            echo "VERIFIED_R62_LISTENER_OWNER pid=$pid"
        fi
    done
    [ "$ready" -eq 1 ] && break
    sleep 1
done
test "$ready" -eq 1
sha256sum -c "$backup/protected-files.sha256" > "$backup/protected-files-check.txt"
/usr/sbin/ccli --version
wc -l /etc/ccli/security-events/events.jsonl*
echo RESTART_AND_PROTECTED_FILE_CHECK_PASS

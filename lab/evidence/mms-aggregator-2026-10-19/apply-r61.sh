#!/bin/sh
set -eu
stage=/tmp/ccli-aggregator-20261019
backup=/etc/ccli/aggregator-backup-20261019
expected=861ec666a0aced3b9c97ac0d210c2bffe4c14233d7cd5ef8e3ef66ad4fbca4d0
test ! -e "$backup"
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f 1)" = e61fdf9c5b14be9dac84edffe61d8b6bab650d6546a7e4c722fc2075ce819188
test "$(sha256sum /etc/ccli/lab.yaml | cut -d ' ' -f 1)" = 346e0b653d224231c542e664cf50ada443835e44436e3fe43705b5825a203c5c
test "$(sha256sum "$stage/ccli" | cut -d ' ' -f 1)" = "$expected"
mkdir -m 700 "$backup"
cp -p /usr/sbin/ccli "$backup/ccli"
cp -p /etc/ccli/lab.yaml "$backup/lab.yaml"
cp "$stage/ccli" /usr/sbin/ccli.r61.new
chmod 755 /usr/sbin/ccli.r61.new
changed=0
rollback() {
    code=$?
    trap - EXIT
    if [ "$changed" -eq 1 ]; then
        /etc/init.d/ccli stop || true
        cp -p "$backup/ccli" /usr/sbin/ccli
        /etc/init.d/ccli start || true
        echo 'r61 startup failed; previous CCI restored' >&2
    fi
    exit "$code"
}
trap rollback EXIT
old_pids=$(pidof ccli || true)
/etc/init.d/ccli stop
changed=1
for attempt in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do
    alive=0
    for pid in $old_pids; do if kill -0 "$pid" 2>/dev/null; then alive=1; fi; done
    [ "$alive" -eq 0 ] && break
    sleep 1
done
test "$alive" -eq 0
mv /usr/sbin/ccli.r61.new /usr/sbin/ccli
/etc/init.d/ccli start
ready=0
for attempt in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15; do
    for pid in $(pidof ccli || true); do
        if [ "$(sha256sum /proc/"$pid"/exe 2>/dev/null | cut -d ' ' -f 1)" = "$expected" ] &&
            netstat -lntp 2>/dev/null | grep "192.168.0.1:3782 .* $pid/ccli" >/dev/null; then
            ready=1
        fi
    done
    [ "$ready" -eq 1 ] && break
    sleep 1
done
test "$ready" -eq 1
changed=0
trap - EXIT
/usr/sbin/ccli --version
pidof ccli
sha256sum /usr/sbin/ccli /etc/ccli/lab.yaml /etc/ccli/rbac-dual/package/*.xml
echo "AGGREGATOR_R61_APPLIED backup=$backup"

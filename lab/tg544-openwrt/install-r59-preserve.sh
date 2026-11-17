#!/bin/sh
# Controlled r59 upgrade, preserving device keys and all activation state.
set -eu
umask 077
backup=/root/ccli-before-r59
stage=/etc/ccli/rbac-dual
apk_file=/tmp/ccli-0.1.0-r59.apk
apk_hash=ecbcdeacf0dcc3925bcf25ee7df5d5df9bd15954bdb87ed3652459bdfa44d875
binary_hash=42af88a5a555c11a77ababd827445815f1d741dc1438f385088ca0cec824f64f
new_state='3 0a0712723b81d5bd1a0f4796d595c18162e523da6d92c87dc6ed74eeb606d5b7'
test -f "$backup/system-backup.tar.gz"
test "$(sha256sum "$apk_file" | cut -d ' ' -f 1)" = "$apk_hash"
sha256sum -c "$backup/config.sha256"
sha256sum -c "$backup/uci.sha256"
sha256sum -c "$backup/tls.sha256"
cmp /etc/ccli/rbac-state/activation.state "$backup/activation-before.state"
test ! -e /etc/ccli-upgrade.pending
test ! -e /etc/ccli-upgrade.hold
! netstat -lnt | grep -q :3783
(cd "$stage" && sha256sum -c stage.sha256)
cp -p /usr/sbin/ccli "$backup/ccli-r58"
cp -p /etc/init.d/ccli "$backup/init-r58"
apk add --simulate --allow-untrusted --no-network "$apk_file"
changed=0
rollback() {
    result=$?
    trap - EXIT
    if [ "$changed" = 1 ] && [ "$result" != 0 ]; then
        echo 'r59 verification failed: recovering previous runtime; activation state will not be rolled back' >&2
        /etc/init.d/ccli stop || true
        if ! apk add --allow-untrusted --no-network "$backup/ccli-0.1.0-r58.apk"; then
            cp -p "$backup/ccli-r58" /usr/sbin/ccli
            cp -p "$backup/init-r58" /etc/init.d/ccli
            echo 'Package registration requires review; previous runtime restored' >&2
        fi
        state=$(cat /etc/ccli/rbac-state/activation.state)
        if [ "$state" = "$new_state" ]; then
            cp -p "$stage/lab.yaml" /etc/ccli/lab.yaml
        elif cmp /etc/ccli/rbac-state/activation.state "$backup/activation-before.state"; then
            cp -p "$backup/lab.yaml" /etc/ccli/lab.yaml
        else
            echo 'Unexpected activation state: preserving it and keeping service held for review' >&2
            exit "$result"
        fi
        sha256sum -c "$backup/tls.sha256" || exit "$result"
        sha256sum -c "$backup/uci.sha256" || exit "$result"
        rm -f /etc/ccli-upgrade.pending /etc/ccli-upgrade.hold
        /etc/init.d/ccli start || true
    fi
    exit "$result"
}
trap rollback EXIT
changed=1
/etc/init.d/ccli stop
apk add --allow-untrusted --no-network "$apk_file"
test -f /etc/ccli-upgrade.hold
test ! -e /etc/ccli-upgrade.pending
if pidof ccli >/dev/null; then echo 'Unexpected implicit start' >&2; exit 1; fi
sha256sum -c "$backup/config.sha256"
sha256sum -c "$backup/uci.sha256"
sha256sum -c "$backup/tls.sha256"
cmp /etc/ccli/rbac-state/activation.state "$backup/activation-before.state"
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f 1)" = "$binary_hash"
/usr/sbin/ccli --version | tee "$backup/r59-version.txt"
grep -q '0.1.0-r59' "$backup/r59-version.txt"
cp -p "$stage/lab.yaml" /etc/ccli/lab.yaml.new
chmod 600 /etc/ccli/lab.yaml.new
mv /etc/ccli/lab.yaml.new /etc/ccli/lab.yaml
rm -f /etc/ccli-upgrade.hold
/etc/init.d/ccli start
for n in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15; do
    if netstat -lnt | grep -q '192.168.0.1:3782'; then break; fi
    sleep 1
done
set -- $(pidof ccli)
test "$#" = 1
netstat -lnt | grep '192.168.0.1:3782'
test "$(cat /etc/ccli/rbac-state/activation.state)" = "$new_state"
sha256sum -c "$backup/tls.sha256"
sha256sum -c "$backup/uci.sha256"
openssl s_client -connect 192.168.0.1:3782 -tls1_2 -cert /etc/ccli/tls/client_tls.pem -key /etc/ccli/tls/client_tsp.key -CAfile /etc/ccli/tls/root_CA.pem -verify_return_error -brief </dev/null > "$backup/original-tsp-tls-after.log" 2>&1
grep -q 'Verification: OK' "$backup/original-tsp-tls-after.log"
grep -q 'TLSv1.2' "$backup/original-tsp-tls-after.log"
cat "$backup/original-tsp-tls-after.log"
sha256sum /usr/sbin/ccli /etc/ccli/lab.yaml /etc/config/ccli
cat /etc/ccli/rbac-state/activation.state
pidof ccli
echo 'PASS: r59 installed, original TLS credentials preserved, additional TSP TLS client accepted, sequence 3 activated'

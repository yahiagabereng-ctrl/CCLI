#!/bin/sh
# Controlled TLS-verifier correction; credentials and active settings unchanged.
set -eu
apk_file=/tmp/ccli-0.1.0-r58.apk
backup=/root/ccli-before-r58
expected_apk="${1:?APK SHA256 required}"
expected_binary="${2:?payload SHA256 required}"
test -f "$backup/system-backup.tar.gz"
test "$(sha256sum "$apk_file" | cut -d ' ' -f 1)" = "$expected_apk"
sha256sum -c "$backup/config.sha256"
sha256sum -c "$backup/uci.sha256"
test ! -e /etc/ccli-upgrade.pending
test ! -e /etc/ccli-upgrade.hold
if grep -Eq '^[[:space:]]*rbac_profile_a:[[:space:]]*(true|yes|1)' /etc/ccli/lab.yaml; then
    echo 'Refusing: Profile A must remain off pending commissioning' >&2
    exit 1
fi
apk add --simulate --allow-untrusted --no-network "$apk_file"
changed=0
rollback() {
    result=$?
    trap - EXIT
    if [ "$changed" = 1 ] && [ "$result" != 0 ]; then
        echo 'Verification failed; restoring the saved r57 runtime' >&2
        /etc/init.d/ccli stop || true
        tar -xzf "$backup/system-backup.tar.gz" -C /
        rm -f /etc/ccli-upgrade.pending /etc/ccli-upgrade.hold
        /etc/init.d/ccli start || true
        echo 'Runtime restored; review package registration before retry.' >&2
    fi
    exit "$result"
}
trap rollback EXIT
changed=1
/etc/init.d/ccli stop
apk add --allow-untrusted --no-network "$apk_file"
test -f /etc/ccli-upgrade.hold
test ! -e /etc/ccli-upgrade.pending
if pidof ccli >/dev/null; then echo 'Unexpected automatic start' >&2; exit 1; fi
sha256sum -c "$backup/config.sha256"
sha256sum -c "$backup/uci.sha256"
sha256sum -c "$backup/tls.sha256"
/usr/sbin/ccli --version | tee "$backup/r58-version.txt"
grep -q '0.1.0-r58' "$backup/r58-version.txt"
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f 1)" = "$expected_binary"
test "$(ls -ln /usr/sbin/ccli | awk '{print $3}')" = 0
test "$(ls -ln /etc/init.d/ccli | awk '{print $3}')" = 0
rm -f /etc/ccli-upgrade.hold
/etc/init.d/ccli start
sleep 3
set -- $(pidof ccli)
test "$#" = 1
openssl s_client -connect 192.168.0.1:3782 -tls1_2 \
    -cert /etc/ccli/tls/client_tls.pem -key /etc/ccli/tls/client_tsp.key \
    -CAfile /etc/ccli/tls/root_CA.pem -verify_return_error -brief </dev/null \
    > "$backup/tls-valid-after.log" 2>&1
grep -q 'Verification: OK' "$backup/tls-valid-after.log"
grep -q 'TLSv1.2' "$backup/tls-valid-after.log"
if openssl s_client -connect 192.168.0.1:3782 -tls1_2 \
    -cert /etc/ccli/tls/revoked_tls.pem -key /etc/ccli/tls/revoked.key \
    -CAfile /etc/ccli/tls/root_CA.pem -verify_return_error -brief </dev/null \
    > "$backup/tls-revoked-after.log" 2>&1; then
    echo 'Revoked TLS client was accepted' >&2; exit 1
fi
grep -qi 'alert' "$backup/tls-revoked-after.log"
cat "$backup/tls-valid-after.log" "$backup/tls-revoked-after.log"
netstat -lntp | grep '192.168.0.1:3782'
echo 'PASS: r58 installed, configuration/TLS unchanged, valid client accepted, revoked client rejected at TLS'

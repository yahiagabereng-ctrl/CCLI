#!/bin/sh
# Run only against the explicitly confirmed lab Tespro; no credentials embedded.
set -eu
apk_file=/tmp/ccli-0.1.0-r57.apk
backup=/root/ccli-before-r57
expected="${1:?expected APK SHA256 required}"
test -f "$backup/system-backup.tar.gz"
test "$(sha256sum "$apk_file" | cut -d ' ' -f 1)" = "$expected"
sha256sum -c "$backup/config.sha256"
sha256sum -c "$backup/uci.sha256"
test ! -e /etc/ccli-upgrade.pending
test ! -e /etc/ccli-upgrade.hold
if grep -Eq '^[[:space:]]*rbac_profile_a:[[:space:]]*(true|yes|1)' /etc/ccli/lab.yaml; then
    echo 'Refusing: existing configuration enables RBAC before commissioning' >&2
    exit 1
fi
repair="${2:-no}"
if [ "$repair" = repair ]; then
    # An earlier guarded attempt can leave r57 registered while restoring the
    # old runtime. Restore only these two verified payload files, not config.
    apk list --installed ccli | grep -q '^ccli-0.1.0-r57 '
    mkdir -p /tmp/ccli-r57-repair
    apk extract --allow-untrusted --no-chown --destination /tmp/ccli-r57-repair "$apk_file"
    test "$(sha256sum /tmp/ccli-r57-repair/usr/sbin/ccli | cut -d ' ' -f 1)" = d7b69c916f2cc79dd0fdfe077e6c68ce1552ddba2d7fc301b04158cb43334593
    /tmp/ccli-r57-repair/usr/sbin/ccli --version
else
    apk add --simulate --allow-untrusted --no-network "$apk_file"
fi
changed=0
rollback() {
    result=$?
    trap - EXIT
    if [ "$changed" = 1 ] && [ "$result" != 0 ]; then
        echo 'Verification failed: restoring the original binary/configuration' >&2
        /etc/init.d/ccli stop || true
        tar -xzf "$backup/system-backup.tar.gz" -C /
        rm -f /etc/ccli-upgrade.pending /etc/ccli-upgrade.hold
        /etc/init.d/ccli start || true
        echo 'Original runtime restored; APK metadata must be reviewed before retry.' >&2
    fi
    exit "$result"
}
trap rollback EXIT
changed=1
/etc/init.d/ccli stop
if [ "$repair" = repair ]; then
    : > /etc/ccli-upgrade.hold
    chmod 600 /etc/ccli-upgrade.hold
    cp /tmp/ccli-r57-repair/usr/sbin/ccli /usr/sbin/ccli.r57-new
    chmod 755 /usr/sbin/ccli.r57-new
    chown root:root /usr/sbin/ccli.r57-new
    mv /usr/sbin/ccli.r57-new /usr/sbin/ccli
    cp /tmp/ccli-r57-repair/etc/init.d/ccli /etc/init.d/ccli
    chmod 755 /etc/init.d/ccli
    chown root:root /etc/init.d/ccli
else
    apk add --allow-untrusted --no-network "$apk_file"
fi
test -f /etc/ccli-upgrade.hold
test ! -e /etc/ccli-upgrade.pending
# The package must not have started a new process before verification.
if pidof ccli >/dev/null; then echo 'Unexpected automatic CCLI start' >&2; exit 1; fi
sha256sum -c "$backup/config.sha256"
sha256sum -c "$backup/uci.sha256"
sha256sum -c "$backup/tls.sha256"
/usr/sbin/ccli --version | tee "$backup/r57-version.txt"
grep -q '0.1.0-r57' "$backup/r57-version.txt"
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f 1)" = d7b69c916f2cc79dd0fdfe077e6c68ce1552ddba2d7fc301b04158cb43334593
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
    > "$backup/tls-after.log" 2>&1
grep -q 'Verification: OK' "$backup/tls-after.log"
grep -q 'TLSv1.2' "$backup/tls-after.log"
cat "$backup/tls-after.log"
netstat -lntp | grep '192.168.0.1:3782'
echo 'PASS: r57 installed, automatic start held, configuration/TLS unchanged, one service and mutual TLS verified'

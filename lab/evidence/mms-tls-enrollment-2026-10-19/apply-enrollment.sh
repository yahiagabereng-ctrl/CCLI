#!/bin/sh
set -eu
stage=/tmp/ccli-tls-enrollment-20261019
backup=/etc/ccli/tls-enrollment-backup-20261019
test ! -e "$backup"
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f 1)" = f0748d747589826b14a509bd99ded6bcee001d19924249070d19b0e3006d65dd
test "$(sha256sum /etc/ccli/lab.yaml | cut -d ' ' -f 1)" = 5d24eda5a6beb7b725697ef02f504153cb86d67b5efb2bbc9c1cf50dda11b9d9
cd "$stage"
sha256sum -c stage.sha256
test "$(ls ./*.pem | wc -l)" -eq 15
test "$(grep -c '^  tls_allowed_client_cert:' lab.yaml)" -eq 15
if grep -l 'PRIVATE KEY' ./*.pem; then exit 1; fi
mkdir -m 700 "$backup"
cp -p /usr/sbin/ccli "$backup/ccli"
cp -p /etc/ccli/lab.yaml "$backup/lab.yaml"
changed=0
rollback() {
    code=$?
    trap - EXIT
    if [ "$changed" -eq 1 ]; then
        /etc/init.d/ccli stop || true
        cp -p "$backup/ccli" /usr/sbin/ccli
        cp -p "$backup/lab.yaml" /etc/ccli/lab.yaml
        /etc/init.d/ccli start || true
        echo 'Enrollment failed; restored original CCI and configuration' >&2
    fi
    exit "$code"
}
trap rollback EXIT
for cert in ./*.pem; do
    name=$(basename "$cert")
    test ! -e "/etc/ccli/rbac-clients/$name"
    cp "$cert" "/etc/ccli/rbac-clients/$name"
    chmod 600 "/etc/ccli/rbac-clients/$name"
done
cp ccli /usr/sbin/ccli.enrolled
chmod 755 /usr/sbin/ccli.enrolled
cp lab.yaml /etc/ccli/lab.yaml.enrolled
chmod 600 /etc/ccli/lab.yaml.enrolled
/etc/init.d/ccli stop
changed=1
mv /usr/sbin/ccli.enrolled /usr/sbin/ccli
mv /etc/ccli/lab.yaml.enrolled /etc/ccli/lab.yaml
/etc/init.d/ccli start
ready=0
for attempt in 1 2 3 4 5 6 7 8 9 10; do
    if netstat -lntp 2>/dev/null | grep '192.168.0.1:3782 ' >/dev/null; then ready=1; break; fi
    sleep 1
done
test "$ready" -eq 1
pidof ccli
sha256sum /usr/sbin/ccli /etc/ccli/lab.yaml /etc/ccli/rbac-dual/package/*.xml
echo "TLS_ENROLLMENT_APPLIED backup=$backup"
changed=0
trap - EXIT

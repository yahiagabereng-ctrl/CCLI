#!/bin/sh
# Commission the approved EJBCA-only trust migration on the current lab DUT.
# Run after APK upgrade/preservation checks; argument is the public EJBCA base URL.
set -eu
umask 077
[ "$#" -eq 1 ] || { echo 'usage: commission-crl-pull.sh http(s)://EJBCA:PORT' >&2; exit 2; }
base=${1%/}
case "$base" in http://*|https://*) ;; *) exit 2;; esac
[ ! -e /etc/ccli-upgrade.hold ] && [ ! -e /etc/ccli-upgrade.pending ]
[ -x /usr/sbin/ccli-crl-refresh ]
state=/etc/ccli/crl-pull
backup=$(mktemp -d /etc/ccli-crl-migration.XXXXXX)
cp -p /etc/ccli/lab.yaml "$backup/lab.yaml"
cp -p /etc/config/ccli-crl "$backup/ccli-crl"
# User approved retirement of this superseded legacy test fixture. Keep the
# EJBCA viewer-r0/viewer-r1 credentials and their commissioned permissions.
awk '!($1 == "tls_allowed_client_cert:" && $2 == "/etc/ccli/rbac-clients/dual-viewer.pem")' \
    /etc/ccli/lab.yaml > "$backup/migration-input.yaml"
mkdir -p "$state/anchors"
chmod 700 "$state" "$state/anchors"
cp /etc/ccli/rbac-trust/ccli-lab-ca-20261028.pem "$state/anchors/tls-ca.pem"
cp /etc/ccli/rbac-trust/rbac_ca.pem "$state/anchors/rbac-ca.pem"
cat "$state/anchors/tls-ca.pem" "$state/anchors/rbac-ca.pem" > "$state/anchors/ca-bundle.pem"
printf '%s\n' \
 "tlsroot tls $base/ejbca/publicweb/webdist/certdist?cmd=crl&issuer=CN%3DCCLI_Lab_Root_CA_2026%2COU%3DCCLI-Lab%2CO%3DHiTEKS%2CC%3DIT $state/anchors/tls-ca.pem" \
 "rbac both $base/ejbca/publicweb/webdist/certdist?cmd=crl&issuer=CN%3DCCLI%20RBAC%20Lab%20CA%2CO%3DHiTEKS%2CC%3DIT $state/anchors/rbac-ca.pem" > "$state/issuers.conf"
/usr/sbin/ccli-crl-refresh "$state/issuers.conf" "$state"
# Prove all configured transport/association server and client certificates
# still validate before retiring legacy transport trust. No key files are read.
awk '$1 ~ /^(tls_own_cert:|tls_additional_own_cert:|tls_acse_cert:|tls_client_cert:|tls_acse_client_cert:|tls_allowed_client_cert:)$/ {print $2}' \
    "$backup/migration-input.yaml" > "$backup/certificates"
while read -r cert; do
    openssl verify -no-CApath -no-CAstore -CAfile "$state/anchors/ca-bundle.pem" \
        -CRLfile "$state/current/tls-crl-bundle.pem" -crl_check "$cert"
done < "$backup/certificates"
awk -v state="$state" '
 /^mms:/ {m=1; print; print "  tls_revocation_check_s: 60"; next}
 /^[^ #][^:]*:/ {m=0}
 m && /^[ ]+tls_revocation_check_s:/ {next}
 m && /^[ ]+tls_ca_cert:/ {print "  tls_ca_cert: " state "/anchors/ca-bundle.pem"; next}
 m && /^[ ]+tls_crl:/ {print "  tls_crl: " state "/current/tls-crl-bundle.pem"; next}
 m && /^[ ]+rbac_identity_crl:/ {print "  rbac_identity_crl: " state "/current/rbac_crl.pem"; next}
 {print}
' "$backup/migration-input.yaml" > "$backup/new.yaml"
grep -q 'tls_crl: /etc/ccli/crl-pull/current/tls-crl-bundle.pem' "$backup/new.yaml"
rollback() {
    echo "Commissioning failed; restoring configuration from $backup" >&2
    /etc/init.d/ccli-crl stop || :
    cp -p "$backup/lab.yaml" /etc/ccli/lab.yaml
    cp -p "$backup/ccli-crl" /etc/config/ccli-crl
    /etc/init.d/ccli restart || :
}
trap rollback EXIT
/etc/init.d/ccli stop
released=0
for attempt in 1 2 3 4 5 6 7 8 9 10; do
    if ! netstat -lnt | grep -q ':3782 .*LISTEN'; then released=1; break; fi
    sleep 1
done
[ "$released" = 1 ]
cp "$backup/new.yaml" /etc/ccli/lab.yaml
/etc/init.d/ccli start
ready=0
for attempt in 1 2 3 4 5 6 7 8 9 10; do
    if netstat -lnt | grep -q ':3782 .*LISTEN'; then ready=1; break; fi
    sleep 1
done
[ "$ready" = 1 ]
uci set ccli-crl.main.enabled=1
uci set ccli-crl.main.issuers="$state/issuers.conf"
uci set ccli-crl.main.state="$state"
uci set ccli-crl.main.interval=3600
uci set ccli-crl.main.retry=60
uci commit ccli-crl
/etc/init.d/ccli-crl enable
/etc/init.d/ccli-crl start
trap - EXIT
echo "CRL pull commissioned; rollback configuration: $backup"

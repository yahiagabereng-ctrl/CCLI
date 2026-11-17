#!/bin/sh
set -eu
umask 077
testroot=/etc/ccli/rbac-test-20261016/tsp-client-20261020
oldroot=/etc/ccli/rbac-test-20261016/tsp-r63-20261020
stage=/tmp/ccli-tsp-r63-20261020
if netstat -lnt 2>/dev/null | grep ':3783 .*LISTEN' >/dev/null; then exit 1; fi
test ! -e "$testroot"
mkdir -m 700 "$testroot" "$testroot/package"
cp /etc/ccli/rbac-dual/package/*.xml "$testroot/package/"
cp /etc/ccli/rbac-state/activation.state "$testroot/activation.state"
sed "s|$oldroot|$testroot|g" "$oldroot/server.yaml" > "$testroot/server.yaml"
echo $$ > "$stage/tsp-client-harness.pid"
exec "$stage/mms-rbac-device-server-r63" "$testroot/server.yaml" 300 > "$stage/tsp-client-harness.log" 2>&1

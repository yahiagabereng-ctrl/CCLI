#!/bin/sh
set -eu
umask 077
stage=/tmp/ccli-logfix-r62-20261020
testroot=/etc/ccli/rbac-test-20261016/logfix-r62-20261020
test ! -e "$testroot"
if netstat -lnt 2>/dev/null | grep ':3783 .*LISTEN' >/dev/null; then
    echo 'Isolated test port 3783 already in use; refusing to interfere' >&2
    exit 1
fi
test "$(sha256sum "$stage/ccli-r62" | cut -d ' ' -f 1)" = 8a7963740d88f79e26adf162075300f2afe08ff6f5a7b4619c1111713c22449c
test "$(sha256sum "$stage/mms-rbac-device-server-r62" | cut -d ' ' -f 1)" = 0b75adf05d4b7cea016b17889ba43a93e34c96c8904e3275e0a409df10fc5483
test "$(sha256sum "$stage/event-store-rotation-test" | cut -d ' ' -f 1)" = d5d7ffb6b4fc2df7728c3d43fdaf5903060699789f88cc0e552d6675899dc36d
chmod 755 "$stage/ccli-r62" "$stage/mms-rbac-device-server-r62" "$stage/event-store-rotation-test"
"$stage/ccli-r62" --version
"$stage/event-store-rotation-test" > "$stage/rotation-test.log" 2>&1
echo ON_DEVICE_ROTATION_REGRESSIONS_PASS
mkdir -m 700 "$testroot" "$testroot/package"
cp /etc/ccli/rbac-dual/package/*.xml "$testroot/package/"
cp /etc/ccli/rbac-state/activation.state "$testroot/activation.state"
awk -v root="$testroot" '
/^[a-zA-Z_]+:/ { section=$0 }
section == "mms:" && $1 == "tcp_port:" { $0="  tcp_port: 3783" }
section == "mms:" && $1 == "comms_loss_fallback_s:" { $0="  comms_loss_fallback_s: 0" }
section == "mms:" && $1 == "rbac_policy:" { $0="  rbac_policy: " root "/package/commissioning-policy.xml" }
section == "mms:" && $1 == "rbac_activation_state:" { $0="  rbac_activation_state: " root "/activation.state" }
section == "mms:" && $1 == "model_cfg:" { next }
section == "event_log:" && $1 == "path:" { $0="  path: " root "/events.jsonl" }
{ print }
' /etc/ccli/lab.yaml > "$testroot/server.yaml"
# Startup POLICY_ACTIVATED fills line 2048. The first TLS event must rotate
# the log while serving that connection; this fixture never touches live logs.
awk 'BEGIN {for (i=0;i<2047;i++) printf "{\"ts_ms\":%d,\"type\":\"test\",\"detail\":\"seed-%d\"}\n",i,i}' > "$testroot/events.jsonl"
wc -l "$testroot/events.jsonl"
echo ISOLATED_CONFIG_PREPARED

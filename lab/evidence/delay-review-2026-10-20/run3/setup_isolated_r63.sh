#!/bin/sh
set -eu
umask 077
stage=/tmp/ccli-tsp-r63-20261020
testroot=/etc/ccli/rbac-test-20261016/tsp-r63-20261020
test ! -e "$testroot"
if netstat -lnt 2>/dev/null | grep ':3783 .*LISTEN' >/dev/null; then
    echo 'Isolated port in use' >&2; exit 1
fi
test "$(sha256sum "$stage/ccli-r63" | cut -d ' ' -f 1)" = cd1b233b41c8d5360846ec447458fb1fbb599d9eaf8e57c8b162a1c2cdfda2de
test "$(sha256sum "$stage/mms-rbac-device-server-r63" | cut -d ' ' -f 1)" = 608875c933a75b8fb920b7f0ea69af5a7cdd258b7ad5caaeec68f2b27579cd8b
chmod 755 "$stage/ccli-r63" "$stage/mms-rbac-device-server-r63"
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
"$stage/ccli-r63" --version
echo ISOLATED_R63_PREPARED_LIVE_SERVICE_UNCHANGED

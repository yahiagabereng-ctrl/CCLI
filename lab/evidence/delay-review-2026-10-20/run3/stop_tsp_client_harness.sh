pid=$(cat /tmp/ccli-tsp-r63-20261020/tsp-client-harness.pid)
if test -r /proc/$pid/cmdline && tr '\000' ' ' < /proc/$pid/cmdline | grep 'mms-rbac-device-server-r63 /etc/ccli/rbac-test-20261016/tsp-client-20261020/server.yaml' >/dev/null; then kill -TERM "$pid"; fi
netstat -lnt | grep 378
sha256sum /usr/sbin/ccli

#!/bin/sh
set -eu
test "$(sha256sum /usr/sbin/ccli | cut -d ' ' -f1)" = d336618c3e6eddc1c2661b8a2715fdce775c9e9138d1c5684e1ae5be17d81616
if [ "$(cat /proc/25877/comm 2>/dev/null)" = logread ] &&
   [ "$(awk '/^PPid:/ {print $2}' /proc/25877/status)" = 1 ]; then kill 25877; fi
sleep 1
test "$(pidof logread | wc -w)" = 1
collector=$(pidof logread)
/usr/sbin/ccli --config /etc/ccli/lab.yaml --event-dump --count 2 --json > /tmp/r74-readout.json
sleep 3
test "$(pidof logread)" = "$collector"
grep 'CYBSECEV_LOG_READ_SUCCESS' /etc/ccli/security-events/events.jsonl | tail -1
echo READOUT_ONE_COLLECTOR_PASS
/etc/init.d/ccli stop
attempt=0
while pidof ccli >/dev/null && [ "$attempt" -lt 20 ]; do sleep 1; attempt=$((attempt + 1)); done
test -z "$(pidof ccli || true)"
sleep 1
if [ -e "/proc/$collector" ]; then
    /etc/init.d/ccli start
    echo COLLECTOR_CLEANUP_FAIL
    exit 1
fi
echo COLLECTOR_PARENT_EXIT_PASS
/etc/init.d/ccli start
sleep 3
test "$(pidof logread | wc -w)" = 1
main=$(pidof ccli)
child=$(pidof logread)
test "$(awk '/^PPid:/ {print $2}' /proc/$child/status)" = "$main"
for fd in /proc/$main/fd/*; do readlink "$fd" || true; done | grep '^socket:' > /tmp/r74-parent-sockets
for fd in /proc/$child/fd/*; do
    target=$(readlink "$fd" || true)
    case "$target" in socket:*)
        if grep -Fx "$target" /tmp/r74-parent-sockets; then echo SOCKET_INHERITANCE_FAIL; exit 1; fi;;
    esac
done
echo NO_INHERITED_CCI_SOCKETS_PASS
ps w | grep '[l]ogread -f'
netstat -lnt | grep ':3782'
test ! -e /etc/ccli-upgrade.pending
test ! -e /etc/ccli-upgrade.hold
echo R74_LOGGER_VERIFICATION_PASS

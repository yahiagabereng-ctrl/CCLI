#!/bin/sh
# TR400 Phase 1 — DIO smoke test (run ON the device via SSH)
# DIO-1 = curtailment relay (DO), DIO-2 = permissive (DI)

set -e

echo "=== Configure DIO-2 as DI (UCI + ubus set_mode + gd32.set_mode) ==="
uci set didoservice_v2.channel_2.mode=di
uci commit didoservice_v2
/etc/init.d/didoservice_v2 restart
sleep 2
ubus call dido_v2 set_mode '{"channel":2,"mode":"DI"}'
ubus call dido_v2 gd32.set_mode '{"channel":2,"mode":"DI"}'
sleep 1

echo "=== Channel status ==="
ubus call dido_v2 status

echo ""
echo "=== DIO-1 relay blink x3 (listen for click if wired) ==="
i=1
while [ "$i" -le 3 ]; do
	echo "  pulse $i ON"
	ubus call dido_v2 set_relay '{"channel":1,"value":1}'
	sleep 1
	echo "  pulse $i OFF"
	ubus call dido_v2 set_relay '{"channel":1,"value":0}'
	sleep 1
	i=$((i + 1))
done

echo ""
echo "=== Read DIO-2 permissive (jumper DIO-2 to GND → state 1) ==="
ubus call dido_v2 read_di '{"channel":2}'
ubus call dido_v2 gd32.read_di '{}'

echo ""
echo "=== PASS if relay clicked and read_di returns state 0/1 when toggled ==="

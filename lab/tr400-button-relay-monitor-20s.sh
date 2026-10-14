#!/bin/sh
# Monitor DIO1 relay + DIO2 permissive for 20s (run on TG544 while operator holds button).
echo "=== 20s monitor — hold permissive (DIO2); listen for relay (DIO1) ==="
echo "sec  ch1_state  di[1]  note"
i=0
while [ "$i" -lt 20 ]; do
	ch1=$(ubus call dido_v2 status 2>/dev/null | grep -A5 '"channel": 1' | grep '"state"' | head -1 | grep -o '[0-9]')
	# gd32.read_di returns multi-line JSON; di[1] = ubus channel 2 (DIO2)
	di1=$(ubus call dido_v2 gd32.read_di '{}' 2>/dev/null | tr -d '\n ' | sed 's/.*"di":\[\([^]]*\)\].*/\1/' | cut -d, -f2 | tr -cd '0-9')
	note="open/BLOCKED"
	[ "$di1" = "1" ] && note="closed/OK"
	printf "%2d   %s         %s     %s\n" "$i" "${ch1:-?}" "${di1:-?}" "$note"
	i=$((i + 1))
	sleep 1
done
echo "=== ubus relay tick (listen for click) ==="
ubus call dido_v2 set_relay '{"channel":1,"value":0}'
sleep 1
ubus call dido_v2 set_relay '{"channel":1,"value":1}'
sleep 1
ubus call dido_v2 set_relay '{"channel":1,"value":0}'
echo "=== ccli io last lines ==="
logread 2>/dev/null | grep -E 'curtail_do=|permissive=' | tail -3
echo "=== done ==="

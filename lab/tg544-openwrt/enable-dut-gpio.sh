#!/bin/sh
# Enable TG544 GD32 DIO for CCLI curtail (DIO1) + permissive (DIO2).
# Run on DUT after deploy or before P5 / Wlim actuation (U-P5-01 DIO trace).
set -eu

echo "=== enable-dut-gpio: didoservice_v2 ==="
/etc/init.d/didoservice_v2 enable 2>/dev/null || true
/etc/init.d/didoservice_v2 restart 2>/dev/null || /etc/init.d/didoservice_v2 start
sleep 2

echo "=== UCI channel modes (ch1 DO, ch2 DI, ch3/4 DO lab) ==="
uci set didoservice_v2.channel_1.mode=do 2>/dev/null || true
uci set didoservice_v2.channel_2.mode=di 2>/dev/null || true
uci set didoservice_v2.channel_3.mode=do 2>/dev/null || true
uci set didoservice_v2.channel_4.mode=do 2>/dev/null || true
uci set didoservice_v2.channel_1.enabled=1 2>/dev/null || true
uci set didoservice_v2.channel_2.enabled=1 2>/dev/null || true
uci set didoservice_v2.channel_3.enabled=1 2>/dev/null || true
uci set didoservice_v2.channel_4.enabled=1 2>/dev/null || true
uci commit didoservice_v2 2>/dev/null || true
/etc/init.d/didoservice_v2 restart
sleep 2

echo "=== ubus set_mode (DI needs gd32.set_mode on TG544) ==="
ubus call dido_v2 set_mode '{"channel":1,"mode":"DO"}' 2>/dev/null || true
ubus call dido_v2 set_mode '{"channel":2,"mode":"DI"}' 2>/dev/null || true
ubus call dido_v2 set_mode '{"channel":3,"mode":"DO"}' 2>/dev/null || true
ubus call dido_v2 set_mode '{"channel":4,"mode":"DO"}' 2>/dev/null || true
ubus call dido_v2 gd32.set_mode '{"channel":1,"mode":"DO"}' 2>/dev/null || true
ubus call dido_v2 gd32.set_mode '{"channel":2,"mode":"DI"}' 2>/dev/null || true
ubus call dido_v2 gd32.set_mode '{"channel":3,"mode":"DO"}' 2>/dev/null || true
sleep 1

echo "=== gd32.status ==="
ubus call dido_v2 gd32.status '{}'

echo "=== dido_v2 status (expect ch1 DO, ch2 DI) ==="
ubus call dido_v2 status

echo "=== lab.yaml io (permissive_bypass must be false for cert actuation) ==="
grep -E 'permissive_bypass|do_curtail|di_permissive|gpio:|active_' /etc/ccli/lab.yaml 2>/dev/null | head -10 || echo "WARN: no /etc/ccli/lab.yaml"

echo "=== restart ccli (reload HAL + svc_io) ==="
/etc/init.d/ccli restart 2>/dev/null || {
	killall ccli 2>/dev/null || true
	sleep 1
	/usr/sbin/ccli --config /etc/ccli/lab.yaml >>/tmp/ccli.log 2>&1 &
	sleep 3
}

echo "=== ccli GPIO init lines ==="
logread 2>/dev/null | grep -iE 'hal_gpio_ll|svc_io:' | tail -5 || tail -30 /tmp/ccli.log | grep -iE 'hal_gpio|svc_io' || true

echo "=== enable-dut-gpio: done ==="

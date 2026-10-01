#!/bin/sh
# TR400 Phase 1 — verify DIO1=ubus ch1 (DO) and DIO2=ubus ch2 (DI)
# Run on TG544: sh tr400-io-verify.sh

set -e

echo "=== dido_v2 status (expect ch1 DO, ch2 DI) ==="
ubus call dido_v2 status 2>/dev/null || { echo "FAIL: dido_v2 unavailable"; exit 1; }

echo ""
echo "=== DO channel 1 (DIO1): OFF -> ON -> OFF ==="
ubus call dido_v2 set_relay '{"channel":1,"value":0}'
sleep 1
ubus call dido_v2 set_relay '{"channel":1,"value":1}'
echo "    Relay should be ON / DIO1 ~0V vs GND — press Enter when checked"
read _r
ubus call dido_v2 set_relay '{"channel":1,"value":0}'
echo "    Relay should be OFF"

echo ""
echo "=== DI channel 2 (DIO2): gd32.read_di (di[1] = ch2) ==="
echo "    Open permissive key, then Enter"
read _r
ubus call dido_v2 gd32.read_di '{}'
echo "    Close key (12V on DIO2), then Enter"
read _r
ubus call dido_v2 gd32.read_di '{}'

echo ""
echo "=== lab.yaml gpio lines (if deployed) ==="
if [ -f /etc/ccli/lab.yaml ]; then
	grep -E 'gpio:|active_|permissive_bypass' /etc/ccli/lab.yaml | head -20
else
	echo "    /etc/ccli/lab.yaml not found — copy from apps/ccli/config/lab_tr400.yaml"
fi

echo ""
echo "Done. See lab/TR400_HW_IO_CONFIG.md for pass criteria."

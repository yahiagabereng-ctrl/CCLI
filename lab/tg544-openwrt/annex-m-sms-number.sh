#!/bin/sh
# Print modem MSISDN hints for Annex M lab SMS (send TRIP / CLEAR to this number).
set -u

AT_REQ="/usr/lib/sim-manager/at-req.sh"

echo "=== Annex M lab SMS target ==="
echo "Send from your mobile:"
echo "  TRIP   → telescatto ON  (DIO3, inhibits PF2 DIO1)"
echo "  CLEAR  → telescatto OFF"
echo ""

if [ ! -x "$AT_REQ" ]; then
	echo "WARN: $AT_REQ not found"
	exit 1
fi

echo "--- AT+CNUM (own number) ---"
"$AT_REQ" 10 'AT+CNUM' 2>&1

echo ""
echo "--- SIM ubus (if available) ---"
ubus call sim-manager status '{}' 2>/dev/null | head -40 || true

echo ""
echo "Log: /tmp/annex-m-sms.log"
echo "Poller: /usr/sbin/annex-m-sms-poller.sh (cron every 30 s when installed)"

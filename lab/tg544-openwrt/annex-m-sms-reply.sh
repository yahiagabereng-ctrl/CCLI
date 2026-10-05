#!/bin/sh
# Build and send device ack after DIO3 action.
# Usage: annex-m-sms-reply.sh <on|off|err> [sender_msisdn] [detail]
#
# Always writes /tmp/annex-m-sms-last-reply.txt (reliable).
# Best-effort MO-SMS via annex-m-sms-send.sh (1NCE Portal MO tab).
set -u

ACTION="${1:-}"
SENDER="${2:-}"
DETAIL="${3:-}"
SEND="/usr/sbin/annex-m-sms-send.sh"
LOG="/tmp/annex-m-sms.log"
ACK="/tmp/annex-m-sms-last-reply.txt"

read_dio3_state() {
	local s
	s="$(ubus call dido_v2 status 2>/dev/null | awk '
		/"channel": 3,/ { show=1 }
		show && /"state":/ { gsub(/[^0-9]/,"",$2); print $2; exit }
	' | head -1)"
	[ -n "$s" ] || s="?"
	echo "$s"
}

ch3="$(read_dio3_state)"
ts="$(date -Iseconds)"

case "$ACTION" in
on)  MSG="CCLI OK TRIP DIO3=${ch3}" ;;
off) MSG="CCLI OK CLEAR DIO3=${ch3}" ;;
err) MSG="CCLI ERR ${DETAIL:-unknown cmd} DIO3=${ch3}" ;;
*)   MSG="CCLI OK DIO3=${ch3}" ;;
esac

echo "${ts} ${MSG}" >"$ACK"
echo "${ts} sms_ack: ${MSG}" >>"$LOG"

if [ -x "$SEND" ]; then
	# Background MO-SMS; send script has bounded AT read timeouts (~40s max).
	"$SEND" "${SENDER:--}" "$MSG" >>"$LOG" 2>&1 &
fi

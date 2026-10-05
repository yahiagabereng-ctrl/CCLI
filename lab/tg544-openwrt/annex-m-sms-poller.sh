#!/bin/sh
# P6 lab — poll modem SMS via TesPro sim-manager AT and drive DIO3 telescatto relay.
# Sends MO-SMS ack after DO change (1NCE Portal MO tab / reply to sender if known).
#
# SMS commands (1NCE Portal MT-SMS):
#   TRIP   → telescatto ON  + reply "CCLI OK TRIP DIO3=1"
#   CLEAR  → telescatto OFF + reply "CCLI OK CLEAR DIO3=0"
set -u

AT_REQ="/usr/lib/sim-manager/at-req.sh"
TRIP_SCRIPT="/usr/sbin/annex-m-trip.sh"
REPLY_SCRIPT="/usr/sbin/annex-m-sms-reply.sh"
LOG="/tmp/annex-m-sms.log"
LOCK="/var/run/annex-m-sms-poller.lock"
STATE="/var/run/annex-m-sms-last.cmd"

log() {
	echo "$(date -Iseconds) $*" >>"$LOG"
}

run_at() {
	"$AT_REQ" "$1" "$2" 2>&1
}

normalize_cmd() {
	echo "$1" | tr -d '\r' | sed 's/^[[:space:]]*//;s/[[:space:]]*$//' | tr 'a-z' 'A-Z'
}

map_sms_to_action() {
	case "$(normalize_cmd "$1")" in
	TRIP | ON | SCATTO | TELESCATTO | "CCLI TRIP") echo "on" ;;
	CLEAR | OFF | RESET | "CCLI CLEAR") echo "off" ;;
	*) echo "" ;;
	esac
}

parse_cmgl_sender() {
	echo "$1" | sed -n 's/^+CMGL: [0-9]*,"[^"]*","\([^"]*\)".*/\1/p'
}

apply_trip_action() {
	local action="$1"
	if [ -x "$TRIP_SCRIPT" ]; then
		"$TRIP_SCRIPT" "$action"
	else
		if [ "$action" = "on" ]; then
			ubus call dido_v2 set_relay '{"channel":3,"value":1}'
		else
			ubus call dido_v2 set_relay '{"channel":3,"value":0}'
		fi
	fi
}

send_reply() {
	local action="$1"
	local sender="$2"
	local detail="$3"
	[ -x "$REPLY_SCRIPT" ] || return 0
	"$REPLY_SCRIPT" "$action" "$sender" "$detail" &
}

process_cmgl_output() {
	local raw="$1"
	local idx=""
	local body=""
	local sender=""
	local action=""
	local last=""
	local line

	[ -f "$STATE" ] && last="$(cat "$STATE" 2>/dev/null || true)"

	while IFS= read -r line || [ -n "$line" ]; do
		case "$line" in
		+CMGL:*)
			idx="$(echo "$line" | sed -n 's/^+CMGL: \([0-9]*\),.*/\1/p')"
			sender="$(parse_cmgl_sender "$line")"
			body=""
			;;
		OK | ERROR | +CME\ ERROR:* | +CMS\ ERROR:*)
			;;
		"")
			;;
		*)
			if [ -n "$idx" ] && [ -z "$body" ]; then
				body="$line"
				action="$(map_sms_to_action "$body")"
				if [ -n "$action" ]; then
					tag="${idx}:${action}:${body}"
					if [ "$last" != "$tag" ]; then
						apply_trip_action "$action"
						echo "$tag" >"$STATE"
						log "SMS idx=$idx from='${sender:-?}' body='$body' → trip $action"
						send_reply "$action" "$sender" ""
						last="$tag"
					fi
					run_at 5 "AT+CMGD=${idx}" >/dev/null || true
				else
					log "SMS idx=$idx from='${sender:-?}' ignored body='$body'"
					send_reply "err" "$sender" "$body"
					run_at 5 "AT+CMGD=${idx}" >/dev/null || true
				fi
				idx=""
				body=""
				sender=""
			fi
			;;
		esac
	done <<EOF
$raw
EOF
}

if [ ! -x "$AT_REQ" ]; then
	log "FAIL: $AT_REQ missing"
	exit 1
fi

if ! mkdir "$LOCK" 2>/dev/null; then
	exit 0
fi
trap 'rmdir "$LOCK" 2>/dev/null || true' EXIT INT TERM

run_at 5 'AT+CMGF=1' >/dev/null || true

raw="$(run_at 20 'AT+CMGL="REC UNREAD"')"
if echo "$raw" | grep -q '+CMGL:'; then
	process_cmgl_output "$raw"
fi

raw_all="$(run_at 20 'AT+CMGL="ALL"')"
if echo "$raw_all" | grep -q '+CMGL:'; then
	process_cmgl_output "$raw_all"
fi

exit 0

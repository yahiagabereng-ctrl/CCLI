#!/bin/sh
# P6 lab — best-effort MO-SMS (1NCE Portal MO tab). See annex-m-sms-last-reply.txt for reliable ack.
# Usage: annex-m-sms-send.sh <dest|-> <message>
set -u

DEST="${1:--}"
MSG="${2:-}"
LOG="/tmp/annex-m-sms.log"
SM_SVC="/etc/init.d/sim-manager"
SMSC="+882285000016868"

log() {
	echo "$(date -Iseconds) sms_tx: $*" >>"$LOG"
}

[ -n "$MSG" ] || exit 1
[ -z "$DEST" ] || [ "$DEST" = "-" ] && DEST="$SMSC"

. /usr/lib/sim-manager/common.sh 2>/dev/null || true

PORT=""
[ -f /tmp/sim_manager/at_broker.json ] &&
	PORT="$(sed -n 's/.*"at_port"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' \
		/tmp/sim_manager/at_broker.json | head -1)"
[ -n "$PORT" ] || PORT="/dev/ttyUSB2"

# Must own AT port for CMGS prompt handling.
if [ -x "$SM_SVC" ]; then
	"$SM_SVC" stop >/dev/null 2>&1 || true
	sleep 1
fi

sm_at_set_raw "$PORT" 2>/dev/null || true
exec 3<>"$PORT" 2>/dev/null || {
	log "FAIL open $PORT"
	[ -x "$SM_SVC" ] && "$SM_SVC" start >/dev/null 2>&1 || true
	exit 1
}

read_line() {
	local line=""
	IFS= read -r -t 2 line <&3 2>/dev/null || return 1
	printf '%s' "$line" | tr -d '\r'
}

drain() {
	local n=0
	while [ "$n" -lt 8 ]; do
		read_line >/dev/null || break
		n=$((n + 1))
	done
}

send_at() {
	printf '%s\r' "$1" >&3
}

wait_for() {
	local want="$1" step="$2" n=0 line
	while [ "$n" -lt 20 ]; do
		line="$(read_line)" || {
			n=$((n + 1))
			continue
		}
		case "$line" in
		OK)
			[ "$want" = OK ] && return 0
			;;
		">" | *">")
			[ "$want" = PROMPT ] && return 0
			;;
		+CMGS:* | +CMSS:*)
			[ "$want" = SENT ] && return 0
			;;
		ERROR | +CME\ ERROR:* | +CMS\ ERROR:*)
			log "FAIL $step: $line"
			return 1
			;;
		esac
	done
	log "FAIL $step: timeout"
	return 1
}

rc=1
drain
send_at "AT+CMGF=1"
if wait_for OK CMGF; then
	send_at "AT+CMGS=\"${DEST}\""
	if wait_for PROMPT CMGS; then
		printf '%s\x1a' "$MSG" >&3
		if wait_for SENT CMGS_SEND && wait_for OK CMGS_OK; then
			rc=0
		fi
	fi
fi

exec 3>&-
[ -x "$SM_SVC" ] && "$SM_SVC" start >/dev/null 2>&1 || true

[ "$rc" -eq 0 ] && log "OK MO-SMS to '$DEST' body='$MSG'" || log "FAIL MO-SMS to '$DEST'"
exit "$rc"

#!/bin/sh
# P6 lab — drive Annex M telescatto relay on DIO3 (ubus ch3).
# Same coil wiring as DIO1 curtail (12 V + flyback). External LTE/SMS handler
# may call this script when a trip command is received.
#
# Usage on DUT:
#   annex-m-trip.sh on|off|status
set -u

CH=3

case "${1:-}" in
on|1|trip)
	ubus call dido_v2 set_relay "{\"channel\":${CH},\"value\":1}"
	echo "annex-m-trip: DIO${CH} ON (telescatto active)"
	[ -x /usr/sbin/annex-m-sms-reply.sh ] && /usr/sbin/annex-m-sms-reply.sh on "" "" &
	;;
off|0|clear)
	ubus call dido_v2 set_relay "{\"channel\":${CH},\"value\":0}"
	echo "annex-m-trip: DIO${CH} OFF (telescatto cleared)"
	[ -x /usr/sbin/annex-m-sms-reply.sh ] && /usr/sbin/annex-m-sms-reply.sh off "" "" &
	;;
status)
	ubus call dido_v2 status
	;;
*)
	echo "Usage: $0 on|off|status" >&2
	exit 1
	;;
esac

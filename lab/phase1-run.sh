#!/bin/sh
# Phase 1 — run CCLI without APK / init script (binary-only deploy).
# Usage on TG544:
#   sh phase1-run.sh              # Modbus RTU from /etc/ccli/lab.yaml
#   sh phase1-run.sh --lab-demo   # open-loop PF2 test (no PC slave)
#   sh phase1-run.sh --lab-demo --foreground

set -e
CFG="${CCLI_CONFIG:-/etc/ccli/lab.yaml}"
BIN="${CCLI_BIN:-/usr/sbin/ccli}"
FOREGROUND=0
EXTRA=""

for arg in "$@"; do
	case "$arg" in
	--foreground|-f) FOREGROUND=1 ;;
	*) EXTRA="$EXTRA $arg" ;;
	esac
done

if [ ! -x "$BIN" ]; then
	echo "Missing $BIN — copy binary: pscp ccli-bin root@192.168.1.130:/usr/sbin/ccli"
	exit 1
fi
if [ ! -f "$CFG" ]; then
	echo "Missing $CFG"
	exit 1
fi

killall ccli 2>/dev/null || true

echo "=== CCLI Phase 1 (no APK required) ==="
echo "Binary: $BIN"
echo "Config: $CFG"
echo "Extra:  $EXTRA"
echo "Tip: short DIO2→GND for permissive; PC slave for RTU mode"
echo ""

if [ "$FOREGROUND" -eq 1 ]; then
	exec "$BIN" --config "$CFG" $EXTRA
fi

"$BIN" --config "$CFG" $EXTRA >> /tmp/ccli.log 2>&1 &
echo "Started PID $! — log: /tmp/ccli.log"
sleep 2
tail -10 /tmp/ccli.log

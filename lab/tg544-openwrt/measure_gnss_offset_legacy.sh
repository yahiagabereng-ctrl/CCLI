#!/bin/sh
# Legacy BusyBox measure (1 s resolution — up to ±999 ms quantization).
# Prefer /tmp/measure_gnss_offset (ms C tool) for T.3.3.4.5.
set -eu
SYS_BEFORE=$(date -u +%s)
JSON=$(ubus call sim-manager gnss_get_position 2>/dev/null || true)
SYS_AFTER=$(date -u +%s)
SYS_MID=$(( (SYS_BEFORE + SYS_AFTER) / 2 ))

RMC=$(printf '%s' "$JSON" | tr '\n' ' ' | sed -n 's/.*\$GNRMC,\([0-9.]*\),\([AV]\),.*,\([0-9]\{6\}\),.*/\1 \2 \3/p')
GGA=$(printf '%s' "$JSON" | tr '\n' ' ' | sed -n 's/.*\$G[NP]GGA,\([0-9.]*\),.*/\1/p')
set -- $RMC
HHMMSS=${1:-}
STATUS=${2:-}
DDMMYY=${3:-}
if [ -n "$GGA" ]; then
  HHMMSS=$GGA
fi

echo "sys_before=$SYS_BEFORE"
echo "sys_after=$SYS_AFTER"
echo "sys_mid=$SYS_MID"
echo "rmc_raw=$RMC gga=$GGA"
echo "status=$STATUS hhmmss=$HHMMSS ddmmyy=$DDMMYY"

if [ -z "$HHMMSS" ] || [ "$STATUS" != "A" ]; then
  echo "VERDICT: FAIL — no valid RMC fix"
  exit 2
fi

HH=$(echo "$HHMMSS" | cut -c1-2)
MM=$(echo "$HHMMSS" | cut -c3-4)
SS=$(echo "$HHMMSS" | cut -c5-6)
FRAC=$(echo "$HHMMSS" | cut -d. -f2)
FRAC=${FRAC:-00}
case ${#FRAC} in
  1) MS=$((FRAC * 100)) ;;
  2) MS=$((FRAC * 10)) ;;
  *) MS=$FRAC; MS=$((MS % 1000)) ;;
esac

DD=$(echo "$DDMMYY" | cut -c1-2)
MO=$(echo "$DDMMYY" | cut -c3-4)
YY=$(echo "$DDMMYY" | cut -c5-6)
YYYY=$((2000 + YY))

GNSS_EPOCH=$(date -u -d "$YYYY-$MO-$DD $HH:$MM:$SS" +%s 2>/dev/null || true)
if [ -z "$GNSS_EPOCH" ]; then
  GNSS_EPOCH=$(date -u -D "%Y-%m-%d %H:%M:%S" -d "$YYYY-$MO-$DD $HH:$MM:$SS" +%s 2>/dev/null || true)
fi
if [ -z "$GNSS_EPOCH" ]; then
  echo "VERDICT: FAIL — cannot parse GNSS calendar time"
  exit 3
fi

GNSS_MS=$((GNSS_EPOCH * 1000 + MS))
OFFSET_MS=$(( SYS_AFTER * 1000 - GNSS_MS ))
ABS=$OFFSET_MS
[ "$ABS" -lt 0 ] && ABS=$((-ABS))

echo "gnss_utc=${YYYY}-${MO}-${DD}T${HH}:${MM}:${SS}.${FRAC}Z"
echo "gnss_epoch_ms=$GNSS_MS"
echo "sys_after_ms=$((SYS_AFTER * 1000))"
echo "ubus_rtt_s=$((SYS_AFTER - SYS_BEFORE))"
echo "offset_ms=$OFFSET_MS  (sys - gnss; + means system ahead)"
echo "abs_offset_ms=$ABS"
echo "limit_ms=100"
echo "NOTE: BusyBox date is 1 s resolution — use measure_gnss_offset binary for ±100 ms"

if [ "$ABS" -le 100 ]; then
  echo "VERDICT: PASS — |offset|<=100 ms (T.3.3.4.5)"
  exit 0
else
  echo "VERDICT: FAIL — |offset|=$ABS ms > 100 ms"
  exit 1
fi

#!/bin/sh
# Wrapper: prefer aarch64 ms tool; fall back to second-resolution BusyBox script.
set -eu
DIR=$(dirname "$0")
if [ -x "$DIR/measure_gnss_offset" ]; then
  exec "$DIR/measure_gnss_offset"
fi
if [ -x /tmp/measure_gnss_offset ]; then
  exec /tmp/measure_gnss_offset
fi
# Legacy second-resolution path (BUSYBOX) — quantization up to ±999 ms.
exec sh /tmp/measure_gnss_offset_legacy.sh 2>/dev/null || {
  echo "VERDICT: FAIL — deploy measure_gnss_offset (ms) binary" >&2
  exit 3
}

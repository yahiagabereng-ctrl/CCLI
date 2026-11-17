#!/usr/bin/env bash
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
repo="$(cd "$here/../../../.." && pwd)"
lib="$repo/apps/ccli/third_party/libiec61850"
build="$repo/apps/ccli/build-mms-client"
work=$(mktemp -d /tmp/ccli-run3-parser.XXXXXX)
inc=( -I "$build/generated/dual-certificate" -I "$build/third_party/libiec61850/config"
      -I "$lib/src/common/inc" -I "$lib/src/mms/inc" -I "$lib/src/mms/inc_private"
      -I "$lib/hal/inc" )
gcc "${inc[@]}" "$here/replay_presentation.c" \
    "$build/third_party/libiec61850/src/libiec61850.a" \
    "$build/third_party/libiec61850/hal/libhal.a" -lpthread -lm -o "$work/original"
set +e
"$work/original" "$here/captured-presentation.bin"
original=$?
set -e
test "$original" -eq 1
# Change the generated parser's recognized syntax only, inside a disposable
# offline build. The captured request and actual application sources are intact.
python3 - "$build/generated/dual-certificate/iso_presentation.c" "$work/corrected.c" <<'PY'
from pathlib import Path
import sys
text = Path(sys.argv[1]).read_text()
old = '{0x28,0x86,0x48,0x00,0x00,0x01,0x01,0x04,0x01}'
new = '{0x28,0x86,0x48,0x00,0x01,0x01,0x04,0x01}'
assert text.count(old) == 1
Path(sys.argv[2]).write_text(text.replace(old,new))
PY
gcc "${inc[@]}" -I "$lib/src/logging" -include "$build/generated/dual-certificate/iso_presentation.h" \
    "$here/replay_presentation.c" "$work/corrected.c" \
    "$build/third_party/libiec61850/src/libiec61850.a" \
    "$build/third_party/libiec61850/hal/libhal.a" -lpthread -lm -o "$work/corrected"
"$work/corrected" "$here/captured-presentation.bin"
echo OFFLINE_OID_MISMATCH_CONFIRMED

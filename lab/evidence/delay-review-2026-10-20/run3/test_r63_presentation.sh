#!/usr/bin/env bash
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
repo="$(cd "$here/../../../.." && pwd)"
lib="$repo/apps/ccli/third_party/libiec61850"
old="$repo/apps/ccli/build-mms-client"
work=$(mktemp -d /tmp/ccli-r63-presentation.XXXXXX)
python3 "$repo/scripts/generate_mms_certificate_auth.py" --vendor "$lib" --output "$work/generated"
inc=( -I "$work/generated" -I "$old/third_party/libiec61850/config"
      -I "$lib/src/common/inc" -I "$lib/src/mms/inc" -I "$lib/src/mms/inc_private"
      -I "$lib/hal/inc" -I "$lib/src/logging" )
gcc "${inc[@]}" -include "$work/generated/iso_presentation.h" \
    "$repo/apps/ccli/tests/unit/mms_presentation_context_probe.c" "$work/generated/iso_presentation.c" \
    "$old/third_party/libiec61850/src/libiec61850.a" \
    "$old/third_party/libiec61850/hal/libhal.a" -lpthread -lm -o "$work/probe"
python3 "$repo/apps/ccli/tests/unit/mms_presentation_context_test.py" "$work/probe" "$here/captured-presentation.bin" | tee "$here/r63-presentation-test.txt"

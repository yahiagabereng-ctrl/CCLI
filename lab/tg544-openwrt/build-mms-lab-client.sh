#!/usr/bin/env bash
# Build MMS lab clients against host libiec61850 (WSL), including TLS client.
set -eu
REPO="/mnt/c/Yahia/projects/CCLI"
LIB="$REPO/apps/ccli/third_party/libiec61850"
BUILD="$REPO/apps/ccli/build-mms-host"
OUTDIR="$REPO/lab/tg544-openwrt"

mkdir -p "$BUILD"
cmake -S "$REPO/apps/ccli" -B "$BUILD" \
  -DBUILD_PLATFORM=tg500 -DCCLI_BUILD_TESTS=OFF -DCCLI_WITH_LIBIEC61850=ON
cmake --build "$BUILD" --target iec61850 -j"$(nproc)"

IEC_A="$BUILD/third_party/libiec61850"
IEC_B="$BUILD/libiec61850"
if [ -d "$IEC_A/config" ]; then
  IEC_BIN="$IEC_A"
elif [ -d "$IEC_B/config" ]; then
  IEC_BIN="$IEC_B"
else
  echo "Cannot find libiec61850 build config dir"
  exit 1
fi

HAL_A=$(find "$BUILD" -name 'libhal.a' | head -1)
LIB_A=$(find "$BUILD" -name 'libiec61850.a' | head -1)

build_one() {
  local src="$1"
  local out="$2"
  gcc -O2 -Wall \
    -I"$LIB/src/iec61850/inc" \
    -I"$LIB/src/common/inc" \
    -I"$LIB/src/mms/inc" \
    -I"$LIB/src/logging" \
    -I"$LIB/hal/inc" \
    -I"$IEC_BIN/config" \
    -DCONFIG_MMS_SUPPORT_TLS=1 \
    "$src" \
    "$LIB_A" "$HAL_A" \
    -lpthread -lm -lrt \
    -o "$out"
  echo "BUILT $out"
  file "$out"
}

build_one "$REPO/lab/mms_lab_client.c" "$OUTDIR/mms_lab_client"
build_one "$REPO/lab/mms_wlim_client.c" "$OUTDIR/mms_wlim_client"
build_one "$REPO/lab/mms_tls_client.c" "$OUTDIR/mms_tls_client"
build_one "$REPO/lab/mms_tls_noclient.c" "$OUTDIR/mms_tls_noclient"
build_one "$REPO/lab/mms_tls_wlim_client.c" "$OUTDIR/mms_tls_wlim_client"
build_one "$REPO/lab/mms_timeq_client.c" "$OUTDIR/mms_timeq_client"
build_one "$REPO/lab/mms_ln_browser.c" "$OUTDIR/mms_ln_browser"
build_one "$REPO/lab/mms_varsd_client.c" "$OUTDIR/mms_varsd_client"

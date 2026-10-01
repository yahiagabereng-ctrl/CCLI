#!/usr/bin/env bash
set -eu
SDK="${HOME}/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64"
CC=$(find "$SDK/staging_dir" -path '*/bin/aarch64-openwrt-linux-musl-gcc' | head -1)
export STAGING_DIR="${CC%/bin/aarch64-openwrt-linux-musl-gcc}"
export CC
export CFLAGS="-O2"
export LDFLAGS=""
REPO="/mnt/c/Yahia/projects/CCLI"
OUT="$REPO/lab/tg544-openwrt"
BUILD="${HOME}/chrony-cross-build"
mkdir -p "$BUILD"
cd "$BUILD"
if [ ! -f chrony-4.8.tar.gz ]; then
  wget -q https://chrony-project.org/releases/chrony-4.8.tar.gz
fi
rm -rf chrony-4.8
tar xf chrony-4.8.tar.gz
cd chrony-4.8
./configure \
  --host-machine=aarch64 \
  --host-release="" \
  --host-system=Linux \
  --prefix=/usr \
  --sysconfdir=/etc/chrony \
  --chronyrundir=/var/run/chrony \
  --disable-readline \
  --disable-rtc \
  --disable-nts \
  --disable-sechash \
  --disable-privdrop \
  --without-nettle \
  --without-nss \
  --without-tomcrypt \
  --without-gnutls
make -j"$(nproc)"
cp -f chronyd chronyc "$OUT/"
chmod +x "$OUT/chronyd" "$OUT/chronyc"
file "$OUT/chronyd" "$OUT/chronyc"
"$OUT/chronyd" --help 2>&1 | head -2 || true
echo BUILT_CHRONY_OK

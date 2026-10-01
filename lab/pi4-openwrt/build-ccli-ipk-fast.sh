#!/bin/bash
# Fast ccli .ipk build — SDK toolchain + libgpiod from SDK build_dir.
set -euo pipefail

REPO="/mnt/c/Yahia/projects/CCLI"
SDK="$HOME/openwrt-sdk-24.10.5-bcm27xx-bcm2711_gcc-13.3.0_musl.Linux-x86_64"
PKG_VER="0.1.0"
PKG_REL="1"
ARCH="aarch64_cortex-a72"
OUT_DIR="${REPO}/lab/pi4-openwrt"
STAGING="${SDK}/staging_dir/target-aarch64_cortex-a72_musl"
TOOLCHAIN="${SDK}/staging_dir/toolchain-aarch64_cortex-a72_gcc-13.3.0_musl"
BUILD="${SDK}/build_dir/ccli-fast"
IPKG="${SDK}/build_dir/ccli-ipkg"
CCLI_USE_GPIOD="${CCLI_USE_GPIOD:-1}"
CCLI_USE_LIBMODBUS="${CCLI_USE_LIBMODBUS:-1}"

export PATH="${TOOLCHAIN}/bin:${SDK}/staging_dir/host/bin:${PATH}"
export STAGING_DIR="${STAGING}"

ensure_libmodbus() {
    if [ ! -f "${STAGING}/usr/lib/libmodbus.so" ] && [ ! -f "${STAGING}/usr/lib/libmodbus.so.5" ]; then
        echo "=== Building libmodbus in OpenWrt SDK (one-time) ==="
        (
            cd "${SDK}"
            if [ ! -L feeds/packages ]; then
                ./scripts/feeds update packages
                ./scripts/feeds install -p packages libmodbus
            fi
            make package/feeds/packages/libmodbus/compile V=s
        )
    fi
    if [ ! -f "${STAGING}/usr/lib/libmodbus.so" ] && [ ! -f "${STAGING}/usr/lib/libmodbus.so.5" ]; then
        echo "ERROR: libmodbus missing in ${STAGING}/usr/lib after SDK build"
        exit 1
    fi
}

ensure_libgpiod() {
    if [ ! -f "${STAGING}/usr/lib/libgpiod.so" ]; then
        echo "=== Building libgpiod in OpenWrt SDK (one-time) ==="
        (
            cd "${SDK}"
            if [ ! -L feeds/packages ]; then
                ./scripts/feeds update packages
                ./scripts/feeds install -p packages libgpiod
            fi
            make package/feeds/packages/libgpiod/compile V=s
        )
    fi
    if [ ! -f "${STAGING}/usr/lib/libgpiod.so" ]; then
        echo "ERROR: libgpiod missing in ${STAGING}/usr/lib after SDK build"
        exit 1
    fi
}

CMAKE_GPIOD=""
CMAKE_MODBUS=""
CMAKE_PREFIX="-DCMAKE_PREFIX_PATH=${STAGING};${TOOLCHAIN}"
PKG_DEPENDS="libstdcpp6"

if [ "${CCLI_USE_LIBMODBUS}" = "1" ]; then
    ensure_libmodbus
    PKG_DEPENDS="${PKG_DEPENDS}, libmodbus"
    MODBUS_SO="$(ls -1 "${STAGING}/usr/lib/libmodbus.so" "${STAGING}/usr/lib/libmodbus.so."* 2>/dev/null | head -1)"
    CMAKE_MODBUS="-DCCLI_FORCE_LIBMODBUS=ON -DCCLI_MODBUS_INCLUDE_DIR=${STAGING}/usr/include -DCCLI_MODBUS_LINK_DIR=${STAGING}/usr/lib -DCCLI_MODBUS_LIBRARY=${MODBUS_SO}"
    echo "=== Building with libmodbus (${MODBUS_SO}) ==="
else
    echo "=== Building without libmodbus link (CCLI_USE_LIBMODBUS=0) ==="
fi

if [ "${CCLI_USE_GPIOD}" = "1" ]; then
    ensure_libgpiod
    CMAKE_GPIOD="-DCCLI_FORCE_LIBGPIOD=ON"
    CMAKE_PREFIX="-DCMAKE_PREFIX_PATH=${STAGING};${TOOLCHAIN}"
    PKG_DEPENDS="${PKG_DEPENDS}, libgpiod"
    GPIOD_CMAKE="-DCCLI_GPIOD_INCLUDE_DIR=${STAGING}/usr/include -DCCLI_GPIOD_LINK_DIR=${STAGING}/usr/lib -DCCLI_GPIOD_LIBRARY=${STAGING}/usr/lib/libgpiod.so"
    echo "=== Building with libgpiod GPIO HAL ==="
else
    echo "=== Building with mock GPIO HAL (CCLI_USE_GPIOD=0) ==="
fi

mkdir -p "${BUILD}" "${IPKG}" "${OUT_DIR}"
rm -rf "${BUILD:?}"/* "${IPKG:?}"/*

cmake -S "${REPO}/apps/ccli" -B "${BUILD}" -G Ninja \
  -DCMAKE_SYSTEM_NAME=Linux \
  -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
  -DCMAKE_FIND_ROOT_PATH="${STAGING};${TOOLCHAIN}" \
  -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
  -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
  -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
  -DCMAKE_C_COMPILER="${TOOLCHAIN}/bin/aarch64-openwrt-linux-musl-gcc" \
  -DCMAKE_CXX_COMPILER="${TOOLCHAIN}/bin/aarch64-openwrt-linux-musl-g++" \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_PLATFORM=pi \
  -DCCLI_BUILD_TESTS=OFF \
  -DCMAKE_SKIP_RPATH=ON \
  ${CMAKE_GPIOD} \
  ${CMAKE_MODBUS} \
  ${CMAKE_PREFIX} \
  ${GPIOD_CMAKE:-} \

cmake --build "${BUILD}" --target ccli

ROOT="${IPKG}"
CTRL="${IPKG}/CONTROL"
mkdir -p "${ROOT}/usr/sbin" "${ROOT}/etc/init.d" "${ROOT}/etc/config" \
  "${ROOT}/etc/ccli" "${ROOT}/usr/share/ccli/config/modbus" \
  "${ROOT}/usr/share/ccli/config/icd" "${ROOT}/etc/uci-defaults" "${CTRL}"

install -m755 "${BUILD}/ccli" "${ROOT}/usr/sbin/ccli"
install -m755 "${REPO}/package/ccli/files/ccli.init" "${ROOT}/etc/init.d/ccli"
install -m644 "${REPO}/package/ccli/files/ccli.config" "${ROOT}/etc/config/ccli"
install -m644 "${REPO}/package/ccli/files/lab_openwrt.yaml" "${ROOT}/etc/ccli/lab.yaml"
install -m755 "${REPO}/package/ccli/files/99-ccli-enable" "${ROOT}/etc/uci-defaults/99-ccli-enable"
sed -i 's/\r$//' "${ROOT}/etc/init.d/ccli" "${ROOT}/etc/uci-defaults/99-ccli-enable"
cp -r "${REPO}/apps/ccli/config/modbus/." "${ROOT}/usr/share/ccli/config/modbus/"
cp -r "${REPO}/apps/ccli/config/icd/." "${ROOT}/usr/share/ccli/config/icd/"

cat > "${CTRL}/control" <<EOF
Package: ccli
Version: ${PKG_VER}-${PKG_REL}
Depends: ${PKG_DEPENDS}
Architecture: ${ARCH}
Installed-Size: 0
Description: CCLI Central Plant Controller (PF2 lab)
EOF

echo "#!/bin/sh" > "${CTRL}/postinst"
echo "[ \"\${IPKG_NO_SCRIPT}\" = \"1\" ] && exit 0" >> "${CTRL}/postinst"
echo "/etc/init.d/ccli enable" >> "${CTRL}/postinst"
echo "exit 0" >> "${CTRL}/postinst"
chmod 755 "${CTRL}/postinst"

"${SDK}/scripts/ipkg-build" "${IPKG}" "${OUT_DIR}"
IPK=$(ls -1 "${OUT_DIR}"/ccli_*.ipk | tail -1)
echo "BUILT ${IPK}"

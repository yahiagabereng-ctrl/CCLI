#!/usr/bin/env python3
from pathlib import Path

script = r"""#!/usr/bin/env bash
set -eu
REPO=/mnt/c/Yahia/projects/CCLI
SDK=$HOME/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64
OUT=/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt
cd "$SDK"
mkdir -p staging_dir/host tmp/info
touch staging_dir/host/.prereq-build
grep -q 'src-link ccli' feeds.conf 2>/dev/null || echo "src-link ccli ${REPO}/package" >> feeds.conf
./scripts/feeds update ccli
./scripts/feeds install -p ccli ccli
./scripts/feeds install -p packages libmodbus || true
make package/feeds/ccli/ccli/clean V=s 2>/dev/null || true
make package/feeds/ccli/ccli/compile V=s -j$(nproc)
make package/index || true
APK=$(ls -1 bin/packages/*/ccli/ccli-*.apk | head -1)
cp -f "$APK" "$OUT/"
echo BUILT "$OUT/$(basename "$APK")"
PKGDIR=$(find build_dir -path '*/.pkgdir/ccli/usr/sbin/ccli' 2>/dev/null | head -1)
cp -f "$PKGDIR" "$OUT/ccli-bin"
chmod +x "$OUT/ccli-bin"
file "$OUT/ccli-bin"
"""

out = Path(r"C:\Users\yahia\AppData\Local\Temp\wsl-build-ccli.sh")
out.write_bytes(script.encode("utf-8").replace(b"\r\n", b"\n"))
print("wrote", out, "bytes", out.stat().st_size)

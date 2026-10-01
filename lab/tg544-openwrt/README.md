# TG544 / TR500 — OpenWrt 25.12 SDK (MediaTek Filogic / MT798X)

Cross-compile **CCLI** `.ipk` for the lab DUT (**TG544**, TesproOS 32-2.1.0, OpenWrt **25.12**, kernel **6.12**).

**Phase 1 lab runbook:** [`../PHASE1.md`](../PHASE1.md)

| Item | Value |
|------|--------|
| OpenWrt release | **25.12.5** (latest 25.12.x patch; matches `DISTRIB_RELEASE='25.12'`) |
| Target | `mediatek/filogic` (MT7981 / MT7986 / MT7988) |
| Toolchain | `gcc-14.3.0_musl` |
| Build host | **WSL2 Ubuntu** (recommended) or native Linux x86_64 |
| Lab DUT IP | `192.168.1.130` (`lab_tr400.yaml`) |

## Quick start

```bash
# WSL — tarball cached on /mnt/c; SDK extracted under ~/ (Linux ext4)
cd /mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt
sed -i 's/\r$//' *.sh   # if cloned on Windows
chmod +x *.sh
./download-sdk.sh          # ~240 MB download + extract to ~/openwrt-sdk-...
./build-ccli-ipk-sdk.sh    # produces ccli-*.apk in this directory
```

**Important:** Do **not** extract the SDK on `C:\` — kernel tree symlinks fail on DrvFS. The scripts extract to `$HOME/openwrt-sdk-25.12.5-mediatek-filogic_*` in WSL.

Deploy: see [`DEPLOY_CCLI.md`](DEPLOY_CCLI.md).

## Upstream vs TesPro SDK

| SDK | Status | Use |
|-----|--------|-----|
| **Upstream** OpenWrt 25.12.5 `mediatek/filogic` | **HAVE** — this folder | Host cross-compile; `platform_tg500` |
| **TesPro vendor** SDK (exact TesproOS image) | **MISSING** — request V-001 | Kernel config, prebuilt libs, board DTS parity |

Upstream SDK is sufficient to build and run portable CCLI on TG544. Validate with `uname -r` and `opkg list-installed` on device if link errors appear — then request matching sysroot from TesPro.

## References

- `apps/ccli/config/lab_tr400.yaml` — TG544 runtime config
- `knowledge-base/08-engineering/CCI_OpenWrt_Freeze.md` — OS freeze
- `package/ccli/` — OpenWrt feed / Makefile

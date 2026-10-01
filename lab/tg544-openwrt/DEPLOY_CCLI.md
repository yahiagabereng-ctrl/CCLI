# Deploy CCLI on TG544 (OpenWrt 25.12 / TesproOS)

OpenWrt **25.12** uses **`apk`** packages (not `opkg`).

## Versioning (required before every upload)

| Source | Purpose |
|--------|---------|
| `apps/ccli/VERSION` | Semver + **rN** release + codename (single truth) |
| `apps/ccli/CHANGELOG.md` | Human-readable changes per release |
| `lab/tg544-openwrt/deploy-manifests/` | Per-upload manifest (SHA256, changes, checks) |
| DUT `/etc/ccli/VERSION` | Installed binary identity after deploy |

**Bump workflow:**

1. Edit `apps/ccli/VERSION` (line 2: increment **17** → **18**, etc.)
2. `.\scripts\sync-ccli-version.ps1` — syncs `package/ccli/Makefile` `PKG_RELEASE`
3. Document changes in `apps/ccli/CHANGELOG.md`
4. Build → verify `ccli-bin --version` shows `0.1.0-rN`
5. Deploy with **`-Changes`** (mandatory):

```powershell
$env:CCLI_TG544_PW = "..."
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 -HostAddr 192.168.1.130 -Changes @(
  "Describe change 1",
  "Describe change 2"
)
```

**Prerequisites:** SDK built (`build-ccli-ipk-sdk.sh` or `wsl-build-ccli.sh`), SSH to TG544.

## 1. Build host setup (WSL, one time)

```bash
sudo apt install build-essential clang flex bison g++ gawk gcc-multilib git \
  gettext libncurses-dev libssl-dev python3-distutils rsync unzip zlib1g-dev \
  file wget zstd
```

## 2. Download SDK + build

```bash
cd /mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt
chmod +x download-sdk.sh build-ccli-ipk-sdk.sh
./download-sdk.sh
./build-ccli-ipk-sdk.sh
```

Output: `ccli-0.1.0-r1.apk` in this directory.

## 3. Install on TG544

```bash
APK=/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/ccli-*.apk
scp $APK root@192.168.1.130:/tmp/
ssh root@192.168.1.130 'apk add --allow-untrusted /tmp/ccli-*.apk && /etc/init.d/ccli enable && /etc/init.d/ccli start'
```

## 4. Lab config (TG544-specific)

The default `.ipk` ships Pi-oriented `lab.yaml`. Replace with TR400 lab config:

```bash
scp /mnt/c/Yahia/projects/CCLI/apps/ccli/config/lab_tr400_phase1_regulation.yaml \
  root@192.168.1.130:/etc/ccli/lab.yaml
ssh root@192.168.1.130 '/etc/init.d/ccli enable; /etc/init.d/ccli restart'
```

## 5. Verify

```bash
ssh root@192.168.1.130 'cat /etc/openwrt_release; uname -r'
ssh root@192.168.1.130 'pgrep -a ccli; logread | grep ccli | tail -10'
ssh root@192.168.1.130 'ubus call dido_v2 status'
```

## Troubleshooting

| Issue | Fix |
|-------|-----|
| `apk` dependency errors | `apk update`; install `libstdcpp libmodbus` manually if needed |
| Wrong architecture package | Rebuild with **mediatek/filogic** SDK, not Pi SDK |
| DIO not toggling | DIO is **ubus dido_v2**, not libgpiod — see `lab/TR400_DIO_SMOKE_TEST.md` |
| ABI / glibc mismatch vs TesproOS | Compare `readelf -d` on device libs; request TesPro vendor SDK (V-001) |

## SDK artifact (reference)

| File | SHA256 |
|------|--------|
| `openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64.tar.zst` | `ff4a38a397caa2cfe1c39e18f84ddede14878221b3593c3f2c4cfe24e3ec4c25` |

URL: https://downloads.openwrt.org/releases/25.12.5/targets/mediatek/filogic/

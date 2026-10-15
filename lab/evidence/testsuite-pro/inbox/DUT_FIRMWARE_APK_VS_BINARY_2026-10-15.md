# DUT firmware — why Connect still fails (APK vs binary, 2026-10-15)

## Network (logger PC)

With **one** Ethernet NIC you must pick **either** LAN1 **or** LAN2 script (not both at once):

| Script | PC IP | DUT SSH |
|--------|-------|---------|
| `lab\set_pc_lan1_logger.cmd` | 192.168.10.20 | 192.168.10.1 |
| `lab\set_pc_lan2_oa.cmd` | 192.168.1.183 | 192.168.1.130 |

If ping to both `.10.1` and `.1.130` fails, fix cable/power/IP before debugging MMS.

## We did **not** deploy an APK

`deploy-ccli-session-fix.ps1` + `deploy-ccli-dut.sh`:

1. `pscp` → `/tmp/ccli.new`
2. `mv /tmp/ccli.new /usr/sbin/ccli` (overwrites binary)
3. **No** `apk add ccli-0.1.0-r51.apk`

So OpenWrt package metadata may still show an **old** `apk list-installed` line while `/usr/sbin/ccli` is a **hand-copied** file.

## There is **no** `ccli-0.1.0-r51.apk` in the repo

`lab/tg544-openwrt/` has APKs through **r30**, then **r53–r55**. **r51 APK was never checked in.**

## The "Oct-08 r51" binary we uploaded was **mislabeled**

| Source | Claim | Actual on DUT after deploy |
|--------|--------|---------------------------|
| Git commit `4c73add` + `ccli-bin.r51` | VERSION file says **r51** / P3-07-AARE-SIGN-RAW-GT | `ccli --version` → **`0.1.0-r30 (P5-R09-O14-VARSD)`** |
| SHA256 uploaded | `1882301468c10da90b7cd8cb03e8d4b63d7493f7126572ad7592e33deee8ce9d` | Matches archived `ccli-bin.r51` |

Strings in that binary **do** include `mms: iso session CONNECT` (session fix present), but it is **not** the same build identity as Oct-08 PASS evidence (`ccli-0.1.0-r51.apk`).

## TLS/config were aligned; typical DUT log when TSP still fails

```text
mms: client CONNECT count=1
mms: client DISCONNECT count=0
(no iso session CONNECT, no ACSE auth ACCEPT)
```

That pattern = **after transport TLS**, before Oct-08 PASS ladder — often **TestPro MMS paths/UI** or **client abort**, not wrong DUT CA.

Oct-08 PASS required:

```text
mms: iso session CONNECT (payload=…)
mms: ACSE auth ACCEPT — mech=… role=DSO_OPERATOR
mms: AARE responder auth [10]EXTERNAL …
```

## What to run when LAN2 is up

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\verify-dut-ccli-install.ps1 -HostAddr 192.168.1.130
powershell -File scripts\verify-dut-p3-07-connect-alignment.ps1 -HostAddr 192.168.1.130
```

## Correct firmware path (recommended)

1. **Build** matching `apps/ccli/VERSION` (set release **51** or current **55** with P3-07 codename if sources still have AARE fixes):
   `wsl bash lab/tg544-openwrt/build-ccli-ipk-sdk.sh`
2. **Install APK** on DUT:
   `apk add --allow-untrusted --force-overwrite /tmp/ccli-0.1.0-rNN.apk`
3. **Re-push yaml + tls** with `deploy-ccli-session-fix.ps1` (config only) **or** full deploy using **fresh** `ccli-bin` from that build (verify `ccli --version` shows **rNN** before upload).

Until `ccli --version` on DUT matches the build you intend, treat "we deployed r51" as **unverified**.

# GitHub sync + DUT — 2026-10-09

## Git (this PC)

| Item | Value |
|------|--------|
| **Merged** | `origin/main` → `master` commit **`5939fb8`** |
| **Upstream** | `e31d2d7` — *Release 0.1.0-r49: fix 104 TotW published flag and lab bench tooling* |
| **Version kept** | **`0.1.0-r51`** (P3-07-AARE-SIGN-RAW-GT) — merge conflict resolved on HEAD |
| **New in tree** | `TSP_TWO_PC_BENCH_RUNBOOK.md`, `lab_tr400_phase4_eth_b_ttyS1.yaml`, matrix/HW updates |

**Stashes (local TLS/staging):** `git stash list` — pop when ready; do not overwrite DUT TLS blindly.

**Not done:** Full `deploy-ccli-session-fix.ps1` — **`lab/tg544-openwrt/ccli-bin`** does not embed **r51** (rebuild required).

## DUT sync (via LAN1 SSH)

| Action | Result |
|--------|--------|
| **`/etc/ccli/lab.yaml`** | Updated from **`lab_tr400_phase4_eth_b_ttyS1.yaml`** (3782 + 104 enabled + ttyS1 Modbus) |
| **`ccli` restart** | Done |
| **104 :2404** | Still **stub** until **ccli built with lib60870** (install **r49+** IPK from second PC or rebuild r51 with 60870) |

## Next build/deploy (for 104 on LAN2)

```powershell
$env:CCLI_TG544_PW = '000000'
# After WSL: lab/tg544-openwrt/build-ccli-ipk-sdk.sh OR wsl-build-ccli.sh → refresh ccli-bin
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 -HostAddr 192.168.10.1 `
  -LabYaml D:\CCLI\CCLI\CCLI\apps\ccli\config\lab_tr400_phase4_eth_b_ttyS1.yaml `
  -Changes @("r51+r49 merge: phase4 ttyS1 + lib60870")
```

Or copy **`ccli-0.1.0-r49.apk`** from second PC and `opkg install` on DUT (if that build includes 60870 + P3-07 fixes — verify version strings).

---

## SW status snapshot (2026-10-09 ~12:39)

| Layer | UP? |
|-------|-----|
| Repo `5939fb8` + DUT yaml | Yes |
| PC-B TSP (:3782) | Yes |
| DUT MMS `ccli` :3782 | Yes (r48) |
| DUT 104 :2404 | No (60870 stub) |
| PC-A LAN2 client | No (NIC not `.1.183`) |
| Chrony | Yes |
| GNSS | No |

Full table: [`BENCH_DEVICE_STATUS_2026-10-09.md`](BENCH_DEVICE_STATUS_2026-10-09.md).

# Trivy evidence (IEC 62443 SCA / SVV-3)

**Tool:** Aqua Trivy · scanners **vuln** + **secret** (Udemy course parity).

**Pinned CCI app firmware:** `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_LATEST.md` (currently **0.1.0-r64**).

```powershell
cd c:\Yahia\projects\CCLI
powershell -File scripts\trivy\scan-cci-firmware.ps1
```

Uses **repo-local** firmware only: `CCLI_DEPLOY_LATEST`, `ccli-bin`, `dut-extract-*/ccli`, `apps/ccli`, `product/apps/ccli`.

**Live DUT (LAN1 Eth_A):** from bench PC on `192.168.10.x`:

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\trivy\scan-dut-lan1-trivy.ps1
```

Pulls `usr lib bin sbin etc` via plink/pscp to `192.168.10.1`, then `trivy rootfs` on PC. Tarball is extracted under **WSL** (`\\wsl.localhost\<distro>\tmp\ccli-trivy-<runId>`) because Windows tar cannot create paths such as `usr/bin/[`.

Optional: `-RootfsPath` (vendor tarball) or `-PullFromDut -DutHost 192.168.1.130` (LAN2).

**Signed-off evidence narrative:** `EVD-SCA-CCI-r64-Trivy.md` (rev 2, **FINAL** for 0.1.0-r64).

**Keys on device (now):** `powershell -File scripts/trivy/verify-dut-tls-keys.ps1 -CompareRepo` · Trivy secrets on pulled rootfs: `trivy-dut-rootfs.json` in latest LAN1 run folder (see `dut-tls-keys-live.txt` for live SHA256 list).

**Detailed JSON (findings + analysis):** `CCLI-TRIVY-FINDINGS-DETAIL-0.1.0-r64.json` · `powershell -File scripts/trivy/export-trivy-findings-detail-json.ps1`

**Scope labels in each run folder:**

| Label | Meaning |
|-------|---------|
| `ccli-staging` | Deploy **`ccli-bin`** laid out as mini rootfs (apk v3 staging) |
| `apps-ccli` | HiTEKS `apps/ccli` tree (implementation SCA) |
| `dut-extract-r57` | Pinned ELF from `dut-extract-connect-pass-r57-2026-10-16/ccli` |
| `dut-rootfs` | Optional live pull (`-PullFromDut`) |
| `rootfs` | Manual vendor/sysupgrade extract (`-RootfsPath`) |

**Product note:** Complete **TesproOS** image scan requires DUT online or TesPro SDK image (V-001). HiTEKS evidence must label **app (rN)** vs **platform OS**.

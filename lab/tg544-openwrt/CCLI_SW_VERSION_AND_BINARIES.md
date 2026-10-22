# CCLI software version and binaries (lab)

**Document ID:** CCLI-LAB-SW-BIN-001  
**Purpose:** Every bench upload and cert session must trace **semver/release**, **staging binaries**, and **DUT `/usr/sbin/ccli`** identity.

---

## Requirements (always)

| ID | Rule |
|----|------|
| **REQ-SW-001** | Edit **`apps/ccli/VERSION`** (semver, release **rN**, codename) before a labelled upload. |
| **REQ-SW-002** | Run **`lab/tg544-openwrt/wsl-build-ccli.sh`** â€” refreshes **`ccli-bin`**, **`ccli-0.1.0-rN.apk`**, **`ccli-VERSION`**. |
| **REQ-SW-003** | Deploy only via **`deploy-ccli-session-fix.ps1`** with **`-Changes`** â€” creates **`deploy-manifests/CCLI_DEPLOY_*.{md,json}`**. |
| **REQ-SW-004** | After deploy, **staging SHA256** must **match** **DUT** `sha256sum /usr/sbin/ccli` (recorded in **LATEST**). |
| **REQ-SW-005** | Pin sessions / evidence to **`deploy-manifests/CCLI_DEPLOY_LATEST.md`** + manifest file name. |

**Never** claim a firmware revision from memory â€” use the manifest SHA and DUT check.

---

## Source-of-truth paths

| Artifact | Path |
|----------|------|
| Version source | `apps/ccli/VERSION` |
| Staging TG544 binary | `lab/tg544-openwrt/ccli-bin` (aarch64 musl) |
| Matching OpenWrt APK | `lab/tg544-openwrt/ccli-0.1.0-rN.apk` |
| Build sidecar | `lab/tg544-openwrt/ccli-VERSION` |
| Latest deploy rollup | `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_LATEST.{md,json}` |
| Per-upload record | `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-rN_*.md` |
| Deploy workflow | `lab/tg544-openwrt/DEPLOY_REVISION_RECAP.md` |

---

## Build and deploy (minimal)

```powershell
# 1) Version (if bumping rN)
#    apps/ccli/VERSION + CHANGELOG

# 2) Build (WSL)
wsl bash -lc "cd /mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt && bash wsl-build-ccli.sh"

# 3) Host unit smoke (optional, WSL)
wsl bash -lc "/mnt/c/Yahia/projects/CCLI/apps/ccli/build-host/OPTIONAL/dso_phase1_test"

# 4) Deploy (LAN1 DSO / MMS)
. .\scripts\source-lab-env.ps1
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr 192.168.10.1 `
  -LabYaml apps\ccli\config\product_tr400_p3_07_connect_5ln.yaml `
  -TlsDir apps\ccli\config\tls\p3_07_connect `
  -Changes @("describe this upload")
```

Post-deploy manual check:

```bash
/usr/sbin/ccli --version
sha256sum /usr/sbin/ccli
netstat -tlnp | grep -E '3782|2404'
```

---

<!-- CURRENT_START -->
## Current lab DUT pin (auto-updated)

| Item | Value |
|------|-------|
| **Software** | **0.1.0-r57** (P5-5LN-ISO-PRES-AARE) |
| **Git (build tree)** | `ffb0f19` |
| **DUT SSH** | `root@192.168.10.1` (LAN1 `192.168.10.1` or LAN2 `192.168.1.130`) |
| **Staging `ccli-bin`** | SHA256 `51afd8a74ef3e604f90fd89dbb7f0fd8f7d08c75ce6eef7f0d1fa68b7d4e0533` (7805208 B) |
| **OpenWrt APK** | ``ccli-0.1.0-r57.apk`` SHA256 ``11dc9addf609531f18d22948898eebfb254a5768a176abe4ab9fab79e0298767`` |
| **DUT `/usr/sbin/ccli`** | SHA256 `51afd8a74ef3e604f90fd89dbb7f0fd8f7d08c75ce6eef7f0d1fa68b7d4e0533` **MATCH** |
| **Config on DUT** | `/etc/ccli/lab.yaml` = `product_tr400_p3_07_connect_5ln.yaml` |
| **Last manifest** | [CCLI_DEPLOY_0.1.0-r57_2026-10-20_174111.md](deploy-manifests/CCLI_DEPLOY_0.1.0-r57_2026-10-20_174111.md) |
| **Updated (UTC)** | 2026-10-20T15:44:39.5815273Z |

**Changes in last deploy:** O.13.1.2 OR fallback via commissioned dso O.11 min; MmsService operating_rule_fallback_latched forward
<!-- CURRENT_END -->

---

## Related

- [`deploy-manifests/README.md`](deploy-manifests/README.md)
- [`LAB_ACCESS_FREEZE.md`](../LAB_ACCESS_FREEZE.md) â€” LAN1/LAN2 IPs
- [`../conformance/LAB_UNIFIED_SESSION_PIN_2026-10-14.md`](../conformance/LAB_UNIFIED_SESSION_PIN_2026-10-14.md) â€” update session pin when DUT binary changes


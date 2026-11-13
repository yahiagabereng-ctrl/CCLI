# DUT platform SBOM intake — LAN1 live pull

**When:** 2026-11-12 (local bench)  
**Host:** `root@192.168.10.1` (Eth_A / LAN1) — **TR500** reachable  
**Maps to:** EVD-SM-9 step 2 (platform intake) · CR 7.8 runtime inventory · SM-10 vendor CycloneDX **not on device**

## Identity (runtime)

| Field | Value |
|-------|--------|
| OS | TesproOS **32-2.1.0** `r601-af70fcaaa2` |
| Target | mediatek/filogic · aarch64_cortex-a53 |
| Running `ccli` | **0.1.0-r64** SHA256 `967df276…` |
| apk metadata `ccli` | **0.1.0-r51** (stale ipk record; binary upgraded via staging deploy) |

## What “SBOM” means on this DUT

TesPro release tree ships **`<image>.bom.cdx.json`** beside factory images (build artefact). A filesystem search on the **running** rootfs found **no** `*.bom.cdx.json`, `config.buildinfo`, or `*.manifest`.

**Authoritative on-device inventory today:** `apk list -I` → **302** installed packages.

## Files

| File | Role |
|------|------|
| `dut-platform-identity.txt` | `openwrt_release` + `ccli --version` + binary hash |
| `platform-apk-list-installed.txt` | Raw `apk list -I` (runtime SBOM proxy) |
| `platform-runtime-inventory.cdx.json` | CycloneDX 1.5 generated from apk lines (intake only; not vendor-signed SBOM) |

## Pull commands (repeat)

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
plink -ssh root@192.168.10.1 -pw $env:CCLI_TG544_PW -batch "apk list -I"
```

Vendor formal SBOM: request matching **r601-af70fcaaa2** image bundle from TesPro (SM-10).

**HiTEKS app SBOM (compile-time, r64):**  
`lab/evidence/EVD-SM-9-External-Components/CCLI_APP_SBOM_0.1.0-r64.cdx.json` · architecture map `CCLI_SW_ARCHITECTURE_AND_IMPLEMENTATION_0.1.0-r64.md`.

# Product cyber bundle — 0.1.0-r61 (EMT432 phase-A MMS)

**Date:** 2026-10-22  
**Codename:** P5-EMT432-A-PHSA-BRIDGE  
**Scope:** Lab DUT deploy — Modbus **I1 @922** → **`PdCMMXU1.A.phsA.cVal.mag.f`** (MX) on P3-07 TLS 5-LN model.  
**Parent:** `ccli-product-sw-freeze` · Connect baseline remains **0.1.0-r57** until full cert re-sign-off.

## Frozen pair (do not split)

| Item | Value |
|------|--------|
| **APK** | `ccli-0.1.0-r61.apk` · APK SHA-256 `e384b3998150e56dcb3094ea1b5aa956843413394d3b9172a265ed22a3ef0a8c` |
| **ELF** | `ccli-bin-aarch64-musl` · SHA-256 `448007a2bd89e594e4df788bbc47dfd39535d45b5aa0b5b940c6fe8c07383819` · 7814864 B |
| **Config** | `product_tr400_p3_07_connect_5ln.yaml` SHA-256 `f4d0d29c25788633a0dc5431be253083b3382ee9f5c134af0c07923c2d24d357` |
| **Model** | `product_tr400_eth_a_5ln.cfg` SHA `c64ccdf884f60806603b6b53e71405a92c578bcdeb14c130b2abdc4a19b4937` |
| **CID (TestPro)** | `product_tr400_eth_a_5ln.cid` SHA `431047507cdc16b117ea1740a37805c4235f9cdf6a22d6c2f724f6cceaa8bb4b` |
| **TLS** | `apps/ccli/config/tls/p3_07_connect` · **crl=off** |
| **MMS** | `192.168.10.1:3782` TLS 1.2 |

## Lab pin

| Field | Value |
|-------|--------|
| Staging | `lab/tg544-openwrt/ccli-bin` |
| Archive | `lab/tg544-openwrt/ccli-bin.r61.p5-emt432-a-phsa-bridge` |
| Manifest | `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r61_2026-10-22_181748.md` |
| Git (feature commit) | `b6a81ef` |

## Restore (binary only)

```powershell
Copy-Item lab\tg544-openwrt\ccli-bin.r61.p5-emt432-a-phsa-bridge lab\tg544-openwrt\ccli-bin -Force
. .\scripts\source-lab-env.ps1
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr 192.168.10.1 `
  -LabYaml apps\ccli\config\product_tr400_p3_07_connect_5ln.yaml `
  -TlsDir apps\ccli\config\tls\p3_07_connect `
  -Changes @("restore r61 pin")
```

## Evidence

- DUT log: `modbus: … I1=…A` · `mms: PdCMMXU1.A.phsA.cVal.mag.f publish …A`
- TestPro read **`CCI016_01LD_Plant/PdCMMXU1.A.phsA.cVal.mag.f`** FC **MX** (leaf `mag.f`)

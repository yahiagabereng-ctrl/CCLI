# Product cyber bundle — 0.1.0-r64 (EMT432 → PdCMMXU1 LN)

**Date:** 2026-10-23  
**Codename:** P5-EMT432-CMV-ANG-URCB-PF1  
**Scope:** Lab DUT — EMT432 **906–956** → **PdCMMXU1** TotW/TotVAr/PPV/A **mag+ang**; URCB **ang.f**; meter map freeze rev **1.0**.  
**Parent:** `ccli-product-sw-freeze` · [EMT432_METER_CODE_FREEZE.md](../../../lab/conformance/EMT432_METER_CODE_FREEZE.md)

## Frozen pair (do not split)

| Item | Value |
|------|--------|
| **APK** | `ccli-0.1.0-r64.apk` · APK SHA-256 `cbd21ab2c2b820680d8a1419ecc0e897348fa1a93d6b12e2635a9deb80cf2b7c` |
| **ELF** | `ccli-bin-aarch64-musl` · SHA-256 `967df2764819c3b616e5467a16a3f9a923b4002ad1ca4a73d156d16e91ab2df7` · 7824384 B |
| **Config** | `product_tr400_p3_07_connect_5ln.yaml` SHA-256 `366a7d06100d13cff0d1f1c1c2ecdc171aa8e548c4dcccbf9e9f3860b04713a4` |
| **Model** | `product_tr400_eth_a_5ln.cfg` SHA `68fc1de01c67037ca502945f0c7a94628b0847f7da7f342f5f4f7d102a0f5741` |
| **CID (TestPro)** | `product_tr400_eth_a_5ln.cid` SHA `431047507cdc16b117ea1740a37805c4235f9cdf6a22d6c2f724f6cceaa8bb4b` |
| **TLS** | `apps/ccli/config/tls/p3_07_connect` · **crl=off** |
| **MMS** | `192.168.10.1:3782` TLS 1.2 |

## Lab pin

| Field | Value |
|-------|--------|
| Staging | `lab/tg544-openwrt/ccli-bin` |
| Archive | `lab/tg544-openwrt/ccli-bin.r64.p5-emt432-cmv-ang-urcb-pf1` |
| Manifest | `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r64_2026-10-22_185559.md` |
| Git (feature) | `c0dc4bd` |
| LN evidence | `lab/evidence/phase5/P5_EMT432_LN_VERIFY_2026-10-23_091200.txt` |

## Restore (binary only)

```powershell
Copy-Item lab\tg544-openwrt\ccli-bin.r64.p5-emt432-cmv-ang-urcb-pf1 lab\tg544-openwrt\ccli-bin -Force
. .\lab\tg544-openwrt\lab-env.ps1
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr 192.168.1.130 `
  -LabYaml apps\ccli\config\product_tr400_p3_07_connect_5ln.yaml `
  -TlsDir apps\ccli\config\tls\p3_07_connect `
  -Changes @("restore r64 pin")
```

TestPro: P3-07 certs · re-import **`CCLI_LAB_5LN_cert_points.csv`** (includes angle rows).

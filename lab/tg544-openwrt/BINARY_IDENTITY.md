# CCLI binary identity (TG544) — Connect PASS

**Rule:** Trust **`/usr/sbin/ccli --version`** and **`sha256sum /usr/sbin/ccli`**, not the filename `ccli-bin` alone.

## Current lab DUT (2026-10-23) — **r64 EMT432 LN + ang URCB**

| Item | Value |
|------|--------|
| **Version** | `ccli 0.1.0-r64 (P5-EMT432-CMV-ANG-URCB-PF1)` |
| **ELF SHA-256** | `967df2764819c3b616e5467a16a3f9a923b4002ad1ca4a73d156d16e91ab2df7` |
| **Size** | **7 824 384** B |
| **Local pin** | `lab/tg544-openwrt/ccli-bin.r64.p5-emt432-cmv-ang-urcb-pf1` |
| **APK** | `ccli-0.1.0-r64.apk` SHA `cbd21ab2c2b820680d8a1419ecc0e897348fa1a93d6b12e2635a9deb80cf2b7c` |
| **Yaml** | `product_tr400_p3_07_connect_5ln.yaml` SHA `366a7d06…` |
| **Model** | `product_tr400_eth_a_5ln.cfg` SHA `68fc1de0…` (URCB **ang.f**) |
| **Bundle** | `releases/product-cyber/0.1.0-r64/` |
| **Meter freeze** | `lab/conformance/EMT432_METER_CODE_FREEZE.md` |
| **Manifest** | `deploy-manifests/CCLI_DEPLOY_0.1.0-r64_2026-10-22_185559.md` |
| **Deploy path** | PC → **`192.168.1.130`** (LAN2) |

**MMS:** EMT432 **906–956** (`FC3 qty=52`) → PdCMMXU1 **mag+ang**; φ from **atan2(Q,P)** or **PF1** when Q≈0. **LN verify:** `P5_EMT432_LN_VERIFY_2026-10-23_091200.txt`.

## Previous lab DUT (2026-10-22) — **r62 PdC 3φ CMV**

| Item | Value |
|------|--------|
| **Version** | `ccli 0.1.0-r62 (P5-EMT432-PDC-3PH-CMV)` |
| **ELF SHA-256** | `20a974f14ddf73175a48d97105ccffca247d591b0f7a1efbecad873aa15baf3e` |
| **Local pin** | `lab/tg544-openwrt/ccli-bin.r62.p5-emt432-pdc-3ph-cmv` |

## Previous lab DUT (2026-10-22) — **r61 EMT432 phase-A**

| Item | Value |
|------|--------|
| **Version** | `ccli 0.1.0-r61 (P5-EMT432-A-PHSA-BRIDGE)` |
| **ELF SHA-256** | `448007a2bd89e594e4df788bbc47dfd39535d45b5aa0b5b940c6fe8c07383819` |
| **Local pin** | `lab/tg544-openwrt/ccli-bin.r61.p5-emt432-a-phsa-bridge` |
| **Manifest** | `deploy-manifests/CCLI_DEPLOY_0.1.0-r61_2026-10-22_181748.md` |

## Connect-PASS reference (2026-10-16) — **r57 + 5-LN**

Pulled / pinned while TestPro showed **Connected** (ACSE ACCEPT + AARE + session ACCEPT).

| Item | Value |
|------|--------|
| **Version** | `ccli 0.1.0-r57 (P5-5LN-ISO-PRES-AARE)` |
| **ELF SHA-256** | `96963619dbebb87e366510a555fdac729a4b948847b76f1f91d94b5b6cd7d697` |
| **Size** | **7 805 000** B (debug `ccli-bin`, same SHA on DUT) |
| **Local pin** | `lab/tg544-openwrt/ccli-bin.r57.dut-connect-pass` |
| **APK** | `ccli-0.1.0-r57.apk` SHA `422e731cc236598fd4520d8f43e815fb105f4507a0e5f118eea074931c7fc1ae` |
| **Yaml** | `product_tr400_p3_07_connect_5ln.yaml` SHA `ed7d8778…` |
| **Model** | `product_tr400_eth_a_5ln.cfg` SHA `7e13de13…` |
| **CID** | `product_tr400_eth_a_5ln.cid` SHA `68c72e7f…` |
| **TLS** | `p3_07_connect` · root_CA MD5 `7ada0cbe2be1bb4bddff7b6961461bc5` · **crl=off** |
| **Freeze** | `releases/product-cyber/0.1.0-r57/` |
| **Extract** | `lab/tg544-openwrt/dut-extract-connect-pass-r57-2026-10-16/MANIFEST.json` |

## Previous Connect-PASS (31-LN r51, 2026-10-16 morning)

| Item | Value |
|------|--------|
| **Version** | `ccli 0.1.0-r51 (P3-07-AARE-SIGN-RAW-GT)` |
| **ELF SHA-256** | `7b05d7ccbbecd7c041d30cc6a362e1e8d1d12dea0e39792f37159d8cc5a6fabe` |
| **Model** | `lab_tg544_eth_a.cfg` (31-LN) |
| **Extract** | `lab/tg544-openwrt/dut-extract-connect-pass-2026-10-16/MANIFEST.json` |

Keep r51 only as rollback for the **31-LN** CID.

## Quick check on DUT

```bash
/usr/sbin/ccli --version
sha256sum /usr/sbin/ccli
grep model_cfg /etc/ccli/lab.yaml
md5sum /etc/ccli/tls/root_CA.pem
```

Pass path: **r57 ELF `96963619…`** + yaml **`product_tr400_p3_07_connect_5ln.yaml`** + 5-LN cfg + P3-07 TLS.

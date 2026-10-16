# CCLI binary identity (TG544) — Connect PASS

**Rule:** Trust **`/usr/sbin/ccli --version`** and **`sha256sum /usr/sbin/ccli`**, not the filename `ccli-bin` alone.

## Current Connect-PASS (2026-10-16 live) — **r57 + 5-LN**

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

# CCLI deploy manifest â€” 0.1.0-r57

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-22 14:52:39 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r57** |
| **Codename** | P5-5LN-ISO-PRES-AARE |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `51afd8a74ef3e604f90fd89dbb7f0fd8f7d08c75ce6eef7f0d1fa68b7d4e0533` |
| **Size (bytes)** | 7805208 |
| **Lab yaml** | `product_tr400_p3_07_connect_5ln.yaml` |
| **TLS dir** | `p3_07_connect` |
| **ICD cfg** | `product_tr400_eth_a_5ln.cfg` |
| **Revision pin** | `PRODUCT-5LN-CERT` |
| **Git (short)** | `2516bc8` |


## Changes in this deploy

- Product r57 5-LN deploy 2026-10-22
- product_tr400_p3_07_connect_5ln.yaml
- p3_07_connect TLS

## Binary identity (pre-upload)

```
/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/ccli-bin: ELF 64-bit LSB executable, ARM aarch64, version 1 (SYSV), dynamically linked, interpreter /lib/ld-musl-aarch64.so.1, with debug_info, not stripped
```

## Post-deploy checks (DUT)

```bash
/usr/sbin/ccli --version
/usr/sbin/ccli --version-json
cat /etc/ccli/VERSION
ss -tlnp | grep -E '3782|2404'
grep -E '^(mms|iec104):' /etc/ccli/lab.yaml
tail -20 /tmp/ccli.log
```

## Rollback

Previous manifest + matching `ccli-bin` in `deploy-manifests/` archive folder.


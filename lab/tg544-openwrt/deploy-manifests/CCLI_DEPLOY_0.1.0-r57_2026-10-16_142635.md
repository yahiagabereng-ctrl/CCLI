# CCLI deploy manifest â€” 0.1.0-r57

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-16 14:26:35 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r57** |
| **Codename** | P5-5LN-ISO-PRES-AARE |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `96963619dbebb87e366510a555fdac729a4b948847b76f1f91d94b5b6cd7d697` |
| **Size (bytes)** | 7805000 |
| **Lab yaml** | `product_tr400_p3_07_connect_5ln.yaml` |
| **TLS dir** | `p3_07_connect` |
| **ICD cfg** | `product_tr400_eth_a_5ln.cfg` |
| **Revision pin** | `PRODUCT-5LN-CERT` |
| **Git (short)** | `68563b8` |


## Changes in this deploy

- Product FW 0.1.0-r55 + 5-LN bench
- product_tr400_p3_07_connect.yaml
- TLS p3_07_connect (same as r51 Oct-08 Connect, crl off)
- Single ccli; iec61850service off

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


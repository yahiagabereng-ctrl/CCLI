# CCLI deploy manifest â€” 0.1.0-r60

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-22 18:13:55 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r60** |
| **Codename** | P5-EMT432-A-PHSA-PUBLISH |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `f7b7bb10efa9bdf54fe4b415c9e492977d3b18f13155889742ea2fbee1960d47` |
| **Size (bytes)** | 7814864 |
| **Lab yaml** | `product_tr400_p3_07_connect_5ln.yaml` |
| **TLS dir** | `p3_07_connect` |
| **ICD cfg** | `product_tr400_eth_a_5ln.cfg` |
| **Git (short)** | `546d489` |


## Changes in this deploy

- r60 P5-EMT432-A-PHSA-PUBLISH
- MMS_FLOAT wire + URCB leaf DE
- bench phs_a_a + modbus I1 trace

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


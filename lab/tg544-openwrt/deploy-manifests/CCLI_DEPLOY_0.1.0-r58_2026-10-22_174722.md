# CCLI deploy manifest â€” 0.1.0-r58

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-22 17:47:22 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r58** |
| **Codename** | P5-EMT432-A-PHSA-MAP |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `e1d0341b3350cc114bc85a98fc8db898d4af6b2aa21f9b1cdc50ac0f723f5bfb` |
| **Size (bytes)** | 7814688 |
| **Lab yaml** | `product_tr400_p3_07_connect_5ln.yaml` |
| **TLS dir** | `p3_07_connect` |
| **ICD cfg** | `product_tr400_eth_a_5ln.cfg` |
| **Revision pin** | `product_tr400_p3_07_connect_5ln.yaml` |
| **Git (short)** | `546d489` |


## Changes in this deploy

- r58 EMT432 I1->A.phsA MMS map

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


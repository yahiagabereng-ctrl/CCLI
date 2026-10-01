# CCLI deploy manifest â€” 0.1.0-r24

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-09-30 21:17:08 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.10.1 |
| **Version** | **0.1.0-r24** |
| **Codename** | P5-R01 |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `37d5ea737a6f3b7ef855fdfd64718c9fee58d32a9e4c139ec73a7d5268e717e9` |
| **Size (bytes)** | 7713720 |
| **Lab yaml** | `lab_tr400_cleartext_tsp.yaml` |

## Changes in this deploy

- P5 full circle cleartext TSP
- mms tcp_port 102 tls_enabled false
- permissive_bypass true for actuation

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


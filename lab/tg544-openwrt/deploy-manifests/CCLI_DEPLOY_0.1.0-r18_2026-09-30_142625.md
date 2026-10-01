# CCLI deploy manifest â€” 0.1.0-r18

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-09-30 14:26:25 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r18** |
| **Codename** | P4-104-tls |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `1c8e8616a1fb4502ce291f6cab213453885da7547b80480c5f00d44f68512246` |
| **Size (bytes)** | 7699080 |
| **Lab yaml** | `r18 P4-104-tls` |

## Changes in this deploy

- CS104 TLS 62351-3 Eth_B

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


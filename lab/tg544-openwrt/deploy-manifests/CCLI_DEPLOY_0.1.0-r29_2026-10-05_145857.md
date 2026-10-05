# CCLI deploy manifest â€” 0.1.0-r29

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-05 14:58:57 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.10.1 |
| **Version** | **0.1.0-r29** |
| **Codename** | EQ-PLANE-R07-SPACING |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `2db289733cfaa2ac911522cf1c3371c3b773106c7ede93066ea0db70d0ec7425` |
| **Size (bytes)** | 7776552 |
| **Lab yaml** | `lab_tr400_cleartext_tsp_ttyS1.yaml` |

## Changes in this deploy

- Restore r29 after r30 segfault investigation

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


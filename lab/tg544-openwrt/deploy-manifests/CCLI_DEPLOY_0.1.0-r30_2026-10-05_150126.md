# CCLI deploy manifest â€” 0.1.0-r30

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-05 15:01:26 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.10.1 |
| **Version** | **0.1.0-r30** |
| **Codename** | P5-R09-O14-VARSD |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `1882301468c10da90b7cd8cb03e8d4b63d7493f7126572ad7592e33deee8ce9d` |
| **Size (bytes)** | 7776624 |
| **Lab yaml** | `lab_tr400_cleartext_tsp_ttyS1.yaml` |

## Changes in this deploy

- P5-R09 varsd_operate O.14 detail (string concat)

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


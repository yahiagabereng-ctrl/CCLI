# CCLI deploy manifest â€” 0.1.0-r30

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-05 14:52:22 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.10.1 |
| **Version** | **0.1.0-r30** |
| **Codename** | P5-R09-O14-VARSD |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `a6adc8e86ceee88f26fe0081bceea3877149a04b50b48c5b3b53bfd06c6e3483` |
| **Size (bytes)** | 7776552 |
| **Lab yaml** | `lab_tr400_cleartext_tsp_ttyS1.yaml` |

## Changes in this deploy

- P5-R09 O.14 varsd_operate event detail
- P5-G03 merge policy session evidence

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


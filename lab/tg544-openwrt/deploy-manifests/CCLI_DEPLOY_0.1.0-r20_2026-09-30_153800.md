# CCLI deploy manifest â€” 0.1.0-r20

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-09-30 15:38:00 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r20** |
| **Codename** | P4-104-o14-params |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `9328fe6cbd7fc3ae5b707ef077a69fb83404557c0bf5cee8707ce6a71f6944df` |
| **Size (bytes)** | 7703888 |
| **Lab yaml** | `lab_tr400_phase4_eth_b.yaml` |

## Changes in this deploy

- O.14 audit detail type+ioa+value+result
- command mode MSD stub O.10.3.1
- lab_tr400_phase4_eth_b_command.yaml

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


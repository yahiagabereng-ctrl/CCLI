# CCLI deploy manifest â€” 0.1.0-r19

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-09-30 15:22:05 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r19** |
| **Codename** | P4-104-operator-rule |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `d34df23e052ac12915fa57cfe666daa7e8b681523a5cf433a91a51984dbc73a1` |
| **Size (bytes)** | 7699320 |
| **Lab yaml** | `lab_tr400_phase4_eth_b.yaml` |

## Changes in this deploy

- Operating Rule yaml (operator_rule_mode monitor_only)
- O.14 reject audit on command ASDUs
- iec104_operator_rule.hpp catalog

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


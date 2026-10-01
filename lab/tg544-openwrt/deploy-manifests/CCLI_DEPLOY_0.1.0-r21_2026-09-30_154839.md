# CCLI deploy manifest â€” 0.1.0-r21

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-09-30 15:48:39 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r21** |
| **Codename** | P5-TotVAr |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `1618f0b97bab97c69a81aa0eee1e28dba7aaff296f6390f0d89e355992943553` |
| **Size (bytes)** | 7703968 |
| **Lab yaml** | `lab_tr400_phase4_eth_b.yaml` |

## Changes in this deploy

- P5-M07 TotVAr MMS DO
- DS_R_PdC_Mis4sec + TotVAr member
- update_totvar_kvar from Modbus q_kvar

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


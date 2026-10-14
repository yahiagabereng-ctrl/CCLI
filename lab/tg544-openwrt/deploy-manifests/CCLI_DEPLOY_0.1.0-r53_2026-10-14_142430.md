# CCLI deploy manifest â€” 0.1.0-r53

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-14 14:24:30 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r53** |
| **Codename** | P5-PPV-EMT432-UNIFIED-CERT |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `11a9a4f0fe70bb066e31d9cdca38321e7c900cf7914a20fd0c534d011b3e14da` |
| **Size (bytes)** | 7802088 |
| **Lab yaml** | `lab_tr400_phase4_eth_b_ttyS1.yaml` |

## Changes in this deploy

- Two-PC bench lab_tr400_phase4_eth_b_ttyS1.yaml
- MMS TLS 3782 LAN1 + IEC104 2404 LAN2 + Modbus RTU /dev/ttyS1
- Unified cert test session deploy

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


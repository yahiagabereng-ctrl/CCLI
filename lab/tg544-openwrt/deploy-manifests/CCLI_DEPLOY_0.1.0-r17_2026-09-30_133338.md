# CCLI deploy manifest â€” 0.1.0-r17

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-09-30 13:33:38 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r17** |
| **Codename** | P4-104-eth-b |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `7b7cf11574f36f7478247fb523c57df558b4832d35d4b80c49d5b6ff7d2d72e9` |
| **Size (bytes)** | 7694408 |
| **Lab yaml** | `lab_tr400_phase4_eth_b.yaml` |

## Changes in this deploy

- CS104 slave bind 192.168.1.130:2404
- lab_tr400_phase4_eth_b.yaml deployed
- Version 0.1.0-r17 aarch64 SDK build

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


# CCLI deploy manifest â€” 0.1.0-r22

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-09-30 16:21:31 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.10.1 |
| **Version** | **0.1.0-r22** |
| **Codename** | P5-PPV |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `0592232ce5f3b5b614464accbc16d87f3f517b9f8c7bc0eb2b3587c7b2967be3` |
| **Size (bytes)** | 7704040 |
| **Lab yaml** | `lab_tr400_phase4_eth_b.yaml` |

## Changes in this deploy

- P5-PPV r22
- PdCMMXU1 PPV DEL phsAB
- TotVAr + ENC stub fix
- urcb DS_R_PdC_Mis4sec

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


# CCLI deploy manifest â€” 0.1.0-r55

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-15 18:16:40 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r55** |
| **Codename** | P5-5LN-CERT-MODEL |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `e192d3cce1a0bdff54bb712c6cefdbbd0ff7a2f5057f74c0bc0b4dac5a950b31` |
| **Size (bytes)** | 7802088 |
| **Lab yaml** | `lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml` |

## Changes in this deploy

- P3-07 connect profile p3_07_connect
- lab_tg544_eth_a.cfg crl=off
- ccli-bin from apps/ccli/VERSION (not legacy r51 git blob)

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


# CCLI deploy manifest â€” 0.1.0-r55

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-15 16:31:27 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r55** |
| **Codename** | P5-5LN-CERT-MODEL |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `a2c25122e777b4f62b5c856ee77cfb114de33fef9200b70c4ba206bd296be653` |
| **Size (bytes)** | 7802088 |
| **Lab yaml** | `lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml` |

## Changes in this deploy

- P3-07 strict TLS replace (p3_07_connect only)
- align Masomeh 43e8e85
- lab_tg544_eth_a.cfg
- crl=off

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


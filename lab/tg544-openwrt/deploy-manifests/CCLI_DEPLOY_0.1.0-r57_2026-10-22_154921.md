# CCLI deploy manifest â€” 0.1.0-r57

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-22 15:49:21 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r57** |
| **Codename** | P5-5LN-ISO-PRES-AARE |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `8c465f482534452d2379fc348ad3e1f845a4560199286ab1b2ab470c162aa825` |
| **Size (bytes)** | 7814328 |
| **Lab yaml** | `lab_tr400_emt432_lan3_tcp_102.yaml` |
| **TLS dir** | `tls` |
| **ICD cfg** | `lab_tg544_eth_a.cfg` |
| **Git (short)** | `f6deecb` |


## Changes in this deploy

- EMT432 commissioning rev5 40245/40247
- lab yaml revision 5

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


# CCLI deploy manifest â€” 0.1.0-r25

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-02 11:25:46 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.10.1 |
| **Version** | **0.1.0-r25** |
| **Codename** | P5-COMPARE-FIX |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `bf0f7bd4558a311f8dfbdb6224604bbdf8f29958a31628c367bcb255c1fb48a0` |
| **Size (bytes)** | 7720112 |
| **Lab yaml** | `lab_tr400_cleartext_tsp.yaml` |

## Changes in this deploy

- P5 full circle cleartext TSP
- mms tcp_port 102 tls_enabled false
- permissive_bypass true for actuation

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


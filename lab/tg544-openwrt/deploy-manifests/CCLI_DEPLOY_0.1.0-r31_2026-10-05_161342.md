# CCLI deploy manifest â€” 0.1.0-r31

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-05 16:13:42 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.10.1 |
| **Version** | **0.1.0-r31** |
| **Codename** | P5-R02-PFSP |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `e3659ed074a355e100b5e5a0450c3c7174229547f262298e0ae36dda4fa8a38f` |
| **Size (bytes)** | 7781240 |
| **Lab yaml** | `lab_tr400_cleartext_tsp_ttyS1.yaml` |

## Changes in this deploy

- P5-R02 PFSP cosphi set-point O.9.1.1
- MMS PFSPDFPF1 Mod+PFGnTgtSpt Operate
- Modbus FC16 Q from cosphi+live P
- O.14 pfsp_operate event
- 3s reactive command spacing gate

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


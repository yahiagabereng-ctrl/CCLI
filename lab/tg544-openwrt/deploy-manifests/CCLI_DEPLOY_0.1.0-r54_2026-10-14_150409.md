# CCLI deploy manifest â€” 0.1.0-r54

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-14 15:04:09 UTC |
| **Operator** | yahia |
| **Target DUT** | TG544 @ 192.168.1.130 |
| **Version** | **0.1.0-r54** |
| **Codename** | P5-5LN-CERT-MODEL |
| **Binary** | `C:\Yahia\projects\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `705752f8c0440a659e1726cba516753be42fff6d6297feba4a094db5cd43e885` |
| **Size (bytes)** | 7802088 |
| **Lab yaml** | `lab_tr400_phase4_eth_b_ttyS1_5ln.yaml` |

## Changes in this deploy

- r54: MMS reactive LNs optional (5-LN cert model boots)
- model_cfg -> product_tr400_eth_a_5ln.cfg (configRev 20261014, 5 LNs)
- U-T1-02/U-T1-03 Compare alignment

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


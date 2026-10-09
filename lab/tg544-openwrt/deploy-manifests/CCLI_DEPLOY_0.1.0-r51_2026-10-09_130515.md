# CCLI deploy manifest â€” 0.1.0-r51

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-09 13:05:15 UTC |
| **Operator** | Federico |
| **Target DUT** | TG544 @ 192.168.10.1 |
| **Version** | **0.1.0-r51** |
| **Codename** | P3-07-AARE-SIGN-RAW-GT |
| **Binary** | `D:\CCLI\CCLI\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `2b166430fdb5851a46bad31c1741f11f7c553161fe98bce8b6bbc6558cbdf7ae` |
| **Size (bytes)** | 7805952 |
| **Lab yaml** | `lab_tr400_phase4_eth_b_ttyS1.yaml` |

## Changes in this deploy

- r51 lib60870 CS104 Eth_B :2404
- phase4 ttyS1 lab.yaml

## Binary identity (pre-upload)

```
(run on WSL/Linux host to capture --version output)
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


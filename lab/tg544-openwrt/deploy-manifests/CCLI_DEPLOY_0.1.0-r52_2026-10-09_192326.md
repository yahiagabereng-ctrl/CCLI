# CCLI deploy manifest â€” 0.1.0-r52

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-09 19:23:26 UTC |
| **Operator** | Federico |
| **Target DUT** | TG544 @ 192.168.10.1 |
| **Version** | **0.1.0-r52** |
| **Codename** | P3-07-AARE-SIGN-RAW-GT |
| **Binary** | `D:\CCLI\CCLI\CCLI\lab\tg544-openwrt\ccli-bin` |
| **SHA256** | `1df82c26243021a86c533a179781c749fc852c2f724efa3b0aa4018b2969eca1` |
| **Size (bytes)** | 7806112 |
| **Lab yaml** | `lab_tr400_phase4_emt432_lan3_tcp.yaml` |

## Changes in this deploy

- a4b8d1c: CID rev9 configRev 20261009; MMXU2 drop PPV/A; URCB OptFlds entryID=false; regen lab_tg544_eth_a.cfg

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


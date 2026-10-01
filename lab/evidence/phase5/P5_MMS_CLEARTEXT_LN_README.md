# P5 — Full CID logical nodes + Wireshark (cleartext :102)

**Build:** 0.1.0-r23 (P5-FULL-CID)  
**Requires:** DUT deployed with `model_cfg: /etc/ccli/icd/lab_tg544_eth_a.cfg`

## Run (Admin PowerShell, LAN1 cable)

```powershell
$env:CCLI_TG544_PW = "<root>"
# 1. Deploy r23 + cfg
.\lab\tg544-openwrt\run-p5-fullcircle-cleartext.ps1

# 2. LN browser + Wireshark capture + tshark text analysis
.\lab\tg544-openwrt\run-p5-cleartext-ln-capture.ps1
```

Modbus slave (parallel): `python -u lab\modbus_rtu_slave.py --port COM5 --power-kw 450 --trace`

## Pass criteria

| Check | Expected |
|-------|----------|
| `mms_ln_browser` | `ln_count=31` · `VERDICT: PASS` |
| DUT log | `model=full_cid_cfg` |
| PCAP | MMS Initiate + GetNameList + Read on `:102` |
| Analysis | `PASS no TLS records on :102` |
| TSP Compare Model | Structure vs `lab_tg544_eth_a.cid` (values: live TotW/TotVAr + CID CF) |

## Evidence files

| File | Content |
|------|---------|
| `P5_MMS_CLEARTEXT_LN.pcapng` | Stable Wireshark capture |
| `P5_MMS_CLEARTEXT_LN_ANALYSIS.txt` | tshark MMS / frame analysis |
| `P5_MMS_CLEARTEXT_LN_SESSION.txt` | Client stdout (LN list + reads) |

## Regenerate model from CID

```powershell
.\scripts\gen-mms-model-cfg.ps1
```

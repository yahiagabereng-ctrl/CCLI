# P5-M06 — Secure MMS (IEC 62351-3 TLS on Eth_A)

**Date:** 2026-09-30  
**Build:** `0.1.0-r22` (P5-PPV)  
**Regulation:** P5-M06 · IEC 62351-3 · T.3.1.3 measurements on Eth_A

## Verify TLS MMS + live measurements

```powershell
# PC on LAN1 (192.168.10.10/24)
python lab\modbus_rtu_slave.py --port COM5 --power-kw 450 --trace

# Capture + client (Ethernet = tshark iface 8)
.\lab\tg544-openwrt\run-p5-mms-tls-capture.ps1 -CaptureSeconds 25
```

## Artifacts

| File | Content |
|------|---------|
| `P5_MMS_TLS.pcapng` | Wireshark capture (stable copy) |
| `P5_MMS_TLS_ANALYSIS.txt` | tshark TLS / cleartext analysis |
| `P5_MMS_TLS_SESSION.txt` | mms_tls_client log |
| `P5_DUT_MODBUS.log` | DUT FC03 P/Q trace |
| `P5_FINAL_VERIFY.txt` | End-to-end gate summary |

## VERDICT

- [x] r22 on DUT (`0.1.0-r22`, MMS TLS :3782)
- [x] Modbus P=450 kW Q=45 kvar → MMS TotW/TotVAr
- [x] Wireshark: TLS Client Hello + Application Data; no cleartext :102
- [x] PPV=20 kV from yaml (meter V register still P5-06 GAP)

# P4-03 — Secure 104 (IEC 62351-3 TLS on Eth_B)

**Date:** 2026-09-30  
**Build:** `0.1.0-r18` (P4-104-tls) — deploy required  
**Regulation:** P4-03 · IEC 62351-3 · O.13.1.1.1 Eth_B operator port

## Code traceability

| Artifact | Clause / gate |
|----------|----------------|
| `iec104_regulation.hpp` | O.13.1.1.1, O.13.1.3, O.14, 60870-5-104, 62351-3 |
| `iec104_tls.cpp` | P4-03 TLS PEM load, TLS 1.2 min |
| `iec104_adapter.cpp` | P4-01 bind, P4-02 ASDU, P4-03 `CS104_Slave_createSecure` |
| `lab_tr400_phase4_eth_b.yaml` | `iec104.tls_enabled: true` |

## Deploy r18

```powershell
$env:CCLI_TG544_PW = "<root pw>"
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr 192.168.1.130 `
  -Changes "CS104 TLS 62351-3","lab yaml tls_enabled","r18"
```

Expect log: `iec104: listening 192.168.1.130:2404 ... tls=on`

## Verify TLS session

```bash
# Rebuild client with mbedtls (includes --tls)
wsl bash -c "cd /mnt/c/Yahia/projects/CCLI/lab/104 && bash wsl-build-p4-client.sh"
wsl /mnt/c/Yahia/projects/CCLI/lab/104/p4_cs104_client 192.168.1.130 2404 1 1001 --tls
```

Save output → `P4_03_104_TLS.txt`

## Cleartext must fail after deploy

```bash
wsl /mnt/c/Yahia/projects/CCLI/lab/104/p4_cs104_client 192.168.1.130 2404 1 1001
# Expect: connect failed (plain TCP rejected or no STARTDT)
```

## Wireshark capture

```powershell
.\lab\tg544-openwrt\run-p4-03-tls-capture.ps1 -CaptureSeconds 30
```

Artifacts:
- `P4_03_104_TLS.pcapng` — stable capture copy
- `P4_03_104_TLS_ANALYSIS.txt` — tshark TLS / 0x68 analysis
- `P4_03_104_TLS_SESSION.txt` — client log

## VERDICT

- [x] r18 deployed (`0.1.0-r18`, tls=on on 192.168.1.130:2404)
- [x] `--tls` client PASS
- [x] Wireshark: TLS Client Hello + Application Data; no cleartext 0x68
- [x] cleartext client FAIL (exit 11)

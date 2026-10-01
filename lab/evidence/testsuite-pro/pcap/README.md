# TSP / MMS Wireshark captures (P3-07)

## One-time setup (done on this PC)

- **Wireshark 4.6.8** — `winget install WiresharkFoundation.Wireshark`
- **Npcap** — finish installer if prompted (`%TEMP%\npcap-install.exe`); enable *WinPcap API-compatible Mode*

## Capture during TSP Connect

```powershell
cd C:\Yahia\projects\CCLI
.\scripts\capture-mms-tsp.ps1 -DurationSec 120
```

Or GUI:

```powershell
.\scripts\capture-mms-tsp-gui.ps1
```

Filter: `host 192.168.10.1 and port 3782` on **Ethernet** (192.168.10.10).

## Decrypt TLS (see MMS/ACSE inside)

Wireshark → **Edit → Preferences → Protocols → TLS → RSA keys list → Edit**:

| IP | Port | Protocol | Key file |
|----|------|----------|----------|
| 192.168.10.1 | 3782 | http | `C:\CCLI_tls\server.key` |

Then **Analyze → Decode As…** if needed: port 3782 → TLS.

Follow **TLS stream** or filter `tls && ip.addr==192.168.10.1` — inner SPdu shows ACSE AARE and `responding-authentication-value`.

## DUT hex (no Wireshark)

```powershell
plink root@192.168.10.1 "grep -E 'TX_ACCEPT|AARE auth|responder auth' /tmp/ccli.log | tail -20"
```

Copy hex to `lab/tmp_tx_latest.hex`, then:

```powershell
python lab\locate_p3_07_failure.py lab\tmp_tx_latest.hex --cert apps\ccli\config\tls\server_combined.pem
```

Full matrix: `lab/CCI_MMS_MultiTool_Verification.md`

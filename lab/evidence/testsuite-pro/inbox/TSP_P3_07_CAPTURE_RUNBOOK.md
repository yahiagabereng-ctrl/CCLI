# P3-07 — TSP Connect + Wireshark capture runbook

**Goal:** Non-empty pcap showing AARE → Encrypted Alert (~2–5 ms) for Step 7 evidence.

## Before capture

- [ ] DUT `192.168.10.1` ping OK
- [ ] `ccli` listening on `:3782` (Solution C yaml on DUT)
- [ ] TSP: MMS Security + Transport TLS, port **3782**, target **CCI016_01**
- [ ] Certs in `C:\CCLI_tls\` match lab PKI

## Capture (Terminal A)

```powershell
cd C:\Yahia\projects\CCLI
.\scripts\capture-mms-tsp.ps1 -DurationSec 60
```

**During the 60 s window:** TSP → System Status → **Connect**.

## After capture

### 1. Packet count

```powershell
$t = Get-ChildItem lab\evidence\testsuite-pro\pcap\TSP_P3_07_mms_*.pcapng | Sort-Object LastWriteTime -Descending | Select-Object -First 1
& "C:\Program Files\Wireshark\tshark.exe" -r $t.FullName -q -z io,stat,0
```

Expect **> 0** packets if Connect ran during capture.

### 2. TLS decrypt (Wireshark GUI)

Edit → Preferences → Protocols → TLS → RSA keys:

| IP | Port | Protocol | Key |
|----|------|----------|-----|
| 192.168.10.1 | 3782 | http | `C:\CCLI_tls\server.key` |

Filter: `tls && ip.addr==192.168.10.1`

Look for: large **Application Data** (AARE) then **Encrypted Alert** within ~5 ms.

### 3. Correlate DUT hex

```powershell
$env:CCLI_TG544_PW = "<set>"
.\lab\pull_tx_accept_hex.ps1
python lab\locate_p3_07_failure.py lab\tmp_tx_step7_latest.hex --cert apps\ccli\config\tls\server_combined.pem
```

### 4. Record TSP log line

Copy exact TSP error (e.g. `Unable to verify signature (+)`) into evidence note.

## Pass criteria

| Check | Pass |
|-------|------|
| pcap packets > 0 | Capture timed with Connect |
| TLS handshake complete | Steps 1–2 |
| Session + ACSE visible after decrypt | Steps 3–6 |
| Alert after AARE app data | Step 7 reject on wire |
| Offline verify PASS | DUT crypto OK → TSP-side issue |

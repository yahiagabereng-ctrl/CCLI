# Connect capture — 2026-10-15 16:16:53 (PC1 local)

## Artifacts

| File | Description |
|------|-------------|
| `TSP_CONNECT_DUT_LOG_2026-10-15_161653.txt` | DUT `logread -e ccli` + netstat |
| `pcap/TSP_CONNECT_2026-10-15_161653.pcapng` | **105 frames**, ~30 s, `:3782` |
| `TSP_CONNECT_2026-10-15_161653_WIRE.txt` | tshark frame summary |

## Wireshark

- **3× TLS Client Hello** from `192.168.10.10` (frames 6, 41, 76)
- Full TLS handshake + application data present (~34 KB)

## DUT (22:16:50) — Transport cert FIXED

- Client cert on wire: serial **`DC:9A:9E:26:D5:47:EF:38`** · **`CN=DSO_OPERATOR`** (`client_tls.pem`)
- `mms: client CONNECT count=1`
- Still **no** `mms: ACSE auth ACCEPT`
- Session drops → `no clients — Operating Rule fallback armed`

## Next (MMS layer)

- Confirm MMS Security: `client.pem` + `client_tsp.key`
- AP Title **`1,1,1,999,1`** on all stack tabs
- Sisco **off** · chaining **1**
- Paste TSP **Output** after Connect

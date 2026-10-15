# TSP Connect evidence — USB TLS pack (2026-10-15)

**Git at capture:** `51fee2a`  
**PC1 TLS:** `C:\Users\Firmware Engineer\Downloads\CCLI_TSP_TLS_COMPLETE\CCLI_TSP_TLS_COMPLETE\`  
**TSP:** `Initiate Response Failed: Transport connection closed`

## PC1 — PASS

| Check | Result |
|-------|--------|
| TCP `192.168.10.1:3782` from `192.168.10.10` | OK |
| MD5 vs DUT pin | `13c231ee…` transport, `7bb6b77c…` MMS |
| `verify-tsp-tls-key-match.ps1` on USB folder | **PASS** |

File: [PC1_EVIDENCE_CAPTURE_2026-10-15_b.txt](PC1_EVIDENCE_CAPTURE_2026-10-15_b.txt)

## DUT — **root cause candidate**

| Item | Value |
|------|--------|
| **`ccli` processes** | **TWO** (pids 11602 + 11745) |
| **3782 listener** | pid **11602** only |
| Second `ccli` | **`mms: FAILED to bind 192.168.10.1:3782 (TLS) — port busy`** |
| **`iec61850d`** | running |
| **`iec61850-mmsd`** | running |
| DUT MD5 | matches USB pack |

File: [DUT_SNAPSHOT_2026-10-15_b.txt](DUT_SNAPSHOT_2026-10-15_b.txt)

## Action for Yahia

Run preflight on **`192.168.10.1`**: single **`ccli`**, stop **`iec61850d`** / **`iec61850-mmsd`**, no duplicate `ccli` restart.

## Masomeh

Keep TSP paths on USB **`CCLI_TSP_TLS_COMPLETE\CCLI_TSP_TLS_COMPLETE\`** (full paths in handoff). Re-browse after any DUT restart.

# TSP Connect still FAIL — round 2 (2026-10-14)

**TSP (Masomeh):** `CCI016_01: Initiate Response Failed: Transport connection closed`  
**PC1:** `192.168.10.10` → DUT `192.168.10.1:3782` · 5LN workspace · TLS paths from `C:\CCLI_TSP_FIELD_PACK\tls\`

## PC1 automated checks — all PASS

| Check | Result |
|-------|--------|
| `verify-dut-pc1-tls-alignment.ps1` (repo/field pack vs DUT) | **PASS** (14 files) |
| `verify-tsp-tls-key-match.ps1` | **PASS** |
| OpenSSL mutual TLS **1.2** | **CONNECTION ESTABLISHED** |
| MD5 `client_tls.pem` | **`13c231ee…`** (matches DUT pin) |

Raw log: [PC1_CONNECT_FAIL_2026-10-14_round2.txt](PC1_CONNECT_FAIL_2026-10-14_round2.txt)

## DUT snapshot (SSH `192.168.10.1`)

| Item | Value |
|------|--------|
| `ccli` | **0.1.0-r55** · one process on **3782** |
| **`iec61850d`** | **RUNNING** (pid 10689) — should be **stopped** for cert bench |
| TLS allowlist on TSP attempt | **match OK** (logread) |
| After TLS | `mms: client CONNECT count=1` then **no** `mms: ACSE auth ACCEPT` |
| Not seen | `acse AARQ auth failed`, `tlsv1 alert access denied` |

Full capture: [DUT_CCLI_LOG_SNAPSHOT_2026-10-14.txt](DUT_CCLI_LOG_SNAPSHOT_2026-10-14.txt)

## Interpretation

Transport TLS and client cert allowlist **succeed** on DUT. Session drops **before or during** ISO presentation / ACSE / MMS Initiate — same pattern as [TSP_INITIATE_RESPONSE_TRIAGE_2026-10-14.md](TSP_INITIATE_RESPONSE_TRIAGE_2026-10-14.md).

**Actions for Yahia:**

1. Run [DUT_MMS_PREFLIGHT_2026-10-14.txt](DUT_MMS_PREFLIGHT_2026-10-14.txt) output — confirm `iec61850d` stopped after preflight.
2. Capture `/tmp/ccli.log` + TSP sniffer (SSL + MMS) on **one** Connect after preflight.
3. If still failing: ACSE/MMS debug (AARE size / 62351-4 responder auth vs TSP MMS Security cert).

**Actions for Masomeh (unchanged):**

- Re-browse **all** TLS paths; MMS = `client.pem` + `client_acse.key`; Transport = `client_tls.pem` + `client_tsp.key`.
- **Allow TLS v1.3** off (TLS 1.2).
- Stack: AP title `1,1,1,999,1`, AE qual `12`, port **3782**.

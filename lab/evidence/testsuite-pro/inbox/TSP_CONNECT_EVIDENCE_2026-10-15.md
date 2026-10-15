# TSP Connect evidence — 2026-10-15 (PC1 Masomeh)

**Git:** `c1f3553` · **DUT:** `ccli 0.1.0-r55` · **3782** · 5-LN **`product_tr400_eth_a_5ln.cid`**

## TSP symptom

```text
CCI016_01: Initiate Response Failed: Transport connection closed
```

(Output sniffer: APP rows only when filter = MMS; no SSL rows captured in UI.)

## PC1

| Check | Result |
|-------|--------|
| Field pack TLS MD5 | DUT pin (`13c231ee…` transport) |
| `verify-tsp-tls-key-match.ps1` | PASS |
| LAN1 | `192.168.10.10/24` → `:3782` (fix subnet from `255.0.0.0`) |

Files: [PC1_EVIDENCE_CAPTURE_2026-10-15.txt](PC1_EVIDENCE_CAPTURE_2026-10-15.txt)

## DUT (SSH snapshot)

| Item | Value |
|------|--------|
| `ccli` on **3782** | yes |
| **`iec61850d`** | **running** (bench should stop) |
| **`iec61850-mmsd`** | **running** |
| Recent MMS | `client CONNECT` then fallback — no sustained session |

File: [DUT_SNAPSHOT_2026-10-15.txt](DUT_SNAPSHOT_2026-10-15.txt)

## Prior art

- [TSP_CONNECT_FAIL_ROUND2_2026-10-14.md](TSP_CONNECT_FAIL_ROUND2_2026-10-14.md) — TLS OK, drop before ACSE ACCEPT
- Handoff: `handoff/masomeh95/PC1_PULL_TLS_DUT_ALIGNED.md` → field pack `00p_PC1_PULL_TLS_DUT_ALIGNED.md`

## Next

1. Yahia: `dut-mms-tsp-preflight` on **`192.168.10.1`** (stop TesPro stack).
2. Masomeh: re-browse **`C:\CCLI_TSP_FIELD_PACK\tls\`**, Connect once, Wireshark `tls` on :3782 if Output has no SSL.

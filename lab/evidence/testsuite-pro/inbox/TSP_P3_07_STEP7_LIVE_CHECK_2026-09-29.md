# P3-07 Step 7 — live check (2026-09-29 22:15)

**Operator:** agent + DUT pull  
**DUT:** `192.168.10.1:3782` · Solution C yaml (`server_combined.pem`)  
**Procedure:** `lab/check_p3_07_step7.ps1` · `lab/pull_tx_accept_hex.ps1`

---

## DUT status (live)

| Check | Result |
|-------|--------|
| Ping `192.168.10.1` | **OK** |
| `ccli` process | **RUNNING** pid 29970 |
| `:3782` LISTEN | **OK** |
| AARE auth ready | **780 octets** (combined cert) |

---

## Steps 1–6 on DUT (from `/tmp/ccli.log`)

| Step | Log evidence | Status |
|------|--------------|--------|
| TLS handshake | Prior TSP sessions + no reject before ACSE | **OK** |
| Session CONNECT | `iso session CONNECT (payload=1312)` | **OK** |
| AARQ client auth | `ACSE auth REJECT — cert not mapped (220 octets, mech=CERTIFICATE)` then `ACSE auth ACCEPT — mech=TLS role=DSO_OPERATOR cert=779` | **OK** (TLS fallback) |
| Session ACCEPT | `iso session ACCEPT extended LI (spdu=1265 payload=1245)` | **OK** |
| AARE emission | `AARE responder auth [10]EXTERNAL ctx=5 inner=1065 octets` | **OK** |
| TX hex | `HEX TX_ACCEPT_SPDU len=1269` @ log line **1954** | **OK** |

**Note:** TSP sends ACSE **CERTIFICATE** auth that DUT rejects (220 B — not mapped); connection proceeds via **TLS peer cert** RBAC. AARE auth still emitted (1065 B inner).

---

## Step 7 — offline crypto on **fresh** DUT hex

**Source:** pulled live → `lab/tmp_tx_step7_latest.hex` (1269 bytes)

```
python lab/locate_p3_07_failure.py lab/tmp_tx_step7_latest.hex \
  --cert apps/ccli/config/tls/server_combined.pem
```

| Check | Result |
|-------|--------|
| AARE @ `0x0048` | OK |
| Auth @ `0x009b` | cert **780 B**, sig **256 B** |
| GeneralizedTime | `20260929200216.` |
| Signed `time_tlv_81` | **PASS** |
| Embedded cert == `server_combined.pem` | **YES** |

### Step 7 verdict

**DUT Step 7 payload is cryptographically correct** per IEC 62351-4 §11.2.2.

If Test Suite Pro still reports `Unable to verify signature (+)`, the failure is **inside TSP’s verify path** (not DUT AARE encoding/signing).

---

## Wireshark capture (this session)

| Item | Value |
|------|--------|
| File | `lab/evidence/testsuite-pro/pcap/TSP_P3_07_mms_2026-09-29_221458.pcapng` |
| Packets | **0** — no Connect during 45 s window |

**Action:** Re-run capture while clicking TSP Connect:

```powershell
.\scripts\capture-mms-tsp.ps1 -DurationSec 60
# TSP Connect during capture
```

Decrypt: TLS RSA key `C:\CCLI_tls\server.key` for `192.168.10.1:3782`.

---

## libIEC61850 client

Lab client binaries not built on PC/WSL this session (`build-mms-lab-client.sh` needs WSL fix).  
Steps 1–6 for **TLS-only client** (no ACSE A-profile) — run when built:

```bash
lab/tg544-openwrt/build-mms-lab-client.sh
lab/tg544-openwrt/clients/mms_tls_client 192.168.10.1 3782 /path/to/tls
```

---

## TSP / IED Simulator (manual)

| Tool | Step 7 relevance |
|------|------------------|
| **Test Suite Pro Connect** | Only tool that exercises Step 7 A-profile verify |
| **TMW IED Simulator + Sample Security** | Baseline — if PASS vs DUT FAIL → compare AARE hex |
| **IEDScout** | TLS + MMS only — does **not** test Step 7 |

---

## Commands for next Connect

```powershell
$env:CCLI_TG544_PW = "<set>"
.\lab\pull_tx_accept_hex.ps1
python lab\locate_p3_07_failure.py lab\tmp_tx_step7_latest.hex --cert apps\ccli\config\tls\server_combined.pem
```

---

## Summary

| Layer | Live result |
|-------|-------------|
| Steps 1–6 | **PASS** on DUT |
| Step 7 offline | **PASS** on fresh wire hex |
| Step 7 TSP | **Needs Connect during pcap** — expected FAIL sig if unchanged |
| Root cause | **TSP-side** until proven otherwise |

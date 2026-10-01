# CCLI — MMS multi-tool verification matrix (P3-07 / Eth_A)

**Document ID:** CCLI-LAB-MMS-MULTI-001  
**Revision:** 1.0  
**Date:** 2026-09-29  
**DUT:** TG544 `ccli` · **CCI016_01** · `192.168.10.1:3782` TLS  
**Companions:** `lab/CCI_TestSuitePro_Verification_Layer.md` · `lab/locate_p3_07_failure.py`  
**RAG source_id:** `ccli-mms-multitool-verification`

---

## Purpose

Confirm MMS / 62351 behaviour using **five independent paths** before blaming only the DUT or only Test Suite Pro:

| # | Tool | Role |
|---|------|------|
| A | **libIEC61850 lab client** | TLS + browse/read (no ACSE A-profile on client) |
| B | **Triangle Test Suite Pro** | Full 62351-3 + 62351-4 A-profile (reference tester) |
| C | **TMW IED Simulator** | Known-good **server** baseline vs TSP client |
| D | **Omicron IEDScout** | Commercial client cross-check (optional licence) |
| E | **Wireshark + DUT hex** | Transport + ACSE byte evidence |

---

## Lab constants

| Item | Value |
|------|--------|
| PC IP | `192.168.10.10` (confirm: `ipconfig`) |
| DUT IP | `192.168.10.1` |
| MMS TLS port | **3782** |
| Cleartext MMS (lib client only if enabled) | 102 — **not** product path |
| CID | `apps/ccli/config/icd/lab_tg544_eth_a.cid` |
| Lab yaml | `apps/ccli/config/lab_tr400_phase1_regulation.yaml` |
| PC TLS staging | `C:\CCLI_tls\` (`scripts/stage-tsp-tls.ps1`) |
| DUT log | `/tmp/ccli.log` |

---

## Phase 0 — DUT health (all tools)

```powershell
# Deploy + confirm ccli listening
$env:CCLI_TG544_PW = "<password>"
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1
```

On DUT (SSH):

```sh
ps w | grep ccli
netstat -tlnp | grep 3782
grep -E 'auth ready|iso session|ACSE' /tmp/ccli.log | tail -15
```

**Pass:** process running, `:3782` LISTEN, `AARE 62351-4 responder auth ready`.

---

## Phase A — libIEC61850 lab client (L-HLP)

Build (WSL or lab host with libiec61850 + mbedtls):

```bash
cd lab/tg544-openwrt
./build-mms-lab-client.sh
```

Run from PC or WSL (certs = repo TLS or `C:\CCLI_tls`):

```bash
# P3-06 TLS connect + read Mod.stVal
./lab/clients/mms_tls_client 192.168.10.1 3782 /path/to/apps/ccli/config/tls

# P3-04 Wlim write (DSO cert)
./lab/clients/mms_tls_wlim_client 192.168.10.1 3782 /path/to/tls dso 12 write

# P3-08 RBAC negative
./lab/clients/mms_tls_wlim_client 192.168.10.1 3782 /path/to/tls viewer 12 write
```

Or on PC if clients built:

```powershell
.\scripts\run-mms-multitool-matrix.ps1 -SkipCapture -SkipTsp
```

| Test | Pass criteria | Gate |
|------|---------------|------|
| `mms_tls_client` | `CONNECT OK (TLS)` + read OK | P3-06 |
| `mms_tls_wlim_client dso write` | Write accepted | P3-04 |
| `viewer write` | Rejected | P3-08 |

**Note:** libiec61850 client uses **TLS (62351-3)** only — it does **not** send MMS Security ACSE A-profile auth. A PASS here proves transport + MMS data path; it does **not** prove TSP A-profile interop.

Save log: `lab/evidence/testsuite-pro/inbox/TSP_P3_06_LIBIEC61850_CLIENT_YYYY-MM-DD.txt`

---

## Phase B — Test Suite Pro (L-VER)

### B.1 Stage TLS + workspace

```powershell
powershell -File scripts\stage-tsp-tls.ps1
```

Launch: `C:\Program Files\Triangle MicroWorks\TMW Test Suite Pro\Bin\TestSuite.exe`

IED Connection → **How to Connect**:

| Setting | Value |
|---------|--------|
| IP | `192.168.10.1` |
| Port | **3782** |
| Transport TLS | ON · `root_CA.pem` · `client.pem` · `client_tsp.key` |
| MMS Security | ON · same CA · `client.pem` · `client_tsp.key` · `lab_crl.pem` |
| Directory to CA | **empty** |
| TLS v1.3 | OFF (lab) |

### B.2 Capture while connecting

Terminal 1:

```powershell
.\scripts\capture-mms-tsp.ps1 -DurationSec 120
```

Terminal 2: TSP **System Status → Connect**

### B.3 Analyse failure layer

If TSP fails with `Unable to verify signature (+)`:

1. DUT: copy latest `TX_ACCEPT` hex from `/tmp/ccli.log`
2. PC:

```powershell
python lab\locate_p3_07_failure.py lab\tmp_tx_latest.hex `
  --cert apps\ccli\config\tls\server_combined.pem
```

3. Wireshark: see Phase E

Save: `TSP_P3_07_CONNECT_YYYY-MM-DD.log` + pcap under `lab/evidence/testsuite-pro/pcap/`

---

## Phase C — TMW IED Simulator (baseline server)

**Goal:** If TSP **Connect passes** to IED Simulator with **Use TMW Sample Security**, but **fails** to CCLI with same TSP profile, the delta is **DUT encoding** (or cert profile). If TSP **also fails** on IED Simulator secure mode, suspect **TSP config/version**.

### C.1 Enable simulator

1. TSP → create/open workspace with **IED Simulator** enabled (see Help: *Simulate an IED from an SCL File*).
2. Load TMW example SCL or `lab_tg544_eth_a.cid`.
3. IED Simulator → **Start** simulated server (note IP/port — often localhost).

### C.2 TSP client → IED Simulator

1. New IED connection pointing at simulator IP/port.
2. Security: right-click **Certificate Authority** → **Use TMW Sample Security**.
3. Connect — record PASS/FAIL.

### C.3 Optional — compare AARE wire

If simulator uses secure MMS, capture with Wireshark and compare ACSE auth shape to DUT `TX_ACCEPT` hex (cert + time + signature layout).

Save: `TSP_P3_07_IEDSIM_BASELINE_YYYY-MM-DD.txt`

---

## Phase D — Omicron IEDScout (optional)

**Licence required.** Use as independent MMS client (same as libiec61850 path for TLS).

| Step | Action |
|------|--------|
| 1 | Import `lab_tg544_eth_a.cid` or discover `192.168.10.1:3782` |
| 2 | Configure TLS mutual auth with `C:\CCLI_tls\` PEMs |
| 3 | Browse `CCI016_01` → read `LLN0.Mod.stVal` |
| 4 | Write `WlimDWMX1.WMaxSptPct` (if control enabled) |

**Pass:** associate + read (and write if supported).  
**Note:** IEDScout may or may not enforce 62351-4 ACSE A-profile — check product security settings.

Save: `TSP_P3_06_IEDSCOUT_CLIENT_YYYY-MM-DD.txt` (use TSP naming prefix for one evidence folder)

---

## Phase E — Wireshark confirmation

### E.1 Capture

```powershell
.\scripts\capture-mms-tsp.ps1 -DurationSec 120
```

### E.2 Decrypt TLS (see inner MMS/ACSE)

Wireshark → **Edit → Preferences → Protocols → TLS → RSA keys**:

| IP | Port | Key file |
|----|------|----------|
| 192.168.10.1 | 3782 | `C:\CCLI_tls\server.key` |

**Decode As:** port 3782 → TLS.

### E.3 What to confirm in trace

| Frame sequence | Meaning |
|----------------|---------|
| TCP SYN → SYN/ACK | L4 OK |
| Client Hello → Server Hello → Finished | TLS OK |
| App data client → server (large) | Session CONNECT + AARQ |
| App data server → client (large) | Session ACCEPT + **AARE** |
| Encrypted Alert ~2–5 ms later | TSP reject (P3-07 signature) |
| Further Initiate Request | **Should appear on PASS** — absent today |

Filters:

```text
tcp.port == 3782
tls && ip.addr == 192.168.10.1
```

### E.4 When TLS decrypt fails

Use **DUT plaintext hex** (authoritative for ACSE):

```sh
grep TX_ACCEPT /tmp/ccli.log | tail -1
```

Copy hex → `lab/tmp_tx_latest.hex` → `locate_p3_07_failure.py`.

---

## Decision matrix (after all phases)

| libiec61850 | IEDScout | TSP → DUT | TSP → IED Sim | Verdict |
|-------------|----------|-----------|---------------|---------|
| PASS | PASS | FAIL sig | PASS | **DUT A-profile delta** — diff AARE vs simulator |
| PASS | PASS | FAIL sig | FAIL | **TSP / profile / version** — open TMW ticket |
| PASS | PASS | PASS | — | **Green** — run P3-03/04 sequencer |
| FAIL | FAIL | FAIL | — | **DUT down / TLS / firewall** — fix Phase 0 |
| PASS | — | FAIL | not tested | **Inconclusive** — run Phase C |

---

## One-command helper (PC)

```powershell
cd C:\Yahia\projects\CCLI
.\scripts\run-mms-multitool-matrix.ps1
```

Options: `-SkipCapture` · `-SkipLibClient` · `-DutHost 192.168.10.1`

---

## Evidence checklist

Copy to `lab/evidence/testsuite-pro/inbox/MMS_MULTITOOL_MATRIX_YYYY-MM-DD.md`:

- [ ] Phase 0 DUT health log excerpt
- [ ] Phase A `mms_tls_client` output
- [ ] Phase A `mms_tls_wlim_client` DSO + viewer
- [ ] Phase B TSP Connect log + screenshot
- [ ] Phase B pcap file path
- [ ] Phase B `locate_p3_07_failure.py` output
- [ ] Phase C IED Simulator baseline result
- [ ] Phase D IEDScout result (or N/A)
- [ ] Phase E Wireshark frame numbers noted

---

## Keywords

`MMS`, `62351`, `Test Suite Pro`, `IED Simulator`, `IEDScout`, `libiec61850`, `Wireshark`, `P3-07`, `ccli-mms-multitool-verification`

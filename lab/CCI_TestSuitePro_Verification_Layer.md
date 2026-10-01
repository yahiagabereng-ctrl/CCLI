# CCLI — Triangle MicroWorks 61850 Test Suite Pro (verification layer)

**Document ID:** CCLI-LAB-TSP-VER-001  
**Revision:** 1.3  
**Date:** 2026-09-26  
**Tool:** Triangle MicroWorks **61850 Test Suite Pro** · **INSTALLED** on lab PC · **v4.7.4.5037**  
**DUT:** TG544 `ccli` IED **CCI016_01** · LAN1 Eth_A `192.168.10.1:3782` TLS  
**Architecture:** `Architecture/CCI_Runtime_Dataflow_Architecture.md` · REQ-VER-001  
**Naming:** **Test Suite Pro** ≠ **TesPro** (TG544 OEM)

**RAG source_id:** `ccli-testsuite-pro-procedure`  
**Corpus:** `knowledge-base/08-engineering/CCI_TestSuitePro_Extract.md` (`ccli-testsuite-pro-extract`)  
**Capture plan:** `knowledge-base/08-engineering/CCI_TestSuitePro_Capture_Plan.md`

---

## 0. Paths (do not improvise)

### 0.1 Lab PC — application install (HAVE)

| What | Path |
|------|------|
| Install root | `C:\Program Files\Triangle MicroWorks\TMW Test Suite Pro\` |
| **Executable** | `C:\Program Files\Triangle MicroWorks\TMW Test Suite Pro\Bin\TestSuite.exe` |
| Vendor Online Help | `C:\Program Files\Triangle MicroWorks\TMW Test Suite Pro\Help\` (`Default.htm`) |
| Vendor print/help PDF | `...\Help\TMW_Print.pdf` |
| Workspaces / config | `C:\ProgramData\Triangle MicroWorks\61850 Test Suite Pro\TestSuitePro\` |

### 0.2 CCLI repo

| What | Path |
|------|------|
| This procedure | `lab/CCI_TestSuitePro_Verification_Layer.md` |
| **Log drop (you)** | `lab/evidence/testsuite-pro/inbox/` |
| Offline SCL exports | `lab/evidence/testsuite-pro/offline-scl/` |
| Evidence README / naming | `lab/evidence/testsuite-pro/README.md` |
| CID under test | `apps/ccli/config/icd/lab_tg544_eth_a.cid` |
| Object freeze | `apps/ccli/config/icd/signal_map.yaml` |
| TLS PEMs | `apps/ccli/config/tls/` |
| Lab MMS yaml | `apps/ccli/config/lab_tr400_phase1_regulation.yaml` |
| Raw TMW web harvest | `knowledge-base/08-engineering/testsuite-pro/raw/` |
| **RAG Online Help copy** | `knowledge-base/08-engineering/testsuite-pro/help-offline/` (**HAVE** — mirrored from install) |
| Download docs | `powershell -File scripts/download-testsuite-pro-docs.ps1` |
| Ingest RAG | `powershell -File scripts/ingest-testsuite-pro.ps1` |

---

## 1. Role in the stack

```text
L-VER  Triangle MicroWorks Test Suite Pro     ← primary Eth_A verification
L-DUT  ccli MMS server (libiec61850)          ← product under test
L-ACT  PF2 → DIO1 + EventRing / stderr        ← actuation evidence
L-HLP  mms_lab_client / mms_wlim_client       ← secondary / CI helpers only
L-RAG  extract + your logs in evidence/       ← Agent triage when you paste
```

**Multi-tool MMS matrix (TSP + IED Simulator + IEDScout + Wireshark):**  
`lab/CCI_MMS_MultiTool_Verification.md` · runner: `scripts/run-mms-multitool-matrix.ps1`

---

## 2. Phase A — Offline now (no DUT) — **app already installed**

### A.0 Launch

```text
C:\Program Files\Triangle MicroWorks\TMW Test Suite Pro\Bin\TestSuite.exe
```

Version check: FileVersion **4.7.4.5037** (confirmed on this PC).

### A.1 Refresh vendor corpus + ingest help

Online Help was mirrored from the install into the repo (`help-offline/`). Re-ingest:

```powershell
cd c:\Yahia\projects\CCLI
powershell -File scripts\ingest-testsuite-pro.ps1 -SkipRag
# If RAG service up:
# powershell -File scripts\ingest-testsuite-pro.ps1
```

### A.2 SCL only (do this next)

1. In Test Suite Pro → open/import:  
   `c:\Yahia\projects\CCLI\apps\ccli\config\icd\lab_tg544_eth_a.cid`  
2. **SCL Viewer** — confirm IED / LD / LNs.  
3. **SCL Verify** — run checks.  
4. Export/copy results to:

```text
c:\Yahia\projects\CCLI\lab\evidence\testsuite-pro\offline-scl\TSP_OFF_SCLVERIFY_YYYY-MM-DD.txt
```

   (Excel export also OK — e.g. `Report.xlsx` → copy into `offline-scl/`.)

5. Paste that log in chat (or leave in `inbox/`) with metadata header — Agent triages vs `signal_map.yaml`.

**Status 2026-09-26:** r1 = 168 findings (ctlModel spacing). Fixed in `lab_tg544_eth_a.cid` rev 3. **r2 = 75 findings (5 Error)** — `Report 2.xlsx` / `TSP_OFF_SCLVERIFY_2026-09-26_r2_TRIAGE.md`. Blocking enum corruption **cleared**; residual Errors = Mod `on-blocked` + SupSubscription (ACCEPT/DEFER).

### A.3 Draft sequencer (do not Connect) — **DRAFTED 2026-09-26**

Full step list + MMS refs: **`lab/CCI_TestSuitePro_Sequencer_Draft.md`**.

Build in TSP **Test Sequencer** (save workspace; optional export under `inbox/`):

| # | Action | Target |
|---|--------|--------|
| 1 | Connect | `192.168.10.1:3782` TLS · client.pem |
| 2 | EnableReport | `LLN0` / `urcb_PdC_Mis4sec` |
| 3 | Operate | `WlimDWMX1.Mod=1`, `WMaxSptPct=70` |
| 4 | Operate | `WSdDAGC1.Mod=1`, `WSptPct=20` |
| 5 | Disconnect + wait **15 s** | P3-05 (`comms_loss_fallback_s`) |

**Status:** Offline draft ready — enter steps in TSP UI; **do not Run/Connect** until §5 Phase B go/no-go.

### A.4 Stage TLS files on PC (no DUT) — **DONE 2026-09-26**

```powershell
powershell -File scripts\stage-tsp-tls.ps1
```

Staged to: `C:\ProgramData\Triangle MicroWorks\61850 Test Suite Pro\CCLI_tls\`

| Role | Files |
|------|-------|
| Trust | `root_CA.pem` |
| DSO write | `client.pem` + `client.key` |
| Viewer | `viewer.pem` + `viewer.key` |
| CRL neg. | `revoked.*` + `lab_crl.pem` |

Point TSP IED Connection Configuration at that folder; leave **Disconnected**.

---

## 3. Phase B — Online (when DUT available)

| Item | Value |
|------|--------|
| PC NIC | `192.168.10.0/24` |
| DUT | `192.168.10.1:3782` |
| TLS | on · DSO client cert |

Minimum live sequence: Connect → EnableReport TotW → Operate Wlim/WSd → save under `inbox/` as `TSP_P3_*`.  
Actuation check outside TSP: `ubus call dido_v2 status`.

---

## 4. Tool map → CCLI gates

| Test Suite Pro module | CCLI path | Gate |
|-----------------------|-----------|------|
| SCL Verify / Viewer | CID | OFF / P3 model |
| Advanced Client | MMS Operate Wlim/WSd | P3-04 |
| Report Viewer | URCB TotW 4 s | P3-03 · P5-M07 |
| Compare Model | CID vs online | P3 |
| Test Sequencer | Scripted evidence | All |
| Sniffer | TLS vs cleartext | P3-06 |
| GOOSE Tracker | — | N/A until GOOSE |

Full extract: `CCI_TestSuitePro_Extract.md` §2.

---

## 5. How you pass logs (contract)

1. Prefer **files** in `lab/evidence/testsuite-pro/inbox/` with `TSP_<GATE>_<TOOL>_<date>.*` names.  
2. Or **paste** in chat with the metadata header (§A.3).  
3. Say: “TSP log ready” — Agent triages against RAG corpus (`ccli-testsuite-pro-*` + Annex T + `signal_map`).  
4. After triage: `powershell -File scripts/ingest-testsuite-pro.ps1` so the log is queryable.

---

## 6. Out of scope for TSP

Modbus meter · DIO electrical · Eth_B 104 · UCA product certificate (P7).

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.2 | 2026-09-26 | Lab PC install paths + Help mirrored; offline starts at SCL Verify |
| 1.3 | 2026-09-26 | A.2 r2 PASS disposition; A.3 sequencer draft; A.4 TLS staged |
| 1.1 | 2026-09-26 | Precise paths · download/ingest · offline log contract for RAG |
| 1.0 | 2026-09-26 | Initial verification-layer definition |

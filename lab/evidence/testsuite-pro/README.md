# Drop zone — DSO MMS verification logs / exports

**Path:** `lab/evidence/testsuite-pro/`  
**Procedure:** `lab/CCI_TestSuitePro_Verification_Layer.md`  
**Test identification (post-Connect):** [TSP_TEST_IDENTIFICATION_POST_CONNECT.md](TSP_TEST_IDENTIFICATION_POST_CONNECT.md)  
**Multi-tool MMS (TSP + IED Simulator + IEDScout + Wireshark + libiec61850):** `lab/CCI_MMS_MultiTool_Verification.md`  
**RAG:** paste or save here → Agent ingests via `ccli-testsuite-pro-lab-logs` / session extract

**2026-10-01:** Test Suite Pro license **blocked** (expired Sentinel trial). New manual DSO runs may use **IEDExplorer** — prefix `IEDEX_` instead of `TSP_`. Historical TSP exports remain valid for closed gates. See `lab/evidence/phase5/P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md`.

## Folders

| Folder | Use |
|--------|-----|
| `inbox/` | **Drop raw exports here first** (you paste / copy) |
| `offline-scl/` | SCL Verify / SCL Viewer exports (no DUT) |
| `YYYY-MM-DD_*` | Session folders after triage |

## Naming (required)

```text
TSP_<GATE>_<TOOL>_<YYYY-MM-DD>.<ext>

Examples:
  TSP_OFF_SCLVERIFY_2026-09-26.txt
  TSP_OFF_SCLVIEWER_CCI016_01_2026-09-26.png
  TSP_P3_01_CONNECT_2026-09-26.log
  TSP_P3_03_REPORT_TotW_2026-09-26.csv
  TSP_P3_04_OPERATE_WSd_2026-09-26.txt
  TSP_P3_06_TLS_FAIL_viewer_2026-09-26.log
  IEDEX_P5_FULLCIRCLE_CLEARTEXT_2026-10-01_153000.txt
```

| Token | Values |
|-------|--------|
| GATE | `OFF` (offline) · `P3_01` · `P3_03` · `P3_04` · `P3_05` · `P3_06` · `P5_M07` · `P5_R01` · … |
| TOOL | `SCLVERIFY` · `SCLVIEWER` · `CONNECT` · `REPORT` · `OPERATE` · `COMPARE` · `SNIFFER` · `SEQ` · `SYSSTATUS` |

## What to export from Test Suite Pro

| Tool | Preferred export |
|------|------------------|
| SCL Verify | Result list (copy text / CSV / screenshot) |
| System Status | Errors/Warnings list |
| Advanced Client | Connection status + Operate result |
| Report Viewer | Recording / period notes |
| Execution Log (Sequencer) | Full step log |
| Sniffer / Protocol Analyzer | pcapng or text dump |
| Compare Model | Diff list vs CID |

## Metadata header (paste at top of every .txt log)

```text
# CCLI TSP evidence
tsp_version: 4.7.4
pc_ip: 192.168.10.x
dut_ip: 192.168.10.1
dut_port: 3782
tls: true|false
cid: apps/ccli/config/icd/lab_tg544_eth_a.cid
ccli_pkg: 0.1.0-rNN
gate: OFF|P3_xx
tool: SCLVERIFY|…
operator: <name>
date: YYYY-MM-DD
```

## Chat workflow

1. Save file under `inbox/` **or** paste log in chat with the metadata header.  
2. Agent triages → moves to `offline-scl/` or session folder → updates gap list vs `signal_map.yaml`.  
3. Re-ingest: `pwsh scripts/ingest-testsuite-pro.ps1`

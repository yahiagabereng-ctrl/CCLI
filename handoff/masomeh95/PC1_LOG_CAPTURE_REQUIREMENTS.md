# PC1 (Masomeh) — mandatory TestPro log capture

**Role:** PC-B · LAN1 `192.168.10.10` → DUT `192.168.10.1:3782`  
**Date:** 2026-10-14  
**Matrix:** unified cert test matrix rows **U-T1-*** · **U-T3-*** · **U-P5-01**  
**Full map:** repo `lab/conformance/LAB_UNIFIED_TSP_5LN_EXECUTION.md` (pack: `00g_TSP_5LN_EXECUTION_AND_LOGS.md`)

---

## Requirements (must satisfy for cert evidence)

| ID | Requirement |
|----|-------------|
| REQ-PC1-LOG-001 | Install **Wireshark** (includes **tshark**) on PC1 — default path `C:\Program Files\Wireshark\` |
| REQ-PC1-LOG-002 | Run **`tsp-evidence-logger.ps1 -Action Init`** once per test day **before** first **Start** |
| REQ-PC1-LOG-003 | Each matrix row = exactly one **`Start` → one TSP step → `Stop -PasteClipboard`** pair |
| REQ-PC1-LOG-004 | Every PC-B row produces **four** artefacts: `.txt`, `_DUT_LOG.txt`, `_WIRE.txt`, `.pcapng` |
| REQ-PC1-LOG-005 | Set **`result: PASS`** or **`result: FAIL`** in each `.txt` header after paste |
| REQ-PC1-LOG-006 | **U-T1-02 Compare:** export TSP **Compare `.xlsx`** beside `TSP_P3_COMPARE_<date>.txt` |
| REQ-PC1-LOG-007 | Closeout **U-CLO-01:** copy `inbox/` + `pcap/` to Yahia → `lab/evidence/testsuite-pro/` |

---

## Where files go (pick one root — stay consistent all day)

### Option A — Field pack only (no full git clone)

```text
D:\CCLI_TSP_FIELD_PACK\
  scripts\tsp-evidence-logger.ps1
  lab\evidence\testsuite-pro\inbox\     ← TSP_*.txt, *_DUT_LOG, *_WIRE
  lab\evidence\testsuite-pro\pcap\      ← TSP_*.pcapng
```

```powershell
cd D:\CCLI_TSP_FIELD_PACK
```

### Option B — Full repo (recommended if you also commit evidence)

```text
D:\CCLI\CCLI\lab\evidence\testsuite-pro\inbox\
D:\CCLI\CCLI\lab\evidence\testsuite-pro\pcap\
```

```powershell
cd D:\CCLI\CCLI
```

**Do not** mix Option A and B on the same session without copying files together at end of day.

---

## Prerequisites on PC1

| Tool | Purpose |
|------|---------|
| Wireshark / tshark | Live capture filter `host 192.168.10.1 and port 3782` |
| PuTTY **plink** + **pscp** (optional) | DUT SSH PRE/POST in `_DUT_LOG.txt` |
| Test Suite Pro | One step per logger cycle |

**SSH to DUT (LAN1):** Yahia provides DUT password once. Before **Start**:

```powershell
$env:CCLI_TG544_PW = "<from Yahia — do not commit>"
```

If SSH is unavailable, use **`-NoSsh`** on **Start** (still capture Wireshark + TSP paste; note gap in session index).

**NIC name:** logger default `-InterfaceAlias "Ethernet"`. If capture is empty, run `Get-NetAdapter` and pass e.g. `-InterfaceAlias "Ethernet 2"`.

---

## Day start (once)

```powershell
$dt = Get-Date -Format yyyy-MM-dd
powershell -File scripts\tsp-evidence-logger.ps1 `
  -Action Init `
  -SessionDate $dt `
  -Operator Masomeh `
  -CcliPkg "0.1.0-r52" `
  -TspVersion "<TSP Help → About>"
```

Creates stubs for **T1-01 … T1-10**, **T1-05b/c/d**, **T3-01 … T3-05**, **P5-01** and:

`lab\evidence\testsuite-pro\inbox\TSP_UNIFIED_5LN_SESSION_INDEX_<date>.md`

Fill matrix session header in `00_UNIFIED_CERT_TEST_MATRIX.md` (same date, TSP version, operator).

---

## Per-test workflow (repeat for every row)

Example **U-T1-07** (Operate Wlim):

```powershell
powershell -File scripts\tsp-evidence-logger.ps1 -Action Start -TestId T1-07 -Operator Masomeh
```

1. Run **exactly one** TSP action (Operate Wlim).  
2. In TSP: select **Output** → **Copy**.  
3. Stop:

```powershell
powershell -File scripts\tsp-evidence-logger.ps1 -Action Stop -PasteClipboard
```

4. Open the `.txt` under `inbox\`; set `result: PASS` or `FAIL` if not obvious from paste.  
5. Tick **□** on matching **U-ID** in `00_UNIFIED_CERT_TEST_MATRIX.md` and the session index.

**Alias:** `-UnifiedId U-T1-07` same as `-TestId T1-07`.

---

## Logger `-TestId` ↔ matrix (PC-B only)

| U-ID | `-TestId` | Evidence basename (under `inbox/` + `pcap/`) |
|------|-----------|-----------------------------------------------|
| U-T1-01 | T1-01 | `TSP_P3_01_CONNECT_*` |
| U-T1-02 | T1-02 | `TSP_P3_COMPARE_*` + **`.xlsx`** |
| U-T1-03 | T1-03 | `TSP_P3_03_BROWSE_*` |
| U-T1-04 | T1-04 | `TSP_P3_03_READ_PdC_*` |
| U-T1-05 | T1-05 | `TSP_P3_03_REPORT_URCB_*` |
| U-T1-05b | T1-05b | `TSP_P3_03_REPORT_DCHG_*` |
| U-T1-05c | T1-05c | `TSP_P3_03_REPORT_QCHG_*` |
| U-T1-05d | T1-05d | `TSP_P3_03_REPORT_DUPD_*` |
| U-T1-06 | T1-06 | `TSP_P3_03_REPORT_TotW_*` |
| U-T1-07 | T1-07 | `TSP_P3_04_OPERATE_Wlim_*` |
| U-T1-08 | T1-08 | `TSP_P3_04_OPERATE_WSd_*` |
| U-T1-09 | T1-09 | `TSP_P3_05_FALLBACK_*` |
| U-T1-10 | T1-10 | `TSP_P3_01_RECONNECT_*` |
| U-T3-01 | T3-01 | `TSP_P3_06_TLS_FAIL_*` |
| U-T3-02 | T3-02 | `TSP_P3_08_VIEWER_READ_*` |
| U-T3-03 | T3-03 | `TSP_P3_08_VIEWER_OPERATE_DENY_*` |
| U-T3-04 | T3-04 | `TSP_P3_09_CRL_*` |
| U-T3-05 | T3-05 | `TSP_P3_07_DSO_ACSE_*` |
| U-P5-01 | P5-01 | `TSP_P5_FULLCIRCLE_TLS_*` |

**Not on PC1:** plant/104/O.14 rows (PC-A) — no Wireshark logger; see `00g` PC-A table.

---

## Four artefacts (per row)

| # | File | Content |
|---|------|---------|
| 1 | `TSP_<gate>_<tool>_<date>.txt` | Header + pasted TSP Output |
| 2 | `..._DUT_LOG.txt` | SSH PRE (Start) + POST (Stop) |
| 3 | `..._WIRE.txt` | tshark frame summary (auto at Stop) |
| 4 | `pcap/TSP_..._<date>.pcapng` | TLS MMS to :3782 |

Pass check (spot): pcap **≥ ~128 bytes**, `_WIRE.txt` shows frames, TLS to **3782**.

---

## Troubleshooting

| Issue | Action |
|-------|--------|
| `Evidence session active` | `-Action Stop` then **Start** again |
| Tiny / empty pcap | Fix LAN1 (`bench_scripts\set_pc_lan1_dso.cmd`), ping `.10.1`, correct `-InterfaceAlias` |
| No Wireshark | Install Wireshark; or `-NoCapture` on Start (TSP log only — document waiver) |
| `Capture already active` / stuck tshark | `-Action Stop`; kill tshark in Task Manager if needed |
| Compare | Export `.xlsx` manually; save `validate-5ln-cid.ps1` output to `inbox\VALIDATE_5LN_CID_<date>.txt` |

---

## Return to Yahia (U-CLO-01)

USB or shared folder:

```powershell
robocopy D:\CCLI_TSP_FIELD_PACK\lab\evidence\testsuite-pro\inbox \\YAHIA\share\inbox /E
robocopy D:\CCLI_TSP_FIELD_PACK\lab\evidence\testsuite-pro\pcap \\YAHIA\share\pcap /E
```

Or `git pull` on repo PC after you push a branch with evidence (coordinate with Yahia).

---

## Read order in field pack

1. `00_START_PC1_MASOMEH.md`  
2. **`00h_PC1_LOG_CAPTURE_REQUIREMENTS.md`** (this file in pack)  
3. `00g_TSP_5LN_EXECUTION_AND_LOGS.md`  
4. `05b_EVIDENCE_PER_TEST_PROTOCOL.md`  
5. `00_UNIFIED_CERT_TEST_MATRIX.md`

**Keywords:** Masomeh, PC1, tsp-evidence-logger, Wireshark, unified matrix, REQ-PC1-LOG

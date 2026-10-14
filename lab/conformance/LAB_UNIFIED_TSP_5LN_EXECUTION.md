# Test Suite Pro — 5-LN unified matrix execution & log capture

**Document ID:** CCLI-LAB-UNIFIED-TSP-001  
**Revision:** 1.1  
**Date:** 2026-10-14  
**Matrix rev:** 1.2 (Logger column + RAG corpus)  
**RAG source_id:** `ccli-lab-unified-tsp-5ln-execution`  
**Matrix:** [LAB_UNIFIED_CERT_TEST_MATRIX.md](LAB_UNIFIED_CERT_TEST_MATRIX.md)  
**Evidence protocol:** [../evidence/testsuite-pro/EVIDENCE_PER_TEST_PROTOCOL.md](../evidence/testsuite-pro/EVIDENCE_PER_TEST_PROTOCOL.md)  
**Logger:** `scripts/tsp-evidence-logger.ps1`

This is the **TestPro finalize** view: every **PC-B TSP** row mapped to **Start/Stop** logger IDs, TSP actions, and **mandatory artefacts**.

---

## Requirements

| ID | Requirement |
|----|-------------|
| REQ-TSP-5LN-001 | TSP Compare/Browse/MICS use **5-LN** CID (`product_tr400_eth_a_5ln.cid` when in repo) |
| REQ-TSP-5LN-002 | Browse `LD_Plant` shows **exactly 5** MMS names (binding table in matrix) |
| REQ-TSP-EV-001 | Each **U-T1-*** / **U-T3-*** / **U-P5-01** on PC-B: **txt + _DUT_LOG + _WIRE + pcapng** per protocol |
| REQ-TSP-EV-002 | Session **`Init`** once per day before first **Start** |
| REQ-TSP-EV-003 | **Compare** row: attach exported **Compare xlsx** beside `TSP_P3_COMPARE_*.txt` |

---

## TSP project (PC-B) — lock before Phase 1

| Setting | Value |
|---------|--------|
| IED name | `CCI016_01` |
| Host | `192.168.10.1` |
| Port | **3782** |
| Security | **TLS** mutual auth |
| TLS trust | Field pack `tls/root_CA.pem` |
| Client cert | `tls/client.pem` + key **`client_tsp.key`** (TSP OpenSSL quirk) |
| Compare CID | **`product_tr400_eth_a_5ln.cid`** (interim: document waiver in Compare log) |
| Lab reference only | `lab_tg544_eth_a.cid` — **not** cert Compare |

---

## Day start (PC-B)

```powershell
cd D:\CCLI\CCLI   # repo on PC1
. lab\tg544-openwrt\lab-env.ps1   # optional: DUT SSH on LAN1

$dt = Get-Date -Format yyyy-MM-dd
powershell -File scripts\tsp-evidence-logger.ps1 -Action Init -SessionDate $dt -Operator Masomeh -CcliPkg "0.1.0-r53" -TspVersion "<TSP build>"
```

Creates stubs under `lab/evidence/testsuite-pro/inbox/` and `pcap/`, plus **`TSP_UNIFIED_5LN_SESSION_INDEX_<date>.md`**.

**PC-A** in parallel: complete matrix **U-P0-*** / **U-P0b-*** before **U-T1-04** needs live P/Q/PPV.

---

## PC-B TSP rows — logger map (mandatory capture)

**Workflow each row:** `-Action Start` → one TSP action → copy Output → `-Action Stop -PasteClipboard` → set `result: PASS|FAIL` in `.txt` header.

| U-ID | Logger `-TestId` | TSP action (single step) | Evidence base (`inbox/` + `pcap/`) |
|------|------------------|---------------------------|-------------------------------------|
| U-T1-01 | `T1-01` | **Connect** mutual TLS | `TSP_P3_01_CONNECT_*` |
| U-T1-02 | `T1-02` | **Compare Model** vs 5-LN CID (pre-URCB) | `TSP_P3_COMPARE_*` + `.xlsx` |
| U-T1-03 | `T1-03` | **Browse** `LD_Plant` — confirm **5 LNs** | `TSP_P3_03_BROWSE_*` |
| U-T1-04 | `T1-04` | **Read** `PdCMMXU1` TotW, TotVAr, PPV | `TSP_P3_03_READ_PdC_*` |
| U-T1-05 | `T1-05` | **SetURCB** `urcb_PdC_Mis4sec` IntgPd=4000, RptEna=true | `TSP_P3_03_REPORT_URCB_*` |
| U-T1-05b | `T1-05b` | SetURCB **dchg**; PC-A changes P (meter/Sun) | `TSP_P3_03_REPORT_DCHG_*` |
| U-T1-05c | `T1-05c` | SetURCB **qchg** | `TSP_P3_03_REPORT_QCHG_*` |
| U-T1-05d | `T1-05d` | SetURCB **dupd** | `TSP_P3_03_REPORT_DUPD_*` |
| U-T1-06 | `T1-06` | **Report Viewer**: integrity period ~4 s | `TSP_P3_03_REPORT_TotW_*` |
| U-T1-07 | `T1-07` | **Operate** Wlim enhanced (Mod + WMaxSptPct) | `TSP_P3_04_OPERATE_Wlim_*` |
| U-T1-08 | `T1-08` | **Operate** WSd (Mod + WSptPct) | `TSP_P3_04_OPERATE_WSd_*` |
| U-T1-09 | `T1-09` | **Release** association · 15 s fallback | `TSP_P3_05_FALLBACK_*` |
| U-T1-10 | `T1-10` | **Disconnect** · **Connect** again | `TSP_P3_01_RECONNECT_*` |
| U-T3-01 | `T3-01` | Connect **without** client cert → must **fail** | `TSP_P3_06_TLS_FAIL_*` |
| U-T3-02 | `T3-02` | **Viewer** cert: read OK | `TSP_P3_08_VIEWER_READ_*` |
| U-T3-03 | `T3-03` | Viewer **Operate Wlim** → **deny** | `TSP_P3_08_VIEWER_OPERATE_DENY_*` |
| U-T3-04 | `T3-04` | **Revoked** client + CRL | `TSP_P3_09_CRL_*` |
| U-T3-05 | `T3-05` | DSO / dual cert ACSE (normal profile) | `TSP_P3_07_DSO_ACSE_*` |
| U-P5-01 | `P5-01` | Run **5-LN** annex trace ([P5_FULLCIRCLE_5LN_SCOPE.md](P5_FULLCIRCLE_5LN_SCOPE.md)) on **3782 TLS** | `TSP_P5_FULLCIRCLE_TLS_*` |

**Alias:** `-UnifiedId U-T1-07` works the same as `-TestId T1-07`.

**U-T1-11 (PC-A):** append `ubus call dido_v2 status` to **`TSP_P3_04_OPERATE_Wlim_*_DUT_LOG.txt`** (POST already pulled by logger).

---

## PC-A / SSH rows — log naming (no Wireshark on PC-B)

| U-ID | Artefact | Path / pattern |
|------|----------|----------------|
| U-P0-02/03/04 | Preflight | `inbox/PREFLIGHT_LAN1|LAN2|LAN3_<date>.txt` (manual ping output) |
| U-P0b-01 | Meter probe | `inbox/modbus_emt432_<host>_<date>.txt` (redirect probe stdout) |
| U-P0b-02 | Sun mock | `inbox/PLANT_SUN2000_COM5_<date>.txt` (first 50 lines RTU log) |
| U-P0b-03/04 | Events | `inbox/P7_04_MODBUS_LINK_<date>.txt`, `P7_01_WRAP_<date>.txt` |
| U-P3-11 | Time sync | `inbox/P3_11_CHRONY_<date>.txt` (`chronyc tracking` on DUT) |
| U-P4-01/02 | IEC 104 | `lab/evidence/phase4/P4_*_<date>.txt` |
| U-P7-01…04 | O.14 | `lab/evidence/phase7/P7_*_<date>.txt` |
| U-DOC-05 | Offline SCL | `offline-scl/TSP_OFF_SCLVERIFY_<date>.txt` |

---

## Phase order on TestPro (recommended)

1. **Phase 0** matrix prep (no TSP traffic) — **U-P0-06** CID/TLS import only.  
2. **Phase 1** — **U-T1-01 → U-T1-10** in table order (Compare **before** enabling URCB if sequencing P5 step 3).  
3. **Phase 2** — **U-P5-01** after T1 core (or fold into one day after T1-07/08).  
4. **Phase 3** — **U-T3-01 → U-T3-05** (swap TLS profiles between rows; reconnect as needed).  
5. **Phase 6** — **U-CLO-01**: robocopy `inbox/` + `pcap/` to USB or git push from logger PC.

**Do not** run 31-LN P5 sequencer or VArSd/PFSP steps on this cert run.

---

## Closeout checklist

| □ | Item |
|---|------|
| □ | Every PC-B row above has **result: PASS** or **PART + waiver ID** in `.txt` |
| □ | `TSP_UNIFIED_5LN_SESSION_INDEX_<date>.md` updated |
| □ | Matrix session header filled (TSP version, `ccli --version`, cfg SHA) |
| □ | `validate-5ln-cid.ps1` log saved for **U-T1-02** |
| □ | **U-CLO-01** → `lab/evidence/testsuite-pro/inbox/` |
| □ | **U-CLO-03** ingest on repo PC |

```powershell
powershell -File scripts\ingest-testsuite-pro.ps1
```

---

## Verification

| Requirement | Test | Pass |
|-------------|------|------|
| REQ-TSP-5LN-002 | T1-03 paste Browse list | 5 names only |
| REQ-TSP-EV-001 | Spot-check 3 pcaps | ≥128 B, TLS to :3782 |
| REQ-TSP-5LN-001 | Compare xlsx LN count | Matches 5-LN CID |

**Keywords:** TestPro, 5-LN, evidence, tsp-evidence-logger, unified matrix

# Test Suite Pro — test identification (post-Connect)

**Document ID:** CCLI-LAB-TSP-TEST-ID-001  
**Revision:** 1.0  
**Date:** 2026-10-08  
**Status:** Connect **PASS** on `:3782` TLS (update header below on first green run)  
**RAG source_id:** `ccli-tsp-test-identification`  
**DUT:** TG544 · IED **`CCI016_01`** · **`192.168.10.1:3782`** · yaml `lab_tr400_phase1_regulation.yaml` (or phase4 combined if 104 also up)  
**CID:** `apps/ccli/config/icd/lab_tg544_eth_a.cid`  
**PICS scope (first pass):** [`../../conformance/2026-10-08_UCA_Document_Phase_Checklist.md`](../../conformance/2026-10-08_UCA_Document_Phase_Checklist.md) §3 — server, 8-1 MMS, URCB, control, association; **exclude** BRCB/GOOSE/SV/files/SG unless you expand PICS  

**Companion procedures:**  
[`../../CCI_TestSuitePro_Verification_Layer.md`](../../CCI_TestSuitePro_Verification_Layer.md) · [`../../CCI_TestSuitePro_Sequencer_Draft.md`](../../CCI_TestSuitePro_Sequencer_Draft.md) · [`../phase5/P5_TSP_FULLCIRCLE_SEQUENCER.md`](../phase5/P5_TSP_FULLCIRCLE_SEQUENCER.md) · [`../../../knowledge-base/08-engineering/CCI_61850-10_Extract.md`](../../../knowledge-base/08-engineering/CCI_61850-10_Extract.md) §4.4–§9  

---

## Session header (fill once per run)

```text
tsp_version:
ccli_pkg:          # ccli --version on DUT
connect_result:    PASS
date:
operator:
firmware_manifest: # deploy-manifest id if any
```

**Evidence drop:** `lab/evidence/testsuite-pro/inbox/` · naming [`README.md`](README.md)

---

## 1. What you are proving (three layers)

| Layer | Standard / audience | Tool |
|-------|---------------------|------|
| **A — CEI Annex T function** | Allegato T + O.9.2.x (Wlim, WSd, 4 s P/Q/V) | TSP Sequencer + Advanced Client + Report Viewer |
| **B — IEC 61850-10 server conformance** | UCA lab prep (maps to **PICS** rows) | TSP built-in **Server** test groups where available + manual steps below |
| **C — Transport / MMS security** | 62351-3 TLS · 62351-4 ACSE | Connect + Sniffer/pcap; formal **62351-100-*** is accredited lab |

TSP **does not** issue the UCA certificate; it produces **traceable logs** for PICS claims and internal gates **P3 / P5**.

---

## 2. Run order (recommended)

```text
Offline SCL Verify (optional refresh)
    → Connect :3782 (record System Status)
    → Compare Model vs CID
    → Tier 1 (P0 minimum)
    → Tier 2 (Annex T full-circle on TLS)
    → Tier 3 (security negatives)
    → Tier 4 only if claimed in PICS
    → Export Sequencer log + pcap
```

---

## 3. Tier 1 — **P0** (run first; matches minimum PICS / 61850-10 §9)

These align with **sAss**, **sSrv**, **sRp**, **sCtl** in [`CCI_61850-10_Extract.md`](../../../knowledge-base/08-engineering/CCI_61850-10_Extract.md).

| ID | Gate | TSP module | Action | Objects / notes | 61850-10 | Evidence |
|----|------|------------|--------|-----------------|----------|----------|
| T1-01 | P3-01 | System Status | **Connect** TLS mutual | `CCI016_01` · `client_tls.pem` + Cert B as configured | sAss | `TSP_P3_01_CONNECT_*.log` |
| T1-02 | P3 | Compare Model | Online vs CID | Run **before** enabling URCB (less noise) | Table 3 | `TSP_P3_COMPARE_*.xlsx` |
| T1-03 | P3-03 | Advanced Client | **GetServerDirectory** / LD / LN browse | `LD_Plant` · expect full LN count (P5: ~31) | sSrv | Sequencer or client log |
| T1-04 | P3-03 | Advanced Client | **GetDataValues** | `PdCMMXU1$MX$TotW`, TotVAr, PPV | sSrv | `TSP_P3_03_READ_PdC_*.txt` |
| T1-05 | P3-03 | Report Viewer | **SetURCBValues** + enable | `LLN0.urcb_PdC_Mis4sec` · IntgPd=**4000** · RptEna=true | sRp Table 14 | `TSP_P3_03_REPORT_TotW_*.csv` |
| T1-06 | P3-03 | Report Viewer | Observe **integrity period** | Δt **3900–4100 ms** · TotW in payload | sRp | same + timing notes |
| T1-07 | P3-04 | Advanced Client | **Operate** (enhanced security) | `WlimDWMX1.Mod`=1 · `WMaxSptPct`=10 or 70 | sCtl Table 26 | `TSP_P3_04_OPERATE_Wlim_*.txt` |
| T1-08 | P3-04 | Advanced Client | **Operate** | `WSdDAGC1.Mod`=1 · `WSptPct`=20 | sCtl | `TSP_P3_04_OPERATE_WSd_*.txt` |
| T1-09 | P3-05 | Sequencer | **Release** + wait **15 s** | `comms_loss_fallback_s` in yaml | App + O.9.2.2 | `TSP_P3_05_FALLBACK_*.txt` |
| T1-10 | P3-01 | System Status | **Disconnect** / re-**Connect** | Clean associate/release | sAss | System Status export |

**ctlModel in CID:** `direct-with-enhanced-security` on Wlim/WSd — use TSP **Operate** path that matches enhanced direct (not plain direct-with-normal-security).

**Minimum sequencer:** steps 1–6 in [`CCI_TestSuitePro_Sequencer_Draft.md`](../../CCI_TestSuitePro_Sequencer_Draft.md) — extend with T1-03/T1-04 reads if not already in workspace.

---

## 4. Tier 2 — **Annex T / P5** (same as full-circle, port **3782**)

Re-run [`P5_TSP_FULLCIRCLE_SEQUENCER.md`](../phase5/P5_TSP_FULLCIRCLE_SEQUENCER.md) steps on **TLS** (not cleartext :102). Adds CEI traceability beyond minimum UCA blocks.

| Steps (P5 doc) | Focus | Extra vs Tier 1 |
|----------------|-------|-----------------|
| 0 | chrony ±100 ms on DUT | T.3.3.4.5 |
| 12b | LN count under `LD_Plant` | TR 57-126 |
| 12 | Compare Model | waivers doc |
| 2–4 | TotW / TotVAr / PPV read | O.8.3 |
| 5–5d | URCB payload t, q, same timestamp | REQ-MET-001/002 |
| 6–7 | Mod baseline reads Wlim/WSd | Tab. 85–86 |
| 8–11 | Wlim, WSd, **VArSd** operate | O.9.2.x / O.9.1.4 |
| 13 | Modbus P sweep (if plant sim running) | dynamic read |
| 14 | Disconnect 15 s fallback | O.11 / P3-05 |

**Evidence:** `TSP_P5_FULLCIRCLE_TLS_YYYY-MM-DD.log` (Sequencer execution log) + DUT `ubus call dido_v2 status` after Wlim.

**Preconditions:** Modbus plant sim if you need TotW ≈ 450 kW; GNSS/chrony for quality fields.

---

## 5. Tier 3 — Security (62351 / Annex T tables; not 61850-10 Table 14)

Run as **separate** sequencer runs or manual connects.

| ID | Gate | Action | Expect | Evidence |
|----|------|--------|--------|----------|
| T3-01 | P3-06 | Connect **without** client cert | TLS fail | `TSP_P3_06_TLS_FAIL_*.log` |
| T3-02 | P3-08 | Connect as **viewer** · read OK | Browse/read OK | `TSP_P3_08_VIEWER_READ_*.txt` |
| T3-03 | P3-08 | Viewer **Operate** Wlim | **Denied** | same |
| T3-04 | P3-09 | Connect with **revoked** cert + CRL | **Fail** | `TSP_P3_09_CRL_*.txt` |
| T3-05 | P3-07 | Connect with DSO cert | ACSE **ACCEPT** (already proven if Connect green) | DUT log + System Status |

Optional wire: `scripts/capture-mms-tsp.ps1` during T3-05 → `pcap/TSP_P3_07_mms_*.pcapng`

---

## 6. Tier 4 — **Only if marked M in PICS** (otherwise N/A at accredited lab)

| Feature | CID hint | TSP module | 61850-10 | Notes |
|---------|----------|------------|----------|--------|
| **Buffered reporting** | `brcb_Stato_Allarmi_Segnali` | Report Viewer | sBr 16–17 | Extra test days; exclude from first PICS if possible |
| **GOOSE publish** | `gcb_PdC_Mis4sec`, `gcb_Stato_Allarmi` | GOOSE Tracker / Sniffer | sGop 20–25 | P5-G02 wire evidence exists; TSP if claimed |
| **File transfer** | `<FileHandling />` in CID | File transfer tool | sFt 33–34 | Confirm runtime before claiming |
| **Setting groups** | — | — | sSg | Not in MVP claim |
| **Time sync (server)** | TimeSyncProt in CID | Time / read clock | sTm 31–32 | chrony P3-11 + TSP read UTC |
| **Second URCB** | urcb_GenAcc / urcb_SingGen | Report Viewer | sRp | Repeat T1-05 pattern |

---

## 7. Not in Test Suite Pro (do not block 61850 PICS tier 1)

| Topic | Where |
|-------|--------|
| IEC **60870-5-104** `:2404` Eth_B | `lab/evidence/phase4/` · lib60870 client |
| DIO / PF2 electrical | `P3_04_WLIM_ACTUATOR.txt` · ubus |
| Formal **UCA** automated suite | Accredited lab after PICS |
| **62351-100-3** certificate | Separate PID + lab |
| TesPro supplier **iec61850d** | P4-00 / P5 northbound MQTT |

---

## 8. TSP built-in conformance vs custom Sequencer

| Approach | When to use |
|----------|-------------|
| **Vendor “Server” / conformance test tree** (61850-10 abstract cases) | After PICS filled — run groups matching **M** rows (association, unbuffered report, control). Filter out BRCB/GOOSE if N/A in PICS. |
| **Custom Test Sequencer** (this programme) | Annex T traceability, Wlim/WSd/fallback, repeatable evidence names |

If the TSP UI lists test groups by **table number** (e.g. Table 14 reporting), cross-check to §4.4 in `ccli-61850-10-extract`. **Positive + negative** cases exist per service group — run negatives for sSrv/sAss when preparing for formal lab.

---

## 9. Pass criteria summary (internal exit for this milestone)

| Milestone | Criteria |
|-----------|----------|
| **Connect milestone** | T1-01 PASS · System Status no Initiate/Connect errors |
| **PICS-minimum TSP pack** | T1-01…T1-10 PASS · Compare documented (waivers OK) · one pcap optional |
| **Annex T TLS pack** | Tier 2 full-circle log on **3782** · Wlim DIO check once |
| **Security pack** | T3-01…T3-03 PASS (T3-04 if CRL loaded on DUT) |

Update [`CCLI_PHASE_LAB_MASTER_LOG.md`](../../CCLI_PHASE_LAB_MASTER_LOG.md) §3.7 when Connect + Initiate path is green (date, tsp_version, ccli_pkg, link to `TSP_P3_01_CONNECT_*.log`).

---

## 10. Quick reference — MMS paths

```text
IED:     CCI016_01
LD:      LD_Plant
URCB:    CCI016_01LD_Plant/LLN0.urcb_PdC_Mis4sec
Wlim:    CCI016_01LD_Plant/WlimDWMX1.Mod
         CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct
WSd:     CCI016_01LD_Plant/WSdDAGC1.Mod
         CCI016_01LD_Plant/WSdDAGC1.WSptPct
VArSd:   CCI016_01LD_Plant/VArSdDVAR1.Mod
         CCI016_01LD_Plant/VArSdDVAR1.VArTgtSptPct
PdC:     CCI016_01LD_Plant/PdCMMXU1$MX$TotW
```

---

**RAG tags:** `TSP`, `61850-10`, `PICS`, `P3`, `P5`, `Annex-T`, `3782`, `ccli-tsp-test-identification`

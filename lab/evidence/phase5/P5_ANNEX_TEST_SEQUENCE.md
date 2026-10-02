# P5 — Annex O / Annex T test sequence (lab)

**Document ID:** P5-ANNEX-SEQ-001  
**Date:** 2026-10-01  
**Gate:** P5_FULLCIRCLE — **CLOSED** (lab) 2026-10-01  
**Build:** `0.1.0-r25`  
**DUT:** TG544 · Eth_A `192.168.10.1:102` cleartext  
**Operator UI:** Test Suite Pro (r25) · **IEDExplorer** (re-runs) · DUT shell (`chronyc`, `logread`)  
**Corpus:** `Architecture/_extracted_reg_analysis/annex_o.txt`, `annex_t.txt`  
**Companion:** [P5_TIMING_ANNEX_AUDIT_2026-10-01.md](P5_TIMING_ANNEX_AUDIT_2026-10-01.md) · [P5_TSP_FULLCIRCLE_SEQUENCER.md](P5_TSP_FULLCIRCLE_SEQUENCER.md)

---

## 1. Execution order (critical)

Annex clauses do not define TSP order; this sequence minimises false failures:

```text
P5-00  Preconditions (deploy, Modbus, chrony)
P5-A   Model / LN inventory          ← before Compare
P5-B   SCL / Compare Model           ← BEFORE URCB (RCB OptFlds drift)
P5-C   POC reads (T.3.1.3)
P5-D   URCB integrity + timing       ← T.3.2.1, O.8.3
P5-E   LN t/q coherence in reports   ← O.8.3, T.3.3.4.5
P5-F   Control LNs (O.9 / T.3.1.4)   ← Wlim, WSd, VArSd
P5-G   Tracking + fallback           ← manual
P5-H   TesPro northbound MQTT        ← collector cross-check (P4-00)
P5-Z   Revert product profile
```

**Do not** enable `urcb_PdC_Mis4sec` before Compare Model (step P5-B02) unless waiving RCB OptFlds rows.

---

## 2. Preconditions — P5-00

| Step | REQ-ID | Annex | Procedure | Pass criteria | Lab |
|------|--------|-------|-----------|---------------|-----|
| P5-00-01 | — | — | Deploy `run-p5-fullcircle-cleartext.ps1` | `ccli 0.1.0-r25`; listen `:102` tls=off | **PASS** |
| P5-00-02 | — | — | Modbus slave COM5 P=450 kW, Q=45 kvar | FC3 @ 40001/40003 stable | **PASS** |
| P5-00-03 | REQ-TIM-001 | **T.3.3.4.5** | DUT: `chronyc tracking` | \|offset\| ≤ **100 ms** | **PASS** (~0–5 ms) |
| P5-00-04 | REQ-TIM-002 | **T.3.3.4.5** | `verify-gnss-time-quality.ps1` | `gnss_fix=1`, `clockNotSynchronized=0` | **PASS** (13 sats, sky lock) |
| P5-00-05 | — | **T.3.3.3** | TSP connect IED `CCI016_01` :102 TLS off | Association OK | **PASS** |

**Annex T.3.3.4.5:** UTC reference uncertainty shall not exceed **±100 ms**.  
**Annex O.8.3:** All periodic P/Q/V transmissions shall be **synchronous with :00,:04,:08…** (verified in P5-D/E).

---

## 3. Part A — Model & logical nodes (Annex T T.3.1 / TR 57-126)

| Step | REQ-ID | Annex clause | TSP / tool | Object / action | Pass criteria | Lab |
|------|--------|--------------|------------|-----------------|---------------|-----|
| P5-A01 | REQ-LN-003 | **T.3.1** · TR CID | Advanced Client | GetNameList `LD_Plant` | **31** LNs (not 9 MVP) | **PASS** |
| P5-A02 | REQ-LN-001 | **T.3.1.3** | Advanced Client | Browse | `PdCMMXU1` present (`prefix PdC`, `MMXU`, inst 1) | **PASS** |
| P5-A03 | REQ-LN-006 | **T.3.1.4** Tab.85–88 | Advanced Client | Browse | `WlimDWMX1`, `WSdDAGC1`, `VArSdDVAR1` present | **PASS** |
| P5-A04 | REQ-LN-003 | TR 57-126 | Script | `python scripts/audit-ln-matrix.py --r23` | CID=CFG=31; no CFG orphans | **PASS** |
| P5-A05 | — | **T.3.1.3** optional | Advanced Client | Browse `GenPVMMXU1`, `StMMXU1`, … | On wire (static cfg); values nameplate | **PASS** browse |

**Normative mapping (T.3.1.3 — mandatory @ PdC):**

| Quantity | Unit | IEC 61850 | Step |
|----------|------|-----------|------|
| Active power | kW | `PdCMMXU1.TotW` | P5-C01 |
| Reactive power | kVAr | `PdCMMXU1.TotVAr` | P5-C02 |
| Phase-phase voltage | kV | `PdCMMXU1.PPV` | P5-C03 |
| Phase current | A | `PdCMMXU1.A` | **GAP** (static 1970) |

---

## 4. Part B — SCL / Compare (before reports)

| Step | REQ-ID | Annex | TSP tool | Action | Pass criteria | Lab |
|------|--------|-------|----------|--------|---------------|-----|
| P5-B01 | REQ-P5-007 | **T.3.1** SCL | Offline SCL Verify | CID vs 61850-7-4/7-420 | No blocking errors (known waivers) | **PASS** |
| P5-B02 | REQ-P5-008 | TR 57-126 | Compare Model | vs `lab_tg544_eth_a.cid` **before URCB** | Structure 31 LN; value waivers per doc | **PASS** waived |
| P5-B03 | — | — | — | Export Compare | `TSP_P5_COMPARE_*.xlsx` archived | r25: 69 rows waivable |

Waivers: `lab/evidence/testsuite-pro/inbox/TSP_P5_COMPARE_WAIVERS.md`

---

## 5. Part C — POC measurements (T.3.1.3 / T.3.2.1)

**Annex T.3.2.1:** Plant measurements — periodic **every 4 s**, Performance Class **Type 3**.  
**Annex T.3.1.3:** Sampling/transmission period **4 s** for MMXU `mag` (and DEL `cVal` for PPV).

| Step | REQ-ID | Annex clause | TSP tool | Read object | Expected (lab) | Pass band | Lab |
|------|--------|--------------|----------|-------------|----------------|-----------|-----|
| P5-C01 | REQ-P5-002 | **T.3.1.3** | Advanced Client | `PdCMMXU1.TotW.mag.f` | Modbus P | 450 ±0.5 kW | **PASS** |
| P5-C02 | REQ-P5-002 | **T.3.1.3** | Advanced Client | `PdCMMXU1.TotVAr.mag.f` | Modbus Q | 45 ±0.5 kvar | **PASS** |
| P5-C03 | REQ-P5-002 | **T.3.1.3** | Advanced Client | `PdCMMXU1.PPV.phsAB.cVal.mag.f` | yaml POC V | 20.0 ±0.1 kV | **PASS** |
| P5-C04 | REQ-LN-002 | **T.3.1.3** opt. | Advanced Client | `PdCMMXU1.A` | — | Live or omit from URCB | **GAP** |

Cross-check: Modbus FC3 reg 40001/40003 vs MMS reads (automated in `P5_FULLCIRCLE_CLEARTEXT*.txt`).

---

## 6. Part D — Periodic transmission & integrity (T.3.2.1 / O.8.3 / O.8.4)

**Annex O.8.3:** P, Q, V @ POC → DSO every **4 s**, sync **:00/:04/:08…**, with **timestamp and quality**.  
**Annex O.8.4:** Overwrites previous value; no buffering at interface.

| Step | REQ-ID | Annex clause | TSP tool | Action | Pass criteria | Lab |
|------|--------|--------------|----------|--------|---------------|-----|
| P5-D01 | REQ-MET-001 | **T.3.2.1** · **T.3.1.3** | Report Viewer | Enable `LLN0.RP.urcb_PdC_Mis4sec01` | RptId `…/LLN0.urcb_PdC_Mis4sec`; IntgPd=**4000** | **PASS** |
| P5-D02 | REQ-MET-001 | **T.3.2.1** | Report Viewer | Collect ≥5 reports / 20 s | Δt between integrity reports **3900–4100 ms** | **PASS** (~3950 ms avg) |
| P5-D03 | REQ-MET-002 | **O.8.3** | Report Viewer | Parse report `t` UTC seconds | sec mod 4 **= 0** on each report | **GAP** (sec mod 4 = 3) |
| P5-D04 | REQ-MET-003 | **O.8.3** | Report Viewer | Each report has t + q on TotW/TotVAr/PPV | Fields present | **PART** (A static) |
| P5-D05 | REQ-MET-004 | **T.3.2.1** Type 3 | — | E2E transit ~500 ms (substation) | Not lab-gated | **OPEN** |

Automated duplicate: `lab/mms_lab_client` — see `P5_FULLCIRCLE_CLEARTEXT_2026-10-01_111628.txt`.

---

## 7. Part E — LN timing alignment (O.8.3 / T.3.3.4.5)

| Step | REQ-ID | Annex clause | TSP / DUT | Check | Pass criteria | Lab |
|------|--------|--------------|-----------|-------|---------------|-----|
| P5-E01 | REQ-LN-004 | **O.8.3** | Report Viewer | Same report: TotW.t, TotVAr.t, PPV.t | **Identical** (≤1 ms) | **PASS** (0 ms) |
| P5-E02 | REQ-LN-005 | **T.3.3.4.5** · **O.8.3** | Report Viewer | TotW.q, TotVAr.q, PPV.phsAB.q | All reflect sync state | **PART** (LN-GAP-08) |
| P5-E03 | REQ-TIM-001 | **T.3.3.4.5** | DUT shell | `chronyc tracking` during report window | \|offset\| ≤ 100 ms | **PASS** |
| P5-E04 | REQ-TIM-002 | **T.3.3.4.5** | DUT log | `clockNotSynchronized` / `gnss_fix` | Product: =0 when GNSS+chrony | **WAIVED** lab |
| P5-E05 | REQ-LN-007 | **O.8.3** | Report Viewer | 10+ reports: `t.sec % 4` | **= 0** | **GAP** (product) |

Detail: [P5_TIMING_ANNEX_AUDIT_2026-10-01.md](P5_TIMING_ANNEX_AUDIT_2026-10-01.md) §2.3–§2.5.

---

## 8. Part F — Control functions (Annex T T.3.1.4 / Annex O O.9)

Control is **Optional** in Annex T but implemented for PF2 lab paths.

### 8.1 Baseline reads (Annex defaults Tab.85–88)

| Step | REQ-ID | Annex | TSP | Object | Annex default | Lab cfg | Pass |
|------|--------|-------|-----|--------|---------------|---------|------|
| P5-F01 | REQ-LN-006 | **T.3.1.4** Tab.85 | Read | `WlimDWMX1.Mod.stVal` | **5** (Not-Operating) | **5** @ boot | **PASS** |
| P5-F02 | REQ-LN-006 | **T.3.1.4** Tab.86 | Read | `WSdDAGC1.Mod.stVal` | **5** | **1** in r23 cfg | **NOTE** LN-GAP-05 |
| P5-F03 | — | **T.3.1.4** Tab.88 | Read | `VArSdDVAR1.Mod.stVal` | **5** | **5** @ baseline | **PASS** |
| P5-F04 | — | **T.3.1.4** Tab.90–91 | Read | `PFSPDFPF1`, `VArVDVVR1`, `PFWDPFW1` Mod | **5** | **5** stub | **PASS** browse |

### 8.2 Active power limitation — O.9.2.2 / T Tab.85

| Step | REQ-ID | Annex clause | TSP | Action | Pass criteria | Lab |
|------|--------|--------------|-----|--------|---------------|-----|
| P5-F10 | REQ-P5-004 | **O.9.2.2** · **T.3.1.4** | Direct Operate | `WlimDWMX1`: Mod=**1**, `WMaxSptPct=**10**` | Operate OK | **PASS** |
| P5-F11 | REQ-P5-004 | **O.9.2.2** | DUT log / DIO | PF2 threshold + curtail DO | `mms→pf2 Wlim=on@10%`; DO ON | **PASS** |

**Annex O.9.2.2:** Limitation on external DSO command — maps to `WlimDWMX1` APC.

### 8.3 Active power modulation — O.9.2.3 / T Tab.86

| Step | REQ-ID | Annex clause | TSP | Action | Pass criteria | Lab |
|------|--------|--------------|-----|--------|---------------|-----|
| P5-F20 | REQ-P5-005 | **O.9.2.3** · **T.3.1.4** | Direct Operate | `WSdDAGC1`: Mod=**1**, `WSptPct=**20**` | Operate OK | **PASS** |
| P5-F21 | REQ-P5-005 | **O.9.2.3** | DUT log | `mms→pf2` WSd active | `p_wsd` updated | **PASS** |

### 8.4 Reactive voltage control — O.9.1.4 / T Tab.88

| Step | REQ-ID | Annex clause | TSP | Action | Pass criteria | Lab |
|------|--------|--------------|-----|--------|---------------|-----|
| P5-F30 | REQ-P5-006 | **O.9.1.4** · **T.3.1.4** | Direct Operate | `VArSdDVAR1`: Mod=**1**, `VArTgtSptPct=**10**` | Operate OK | **PASS** |
| P5-F31 | REQ-P5-006 | **O.9.1.4** | DUT log + Modbus | `mms→plant Q=21 kvar`; FC16 @ 40003 | Q write + TotVAr tracks | **PASS** |

### 8.5 Deferred control (not P5-gated)

| REQ-ID | Annex | Function | LN | Status |
|--------|-------|----------|-----|--------|
| REQ-CTL-001 | **O.7.3.1** | TsP ≤ 60 s | PF2 ring | **DEFERRED** |
| REQ-CTL-002 | **O.7.3.1** | TsQ ≤ 10 s | VArSd plant | **DEFERRED** |
| REQ-CTL-003 | **O.7.3.2** | Slow ring ΔT 10–600 s | — | **GAP** |
| REQ-CTL-004 | **O.7.3.3** | Min **3 s** between set-points | regulation_map | **GAP** |
| — | **O.9.1.1–1.3** | PFSP / VArV / PFW | stub LNs | **DEFERRED** P6+ |
| — | **O.8.6** | Status notify ≤ 4 s | BRCB alarms | **DEFERRED** |

**Annex O.7.3.3:** External set-points arriving faster than **3 s** shall be **rejected**.

---

## 9. Part G — Tracking & comms fallback

| Step | REQ-ID | Annex | Procedure | Pass criteria | Lab |
|------|--------|-------|-----------|---------------|-----|
| P5-G01 | REQ-P5-002 | **T.3.1.3** | Modbus P sweep 400→500 kW | `TotW` tracks ±0.5 kW | Manual |
| P5-G02 | — | **O.11** / P3-05 | Disconnect TSP ≥15 s | Wlim/WSd/VArSd cleared; PF2 yaml mock | Manual step 14 |
| P5-G03 | — | **T.3.3.4.5** | Product TLS session (optional) | `:3782` MMS+TLS evidence separate | `P5_MMS_TLS.pcapng` |

---

## 10. Part H — TesPro northbound MQTT (collector cross-check)

**Not Annex T DSO path** — supplier **MMS client + MQTT upload** (P4-00). Cross-validates **T.3.1.3** values and **4 s** cadence vs ccli/TSP.

**Config guide:** [P5_TESPRO_NORTHBOUND.md](P5_TESPRO_NORTHBOUND.md)

| Step | REQ-ID | Annex (cross-ref) | Tool | Procedure | Pass criteria | Lab |
|------|--------|-------------------|------|-----------|---------------|-----|
| P5-H00 | REQ-NB-000 | — | LuCI | Service **Running**; `iec61850d` up | Processes OK | **PASS** |
| P5-H01 | REQ-NB-001 | **T.3.1.3** | LuCI Device | `CCLI_LAB` → ccli `192.168.10.1:102` | Green dot; connect OK | **PASS** |
| P5-H02 | REQ-NB-001 | **T.3.1.3** | LuCI Points | 9 leaf points; POC **Interval=4 s** | Realtime **PdC_TotW≈450** | **PASS** |
| P5-H03 | REQ-NB-002 | — | LuCI Northbound | `channel_1` Enabled; Host **`192.168.10.10:1883`** | **Connected** | **PASS** |
| P5-H04 | REQ-NB-003 | **T.3.1.3** | PC | `verify-tespro-northbound.ps1` | JSON `PdC_TotW` ≈ Modbus P | **PASS** |
| P5-H05 | REQ-MET-001 | **T.3.2.1** | MQTT log | Δt between POC uploads | **4000 ms** | **PASS** |
| P5-H06 | REQ-LN-004 | **O.8.3** | MQTT JSON | Same message TotW/TotVAr/PPV ts | Coherent within one report | **PASS** |
| P5-H07 | — | V-005 | Concurrent | URCB + collector active (r25 session) | Both receive ~4 s data | **PASS** |
| P5-H08 | — | — | LuCI | View Logs → export | No `IedClientError=5` | **PASS** (after bind fix) |

**Evidence:** [P5_TESPRO_NORTHBOUND_2026-10-01_142810.txt](P5_TESPRO_NORTHBOUND_2026-10-01_142810.txt) · [P5_TESPRO_NORTHBOUND_2026-10-01.md](P5_TESPRO_NORTHBOUND_2026-10-01.md)

**Recovery (Connected, Sent=0):** `restart-tespro-northbound.ps1 -Verify`

---

## 11. Waiver & gap register (Phase 5)

| ID | Annex | Issue | Test step | Disposition |
|----|-------|-------|-----------|-------------|
| LN-GAP-07 | T.3.3.4.5 | q=Questionable without GNSS fix | P5-E02, P5-00-04 | **CLOSED** 2026-10-01 |
| LN-GAP-08 | O.8.3 | PPV.q not refreshed with TotW/TotVAr | P5-E02 | **WAIVED** lab; fix product |
| LN-GAP-05 | T.3.1.4 | WSd Mod cfg ≠ CID default | P5-F02 | Document; Operate before compare |
| REQ-MET-002 | O.8.3 | No :00/:04/:08 alignment | P5-D03, P5-E05 | **GAP** product |
| REQ-CTL-004 | O.7.3.3 | 3 s spacing not enforced | — | **GAP** product |
| PLANT-GAP-01 | T.3.1.3 | Gen* TotW static 0 | P5-A05 | Compare waiver |
| P5-DEFER-01 | O.9.1.x | PFSP/VArV/PFW no plant path | P5-F04 | P6+ |

---

## 12. Verification matrix (requirements → tests)

| Requirement | Annex | Test ID(s) | Pass (lab) |
|-------------|-------|------------|------------|
| REQ-P5-001 31 LN wire | TR | P5-A01, P5-A04 | **PASS** |
| REQ-P5-002 POC reads | T.3.1.3 | P5-C01–03 | **PASS** |
| REQ-P5-003 4 s URCB | T.3.2.1 | P5-D01–02 | **PASS** |
| REQ-P5-004 Wlim | O.9.2.2 | P5-F10–11 | **PASS** |
| REQ-P5-005 WSd | O.9.2.3 | P5-F20–21 | **PASS** |
| REQ-P5-006 VArSd | O.9.1.4 | P5-F30–31 | **PASS** |
| REQ-P5-007 SCL | T.3.1 | P5-B01 | **PASS** |
| REQ-P5-008 Compare | TR | P5-B02 | **PASS** waived |
| REQ-TIM-001 ±100 ms | T.3.3.4.5 | P5-00-03, P5-E03 | **PASS** |
| REQ-TIM-002 sync flag | T.3.3.4.5 | P5-00-04, P5-E04 | **PART** waived |
| REQ-MET-002 grid | O.8.3 | P5-D03, P5-E05 | **GAP** |
| REQ-LN-004 t coherence | O.8.3 | P5-E01 | **PASS** |
| REQ-LN-005 q coherence | O.8.3 | P5-E02 | **PART** |
| REQ-NB-001 collector POC | T.3.1.3 | P5-H01–02 | **PASS** |
| REQ-NB-002 MQTT connected | — | P5-H03 | **PASS** |
| REQ-NB-003 JSON values | T.3.1.3 | P5-H04 | **PASS** |

---

## 13. Evidence checklist

- [x] TSP export → `TSP_P5_FULLCIRCLE_CLEARTEXT_<date>_<time>.txt` (r25 session)
- [x] Compare export (pre-URCB) → `TSP_P5_COMPARE_*.xlsx` (r25 −8 rows waived)
- [x] Report Viewer / automated timing notes (Δt, t sec mod 4, q fields)
- [x] DUT `chronyc tracking` snapshot — [P5_GNSS_FIX_VERIFY_2026-10-01.md](P5_GNSS_FIX_VERIFY_2026-10-01.md)
- [x] `P5_FULLCIRCLE_CLEARTEXT_2026-10-01_111628.txt` automated log
- [x] TesPro northbound log → [P5_TESPRO_NORTHBOUND_2026-10-01_142810.txt](P5_TESPRO_NORTHBOUND_2026-10-01_142810.txt)
- [x] [P5_FINAL_CLOSEOUT.md](P5_FINAL_CLOSEOUT.md) signed 2026-10-01
- [x] Verification client status → [P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md](P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md)

**Gate status:** **CLOSED** (lab) 2026-10-01 on structure + 4 s period + chrony + LN **t** coherence + Wlim/WSd/VArSd + northbound. Track **REQ-MET-002**, **LN-GAP-08**, **REQ-CTL-004**, **P5-R02–R09** for product.

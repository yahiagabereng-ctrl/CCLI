# P5 — Phase closeout (lab evidence chain)

**Date:** 2026-10-01 (updated 2026-10-02 IEDExplorer · **2026-10-05 P5-G02 GOOSE**)  
**Gate:** P5_FULLCIRCLE  
**DUT:** TesPro TG544 @ `192.168.10.1`  
**Product build:** **0.1.0-r25** (P5-COMPARE-FIX)  
**Lab profile:** cleartext MMS `:102` · `bind_address: 0.0.0.0` · `lab_tr400_cleartext_tsp.yaml`  
**Operator UI:** Test Suite Pro (r25 session) + automated `mms_lab_client` · **TSP license blocked 2026-10-01** — re-runs use [IEDExplorer](https://sourceforge.net/projects/iedexplorer/) per [P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md](P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md)  
**Annex test sequence:** [P5_ANNEX_TEST_SEQUENCE.md](P5_ANNEX_TEST_SEQUENCE.md)  
**Evidence index:** [lab/evidence/README.md](../README.md)

---

## Requirements closed

| ID | Requirement | Annex | Evidence | Status |
|----|-------------|-------|----------|--------|
| REQ-P5-001 | Full CID (31 LN) on MMS wire | TR 57-126 | `P5_LN_WIRE_STATUS.md`, GetNameList | **PASS** |
| REQ-P5-002 | Live POC measurements (TotW, TotVAr, PPV) | **T.3.1.3** | TSP steps 2–4, automated client | **PASS** |
| REQ-P5-003 | URCB integrity ~4 s | **T.3.2.1** | Report Viewer / `mms_lab_client` | **PASS** |
| REQ-P5-004 | Wlim Direct Operate → PF2 + DIO | **O.9.2.2** | steps 8, Wlim client, DUT log | **PASS** |
| REQ-P5-005 | WSd structure + Operate path | **O.9.2.3** | step 9, cfg Mod=1 | **PASS** |
| REQ-P5-006 | VArSd reactive command (P5-R01) | **O.9.1.4** | step 11, `mms_varsd_client`, FC16 | **PASS** |
| REQ-P5-007 | Offline SCL Verify | **T.3.1** | `TSP_OFF_SCLVERIFY_*_TRIAGE.md` | **PASS** |
| REQ-P5-008 | Compare Model structure vs CID | TR | waivers doc; r25 −8 rows | **PASS** (waived values) |
| REQ-TIM-001 | UTC ±100 ms | **T.3.3.4.5** | `chronyc tracking` | **PASS** |
| REQ-TIM-002 | GNSS lock + Good q | **T.3.3.4.5** | GNSS sky lock verify | **PASS** |
| REQ-LN-004 | Intra-LN timestamp coherence | **O.8.3** | URCB reports §2.5.4 | **PASS** |
| REQ-NB-002/003 | TesPro MQTT northbound | P4-00 | `P5_TESPRO_NORTHBOUND_2026-10-01_142810.txt` | **PASS** |

## Partial / waived (lab accepted)

| ID | Requirement | Annex | Status | Track |
|----|-------------|-------|--------|-------|
| REQ-LN-005 | POC q coherent (TotW=TotVAr=PPV) | **O.8.3** | **PART** | LN-GAP-08 (PPV.q only) |
| REQ-MET-003 | 4 s block t+q complete | **O.8.3** | **PART** | A DO static @ 1970 |

## Deferred / product gaps (non-blocking P5 lab)

| Item | Annex | Reason | Track |
|------|-------|--------|-------|
| REQ-MET-002 / REQ-LN-007 | **O.8.3** | :00/:04/:08 grid alignment | boundary scheduler |
| REQ-CTL-004 | **O.7.3.3** | 3 s set-point reject | regulation_map R07 |
| REQ-CTL-001/002 | **O.7.3.1** | TsP 60 s / TsQ 10 s | regulation bench |
| REQ-TIM-003 | **T.3.3.4.5** | NTS mandatory (product) | C5 time architecture |
| PFSP / VArV / PFW | **O.9.1.x** | Annex T Tab.88–91 | P6+ |
| GenPV live values | **T.3.1.3** | No Sun2000 bus | plant slice |
| Product TLS `:3782` TSP | **T.3.3.4.1** | cleartext lab bypass | revert below |
| TSP steps 13–14 | — | manual optional | sequencer |
| P5-R02–R09 (PFSP/VArV/PFW full plant) | **O.9.1.x** | lab scope VArSd only | P6+ |
| P5-04 accuracy / V5 chain | **O.13.2** | not bench-calibrated | metrology |
| TSP license (post-close) | — | Sentinel trial expired | IEDExplorer fallback |

---

## Automated evidence (r25 deploy)

| Artifact | Path |
|----------|------|
| Phase 5 index | `lab/evidence/phase5/P5_README.md` |
| Annex test sequence | `lab/evidence/phase5/P5_ANNEX_TEST_SEQUENCE.md` |
| TSP sequencer | `lab/evidence/phase5/P5_TSP_FULLCIRCLE_SEQUENCER.md` |
| Timing + LN audit | `lab/evidence/phase5/P5_TIMING_ANNEX_AUDIT_2026-10-01.md` |
| GNSS fix verify | `lab/evidence/phase5/P5_GNSS_FIX_VERIFY_2026-10-01.md` |
| TesPro northbound guide | `lab/evidence/phase5/P5_TESPRO_NORTHBOUND.md` |
| TesPro northbound finalize | `lab/evidence/phase5/P5_TESPRO_NORTHBOUND_2026-10-01.md` |
| MQTT broker scripts | `lab/tg544-openwrt/install-mqtt-lab-broker.ps1`, `verify-tespro-northbound.ps1` |
| Full circle log | `lab/evidence/phase5/P5_FULLCIRCLE_CLEARTEXT_2026-10-01_111628.txt` |
| Compare fix notes | `lab/evidence/testsuite-pro/inbox/TSP_P5_COMPARE_FIX_2026-10-01.md` |
| Compare waivers | `lab/evidence/testsuite-pro/inbox/TSP_P5_COMPARE_WAIVERS.md` |
| LN matrix | `lab/evidence/phase5/P5_LN_VALUE_MATRIX.md` |
| Wire status | `lab/evidence/phase5/P5_LN_WIRE_STATUS.md` |

### Key r25 validation snippets

```text
ccli 0.1.0-r25 (P5-COMPARE-FIX)
mms: listening on 0.0.0.0:102 tls=off model=full_cid_cfg
PdCMMXU1.TotW = 450 kW · TotVAr = 45 kvar · PPV.phsAB = 20 kV
URCB avg_interval_ms=3950 (IntgPd=4000)
TotW.t = TotVAr.t = PPV.t (0 ms delta per report)
chrony offset ~0–5 ms (T.3.3.4.5 PASS)
GNSS: 13 sats sky lock; gnss_fix=1 clockNotSynchronized=0 (P5_GNSS_FIX_VERIFY PASS)
OPERATE OK VArSdDVAR1 · Modbus FC16 Q=21 kvar
TesPro iec61850d PdC_TotW=450.0000 (bind 0.0.0.0 fix)
TesPro MQTT northbound: Connected · REPORT_PROPERTY · PdC_TotW=450 @ 4 s (P5_TESPRO_NORTHBOUND_2026-10-01)
```

---

## 2026-10-05 extension — P5-G02 GOOSE publish (r29)

| Item | Evidence | Status |
|------|----------|--------|
| P5-G02 integrated GOOSE publish | [P5_GOOSE_SESSION_RECORD_2026-10-05.md](P5_GOOSE_SESSION_RECORD_2026-10-05.md) | **PASS** |
| Wire pcap LAN3 | `P5_GOOSE_WIRE_2026-10-05_140838.pcapng` | **PASS** |
| GoCB MMS | `P5_GOCB_MMS_2026-10-05.txt` | **PASS** |

Plant egress: **`goose.interface: lan3`** (192.168.30.1) — not empty `br-lan`.  
**P5-G03:** merge policy PASS — [`P5_G03_MERGE_POLICY.md`](P5_G03_MERGE_POLICY.md) (2026-10-05).  
**Next:** P5-G01 subscribe (LAN3) · P5-G03 runtime merge implementation.

---

## 2026-10-02 re-validation (IEDExplorer + CID rev4)

| Item | Tool / evidence | Status |
|------|-----------------|--------|
| Browse / reads / URCB | IEDExplorer ICD3 + Report4.csv | **PASS** |
| Operate Wlim / WSd / VArSd | `mms_wlim_client`, `mms_wsd_client`, `mms_varsd_client` | **PASS** |
| IEDExplorer Operate | CO Write denied (expected) | **WAIVED** — use lab clients |
| MQTT northbound | `P5_TESPRO_NORTHBOUND_2026-10-02_151845.txt` | **PASS** TotW=450 @ ~2 s |
| IEDEX log | `IEDEX_P5_FULLCIRCLE_CLEARTEXT_2026-10-02_133500.txt` | **CLOSED** |
| P5-G02 fallback | IEDExplorer disconnect ≥15 s → P3-05 log | **PASS** |

---

## Revert to product profile

When lab TSP session is complete:

```powershell
$env:CCLI_TG544_PW = '000000'
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr 192.168.10.1 `
  -LabYaml apps\ccli\config\lab_tr400_phase4_eth_b.yaml `
  -Changes @("revert TLS :3782 product profile")
```

Product MMS: `192.168.10.1:3782` TLS **ON** · client certs in `apps/ccli/config/tls/`.

---

## Verification summary

| Requirement | Test (Annex seq) | Pass criteria | Result |
|-------------|------------------|---------------|--------|
| REQ-P5-001 | P5-A01 | 31 LNs | **PASS** |
| REQ-P5-002 | P5-C01–03 | ≈450 / ≈45 / 20 kV | **PASS** |
| REQ-P5-003 | P5-D01–02 | IntgPd=4000, Δt 3900–4100 ms | **PASS** |
| REQ-P5-004 | P5-F10–11 | DIO curtail, pf2 threshold | **PASS** |
| REQ-P5-006 | P5-F30–31 | Q=21 kvar FC16 + log | **PASS** |
| REQ-TIM-001 | P5-00-03 | \|offset\| ≤ 100 ms | **PASS** |
| REQ-LN-004 | P5-E01 | TotW.t = TotVAr.t = PPV.t | **PASS** |
| REQ-MET-002 | P5-D03 | sec mod 4 = 0 | **GAP** (product) |

**P5 lab gate: CLOSED** on functional + interval + chrony + LN structure/coherent **t** (2026-10-01, build **0.1.0-r25**).  
**Product open:** REQ-MET-002 grid alignment · LN-GAP-08 · REQ-CTL-004.

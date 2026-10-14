# Unified certification test matrix — functional + cyber + O.15 (5-LN first run)

**Document ID:** CCLI-LAB-UNIFIED-MTX-001  
**Revision:** 1.1  
**Date:** 2026-10-14  
**5-LN validation:** [PRODUCT_SPECIMEN_5LN_VALIDATION.md](PRODUCT_SPECIMEN_5LN_VALIDATION.md) · `scripts/validate-5ln-cid.ps1`
**RAG source_id:** `ccli-lab-unified-cert-test-matrix`  
**Roadmap:** [LAB_UNIFIED_CERT_TEST_ROADMAP.md](LAB_UNIFIED_CERT_TEST_ROADMAP.md)  
**Specimen:** [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md) (**5-LN** product)  
**Excel import:** [LAB_UNIFIED_CERT_TEST_MATRIX.csv](LAB_UNIFIED_CERT_TEST_MATRIX.csv)

**Supersedes as single □ checklist:** merges [LAB_REQUIREMENT_TEST_MATRIX.md](LAB_REQUIREMENT_TEST_MATRIX.md) + [LAB_REQUIREMENT_TEST_MATRIX_CYBER.md](LAB_REQUIREMENT_TEST_MATRIX_CYBER.md) + O.15 non-TSP deliverables from `ccli-dnv-product-readiness`. Legacy files remain authoritative for row detail; tick **here** for programme traceability.

**Field pack:** `scripts/stage-tsp-field-pack.ps1` → `00_UNIFIED_CERT_TEST_MATRIX.md` + `00b_UNIFIED_CERT_ROADMAP.md`

---

## Session header (fill before Phase 1)

| Field | Value |
|-------|--------|
| Date | *(fill)* |
| Operator | *(fill)* |
| Specimen | **5-LN product** per PRODUCT_SPECIMEN_5LN |
| TSP version | *(fill)* |
| DUT `ccli --version` | *(fill)* |
| CID file | `product_tr400_eth_a_5ln.cid` (or interim note) |
| cfg SHA-256 | *(fill)* |
| Bench | **Two-PC** · PC-B TSP · PC-A 104/LAN3/COM5 |
| Git / manifest | *(fill)* |

---

## Hardware map (your bench)

| Link | DUT | PC-B (TSP) | PC-A (plant/logger) |
|------|-----|------------|------------------------|
| MMS TLS | `192.168.10.1:3782` | `192.168.10.10` | — |
| IEC 104 | `192.168.1.130:2404` | — | `192.168.1.183` |
| Plant LAN3 | `192.168.30.1/24` | — | `192.168.30.10` |
| EMT432 Modbus TCP | polled by DUT | — | `192.168.30.50:502` *(or `.178.250` Chronos)* |
| RS485 mock | A1/B1 → `ttyS1` | — | USB **COM5** **SUN2000** RTU sim (`lab/start-bench-plant.ps1`) |

Scripts: `lab/set_pc_lan1_dso.cmd` · `set_pc_lan2_oa.cmd` · `set_pc_lan3_plant.cmd` (or `set_pc_lan3_plant_178.cmd`).

---

## Normative RAG corpus (query when disputing scope)

| source_id | Use |
|-----------|-----|
| `ccli-lab-unified-cert-test-matrix` | This file |
| `ccli-lab-unified-cert-test-roadmap` | Phase order |
| `ccli-product-specimen-5ln` | 5-LN pin |
| `ccli-lab-requirement-test-matrix` | Functional detail |
| `ccli-lab-requirement-test-matrix-cyber` | Cyber detail |
| `ccli-tsp-two-pc-bench-runbook` | Two-PC execution |
| `ccli-dnv-product-readiness` | O.15 deliverable map |
| `ccli-lab-dut-finalize-cert-path` | Internal vs accredited done |
| `ccli-61850-10-extract` | sAss/sSrv/sRp/sCtl |
| `ccli-62351-100-3-extract` | Accredited TLS cases |
| `cei-0-16-allegato-o-extract` | O.14 · O.15 · §12 |
| `cei-0-16-allegato-t-extract` | T.3.3.4 security |

---

## 5-LN binding (mandatory for this matrix)

| Rule | Detail |
|------|--------|
| **Browse (U-T1-03)** | Exactly **`LLN0`**, **`LPHD1`**, **`PdCMMXU1`**, **`WlimDWMX1`**, **`WSdDAGC1`** under `LD_Plant` |
| **Compare (U-T1-02)** | CID = `product_tr400_eth_a_5ln.cid` · `validate-5ln-cid.ps1` **PASS** before TSP |
| **P5 (U-P5-01)** | [P5_FULLCIRCLE_5LN_SCOPE.md](P5_FULLCIRCLE_5LN_SCOPE.md) only — **not** 31-LN [P5_TSP_FULLCIRCLE_SEQUENCER.md](../evidence/phase5/P5_TSP_FULLCIRCLE_SEQUENCER.md) |
| **PICS (U-DOC-01)** | **5-LN** Cover + model sheet — **do not** submit 31-LN [PICS_FILLED_WORKBOOK.md](PICS_FILLED_WORKBOOK.md) as cert without trim |
| **Field pack CID** | TSP Compare source = **product** CID; `lab_tg544_eth_a.cid` is lab reference only |

---

## Master table

**Columns:** □ · **U-ID** · **Phase** · **O.15 / cert** · **REQ** · **Gate** · **Executor** · **HW** · **Action** · **Pass** · **Evidence** · **RAG**

| □ | U-ID | Phase | O.15 / cert | REQ / gate | Exec | HW | Action (summary) | Pass criteria | Evidence | RAG |
|---|------|-------|-------------|------------|------|-----|------------------|---------------|----------|-----|
| □ | U-P0-01 | 0 | Deploy | REQ-ROAD-HW | Both | — | `git pull` · `stage-tsp-field-pack.ps1` on PC-B | Pack complete | PACK_MANIFEST.json | ccli-tsp-field-pack |
| □ | U-P0-02 | 0 | Deploy | P3-01 | PC-B | LAN1 | `set_pc_lan1_dso.cmd` · ping `.10.1` | RTT OK | preflight txt | ccli-tsp-two-pc-bench-runbook |
| □ | U-P0-03 | 0 | Deploy | P4 | PC-A | LAN2 | `set_pc_lan2_oa.cmd` · ping `.1.130` | RTT OK | preflight txt | ccli-tsp-two-pc-bench-runbook |
| □ | U-P0-04 | 0 | Deploy | P5 plant | PC-A | LAN3 | `set_pc_lan3_plant*.cmd` · ping DUT + meter | Both OK | probe log | ccli-tsp-two-pc-bench-runbook |
| □ | U-P0-05 | 0 | Deploy | REQ-ROAD-HW-003 | PC-A | RS485 | Wire A1/B1 + GND | Continuity | P5_A1B1 map | ccli-tr500-io-peripheral |
| □ | U-P0-06 | 0 | UCA prep | D5 | PC-B | — | Import **5-LN** CID + TLS PEMs | Load OK | TSP project | ccli-product-specimen-5ln |
| □ | U-P0-07 | 0 | UCA prep | — | PC-A | SSH | Stop `iec61850service` · one `ccli` · deploy **product cfg** | :3782 listen | PREFLIGHT_* | ccli-lab-dut-finalize-cert-path |
| □ | U-P0-08 | 0 | Trace | REQ-LAB-MTX-003 | Both | — | Record version + cfg SHA in header | Match pin doc | session header | ccli-product-specimen-5ln |
| □ | U-P0b-01 | 0b | Annex T | T.3.1 | PC-A | LAN3 | `modbus_emt432_tcp_probe.py` · **V_L1_L2 @980** → PPV kV | Modbus OK + PPV hint | modbus_emt432_* | ccli-validation-strategy |
| □ | U-P0b-02 | 0b | Annex T | T.3.1 | PC-A | COM5 | `start-bench-plant.ps1 -ScanOnly` then Sun on COM5 (or `-Port COM5` only) | FC03 inverter map | RTU log | ccli-tsp-two-pc-bench-runbook |
| □ | U-P0b-03 | 0b | O.14 | P7-04 | PC-A | SSH | DUT modbus `link_up` in event-dump | Category present | event snippet | ccli-lab-requirement-test-matrix |
| □ | U-P0b-04 | 0b | O.14 | P7-01 | PC-A | SSH | Optional `ccli --event-wrap-test` once | Log saved | P7_01_WRAP_* | ccli-lab-dut-finalize-cert-path |
| □ | U-P0b-05 | 0b | — | — | PC-A | — | Do **not** start TSP until plant path chosen | yaml matches HW | DUT lab.yaml | ccli-lab-config-guide |
| □ | U-DOC-01 | DOC | O.15 #5 | D1 PICS | Eng | — | PICS **5-LN** Cover + ACSI **M** ([PICS_FILL.md](PICS_FILL.md) trim) | Lab quote ready · **not** raw 31-LN xlsx | PICS xlsx | ccli-lab-pics-fill |
| □ | U-DOC-02 | DOC | O.15 #5 | D2 PIXIT | Eng | — | Paste PIXIT (3782, IntgPd 4000, ctl enhanced) | Complete | PIXIT doc | ccli-lab-pixit-draft |
| □ | U-DOC-03 | DOC | O.15 #5 | D3 MICS | Eng | — | MICS = **5-LN** CID | Match Compare | MICS doc | ccli-lab-mics-draft |
| □ | U-DOC-04 | DOC | O.15 #5 | D4 TICS | Eng | — | TICS after lab TISSUES date | Signed | TICS doc | ccli-lab-templates-test-scope |
| □ | U-DOC-05 | DOC | O.15 #5 | D5 CID | Eng | — | Submit cert CID + SCL Verify **0 Error** | Verify log | TSP_OFF_SCLVERIFY_* | ccli-testsuite-pro-procedure |
| □ | U-DOC-06 | DOC | O.15 #6 | D7 | Eng | — | Submit D7 62351-3 PID | Tables 1–5 | D7 draft | ccli-lab-d7-62351-3-pid |
| □ | U-DOC-07 | DOC | O.14 | D6 | Eng | — | LAB_CONFIG_GUIDE + firmware SHA | D6 pack | LAB_CONFIG_GUIDE | ccli-lab-config-guide |
| □ | U-DOC-08 | DOC | O.15 | REQ-LAB-DOC-004 | Lab | DUT | P7-01/04/11 PASS before ship | Evidence set | P7_* | ccli-lab-dut-finalize-cert-path |
| □ | U-T1-01 | 1 | O.15 #5 UCA | P3-01 · sAss | PC-B | LAN1 | TSP **Connect** mutual TLS | Initiate Success | TSP_P3_01_CONNECT_* | ccli-lab-requirement-test-matrix |
| □ | U-T1-02 | 1 | O.15 #5 | P3-02 · D3 | PC-B | LAN1 | **Compare Model** vs **5-LN** CID (pre-URCB) | `validate-5ln-cid.ps1` PASS · Compare 0 MISSING · **0 Value** target | TSP_P3_COMPARE_* | ccli-product-specimen-5ln-validation |
| □ | U-T1-03 | 1 | O.15 #5 | P3-03 · sSrv | PC-B | LAN1 | Browse `LD_Plant` | **Exactly 5:** LLN0,LPHD1,PdCMMXU1,WlimDWMX1,WSdDAGC1 | TSP_P3_03_BROWSE_* | ccli-product-specimen-5ln |
| □ | U-T1-04 | 1 | O.15 #5 · T | P3-03 | PC-B | LAN1 | Read `PdCMMXU1` TotW, TotVAr, PPV | Plausible values | TSP_P3_03_READ_* | cei-tr-57-126-extract |
| □ | U-T1-05 | 1 | O.15 #5 · T | P3-03 · sRp | PC-B | LAN1 | Enable `urcb_PdC_Mis4sec` IntgPd=4000 | RptEna true | TSP_P3_03_REPORT_* | ccli-lab-requirement-test-matrix |
| □ | U-T1-05b | 1 | O.15 #5 | P3-03 · S27-1 | PC-B | LAN1+PC-A | SetURCB **dchg** · change P (mock/meter) | Report | TSP_P3_03_REPORT_DCHG_* | ccli-lab-pics-filled-workbook |
| □ | U-T1-05c | 1 | O.15 #5 | S27-2 | PC-B | LAN1 | SetURCB **qchg** | Report or N/A | TSP_P3_03_REPORT_QCHG_* | ccli-lab-requirement-test-matrix |
| □ | U-T1-05d | 1 | O.15 #5 | S27-3 | PC-B | LAN1 | SetURCB **dupd** | Report | TSP_P3_03_REPORT_DUPD_* | ccli-61850-10-extract |
| □ | U-T1-06 | 1 | O.15 #5 | P3-03 | PC-B | LAN1 | Integrity period observe | Δt 3900–4100 ms | same report log | ccli-lab-pixit-draft |
| □ | U-T1-07 | 1 | O.15 #5 · O.9 | P3-04 · sCtl | PC-B | LAN1 | Operate **Wlim** enhanced | Success | TSP_P3_04_OPERATE_Wlim_* | cei-0-16-allegato-o-extract |
| □ | U-T1-08 | 1 | O.15 #5 · O.9 | P3-04 | PC-B | LAN1 | Operate **WSd** | Success | TSP_P3_04_OPERATE_WSd_* | cei-0-16-allegato-o-extract |
| □ | U-T1-09 | 1 | O.15 #5 | P3-05 | PC-B | LAN1 | Release · wait 15 s fallback | Behaviour per PIXIT | TSP_P3_05_FALLBACK_* | ccli-lab-requirement-test-matrix |
| □ | U-T1-10 | 1 | O.15 #5 | P3-01 | PC-B | LAN1 | Disconnect · reconnect | Associate OK | TSP_P3_01_RECONNECT_* | ccli-lab-requirement-test-matrix |
| □ | U-T1-11 | 1 | O.9.2.2 | P3-04 | PC-A | SSH | After Wlim: `ubus call dido_v2 status` | Actuation trace | append to Wlim txt | ccli-lab-requirement-test-matrix |
| □ | U-T3-01 | 3 | O.15 #6 · T | P3-06 · REQ-D7-MUT | PC-B | LAN1 | Connect **without** client cert | Fail | TSP_P3_06_TLS_FAIL_* | ccli-lab-requirement-test-matrix-cyber |
| □ | U-T3-02 | 3 | O.15 #6 | P3-08 | PC-B | LAN1 | Viewer **read** OK | Pass | TSP_P3_08_VIEWER_READ_* | ccli-62351-8-extract |
| □ | U-T3-03 | 3 | O.15 #6 | P3-08 | PC-B | LAN1 | Viewer **Operate Wlim** deny | Deny | same | ccli-62351-8-extract |
| □ | U-T3-04 | 3 | O.15 #6 | P3-09 | PC-B | LAN1 | Revoked cert + CRL | Fail establish | TSP_P3_09_CRL_* | ccli-62351-9-extract |
| □ | U-T3-05 | 3 | O.15 #6 · T | P3-07 | PC-B | LAN1 | DSO profile ACSE / dual cert | AARE OK | TSP_P3_07_* · pcap | ccli-62351-4-extract |
| □ | U-D7-01 | 3 | O.15 #6 | REQ-D7-PORT | Eng | DUT | Port audit **3782** only product | No :102 ship | yaml audit | ccli-product-sw-freeze |
| □ | U-D7-02 | 3 | O.15 #6 | REQ-D7-CRYPTO | Eng | — | `verify_annex_pki.py` | RSA≥2048 SHA-256 | PKI log | ccli-lab-d7-62351-3-pid |
| □ | U-D7-03 | 3 | O.15 #6 | REQ-D7-1003 | Accredited | Lab | **62351-100-3** formal suite | Report ID | cert report | ccli-62351-100-3-extract |
| □ | U-P5-01 | 2 | O.8 · O.9 | P5 | PC-B | LAN1 | **5-LN** annex trace **3782** ([P5_FULLCIRCLE_5LN_SCOPE.md](P5_FULLCIRCLE_5LN_SCOPE.md)) | No VArSd/31-LN steps | TSP_P5_FULLCIRCLE_TLS_* | ccli-p5-fullcircle-5ln-scope |
| □ | U-P5-02 | 2 | O.8 | P5 | PC-A | COM5/meter | Vary P during seq step 13 | Dynamic read | RTU/meter log | ccli-tsp-two-pc-bench-runbook |
| □ | U-P4-01 | 4 | O.13 | P4 | PC-A | LAN2 | 104 client → :2404 TLS | Session up | phase4/P4_* | ccli-phase-regulation-checklists |
| □ | U-P4-02 | 4 | O.13 | P4 | PC-A | LAN2 | Command → **reject** monitor_only | Reject | P4 log + event-dump | cei-0-16-allegato-o-extract |
| □ | U-P7-01 | 5 | O.14 | P7-01 | PC-A | SSH | `--event-wrap-test` · `--event-clear` **fail** | exit 2 clear | P7_01_WRAP_* | ccli-lab-requirement-test-matrix |
| □ | U-P7-02 | 5 | O.14 | P7-02 | PC-A | SSH | `--event-dump --count 20` | UTC timestamp format | P7_02_DUMP_* | cei-0-16-allegato-o-extract |
| □ | U-P7-03 | 5 | O.14 | P7-04 | Eng | — | Update event coverage matrix | Gaps documented | P7-04_EVENT_COVERAGE_MATRIX | ccli-lab-requirement-test-matrix |
| □ | U-P7-04 | 5 | O.14 · K7.5 | P7-11 | PC-A | SSH | P7-11 DSO demo script | Log tail PASS | P7-11_DSO_DEMO_* | ccli-lab-dut-finalize-cert-path |
| □ | U-P3-11 | 1/2 | T.3.3.4.5 | P3-11 | PC-A | SSH | `chronyc tracking` | \|offset\| ≤ 100 ms | P3_11_* | cei-0-16-allegato-t-extract |
| □ | U-N/A-01 | — | — | — | — | — | **BRCB / GOOSE / File / SG** | PICS **N/A** first quote | — | ccli-lab-cid-pics-alignment |
| □ | U-CLO-01 | 6 | Internal | REQ-LAB-MTX-002 | Both | USB | Copy all TSP_* → inbox | Names match | inbox/ | ccli-testsuite-pro-procedure |
| □ | U-CLO-02 | 6 | Internal | REQ-LAB-MTX-001 | Eng | — | Save `LAB_UNIFIED_CERT_TEST_MATRIX_YYYY-MM-DD_done.md` | All **M** □ | done copy | ccli-lab-unified-cert-test-matrix |
| □ | U-CLO-03 | 6 | Internal | REQ-ROAD-RAG-001 | Repo PC | — | ingest-testsuite-pro + product-deploy RAG | Jobs OK | — | ccli-rag-source-index |
| □ | U-ACC-01 | ACC | O.15 #5 | Formal UCA | Accredited | Lab | IEC 61850-10 full programme | Certificate | lab report | ccli-61850-10-extract |
| □ | U-ACC-02 | ACC | O.15 #6 · §12 | P7-07 | Accredited | Lab | **62351-100-3** report | Report ID | 100-3 cert | ccli-62351-100-3-extract |
| □ | U-ACC-03 | ACC | O.15 #6 | T.3.3.4 | Accredited | Lab | Optional **62351-100-4** | If quoted | 100-4 report | ccli-62351-100-4-extract |
| □ | U-ACC-04 | ACC | O.15 #4 | 61557-12 | External | Lab | Metrology / accuracy | Cert **OPEN** | — | ccli-61557-12-extract |
| □ | U-ACC-05 | ACC | O.15 #4 | 62443-4-1/4-2 | Accredited | Lab | Component cyber cert | **OPEN** | CSA-style | ccli-62443-4-2-extract |
| □ | U-ACC-06 | ACC | CEI 0-16 | Product cert | Notified body | Lab | Full product CEI path | Programme | — | cei-0-16-consolidata-2025-12 |

---

## Verification (programme gates)

| REQ-ID | Pass when |
|--------|-----------|
| REQ-UNI-MTX-001 | Every **U-T1-*** / **U-T3-*** row for submitted PICS **M** is □ or PART+waiver |
| REQ-UNI-MTX-002 | Specimen = **5-LN** per PRODUCT_SPECIMEN_5LN · validator PASS |
| REQ-5LN-VAL-001 | `validate-5ln-cid.ps1` exit 0 on Compare CID |
| REQ-5LN-VAL-002 | Browse = exactly 5 MMS names (binding table above) |
| REQ-UNI-MTX-003 | Cyber **U-D7-03** tracked separately — TSP ≠ accredited 100-3 |
| REQ-UNI-MTX-004 | **U-ACC-*** rows only □ when external report ID recorded |
| REQ-LAB-MTX-001…005 | Same as legacy functional matrix verification section |

---

## Return path

1. Copy evidence → `lab/evidence/testsuite-pro/inbox/`  
2. Commit done-matrix copy (optional): `LAB_UNIFIED_CERT_TEST_MATRIX_YYYY-MM-DD_done.md`  
3. Update [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md) SHA table  
4. RAG ingest (see roadmap)

**Keywords:** `unified`, `matrix`, `O.15`, `5-LN`, `TSP`, `62351`, `UCA`, `ccli-lab-unified-cert-test-matrix`

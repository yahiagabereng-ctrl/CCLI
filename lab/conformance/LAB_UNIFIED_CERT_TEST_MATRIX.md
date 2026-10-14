# Unified certification test matrix — functional + cyber + O.15 (5-LN first run)

**Document ID:** CCLI-LAB-UNIFIED-MTX-001  
**Revision:** 1.3  
**Date:** 2026-10-14  
**Session pin:** [LAB_UNIFIED_SESSION_PIN_2026-10-14.md](LAB_UNIFIED_SESSION_PIN_2026-10-14.md) (`ccli-lab-unified-session-pin-2026-10-14`)  
**Finalize path:** [DUT_LAB_FINALIZE_AND_CERT_PATH.md](DUT_LAB_FINALIZE_AND_CERT_PATH.md)  
**RAG init:** [RAG_SOURCE_INDEX.md](RAG_SOURCE_INDEX.md) · [PRODUCT_DEPLOYMENT_INIT.md](PRODUCT_DEPLOYMENT_INIT.md)
**TestPro execution:** [LAB_UNIFIED_TSP_5LN_EXECUTION.md](LAB_UNIFIED_TSP_5LN_EXECUTION.md) (`ccli-lab-unified-tsp-5ln-execution`)  
**Evidence protocol:** [../evidence/testsuite-pro/EVIDENCE_PER_TEST_PROTOCOL.md](../evidence/testsuite-pro/EVIDENCE_PER_TEST_PROTOCOL.md) · `scripts/tsp-evidence-logger.ps1`
**5-LN validation:** [PRODUCT_SPECIMEN_5LN_VALIDATION.md](PRODUCT_SPECIMEN_5LN_VALIDATION.md) · `scripts/validate-5ln-cid.ps1`
**RAG source_id:** `ccli-lab-unified-cert-test-matrix`  
**Roadmap:** [LAB_UNIFIED_CERT_TEST_ROADMAP.md](LAB_UNIFIED_CERT_TEST_ROADMAP.md)  
**Specimen:** [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md) (**5-LN** product)  
**Excel import:** [LAB_UNIFIED_CERT_TEST_MATRIX.csv](LAB_UNIFIED_CERT_TEST_MATRIX.csv)

**Supersedes as single □ checklist:** merges [LAB_REQUIREMENT_TEST_MATRIX.md](LAB_REQUIREMENT_TEST_MATRIX.md) + [LAB_REQUIREMENT_TEST_MATRIX_CYBER.md](LAB_REQUIREMENT_TEST_MATRIX_CYBER.md) + O.15 non-TSP deliverables from `ccli-dnv-product-readiness`. Legacy files remain authoritative for row detail; tick **here** for programme traceability.

**Field pack:** `scripts/stage-tsp-field-pack.ps1` → `00_UNIFIED_CERT_TEST_MATRIX.md` + `00b_UNIFIED_CERT_ROADMAP.md`

**Use this file as the live finalize checklist:** tick □ in the master table; record **Status** in the session header; on closeout save `LAB_UNIFIED_CERT_TEST_MATRIX_<date>_done.md` (**U-CLO-02**).

---

## Finalize clauses (how to run the matrix)

### Row status (write in `_done` copy or session index)

| Mark | Meaning | When |
|------|---------|------|
| **PASS** | Evidence meets pass criteria | Default for executed rows |
| **PART** | Executed with documented waiver | Compare Value rows · interim 31-LN cfg vs 5-LN CID |
| **FAIL** | Ran; criteria not met | Block Milestone A until fixed or waived |
| **N/A** | Out of specimen PICS scope | **U-N/A-01** · PICS **N/A** rows |
| **ACC** | External lab only | **U-ACC-*** · **U-D7-03** — track report ID, not bench □ |
| **SKIP** | Not run this session | Document reason in session header |

### Programme requirements (finalize)

| ID | Clause |
|----|--------|
| REQ-UNI-FIN-001 | **Single checklist:** only this matrix □ for programme traceability (legacy matrices = detail) |
| REQ-UNI-FIN-002 | **Phase 0a** (**U-P0-09…11**) complete before **U-T1-01** |
| REQ-UNI-FIN-003 | **Milestone A** (internal DUT finalize): minimum row set in table below — all **M** PICS rows **PASS** or **PART+waiver** |
| REQ-UNI-FIN-004 | **Milestone B** (lab quote): **U-DOC-01…07** + Milestone A |
| REQ-UNI-FIN-005 | **Milestone C** (certificate): **U-ACC-*** + accredited report IDs — not gated on bench day |
| REQ-UNI-FIN-006 | Every PC-B logger row: four artefacts per [EVIDENCE_PER_TEST_PROTOCOL.md](../evidence/testsuite-pro/EVIDENCE_PER_TEST_PROTOCOL.md) |
| REQ-UNI-FIN-007 | Session header + [session pin](LAB_UNIFIED_SESSION_PIN_2026-10-14.md) + deploy manifest SHA agree |

### Milestone A — internal lab finalize (minimum □ set)

| Include | U-IDs |
|---------|--------|
| Prep | **U-P0-01 … U-P0-11**, **U-P0b-01 … U-P0b-05** |
| MMS core | **U-T1-01 … U-T1-10**, **U-T1-05b/c/d** if PICS **M**, **U-T1-11** |
| Annex trace | **U-P5-01** (+ **U-P5-02** if dynamic P in scope) |
| Cyber (recommended before ship) | **U-T3-01 … U-T3-05**, **U-D7-01**, **U-D7-02** |
| Regulation / time | **U-P3-11** |
| Closeout | **U-CLO-01 … U-CLO-03** |

**Strongly recommended same programme:** **U-P4-01/02**, **U-P7-01/02/04**, **U-DOC-05** (SCL Verify).

### Pre-test RAG (Phase 0a — repo PC, before bench)

Run from **CCLI Lab** root (RAG on `localhost:8001`):

```powershell
powershell -File scripts\sync-ccli-corpus-to-telematry.ps1
powershell -File scripts\ingest-product-deploy-rag.ps1
powershell -File scripts\ingest-next-phases.ps1 -SkipDocker
powershell -File scripts\ingest-62351-100-4.ps1 -SkipSync
# Optional before U-T3-02…04 disputes:
powershell -File scripts\ingest-62351-security-debug.ps1
```

**Probe:** `ccli-lab-unified-cert-test-matrix` · `ccli-lab-unified-tsp-5ln-execution` · `ccli-lab-unified-session-pin-2026-10-14` · `ccli-pc1-log-capture-requirements`

Evidence of run (optional): `lab/evidence/testsuite-pro/inbox/RAG_PREFLIGHT_<date>.txt` (console log).

---

## Session header (fill before Phase 1)

| Field | Value |
|-------|--------|
| Date | *(fill)* |
| Operator | *(fill)* |
| Specimen | **5-LN product** per PRODUCT_SPECIMEN_5LN |
| TSP version | *(fill)* |
| DUT `ccli --version` | **0.1.0-r53** (target) · see `apps/ccli/VERSION` |
| CID file | `product_tr400_eth_a_5ln.cid` (or interim note) |
| cfg SHA-256 | *(fill)* |
| Bench | **Two-PC** · PC-B TSP · PC-A 104/LAN3/COM5 |
| Git / manifest | `4fdfcd9`+ · [LAB_UNIFIED_SESSION_PIN_2026-10-14.md](LAB_UNIFIED_SESSION_PIN_2026-10-14.md) |
| Evidence index | `TSP_UNIFIED_5LN_SESSION_INDEX_<date>.md` in `lab/evidence/testsuite-pro/inbox/` |
| RAG preflight (Phase 0a) | **U-P0-09** done · probe ids return current rev |
| RAG ingest after closeout | `ingest-testsuite-pro.ps1` + `ingest-product-deploy-rag.ps1` (U-CLO-03) |
| Finalize milestone target | **A** / **B** / **C** (see clauses above) |
| Matrix `_done` copy | `LAB_UNIFIED_CERT_TEST_MATRIX_<date>_done.md` (**U-CLO-02**) |

---

## TestPro log bundle (PC-B — every U-T1-* / U-T3-* / U-P5-01)

One matrix row = one **`Start` → TSP step → `Stop -PasteClipboard`** pair. Each row produces **four** artefacts (see execution doc):

| # | Suffix / folder | Example |
|---|-----------------|--------|
| 1 | `inbox/TSP_*.txt` | TSP Output paste + header `result: PASS` |
| 2 | `inbox/TSP_*_DUT_LOG.txt` | SSH PRE/POST (`lab-env.ps1` on PC-B LAN1) |
| 3 | `inbox/TSP_*_WIRE.txt` | tshark frame summary |
| 4 | `pcap/TSP_*.pcapng` | `host 192.168.10.1 and port 3782` |

**Day start (PC-B):** `tsp-evidence-logger.ps1 -Action Init -SessionDate <date> -Operator … -CcliPkg … -TspVersion …`

**Logger `-TestId` map:** full table in [LAB_UNIFIED_TSP_5LN_EXECUTION.md](LAB_UNIFIED_TSP_5LN_EXECUTION.md) (aliases **U-T1-07** via `-UnifiedId`).

**Compare (U-T1-02):** also export TSP **Compare `.xlsx`** beside `TSP_P3_COMPARE_*`; save `validate-5ln-cid.ps1` console to `inbox/VALIDATE_5LN_CID_<date>.txt`.

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
| `ccli-lab-unified-tsp-5ln-execution` | TestPro row ↔ logger ↔ filenames |
| `ccli-p5-fullcircle-5ln-scope` | U-P5-01 steps (not 31-LN sequencer) |
| `ccli-product-specimen-5ln-validation` | Compare/Browse gates |
| `ccli-testsuite-pro-procedure` | Evidence naming · inbox layout |
| `ccli-tsp-two-pc-bench-runbook` | Two-PC phases 0–3 |
| `ccli-tsp-field-pack` | Stage script · TLS/CID pack |
| `cei-tr-57-126-extract` | PdCMMXU1 / TR-57-126 LNs |
| `ccli-lab-unified-session-pin-2026-10-14` | r53 deploy · cfg · matrix readiness |
| `ccli-pc1-log-capture-requirements` | PC-B Wireshark · OpenSSL · logger |
| `ccli-62351-8-extract` | Viewer RBAC (**U-T3-02/03**) |
| `ccli-62351-9-extract` | CRL (**U-T3-04**) |
| `ccli-phase-regulation-checklists` | IEC 104 (**U-P4-***) |
| `ccli-rag-source-index` | Ingest index · gap list |
| `ccli-product-deployment-init` | Pre-matrix RAG one-shot |

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

**Columns:** □ · **U-ID** · **Phase** · **O.15 / cert** · **REQ** · **Gate** · **Executor** · **HW** · **Action** · **Pass** · **Evidence** · **Logger** · **RAG**

**Logger:** PC-B TSP rows use `tsp-evidence-logger.ps1 -TestId` as shown; **—** = manual / PC-A / engineering.

| □ | U-ID | Phase | O.15 / cert | REQ / gate | Exec | HW | Action (summary) | Pass criteria | Evidence | Logger | RAG |
|---|------|-------|-------------|------------|------|-----|------------------|---------------|----------|--------|-----|
| □ | U-P0-01 | 0 | Deploy | REQ-ROAD-HW | Both | — | `git pull` · `stage-tsp-field-pack.ps1` on PC-B | Pack complete | PACK_MANIFEST.json | — | ccli-tsp-field-pack |
| □ | U-P0-02 | 0 | Deploy | P3-01 | PC-B | LAN1 | `set_pc_lan1_dso.cmd` · ping `.10.1` | RTT OK | PREFLIGHT_LAN1_*.txt | — | ccli-tsp-two-pc-bench-runbook |
| □ | U-P0-03 | 0 | Deploy | P4 | PC-A | LAN2 | `set_pc_lan2_oa.cmd` · ping `.1.130` | RTT OK | PREFLIGHT_LAN2_*.txt | — | ccli-tsp-two-pc-bench-runbook |
| □ | U-P0-04 | 0 | Deploy | P5 plant | PC-A | LAN3 | `set_pc_lan3_plant*.cmd` · ping DUT + meter | Both OK | modbus_emt432_*.txt | — | ccli-tsp-two-pc-bench-runbook |
| □ | U-P0-05 | 0 | Deploy | REQ-ROAD-HW-003 | PC-A | RS485 | Wire A1/B1 + GND | Continuity | P5_A1B1 map | — | ccli-tr500-io-peripheral |
| □ | U-P0-06 | 0 | UCA prep | D5 | PC-B | — | Import **5-LN** CID + TLS PEMs | Load OK | TSP project snapshot | — | ccli-product-specimen-5ln |
| □ | U-P0-07 | 0 | UCA prep | — | PC-A | SSH | `validate-dut-unified-matrix.ps1 -FixServices -WriteEvidence` | 3782+2404 · 1× ccli · no :102 | PREFLIGHT_DUT_UNIFIED_*.txt | — | ccli-lab-dut-finalize-cert-path |
| □ | U-P0-08 | 0 | Trace | REQ-LAB-MTX-003 | Both | — | Record version + cfg SHA in header | Match pin doc | session header + index md | — | ccli-product-specimen-5ln |
| □ | U-P0-09 | 0a | RAG | REQ-UNI-FIN-002 | Repo PC | — | Sync + ingest P0 corpus (see **Pre-test RAG** above) | Probes hit · no skip for unified ids | RAG_PREFLIGHT_*.txt | — | ccli-rag-source-index |
| □ | U-P0-10 | 0a | Evidence | REQ-UNI-FIN-006 | PC-B | — | Wireshark · OpenSSL · read **PC1 log** doc · stage field pack | Tools OK · `00h` in pack | PREFLIGHT_PC1_TOOLS_*.txt | — | ccli-pc1-log-capture-requirements |
| □ | U-P0-11 | 0a | Deploy | REQ-UNI-FIN-007 | PC-A | SSH | Match [session pin](LAB_UNIFIED_SESSION_PIN_2026-10-14.md) · deploy manifest · **U-P0-07** | r53 (or header version) on DUT | PREFLIGHT_DUT_UNIFIED_*.txt | — | ccli-lab-unified-session-pin-2026-10-14 |
| □ | U-P0b-01 | 0b | Annex T | T.3.1 | PC-A | LAN3 | `modbus_emt432_tcp_probe.py` · **V_L1_L2 @980** → PPV kV | Modbus OK + PPV hint | modbus_emt432_*.txt | — | ccli-validation-strategy |
| □ | U-P0b-02 | 0b | Annex T | T.3.1 | PC-A | COM5 | `start-bench-plant.ps1` Sun on COM5 | FC03 inverter map | PLANT_SUN2000_*.txt | — | ccli-tsp-two-pc-bench-runbook |
| □ | U-P0b-03 | 0b | O.14 | P7-04 | PC-A | SSH | DUT modbus `link_up` in event-dump | Category present | P7_04_MODBUS_LINK_*.txt | — | ccli-lab-requirement-test-matrix |
| □ | U-P0b-04 | 0b | O.14 | P7-01 | PC-A | SSH | Optional `ccli --event-wrap-test` once | Log saved | P7_01_WRAP_*.txt | — | ccli-lab-dut-finalize-cert-path |
| □ | U-P0b-05 | 0b | — | — | PC-A | — | Do **not** start TSP until plant path chosen | yaml matches HW | DUT lab.yaml copy | — | ccli-lab-config-guide |
| □ | U-DOC-01 | DOC | O.15 #5 | D1 PICS | Eng | — | PICS **5-LN** Cover + ACSI **M** | Lab quote ready | PICS xlsx | — | ccli-lab-pics-fill |
| □ | U-DOC-02 | DOC | O.15 #5 | D2 PIXIT | Eng | — | Paste PIXIT (3782, IntgPd 4000) | Complete | PIXIT doc | — | ccli-lab-pixit-draft |
| □ | U-DOC-03 | DOC | O.15 #5 | D3 MICS | Eng | — | MICS = **5-LN** CID | Match Compare | MICS doc | — | ccli-lab-mics-draft |
| □ | U-DOC-04 | DOC | O.15 #5 | D4 TICS | Eng | — | TICS after lab TISSUES date | Signed | TICS doc | — | ccli-lab-templates-test-scope |
| □ | U-DOC-05 | DOC | O.15 #5 | D5 CID | Eng | — | Cert CID + SCL Verify **0 Error** | Verify log | TSP_OFF_SCLVERIFY_* | — | ccli-testsuite-pro-procedure |
| □ | U-DOC-06 | DOC | O.15 #6 | D7 | Eng | — | Submit D7 62351-3 PID | Tables 1–5 | D7 draft | — | ccli-lab-d7-62351-3-pid |
| □ | U-DOC-07 | DOC | O.14 | D6 | Eng | — | LAB_CONFIG_GUIDE + firmware SHA | D6 pack | LAB_CONFIG_GUIDE | — | ccli-lab-config-guide |
| □ | U-DOC-08 | DOC | O.15 | REQ-LAB-DOC-004 | Lab | DUT | P7-01/04/11 PASS before ship | Evidence set | P7_* | — | ccli-lab-dut-finalize-cert-path |
| □ | U-T1-01 | 1 | O.15 #5 UCA | P3-01 · sAss | PC-B | LAN1 | TSP **Connect** mutual TLS | Initiate Success | TSP_P3_01_CONNECT_* bundle | T1-01 | ccli-lab-unified-tsp-5ln-execution |
| □ | U-T1-02 | 1 | O.15 #5 | P3-02 · D3 | PC-B | LAN1 | **Compare** vs **5-LN** CID | validate PASS · 0 MISSING | TSP_P3_COMPARE_* + xlsx | T1-02 | ccli-product-specimen-5ln-validation |
| □ | U-T1-03 | 1 | O.15 #5 | P3-03 · sSrv | PC-B | LAN1 | Browse `LD_Plant` | **Exactly 5** LNs | TSP_P3_03_BROWSE_* bundle | T1-03 | ccli-product-specimen-5ln |
| □ | U-T1-04 | 1 | O.15 #5 · T | P3-03 | PC-B | LAN1 | Read TotW, TotVAr, PPV | Plausible | TSP_P3_03_READ_PdC_* bundle | T1-04 | cei-tr-57-126-extract |
| □ | U-T1-05 | 1 | O.15 #5 · T | P3-03 · sRp | PC-B | LAN1 | URCB IntgPd=4000 | RptEna true | TSP_P3_03_REPORT_URCB_* bundle | T1-05 | ccli-lab-requirement-test-matrix |
| □ | U-T1-05b | 1 | O.15 #5 | S27-1 | PC-B | LAN1+PC-A | SetURCB **dchg** + ΔP | Report | TSP_P3_03_REPORT_DCHG_* bundle | T1-05b | ccli-lab-pics-filled-workbook |
| □ | U-T1-05c | 1 | O.15 #5 | S27-2 | PC-B | LAN1 | SetURCB **qchg** | Report or N/A | TSP_P3_03_REPORT_QCHG_* bundle | T1-05c | ccli-lab-requirement-test-matrix |
| □ | U-T1-05d | 1 | O.15 #5 | S27-3 | PC-B | LAN1 | SetURCB **dupd** | Report | TSP_P3_03_REPORT_DUPD_* bundle | T1-05d | ccli-61850-10-extract |
| □ | U-T1-06 | 1 | O.15 #5 | P3-03 | PC-B | LAN1 | Integrity ~4 s | Δt 3900–4100 ms | TSP_P3_03_REPORT_TotW_* bundle | T1-06 | ccli-lab-pixit-draft |
| □ | U-T1-07 | 1 | O.15 · O.9 | P3-04 · sCtl | PC-B | LAN1 | Operate **Wlim** | Success | TSP_P3_04_OPERATE_Wlim_* bundle | T1-07 | cei-0-16-allegato-o-extract |
| □ | U-T1-08 | 1 | O.15 · O.9 | P3-04 | PC-B | LAN1 | Operate **WSd** | Success | TSP_P3_04_OPERATE_WSd_* bundle | T1-08 | cei-0-16-allegato-o-extract |
| □ | U-T1-09 | 1 | O.15 #5 | P3-05 | PC-B | LAN1 | Release · 15 s fallback | Per PIXIT | TSP_P3_05_FALLBACK_* bundle | T1-09 | ccli-lab-requirement-test-matrix |
| □ | U-T1-10 | 1 | O.15 #5 | P3-01 | PC-B | LAN1 | Disconnect · reconnect | Associate OK | TSP_P3_01_RECONNECT_* bundle | T1-10 | ccli-lab-requirement-test-matrix |
| □ | U-T1-11 | 1 | O.9.2.2 | P3-04 | PC-A | SSH | `ubus call dido_v2 status` after Wlim | Actuation | append `_DUT_LOG` | — | ccli-lab-requirement-test-matrix |
| □ | U-T3-01 | 3 | O.15 #6 · T | P3-06 | PC-B | LAN1 | Connect **no** client cert | Fail | TSP_P3_06_TLS_FAIL_* bundle | T3-01 | ccli-lab-requirement-test-matrix-cyber |
| □ | U-T3-02 | 3 | O.15 #6 | P3-08 | PC-B | LAN1 | Viewer read OK | Pass | TSP_P3_08_VIEWER_READ_* bundle | T3-02 | ccli-62351-8-extract |
| □ | U-T3-03 | 3 | O.15 #6 | P3-08 | PC-B | LAN1 | Viewer Operate deny | Deny | TSP_P3_08_VIEWER_OPERATE_DENY_* | T3-03 | ccli-62351-8-extract |
| □ | U-T3-04 | 3 | O.15 #6 | P3-09 | PC-B | LAN1 | Revoked + CRL | Fail TLS | TSP_P3_09_CRL_* bundle | T3-04 | ccli-62351-9-extract |
| □ | U-T3-05 | 3 | O.15 #6 · T | P3-07 | PC-B | LAN1 | DSO ACSE / dual cert | AARE OK | TSP_P3_07_DSO_ACSE_* bundle | T3-05 | ccli-62351-4-extract |
| □ | U-D7-01 | 3 | O.15 #6 | REQ-D7-PORT | Eng | DUT | Port audit **3782** only | No :102 ship | yaml audit txt | — | ccli-product-sw-freeze |
| □ | U-D7-02 | 3 | O.15 #6 | REQ-D7-CRYPTO | Eng | — | `verify_annex_pki.py` | RSA≥2048 | PKI log | — | ccli-lab-d7-62351-3-pid |
| □ | U-D7-03 | 3 | O.15 #6 | REQ-D7-1003 | Accredited | Lab | **62351-100-3** suite | Report ID | cert report | — | ccli-62351-100-3-extract |
| □ | U-P5-01 | 2 | O.8 · O.9 | P5 | PC-B | LAN1 | **5-LN** annex trace TLS | No 31-LN steps | TSP_P5_FULLCIRCLE_TLS_* bundle | P5-01 | ccli-p5-fullcircle-5ln-scope |
| □ | U-P5-02 | 2 | O.8 | P5 | PC-A | COM5/meter | Vary P step 13 | Dynamic read | RTU/meter log | — | ccli-tsp-two-pc-bench-runbook |
| □ | U-P4-01 | 4 | O.13 | P4 | PC-A | LAN2 | 104 → :2404 TLS | Session up | phase4/P4_* | — | ccli-phase-regulation-checklists |
| □ | U-P4-02 | 4 | O.13 | P4 | PC-A | LAN2 | Command reject | Reject | P4 log + event-dump | — | cei-0-16-allegato-o-extract |
| □ | U-P7-01 | 5 | O.14 | P7-01 | PC-A | SSH | event-wrap / clear fail | exit 2 clear | P7_01_WRAP_* | — | ccli-lab-requirement-test-matrix |
| □ | U-P7-02 | 5 | O.14 | P7-02 | PC-A | SSH | event-dump ×20 | UTC format | P7_02_DUMP_* | — | cei-0-16-allegato-o-extract |
| □ | U-P7-03 | 5 | O.14 | P7-04 | Eng | — | Event coverage matrix | Gaps doc | P7-04_EVENT_COVERAGE_MATRIX | — | ccli-lab-requirement-test-matrix |
| □ | U-P7-04 | 5 | O.14 · K7.5 | P7-11 | PC-A | SSH | P7-11 DSO demo | PASS tail | P7-11_DSO_DEMO_* | — | ccli-lab-dut-finalize-cert-path |
| □ | U-P3-11 | 1/2 | T.3.3.4.5 | P3-11 | PC-A | SSH | `chronyc tracking` | \|offset\| ≤ 100 ms | P3_11_* | — | cei-0-16-allegato-t-extract |
| □ | U-N/A-01 | — | — | — | — | — | BRCB / GOOSE / File / SG | PICS **N/A** | — | — | ccli-lab-cid-pics-alignment |
| □ | U-CLO-01 | 6 | Internal | REQ-LAB-MTX-002 | Both | USB | Copy inbox + pcap | Names match | inbox/ + pcap/ | — | ccli-testsuite-pro-procedure |
| □ | U-CLO-02 | 6 | Internal | REQ-LAB-MTX-001 | Eng | — | Save `*_done.md` matrix | All **M** □ | done copy | — | ccli-lab-unified-cert-test-matrix |
| □ | U-CLO-03 | 6 | Internal | REQ-ROAD-RAG-001 | Repo PC | — | ingest TSP + product RAG | Jobs OK | ingest log | — | ccli-rag-source-index |
| □ | U-ACC-01 | ACC | O.15 #5 | Formal UCA | Accredited | Lab | 61850-10 programme | Certificate | lab report | — | ccli-61850-10-extract |
| □ | U-ACC-02 | ACC | O.15 #6 | P7-07 | Accredited | Lab | **62351-100-3** | Report ID | 100-3 cert | — | ccli-62351-100-3-extract |
| □ | U-ACC-03 | ACC | O.15 #6 | T.3.3.4 | Accredited | Lab | Optional **100-4** | If quoted | 100-4 report | — | ccli-62351-100-4-extract |
| □ | U-ACC-04 | ACC | O.15 #4 | 61557-12 | External | Lab | Metrology | **OPEN** | — | — | ccli-61557-12-extract |
| □ | U-ACC-05 | ACC | O.15 #4 | 62443 | Accredited | Lab | Component cyber | **OPEN** | CSA-style | — | ccli-62443-4-2-extract |
| □ | U-ACC-06 | ACC | CEI 0-16 | Product cert | Notified body | Lab | Full CEI path | Programme | — | — | cei-0-16-consolidata-2025-12 |

---

## Verification (programme gates)

| REQ-ID | Pass when |
|--------|-----------|
| REQ-UNI-FIN-002 | **U-P0-09…11** □ before first **U-T1-01** |
| REQ-UNI-FIN-003 | Milestone A row set □ with **PASS** or **PART** + waiver ID in evidence |
| REQ-UNI-FIN-006 | Spot-check ≥3 logger bundles: `.txt` + `_DUT_LOG` + `_WIRE` + `.pcapng` |
| REQ-UNI-FIN-007 | DUT `ccli --version` = session header = pin doc = deploy manifest |
| REQ-UNI-MTX-001 | Every **U-T1-*** / **U-T3-*** row for submitted PICS **M** is □ or PART+waiver |
| REQ-UNI-MTX-002 | Specimen = **5-LN** per PRODUCT_SPECIMEN_5LN · validator PASS |
| REQ-5LN-VAL-001 | `validate-5ln-cid.ps1` exit 0 on Compare CID |
| REQ-5LN-VAL-002 | Browse = exactly 5 MMS names (binding table above) |
| REQ-UNI-MTX-003 | Cyber **U-D7-03** tracked separately — TSP ≠ accredited 100-3 |
| REQ-UNI-MTX-004 | **U-ACC-*** rows only □ when external report ID recorded |
| REQ-LAB-MTX-001…005 | Same as legacy functional matrix verification section |

---

## Return path

1. Copy evidence → `lab/evidence/testsuite-pro/inbox/` (**U-CLO-01**)  
2. Save done-matrix: `LAB_UNIFIED_CERT_TEST_MATRIX_YYYY-MM-DD_done.md` with **Status** column (**U-CLO-02**)  
3. Update [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md) SHA table  
4. RAG ingest: `ingest-testsuite-pro.ps1` + `ingest-product-deploy-rag.ps1` (**U-CLO-03**)  
5. If Milestone A claimed: [DUT_LAB_FINALIZE_AND_CERT_PATH.md](DUT_LAB_FINALIZE_AND_CERT_PATH.md) §2 checklist cross-check

## Finalize sign-off (Milestone A)

| Field | Value |
|-------|--------|
| Session date | |
| Operators (PC-A / PC-B) | |
| DUT version + cfg SHA | |
| Milestone | **A** internal / **B** quote prep / **C** accredited track |
| Open waivers (IDs) | |
| Sign-off (name · date) | |

**Keywords:** `unified`, `matrix`, `O.15`, `5-LN`, `TSP`, `62351`, `UCA`, `finalize`, `ccli-lab-unified-cert-test-matrix`

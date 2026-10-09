# Lab session runbook — Annex-mapped (Test Suite Pro)

**Document ID:** CCLI-LAB-SESSION-2026-10-09  
**Revision:** 1.0  
**Date:** 2026-10-09  
**RAG source_id:** `ccli-lab-session-2026-10-09-annex-runbook`  
**Purpose:** Single **live checklist** for today’s bench: every TSP step mapped to **CEI 0-16 Allegato T/O**, internal **P3/P5** gates, and **61850-10** groups. Tick **□** as you go; save evidence under [`../evidence/testsuite-pro/inbox/`](../evidence/testsuite-pro/inbox/).

**Master matrix (same rows, submission view):** [LAB_REQUIREMENT_TEST_MATRIX.md](LAB_REQUIREMENT_TEST_MATRIX.md)  
**Two-PC bench (PC-B TSP + PC-A LAN2/logger):** [TSP_TWO_PC_BENCH_RUNBOOK.md](TSP_TWO_PC_BENCH_RUNBOOK.md)  
**TSP step detail:** [../evidence/testsuite-pro/TSP_TEST_IDENTIFICATION_POST_CONNECT.md](../evidence/testsuite-pro/TSP_TEST_IDENTIFICATION_POST_CONNECT.md)  
**Annex sequence (P5, clause-level):** [../evidence/phase5/P5_ANNEX_TEST_SEQUENCE.md](../evidence/phase5/P5_ANNEX_TEST_SEQUENCE.md)

---

## Annex corpus (open on dev PC while testing)

| Document | Repo path | Use in session |
|----------|-----------|----------------|
| **Allegato T** (DSO interface, timing, security tables) | [`Architecture/_extracted_reg_analysis/cei_0-16_consolidated_allegato_t_it.txt`](../../Architecture/_extracted_reg_analysis/cei_0-16_consolidated_allegato_t_it.txt) | T.3.x MMS, T.3.3.4.5 time, G.2 certs |
| **Allegato O** (POC measures, 4 s sync, controls) | [`Architecture/_extracted_reg_analysis/cei_0-16_consolidated_allegato_o_it.txt`](../../Architecture/_extracted_reg_analysis/cei_0-16_consolidated_allegato_o_it.txt) | O.8.3 · O.9.2.x |
| **TR 57-126 model** | RAG `cei-tr-57-126-extract` | LN classes in CID |
| **61850-10 server tests** | RAG `ccli-61850-10-extract` | sAss / sSrv / sRp / sCtl |

**SCL / IED:** `apps/ccli/config/icd/lab_tg544_eth_a.cid` · IED **`CCI016_01`** · LD **`LD_Plant`**

---

## Session header (fill once)

| Field | Value |
|-------|--------|
| Date | 2026-10-09 |
| Operator | |
| TSP version | |
| DUT `ccli --version` | **0.1.0-r52** (P3-07-AARE-SIGN-RAW-GT) |
| DUT IP:port | `192.168.10.1:3782` TLS · 104 `192.168.1.130:2404` |
| PC role | **PC-B (TSP)** — LAN1 `192.168.10.10` · **PC-A** LAN2/LAN3/RS485 |
| Lab yaml | **`lab_tr400_phase4_emt432_lan3_tcp.yaml`** (MMS+104+EMT432 TCP) |
| Connect T1-01 | **PASS** (prior session) — re-Connect if DUT restarted |
| SW status | [`LAB_SESSION_START_2026-10-09_FINAL.md`](../evidence/testsuite-pro/inbox/LAB_SESSION_START_2026-10-09_FINAL.md) |
| Notes | **GO for Phase 1** · RS485 = **Huawei mock on PC-A** · TotW = **EMT432 TCP** |

---

## Preflight 2026-10-09 (FINAL — test start)

| Check | Result | Evidence / action |
|-------|--------|-------------------|
| **DUT yaml** | **PASS** — **`lab_tr400_phase4_emt432_lan3_tcp.yaml`** · TCP meter + float_word_swap | r52 deploy |
| **TesPro IEC61850** | **STOP** — `/etc/init.d/iec61850service stop` before `ccli` | [`PREFLIGHT_DUT_2026-10-09_200828.txt`](../evidence/testsuite-pro/inbox/PREFLIGHT_DUT_2026-10-09_200828.txt) |
| **LAN1 / MMS** | **PASS** — **r52** · **3782** | TSP GO |
| **LAN2 / 104** | **PASS** — **2404** listening · PC-A `.1.183` (user) | P4 client when scheduled |
| **LAN3 / EMT432** | **PASS** — ping `.178.250` · Modbus P/Q ≈ 0 | [`P5_EMT432_METER_VALUES_2026-10-09.txt`](../evidence/phase5/P5_EMT432_METER_VALUES_2026-10-09.txt) |
| **RS485 inverter** | **PC-A** — start **`modbus_rtu_slave.py --huawei`** on COM5 | Parallel mock; not TotW source |
| **GNSS** | **Before URCB** — `ubus call sim-manager gnss_start` | [`verify-gnss-time-quality.ps1`](../tg544-openwrt/verify-gnss-time-quality.ps1) |
| **Chrony NTP** | **PASS** — offset ~ms | prior evidence |
| **O.14 logger** | **PASS** | `--event-dump` |

**LAN2 evidence:** [`P4_01_LAN2_VERIFY_2026-10-09_115252.txt`](../evidence/phase4/P4_01_LAN2_VERIFY_2026-10-09_115252.txt)

---

## Phase 0 — Preconditions (before TSP)

| □ | Step | Annex / norm | Action | Pass |
|---|------|--------------|--------|------|
| ☑ | P0-01 | — | PC: `lab/set_pc_lan1_dso.cmd` (Admin) → `192.168.10.10/24` | ping DUT OK |
| ☑ | P0-02 | — | DUT: **`ccli` r52** on **3782** + **2404** + EMT432 TCP yaml | single `ccli` |
| □ | P0-03 | **T.3.3.4.5** | DUT: GNSS fix + chrony | **`gnss_start`** before URCB timing claims |
| □ | P0-04 | — | PC: `scripts/stage-tsp-tls.ps1` → `C:\CCLI_product_tls\` | certs present |
| □ | P0-05 | **T G.2** | TSP: MMS `client.pem`+`client_tsp.key`; TLS `client_tls.pem`; CA `root_CA.pem` | paths set |
| □ | P0-06 | **T.3.1** | TSP: import/reload **`lab_tg544_eth_a.cid`** in workspace | IED `CCI016_01` |
| □ | P0-07 | O.8 lab | **PC-A:** COM5 **Huawei mock** + LAN3 meter (TotW from **TCP** on DUT) | T1-04 reads |
| ☑ | P0-08 | **O.13** | **PC-A:** LAN2 + DUT **104 :2404** | r51+ / r52 |

Optional: `powershell -File scripts\stage-tsp-field-pack.ps1` → `C:\CCLI_TSP_FIELD_PACK\`

---

## Phase 1 — Connect + model (Tier 1 / PICS minimum)

Run order: **Connect → browse → Compare → reads → reports → controls → fallback → disconnect.**

| □ | TSP ID | Gate | **CEI 0-16** | **61850-10** | Test Suite Pro action | Evidence file |
|---|--------|------|--------------|--------------|----------------------|---------------|
| □ | **T1-01** | P3-01 · P3-06 · P3-07 | **T.3.3.3** assoc · **T.3.3** TLS · **G.2** MMS sec | **sAss** · §6.3→62351 | **Connect** `CCI016_01` @ `192.168.10.1:3782` | `TSP_P3_01_CONNECT_*.log` |
| □ | **T1-03** | P3-03 | **T.3.3.2** directory | **sSrv** | GetServerDirectory / **`LD_Plant`** · **~31 LNs** | sequencer / client log |
| □ | **T1-02** | P3-02 | **T.3.1** · TR 57-126 | Table **3** SCL | **Compare Model** vs CID (**before URCB enable**) | `TSP_P3_COMPARE_*.xlsx` |
| □ | **T1-04** | P3-03 | **T.3.1.3** @ POC | **sSrv** GetDataValues | Read `PdCMMXU1` TotW, TotVAr, PPV | `TSP_P3_03_READ_*` |
| □ | **T1-05** | P3-03 | **T.3.2.1** · **O.8.3** | **sRp** Table **14** | Enable `LLN0.urcb_PdC_Mis4sec` IntgPd=**4000** | `TSP_P3_03_REPORT_*` |
| □ | **T1-06** | P3-03 | **T.3.2.1** Type 3 · **O.8.3** | **sRp** | Δt integrity **3900–4100 ms**; dataset `DS_R_PdC_Mis4sec` | same |
| □ | **T1-07** | P3-04 | **O.9.2.2** Wlim | **sCtl** Table **26** | Operate `WlimDWMX1.Mod`=1, `WMaxSptPct`=10 (enhanced) | `TSP_P3_04_OPERATE_Wlim_*` |
| □ | **T1-08** | P3-04 | **O.9.2.3** WSd | **sCtl** | Operate `WSdDAGC1.Mod`=1, `WSptPct`=20 | `TSP_P3_04_OPERATE_WSd_*` |
| □ | — | P3-04 | **O.9.2.2** actuation | — | DUT: `ubus call dido_v2 status` after Wlim | outside TSP |
| □ | **T1-09** | P3-05 | **O.9.2.2** / U comms loss | App PIXIT | **Release** · wait **15 s** (`comms_loss_fallback_s`) | `TSP_P3_05_FALLBACK_*` |
| □ | **T1-10** | P3-01 | **T.3.3.3** | **sAss** | Disconnect + **re-Connect** | System Status export |

**Compare waivers (if structure OK):** see [../evidence/testsuite-pro/inbox/TSP_P5_COMPARE_WAIVERS.md](../evidence/testsuite-pro/inbox/TSP_P5_COMPARE_WAIVERS.md) (pattern from P5).

**Exit Phase 1:** T1-01…T1-10 □ green · Compare documented (not 488× MissingFromDiscovery — refresh discovery first).

---

## Phase 2 — Annex T full circle on TLS (Tier 2 / P5)

Same steps as [P5_TSP_FULLCIRCLE_SEQUENCER.md](../phase5/P5_TSP_FULLCIRCLE_SEQUENCER.md) but port **3782** (not cleartext :102).

| □ | P5 ref | Gate | **CEI 0-16** | TSP | Evidence |
|---|--------|------|--------------|-----|----------|
| □ | P5-00 | P3-11 | **T.3.3.4.5** | chrony ≤100 ms | `P3_11_*` |
| □ | P5-A | P3-03 | **T.3.1** · **T.3.1.3** · **T.3.1.4** | 31 LN browse | log |
| □ | P5-B | P3-02 | TR 57-126 | Compare (pre-URCB) | `TSP_P3_COMPARE_*` |
| □ | P5-C | P3-03 | **T.3.1.3** | TotW / TotVAr / PPV reads | `TSP_P5_FULLCIRCLE_TLS_*` |
| □ | P5-D/E | P3-03 | **T.3.2.1** · **O.8.3** | URCB timing + t/q in reports | same |
| □ | P5-F | P3-04 | **O.9.2.2** · **O.9.2.3** · **O.9.1.4** | Wlim · WSd · **VArSd** (opt.) | same |
| □ | P5-G | P3-05 | **O.9.2.2** / U | disconnect 15 s fallback | same |

---

## Phase 3 — Security negatives (Tier 3)

| □ | TSP ID | Gate | **CEI / norm** | Action | Expect | Evidence |
|---|--------|------|----------------|--------|--------|----------|
| □ | T3-01 | P3-06 | **T.3.3** · 62351-3 | Connect **no** client cert | TLS fail | `TSP_P3_06_TLS_FAIL_*` |
| □ | T3-02 | P3-08 | **T** Table 97–102 RBAC | **viewer** read | OK | `TSP_P3_08_VIEWER_READ_*` |
| □ | T3-03 | P3-08 | same | viewer **Operate** Wlim | **Deny** | `TSP_P3_08_*` |
| □ | T3-04 | P3-09 | **T** · 62351-9 | revoked + CRL (if DUT CRL on) | fail | `TSP_P3_09_*` |
| □ | T3-05 | P3-07 | **G.2** dual cert | DSO connect | ACSE ACCEPT (if T1-01 PASS, record) | DUT log |

**Lab note:** DUT **`crl=off`** → TSP CRL warnings OK; skip T3-04 until CRL re-issued.

---

## Phase 4 — Out of TSP (same session, annex trace)

| □ | Gate | **CEI 0-16** | Action | Evidence |
|---|------|--------------|--------|----------|
| □ | P4 | **O.13** Eth_B | 104 client LAN2 `:2404` monitor | `phase4/P4_*` |
| □ | P7-01 | **O.14** | `ccli --event-wrap-test` | `P7_01_WRAP_*` |
| □ | P7-02 | **O.14** | `ccli --event-dump --count 20` | `P7_02_DUMP_*` |
| □ | P7-11 | **K7.5** | DSO demo script | `P7-11_DSO_DEMO_*` |

**Exclude from first PICS quote (Tier 4):** BRCB · GOOSE · File · SG — see matrix rows.

---

## After session

1. Copy all `TSP_*` / `TSP_P5_*` into [`../evidence/testsuite-pro/inbox/`](../evidence/testsuite-pro/inbox/).  
2. Save completed matrix copy: `LAB_REQUIREMENT_TEST_MATRIX_2026-10-09_done.md`.  
3. Optional: `powershell -File scripts\ingest-testsuite-pro.ps1` on dev PC.

---

## Verification (this runbook)

| REQ-ID | Pass when |
|--------|-----------|
| REQ-LAB-SESSION-001 | Every executed step has □ + evidence filename |
| REQ-LAB-SESSION-002 | Each step’s **CEI** column matches signed PICS / Allegato T scope |
| REQ-LAB-SESSION-003 | T1-01 Connect on **3782** before Compare / URCB |

**RAG tags:** `lab`, `Annex-T`, `Annex-O`, `Test-Suite-Pro`, `session`, `ccli-lab-session-2026-10-09-annex-runbook`

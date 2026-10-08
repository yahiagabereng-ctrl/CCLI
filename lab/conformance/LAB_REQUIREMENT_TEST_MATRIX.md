# Lab requirement ↔ test requirement matrix (TSP field checklist)

**Document ID:** CCLI-LAB-REQ-TEST-MTX-001  
**Revision:** 1.0  
**Date:** 2026-10-08  
**RAG source_id:** `ccli-lab-requirement-test-matrix`  
**Purpose:** One checklist for **accredited lab needs** (PICS/PIXIT/MICS/61850-10) and **Test Suite Pro** runs on the **other PC** — same rows, same scope.  
**Preview on dev PC → push** via `scripts/stage-tsp-field-pack.ps1` → `C:\CCLI_TSP_FIELD_PACK\` (USB copy to TSP machine).

**Normative RAG corpus (query these when filling □ or disputing scope):**

| source_id | Use |
|-----------|-----|
| `ccli-lab-uca-document-checklist` | Lab documents D1–D7 |
| `ccli-lab-conformance-questionnaire` | Lab Q&A §1–2 |
| `ccli-61850-10-extract` | Server test groups sAss/sSrv/sRp/sCtl, Tables 3/14/26 |
| `ccli-61850-7-2-extract` | ACSI services behind PICS |
| `ccli-tsp-test-identification` | Tier 1–4 TSP steps (detail) |
| `ccli-testsuite-pro-procedure` | Paths, evidence naming |
| `cei-0-16-allegato-t-extract` · `cei-tr-57-126-extract` | Annex T / TR model |
| `ccli-phase-regulation-checklists` | P3–P7 gate IDs |

---

## Session (fill on TSP PC)

| Field | Value |
|-------|--------|
| Date | |
| Operator | |
| TSP version | |
| DUT `ccli --version` | |
| Connect `:3782` | □ PASS |
| Pack copied from | `C:\CCLI_TSP_FIELD_PACK` |

---

## Hardware prerequisites (one PC — triple NIC + USB RS485)

Full wiring and run order: **[TSP_PC_FULL_BENCH_HW.md](TSP_PC_FULL_BENCH_HW.md)** (`ccli-tsp-full-bench-hw`).

| □ | Interface | PC IP | DUT | Purpose |
|---|-----------|-------|-----|---------|
| □ | **LAN1** | `192.168.10.10` | `192.168.10.1:3782` | **Test Suite Pro** (61850) |
| □ | **LAN2** | `192.168.1.183` | `192.168.1.130:2404` | **IEC 104** client (optional same PC) |
| □ | **LAN3** | `192.168.30.10` | `192.168.30.1` | Plant / GOOSE capture (optional) |
| □ | **USB RS485** | `COM5` (typ.) | A1/B1 → DUT Modbus | **`modbus_rtu_slave.py`** — TotW/TotVAr for reads & P5 step 13 |

Scripts: `lab/set_pc_lan1_dso.cmd` · `set_pc_lan2_oa.cmd` · `set_pc_lan3_plant.cmd` (Admin, rename adapters).

---

## How to use

1. **Preview** this file on the repo PC (or RAG: *“ccli-lab-requirement-test-matrix PICS sRp”*).  
2. Run **`powershell -File scripts\stage-tsp-field-pack.ps1`** → copies this matrix + CID + TLS + sequencers into **`C:\CCLI_TSP_FIELD_PACK`**.  
3. Copy folder to TSP PC (USB / network).  
4. Execute **TSP test** column; tick **□**; save evidence under `inbox/` names in **Evidence** column.  
5. Return USB / git pull with logs — maps 1:1 to **lab submission** when PICS is filled with same **M** claims.

**First-pass PICS scope (aligned with lab quote):** server · 8-1 MMS · association · directory · get/set · datasets · **URCB** · **control** · time; **exclude** BRCB, GOOSE, SV, files, SG, 104.

---

## Master matrix

| □ | Lab / doc need | IEC 61850-10 | PICS / PIXIT | CEI 0-16 | Gate | TSP ID | Test Suite Pro (3782 TLS) | Evidence filename | Notes |
|---|----------------|--------------|--------------|----------|------|--------|---------------------------|-------------------|--------|
| □ | **D1 PICS** submitted for quote | §5.4 PID | Cover + General + ACSI sheets | O.15 path | P7 | — | Fill Excel offline; scope = rows below | `PICS_*_draft.xlsx` | Not a TSP run |
| □ | **D2 PIXIT** | Annex E PIXIT | Timeouts, ports, IntgPd | T.3.3 | P3 | — | Values: **3782**, IntgPd **1000–60000**, default **4000** | PIXIT doc | Template may be missing in repo |
| □ | **D3 MICS** + **D5 CID** match | Table 3 SCL | Model sheet | TR 57-126 | P3-02 | T1-02 | **Compare Model** vs `lab_tg544_eth_a.cid` | `TSP_P3_COMPARE_*.xlsx` | Before URCB enable |
| □ | **D4 TICS** | TISSUES | TICS Word | — | — | — | Offline | `TICS_*` | Lab gives TISSUES date |
| □ | Specimen identity | Table 1 docs | Cover firmware | — | P7 | — | Record `ccli --version` | header in every `.txt` | |
| □ | **Server** role only | sAss | B11 **M**, B12 N/A | T DSO | P3-01 | T1-01 | **Connect** `CCI016_01` mutual TLS | `TSP_P3_01_CONNECT_*` | Client = TSP |
| □ | **SCSM 8-1** MMS | sSrv mapping | B21 **M** | T MMS | P3-01 | T1-01 | Same session | same | Not 9-2 |
| □ | Associate / release | sAss Tables 4–5 | Association **M** | T.3.3.3 | P3-01 | T1-10 | Disconnect + reconnect | System Status | |
| □ | Browse model | sSrv | GetDirectory* **M** | T.3.3.2 | P3-03 | T1-03 | GetServerDirectory / LD / LN | sequencer log | ~31 LNs |
| □ | Read PdC measures | sSrv | GetDataValues **M** | T.3.1.3 | P3-03 | T1-04 | Read TotW, TotVAr, PPV | `TSP_P3_03_READ_*` | |
| □ | **URCB** 4 s integrity | **sRp** Table **14** | Reporting **M** | T.3.2.1 · O.8.3 | P3-03 | T1-05–06 | Enable `urcb_PdC_Mis4sec` IntgPd=4000 | `TSP_P3_03_REPORT_*` | Δt 3900–4100 ms |
| □ | Data sets | sDs | DataSet **M** | T | P3-03 | T1-05 | Dataset in RCB = `DS_R_PdC_Mis4sec` | same | |
| □ | **Wlim** control | **sCtl** Table **26** | Control **M** | **O.9.2.2** | P3-04 | T1-07 | Operate Mod + WMaxSptPct | `TSP_P3_04_OPERATE_Wlim_*` | ctlModel **direct-with-enhanced-security** |
| □ | **WSd** control | sCtl | Control **M** | **O.9.2.3** | P3-04 | T1-08 | Operate Mod + WSptPct | `TSP_P3_04_OPERATE_WSd_*` | |
| □ | Comms loss fallback | App behaviour | PIXIT timeout | O.9.2.2 / U | P3-05 | T1-09 | Release + wait **15 s** | `TSP_P3_05_FALLBACK_*` | yaml `comms_loss_fallback_s` |
| □ | **TLS** transport | §6.3 → 62351-100-3 | 62351-3 PID **D7** | T.3.3 | P3-06 | T1-01 | TLS handshake on **3782** | pcap optional | Not cleartext 102 |
| □ | No client cert | 62351-3 neg | PIXIT | T security | P3-06 | T3-01 | Connect without cert → fail | `TSP_P3_06_TLS_FAIL_*` | |
| □ | **ACSE** / dual cert | 62351-100-4 | PICS + 62351-4 | T G.2 | P3-07 | T3-05 | Connect DSO profile | DUT log + TSP | Connect now PASS |
| □ | **RBAC** viewer | sSrv neg | Security | T Table 97–102 | P3-08 | T3-02–03 | viewer read OK · Operate **deny** | `TSP_P3_08_*` | |
| □ | **CRL** revoked | 62351-9 | PIXIT PKI | T | P3-09 | T3-04 | revoked cert → fail | `TSP_P3_09_*` | |
| □ | Time quality | sTm 31–32 | Time **M**/O | **T.3.3.4.5** | P3-11 | Tier 2 step 0 | DUT `chronyc tracking` ≤100 ms | `P3_11_*` | |
| □ | Annex T **full circle** | Multiple | As PICS | O.8 · O.9 | P5 | Tier 2 | Run P5 sequencer on **3782** | `TSP_P5_FULLCIRCLE_TLS_*` | VArSd step 11 optional |
| □ | Actuation proof | — | — | O.9.2.2 | P3-04 | — | DUT `ubus call dido_v2 status` | outside TSP | After Wlim |
| □ | **BRCB** buffered | sBr 16–17 | Only if **M** in PICS | — | — | Tier 4 | `brcb_Stato_Allarmi_Segnali` | — | **Exclude** first quote |
| □ | **GOOSE** publish | sGop 20–25 | Only if **M** | — | P5-G | Tier 4 | GOOSE Tracker | P5_G02 closeout | **Exclude** first quote |
| □ | **IEC 104** :2404 | — | **Not in 61850 PICS** | **O.13** Eth_B | P4 | — | lib60870 on LAN2 | `phase4/P4_*` | Not TSP |
| □ | Formal **UCA certificate** | Full 61850-10 | Final PICS | O.15 | P7 | — | **Accredited lab** | lab report | After this matrix green |

---

## Lab-only rows (documents — no TSP)

| □ | ID | Action | Owner |
|---|-----|--------|--------|
| □ | D1 | PICS Excel → Nicola (quote) | PM / eng |
| □ | D2 | PIXIT Word when template added | eng |
| □ | D3 | MICS from CID | eng |
| □ | D4 | TICS after lab TISSUES date | eng |
| □ | D6 | Lab config guide + firmware ID | eng |
| □ | D7 | 62351-3 PID (may be second quote) | eng |
| □ | REQ-LAB-DOC-004 | P7-01 / P7-04 / P7-11 PASS before ship | lab TG544 |

---

## Return path (TSP PC → repo)

Copy into `lab/evidence/testsuite-pro/inbox/` on dev machine:

- All `TSP_*` logs / xlsx / csv from this session  
- `MMS_MULTITOOL_RUN_*.log` if used  
- `pcap/TSP_P3_07_mms_*.pcapng` if captured  
- Update □ column in a copy: `LAB_REQUIREMENT_TEST_MATRIX_YYYY-MM-DD_done.md`

Re-ingest RAG (dev PC):

```powershell
powershell -File scripts\ingest-testsuite-pro.ps1
powershell -File scripts\ingest-next-phases.ps1   # if Telematry sync needed
```

---

## Verification

| REQ-ID | Pass when |
|--------|-----------|
| REQ-LAB-MTX-001 | Every **M** row in submitted PICS has □ checked or explicit PART + waiver |
| REQ-LAB-MTX-002 | TSP evidence filenames match **Evidence** column |
| REQ-LAB-MTX-003 | Same `ccli_pkg` on DUT as PICS Cover |

---

**RAG tags:** `lab`, `UCA`, `PICS`, `PIXIT`, `61850-10`, `Test-Suite-Pro`, `Annex-T`, `requirement-matrix`, `ccli-lab-requirement-test-matrix`

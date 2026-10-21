# Normative certification validation matrix — all annexes and O.15 standards

**Document ID:** CCLI-LAB-NORM-CERT-VAL-001  
**Revision:** 1.2  
**Date:** 2026-10-21  
**File:** `lab/conformance/Full Validate.md`  
**RAG source_id:** `ccli-normative-cert-validation-matrix`  
**Specimen:** [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md) · IED `CCI016_01` · 5 LN  
**Parents:** `cei-0-16-allegato-o` · `cei-0-16-allegato-t` · `cei-0-16-allegato-m` · `ccli-lab-unified-cert-test-matrix` · `ccli-annex-o-software-matrix` · `ccli-dnv-product-readiness`  
**Normative edition:** CEI 0-16 consolidated **2025-12** (V5 / ARERA 385)  
**Platform:** TesPro TG544 / TG-524 (`platform_tg500`) only

This is the **single clause-by-clause book** for every normative required to issue the certificates named in **Annex O §12 / O.15**. Add evidence **table by table**. Do not delete **MISSING** rows.

**Companion (execution □):** [LAB_UNIFIED_CERT_TEST_MATRIX.md](LAB_UNIFIED_CERT_TEST_MATRIX.md)  
**Companion (O-only software map):** [ANNEX_O_SOFTWARE_MATRIX.md](ANNEX_O_SOFTWARE_MATRIX.md)

---

## How to fill (part by part)

1. Work **one heading** (A.0, A.1, …) at a time. Fill **Lines** from `apps/ccli` / `scripts/`.  
2. For each row: put evidence filename + date in **Gap** when the row moves.  
3. Tick the section box in [§ Verification log](#verification-log).  
4. **Do not** mark O.15 #2–#8 **PASS** on Milestone A. Accredited = **ACC** + report ID.

| Column | Meaning |
|--------|---------|
| **Clause** | Normative ID only (O.8.3, M.5.1, 100-3 Table 3, CR 3.6, …) |
| **Shall** | Hard requirement from the extract |
| **SW** | Implementation / artefact |
| **Test** | Unified **U-*** / phase **P*** / lab ACC |
| **5-LN DUT** | Runtime on first cert specimen |
| **Gap** | Open item or *None* |
| **Cert** | O.15 number or track (C1 / C2 / T) |
| **Lines** | Code that implements the shall: `` `path:start-end` `` · hooks; `—` if none |

| 5-LN DUT | Meaning |
|----------|---------|
| **PASS** | Bench evidence exists |
| **PART** | Partial / waiver |
| **GAP** | Applies; missing |
| **N/A** | Out of 5-LN / first UCA claim |
| **—** | Document / manufacturer / not DUT runtime |
| **ACC** | External accredited lab only |

**Tracks**

| Track | What | Closes |
|-------|------|--------|
| **C1** | UCA 61850-10 + 62351-100-3 | O.15 **#5** + **#6** |
| **C2** | Full CEI O/T/M product DoC | O.15 **#1 #2 #3 #4 #7 #8** |
| **T** | Annex T profile (feeds C1/C2; 100-4 lab-dependent) | Not a separate O.15 number |

---

## Requirements

| ID | Requirement |
|----|-------------|
| REQ-NCV-001 | One file lists every O.15 certificate type and every supporting annex clause |
| REQ-NCV-002 | First UCA specimen is **5-LN** — do not submit 31-LN PICS as cert without trim |
| REQ-NCV-003 | Milestone **A** = internal · **B** = quote pack D1–D7 · **C** = accredited report IDs |
| REQ-NCV-004 | Annex **M** is LTE/DI-DO → SPI; not Eth_A `Wlim` |
| REQ-NCV-005 | **100-3** is O.15 #6 **M**. **100-4** is T / optional unless the lab quotes it |
| REQ-NCV-006 | 62443 **3-2** is zone evidence. **4-1** = SDLA. **4-2** = CSA. CSA needs SDLA |
| REQ-NCV-007 | Corpus **MISSING** rows stay visible until ingested |

---

## Normative editions (claimed in this book)

Pin these editions in PICS / D7 / CSA quote. English O extract is **2022-03**; **V5** deltas come from the **2025-12** consolidata.

| Family | Edition claimed | Extract / pin | Notes |
|--------|-----------------|---------------|-------|
| **CEI 0-16 Allegato O** | Consolidated **2025-12** (V5 / ARERA 385); EN working text **2022-03** | `CCI_Annex_O_Extract.md` · PDF pp. 480–520 | V5 PF2 Wlim **M** ≥100 kW wind/PV |
| **CEI 0-16 Allegato T** | Consolidated **2025-12**; prior EN **2023-05 V2** | `CCI_Annex_T_Extract.md` · PDF pp. 541–660 | + **CEI TR 57-126** SCL example |
| **CEI 0-16 Allegato M** | Consolidated **2025-12** | `CCI_Annex_M_Extract.md` · PDF pp. 317–321 | ≥100 kW telescatto; **not** MMS |
| **CEI 0-16 Allegato U** | Consolidated **2025-12** (cited) | Extract **MISSING** | Operating Rule timers |
| **IEC 61850-10** | **2012+AMD1:2025** Ed. **2.1** (2025-07) | `CCI_61850-10_Extract.md` | UCA Server TP Ed 2.1 **MISSING** |
| **IEC 62351-3** | **2014+AMD1:2018+AMD2:2020** (TLS **1.2**) | `CCI_62351-3_Extract.md` · D7 | TLS 1.3 **x** first PID |
| **IEC TS 62351-100-3** | **2020** Ed. 1.0 | `CCI_62351-100-3_Extract.md` | O.15 **#6 M** |
| **IEC 62351-4** | **2018+AMD1:2020** | `CCI_62351-4_Extract.md` | A-profile §11 first; §13 **N/A** |
| **IEC TS 62351-100-4** | **2023** | `CCI_62351-100-4_Extract.md` | T / optional unless lab quotes |
| **IEC TS 62351-8** | **2011** | `CCI_62351-8_Extract.md` | RBAC; **not** #6 |
| **IEC 62351-9** | **2023** | `CCI_62351-9_Extract.md` | Key mgmt / CRL |
| **IEC 62351-14** | **CDV 2025** (capture) | `CCI_62351-14_Extract.md` | Syslog mapping |
| **IEC TS 62351-100-6** | cited by 61850-10 §6.3 | — | **N/A** (GOOSE off) |
| **IEC 62443-3-2** | **2020** | `CCI_62443_Zones.md` | ZCR evidence, not CSA |
| **IEC 62443-4-1** | **2018** | `CCI_62443-4-1_Extract.md` | SDLA; ISASecure EDSA v3.0.0 |
| **IEC 62443-4-2** | **2018** | `CCI_62443-4-2_Extract.md` | CSA SL-C `{2222222}` |
| **IEC / CEI EN 61557-12** | extract P1 | `CCI_61557-12_Extract.md` | O.15 **#2** + 61010 |

**Lines column:** repo-relative `` `path:start-end` `` as of **2026-10-21** (`apps/ccli` lab tree). ` · ` separates hooks. `—` = no firmware (docs / HW / accredited / N/A). Refresh after edits.

## Architecture

```text
P0 bind → P1 local PF2 → P2 isolation → P3 DSO MMS → P4 104
         → P5 PF1/Q → P6 Annex M → P7 evidence pack
Milestone A (bench) → B (docs) → C (U-ACC-01…06)
```

| Layer | Annex / standard | Certificate |
|-------|------------------|-------------|
| Function PF1/PF2 | **O** | #1 + feeds #5 |
| DSO 61850 / cyber profile | **T** | #5 + #6 |
| Defence plan | **M** + O.11 | #1 site / #8 file |
| Operating Rule timers | **U** | #1 legal — extract **MISSING** |
| Server ACSI | **61850-10** / UCA CB | **#5** |
| TLS transport | **62351-3** / **100-3** | **#6 M** |
| MMS ACSE / dual cert | **62351-4** / **100-4** | T / optional |
| Zones | **62443-3-2** | #4 evidence |
| SDL + component | **62443-4-1 / 4-2** | **#4** |
| Metrology / EMC | **61557-12** + 61010 | **#2** |
| Crypto module | **FIPS 140-2 L3** | **#3** or no-claim |
| QMS / CE | ISO 9001 · CE · DPR 445 | **#7 #8** |

---

## Interface matrix

| Interface | Source | Destination | Protocol | Annex |
|-----------|--------|-------------|----------|-------|
| Eth_A :3782 | DSO / TSP | CCI MMS | 61850-8-1 + 62351-3/4 | T · O.13.1.1.1 |
| Eth_B :2404 | Operator | CCI 104 | 60870-5-104 + 62351-3 | O.13 (not UCA PICS) |
| Plant RS485 / LAN3 | Meter | CCI | Modbus | O.7.4 · O.13.2 |
| DIO1 | CCI PF2 | Plant curtail | Dry contact | O.9.2 |
| DIO3 / modem DO | Annex M receiver | SPI “scatto esterno” | LTE + DI/DO | M.5.1 |
| Syslog | CCI logger | SIEM | RFC 5424 | O.14 · T.3.3.4.6 |

---

## BOM matrix (cert artefacts)

| Block | Function | Candidate / PN | Status |
|-------|----------|----------------|--------|
| D1–D5 | UCA pack | PICS PIXIT MICS TICS CID | **PART** (TICS HOLD) |
| D7 | 62351-3 PID | `D7_62351-3_PID_DRAFT.md` | **HAVE** draft |
| 62351-4 PID | MMS E2E / Cert B | separate from D7 | **OPEN** |
| 5-LN CID | Cert specimen | `product_tr400_eth_a_5ln.cid` | **HAVE** |
| PKI PEMs | Cert A / Cert B / CRL | `verify_annex_pki.py` | Lab **HAVE** |
| Accredited reports | #2–#6 | DNV / ACCREDIA / ISASecure | **OPEN** |

---

# A. Annex O — CEI 0-16 Allegato O (normative)

**Source:** `CCI_Annex_O_Extract.md` · `cei-0-16-allegato-o` · PDF pp. 480–520  
**Edition:** CEI 0-16 consolidata **2025-12** (V5 / ARERA 385); official EN extract **2022-03** only.  
**Corpus:** **HAVE** (English + V5 §2.1). Official EN extract is 2022-03 only — V5 from consolidata.

## A.0 Scope and applicability

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **O.1–O.4** | Scope, CCI role, terms | Docs | Spec vs extract | — | None (spec) | C2 #1 cover | `knowledge-base/08-engineering/CCI_Annex_O_Extract.md:1-19` · `lab/conformance/PRODUCT_SPECIMEN_5LN.md:1-24` |
| **O.2** | CCI if ≥**1000 kW** (prod+storage), **or** dispatch any size, **or** new MT wind/PV ≥**100 kW**; functions apply to **all** units at PdC | Legal + yaml plant class | **P7-10** | — | Full ARERA 385 text **MISSING** | C2 #8 | `apps/ccli/core/config/ccli_config.hpp:38-46` · `apps/ccli/core/config/ccli_config.cpp:120-135` |
| **O.2 extended** | Plants &lt; 1 MW / retrofits: conditions **ARERA not yet fixed** (extract) | Legal | **P7-10** | — | Do not claim extended perimeter | C2 #8 | — |
| **O.2 V5** | PF2 DSO P-limit **mandatory** wind/PV ≥100 kW (O.6 / O.9) | `WlimDWMX1` | **U-T1-07**, **P3-04** | **PART** | Other PF2 still ARERA-scheduled | C2 #1 | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:173-197` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:403-451` |
| **O.5** | Not a numbered shall-table in extract — use **O.6** classes | — | — | — | Do not invent O.5 tests | — | — |

## A.1 O.6 function classes

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **O.6 PF1** | Monitoring + exchange **always** | `PdCMMXU1` + URCB | **U-T1-04/05/06**, **U-P5-01** | **PART** | 4 s P/Q/V **PASS**; minute-mark + O.8.4 unit P not proven | C1 #5 · C2 #1 | `apps/ccli/adapters/modbus_master/modbus_adapter.cpp:581-593` · `apps/ccli/services/ccli_main.cpp:846-850` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1307-1349` · `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:52-59` |
| **O.6 PF2** | V/P at POC; V5: DSO P-limit **M** ≥100 kW wind/PV | `Wlim` + PF2 FSM | **U-T1-07**, **P3-04** | **PART** | Curves / 110 % Vn not on 5-LN | C2 #1 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:403-451` · `apps/ccli/core/pf2/pf2_fsm.cpp:44-89` · `apps/ccli/services/svc_pf2.cpp:29-40` |
| **O.6 PF3** | Discretionary MSD | `WSd` only; no VArSa | **P5-R08** | **N/A** | Do not claim PF3 on first PICS | C2 if claimed | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:454-501` |

## A.2 O.7 control-loop timing

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **O.7.3.1 TsP** | Active settle ≤ **60 s** to ±5 % at POC; basis MC200 | Allocation / PF2 | Closed-loop POC P step | **GAP** | No POC settling proof | C2 #1 | `apps/ccli/core/dso/dso_active_power.hpp:61-65` · `apps/ccli/core/dso/dso_active_power.cpp:43-48` |
| **O.7.3.1 TsQ** | Reactive settle ≤ **10 s** to ±5 % | Q path | **P5-R05** | **N/A** | No reactive LN on 5-LN CID | C2 #1 | `apps/ccli/core/dso/dso_phase1.cpp:147-166` |
| **O.7.3.1 MSD** | If MSD: settle must also follow TSO Grid Code gradient | PF3 | — | **N/A** | PF3 not claimed | C2 if PF3 | — |
| **O.7.3.2** | ΔT **10–600 s**, default **60 s**; cmds &lt; ΔT rejected; MCΔT | Curve schedulers | **P5-R06** | **N/A** | Curves **WAIVED** TR Fig. 2 | C2 #1 | — |
| **O.7.3.3** | External SP min **3 s**; faster **rejected** | MMS reject | **P5-R06** + flood | **PART** | No formal 3 s flood on TSP | C2 #1 | `apps/ccli/core/dso/setpoint_gate.hpp:22-56` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:209-224` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:897-901` · `apps/ccli/tests/unit/setpoint_gate_test.cpp:22-35` |
| **O.7.4** | MC200 = **200 ms fixed** block; MCΔT; V/I **synchronous**; 61557-12 Annex B/D | Modbus → MMXU | **P5-03** | **PART** | Not certified MC200 blocks | C2 **#2** | `apps/ccli/core/measurement/measurement_store.cpp:5-7` · `apps/ccli/adapters/modbus_master/modbus_adapter.cpp:286-350` |

## A.3 O.8 observability (PF1)

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **O.8.2** | Polygon Pass, Pimm, Qcap, Qind, Smax in Operating Rule | yaml `plant.q_*` | **P5-M09** | **PART** | yaml only | C2 #1 | `apps/ccli/core/config/ccli_config.cpp:131-134` · `apps/ccli/core/dso/plant_envelope.hpp:41-52` · `apps/ccli/services/ccli_main.cpp:713` |
| **O.8.3** | P, Q, V at POC every **4 s**, aligned :00/:04/…, **t + q** | `urcb_PdC_Mis4sec` IntgPd=4000 | **U-T1-05**, **U-T1-06**, **P5-M07/M08** | **PART** | Period ~4 s **PASS**; **:00/:04** wall-clock align **not** proven | C1 #5 | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:58-59` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:853-855` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1307-1349` |
| **O.8.4** | TSO via DSO; overwrite; unit P above 170/50/250 kW | 5-LN **trims** unit LNs | Do not run 31-LN sequencer | **N/A** | V5: not M for 100–500 kW aux-only | C2 if claimed | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:50-57` |
| **O.8.5** | Instantaneous P(/Q) to Aggregator 4 s (PF3) | — | — | **N/A** | PF3 not claimed | — | — |
| **O.8.6** | DG/DDG status ≤ **4 s** | Model / events | **P3-10** | **N/A** | WAIVED → product CID | C2 #1 | — |

## A.4 O.9 / O.10 control functions

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **O.9.1.1** | cosφ set-point PFSP | `PFSPDFPF1` | **P5-R02** | **N/A** | Trimmed from cert CID | C2 #1 | `apps/ccli/services/ccli_main.cpp:909-919` · `apps/ccli/core/dso/dso_phase1.cpp:166` |
| **O.9.1.2** | cosφ=f(P), δcosφ=**0.02** | `PFWDPFW1` | **P5-R04** | **N/A** | **WAIVED** TR Fig. 2 | C2 #1 | — |
| **O.9.1.3** | Q=f(V), δQ=**5 % Qmax**, lock 0.2/0.05 Pfed | `VArVDVVR1` | **P5-R03** | **N/A** | **WAIVED** TR Fig. 2 | C2 #1 | — |
| **O.9.1.4** | DSO Q set-point VArSd | `VArSdDVAR1` | **P5-R01** | **N/A** | Lab PASS 31-LN; not in 5-LN | C2 #1 | `apps/ccli/core/dso/dso_phase1.cpp:147` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:538` |
| **O.9.2.1** | Autonomous P limit ≈**110 % Un**; log every activation | PF2 + V | **P5-05** | **PART** | V path / trip not shown | C2 #1 | `apps/ccli/core/dso/dso_active_power.cpp:17-20` · `apps/ccli/core/dso/dso_active_power.hpp:27-28` |
| **O.9.2.2** | P limit on DSO cmd; no comms → Operating Rule | `Wlim` + `comms_loss_fallback_s` | **U-T1-07**, **U-T1-09**, **P3-04/05** | **PASS** | Field delay = Annex **U** | C1 #5 · C2 #1 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:403-451` · `apps/ccli/services/ccli_main.cpp:856-903` · `apps/ccli/program/outputs.c:170-172` |
| **O.9.2.3** | P modulation = **identical impl** to **O.10.3.1** | Same `WSdDAGC1` path | **U-T1-08** | **PART** | Same Operate as WSd; not a second LN (correct per O) | C1 #5 · C2 #1 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:454-501` · `apps/ccli/core/dso/dso_active_power.cpp:27-36` |
| **O.9.2 step** | Limitation step never larger than CEI 0-16 stated steps | PF2 step limit | None | **GAP** | No step-size test | C2 #1 | — |
| **O.9.3** | On Annex M trip, CCI **must not conflict** | IO3 inhibit | **P6-03** | **PASS** (r26) | Modem class GAP | C2 + M | `apps/ccli/services/svc_io.c:16-36` · `apps/ccli/program/outputs.c:170-172` |
| **O.10.1** | Start-up gradient ≤ **20 % Pn/min** | PF3 | None in unified matrix | **N/A** | Not in 5-LN | C2 if PF3 | — |
| **O.10.2** | Reconnect after PI trip: same gradient; only when V/f meet **§8.8.7.2** | PF3 / PI | None | **N/A** | — | C2 if PF3 | — |
| **O.10.3.1** | External active-power set-point | `WSdDAGC1` DOes | **U-T1-08** | **PART** | Not full ARERA timing | C1 #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:454-501` · `apps/ccli/core/dso/dso_active_power.cpp:27-36` |
| **O.10.3.2** | External Q set-point (MSD) | `VArSa` | **P5-R08** | **N/A** | Discretionary | — | — |

## A.5 O.11 priority

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **O.11 idx 1** | 110 % Un limit highest in table | Arbiter | Extend `derive_*` | **GAP** | Table incomplete | C2 #1 | `apps/ccli/core/dso/dso_active_power.cpp:17-20` |
| **O.11 idx 2** | Wlim next | Arbiter | **U-T1-07** vs others | **GAP** | — | C2 #1 | `apps/ccli/core/dso/dso_active_power.cpp:22-25` · `apps/ccli/core/dso/dso_phase1.cpp:91-139` |
| **O.11 idx 3** | P modulation | Arbiter | — | **GAP** | — | C2 #1 | `apps/ccli/core/dso/dso_active_power.cpp:27-36` |
| **O.11 idx 4** | WSd | Arbiter | **U-T1-08** | **GAP** | — | C2 #1 | `apps/ccli/core/dso/dso_active_power.cpp:27-39` · `apps/ccli/tests/unit/dso_phase1_test.cpp:51-68` |
| **O.11 idx 5–7** | Reactive tier below active | Arbiter | **P5-R07** | **N/A** | No Q LNs on 5-LN | C2 #1 | `apps/ccli/core/dso/dso_phase1.cpp:147-166` |
| **O.11 absolute (a)** | Unit O/F **not** in CCI (CEI 0-16 §8.8.6.3.2/3) | Must stay at inverter | Spec | — | None (spec) | C2 | — |
| **O.11 absolute (b)** | Annex M disconnect beats all CCI functions | Inhibit | **P6-03** | **PASS** (SW) | HW path PART | C2 | `apps/ccli/program/outputs.c:170-172` · `apps/ccli/services/svc_io.c:90-94` |

## A.6 O.12–O.13 installation, comms, power, clock

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **O.12** | Installation + V5 **teledistacco predisposition** | Drawing | **P6-04** | — | **PART** sketch | C2 #8 | — |
| **O.13.1.1.1 roles** | Eth_A = DSO; Eth_B = other remote operators | Zone bind | **P0-01**, **P2-01**, **U-P4-01** | **PASS** (roles) | — | C1 + C2 | `apps/ccli/core/config/ccli_config.hpp:184-185` · `apps/ccli/core/config/ccli_config.hpp:221-223` |
| **O.13.1.1.1 PHY Eth_A** | **100BaseFX**, optical, dual LC, **1310 nm** multimode | Copper lab | Photo / datasheet | **GAP** | Lab is copper — product PHY **not** proven | C2 #1/#8 | — |
| **O.13.1.1.1 PHY Eth_B** | 10/100BaseTX auto-MDIX (SFP allowed) | Copper | **P4-01** | **PART** | — | C2 | — |
| **O.13.1.1.1 stack** | TCP/**IPv4 M**; IPv6 **O**; 61850 MMS/GOOSE; TLS 1.2+ (62351-4/8/9); DHCP; DNS fwd | yaml + stack | **U-T1-01** | **PART** | IPv6 **N/A**; GOOSE not claimed; DHCP/DNS not tested | C1 #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:884-906` · `apps/ccli/core/config/product_cyber_gate.hpp:8-21` |
| **O.13.1.1.1 no-bridge** | **No** switch/bridge among Eth_A, Eth_B, local, plant | Firewall | **P2-01/02/03** | **PASS** | Hard gate 2026-09-25 | C1 + C2 | `apps/ccli/core/policy/net_policy.hpp:21-43` · `apps/ccli/services/ccli_main.cpp:684-691` |
| **O.13.1.1.1 link log** | Physical + data-link status of Eth_A/B in logger | O.14 iface | **P2-06**, **P7-04** | **PART** | P2-06 WAIVED → P7; iface up/down HAVE r35 | C2 #1 | `apps/ccli/services/ccli_main.cpp:1065-1071` · `apps/ccli/program/ccli_bridge.cpp:544-550` |
| **O.13.1.1.2** | Local config **serial/USB only**; IP excluded; authenticated | SSH harden / LuCI off | **P2-04** | **PART** | LuCI residual on OA | C2 #4 | — |
| **O.13.1.2** | DSO lost → delayed autonomous; restore on return | `comms_loss_fallback_s` | **P3-05**, **U-T1-09** | **PASS** | Exact seconds = **U** | C2 #1 | `apps/ccli/core/config/ccli_config.cpp:320-321` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1523-1584` · `apps/ccli/services/ccli_main.cpp:859-864` |
| **O.13.1.3** | Operator/Aggregator lost → same principle | 104 fallback | **P4-04** | **PASS** (lab) | — | C2 | `apps/ccli/adapters/iec60870_104/iec104_adapter.cpp:891-959` · `apps/ccli/core/config/product_cyber_gate.hpp:15-19` |
| **O.13.2** | Converter ≤**0.2**; CT/VT ≤**0.5**; **not** fiscal CT/VT; V5 100–500 kW may **≤5 %** | EMT432 → MMXU | **P5-04**, **U-ACC-04** | **PART** | Metrology **OPEN** | C2 **#2** | `apps/ccli/adapters/modbus_master/modbus_adapter.cpp:581-593` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:674-682` |
| **O.13.3** | Backup ≥ **1 h** for CCI + comms | PSU / UPS | HW test | **GAP** | No U-ID | C2 | — |
| **O.13.5** | UTC via GNSS; ≤ **±100 ms**; drift ≤ **1 s/day** | chrony + GNSS | **P3-11**, **U-P3-11** | **PASS** (±100 ms) | 1 s/day holdover not proven | C1 #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1355-1475` · `apps/ccli/services/ccli_main.cpp:853` |
| **O.13.4** | Firmware update: auth user; signed image; controlled deactivate; logger **never erased** (detail in **T.3.3.4.8**) | Update path | — | **GAP** | Same as T.3.3.4.8 | C2 #3/#4 | `apps/ccli/core/event/event_store.cpp:144-164` |
| **O.13.7** | Hardware tamper protection (detail in T) | Tamper / FIPS | **P7-08** | **GAP** | Ties **#3** | C2 #3 | — |
| **O.13.7.2** | Asset Inventory API (detail **T.3.3.4.7** / LPHD.PhyNam) | `LPHD1` | **U-T1-03** | **PART** | Full API not specified in O extract | T | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:90` |
| **O.13.10** | Availability per Grid Code A.13 Art. 6.2 (ARERA 628/18) | RAMS claim | Legal | — | **OPEN** | C2 | — |

## A.7 O.14 logger

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **O.14 size** | ≥**2048** events; user cannot overwrite | `EventStore` | **P7-01**, **U-P7-01** | **PART** (SW HAVE) | HW cats GAP | C2 #1 | `apps/ccli/core/event/event_ring.hpp:19` · `apps/ccli/core/event/event_store.cpp:189-308` |
| **O.14 stamp** | `yyyy/mm/dd hh:mm:ss` | `--event-dump` | **P7-02**, **U-P7-02** | **PASS** | TZ vs UTC | C2 #1 | `apps/ccli/core/event/event_store.cpp:109-129` · `apps/ccli/services/ccli_main.cpp:267` |
| **O.14 access** | Username/password (or equiv.) | Auth | Admin harden | **PART** | — | C2 | `apps/ccli/core/event/event_store.hpp:57` · `apps/ccli/services/ccli_main.cpp:573-575` |
| **O.14 remote** | Syslog RFC 5424 (T adds TLS) | UDP client r35 | **P7-03** | **PART** | TLS syslog **GAP** | C2 + 62351-14 | `apps/ccli/core/event/event_store.cpp:124-129` · `apps/ccli/core/event/event_store.cpp:273-288` |
| **O.14 cat PSU/FW** | Power presence, on/off cause, FW update + fail | Catalog | **P7-04** | **PART** | PSU HW GAP | C2 #1 | `apps/ccli/core/event/o14_catalog.hpp:8-10` · `apps/ccli/core/event/event_store.cpp:144-164` |
| **O.14 cat DG/DI** | General / interface switch status | Catalog | **P7-04** | **PART** | HW GAP | C2 #1 | `apps/ccli/core/event/o14_catalog.hpp:11` · `apps/ccli/core/event/o14_catalog.hpp:44-46` |
| **O.14 cat comms** | DSO / operator / plant link + data-link | Catalog | **P7-04**, P2-06 | **PART** | P7-04: DSO/104/Modbus **HAVE** r35 | C2 #1 | `apps/ccli/core/event/o14_catalog.hpp:12-15` · `apps/ccli/core/event/o14_catalog.hpp:47-74` |
| **O.14 cat CCI/meter** | CCI function status; measurement-device status | `pf2` / `meter` | **P7-04** | **PASS** (SW r35) | — | C2 #1 | `apps/ccli/core/event/o14_catalog.hpp:16-17` · `apps/ccli/services/svc_pf2.cpp:29-40` · `apps/ccli/services/ccli_main.cpp:833-845` |
| **O.14 cat irregular net** | Irregular connect; abnormal endpoints / off-hours | `iface` / `net` | **P7-04** | **PART** | No time-of-day engine | C2 #1 | `apps/ccli/core/event/o14_catalog.hpp:18` · `apps/ccli/services/ccli_main.cpp:684-691` · `apps/ccli/services/ccli_main.cpp:1065-1071` |
| **O.14 cat IDS** | Port-scan / malformed / authenticity errors | `mms` auth_reject | **P7-04** | **PART** | No packet IDS | C2 #1 | `apps/ccli/core/event/o14_catalog.hpp:48-50` |
| **O.14 cat auth** | Auth attempts, credential change, log reset, bootlock | Catalog | **P7-04**, **U-CYB-1003-07** | **PART** | Bootlock **N/A**; auth HAVE r35 | C2 | `apps/ccli/core/event/o14_catalog.hpp:19` · `apps/ccli/core/event/o14_catalog.hpp:48-50` |
| **O.14 cat control** | Start/stop functions, 110 % Vn, set-points | Catalog | **P5-R09**, **P7-04** | **PART** | — | C2 #1 | `apps/ccli/core/event/o14_catalog.hpp:20` · `apps/ccli/services/svc_pf2.cpp:34-38` |
| **O.14 cat DSO cmds** | Commands + parameters | Catalog | **P4-05**, **U-T1-07/08** | **PART** | — | C2 #1 | `apps/ccli/core/event/o14_catalog.hpp:21` · `apps/ccli/core/event/o14_catalog.hpp:52-57` · `apps/ccli/services/ccli_main.cpp:898-902` |
| **O.14 cat PG/PI** | General / interface protection trip | Catalog | **P7-04** | **GAP** | HW | C2 #1 | — |
| **O.14 cat M** | Defence-plan teletrip | Catalog | **P6-05** | **PASS** (code) | Live trip evidence | C2 #1 | `apps/ccli/core/event/o14_catalog.hpp:22` · `apps/ccli/core/event/o14_catalog.hpp:85-87` |
| **O.14 cat O/F** | Unit over/under-frequency regulation trip | Catalog | **P7-04** | **GAP** | Unit-level | C2 | — |
| **O.14 cat priority** | Change to O.11 settings | `priority` / fallback | **P7-04** | **PART** | HAVE r35 autonomous-after-loss; live priority edit GAP | C2 | `apps/ccli/core/event/o14_catalog.hpp:23` · `apps/ccli/services/ccli_main.cpp:863` |
| **O.14 cat polygon** | New/changed electrical parameters | `dso` / `polygon_seed` | **P7-04** | **PART** | Boot seed; live curve edit GAP | C2 | `apps/ccli/core/event/o14_catalog.hpp:24` · `apps/ccli/services/ccli_main.cpp:713` |

## A.8 O.15 / §12 — the eight certificates

| # | Clause / standard | Shall | Test | 5-LN DUT | Gap | Cert | Lines |
|---|-------------------|-------|------|----------|-----|------|-------|
| **1** | Functional (O.6 + 61850 + logger + self-diag) | All **applicable** PF | **U-P7-04**, TÜV-class O/T | **PART** | 5-LN ≠ full PF2 | **#1** | `apps/ccli/core/event/o14_catalog.hpp:7-26` · `apps/ccli/services/ccli_main.cpp:833-903` |
| **2** | **CEI EN 61557-12** + 61010; ≥K55; **ISO/IEC 17025** | EMC / insulation / metrology | **P7-09**, **U-ACC-04** | **ACC** | **OPEN** | **#2** | — |
| **3** | **FIPS 140-2 Level 3** independent | Crypto module **or no-claim** | **P7-08** | — | **PART** — decide | **#3** | — |
| **4** | **ISASecure EDSA v3.0.0** per **62443-4-1** + **62443-4-2** | SDLC + component CR | **P7-05/06**, **U-ACC-05** | **ACC** | **PART** pack; Higeco sample was **CSA SL-C 1** — CCLI target **SL-C 2** | **#4** | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md:1-8` · `knowledge-base/08-engineering/CCI_62443-4-2_Extract.md:1-8` |
| **5** | **IEC 61850** UCA-accredited lab | 61850-10 Ed 2.1 Server | **U-ACC-01** | **ACC** | Pre-test PART | **#5** | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:1-59` · `lab/conformance/PRODUCT_SPECIMEN_5LN.md:15-24` |
| **6** | **62351-3** via **62351-100-3** accredited | Transport Tables 1–5 | **U-ACC-02**, **U-D7-03** | **ACC** | **P7-07 OPEN** | **#6 M** | `scripts/verify_annex_pki.py:196-420` · `apps/ccli/core/config/product_cyber_gate.hpp:8-13` |
| **7** | **ISO 9001:2000 (as amended)** manufacturing | Factory QMS | Manufacturer audit | — | Outside repo | **#7** | — |
| **8** | **CE** + DoC DPR 445/2000 Art. 47; retain ≥**20 y** | Technical file | **U-ACC-06** | — | **OPEN** | **#8** | — |

---

# B. Annex M — defence plan (normative)

**Source:** `CCI_Annex_M_Extract.md` · PDF pp. 317–321  
**Edition:** CEI 0-16 consolidata **2025-12** Allegato M.  
**Corpus:** **HAVE** (English). Applies **≥ 100 kW** until a later variant.

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **M.1** | One GSM/LTE receiver → **all** SPI telescatto; or DSO+Terna alt. | Path or waiver | **P6-01** | — | **PART** lab mock | C2 #1/#8 | `apps/ccli/services/svc_io.c:16-36` |
| **M.2** | Modem, optional PI interface board, DSO SIM, aux = SPI/DDI hold-in | Not MMS | Site HW | **N/A** | Product modem **TBD** | C2 site | — |
| **M.3 / M.3.1** | DI 24–48 / 115–230 class; DO dry **24–265 V~ / 1 A** | Do not claim TR400 G6K = M | **P6-07** | — | **PART** | C2 HW | — |
| **M.4** | AT/SMS programming, CLI whitelist | Modem config | Site | **N/A** | — | C2 | — |
| **M.5.1 DO** | DO → PI “scatto da segnale esterno” | Wiring | **P6-02** | — | **DEFERRED** | C2 | — |
| **M.5.1 DI** | DI = DDI aux **or** current relay **or** PI aux | Wiring | **P6-02** | — | **DEFERRED** | C2 | — |
| **M ∩ O.11** | CCI inhibits conflicting PF2 | DIO3 monitor | **P6-03** | **PASS** (r26) | Voltage class **P6-06 GAP** | C2 #1 | `apps/ccli/program/outputs.c:170-172` · `apps/ccli/services/svc_io.c:16-36` |
| **M ∩ O.12** | Teledistacco on install drawing | Drawing | **P6-04** | — | **PART** | C2 | — |
| **M ∩ O.14** | Log M trip | Event category | **P6-05** | **PASS** (code) | Live evidence | C2 #1 | `apps/ccli/core/event/o14_catalog.hpp:85-87` |
| **REQ-IO-004** | Product 5× DI 10–120 V, 3× DO — **AiLux class, not CEI** | Expansion | **P6-06** | **GAP** | Not Annex M pin count | C2 product | — |

**Accuracy:** GPIO / `Wlim` is **not** Annex M.

---

# C. Annex T — IEC 61850 / cyber (normative)

**Source:** `CCI_Annex_T_Extract.md` · `cei-tr-57-126`  
**Edition:** CEI 0-16 consolidata **2025-12** Allegato T (prior EN **2023-05 V2**).  
**Corpus:** **HAVE**. Plant-internal comms **out of T scope**.

## C.1 Scope, timing, measurements

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **T.1 / T.3** | 61850 **mandatory** toward DSO; one access point | MMS Eth_A only | **U-T1-01** (not historic P3-01 :102) | **PART** | Product **REFUSE** :102; 2026-09-25 P3-01 was **:102** — do not cite as TLS evidence | C1 #5 | `apps/ccli/core/config/product_cyber_gate.hpp:8-13` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:884-906` |
| **T.3 + TR 57-126** | SCL example for CCI model | 5-LN trim | **U-T1-02**, `validate-5ln-cid.ps1` | **PART** | Declare 5-LN vs TR 31-LN in PICS Cover | C1 #5 | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:1-7` · `scripts/validate-5ln-cid.ps1` |
| **T.3.2.1 measures** | Periodic **4 s**, Type 3 (~500 ms) | URCB IntgPd=4000 | **U-T1-05/06**, **P3-03** | **PASS** | Type 3 E2E transit not measured | C1 #5 | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:58-59` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:853-855` |
| **T.3.2.1 static** | Plant characteristics on request Type 3 | Nameplate | Browse | **PART** | — | C1 #5 | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:50-57` |
| **T.3.2.1 status** | Operating status on request + on change | Model | Browse / report | **PART** | — | C1 #5 | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:63-79` |
| **T.3.2.1 GOOSE** | CCI need not **publish**; shall **subscribe** Type 1 if used | yaml goose off on 5-LN | **P5-G01** | **N/A** | Do not claim GOOSE on C1 | — | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:5` |
| **T.3.1.3 TotW** | PdC P **M** | `PdCMMXU1.TotW` | **U-T1-04**, **P5-02** | **PASS** | — | C1 #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:674-676` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1307-1321` · `apps/ccli/services/ccli_main.cpp:846` |
| **T.3.1.3 TotVAr** | PdC Q **M** | `TotVAr` | **P5-M07** | **PASS** | Must stay on 5-LN wire | C1 #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:677-679` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1323-1336` · `apps/ccli/services/ccli_main.cpp:847` |
| **T.3.1.3 PPV** | PdC voltages **M** | `PPV` | **P5-M08** | **PASS** (yaml) | Meter V register **P5-06 GAP** | C1 #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:680-682` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1339-1349` · `apps/ccli/services/ccli_main.cpp:848-850` |
| **T.3.1.3 A** | PdC phase currents **O** | Not in 5-LN | — | **N/A** | Do not claim | — | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:56` |
| **T.3.1.3 Gen\* TotW** | Aggregate P per source **M** (GenPV/Wi/Ter/Idr) | Trimmed | — | **N/A** | O V5: not M for wind/PV **100–500 kW** aux-only | C2 if claimed | — |
| **T.3.1.3 St TotW** | Storage aggregate P **M** | Trimmed | — | **N/A** | 5-LN POC-only | C2 if storage | — |
| **T.3.1.3 SGG TotW** | Per generating-group P **M** | Trimmed | — | **N/A** | Same | C2 | — |
| **T Table 80** | Nameplate WRtg / VArRtg / VARtg (Smax) at PdC | yaml `plant.q_*` | **P5-M09** | **PART** | No DPCC LNs on 5-LN | C2 #1 | `apps/ccli/core/config/ccli_config.cpp:131-134` · `apps/ccli/core/dso/plant_envelope.hpp:41-52` |

## C.2 T control tables (not on 5-LN CID)

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **T Table 88** | VArSd | `VArSdDVAR1` | **P5-R01** | **N/A** | First UCA must not claim | C2 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:538` · `apps/ccli/core/dso/dso_phase1.cpp:147` |
| **T Table 89** | VArSa MSD | — | **P5-R08** | **N/A** | Discretionary | — | — |
| **T Table 90** | PFSP | `PFSPDFPF1` | **P5-R02** | **N/A** | — | C2 | `apps/ccli/services/ccli_main.cpp:909-919` · `apps/ccli/core/dso/dso_phase1.cpp:166` |
| **T Table 91** | VArV | `VArVDVVR1` | **P5-R03** | **N/A** | WAIVED | C2 | — |
| **T Table 92** | PFW | `PFWDPFW1` | **P5-R04** | **N/A** | WAIVED | C2 | — |

## C.3 T ACSI / RBAC

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **T.3.3.2** | Associate, directory, GetDataValues, static DS, URCB, DOes | libiec61850 | **U-T1-01…08** | **PART** | Match PICS **M** only | C1 #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:859-879` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:884-906` |
| **T.3.3.4.3** | Roles **DSO_OPERATOR** (−1) / **AGGREGATOR_OPERATOR** (−2) + 7 standard 62351-8 roles | 62351-8 | **U-T3-02/03**, **P3-08** | **PASS** (lab) | IECUserRoles product follow-up; **not** O.15 #6 (100-3) | **T** | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:249-322` · `scripts/verify_annex_pki.py:381-411` |
| **T token A/B** | Profile A (X.509 PKI ext) or B (attribute cert) **M**; C/D optional | Cert A/B | PKI | **PART** | Profile B &lt;24 h not used | T | `scripts/verify_annex_pki.py:245-256` · `apps/ccli/core/config/ccli_config.hpp:123-132` |

## C.4 T.3.3.4 cybersecurity profile

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **T.3.3.4.5 time** | UTC ≤±100 ms | chrony | **U-P3-11** | **PASS** | — | C1 #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1355-1475` |
| **T.3.3.4.5 skew** | Peer skew &gt; **10 min** fails association | TimeQuality | Lab inject | **GAP** | No U-row | T | — |
| **T.3.3.4.5 NTP** | NTP client UDP 123 | chrony | **P3-11** | **PART** | — | T | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1398-1403` |
| **T.3.3.4.5 NTS** | **NTS RFC 8915 mandatory** | — | — | **GAP** | Not implemented | T | — |
| **T.3.3.4.5 redundant NTP** | 2n+1 servers | — | — | **GAP** | — | T | — |
| **T.3.3.4 TLS port** | **3782** | yaml | **U-D7-01** | **PART** | — | #6 | `apps/ccli/core/config/product_cyber_gate.hpp:8-13` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:884-886` |
| **T.3.3.4 TLS ver** | ≥ **1.2** | D7 1.2 **m**; 1.3 **x** | **U-CYB-1003-01** | **PART** | — | #6 | `scripts/verify_annex_pki.py:32-34` · `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **T.3.3.4 ciphers** | AES-128 CBC/GCM + ECDHE-ECDSA GCM list | D7 Table 1 | **U-CYB-1003-04** | **PART** | Lab PEMs RSA; ECDSA **o** | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **T.3.3.4 NULL** | TLS_RSA_WITH_NULL_SHA256 required but **disabled by default** | D7 **x** | — | **N/A** | Explicit enable procedure if ever claimed | T | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **T.3.3.4 reneg** | Align CRL; never &lt; 10 min | D7 PART | **U-CYB-1003-03** | **PART** | PIXIT not agreed | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **T.3.3.4 resume** | At least every 2 h; shorter than reneg | D7 PART | **U-CYB-1003-03** | **PART** | — | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **T.3.3.4 CAs** | ≥ **5** root CAs | Lab: **1** EJBCA | D7 | **PART** | T vs lab | T | `scripts/verify_annex_pki.py:225-236` |
| **T.3.3.4 cert size** | Handle ≥ **8192** octets | D7 **m** | **U-D7-02** | **PART** | — | #6 | `scripts/verify_annex_pki.py:32` · `scripts/verify_annex_pki.py:297-303` |
| **T.3.3.4 CRL** | Update ≤ **24 h** | `tls_crl` | **U-T3-04** | **PART** | — | #6 | `scripts/verify_annex_pki.py:196-420` |
| **T.3.3.4 OCSP cache** | ≤ 24 h | D7 **OCSP x** | **U-CYB-1003-06** | **N/A** | T wants OCSP | T | — |
| **T.3.3.4.1 RSA** | ≥ **2048** (rec ≥3072) | Lab 2048 | **U-D7-02** | **PART** | — | #6 | `scripts/verify_annex_pki.py:33-34` · `scripts/verify_annex_pki.py:78-84` · `scripts/verify_annex_pki.py:258-262` |
| **T.3.3.4.1 hash** | SHA-256 | D7 **m** | **U-D7-02** | **PART** | — | #6 | `scripts/verify_annex_pki.py:34` · `scripts/verify_annex_pki.py:86-90` |
| **T.3.3.4.1 E2E** | E2E **mandatory** per 62351-4 | A-profile first | **U-T3-05** | **PART** | Native §13 **N/A** | T | `apps/ccli/adapters/iec61850_mms/mms_aare_auth.cpp:31` · `scripts/verify_annex_pki.py:245-256` |
| **T.3.3.4.9 enrol** | SCEP **or** EST | Deferred | — | **GAP** | SCEP/EST not first PID | T | — |
| **T.3.3.4.9 validity** | CRL **and** OCSP; OCSP **nonce** | CRL only | **U-T3-04** | **PART** | OCSP **x** | T | `scripts/verify_annex_pki.py:196-420` |
| **T.3.3.4.9 anchors** | DSO CA + admin CA + manufacturer CA min | Lab one CA | — | **PART** | — | T | `scripts/verify_annex_pki.py:225-242` |
| **T.3.3.4.8 boot** | Phased secure boot | Product | — | **GAP** | — | C2 #3/#4 | — |
| **T.3.3.4.8 FW** | Auth user; signed image; deactivate; **logger never erased** | Update path | — | **GAP** | — | C2 | `apps/ccli/core/event/event_store.cpp:144-164` |
| **T.3.3.4.6 store** | FIFO ≥2048; not deletable | EventStore | **P7-01** | **PART** | — | C2 | `apps/ccli/core/event/event_ring.hpp:19` · `apps/ccli/core/event/event_store.hpp:57` |
| **T.3.3.4.6 syslog** | RFC 5424/5425 **over TLS** | UDP client | **P7-03** | **PART** | TLS syslog **GAP** | T | `apps/ccli/core/event/event_store.cpp:273-288` |
| **T.3.3.4.6 events** | Login, hash fail, TLS/cert, MMS SecPDU, CertAVL, RBAC | Catalog | **U-CYB-1003-07**, **U-CYB-1004-05** | **PART** | — | T · #6 | `apps/ccli/core/event/o14_catalog.hpp:47-50` · `apps/ccli/core/event/event_store.cpp:124-129` |
| **T.3.3.4.7** | Asset inventory via LPHD.PhyNam | CID LPHD1 | **U-T1-03** | **PART** | — | T | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:90` |
| **T.3.3.4.10/.11** | NAT/VLAN/FW/VPN; local auth | Zones + harden | **P2-03**, **U-CYB-PROD-01** | **PART** | VPN product OPEN | C2 #4 | `apps/ccli/core/policy/net_policy.hpp:21-43` · `apps/ccli/core/config/product_cyber_gate.hpp:8-21` |

---

# D. Annex U — Operating Rule (normative, extract MISSING)

**Corpus:** `cei-0-16-allegato-u` — **MISSING**. V5 touches Allegato U.

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **U / Operating Rule** | Fallback delay, autonomous mode, which PF2 ON, polygon numbers | yaml + DSO letter | **P3-05**, **P7-10** | **PART** | **Extract MISSING** — lab used **15 s** | C2 legal | `apps/ccli/core/config/ccli_config.cpp:320-321` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1523-1584` |
| **U bis** | Intentional island generators | — | — | **N/A** | Out of first CCI cert unless claimed | — | — |

---

# E. IEC 61850-10 / UCA — O.15 #5

**Sources:** `CCI_61850-10_Extract.md` · `CCI_UCA_61850_Accreditation_Extract.md`  
**Edition:** **IEC 61850-10:2012+AMD1:2025** Ed. **2.1** (2025-07).  
**Claim:** server-only 8-1; CB-01, 02, 05, 12c, (13).  
**MISSING:** official UCA Server Test Procedures Ed. 2.1; PIXIT Word; TICS dated.

## E.1 Documents (61850-10 §5.4)

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **D1 PICS** | Cover + ACSI **M** = 5-LN | `PICS_FILL.md` | **U-DOC-01** | — | Do not submit 31-LN | #5 | `lab/conformance/PICS_FILL.md` · `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:1-7` |
| **D2 PIXIT** | Verdictable values (3782, IntgPd 4000) | `PIXIT_DRAFT.md` | **U-DOC-02** | — | Word template **MISSING** | #5 | `lab/conformance/PIXIT_DRAFT.md` |
| **D3 MICS** | Private LN/DO only; = CID | `MICS_DRAFT.md` | **U-DOC-03** | — | — | #5 | `lab/conformance/MICS_DRAFT.md` |
| **D4 TICS** | Mandatory Green TISSUEs | `TICS_DRAFT.md` | **U-DOC-04** | — | **HOLD** | #5 | `lab/conformance/TICS_DRAFT.md` |
| **D5 SCL** | CID schema valid; Services ≡ PICS; `nameStructure=IEDName` | `product_tr400_eth_a_5ln.cid` | **U-DOC-05** | — | SCL Verify **0 Error** | #5 | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:1-59` · `scripts/validate-5ln-cid.ps1` |
| **D6** | Lab config / firmware ID | `ccli --version` | **U-P0-08** | — | Pin SHA | #5 | `apps/ccli/services/ccli_main.cpp` (`--version`) |
| **SICS** | If lab asks (Ed2) | — | — | — | **MISSING** | #5 | — |
| **Pcap per case** | One trace per claimed UCA case | TSP bundles | **U-CLO-01** | **PART** | TSP IDs ≠ UCA TP IDs | #5 | — |
| **U-ACC-01** | Formal 61850-10 programme | — | Accredited | **ACC** | **OPEN** | **#5** | — |

## E.2 Conformance blocks (fee sheet / certificate page-2)

| Clause | Shall / claim | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|---------------|----|------|----------|-----|------|-------|
| **CB-01** | Associate, directory, GetDataValues **M** | MMS server | **U-T1-01/03/04/10** | **PART** | Formal = lab | #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:884-906` |
| **CB-01b** | Associate IPv6 | — | — | **N/A** | Do not claim | — | — |
| **CB-02** | Static DataSet Get **M** | `DS_R_PdC_Mis4sec` | **U-T1-05** | **PART** | — | #5 | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:52-57` |
| **CB-02+** | DataSet create/delete | `ConfDataSet modify=false` | — | **N/A** | — | — | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:34` |
| **CB-03** | Substitution | — | — | **N/A** | — | — | — |
| **CB-04 / 04+** | Setting groups | — | — | **N/A** | — | — | — |
| **CB-05** | URCB **M** (GI, dchg/qchg/dupd, IntgPd) | URCB | **U-T1-05**, **05b/c/d**, **06** | **PART** | qchg may N/A | #5 | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:58-59` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:853-855` |
| **CB-06 / 06+** | BRCB | May exist in code | **U-N/A-01** | **N/A** | Do not claim | — | — |
| **CB-07** | Logging (61850 Log) | — | — | **N/A** | O.14 ≠ CB-07 | — | `apps/ccli/core/event/event_ring.hpp:19` |
| **CB-08** | GSSE | — | — | **N/A** | Deprecated | — | — |
| **CB-09a–c** | GOOSE | Off on 5-LN | **U-N/A-01** | **N/A** | — | — | — |
| **CB-10 / 11** | 9-1 / SV | — | — | **N/A** | — | — | — |
| **CB-12a** | Control DOns | — | — | **N/A** | — | — | — |
| **CB-12b** | Control SBOns | — | — | **N/A** | — | — | — |
| **CB-12c** | Control **DOes** **M** | Wlim / WSd | **U-T1-07/08** | **PART** | — | #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:403-501` |
| **CB-12d** | Control SBOes | — | — | **N/A** | — | — | — |
| **CB-13** | Time SNTP **O** prefer **M** | chrony | **U-P3-11** | **PART** | Not PTP | #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1355-1475` |
| **CB-13b** | Time PTP 9-3 | — | — | **N/A** | — | — | — |
| **CB-14** | File transfer | Do not claim | **U-N/A-01** | **N/A** | CID leftovers | — | — |
| **CB-15** | Service tracking LTRK | — | — | **N/A** | — | — | — |
| **ICT/SCT 21–38** | Config tools | We are IED | — | **N/A** | — | — | — |

## E.3 61850-10 ACSI groups (claimed only)

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **sAss** Tables 4–5, 44–45 | Associate / release; negatives | MMS | **U-T1-01/10** | **PART** | Formal sAss at lab | #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:884-906` |
| **sSrv** Tables 6–7 | Browse, Get/Set; wrong LN reject | MMS | **U-T1-03/04** | **PART** | Get/Set negatives at lab | #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:859-879` |
| **sDs** Tables 8–9 | Static dataset | `DS_R_PdC_Mis4sec` | **U-T1-05** | **PART** | — | #5 | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:52-57` |
| **sRp** Tables 14–15, 49–50 | URCB: GI, IntgPd, dchg/qchg/dupd, ConfRev, segmentation | URCB | **U-T1-05/05b–d/06** | **PART** | Full Table 14 optional fields per PIXIT | #5 | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:58-59` |
| **sCtl** Tables 26–29, 56–60 | Claimed **DOes** only | Wlim/WSd | **U-T1-07/08** | **PART** | Full Table 26 at lab | #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:403-501` |
| **sTm** Tables 31–32 | Time client | chrony | **U-P3-11** | **PART** | — | #5 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1355-1475` |
| **sTrk / sSub / sSg / sBr / sLog / sGop / sFt / sSv** | — | — | **U-N/A-01** | **N/A** | Must match PICS **x** | — | — |
| **61850-10 §6.3** | Cyber → **100-3 / 100-4 / 100-6** | — | See §F | — | 10 does **not** replace #6 | #6 | `knowledge-base/08-engineering/CCI_61850-10_Extract.md:17` |

---

# F. IEC 62351-100-3 / 62351-3 — O.15 #6 M

**Sources:** `CCI_62351-100-3_Extract.md` · `D7_62351-3_PID_DRAFT.md`  
**Edition claimed:** **IEC 62351-3:2014+AMD1:2018+AMD2:2020** · **IEC TS 62351-100-3:2020**. TLS **1.2**. TLS 1.3 **x**.  
**Corpus:** extract **HAVE**; Table 2/3 **case IDs OCR-garbled** — lab uses licensed PDF.

## F.0 Process (§4)

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **100-3 §4.2** | Clause 5 then 6, order preserved | PID | Lab plan | — | **P7-07 OPEN** | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **100-3 §4.2.1** | M / PICS / PIXIT flags | D7 | **U-DOC-06** | — | Lab form TODO | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **100-3 §4.2.2** | Per client **and** server unless waived | **Server only** | **U-CYB-1003-08** | **N/A** client | Write server in D7 | #6 | `apps/ccli/core/config/product_cyber_gate.hpp:8-13` |
| **100-3 §4.3.1** | Real application = **61850-8-1 MMS** | MMS on TLS | **U-CYB-1003-02** | **PART** | Not raw TLS | #6 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:884-906` |
| **100-3 §4.3.2** | PID = PICS+PIXIT + manuals | D7 | **U-DOC-06** | — | — | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **100-3 §4.3.3** | Version = PID; no empty Table 4/5; report | Freeze version | **U-D7-03**, **U-ACC-02** | **ACC** | **OPEN** | **#6 M** | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **100-3 §4.3.4** | Log handshake, reneg, resume, cert, cipher, events | EventStore | **U-CYB-1003-07** | **PART** | — | #6 | `apps/ccli/core/event/o14_catalog.hpp:47-50` |

## F.1 Table 1 / 62351-3 configuration

| Clause | Shall | SW / D7 | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|---------|------|----------|-----|------|-------|
| **62351-3 §5.2** | TLS **1.2** min | 1.2 **m**; 1.0/1.1 **x**; 1.3 **x** | **U-CYB-1003-01** | **PART** | — | #6 | `apps/ccli/core/config/product_cyber_gate.hpp:8-13` · `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **§5.1 / §5.8 ciphers** | Suites ∈ PICS; no NULL/weak | D7 Table 1 **c** | **U-CYB-1003-04** | **PART** | Frozen IANA list not captured | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **§5.6 mutual** | Client + server certs | Mutual | **U-T1-01** | **PART** | — | #6 | `scripts/verify_annex_pki.py:258-286` |
| **§5.6.4 crypto** | RSA ≥2048, SHA-256, cert ≤8192 | `verify_annex_pki.py` | **U-D7-02** | **PART** | ECDSA **o** | #6 | `scripts/verify_annex_pki.py:32-34` · `scripts/verify_annex_pki.py:78-90` · `scripts/verify_annex_pki.py:258-303` |
| **§5.3 resume** | Resumption policy | PART | **U-CYB-1003-03** | **PART** | PIXIT interval | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **§5.4 reneg + RFC 5746** | Reneg; never &lt;10 min | RFC 5746 **m** | **U-CYB-1003-03** | **PART** | — | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **§5.6.4.4 CRL** | Check CRL; unreachable **must not drop** session | `tls_crl` | **U-T3-04** | **PART** | Unreachable soak no U-ID | #6 | `scripts/verify_annex_pki.py:196-420` |
| **OCSP** | T wants it; D7 **x** | — | **U-CYB-1003-06** | **N/A** | Conflict T vs D7 | T | — |
| **§5.7 port** | Secure ≠ clear | **3782** / **102** lab-only | **U-D7-01**, **U-CYB-PROD-01** | **PART** | Product REFUSE :102 | #6 | `apps/ccli/core/config/product_cyber_gate.hpp:8-13` |
| **§4.4 events** | Security events on fail | O.14 | **U-CYB-1003-07** | **PART** | — | #6 | `apps/ccli/core/event/o14_catalog.hpp:47-50` |
| **§8 PICS Table 4 CA** | Multiple CA **o**; T ≥5 | Lab 1 root | D7 | **PART** | — | T vs D7 | `scripts/verify_annex_pki.py:225-242` |

## F.2 Table 2 normal

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **T2 handshake** | Valid peer → success | TLS :3782 | **U-CYB-1003-01**, **U-T1-01** | **PART** | TSP ≠ formal ID | #6 pre | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:884-906` · `scripts/verify_annex_pki.py:258-286` |
| **T2 application** | MMS after TLS | Associate + browse | **U-CYB-1003-02**, **U-T1-03** | **PART** | Same session | #6 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:859-879` |
| **T2 resume** | Within PIXIT | Stack | **U-CYB-1003-03** | **PART** | — | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **T2 reneg** | Within PIXIT | RFC 5746 | **U-CYB-1003-03** | **PART** | — | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **T2 cipher** | Suite ∈ PICS | wolfSSL | **U-CYB-1003-04** | **PART** | Dump submit FW | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |

## F.3 Table 3 resiliency

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **T3 no client cert** | Refuse + event | Mutual | **U-T3-01** | **PART** | — | #6 pre | `scripts/verify_annex_pki.py:258-286` |
| **T3 revoked + CRL** | Refuse + event | CRL | **U-T3-04** | **PART** | — | #6 pre | `scripts/verify_annex_pki.py:196-420` |
| **T3 expired** | Refuse + alarm | Dates | **U-CYB-1003-05** | **ACC** | — | #6 lab | `scripts/verify_annex_pki.py:196-420` |
| **T3 wrong CA** | Refuse + alarm | Trust | **U-CYB-1003-05** | **ACC** | — | #6 lab | `scripts/verify_annex_pki.py:238-242` |
| **T3 weak key** | Refuse (RSA-1024 **x**) | D7 | **U-CYB-1003-05** | **ACC** | — | #6 lab | `scripts/verify_annex_pki.py:33` · `scripts/verify_annex_pki.py:78-84` |
| **T3 bad cipher** | Fail + event | Offer NULL | Lab | **ACC** | No U-ID | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **T3 downgrade** | Alarm 1.0/1.1 | Min 1.2 | Lab | **ACC** | — | #6 | `apps/ccli/core/config/product_cyber_gate.hpp:8-13` |
| **T3 MITM / alter** | Integrity fail | TLS MAC | Lab | **ACC** | — | #6 | — |
| **T3 resume/reneg violate** | Policy reject | PIXIT | Lab | **ACC** | — | #6 | `lab/conformance/D7_62351-3_PID_DRAFT.md` |

## F.4 Tables 4–5 report

| Clause | Shall | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|------|----------|-----|------|-------|
| **Table 4** | Every Table 1 cell × case = P/F/NA | **U-D7-03** | **ACC** | Empty = incomplete | **#6** | `lab/conformance/D7_62351-3_PID_DRAFT.md` |
| **Table 5** | Every Table 2/3 case = P/F/NA | **U-ACC-02** | **ACC** | **OPEN** | **#6 M** | — |

---

# G. IEC 62351-4 / 100-4 — Annex T (not O.15 #6)

**Sources:** `CCI_62351-4_Extract.md` · `CCI_62351-100-4_Extract.md`  
**Edition:** **IEC 62351-4:2018+AMD1:2020** · **IEC TS 62351-100-4:2023**.  
**First PID claim:** **A-profile §11 + TLS**. Native **§13 N/A**.  
**Corpus:** 4 extract **HAVE**; 100-4 OCR **HAVE**, table IDs **PART**.

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **62351-4 §6.3** | Secure MMS **:3782** | yaml | **U-CYB-1004-08** | **PART** | Same as 100-3 | T | `apps/ccli/core/config/product_cyber_gate.hpp:8-13` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:884-886` |
| **§6.2.8** | Revoked/expired terminate; fetch-fail no auto-terminate | CRL | **U-T3-04** | **PART** | — | T | `scripts/verify_annex_pki.py:196-420` |
| **Table 1 Note 2** | A-profile + TLS = allowed full-security combo | First PID | PID wording | — | Do not mark native M | T | `scripts/verify_annex_pki.py:245-256` · `apps/ccli/adapters/iec61850_mms/mms_aare_auth.cpp:31` |
| **§11.2.2** | Auth = X.509 + GeneralizedTime + sha256WithRSA | Cert B ACSE | **U-T3-05**, **U-CYB-1004-01** | **PART** | P3-07 PASS 2026-10-08 | T | `apps/ccli/adapters/iec61850_mms/mms_aare_auth.cpp:31` · `lab/verify_62351_auth.py:1-20` |
| **§11.2.3–4** | Skew &gt;10 min → P-ABORT | Time | Lab inject | **GAP** | No U-row | T | — |
| **G.2** | TLS EE ≠ application EE | `tls_own_cert` ≠ `tls_acse_cert` | **U-CYB-1004-02** | **PART** | — | T | `scripts/verify_annex_pki.py:245-256` · `apps/ccli/core/config/ccli_config.hpp:123-125` |
| **G.3–G.4** | X.509 v3; ≤8192; RSA≥2048; SHA-256+ | EJBCA | **U-D7-02** | **PART** | UUID serial rec. | T | `scripts/verify_annex_pki.py:32-34` · `scripts/verify_annex_pki.py:225-309` |
| **G.5.2** | Cert B KU: digitalSignature **+** keyAgreement | Cert B | **U-CYB-1004-03** | **PART** | — | T | `scripts/verify_annex_pki.py:311-345` |
| **G.5.3** | CRL **or** OCSP per cert | CRL | **U-T3-04** | **PART** | T wants both | T | `scripts/verify_annex_pki.py:196-420` |
| **G.6.2** | Subject OID = AP-title + AE-qualifier | DN dump | **U-CYB-1004-04** | **PART** | — | T | `scripts/verify_annex_pki.py:133-163` · `scripts/verify_annex_pki.py:347-349` |
| **§13.1–§13.3** | Native Handshake / EncrTransfer / DH-14 | — | **U-CYB-1004-06** | **N/A** | Later FW | — | — |
| **§15.2.2** | Native: **no** AARQ authentication-value | Opposite of §11 | — | **N/A** | Do not mix modes | — | — |
| **62351-8** | Viewer read OK / Operate deny | Viewer cert | **U-T3-02/03** | **PASS** (lab) | Not 100-4 body alone | T | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:249-322` · `scripts/verify_annex_pki.py:381-411` |
| **62351-9** | CRL / enrolment profiles | CRL file | **U-T3-04** | **PART** | EST/SCEP GAP | T | `scripts/verify_annex_pki.py:275-286` |
| **62351-14** | Syslog mapping | UDP | **P7-03** | **PART** | — | C2 | `apps/ccli/core/event/event_store.cpp:124-129` · `apps/ccli/core/event/event_store.cpp:273-288` |
| **Lab Q 7.3** | Does lab run 100-4? | — | **U-CYB-1004-07** | — | Written yes/no | optional | — |
| **U-ACC-03** | 100-4 report if quoted | — | Accredited | **ACC** | Not O §12 | T | — |
| **62351-4 PID** | Separate from D7 | Draft | **U-DOC-09** | — | **OPEN** | quote | — |
| **62351-100-6** | GOOSE/SV security | — | — | **N/A** | GOOSE not claimed | — | — |

---

# H. IEC 62443 — O.15 #4

**Sources:** `CCI_62443_Zones.md` · `CCI_62443-4-1_Extract.md` · `CCI_62443-4-2_Extract.md`  
**Edition:** **62443-3-2:2020** · **62443-4-1:2018** · **62443-4-2:2018**.  
**Target:** SL-C `{2 2 2 2 2 2 2}`. Higeco sample was **SL-C 1** — do not copy.  
**Pairing:** 4-1 SDLA **then** 4-2 CSA.

## H.1 62443-3-2 ZCR (evidence, not CSA)

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **ZCR 1** | SUC + access points | Port table | **P0-01/04** | **PART** | — | #4 ev | `knowledge-base/08-engineering/CCI_62443_Zones.md:1-31` · `apps/ccli/core/config/ccli_config.hpp:184-223` |
| **ZCR 2** | Initial cyber risk | Workshop | — | — | **TBD** | #4 | `knowledge-base/08-engineering/CCI_62443_Zones.md` |
| **ZCR 3** | Zones/conduits Z0–Z5 C1–C5 | Zones.md | **P2-05** | **PART** | — | #4 | `knowledge-base/08-engineering/CCI_62443_Zones.md` · `apps/ccli/core/policy/net_policy.hpp:7-43` |
| **ZCR 3.2** | IT ≠ IACS | No IT→Eth_A | **P2-01** | **PASS** | — | #4 | `apps/ccli/core/policy/net_policy.hpp:21-43` |
| **ZCR 3.4** | Temporary USB ≠ permanent | USB/Z4 | **P2-04** | **PART** | LuCI | #4 | — |
| **ZCR 3.5–3.6** | Wireless ≠ wired DSO/plant | Wi-Fi off; LTE isolated | **P2-03** | **PASS** | — | #4 | `apps/ccli/core/policy/net_policy.hpp:36-40` |
| **ZCR 4–5** | Risk vs tolerable; **SL-T** | SL-T 2 / plant 1 | Review | **PART** | Lab validate SL-T | #4 | `knowledge-base/08-engineering/CCI_62443_Zones.md` |
| **ZCR 6** | CRS drawings + threats | Rev 1.6 + drawio | **P2-05** | **PART** | — | #4 | `knowledge-base/08-engineering/CCI_62443_Zones.md` |
| **ZCR 7** | Asset owner approval | Sign-off | — | — | **MISS** | blocks 3-2 | — |

## H.2 62443-4-1 — 47 SDL clauses (P7-05 / U-ACC-05)

| Clause | Shall | Artefact | Status | Cert | Lines |
|--------|-------|----------|--------|------|-------|
| **SM-1** | CM, traceability, V&V, approvals, lifecycle | git + architecture.db + ISO 9001 | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SM-2** | RACI for SDL roles | Roles matrix | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SM-3** | Which products 4-1 applies to | TG-524 in scope | **PART** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SM-4** | Security training | Records | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SM-5** | Scoped security analysis | Scoping doc | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SM-6** | Integrity of scripts/executables | Signed `.ipk` | **PART** | #4 | `apps/ccli/core/event/event_store.cpp:144-164` |
| **SM-7** | Protect product in dev/prod/delivery | Secure CI | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SM-8** | Protect **code-signing private keys** | K6.3 SOP | **MISS** | #4 P0 | `knowledge-base/08-engineering/CCI_K6.3_Key_Ceremony_SOP.md` |
| **SM-9** | 3rd-party component risk | SBOM | **PART** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SM-10** | Custom 3rd-party meets 4-1 | BSP contract | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SM-11** | No release with open high CVSS | Release gate | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SM-12** | Prove SM-5 done | SDL audit | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SM-13** | Continuous improvement + RCA | Table 2 | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SR-1** | Product security context | Zones + deploy | **PART** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SR-2** | Threat model; review ≥ annually | STRIDE | **PART** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SR-3** | Reqs install/operate/maintain/decommission | Trace to CRs | **PART** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SR-4** | Scope + target SL-C `{2222222}` | Spec | **PART** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SR-5** | Independent testers + customer + security review | Minutes | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SD-1** | Characterize every interface (a–j) | Eth_A/B/plant/USB/LTE/RS485/console | **PART** | #4 | `apps/ccli/core/config/ccli_config.hpp:184-223` · `apps/ccli/core/policy/net_policy.hpp:7-43` |
| **SD-2** | Defence in depth / zone architecture | K3.3 | **PART** | #4 | `apps/ccli/core/policy/net_policy.hpp:21-43` · `knowledge-base/08-engineering/CCI_62443_Zones.md` |
| **SD-3** | Security design review | Body pp. 31–32 | **GAP** — extract body **MISSING** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SD-4** | Secure design best practices | Body pp. 31–32 | **GAP** — extract body **MISSING** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SI-1** | Reviews + SCA on every change | CI static | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SI-2** | Coding standard (banned fn, trust-boundary) | CERT C | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SVV-1** | Functional security tests | Independent **dept** | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SVV-2** | Defeat each TM mitigation | Independent **dept** | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SVV-3** | Fuzz, surface, CVE, SBOM, runtime | Independent **person** | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SVV-4** | Pen-test chained vulns | Independent **org** | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SVV-5** | Prove Table 3 independence | QA ≠ dev mgr | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **DM-1** | Intake testers/users/researchers | Disclosure page | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **DM-2** | Timely investigate | ISO 30111 SOP | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **DM-3** | CVSS + essential-function impact | Scoring | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **DM-4** | Fix / defer / accept | Release gate | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **DM-5** | Disclose CVSS + versions + fix | Policy | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **DM-6** | Review process ≥ annually | Audit | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SUM-1** | Qualify patch; PF2 regression | Checklist | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SUM-2** | Patch docs | Notes | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SUM-3** | OS/component compatibility | Feed matrix | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SUM-4** | Authentic delivery (signed) | Signed image | **PART** | #4 | `apps/ccli/core/event/event_store.cpp:144-164` |
| **SUM-5** | SLA (e.g. 30/60/90 d) | Policy | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SG-1** | Defense-in-depth user guide | Manual §1 | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SG-2** | Environment measures | SR-1 | **PART** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SG-3** | Hardening guidelines | LuCI/UCI | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SG-4** | Secure disposal / wipe / TPM clear | SOP | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SG-5** | Secure operation duties | Manual | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |
| **SG-6** | Accounts + default password change | First-boot | **PART** | #4 | `apps/ccli/core/config/product_cyber_gate.hpp:8-21` |
| **SG-7** | Review all manuals before release | Gate | **GAP** | #4 | `knowledge-base/08-engineering/CCI_62443-4-1_Extract.md` |

## H.3 62443-4-2 — SL-C 2 CRs (P7-06 / CSA)

### FR 1 IAC

| Clause | Shall at SL-C 2 | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-----------------|----|------|----------|-----|------|-------|
| **CR 1.1(1)** | Unique human ID+auth every HMI | SSH/console | Unique accounts | **PART** | MFA = SL 3 | #4 | — |
| **CR 1.2** | Process/device auth to peers | MMS mutual | **U-T1-01**, **P3-07** | **PART** | Unique ID = SL 3 | #4 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:884-906` · `scripts/verify_annex_pki.py:258-286` |
| **CR 1.3** | Account lifecycle | UCI | Create/disable | **PART** | — | #4 | — |
| **CR 1.4** | Unique identifiers | Roles | Schema | **PART** | — | #4 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp` (roles) |
| **CR 1.5** | Init / change defaults / protect authenticators | First-boot purge | Default-pwd test | **PART** | HW protect = SL 3 | #4 | — |
| **NDR 1.6(1)** | Wireless unique auth if used | Wi-Fi off; LTE | Policy | **PART** | — | #4 | `apps/ccli/core/policy/net_policy.hpp:36-40` |
| **CR 1.7** | Configurable password strength | PAM | Policy | **PART** | Lifetime = SL 3 | #4 | — |
| **CR 1.8** | PKI when used | Cert A/B + CA | **U-D7-02** | **MISS** | **K6.5** DSO CA | #4 | `scripts/verify_annex_pki.py:196-420` · `knowledge-base/08-engineering/CCI_K6.3_Key_Ceremony_SOP.md` |
| **CR 1.9** | Sig, chain, revocation, key control, map ID | OpenSSL + CRL | **U-T3-04** | **PART** | OCSP x | #4 | `scripts/verify_annex_pki.py:359-379` |
| **CR 1.10** | Mask feedback | Masked login | UX | **PART** | — | #4 | — |
| **CR 1.11** | Lockout; no lockout on emergency | PAM | Lockout + PF2 lives | **PART** | — | #4 | — |
| **CR 1.12** | Banner 4 elements | SSH banner | Capture | **PART** | — | #4 | — |
| **NDR 1.13** | Untrusted-network access (LTE) | VPN/deny | **P2-03** | **PART** | Approval = SL 3 | #4 | `apps/ccli/core/policy/net_policy.hpp:36-40` |
| **CR 1.14** | Symmetric-key auth if used | Prefer PKI | Policy | **PART** | N/A if unused | #4 | `scripts/verify_annex_pki.py:32-34` |

### FR 2 UC

| Clause | Shall at SL-C 2 | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-----------------|----|------|----------|-----|------|-------|
| **CR 2.1(1)(2)** | AuthZ all users + editable role map | 62351-8 | **U-T3-02/03** | **PART** | Viewer deny PASS lab | #4 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:249-322` |
| **CR 2.2** | Wireless use control | Wi-Fi off | Policy | **PART** | — | #4 | `apps/ccli/core/policy/net_policy.hpp:36-40` |
| **CR 2.3** | — | No component CR | — | **N/A** | System/Z4 | — | — |
| **EDR 2.4(1)** | Mobile code | None on Z0 | Review | **PART** | — | #4 | — |
| **CR 2.5** | Session lock + re-auth | SSH/LuCI | Idle lock | **PART** | Exclude emergency DO | #4 | — |
| **CR 2.6** | Remote session terminate | Admin kill | Kill | **PART** | — | #4 | — |
| **CR 2.7** | Concurrent session limit | — | — | **N/A** | SL 3+ | — | — |
| **CR 2.8** | Auditable events a–f | journald + O.14 | **P7-04** | **PART** | — | #4 | `apps/ccli/core/event/o14_catalog.hpp:7-26` · `apps/ccli/core/event/event_store.cpp` |
| **CR 2.9** | Sized store; no crash when full | Wrap | **P7-01** | **PART** | Warn = SL 3 | #4 | `apps/ccli/core/event/event_ring.hpp:19` · `apps/ccli/core/event/event_store.cpp:189-308` |
| **CR 2.10** | Audit fail ≠ stop essential fn | PF2 continues | Fail-log | **PART** | — | #4 | `apps/ccli/core/pf2/pf2_fsm.cpp:44-48` |
| **CR 2.11(1)** | Timestamps **synced** | chrony | **U-P3-11** | **PART** | Alter detect = SL 4 | #4 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1355-1475` |
| **CR 2.12** | Tie human action to identity | User on Operate | **P4-05** | **PART** | All types = SL 4 | #4 | `apps/ccli/services/ccli_main.cpp:898-902` |
| **EDR 2.13** | Physical diagnostic ports | JTAG/USB | HW policy | **PART** | — | #4 | — |

### FR 3 SI

| Clause | Shall at SL-C 2 | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-----------------|----|------|----------|-----|------|-------|
| **CR 3.1(1)** | Integrity + authenticity of origin | TLS | **U-T1-01** | **PART** | Plant Modbus CRC only | #4 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:884-906` |
| **CR 3.2** | Malware | EDR/SAR | — | **DEFER** | Clauses 12–15 | #4 | — |
| **CR 3.3** | Verify security functions | FAT checklist | Boot/deny/lockout | **PART** | During-ops = SL 4 | #4 | `apps/ccli/core/config/product_cyber_gate.hpp:8-21` |
| **CR 3.4(1)** | SW/config authenticity | Secure Boot + signed ipk | Image verify | **PART** | Notify = SL 3 | #4 | `apps/ccli/core/event/event_store.cpp:144-164` |
| **CR 3.5** | Input validation | Parsers | Malformed SP | **PART** | — | #4 | `apps/ccli/core/dso/setpoint_gate.hpp:22-56` |
| **CR 3.6** | Process outputs → predetermined state (Unpowered/Hold/Fixed/Dynamic) | Safe DO + stale-safe | **P1-06**, **P1-04** | **PASS** (unit boot) | Attack-mode write **PART** | #4 | `apps/ccli/core/pf2/pf2_fsm.cpp:24-48` · `apps/ccli/program/outputs.c:170-172` |
| **CR 3.7** | Errors not exploitable | Generic errors | Capture | **PART** | — | #4 | — |
| **CR 3.8** | Session integrity | TLS/MMS | **U-T1-01/10** | **PART** | — | #4 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:884-906` |
| **CR 3.9** | Protect audit logs | No overwrite | **P7-01** | **PART** | — | #4 | `apps/ccli/core/event/event_store.hpp:57` · `apps/ccli/services/ccli_main.cpp:573-575` |
| **EDR 3.10–3.14** | RoT / boot / OTA | Secure Boot | Update | **PART** | T.3.3.4.8 | #4 | `apps/ccli/core/event/event_store.cpp:144-164` |

### FR 4–7

| Clause | Shall at SL-C 2 | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-----------------|----|------|----------|-----|------|-------|
| **CR 4.1** | Confidentiality rest + transit | TLS | **P3-06**, **P4-03** | **PART** | At-rest keys | #4 | `apps/ccli/core/config/product_cyber_gate.hpp:8-21` |
| **CR 4.2** | Purge on decommission | Reset + TPM clear | SOP | **PART** | SG-4 | #4 | — |
| **CR 4.3** | Crypto policy | RSA-2048 / SHA-256 | **U-D7-02** | **PART** | — | #4 | `scripts/verify_annex_pki.py:32-34` |
| **CR 5.1** | Network segmentation | 4 domains | **P2-01/02** | **PASS** | — | #4 | `apps/ccli/core/policy/net_policy.hpp:7-43` |
| **NDR 5.2(1)** | Deny-by-default | nftables | **P2-02/03** | **PASS** | — | #4 | `apps/ccli/core/policy/net_policy.hpp:21-43` |
| **NDR 5.3** | Block unused IT on Z0 | FW | Port audit | **PART** | — | #4 | `apps/ccli/core/config/product_cyber_gate.hpp:8-13` |
| **CR 5.4** | — | No component CR | — | **N/A** | — | — | — |
| **CR 6.1** | Audit log accessible | `--event-dump` | **P7-02/03** | **PART** | 4-2 p.57 body **MISSING** | #4 | `apps/ccli/core/event/event_store.cpp:336-349` · `apps/ccli/services/ccli_main.cpp:267` |
| **CR 6.2** | Continuous monitoring | Syslog | **P7-03** | **PART** | SIEM wiring | #4 | `apps/ccli/core/event/event_store.cpp:273-288` |
| **CR 7.1(1)** | Essential fn under DoS + flood | Rate limits | Flood | **PART** | PF2 must survive | #4 | `apps/ccli/core/dso/setpoint_gate.hpp:22-56` |
| **CR 7.2** | Cap security-job resources | Limits | Load | **PART** | — | #4 | — |
| **CR 7.3(1)** | Verified backup before update | Backup + OTA | Drill | **PART** | — | #4 | `apps/ccli/core/event/event_store.cpp:144-164` |
| **CR 7.4** | Secure rollback | A/B | Rollback | **PART** | — | #4 | — |
| **CR 7.5** | — | No component CR | HW PSU | **N/A** | O.13.3 | C2 | — |
| **CR 7.6** | Export config | UCI + nft | Export | **PART** | — | #4 | — |
| **CR 7.7** | Least functionality | GOOSE off, no :102 | **U-D7-01**, **P7-12** | **PART** | `--lab-demo` off **OPEN** | #4 | `apps/ccli/core/config/product_cyber_gate.hpp:8-13` · `apps/ccli/services/ccli_main.cpp:522` · `apps/ccli/program/ccli_bridge.cpp:379` |
| **CR 7.8** | SBOM / inventory | `.ipk` list | SBOM | **PART** | K6.7 | #4 | — |

---

# I. IEC / CEI EN 61557-12 + 61010 — O.15 #2

**Source:** `CCI_61557-12_Extract.md`  
**Edition:** CEI EN **61557-12** (extract P1) + **61010-1 / 2-201**; 17025 lab.  
**Note:** O cites “Annex B” fixed blocks; **this edition** puts fixed-block in **Annex D** (Fig D.2). CCI = **EPMF SS** (gateway + external analyzer).

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **REQ-MET-001 / §4.8.2.1** | P **gapless** | Analyzer | Chain calc | **PART** | Spec | #2 | `apps/ccli/adapters/modbus_master/modbus_adapter.cpp:286-350` · `apps/ccli/core/measurement/measurement_store.cpp:5-7` |
| **REQ-MET-002 / §4.8.3.1** | Q **gapless** | Analyzer | Chain | **PART** | — | #2 | `apps/ccli/services/ccli_main.cpp:847` · `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:1323-1336` |
| **REQ-MET-003 / §4.8.7.1** | U gapless **not** required | Analyzer | — | **PART** | — | #2 | `apps/ccli/services/ccli_main.cpp:848-850` |
| **REQ-MET-004 / Annex D** | **Fixed** blocks, not sliding | Aggregator TBD | **P5-03** | **PART** | `core/measurement/` TBD | #2 | `apps/ccli/core/measurement/measurement_store.cpp:5-7` |
| **REQ-MET-005 / Table 3+8** | P class ≤ **0,2** | Converter | **P5-04** | **PART** | Analyzer selection | #2 | — |
| **REQ-MET-006** | CT ≤0,5 · VT ≤0,5 | Sensors | Datasheet | **PART** | Annex F | #2 | — |
| **REQ-MET-007** | MC200 = 200 ms fixed | Fast ring | **P5-03** | **PART** | — | #2 · O.7.4 | `apps/ccli/core/measurement/measurement_store.cpp:5-7` |
| **REQ-MET-008** | PF1 TX 4 s fixed, :00/:04/… | URCB | **U-T1-06** | **PART** | Alignment | #2 · O.8.3 | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid:58-59` |
| **REQ-MET-009** | MCΔT over ΔT 10–600 s | Slow ring | **P5-R06** | **N/A** | Curves waived | #2 | — |
| **REQ-MET-010 / Annex H** | TG-524 = **EPMF SS** | Architecture | Doc | **PART** | — | #2 | — |
| **REQ-MET-011** | P/Q rated 80–120 % Un | Analyzer | Type test | **PART** | — | #2 | — |
| **REQ-MET-012** | U range 20–120 % Un (xS) | Analyzer | Type test | **PART** | — | #2 | — |
| **REQ-MET-013** | Climate **K55** (−5…+55 °C) | HW | **P7-09** | **ACC** | 17025 lab | #2 | — |
| **REQ-MET-014** | **No fiscal** CT/VT reuse | Design | Spec | — | None (spec) | #2 | — |
| **61010-1 / 2-201** | Insulation / safety | HW | **P7-09** | **ACC** | Recalled by 61557-12 | #2 | — |
| **ISO/IEC 17025** | Lab accreditation (ACCREDIA IT) | — | **U-ACC-04** | **ACC** | **OPEN** | **#2** | — |
| **CEI EN 61439-2** | Cabinet (competitor sample D) | Enclosure | If claimed | — | Not CCI function cert | optional | — |

---

# J. Other O.15 and legal

| Clause | Shall | SW | Test | 5-LN DUT | Gap | Cert | Lines |
|--------|-------|----|------|----------|-----|------|-------|
| **FIPS 140-2 L3** | Independent crypto module **or drop claim** | TPM | **P7-08** | — | **PART** | **#3** | — |
| **ISO 9001:2000 (as amended)** | Manufacturing QMS | Factory | Audit | — | Outside repo | **#7** | — |
| **CE marking** | Mandatory | File | **U-ACC-06** | — | **OPEN** | **#8** | — |
| **DPR 445/2000 Art. 47** | Manufacturer DoC; retain ≥20 y | File | Legal | — | **OPEN** | **#8** | — |
| **ARERA 385/2025** | ≥100 kW PF2 + V5 dates | Memo | **P7-10** | — | Full text **MISSING** | #8 | — |
| **ARERA 564 / 36/2020** | Observability / CCI mandate | Memo | **P7-10** | — | **PART** | #8 | — |
| **Grid Code A.72** | V5 100–500 kW error ≤5 % | O.13.2 | **P5-04** | **PART** | — | C2 | `apps/ccli/adapters/modbus_master/modbus_adapter.cpp:581-593` |
| **Grid Code A.13** | Availability | RAMS | O.13.10 | — | **OPEN** | C2 | — |
| **CEI 0-16 §8.8.6.3** | Unit O/F **not** in CCI | Inverter | Spec | — | None | C2 | — |
| **CEI 0-16 §8.8.7.1** | Teledistacco predisposition | O.12 | **P6-04** | — | PART | C2 | — |
| **60870-5-104** | Eth_B operator only | :2404 TLS | **U-P4-01/02**, **P4-01…05** | **PASS** (lab) | **Not** UCA PICS | C2 O.13 | `apps/ccli/adapters/iec60870_104/iec104_adapter.cpp` · `apps/ccli/core/config/product_cyber_gate.hpp:15-19` |
| **62351-5** | Secure 104 | 104 TLS | **P4-03** | **PASS** (lab) | — | C2 | `apps/ccli/core/config/product_cyber_gate.hpp:15-19` · `apps/ccli/adapters/iec60870_104/iec104_adapter.cpp` |
| **P7-11 / K7.5** | DSO demo script | `Wlim` steps | **U-P7-04** | **PART** | — | #1 | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp:403-451` |
| **P7-12** | `--lab-demo` **off** in product | Config audit | **P7-12** | **OPEN** | — | C2 | `apps/ccli/services/ccli_main.cpp:258` · `apps/ccli/services/ccli_main.cpp:522` · `apps/ccli/program/ccli_bridge.cpp:379` · `apps/ccli/program/commands.c:89` |

---

# K. 5-LN MMS objects (cert specimen)

| Annex need | 5-LN LN / mechanism | In first UCA? |
|------------|---------------------|---------------|
| P/Q/V PdC (O.8 / T.3.1.3) | `PdCMMXU1` | **Yes** |
| DSO P limit (O.9.2.2) | `WlimDWMX1` | **Yes** |
| External P SP (O.10.3.1) | `WSdDAGC1` | **Yes** |
| 4 s visibility | `LLN0.urcb_PdC_Mis4sec` | **Yes** |
| Device | `LPHD1` | **Yes** |
| Reactive LNs (O.9.1 / T 88–92) | Not in CID | **No** — C2 later |
| GOOSE / BRCB / files | Off / not claimed | **No** |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| **Annex U** | Cited | Operating Rule extract | **MISSING** |
| **ARERA 385** full | V5 summary | Legal dates | **MISSING** |
| **UCA Server TP Ed 2.1** | TSP + 61850-10 | Official TP + TICS date | **MISSING** |
| **PIXIT Word** | Markdown values | Lab template | **MISSING** |
| **TICS** | Empty draft | Lab TISSUE list | **HOLD** |
| **SICS** | — | If lab asks | **MISSING** |
| **100-3 Table 2/3 IDs** | Themes only | Licensed PDF rows | **PART** |
| **100-4 tables** | OCR HAVE | Batch extract | **PART** |
| **62443-4-1 SD-3/SD-4 body** | Names only | pp. 31–32 | **MISSING** |
| **62443-4-2 CR 6.1 body** | SL mapping | p. 57 | **MISSING** |
| **NTS / OCSP / SCEP / EST** | Chrony + CRL | T as written | **GAP / N/A claimed** |
| **M voltage class** | SW inhibit PASS | M.3.1 + P6-06 | **GAP** |
| **Accredited #2–#6** | Plans | Report IDs | **OPEN** |
| **61850-8-1:2011** | 2004 OCR PART | Ed.2 + AMD1 | **PART** |
| **O.13.1.1.1 PHY** | Copper lab | 100BaseFX 1310 nm dual LC | **GAP** |
| **T.3.1.3 Gen/St/SGG** | Trimmed on 5-LN | Full T model or V5 waiver letter | **N/A** first UCA |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-NCV-01 | Submit 31-LN PICS for 5-LN DUT | Fail Compare / UCA | Pin PRODUCT_SPECIMEN_5LN |
| R-NCV-02 | Treat TSP PASS as #5 | False cert | Only **U-ACC-01** |
| R-NCV-03 | Claim O.9.1 on 5-LN CID | PICS lie | N/A + WAIVE |
| R-NCV-04 | Call DIO1 “Annex M” | Wrong annex | M = modem→SPI |
| R-NCV-05 | Skip 100-3 because TLS works | Fail O §12 #6 | Book **U-ACC-02** |
| R-NCV-06 | Treat 100-4 as #6 | Wrong quote | #6 = 100-3 only |
| R-NCV-07 | Claim FIPS L3 without module | DoC reject | P7-08 claim or drop |
| R-NCV-08 | Copy Higeco SL-C 1 vector | Wrong CSA scope | SL-C **2** |
| R-NCV-09 | Treat 3-2 zones as CSA | Missing 4-1/4-2 | SDLA then CSA |

---

## Verification — one cycle order

| Step | Milestone | Execute | Pass when | Closes |
|------|-----------|---------|-----------|--------|
| 1 | A prep | **U-P0-01…11**, **U-P0b-*** | Single `ccli`, :3782, 5-LN SHA | — |
| 2 | A MMS | **U-T1-01…10** + 05b/c/d | PICS M PASS/PART+waiver | #5 evidence |
| 3 | A PF1 | **U-P5-01** TotW/TotVAr/PPV | 4 s t/q | O.8.3 / T.3.1.3 |
| 4 | A cyber | **U-T3-***, **U-D7-01/02**, **U-CYB-*** | Internal 100-3/4 | #6 evidence |
| 5 | A isolation | **P2-01…03** | No Eth_A↔B↔plant↔WAN | O.13 / 3-2 |
| 6 | A defence | **P6-03** (+ P6-01 waiver) | M trip inhibits PF2 | O.11 / M |
| 7 | A logger | **U-P7-01…04** | 2048, dump, DSO demo | O.14 / #1 |
| 8 | B docs | **U-DOC-01…07**, **U-DOC-09** | Quote pack | Unblocks C |
| 9 | C #5 | **U-ACC-01** | UCA report ID | **O.15 #5** |
| 10 | C #6 | **U-ACC-02** (+ **U-ACC-03** if quoted) | 100-3 report ID | **O.15 #6** |
| 11 | C #2/#4/#3/#8 | **U-ACC-04/05**, **P7-08/09**, **U-ACC-06** | External reports | **#2–#4, #8** |

**Do not mark O.15 PASS on Milestone A.** Phase 7 exit is the pack (`REQ-PH-005`).

---

## Verification log (corpus check — 2026-10-21)

Verified against the extracts listed. Status = extract on disk vs this matrix coverage.

| Part | Source verified | Corpus | Rows in this file | Tick when you add evidence |
|------|-----------------|--------|-------------------|----------------------------|
| A.0–A.8 Annex O | `CCI_Annex_O_Extract.md` §2.1, O.2–O.15 · **Lines** rev **1.2** | **HAVE** | + O.13.4/7, PHY, O.14 cats, **Lines** | □ |
| B Annex M | `CCI_Annex_M_Extract.md` M.1–M.5.1 | **HAVE** | M.1–M.5 + O.11/12/14 | □ |
| C Annex T | `CCI_Annex_T_Extract.md` T.3.1.3 **full table** + T.3.3.4 | **HAVE** | + Gen/St/SGG, Table 80 | □ |
| D Annex U | `CCI_CEI_0-16_Consolidated.md` | **MISSING** | Placeholder only | □ extract |
| E 61850-10 / UCA | `CCI_61850-10_Extract.md` · UCA extract CB-01…15 | **HAVE** / TP **MISSING** | D1–D6 + all CBs + sAss…sTm | □ |
| F 100-3 | `CCI_62351-100-3_Extract.md` · D7 | **HAVE** / case IDs **PART** | §4 + T1–T5 | □ |
| G 62351-4 / 100-4 | 62351-4 + 100-4 extracts | **HAVE** / tables **PART** | §6.3, §11, G.2–G.6, §13 N/A | □ |
| H.1 62443-3-2 | `CCI_62443_Zones.md` ZCR 1–7 | **HAVE** / ZCR 7 **MISS** | ZCR 1–7 | □ |
| H.2 62443-4-1 | Extract Annex B — **47** clauses | **HAVE** / SD-3/4 body **MISSING** | SM-1…SG-7 | □ |
| H.3 62443-4-2 | FR 1–7 SL-C 2 checklists | **HAVE** / CR 6.1 p.57 **MISSING** | All SL-2 CRs | □ |
| I 61557-12 | Extract REQ-MET-001…014 + Annex D/H | **HAVE** (P1) | MET-001…014 + 61010 + 17025 | □ |
| J Legal / 104 / FIPS | Phase checklists · cert samples | **PART** | #3 #7 #8 ARERA 104 | □ |

**Intentionally out of first UCA (still listed):** O.9.1.x, T Tables 88–92, CB-06/09/14, 62351-4 §13, 62351-100-6, U bis.

**Not CEI annex but required by O.15:** 61850-10, 62351-100-3, 62443-4-1/4-2, 61557-12, FIPS, ISO 9001, CE/DoC.

---

## Related

| Doc | Role |
|-----|------|
| [ANNEX_O_SOFTWARE_MATRIX.md](ANNEX_O_SOFTWARE_MATRIX.md) | O → software (shorter) |
| [LAB_UNIFIED_CERT_TEST_MATRIX.md](LAB_UNIFIED_CERT_TEST_MATRIX.md) | Live □ execution |
| [dnv/DNV_UCA_PRODUCT_READINESS.md](dnv/DNV_UCA_PRODUCT_READINESS.md) | O.15 vs product deploy |
| `knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md` | P0–P7 gates |
| `scripts/verify_annex_pki.py` | Cert A/B / CRL gates |

---

## Maintenance

When a row moves **GAP → PASS**, write evidence filename + session date in **Gap** and tick the part in the verification log.

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-20 | Initial book: O, M, T, U-gap, 61850-10 CBs, 100-3/100-4, 62443 3-2/4-1/4-2, 61557-12, O.15 #1–8 |
| 1.1 | 2026-10-20 | Second validation: add O.13.4/7, PHY/IPv4, T.3.1.3 Gen/St/SGG + Table 80, O.14 cats; fix overstated PASS; 62351-8 ≠ #6 |
| 1.2 | 2026-10-21 | **Lines** column after **Cert** on all clause tables; pin O/T/M/61850-10/62351/62443 editions |

**Keywords:** `normative`, `O.15`, `Annex O`, `Annex M`, `Annex T`, `61850-10`, `62351-100-3`, `62443`, `5-LN`, `ccli-normative-cert-validation-matrix`

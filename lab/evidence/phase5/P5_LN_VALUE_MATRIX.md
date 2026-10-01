# P5 — Complete LN / value matrix (TSP cleartext lab)

**Document ID:** P5-LN-VALUE-MATRIX-001  
**Date:** 2026-09-30  
**Build:** 0.1.0-r23 (P5-FULL-CID)  
**DUT:** TesPro TG544 · Eth_A `192.168.10.1:102` cleartext (lab bypass)  
**Config:** `apps/ccli/config/lab_tr400_cleartext_tsp.yaml`  
**CID (TSP load):** `apps/ccli/config/icd/lab_tg544_eth_a.cid`  
**Canonical map:** `apps/ccli/config/icd/signal_map.yaml`  
**Runtime:** `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp`  
**Companion:** `P5_00_LN_MATRIX_AUDIT.md` · `P5_TSP_CLEARTEXT_README.md`

---

## Lab baseline (no MMS Operate unless noted)

| Parameter | Value |
|-----------|-------|
| Modbus slave | COM5 → DUT `/dev/rs485_2_uart`, P=**450 kW**, Q=**45 kvar** |
| `plant.smax_kva` | **210 kVA** |
| `plant.poc_ppv_kv` | **20.0 kV** (yaml until P5-06 meter map) |
| DSO yaml mock | `wsd_active=true`, `wspt_pct=20` → **p_wsd=42 kW** |
| PF2 enter @ boot | **42 kW** (yaml mock via `tr57126_table1` profile) |
| PF2 release @ boot | **37 kW** (`release_delta_kw=5`) |
| `permissive_bypass` | **on** → DIO1 curtail without DI wiring |
| Product TLS path | `:3782` — evidence `P5_MMS_TLS.pcapng` (separate session) |

---

## Status legend

| Status | Meaning |
|--------|---------|
| **IMPL** | On wire; live value or live control |
| **PART** | On wire; incomplete vs CID |
| **STUB** | On wire; `Mod.stVal=5` only; no set-point DO / no plant path |
| **DEFERRED** | In CID only; **not in runtime MMS model** |
| **RUNTIME+** | In runtime; **absent from CID** |

**Value sources:** `MB` Modbus · `YAML` plant/dso config · `MMS` Operate · `BOOT` libiec61850 default · `GNSS` time-quality poll

---

## A. Modbus → MMS measurement chain

| Register | Type | Scale | MMS target | Rate | Pass (TSP read) |
|----------|------|-------|------------|------|-----------------|
| **40001** | float32 BE | 1.0 kW | `PdCMMXU1$MX$TotW$mag$f` | ~4 s | = Modbus P ±0.5 kW |
| **40003** | float32 BE | 1.0 kvar | `PdCMMXU1$MX$TotVAr$mag$f` | ~4 s | = Modbus Q ±0.5 kvar |
| **40005** | int16 | 0.001 | — | — | **Not published to MMS** |
| — | — | — | `PdCMMXU1$MX$PPV$phsAB$cVal$mag$f` | every loop | **= 20.0 kV** (yaml; P5-06 GAP) |

Map: `apps/ccli/config/modbus/simulator_map.yaml`

---

## B. Runtime MMS model — all logical nodes

Naming: **`prefix` + `lnClass` + `inst`** (e.g. `Wlim` + `DWMX` + `1` → `WlimDWMX1`).

**Counts (r23 `model_cfg`):** CID **31** LNs · Runtime **31** (genconfig `.cfg`) · legacy MVP **9** if `model_cfg` empty.

**Model file:** `apps/ccli/config/icd/lab_tg544_eth_a.cfg` (regenerate: `scripts/gen-mms-model-cfg.ps1`)

### B1. `LLN0`

| Object | FC | Status | Source @ baseline | TSP expected | CID delta |
|--------|----|--------|-------------------|--------------|-----------|
| `Mod.stVal` | ST | PART | BOOT | readable | CID default `on` |
| `Health.stVal` | ST | PART | BOOT | readable | CID `Ok` |
| `NamPlt.*` | ST | DEFERRED | — | absent | configRev, ldNs |
| `DS_R_PdC_Mis4sec` | — | IMPL | 3 FCDAs | TotW, TotVAr, PPV.phsAB | CID 4th member **A** missing |
| `urcb_PdC_Mis4sec` | BR | IMPL | intgPd **4000 ms** | ~3900–4100 ms | matches CID |
| `brcb_Stato_Allarmi_Segnali` | BR | DEFERRED | — | absent | 18 FCDAs |
| `urcb_GenAcc_Mis4sec` | BR | DEFERRED | — | absent | Gen* TotW |
| `urcb_SingGen_Mis4sec` | BR | DEFERRED | — | absent | SGG + GnGrId |

URCB member paths (exact):

```text
PdCMMXU1$MX$TotW
PdCMMXU1$MX$TotVAr
PdCMMXU1$MX$PPV$phsAB$cVal$mag$f
```

### B2. `WlimDWMX1` — O.9.2.2 lim W (**IMPL**)

| Object | FC | ctlModel (runtime) | @ baseline | After Operate Mod=1, WMaxSptPct=10 | PF2 |
|--------|----|--------------------|------------|-------------------------------------|-----|
| `Mod.stVal` | ST | direct-enhanced | **5** | **1** | live MMS overrides yaml |
| `WMaxSptPct` mxVal | MX/CO | direct-enhanced | **0.0** | **10.0** | p_wlim = **21 kW** |
| `Beh`, `FctOpSt*`, `NamPlt` | ST | — | DEFERRED | — | CID Beh=off |

O.11: `p_wlim_kw = WMaxSptPct/100 × Smax_used` (210 kVA).

### B3. `WSdDAGC1` — O.9.2.3 s.p. W (**IMPL**)

| Object | FC | @ baseline | After Operate Mod=1, WSptPct=20 | CID default |
|--------|----|------------|----------------------------------|-------------|
| `Mod.stVal` | ST | **5** | **1** | CID **1 (on)** |
| `WSptPct` mxVal | MX/CO | **0.0** | **20.0** | CID mxVal **20** |
| `Beh`, `FctOpSt`, `NamPlt` | ST | DEFERRED | — | CID Beh=on |

O.11: `p_wsd_kw = max(0, WSptPct)/100 × Smax_used` → 20% = **42 kW**.

**LN-GAP-05:** PF2 uses yaml mock (**42 kW**) at boot, but MMS reads **Mod=5, WSptPct=0** until Operate. TSP Compare Model vs CID will fail on WSd unless Operate first or compare runtime subset only.

### B4. `WSaDAGC1` — MSD O.10.3.1 (**STUB**, **RUNTIME+**)

| Object | Value | Control |
|--------|-------|---------|
| `Mod.stVal` | **5** (fixed at start) | Operate → **FAIL** (no handler) |

Not in `lab_tg544_eth_a.cid`.

### B5–B8. Reactive stubs (Mod only)

| MMS name | lnClass | On wire | Mod.stVal | Set-point DOs | Operate |
|----------|---------|---------|-----------|---------------|---------|
| `VArSdDVAR1` | DVAR | Mod | **5** | **absent** (CID: VArTgtSptPct) | FAIL / no Q path |
| `PFSPDFPF1` | DFPF | Mod | **5** | **absent** | FAIL |
| `VArVDVVR1` | DVVR | Mod | **5** | **absent** | FAIL |
| `PFWDPFW1` | DPFW | Mod | **5** | **absent** | FAIL |

Phase **P5-R** tracks full implementation.

### B9. `PdCMMXU1` — T.3.1.3 measurements (**PART**)

| DO | MMS read path | Source | Baseline | Quality | CID |
|----|---------------|--------|----------|---------|-----|
| TotW | `…$MX$TotW$mag$f` | MB 40001 | **450.0 kW** | questionable until GNSS good | IMPL |
| TotVAr | `…$MX$TotVAr$mag$f` | MB 40003 | **45.0 kvar** | same | IMPL |
| PPV.phsAB | `…$MX$PPV$phsAB$cVal$mag$f` | YAML | **20.0 kV** | — | PART (BC/CA missing) |
| A | — | — | absent | — | DEFERRED |
| Beh | — | — | absent | — | DEFERRED |

---

## C. PF2 / DIO actuation (TSP evidence)

| Scenario | p_eff enter (kW) | release (kW) | DIO1 @ P=450 |
|----------|------------------|--------------|--------------|
| Boot, yaml WSd 20% | **42** | **37** | **ON** |
| MMS Wlim Mod=1 @ 10% | **21** | **16** | **ON** |
| MMS Wlim Mod=1 @ 70% | **147** | **142** | **ON** |
| MMS WSd Mod=1 @ 20%, Wlim off | **42** | **37** | **ON** |
| Modbus P **30 kW** | (threshold unchanged) | — | **OFF** |
| Eth_A no client **15 s** | yaml mock; MMS Mod→**5** | **42** | per P |

Formula: `p_eff = min(210, p_w110?, p_wlim?, p_wsd?)` — see `dso_active_power.cpp`.

Lab ctlModel: **direct-enhanced** (not CID SBO). Use **Direct Operate** in TSP.

---

## D. Static-on-wire logical nodes (23) — r23 full cfg

**Not “missing from MMS”.** With `model_cfg` (r23), all **31** CID LNs are **on the MMS server** from `lab_tg544_eth_a.cfg`. These **23** have **static cfg defaults** — ccli does not refresh them from Modbus/DIO/plant sim.

| Group | MMS names | TSP read | TSP Operate |
|-------|-----------|----------|-------------|
| Physical | `LPHD1` | OK static | N/A |
| Nameplate DPCC | `PdC_Wi/Wa/Qi/Qc/VA` DPCC1 | OK static | N/A |
| Disconnection | `DisFRDECP1`, `DisFRDGEN1`, `DisFRDSTO1` | OK static | N/A |
| Plant MMXU2 | `GenPV/Wi/Ter/Idr`, `St`, `SGGMMXU1/2` | OK static (TotW=0) | N/A |
| Breaker | `IDGXCBR1` | OK static Pos | N/A |
| SSGG | `SSGGDGEN1/2` | OK static | N/A |
| VArV curve helpers | `VArVDPMC1/2`, `VArVDECP1/2` | OK static | N/A |

**Compare Model (r23):** expect **structure MATCH**, **value DIFF** on live DOs only.  
**Wrong deploy** (`model=mvp_handbuilt`): 22 LNs **MISSING** — do not run TSP until `ln_count=31`.

Canonical wire table: **`P5_LN_WIRE_STATUS.md`** · `python scripts/audit-ln-matrix.py --r23`

---

## E. TSP test matrix (pass criteria)

| Step | Action | Object(s) | Expected | Pass |
|------|--------|-----------|----------|------|
| 1 | Connect TLS **OFF** | — | AARQ/AARE OK | connect |
| 2 | Read | `PdCMMXU1.TotW` | = Modbus P | ±0.5 kW |
| 3 | Read | `PdCMMXU1.TotVAr` | = Modbus Q | ±0.5 kvar |
| 4 | Read | `PdCMMXU1.PPV.phsAB` | **20.0** | exact |
| 5 | EnableReport | `LLN0.urcb_PdC_Mis4sec` | intgPd 4000 ms | 3900–4100 ms |
| 6 | Read | `WlimDWMX1.Mod` | **5** @ baseline | note ≠ CID |
| 7 | Read | `WSdDAGC1.Mod` | **5** @ baseline | note LN-GAP-05 |
| 8 | Operate | Wlim WMaxSptPct=**10**, Mod=**1** | mxVal=10, Mod=1 | DIO ON @ P=450 |
| 9 | Operate | WSd WSptPct=**20**, Mod=**1** | mxVal=20, Mod=1 | p_eff=**21** if Wlim 10% on |
| 10 | Read stubs | VArSd/PFSP/VArV/PFW/WSa `.Mod` | all **5** | TotVAr unchanged |
| 11 | Operate stub | e.g. `VArSdDVAR1.Mod=1` | fail or no Q | negative |
| 12 | Compare Model | full CID | structure match r23 | values: live vs CF |
| 13 | Modbus sweep | P=400→500 kW | TotW tracks; curtail toggles | full circle |
| 14 | Disconnect 15 s | Wlim/WSd Mod | revert **5**; PF2→yaml 42 kW | P3-05 fallback |

Export log → `lab/evidence/testsuite-pro/inbox/TSP_P5_FULLCIRCLE_CLEARTEXT_*.txt`  
Sequencer draft → `lab/evidence/testsuite-pro/inbox/TSP_P5_FULLCIRCLE_SEQUENCER.md`

---

## F. Tracked discrepancies

| ID | Issue | TSP / lab handling |
|----|-------|-------------------|
| LN-GAP-01 | `WSaDAGC1` runtime, not in CID | N/A or ignore |
| LN-GAP-02 | MVP path 9 LN vs r23 cfg 31 | Verify `model=full_cid_cfg`; use wire status doc |
| LN-GAP-03 | PPV from yaml, not analyzer | expect 20 kV fixed |
| LN-GAP-04 | Stub LNs lack APC DOs | Mod read only; P5-R |
| LN-GAP-05 | MMS Mod/WSpt not seeded from yaml | Operate before CID compare |
| LN-GAP-06 | Lab direct-enhanced vs CID SBO | Direct Operate |
| LN-GAP-07 | TotW/TotVAr q questionable until GNSS | optional q check |

---

## G. Quick reference @ baseline

```text
Smax_used     = 210 kVA
p_wsd (yaml)  = 42 kW
p_eff @ boot  = 42 kW  (PF2; yaml mock)
MMS TotW      = 450 kW (Modbus)
MMS TotVAr    = 45 kvar (Modbus)
MMS PPV       = 20 kV  (yaml)
Wlim Mod      = 5      (until Operate)
WSd Mod       = 5      (until Operate)  ← PF2 still 42 kW from yaml
All stub Mod  = 5
```

---

## Verification

```powershell
python scripts/audit-ln-matrix.py
# Deploy r23 + cfg, then LN browser + Wireshark:
.\lab\tg544-openwrt\run-p5-cleartext-ln-capture.ps1
# Expect: ln_count=31, P5_MMS_CLEARTEXT_LN.pcapng, P5_MMS_CLEARTEXT_LN_ANALYSIS.txt
.\lab\tg544-openwrt\run-p5-fullcircle-cleartext.ps1
```

**Evidence index:** `P5_MMS_CLEARTEXT_LN.pcapng` · `P5_MMS_CLEARTEXT_LN_ANALYSIS.txt` · `P5_FULLCIRCLE_CLEARTEXT.txt` · `P5_MMS_TLS.pcapng`

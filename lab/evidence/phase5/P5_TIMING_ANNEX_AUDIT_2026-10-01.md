# P5 — Timing accuracy audit vs Annex O / Annex T

**Date:** 2026-10-01  
**DUT:** TG544 · `ccli 0.1.0-r25` · cleartext MMS `:102`  
**Phase index:** [P5_README.md](P5_README.md)  
**Annex test sequence:** [P5_ANNEX_TEST_SEQUENCE.md](P5_ANNEX_TEST_SEQUENCE.md) (Parts D–E)  
**TSP sequencer:** [P5_TSP_FULLCIRCLE_SEQUENCER.md](P5_TSP_FULLCIRCLE_SEQUENCER.md) (steps 5b–5d)  
**Corpus:** `Architecture/_extracted_reg_analysis/annex_o.txt`, `annex_t.txt`  
**Evidence:** P3-03, P3-11, P5 full circle 2026-10-01, live chrony 2026-10-01

---

## 1. Requirements map (normative → implementation → test)

| REQ-ID | Annex clause | Requirement | Implementation | Lab test | Status |
|--------|--------------|-------------|----------------|----------|--------|
| **REQ-TIM-001** | **T.3.3.4.5** | UTC reference; uncertainty **≤ ±100 ms** | chronyd NTP + `chrony_poll=on`; ccli `refresh_time_quality()` | `chronyc tracking`; ccli log | **PASS** (offset ≈0–5 ms) |
| **REQ-TIM-002** | **T.3.3.4.5** | `clockNotSynchronized=0` when sync valid | GNSS fix **AND** chrony ±100 ms | `verify-gnss-time-quality.ps1` | **PASS** — 13 sats; `gnss_fix=1` ([P5_GNSS_FIX_VERIFY_2026-10-01.md](P5_GNSS_FIX_VERIFY_2026-10-01.md)) |
| **REQ-TIM-003** | **T.3.3.4.5** | NTS mandatory (product) | Not deployed | — | **GAP** (lab NTP only) |
| **REQ-MET-001** | **T.3.2.1** · **T.3.1.3** | Plant measurements **every 4 s**; Type 3 | URCB `urcb_PdC_Mis4sec` **IntgPd=4000**; Modbus `poll_ms=4000` | TSP Report / `mms_lab_client` | **PASS** (Δt ≈3798–4002 ms) |
| **REQ-MET-002** | **O.8.3** · **T.3.1.3** | P/Q/V @ PdC every **4 s**, **sync :00/:04/:08…** | Free-running 4 s timer (RCB enable / poll loop) | Report `t` field analysis | **GAP** — interval OK, **not wall-clock aligned** |
| **REQ-MET-003** | **O.8.3** | 4 s **fixed block** + timestamp + quality | MV `t`/`q` on TotW/TotVAr; MC200 block N/A | Report payload | **PART** — t/q present; MC200 not implemented |
| **REQ-MET-004** | **T.3.2.1** | Type 3 transit **≈500 ms** (in substation) | Not instrumented | — | **OPEN** (not lab-gated) |
| **REQ-CTL-001** | **O.7.3.1** | TsP **≤ 60 s** (±5% band) | PF2 + plant mock | Regulation bench | **DEFERRED** |
| **REQ-CTL-002** | **O.7.3.1** | TsQ **≤ 10 s** | VArSd path partial | P5-R01 Operate | **DEFERRED** |
| **REQ-CTL-003** | **O.7.3.2** | Slow ring ΔT **10–600 s** (default 60 s) | Not implemented | — | **GAP** |
| **REQ-CTL-004** | **O.7.3.3** | Min **3 s** between external set-points | `regulation_map` R07 = **Na** | — | **GAP** |
| **REQ-MET-005** | **O.8.6** | Status change notify **≤ 4 s** | BRCB `brcb_Stato_Allarmi` defined; not driven | — | **DEFERRED** |
| **REQ-MET-006** | **61557-12** · **O.7.3.1** | MC200 **200 ms** blocks | Not in runtime | Architecture REQ | **GAP** (future) |
| **REQ-LN-001** | **T.3.1.3** | PdC POC = **MMXU1 prefix PdC** (`PdCMMXU1`) | CID + cfg + browse | GetNameList / browse | **PASS** |
| **REQ-LN-002** | **T.3.1.3** · **O.8.3** | Mandatory DOs: **TotW, TotVAr, PPV** @ 4 s | Live Modbus/yaml → `PdCMMXU1` | TSP Read + URCB | **PASS** (A optional **GAP**) |
| **REQ-LN-003** | **TR 57-126** · CID | **31 LNs** on wire = CID model | `lab_tg544_eth_a.cfg` genconfig | GetNameList count=31 | **PASS** |
| **REQ-LN-004** | **T.3.1.3** · **O.8.3** | Same report: **TotW.t = TotVAr.t = PPV.t** | Sequential `update_*` + single poll | URCB `dataSetValues` parse | **PASS** (0 ms delta) |
| **REQ-LN-005** | **T.3.3.4.5** · **O.8.3** | POC **q** coherent when sync state changes | `refresh_time_quality()` → TotW/TotVAr q | Report q field | **PART** — **PPV.q not refreshed** |
| **REQ-LN-006** | **O.9** · **T Tab. 84–85** | Control LNs: **Wlim, WSd, VArSd** in CID | W-L-C on Wlim/WSd; VArSd W--C | TSP Operate steps 8–11 | **PASS** structure |
| **REQ-LN-007** | **O.8.3** · **REQ-MET-002** | LN **t** on **:00/:04/:08…** grid | Same scheduler as URCB | Report `t` sec mod 4 | **GAP** (inherits REQ-MET-002) |

---

## 2. Precise measurements (2026-10-01)

### 2.1 UTC / TimeQuality — T.3.3.4.5 (P3-11)

**Live `chronyc tracking` (DUT):**

| Metric | Value | Limit | Verdict |
|--------|------:|-------|---------|
| System time vs NTP | **+0.079 ms** | ±100 ms | **PASS** |
| Last offset | **+0.863 ms** | ±100 ms | **PASS** |
| RMS offset | **5.27 ms** | ±100 ms | **PASS** |
| Stratum | 2 | — | OK |
| Leap status | Normal | — | OK |
| GNSS SOCK source | `#? GNSS` Reach **0** | fix for sync flag | **NO FIX** |

**ccli log (same session):**

```text
mms: time_quality chrony offset_ms=0 within_pm100=yes leap=Normal ref=1FD155F3
     gnss_fix=0 clockNotSynchronized=1 (T.3.3.4.5)
```

| Check | Result |
|-------|--------|
| Absolute UTC accuracy (chrony) | **PASS** |
| MMS `clockNotSynchronized` | **1** (requires GNSS fix in ccli logic) |
| `TotW.q` / `TotVAr.q` | **Questionable** @ 2026-10-01 AM — **re-check after GNSS lock** |

### 2.1b GNSS remediation (2026-10-01)

| Event | Detail |
|-------|--------|
| Prior | `gnss_get_position` → `"message":"off"`, `fix:false` |
| Action | `ubus call sim-manager gnss_start` → `powered_on:true` |
| Operator | GNSS **fixed** (antenna/service) — re-verify §3 in [P5_GNSS_FIX_VERIFY_2026-10-01.md](P5_GNSS_FIX_VERIFY_2026-10-01.md) |
| Pass when | `gnss_fix=1`, `clockNotSynchronized=0`, TotW.q **Good** |

### 2.2 MMS integrity period — T.3.2.1 / T.3.1.3 (P3-03 / P5-003)

**Source:** `P5_FULLCIRCLE_CLEARTEXT_2026-10-01_111628.txt` · URCB `LLN0.RP.urcb_PdC_Mis4sec01` · IntgPd=4000

| Report # | Δt (ms) | TotW (kW) |
|---------|--------:|----------:|
| 1 (GI) | 0 | 450 |
| 2 | 3798 | 450 |
| 3 | 3998 | 450 |
| 4 | 4002 | 450 |
| 5 | 4002 | 450 |

| Metric | Value | Pass band |
|--------|------:|-----------|
| **avg_interval_ms** | **3950** | 3900–4100 ms (lab) |
| IntgPd configured | 4000 ms | CID match |
| P3-03 prior avg | 3961 ms | Consistent |

**Verdict:** **PASS** — 4 s MMS integrity cadence.

### 2.3 Wall-clock boundary alignment — O.8.3 (strict)

**Norm:** Transmissions synchronized to **:00, :04, :08, … :56** within each minute.

**Report `t` timestamps (UTC seconds):**

| Timestamp (UTC) | sec | sec mod 4 | On grid? |
|-----------------|----:|----------:|:--------:|
| 09:18:**19**.941 | 19 | 3 | **No** |
| 09:18:**23**.799 | 23 | 3 | **No** |
| 09:18:**27**.793 | 27 | 3 | **No** |
| 09:18:**31**.864 | 31 | 3 | **No** |
| 09:18:**35**.823 | 35 | 3 | **No** |

Integrity fires **~4 s after RCB enable**, not on minute phase. Modbus `poll_ms=4000` is also free-running.

**Verdict:** **GAP (REQ-MET-002)** — period correct; **boundary alignment not implemented**.

**Product fix (tracked):** align URCB integrity + Modbus poll to next `(epoch_ms / 4000) * 4000` boundary; or chrony-triggered scheduler.

### 2.4 TesPro collector poll — P4-00 cross-check

| Parameter | Config | Notes |
|-----------|--------|-------|
| Point interval | 4 s | Matches MMS |
| Object ref | `…TotW.mag.f` | Leaf read OK |
| Live value | **450.0000** | Matches Modbus (2026-10-01) |

Poll jitter independent of MMS integrity; acceptable for supplier collector gate.

### 2.5 Logical node alignment — T.3.1.3 / O.8.3 / CID

**Tool:** `python scripts/audit-ln-matrix.py --r23` · P5 full-circle browse · URCB reports §2.2

#### 2.5.1 Annex T POC model (T.3.1.3)

| Annex mapping | MMS object | CID | Runtime | Verdict |
|---------------|------------|-----|---------|---------|
| MMXU1 prefix **PdC** · TotW | `PdCMMXU1.TotW.mag.f` | YES | Modbus 40001 | **PASS** |
| MMXU1 prefix **PdC** · TotVAr | `PdCMMXU1.TotVAr.mag.f` | YES | Modbus 40003 | **PASS** |
| MMXU1 prefix **PdC** · PPV | `PdCMMXU1.PPV.phsAB.cVal.mag.f` | YES | yaml 20 kV | **PASS** |
| MMXU1 prefix **PdC** · A (optional) | `PdCMMXU1.A` | YES | static **1970** t | **GAP** — not in Modbus map |

Naming rule verified: `prefix` + `lnClass` + `inst` → **`PdC` + `MMXU` + `1` = `PdCMMXU1`**.

#### 2.5.2 CID / wire inventory (REQ-LN-003)

| Check | Expected | Measured (2026-10-01) | Verdict |
|-------|----------|------------------------|---------|
| CID LN count | 31 | 31 | **PASS** |
| CFG (genconfig) LN count | 31 | 31 | **PASS** |
| Browse GetNameList | 31 under `LD_Plant` | 31 (no `WSaDAGC1`) | **PASS** |
| Live-updated LNs | ≥4 POC/control | LLN0, PdCMMXU1, Wlim, WSd | **PASS** |
| MVP-only orphan | none on wire | `WSaDAGC1` absent r23 cfg | **PASS** |

#### 2.5.3 URCB dataset ↔ LN binding

URCB `urcb_PdC_Mis4sec` · IntgPd=4000 · dataset `DS_R_PdC_Mis4sec`:

| # | FCDA (CID) | Report member | Live in report | Verdict |
|---|------------|---------------|----------------|---------|
| 1 | PdC/MMXU1 TotW | TotW[MX] | 450 kW + t + q | **PASS** |
| 2 | PdC/MMXU1 TotVAr | TotVAr[MX] | 45 kvar + t + q | **PASS** |
| 3 | PdC/MMXU1 PPV | PPV[MX] | 20 kV + t + q | **PASS** value; **PART** q |
| 4 | PdC/MMXU1 A | A[MX] | 0 A, t=**1970** | **GAP** — static cfg default |

#### 2.5.4 Intra-LN timestamp coherence (REQ-LN-004)

Within each integrity report, all live POC MV timestamps must match (V-MET-03).

| Report | TotW.t | TotVAr.t | PPV.t | Δ max | Verdict |
|--------|--------|----------|-------|------:|---------|
| #1 (GI) | …19.941Z | …19.941Z | …19.941Z | 0 | **PASS** |
| #2 | …23.799Z | …23.799Z | …23.799Z | 0 | **PASS** |
| #3 | …27.793Z | …27.793Z | …27.793Z | 0 | **PASS** |
| #4 | …31.864Z | …31.864Z | …31.864Z | 0 | **PASS** |
| #5 | …35.823Z | …35.823Z | …35.823Z | 0 | **PASS** |

**Implementation note:** `ccli_main` calls `update_tot_w_kw` → `update_totvar_kvar` → `update_ppv_kv` in one Modbus poll; each sets `Hal_getTimeInMs()` — sub-ms drift possible but not observed on wire.

#### 2.5.5 Intra-LN quality coherence (REQ-LN-005)

| Attribute | TotW.q | TotVAr.q | PPV.phsAB.q | Verdict |
|-----------|--------|----------|-------------|---------|
| Report #2–5 | `1100000001000` | `1100000001000` | `0000000000000` | **FAIL** PPV |
| Meaning | Questionable + inaccurate | same | **Good (stale cfg default)** | LN-GAP-08 |

`refresh_time_quality()` updates **TotW.q** and **TotVAr.q** only; **`PPV.phsAB.q` not wired** to time-quality refresh (`mms_adapter.cpp`). Product fix: apply same `poc_q` to `ppv_phsab_q`.

#### 2.5.6 LN timestamp vs Annex grid (REQ-LN-007)

Same phase error as §2.3 — LN `t` seconds **≡ 3 (mod 4)**, not **≡ 0**:

| LN DO | Example t (UTC) | sec mod 4 | On O.8.3 grid? |
|-------|-----------------|----------:|:--------------:|
| TotW | 09:18:**23**.799 | 3 | **No** |
| TotVAr | 09:18:**27**.793 | 3 | **No** |
| PPV.phsAB | 09:18:**31**.864 | 3 | **No** |

**Verdict:** LN structure and intra-report alignment **PASS**; wall-clock phase **GAP** (shared with REQ-MET-002).

#### 2.5.7 Control LN alignment (O.9 — TSP cross-check)

| LN | Annex | CID | Wire tier | TSP step | Verdict |
|----|-------|-----|-----------|----------|---------|
| `WlimDWMX1` | O.9.2.2 | YES | W-L-C | Operate WMaxSptPct | **PASS** |
| `WSdDAGC1` | O.9.2.3 | YES | W-L-C | Operate WSptPct | **PASS** |
| `VArSdDVAR1` | O.9.1.4 | YES | W--C | Operate VArTgtSptPct | **PART** — struct only until plant Q path |
| Stub DER (`PFSP`, `VArV`, `PFW`) | O.9.1.x | YES | W--C | Mod=5 read | **PASS** browse |

---

## 3. Clause traceability summary

| Annex | Topic | Lab status |
|-------|-------|------------|
| **T.3.2.1** | Type 3 · 4 s periodic measurements | **PASS** (interval) |
| **T.3.1.3** | TotW / TotVAr / PPV @ PdC | **PASS** (values) |
| **T.3.3.4.5** | UTC ±100 ms | **PASS** (chrony); **PART** (GNSS sync flag) |
| **O.8.3** | 4 s block + t + q @ PdC | **PART** (t/q yes; grid **GAP**) |
| **O.7.3.1** | TsP 60 s / TsQ 10 s | **DEFERRED** |
| **O.7.3.3** | 3 s command spacing | **GAP** (R07 Na) |
| **T.3.1.3** | PdC MMXU1 TotW/TotVAr/PPV | **PASS** (A **GAP**) |
| **TR 57-126** | 31 LN CID = wire | **PASS** |
| **O.8.3** | Intra-LN t/q coherence | **PART** (t **PASS**, q **PPV GAP**) |

---

## 4. Verification plan (next precise tests)

| ID | Requirement | Procedure | Pass criteria |
|----|-------------|-----------|---------------|
| **V-TIM-01** | T.3.3.4.5 ±100 ms | `chronyc tracking` + 24 h log | \|offset\| ≤ 100 ms |
| **V-TIM-02** | GNSS → Good quality | Fix GNSS SOCK / PPS; recheck `gnss_fix=1` | `clockNotSynchronized=0`, q=Good |
| **V-MET-01** | 4 s integrity | TSP Report Viewer 60 s; export Δt | 3900–4100 ms each |
| **V-MET-02** | O.8.3 grid | After boundary fix; check report `t` sec mod 4 | **= 0** on all integrity reports |
| **V-MET-03** | Timestamp coherence | Same report: TotW.t = TotVAr.t = PPV.t | Equal within 1 ms |
| **V-LN-01** | REQ-LN-003 | TSP GetNameList `LD_Plant` | **31** LNs; includes `PdCMMXU1` |
| **V-LN-02** | REQ-LN-001/002 | TSP Read `PdCMMXU1.TotW/TotVAr/PPV.phsAB` | Names + values match matrix |
| **V-LN-03** | REQ-LN-004 | URCB report export; compare t fields | All POC t identical per report |
| **V-LN-04** | REQ-LN-005 | Same report; compare q on TotW/TotVAr/PPV | All q match sync state |
| **V-LN-05** | REQ-LN-007 | Report `t` sec mod 4 on 10+ integrity reports | **= 0** after scheduler fix |
| **V-CTL-01** | O.7.3.3 3 s spacing | Two Wlim Operate < 3 s apart | Second rejected |

---

## 5. Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-TIM-01 | No :00/:04/:08 alignment | DSO energy accounting drift vs spec | REQ-MET-002 fix in scheduler |
| R-TIM-02 | q=Questionable without GNSS fix | DSO may reject data | C5 GNSS/PPS; or lab waiver LN-GAP-07 |
| R-TIM-03 | TesPro poll ≠ MMS phase | Collector sees stale sample | Prefer RCB subscription over poll |
| R-LN-01 | PPV.q stale Good while TotW.q Questionable | DSO rejects inconsistent POC block | LN-GAP-08: refresh PPV.q in `refresh_time_quality()` |
| R-LN-02 | Dataset member A @ epoch 1970 | Compare / DSO validity fail | Wire A from meter or omit from URCB until P5-06 |

---

## 6. Overall timing gate (lab)

| Gate | Verdict |
|------|---------|
| **4 s MMS measurement cadence** | **PASS** |
| **UTC ±100 ms (absolute)** | **PASS** |
| **O.8.3 minute-grid alignment** | **FAIL / GAP** — track REQ-MET-002 |
| **Full TimeQuality Good (TotW/TotVAr)** | **PASS** (GNSS sky lock 2026-10-01) |
| **LN model alignment (31 LNs, PdCMMXU1)** | **PASS** |
| **Intra-LN timestamp coherence (t)** | **PASS** |
| **Intra-LN quality coherence (q)** | **PART** — PPV.q (LN-GAP-08) |
| **O.8.3 LN t grid alignment** | **FAIL / GAP** — REQ-LN-007 = REQ-MET-002 |

**Recommendation:** Accept P5 lab gate on **interval + chrony + LN structure/coherent t**; open **REQ-MET-002 / REQ-LN-007** (boundary scheduler) and **LN-GAP-08** (PPV.q refresh).

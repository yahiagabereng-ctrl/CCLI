# CCLI — Reactive power roadmap · RAG gap report

**Document ID:** CCLI-LAB-Q-ROADMAP-GAP-001  
**Revision:** 1.1  
**Date:** 2026-10-01  
**Authority:** Programme assignment into **Phase 5** (P5-M / P5-R)  
**Checklists:** `knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md` (rev 1.2)  
**Freeze:** `apps/ccli/config/icd/signal_map.yaml`

---

## 1. Verdict

Reactive power was **not** on any closed phase exit. It is now **explicitly assigned to Phase 5**:

| Gate group | Owns | Status |
|------------|------|--------|
| **P5-M** | T.3.1.3 / O.8 — DSO must **see** Q (and V) | **PASS** (lab r25) — TotW/TotVAr/PPV on wire; P5-06 meter map **OPEN** |
| **P5-R** | O.9.1 / T Tables 88–92 — DSO may **command** Q / cosφ | **PART** — **P5-R01 VArSd PASS** (lab); P5-R02–R09 **OPEN** |

**Not** Phase 4 (104). **Not** a Phase 3 reopen (P3 closed on O.9.2 active P).

---

## 2. RAG / corpus check (HAVE vs MISSING)

### 2.1 Normative extracts — HAVE

| Source | `source_id` / path | Assigned data used |
|--------|--------------------|--------------------|
| Annex T §3 TotVAr/PPV **Mandatory** | `CCI_Annex_T_Extract.md` | P5-M07, P5-M08 |
| Annex T §5.4–5.8 Tables 88–92 | same | P5-R01…R04, R08 |
| Annex O §3.1 **TsQ ≤ 10 s** | `CCI_Annex_O_Extract.md` | P5-R05 |
| Annex O §3.2 ΔT 10–600 s / default 60 s | same | P5-R06 |
| Annex O §8 O.11 indices 5–7 | same | P5-R07 |
| Annex O §7.1–7.2 δcosφ / δQ | same | P5-R03, P5-R04 |
| TR 57-126 Figura 2 (reactive **Inactive**) | `CCI_TR_57-126_Extract.md` | WAIVE policy for unused functions |
| 61557-12 MC200 / MCΔT | `CCI_61557-12_Extract.md` | P5-03, P5-R slow ring |
| Phase checklists | `ccli-phase-regulation-checklists` | Gate IDs |
| Product roadmap | `ccli-product-spec-roadmap` | Week cluster P5 |

### 2.2 Corpus — MISSING / weak (do not invent)

| Gap | Impact | Owner |
|-----|--------|-------|
| Full Allegato O Italian PDF page cite for O.9.1.4 vs EN extract | Cross-check before cert | Knowledge |
| Plant-specific CID with VArSd APC | P5-R Operate evidence | Model |
| Inverter / plant **Q actuation API** (Modbus map, GOOSE, vendor) | Blocks P5-R01 PASS | HW/plant |
| Real analyzer Q/V register datasheet | P5-06 / P5-M08 | Metrology |
| O.10.3.2 MSD Q contract | VArSa N/A until DSO/TSO | Commercial |
| Accredited TsQ ≤ 10 s test procedure | P5-R05 / P7 | Lab |

---

## 3. Code / freeze vs assigned data

| Object / clause | Freeze (`signal_map`) | Runtime today | Roadmap gate |
|-----------------|----------------------|---------------|--------------|
| `PdCMMXU1.TotW` | IMPL | Published | **PASS** (lab r25) |
| `PdCMMXU1.TotVAr` | IMPL | Published from Modbus Q | **PASS** P5-M07 (lab r25) |
| `PdCMMXU1.PPV` | IMPL | Published (yaml V) | **PASS** P5-M08 (lab r25) |
| Modbus `q_kvar` | — | **HAVE** in `MeasurementStore` | Wired → TotVAr |
| `VArSdDVAR1` | IMPL Operate | Modbus FC16 Q path | **PASS** P5-R01 (lab r25) |
| `PFSPDFPF1` | STUB | — | **P5-R02** |
| `VArVDVVR1` | STUB | — | **P5-R03** |
| `PFWDPFW1` | STUB | — | **P5-R04** |
| `WSaDAGC1` | STUB | Active-P MSD | P3 residual / O.10.3.1 (not Q) |
| `VArSa` (Table 89) | Not in model | — | **P5-R08** discretionary |
| O.11 reactive tier | Math P-only | `dso_active_power` | **P5-R07** |
| Plant Q → actuator | — | **MISSING** (DIO1 = P curtail only) | **P5-R** blocker |

---

## 4. What was missed before this update

| Miss | Severity | Fix applied |
|------|----------|-------------|
| No phase owned **O.9.1** / TotVAr | **High** — T.3.1.3 mandatory Q unpublished | REQ-PH-006 + P5-M/P5-R gates |
| P5 “Out of scope: New DSO control (already P3)” | **High** — falsely blocked reactive | Rewritten Phase 5 section |
| Cross-walk `T (all) → P3` | **Medium** — hid measurement gaps | Split TotW / TotVAr / Tables 88–92 |
| Lab matrix “R10–R13 → Phase 3+” | **Medium** — unowned placeholder | → **Phase 5-R** |
| Product roadmap P5 exit = TotW only | **Medium** | Exit includes TotVAr + VArSd/GAP |
| No plant-Q BOM / knowledge-gap row | **Medium** | Checklist knowledge gaps + risks R-PH-06/07 |

---

## 5. Recommended implementation order (inside P5)

1. **P5-M07** — Add `TotVAr` MV + dataset/RCB member; `update_tot_var_kvar` from Modbus (symmetric to TotW).  
2. **P5-M08** — PPV when meter map exists (else GAP).  
3. **P5-R01** — Promote `VArSdDVAR1` to ENC+APC + `DsoLiveCommand` Q fields; **plant Q adapter** (new).  
4. **P5-R05/R07** — TsQ bench + extend O.11 arbiter.  
5. **P5-R02…R04** — per Operating Rule / TR case (default WAIVE if Inactive like Figura 2).

Programme **next** remains **Phase 4** (104); P5-M can proceed in parallel after P3 publish path is stable.

---

## 6. Verification of this report

| Check | Result |
|-------|--------|
| TotVAr mandatory in T extract | **HAVE** — T.3.1.3 |
| TsQ 10 s in O extract | **HAVE** — O.7.3.1 |
| O.11 reactive indices 5–7 | **HAVE** — O Table 1 |
| Checklist gates P5-M07 / P5-R01 exist | **HAVE** — rev 1.2 |
| Plant Q path in code | **MISSING** |
| TotVAr in `mms_adapter` model | **MISSING** |

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-26 | First RAG-checked assignment of reactive to P5-M/P5-R + miss list |

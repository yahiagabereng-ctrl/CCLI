# P5 Annex trace — **5-LN certificate scope only**

**Document ID:** CCLI-P5-5LN-SCOPE-001  
**Revision:** 1.0  
**Date:** 2026-10-14  
**RAG source_id:** `ccli-p5-fullcircle-5ln-scope`  
**Replaces for product cert:** steps in [P5_TSP_FULLCIRCLE_SEQUENCER.md](../evidence/phase5/P5_TSP_FULLCIRCLE_SEQUENCER.md) that assume **31 LNs**, **VArSd**, **PFSP**, or `:102` cleartext.

**Matrix row:** `U-P5-01` in [LAB_UNIFIED_CERT_TEST_MATRIX.md](LAB_UNIFIED_CERT_TEST_MATRIX.md)  
**Specimen:** [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md)

---

## In scope (5 LNs on wire)

| MMS name | SCL (lab lineage) | Cert tests |
|----------|-------------------|------------|
| `LLN0` | `LN0` under `LD_Plant` | Datasets · **urcb_PdC_Mis4sec** |
| `LPHD1` | `LPHD` inst 1 | Browse / MICS |
| `PdCMMXU1` | `prefix="PdC"` · `MMXU` inst 1 | T.3.1.3 reads · URCB dataset |
| `WlimDWMX1` | `prefix="Wlim"` · `DWMX` inst 1 | O.9.2.2 Operate |
| `WSdDAGC1` | `prefix="WSd"` · `DAGC` inst 1 | O.9.2.3 Operate |

**Out of scope (must not appear in cert CID or P5 run):** `VArSd*`, `PFSP*`, `Gen*`, `DisFR*`, `brcb_*`, `gcb_*`, extra `urcb_GenAcc_*` / `urcb_SingGen_*`.

---

## Sequencer steps (3782 TLS — align with TSP Tier 1 + matrix)

| # | Annex | Action | Object | Pass |
|---|-------|--------|--------|------|
| 0 | T.3.3.4.5 | DUT `chronyc tracking` | — | \|offset\| ≤ 100 ms |
| 1 | T.3.3.3 | Connect TLS | `CCI016_01` @ `:3782` | OK |
| 2 | T.3.1.3 | Read | `PdCMMXU1.TotW` · `TotVAr` · `PPV` | Plausible (meter/RTU) |
| 3 | TR | Compare Model | vs **5-LN** CID **before** URCB enable | 0 MISSING · Value 0 target |
| 4 | T.3.2.1 | Enable URCB | `LLN0.urcb_PdC_Mis4sec` · IntgPd=**4000** | RptEna · Δt 3900–4100 ms |
| 5 | O.9.2.2 | Operate enhanced | `WlimDWMX1.Mod` + `WMaxSptPct` | OK · DIO trace — run [enable-dut-gpio-full-cycle.ps1](../enable-dut-gpio-full-cycle.ps1) · **close DIO2 permissive** |
| 6 | O.9.2.3 | Operate enhanced | `WSdDAGC1.Mod` + `WSptPct` | OK |
| 7 | T.3.1.3 | Modbus sweep (PC-A) | RTU **COM5** or EMT432 TCP | TotW tracks |
| 8 | P3-05 | Release 15 s | — | Fallback per PIXIT |

**Do not run** legacy steps **10–11** (VArSd / PFSP reads) or **12b** (31-LN GetNameList).

Evidence: `TSP_P5_FULLCIRCLE_TLS_*` with header **`specimen=5-LN`**.

---

## MMS paths (copy into TSP)

```text
LD:      LD_Plant
URCB:    CCI016_01LD_Plant/LLN0.urcb_PdC_Mis4sec
Dataset: DS_R_PdC_Mis4sec
Wlim:    CCI016_01LD_Plant/WlimDWMX1.Mod
         CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct
WSd:     CCI016_01LD_Plant/WSdDAGC1.Mod
         CCI016_01LD_Plant/WSdDAGC1.WSptPct
```

**Keywords:** `P5`, `5-LN`, `Annex-T`, `3782`, `ccli-p5-fullcircle-5ln-scope`

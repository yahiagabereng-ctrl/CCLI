# MICS draft — model vs `lab_tg544_eth_a.cid`

**Document ID:** CCLI-LAB-MICS-001  
**Revision:** 1.0  
**Date:** 2026-10-08  
**RAG source_id:** `ccli-lab-mics-draft`  
**Copy into:** `lab/TemplateMICS_Ed2_FromTP2.0.5.docx`  
**SCL:** `apps/ccli/config/icd/lab_tg544_eth_a.cid` · IED **`CCI016_01`** · LD **`LD_Plant`**

Namespace on CEI LNs: `(Tr)IEC61850-CEI016:2022` (TR 57-126).  
Compare Model (TSP T1-02) must use **this same CID**.

---

## Logical nodes (inventory)

| Prefix + LN | Class | Role | First-pass test relevance |
|-------------|-------|------|---------------------------|
| LLN0 | LLN0 | Datasets + URCBs / BRCB / GoCBs | **M** — sDs / sRp |
| LPHD1 | LPHD | Physical device | **M** — Table 3 |
| PdC_Wi / PdC_Wa / PdC_Qi / PdC_Qc / PdC_VA DPCC | DPCC | POC power capability | Model |
| DisFR DECP / DGEN / DSTO | DECP DGEN DSTO | Disconnect / gen / store flags | Model |
| **PdCMMXU1** | MMXU | **TotW, TotVAr, PPV, A** at POC | **M** — T.3.1.3 |
| GenPV / GenWi / GenTer / GenIdr / St MMXU | MMXU | Generation / storage P | Optional URCB `urcb_GenAcc_*` |
| SGG MMXU1/2 | MMXU | Single generator P | Optional URCB `urcb_SingGen_*` |
| IDG XCBR | XCBR | Breaker (ctlModel status-only) | Status |
| SSGG DGEN | DGEN | Generator group | Model |
| **WlimDWMX1** | DWMX | **O.9.2.2** active-power limit | **M** — sCtl |
| **WSdDAGC1** | DAGC | **O.9.2.3** P modulation | **M** — sCtl |
| VArSdDVAR1 | DVAR | Reactive setpoint | **O** |
| PFSP DFPF | DFPF | Power factor SP (SBO in CID) | **N/A** first pass |
| VArV DVVR / DPMC / DECP | DVVR DPMC DECP | Q(V) | **N/A** first pass |
| PFW DPFW | DPFW | cosφ(P) | **N/A** first pass |

---

## Datasets and reports (claimed vs present)

| SCL name | Type | Claim in PICS |
|----------|------|----------------|
| `DS_R_PdC_Mis4sec` | DataSet | **M** |
| `urcb_PdC_Mis4sec` | URCB intgPd=4000 | **M** |
| `urcb_GenAcc_Mis4sec` | URCB | **O** (same pattern) |
| `urcb_SingGen_Mis4sec` | URCB | **O** |
| `DS_R_Stato_Allarmi_Segnali` + `brcb_Stato_Allarmi_Segnali` | BRCB | **N/A** |
| `gcb_PdC_Mis4sec` / `gcb_Stato_Allarmi` | GOOSE | **N/A** |

---

## Control objects (claimed)

| Object reference | ctlModel | Annex |
|------------------|----------|--------|
| `CCI016_01LD_Plant/WlimDWMX1.Mod` | direct-with-enhanced-security | O.9.2.2 |
| `CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct` | direct-with-enhanced-security | O.9.2.2 |
| `CCI016_01LD_Plant/WSdDAGC1.Mod` | direct-with-enhanced-security | O.9.2.3 |
| `CCI016_01LD_Plant/WSdDAGC1.WSptPct` | direct-with-enhanced-security | O.9.2.3 |

---

## Extensions

Private / TR namespaces: CEI 0-16 TR 57-126 LN classes (DWMX, DAGC, DVAR, …).  
Questionnaire 4.2: minimize TesPro-only LNs — this CID is the CEI TR specimen, not TesPro collector LNs.

---

**RAG tags:** `lab`, `MICS`, `CID`, `TR-57-126`, `ccli-lab-mics-draft`

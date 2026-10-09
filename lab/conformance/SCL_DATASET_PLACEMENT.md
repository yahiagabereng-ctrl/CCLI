# SCL schema — DataSet placement (lab CID)

**Document ID:** CCLI-LAB-SCL-DS-001  
**Revision:** 1.0  
**Date:** 2026-10-09  
**RAG source_id:** `ccli-lab-scl-dataset-placement`  
**Normative:** IEC 61850-6 (DataSet / ReportControl under LN0) · specimen `apps/ccli/config/icd/lab_tg544_eth_a.cid`

---

## Schema header (Ed. 2)

```xml
<SCL … version="2007" revision="B" release="4"
     xsi:schemaLocation="http://www.iec.ch/61850/2003/SCL SCL.xsd"
     xmlns="http://www.iec.ch/61850/2003/SCL">
```

PICS Cover: **Edition 2 · schema 2007B**.

---

## Required tree (where DataSets live)

DataSets and ReportControls **must** be children of **`LN0` (`LLN0`)** in the same `LDevice`. Do **not** put DataSets under MMXU / DAGC / other LNs.

```text
SCL
 └─ IED name="CCI016_01"
     └─ AccessPoint / Server
         └─ LDevice inst="LD_Plant"
             └─ LN0 lnClass="LLN0"          ← DataSet + ReportControl here
                  ├─ DataSet name="DS_R_*"
                  │    └─ FCDA … (same Server only)
                  └─ ReportControl … datSet="DS_R_*"   ← same LN0
```

| Rule | Lab CID |
|------|---------|
| DataSet under LLN0 | Yes |
| `ReportControl/@datSet` names a DataSet in **same LN** | Yes |
| FCDA only from this Server (`ldInst="LD_Plant"`) | Yes |
| Online create/delete | **N/A** — `ConfDataSet modify="false"` |
| DataSet name length | ≤ 32 (practical ≤ 20 often) |

---

## Lab inventory (LLN0)

| DataSet | Bound ReportControl | First-pass PICS |
|---------|---------------------|-----------------|
| `DS_R_PdC_Mis4sec` | `urcb_PdC_Mis4sec` (IntgPd=4000) | **M** — TotW/TotVAr/PPV (+ A in CID) |
| `DS_R_GenAcc_Mis4sec` | `urcb_GenAcc_Mis4sec` | Optional / secondary |
| `DS_R_SingGen_Mis4sec` | `urcb_SingGen_Mis4sec` | Optional / secondary |
| `DS_R_Stato_Allarmi_Segnali` | `brcb_Stato_Allarmi_Segnali` | **N/A** first quote (BRCB) |

### PdC dataset FCDAs (TSP T1-05)

```xml
<DataSet name="DS_R_PdC_Mis4sec" desc="Misure al PdC a 4sec del CCI">
  <FCDA ldInst="LD_Plant" lnClass="MMXU" fc="MX" lnInst="1" prefix="PdC" doName="TotW" />
  <FCDA ldInst="LD_Plant" lnClass="MMXU" fc="MX" lnInst="1" prefix="PdC" doName="TotVAr" />
  <FCDA ldInst="LD_Plant" lnClass="MMXU" fc="MX" lnInst="1" prefix="PdC" doName="PPV" />
  <FCDA ldInst="LD_Plant" lnClass="MMXU" fc="MX" lnInst="1" prefix="PdC" doName="A" />
</DataSet>
```

URCB OptFields (CID): seqNum, timeStamp, dataSet, reasonCode, dataRef, entryID, configRef — **no** bufOvfl on URCB.

---

## TSP / lab use

1. Import **this CID** into Test Suite Pro (same file as Compare Model T1-02).  
2. Enable URCB only **after** Compare — `LLN0.RP.urcb_PdC_Mis4sec01`.  
3. Field pack copies CID via `scripts/stage-tsp-field-pack.ps1`.

**Related:** [MICS_DRAFT.md](MICS_DRAFT.md) · [CID_PICS_ALIGNMENT.md](CID_PICS_ALIGNMENT.md) · [PIXIT_DRAFT.md](PIXIT_DRAFT.md)

---

**RAG tags:** `SCL`, `DataSet`, `LLN0`, `61850-6`, `CID`, `urcb_PdC_Mis4sec`, `ccli-lab-scl-dataset-placement`

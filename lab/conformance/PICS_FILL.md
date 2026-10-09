# PICS fill sheet — first-pass UCA quote (copy into Excel)

**Document ID:** CCLI-LAB-PICS-FILL-001  
**Revision:** 1.0  
**Date:** 2026-10-08  
**RAG source_id:** `ccli-lab-pics-fill`  
**Target workbook:** `lab/TemplatePics_Ed1Ed2Ed2p1_Excel_rev3p0.xlsx`  
**IED / CID:** `CCI016_01` · `apps/ccli/config/icd/lab_tg544_eth_a.cid`  
**Platform:** TesPro TG544 · `ccli` OpenWrt 25.12  
**Code check:** `mms_adapter.cpp` 2026-10-08  

**Notation:** **M** = claimed / lab will test · **O** = optional claim · **N/A** = not claimed (first pass).  
Do **not** copy raw CID `<Services>` into PICS — see [CID_PICS_ALIGNMENT.md](CID_PICS_ALIGNMENT.md).

---

## Cover

| Field | Fill |
|-------|------|
| Vendor | HiTEKS |
| Product | CCLI / CCI (CEI 0-16 Central Plant Controller) |
| IED name | `CCI016_01` |
| Hardware | TesPro TG544 (TR500 / MT798X) |
| Firmware | `ccli --version` on DUT (record at submit) |
| SCL | Edition 2 · schema 2007B |
| SCSM | IEC 61850-8-1 MMS |
| Date | submit date |
| Contact | HiTEKS engineering |

---

## General

| Topic | Fill |
|-------|------|
| Role | **Server only** (not client, not subscriber) |
| Model edition | Ed. 2 (7-3 / 7-4 + TR 57-126 CEI 0-16 LNs) |
| Test procedure target | Confirm lab: 61850-10 Ed. 2.1 / UCA TP 2.0.5–2.0.6 |
| Process bus 9-2 / 61869-9 | **Not claimed** |

---

## ACSI Basic Conformance

| ID (typical) | Capability | Fill | Justification |
|--------------|------------|------|----------------|
| B11 | Server | **M** | `IedServer` on Eth_A |
| B12 | Client | **N/A** | TSP is the client |
| B21 | SCSM 8-1 MMS | **M** | libiec61850 MMS |
| B22 | SCSM 9-1 | **N/A** | |
| B23 | SCSM 9-2 SV | **N/A** | |
| B24 / 9-3 | Time / PTP process | **N/A** | GNSS + chrony NTP, not 9-3 claim |
| B31 | GOOSE | **N/A** | Publisher exists in lab CID — **do not claim** first pass |
| B32 | GSSE | **N/A** | |
| B41 | TCP/IP | **M** | |

---

## ACSI Service Conformance (first pass)

| Service / block | Fill | 61850-10 | Internal gate | Notes |
|-----------------|------|----------|---------------|--------|
| Associate / Abort / Release | **M** | sAss 4–5 | P3-01 T1-01/10 | TLS :3782 product |
| GetServerDirectory | **M** | sSrv | P3-03 T1-03 | |
| GetLogicalDeviceDirectory | **M** | sSrv | P3-03 | `LD_Plant` |
| GetLogicalNodeDirectory | **M** | sSrv | P3-03 | ~31 LNs |
| GetDataValues | **M** | sSrv | P3-03 T1-04 | TotW / TotVAr / PPV |
| SetDataValues | **O** | sSrv | — | Prefer **Control Operate** for Wlim/WSd; mark **O** unless lab requires **M** |
| GetDataSetDirectory / GetDataSetValues | **M** | sDs 8–9 | T1-05 | `DS_R_PdC_Mis4sec` |
| Create/Delete/Set DataSet | **N/A** | sDs | — | `ConfDataSet modify="false"`; static only |
| **Unbuffered reporting** (Get/SetURCB, Report) | **M** | **sRp Table 14** | P3-03 T1-05/06 | IntgPd 4000; TrgOps **period + GI** (no dchg) |
| Buffered reporting | **N/A** | sBr 16–17 | — | `brcb_*` exists in CID — **do not claim** |
| Logging | **N/A** | sLog | — | CID `ConfLogControl` — **do not claim** |
| GOOSE publish / subscribe | **N/A** | sGop 20–25 | P5-G | Exclude first quote |
| **Control** (Operate) | **M** | **sCtl Table 26** | P3-04 T1-07/08 | **direct-with-enhanced-security** on **Wlim / WSd** only |
| Select / SBO | **N/A** | sCtl SBO | — | PFSP/VArV CID says SBO — **not claimed** |
| Time sync (SNTP client) | **O** or **M** | sTm 31–32 | P3-11 | Chrony + GNSS; Annex T ±100 ms. Prefer **M** if lab Time block is cheap |
| File transfer | **N/A** | sFt | — | CID has `<FileHandling />` — **do not claim** |
| Setting groups | **N/A** | sSg | — | |
| Substitution | **N/A** | sSub | — | |
| Service tracking LTRK | **N/A** | sTrk | — | |
| Sampled values | **N/A** | sSvp | — | |

---

## ACSI Model (pointer)

Copy LN list from [MICS_DRAFT.md](MICS_DRAFT.md). Mandatory LPHD / LLN0 present.

---

## How to paste into Excel

1. Open `lab/TemplatePics_Ed1Ed2Ed2p1_Excel_rev3p0.xlsx`.  
2. Fill **Cover** and **General** from tables above.  
3. On **ACSI Basic** / **ACSI Service**: mark **Y/M** only for **M** rows; **N** or blank for **N/A**.  
4. Remove or skip **Instructions** sheet before send.  
5. Filename: `PICS_CCI016_01_draft_YYYY-MM-DD.xlsx` — do not commit live firmware hashes.

**Quote scope sentence (email):**  
*Server-only IEC 61850-8-1 MMS; association; directory; GetDataValues; static datasets; unbuffered reporting (integrity 4 s + GI); direct-with-enhanced-security control on Wlim/WSd; time client. Not claimed: BRCB, GOOSE, SV, files, SG, client role.*

---

**RAG tags:** `lab`, `PICS`, `UCA`, `ccli-lab-pics-fill`

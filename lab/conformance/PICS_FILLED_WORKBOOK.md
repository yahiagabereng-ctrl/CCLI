# PICS filled workbook — canonical lab submit (2026-10-14)

**Document ID:** CCLI-LAB-PICS-WB-001  
**Revision:** 1.1  
**Date:** 2026-10-14  
**RAG source_id:** `ccli-lab-pics-filled-workbook`  
**Markdown values (planning):** `ccli-lab-pics-fill` · [PICS_FILL.md](PICS_FILL.md)  
**Specimen:** [LAB_SPECIMEN_VERSION.md](LAB_SPECIMEN_VERSION.md) · CID History **rev 9** · IED `CCI016_01`

---

## Canonical file (repo)

| Item | Value |
|------|--------|
| **Path** | `lab/conformance/drafts/PICS_CCI016_01_filled_rev3p0_AutoRecovered.xlsx` |
| **Source** | Desktop `TemplatePics_Ed1Ed2Ed2p1_Excel_rev3p0(AutoRecovered).xlsx` (2026-10-14) |
| **Template** | UCAIug PICS rev **3.0** (same lineage as `lab/TemplatePics_Ed1Ed2Ed2p1_Excel_rev3p0.xlsx`) |
| **SHA-256** | `CB2A684EC3ED159363725C6AB5284A30BC59E8A405523E88E26EA7485D3E2CD1` |

**Before accredited submit:** complete **Cover** (device name, firmware, release date); remove **Instructions** sheet; re-hash and update this doc.

---

## Workbook structure (verified)

Sheets: **Cover** · **General** · **ACSI Basic Conformance** · **ACSI Model Conformance** · **ACSI Service Conformance** · 61869-9 · 61850-9-3 · **Instructions** (drop before send).

---

## Alignment vs first-pass programme ([PICS_FILL.md](PICS_FILL.md) · [CID_PICS_ALIGNMENT.md](CID_PICS_ALIGNMENT.md))

Use this table in lab quote email and TSP scope. **Resolve or waive** before locking the specimen for UCA.

| Topic | First-pass intent | Filled workbook (observed) | Action |
|-------|-------------------|----------------------------|--------|
| **Cover** | HiTEKS / CCI016_01 / firmware | Placeholders `<device>` still present | **FIX** before send |
| **B11 Server** | **M** / Yes | Server `c1` (template conditional) | Confirm **Yes** on server column |
| **B12 Client** | **N/A** | Client column **M** on B12 row | **FIX** — server-only DUT |
| **B31 GOOSE publisher** | **N/A** | **O** | Trim to **N/A** or lab waiver + TSP GOOSE days |
| **Associate / Abort / Release** | **M** | S2–S4 largely **M** | OK |
| **Directory / GetDataValues** | **M** | Several marked **O** (S5–S8, S10–S11) | Raise to **M** or accept lab reads as in-scope |
| **Static datasets** | **M** Get/SetDataSetValues; Create/Delete **N/A** | S12–S16 mix **O** | Align Create/Delete to **O**/empty only |
| **Setting groups** | **N/A** | S44 **SelectActiveSG** = **M** | **FIX** — not in MVP / CID trim |
| **URCB / BRCB** | URCB **M**; BRCB **N/A** | **c6** cells on Report / URCB / BRCB | Map **c6** to **M** URCB only; BRCB **N/A** |
| **TrgOps dchg/qchg/dupd** (S27-1…3) | **M** server (expanded Table 14) | TrgOp rows **M** | **IN MATRIX** — [LAB_REQUIREMENT_TEST_MATRIX.md](LAB_REQUIREMENT_TEST_MATRIX.md) **T1-05b–d** · enable TrgOps via SetURCB before test |
| **Logging LCB** | **N/A** | Get/Query Log **M** | **FIX** — remove log claim or implement |
| **GOOSE services** | **N/A** | GetGo* **O** | Trim to **N/A** for first quote |
| **Control Operate** | **M** enhanced Wlim/WSd | **Operate** **M** | OK |
| **File transfer** | **N/A** | GetFile / FILE-SYSTEM directory **M** | **FIX** — conflicts with CID trim |
| **Time** | **O** or **M** | Check sheet row 120+ | Confirm vs P3-11 |

**Quote scope sentence (recommended after trim):**  
*Server-only IEC 61850-8-1 MMS on TLS :3782; association; directory; GetDataValues; static datasets; URCB (integrity 4 s + GI + TrgOps dchg/qchg/dupd per S27-1…3); direct-with-enhanced-security control on Wlim/WSd. Not claimed: BRCB, GOOSE, SV, files, SG, client role.*

---

## Requirements

| REQ-ID | Requirement | Status |
|--------|-------------|--------|
| REQ-PICS-WB-001 | One pinned xlsx SHA on disk for D1 submit | **HAVE** (this doc + `drafts/`) |
| REQ-PICS-WB-002 | Workbook ↔ [MICS_DRAFT.md](MICS_DRAFT.md) / same CID | **REVIEW** Model sheet |
| REQ-PICS-WB-003 | Workbook ↔ [LAB_REQUIREMENT_TEST_MATRIX.md](LAB_REQUIREMENT_TEST_MATRIX.md) **M** rows | **PART** — TrgOp rows **HAVE** (T1-05b–d); other gaps in alignment table |
| REQ-PICS-WB-004 | Separate **D7** 62351-3 PID | [D7_62351-3_PID_DRAFT.md](D7_62351-3_PID_DRAFT.md) |

---

## Verification

| Check | Pass |
|-------|------|
| File exists at `drafts/PICS_CCI016_01_filled_rev3p0_AutoRecovered.xlsx` | □ |
| Cover + firmware recorded | □ |
| Alignment table gaps closed or lab waivers written | □ |
| Instructions sheet removed in zip/email to lab | □ |
| TSP Compare uses same CID as Model sheet | □ |

---

## Maintenance

When replacing the xlsx from Desktop:

```powershell
Copy-Item -Force "$env:USERPROFILE\OneDrive\Desktop\TemplatePics_*.xlsx" `
  "lab\conformance\drafts\PICS_CCI016_01_filled_rev3p0_AutoRecovered.xlsx"
Get-FileHash "lab\conformance\drafts\PICS_CCI016_01_filled_rev3p0_AutoRecovered.xlsx" -Algorithm SHA256
```

Update SHA and revision in this file. Excel is **not** RAG-ingested; this markdown is (`ccli-lab-pics-filled-workbook`).

**Keywords:** `PICS`, `D1`, `UCA`, `filled workbook`, `AutoRecovered`, `ccli-lab-pics-filled-workbook`

# CID `<Services>` vs first-pass PICS (R-LAB-01)

**Document ID:** CCLI-LAB-CID-PICS-001  
**Revision:** 1.0  
**Date:** 2026-10-08  
**RAG source_id:** `ccli-lab-cid-pics-alignment`  
**61850-10:** Table 1 — ICD Services shall match PICS ACSI claims.

Working CID `lab_tg544_eth_a.cid` is a **TR 57-126 lab specimen** (P5-G GOOSE + BRCB present).  
**First-pass UCA quote does not claim** those blocks. Filling PICS from raw `<Services>` will pull extra test days.

---

## Working CID Services (as of rev 5) vs PICS

| CID `<Services>` | Working CID | First-pass PICS | Action |
|------------------|-------------|-----------------|--------|
| DynAssociation | present | **M** | Keep |
| GetDirectory / GetDataObjectDefinition / DataObjectDirectory | present | **M** | Keep |
| GetDataSetValue / DataSetDirectory | present | **M** | Keep |
| ConfDataSet modify=false | present | **N/A** create/delete | Keep; PICS static only |
| ReadWrite | present | Get **M**; Set **O** | Keep |
| ConfReportControl **bufMode="both"** | URCB + BRCB | URCB **M**; BRCB **N/A** | Cert freeze: `bufMode="unbuffered"` |
| GetCBValues | present | **M** (URCB) | Keep |
| ConfLogControl max=1 | present | Logging **N/A** | Cert freeze: **remove** |
| GOOSE max=5 | present | GOOSE **N/A** | Cert freeze: **remove** or max=0 |
| FileHandling | present | Files **N/A** | Cert freeze: **remove** |
| GSEDir / GSESettings | present | GOOSE **N/A** | Cert freeze: **remove** |
| ClientServices (bufReport, goose, …) | present | Client **N/A** | Cert freeze: **remove** (server DUT) |
| TimeSyncProt sntp=true | present | Time **O/M** | Keep |
| TimerActivatedControl | present | **N/A** | Cert freeze: **remove** unless implemented |
| SupSubscription maxGo=10 | present | GOOSE **N/A** | Cert freeze: maxGo=0 or remove |

Objects `brcb_*` / `gcb_*` may remain in the **lab** CID for internal P5-G.  
For the **certificate specimen**, either:

1. Submit a **trimmed Services** CID (recommended), or  
2. Keep objects and **explicitly N/A** in PICS **and** get lab written waiver (weaker).

---

## URCB trigger alignment

| Topic | CID today | Questionnaire old text | PIXIT / PICS now |
|-------|-----------|------------------------|------------------|
| `urcb_PdC_Mis4sec` TrgOps | **period + gi** | said dchg + GI | **period + GI only** — no dchg claim |

Do **not** add `dchg` unless expanding Table 14 optional cases.

---

## ctlModel alignment

| Object | CID | Old questionnaire §10 / 4.5 | Correct fill |
|--------|-----|------------------------------|--------------|
| Wlim / WSd | **direct-with-enhanced-security** | “direct-with-normal-security” | **enhanced** |
| PFSP / VArV / PFW | sbo-with-enhanced-security | — | **N/A** first pass |

---

## Cert-freeze CID (when P7 ship)

Do not silently edit the working lab CID (P5-G evidence). When freezing the DUT for UCA:

1. Copy `lab_tg544_eth_a.cid` → `lab_tg544_eth_a_cert.cid`.  
2. Apply Services edits in the table above.  
3. Point PICS Cover + TSP Compare Model at the **cert** file.  
4. Record `ccli --version` on that image.

---

**RAG tags:** `lab`, `PICS`, `CID`, `Services`, `ccli-lab-cid-pics-alignment`

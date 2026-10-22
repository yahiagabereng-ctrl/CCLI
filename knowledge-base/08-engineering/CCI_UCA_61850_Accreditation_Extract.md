# UCA IUG 61850 pack — what we need vs what we implement

**Document ID:** CCLI-UCA-61850-ACCRED-001  
**Revision:** 1.0  
**Date:** 2026-10-20  
**RAG source_id:** `ccli-uca-61850-accreditation-extract`  
**Raw drop:** `UCA 61850/UCA 61850/`  
**OCR/text:** `knowledge-base/08-engineering/reference/uca-iug/_extract/`  
**Parents:** `ccli-lab-uca-document-checklist` · `ccli-61850-10-extract` · `ccli-lab-pics-fill` · `ccli-dnv-product-readiness`

These files are **UCA International Users Group tester / lab accreditation** (how DNV or another Level A/B lab is allowed to issue certificates). They are **not** the IED Server Test Procedures and **not** a substitute for PICS/PIXIT/MICS.

HiTEKS does **not** implement this pack. We **book** an already-accredited lab. We **do** implement the server DUT + D1–D6 documents that the lab will audit against this pack.

---

## 1. Folder inventory (HAVE)

| File | Date / rev | Audience | CCLI use |
|------|------------|----------|----------|
| `Accreditation_Procedure V1.1.doc` | Tester QAP / Level **A** (ISO 9000/17025) vs **B**; Annex A agreement. Lineage 2004–2005. String-extract 2026-10-20 | UCA accreditors / labs | **N/A** to implement — lab QMS. Text: `reference/uca-iug/_extract/Accreditation_Procedure_V1.1_strings.txt` |
| `Accreditation_Procedure_For_Accreditors_rev1p0.pdf` | **2015-03-31** v1.0 | Accreditor checklist | Lists **vendor docs + traces** we must hand the lab |
| `HowToAuditTestersv1p0.pdf` | **2009-10-19** v1.0 | Audit task force | Explains PICS/MICS/PIXIT/TICS/SCL checks the lab applies to **us** |
| `IEC 61850 Accreditation Fee schedule.xlsx` | v1.4 | Lab quoting Ed2 / **Ed2.1** CBs | Maps **conformance blocks** we claim |

**MISSING (needed for the IED cert, not in this folder):**

| Need | Status in repo |
|------|----------------|
| UCA **Server Test Procedures Ed. 2.1** (current, e.g. v1.3 from 2024-07-22) | **MISSING** — we use TSP + 61850-10 extract |
| Official **PIXIT Word** template | **MISSING** — values in `PIXIT_DRAFT.md` |
| Filled PICS xlsx sent to lab | Draft **HAVE** — `PICS_FILL.md` / filled workbook |
| TICS dated to lab TISSUE list | **HOLD** — `TICS_DRAFT.md` empty |

Corpus: 8-1 extract is still **2004 Ed.1** (**PART**); cert target is **8-1:2011 Ed.2 + AMD1**.

---

## 2. Two different “UCA 61850” jobs

| Job | Who | This folder | HiTEKS action |
|-----|-----|-------------|---------------|
| **A. Tester accreditation** | DNV / IMQ / other Level A–B lab | **Yes** — these 4 files | Do **not** implement. Confirm the booked lab is UCA-listed for **Ed. 2.1 Server**. |
| **B. IED conformance certificate** (O.15 #5) | HiTEKS DUT `CCI016_01` | Indirect — lab uses A to check our pack | Implement **server + D1–D6**. Book job A lab. |

From Jan **2024**, UCA only issues **new** IED certificates after **Ed. 2.1** (Ed.2 + AMD1). Fee sheet already prices **Edition 2.1 Server**.

---

## 3. What the lab will demand from us (from the accreditor checklist)

Accreditor PDF §4–16 (2015) — vendor submission the **test lab** must have (we produce this):

| # | Artifact | Need | Implement / status |
|---|----------|------|--------------------|
| D1 | **PICS** (latest UCA Ed2/Ed2.1 template; cover says Edition 2) | **M** | **HAVE** draft — `PICS_FILL.md` · template `TemplatePics_Ed1Ed2Ed2p1_*.xlsx` |
| D2 | **PIXIT** (verdict-able values) | **M** | Values **HAVE** — Word template **MISSING** |
| D3 | **MICS** (Ed2: private LN/DO only) | **M** | **HAVE** draft — `MICS_DRAFT.md` · 5-LN CEI namespace |
| D4 | **TICS** (mandatory Green TISSUEs) | **M** at lab | **HOLD** |
| D5 | **CID/ICD/SCD** — schema valid, Services ≡ PICS, `nameStructure=IEDName` | **M** | **HAVE** `product_tr400_eth_a_5ln.cid` SCL **2007B4** · IED `CCI016_01` (not TEMPLATE — CID not ICD) |
| D6 | Lab config / firmware ID | **M** | **HAVE** `LAB_CONFIG_GUIDE.md` · `ccli --version` |
| — | **SICS** (Ed2 only) | If lab asks | **MISSING** |
| — | One **pcap/tcpdump per claimed test case** | **M** at accredited day | **PART** — TSP bundles T1–T10; not UCA TP case IDs |
| — | Draft certificate watermarked DRAFT | Lab writes | We do not write this |
| D7 | 62351-3 PID | O.15 #6 (separate) | Draft **HAVE** — not in this UCA folder |

HowToAudit (2009) still says SCL **2003** / schema 1.4 — **stale**. Our CID is **2007B4**. Lab must use **Ed. 2 / 2.1** SCL rules, not that 2003 bullet.

---

## 4. Conformance blocks — need vs implement

Block IDs from the **Accreditation Fee schedule** (same CB numbers the certificate page-2 uses). First-pass PICS (`ccli-lab-pics-fill`).

| CB | Name | Claim (need) | Implement on DUT | Bench / gap |
|----|------|--------------|------------------|-------------|
| **CB-01** | Basic Exchange (Associate, directory, GetDataValues) | **M** | **YES** — T1-01/03/04/09/10 | TLS :3782 |
| CB-01b | Associate IPv6 | **N/A** | No | — |
| **CB-02** | Data Sets (static Get) | **M** | **YES** — `DS_R_PdC_Mis4sec` | T1-05 |
| CB-02+ | DataSet create/delete | **N/A** | `ConfDataSet modify=false` | Do not claim |
| CB-03 | Substitution | **N/A** | No | — |
| CB-04 / 04+ | Setting groups | **N/A** | No | — |
| **CB-05** | Unbuffered reports | **M** | **YES** — URCB 4 s + GI + dchg/qchg/dupd | T1-05/06 · T1-05b–d |
| CB-06 / 06+ | Buffered reports | **N/A** | Code/CID may exist — **do not claim** | — |
| CB-07 | Logging | **N/A** | No | — |
| CB-08 | GSSE | **N/A** (deprecated on fee sheet) | No | — |
| CB-09a–c | GOOSE | **N/A** | Publisher exists in lab CID — **do not claim** | — |
| CB-10 / 11 | 9-1 / SV | **N/A** | No | — |
| CB-12a | Control DOns (direct normal) | **N/A** | — | — |
| CB-12b | Control SBOns | **N/A** | — | — |
| **CB-12c** | Control **DOes** (direct enhanced-security) | **M** | **YES** — Wlim / WSd Operate | T1-07/08 · coil not required for Operate PASS |
| CB-12d | Control SBOes | **N/A** | Not claimed | — |
| **CB-13** | Time sync (SNTP/chrony) | **O** prefer **M** if cheap | **PART** — chrony ±100 ms **HAVE**; GNSS **HAVE** when `gnss_start`; not 9-3 PTP | P3-11 |
| CB-13b | Time PTP | **N/A** | No | — |
| CB-14 | File transfer | **N/A** | CID may list FileHandling — **do not claim** | — |
| CB-15 | Service tracking | **N/A** | No | — |
| ICT / SCT 21–38 | Config tools | **N/A** | We are the IED, not an ICT/SCT | — |

**Quote sentence (unchanged):** server-only 8-1 MMS; association; directory; GetDataValues; static datasets; URCB; direct-with-enhanced-security on Wlim/WSd; time client. Not claimed: BRCB, GOOSE, SV, files, SG, client.

---

## 5. What we do **not** implement from this pack

- Become a UCA **tester** (Level A/B, ISO 17025/9001 61850 scope, quality plan, inspection plan).
- Pay the **accreditation overhead** in the xlsx (sample prepaid **USD 2200** + per-CB hours) — that is the **lab’s** UCA fee, not our IED test quote.
- 2009 audit rule “SCL 2003 schema 1.4”.
- Ed. 1 Server / Ed. 2-only new cert (policy ended **Jan 2024**).

---

## 6. Requirements (CCLI)

| ID | Requirement | Source | Status |
|----|-------------|--------|--------|
| REQ-UCA-DOC-001 | Submit PICS+PIXIT+MICS+TICS+CID matching Services | Accreditor §13–16 · 61850-10 §5.4 | **PART** (TICS HOLD, PIXIT Word MISSING) |
| REQ-UCA-DOC-002 | One network trace per claimed UCA test case | Accreditor §17 | **PART** (TSP pcaps, not TP IDs) |
| REQ-UCA-DOC-003 | Book **Ed. 2.1 Server** at a UCA-listed lab | Fee sheet · UCA policy 2024 | **OPEN** (quote) |
| REQ-UCA-IED-001 | Implement only claimed CBs 01, 02, 05, 12c, (13) | PICS first pass | **HAVE** on 5-LN DUT |
| REQ-UCA-IED-002 | Do not claim GOOSE/BRCB/File/SG even if CID leftovers exist | PICS · CID_PICS_ALIGNMENT | **HAVE** policy |
| REQ-UCA-LAB-001 | Do not treat this folder as DUT test procedures | This extract | **HAVE** |

---

## 7. Verification

| Requirement | Test | Pass |
|-------------|------|------|
| REQ-UCA-IED-001 | TSP T1-01…10 + T1-05b–d + T1-07/08 | Session 2026-10-16: T1-09 P3-05 **PASS**; T3-01 still open |
| REQ-UCA-DOC-001 | Lab intake checklist D1–D5 | PICS/MICS/CID **HAVE**; PIXIT Word + TICS **FAIL** until filled |
| REQ-UCA-DOC-003 | PO to UCA Level A lab Ed. 2.1 Server | Not booked |

**Keywords:** `UCA`, `Ed2.1`, `CB-01`, `CB-05`, `CB-12c`, `PICS`, `ccli-uca-61850-accreditation-extract`

# IEC 61850-10 — Conformance testing extract (Edition 2.1 / AMD1:2025)

**Document ID:** CCLI-61850-10-001  
**Revision:** 1.0  
**Date:** 2026-10-08  
**RAG source_id:** `ccli-61850-10-extract`  
**Full text:** `ccli-61850-10-ocr-corpus` → Telematry `reference/iec61850/IEC_61850-10_2025_full.txt`  
**Corpus (OCR):** `knowledge-base/08-engineering/reference/iec61850/_extract_61850-10/full.txt`  
**Source PDF:** `knowledge-base/08-engineering/reference/iec61850/IEC_61850-10-2025.pdf` (local; gitignored)  
**Standard identity:** **IEC 61850-10:2012+AMD1:2025** — *Communication networks and systems for power utility automation — Part 10: Conformance testing* (Edition **2.1**, 2025-07)

**Use:** DNV / UCA lab booking · HiTEKS CCI server conformance plan · Phase 7 P7-07 transport/MMS test matrix.

---

## 1. Scope (what Part 10 defines)

Part 10 specifies:

- Methods and **abstract test cases** for conformance testing of **client**, **server**, and **sampled-values** devices.
- Methods and abstract test cases for **engineering tools** (IED configurator, system configurator).
- **Performance** tests (latency, GOOSE performance, time sync accuracy).
- References **ISO/IEC 9646** conformance methodology.

**Explicitly out of scope in Part 10:** Role of test facilities for **certifying** results (market/UCA schemes define that). **Cyber security** device tests are **not** in Part 10 — they point to **IEC 62351-100-4** and **IEC 62351-100-6**.

**2025 amendment (intro summary):** Updated server/client/SV device procedures; added tool-related procedures; sampled-values procedures added/updated.

---

## 2. Documents you MUST submit before testing (§5.4.1)

When submitting a device (DUT), the vendor **shall** provide:

| Document | Purpose | HiTEKS CCI |
|----------|---------|------------|
| **PICS** | Protocol capabilities; selects test suite (61850-7-2 Annex A proforma) | **TODO** — derive from `ccli` MMS feature set |
| **PIXIT** | Extra test parameters (timeouts, limits) — **not standardized** | **TODO** — lab values (IntgPd 4000 ms, port 3782, etc.) |
| **MICS** | Data model in **ICD/IID** (61850-6) | **HAVE** — lab CID/ICD path `apps/ccli/config/icd/` |
| **TICS** | Known technical issues implemented | Minimal / empty if none |
| **Manuals** | Install/operate device | Partial (lab yaml + deploy docs) |
| **DUT ready for test** | Frozen hardware/firmware | TG544 + `ccli` binary hash |

**Conformance process (Figure 1):** Static + dynamic requirements → parameterized test suite from PICS/PIXIT → basic interconnection → capability → behaviour tests → final review → **test report**.

**SCL rule (§5.4.2):** DUT shall be delivered with an **ICD**. Test entity generates **SCD** from ICD for the test system (or initiator provides SCD + SSD).

---

## 3. Server device test architecture (§6.2.4.2)

Minimum bench for **server** conformance:

```text
  [Client simulator] ----MMS/TPAA----> [DUT Server]
  [GOOSE simulator] -----> network ---->
  [SV simulator]    -----> network ---->
  [Time master]     -----> DUT
  [Engineering tool] --> configure DUT
  [Protocol analyzer] on all test cases
  [Signal generator] -> binary/analog stimuli (if applicable)
```

**Note (§6.1):** Verification of **functional applications** (e.g. “does GOOSE do the right plant action”) is **not** part of conformance test — only protocol/data-model behaviour.

**HiTEKS CCI:** Primary DSO path is **MMS server** (Eth_A). GOOSE publish exists in lab (P5) — include in PICS only if claimed. **No SV** on CCI for Annex T MVP unless claimed.

---

## 4. Server test procedure structure (§6.2.4)

Tests are grouped in **tables** (abstract test cases). Detailed procedures follow Annex A template (Figure 2: purpose, references, steps, expected behaviour, result).

### 4.1 Documentation & version (Table 1)

| ID | Shall verify |
|----|----------------|
| sDoc1+ | PICS/PIXIT/MICS/TICS **software version** matches DUT |
| | PICS includes **ACSI conformance** (61850-7-2 Annex A) |
| | ICD **services** section matches PICS ACSI claims |

### 4.2 Configuration file / SCL (Table 2 — large)

Key themes (many case IDs sCnf* / sCni*):

- ICD validates against **SCL schema** 2007 **revision B, release 4**; UTF-8; `version="2007"`.
- **Communication** section: OSI selectors (PSEL/SSEL/TSEL), IP, AP-Title, AE-Qualifier when DynAssociation.
- **DataSet / ReportControl / GSEControl** limits vs `Services` caps.
- **FCDA** references valid; naming lengths per 7-2 §22.2.
- **ctlModel** / Oper / SBO ProtNs for controllable DOs (61850-8-1).
- Import/export ICD → SCD workflow with **ICT/SCT** tools.

**HiTEKS:** Align lab ICD with online model exposed on MMS; fix `namPlt` / `configRev` / LPHD presence per table.

### 4.3 Data model (Table 3)

| Theme | Pass criteria |
|-------|----------------|
| Mandatory/conditional CDCs | All required LN/DO/DA present per 7-3/7-4 |
| Types & order | Match 7-3 attribute order; SCSM mapping |
| Extensions | Only per 7-1 §14 rules |
| Online vs SCL | Configured parameters match live browse |
| Trigger options | dchg/qchg/dupd on datasets match 7-2 |
| LPHD, LTRK, LGOS/LSVS | When claimed in Services |

### 4.4 ACSI service groups (§6.2.4.6)

Each group has **positive** and **negative** tables:

| Prefix | Model | Tables (server) | CCI priority |
|--------|--------|-----------------|--------------|
| **sAss** | Application association | 4–5, 44–45 | **P0** — MMS associate/release |
| **sSrv** | Server / LD / LN / Data | 6–7, negative Get/Set | **P0** — browse, GetDataValues, SetDataValues (Wlim) |
| **sDs** | Data set | 8–9, 43–44 | **P1** — DS_R_PdC_Mis4sec |
| **sTrk** | Service tracking (LTRK) | 10, 45 | P2 if LTRK in model |
| **sSub** | Substitution | 11 | P3 |
| **sSg** | Setting groups | 12–13, 47–48 | P3 unless SG in product |
| **sRp** | **Unbuffered reporting** | **14–15**, 49–50 | **P0** — 4 s TotW urcb |
| **sBr** | Buffered reporting | 16–17, 53–54 | P1 if brcb claimed |
| **sLog** | Logging | 18–19 | P2 |
| **sGop/sGos** | GOOSE publish/subscribe | 20–25, 55 | P1 if GOOSE in PICS |
| **sCtl** | Control | **26–29**, 56–60 | **P0** — Wlim/WSd operate paths |
| **sTm** | Time sync | **31–32**, 61–62 | **P1** — UTC ±100 ms (Annex T) |
| **sFt** | File transfer | 33–34, 63–65 | P3 |
| **sSvp/sSvs** | Sampled values | 35–36, 67–68 | N/A unless SV claimed |

### 4.5 Reporting (Table 14 — CCI-critical excerpt)

Unbuffered reporting **shall** verify:

- GetLogicalNodeDirectory(**URCB**), GetURCBValues.
- Optional fields: SeqNum, time stamp, reason, data-set name, data-reference.
- Trigger options: integrity period, dchg, qchg, dupd combinations.
- **General interrogation (GI)** behaviour.
- **Segmentation** (SubSeqNum, MoreSegmentsFollow).
- **ConfRev** on dataset changes.

**Lab mapping:** P3-03 `urcb_PdC_Mis4sec` IntgPd=**4000** ms — maps directly to integrity-trigger reporting tests.

### 4.6 Control (Table 26 — CCI-critical)

Control model tests cover **Direct**, **SBO**, enhanced security variants (sCtl*, sSBO*, sDON*, sSBON*, sDO* cases in standard).

**HiTEKS:** Wlim/WSd via MX/CO/SBO paths — must declare **ctlModel** in ICD and pass corresponding Table 26 cases for claimed model.

### 4.7 Association negative tests (Table 5 / 7)

Wrong object names, wrong LD/LN, service errors on Get/Set — ensures server rejects invalid MMS requests cleanly.

---

## 5. Cyber security (§6.3)

> Cyber security testing of IEC 61850 devices is defined in **IEC 62351-100-4** and **IEC 62351-100-6**.

**Implication for DNV meeting:**

| Topic | Standard | HiTEKS |
|-------|----------|--------|
| MMS/TLS/E2E on :3782 | **62351-100-3** (transport) + **100-4** (MMS security) | Lab PEMs + TSP; firmware session layer |
| GOOSE/SV security | **62351-100-6** | Defer unless GOOSE security claimed |

61850-10 **does not** replace UCA/accredited **62351-100-*** test plans.

---

## 6. Tool conformance (Clause 7)

For **IED configurator** and **system configurator** tools (Tables 75–89+): SCL import/export, SCD/SED/ICD/IID handling, communication engineering, GOOSE/SV data flows.

**HiTEKS:** Out of scope for **product** cert unless shipping a certified configurator — engineering is yaml + ICD in repo.

---

## 7. Performance tests (Clause 8)

| Section | Content | CCI |
|---------|---------|-----|
| 8.2 | Communications **latency** | Optional benchmark |
| 8.2.3 | **GOOSE performance** | If GOOSE claimed |
| 8.3 | **Time synchronisation accuracy** | **Annex T** UTC ±100 ms — link to chrony/GNSS lab |

---

## 8. Test report contents (§5.5)

Report shall include: standards cited by clause, test equipment, vendor/initiator, DUT variants, test facility, date, tester signature, **unique reference**, list of test items, per-item **pass/fail/N/A/not tested**, deviations and **re-test** rules after firmware changes.

---

## 9. HiTEKS CCI — minimum server scope for first DNV/UCA pass

Based on lab evidence (P3) and CEI Annex T DSO interface:

| Priority | 61850-10 area | Lab evidence | Gap |
|----------|---------------|--------------|-----|
| **P0** | Table 1 documentation | Partial | Formal PICS/PIXIT/MICS pack |
| **P0** | Table 2 SCL/ICD | CID exists | Full schema validation + ICT round-trip |
| **P0** | sAss association | MMS connect | TLS :3782 + 62351 separate |
| **P0** | sSrv browse/read | Client browse PASS | UCA formal cases |
| **P0** | sRp urcb 4 s | TotW 3961 ms avg | Table 14 full optional fields |
| **P0** | sCtl Wlim/WSd | Wlim lab PASS | Table 26 ctlModel cases |
| **P1** | sTm time | chrony PASS | Table 31–32 accuracy |
| **P1** | sGop GOOSE | r29 publish | Only if in PICS |
| **P2** | sBr, sLog, sFt | Not focus | N/A in PICS |
| **—** | §6.3 Cyber | — | **62351-100-4/6**, not 61850-10 |

---

## 10. DNV Netherlands — questions mapped to Part 10

Full questionnaire with "why / our position / lab answer" columns: **`lab/meetings/2026-10-08_Lab_Conformance_Questionnaire.md`** (`ccli-lab-conformance-questionnaire`). Part-10-specific items:

### 10.1 Scope and edition

1. Which **61850-10 edition** does the procedure follow — **Ed. 2.1 (2025-07)** or Ed. 2 (2012)? Which **UCA server procedure version** (v2.0.6 or later)?  
2. Which **UCA conformance blocks** are mandatory for a **server-only** DUT? Can Buffered Reporting, Logging, File Transfer, Setting Groups, SV be excluded?  
3. Is **GOOSE publisher** testing required if not needed by the DSO interface? Cost of claiming it later?  
4. Is the deliverable a **UCA IUG certificate** (level A) or a test report — which does the Italian DSO accept under Annex O §12?

### 10.2 Documents (§5.4)

5. **PICS** template: 7-2 Annex A proforma, UCA template, or lab form?  
6. **PIXIT** mandatory items for us: association timeout, max clients, IntgPd range, segmentation, ctlModel, port **3782**?  
7. **MICS** = ICD alone, or separate document? **TICS** against which TISSUES date?  
8. **ICD→SCD**: lab generates SCD, or HiTEKS delivers SCD + SSD? Which ICT/SCT tools?  
9. How is **DUT version** verified (firmware hash / build ID) and what appears on the certificate?

### 10.3 Bench (§6.2.4.2)

10. Client simulator / test tool used — can HiTEKS obtain it for **pre-testing**?  
11. **Time master** protocol (SNTP / PTP) and accuracy class verified for the Time Sync block.  
12. Physical I/O stimulus required, or software-driven data changes acceptable?  
13. Production configuration mandatory, or is a lab mode acceptable?  
14. Ports: 102 plain for 61850-10 and 3782 TLS for 62351 — both in one session?

### 10.4 Model / reporting / control (Tables 2, 3, 14, 26)

15. SCL schema revision for ICD validation (2007B rel 4 / 5)? Private namespaces tolerated?  
16. **Table 14** URCB: all optional fields and trigger options, or only PIXIT-enabled ones? Is **BRCB** required for DSO-facing CCI?  
17. **Table 26**: which ctlModel values will be tested for Wlim / WSd?  
18. Segmentation dataset size; static vs dynamic datasets; strict LPHD/LLN0 checks?

### 10.5 Process

19. 61850-10 session combined with **62351-100-3 / 100-4** in one visit?  
20. Lead time, days on site, pricing per block, re-test rules after firmware change (Table 1 sDoc / TICS), certificate validity (single specimen vs product type).

---

## 11. Extraction quality note

Source PDF is a **doc88-hosted scan** (196 pages). Text recovered via **OCR** (`scripts/extract-61850-10.py`). Test case IDs and table numbers are authoritative; verify ambiguous OCR strings against purchased IEC PDF before legal submission.

**Regenerate corpus:**

```powershell
python scripts/extract-61850-10.py
```

---

## 12. Related corpus

| Doc | RAG id |
|-----|--------|
| Certificate samples (DNV UCA example) | `ccli-certificate-samples-extract` |
| Phase checklists P7-07 | `ccli-phase-regulation-checklists` |
| Annex T cyber | `cei-0-16-allegato-t` |
| 62351-3/4/9 extracts | `ccli-62351-*-extract` |

---

**RAG tags:** `61850-10`, `conformance`, `PICS`, `PIXIT`, `MICS`, `Table 14`, `Table 26`, `sRp`, `sCtl`, `sAss`, `DNV`, `UCA`, `ccli-61850-10-extract`, `ccli-61850-10-ocr-corpus`

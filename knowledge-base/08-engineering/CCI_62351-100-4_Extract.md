# IEC TS 62351-100-4 — MMS / 62351-4 conformance test extract (planning)

**Document ID:** CCLI-SEC-62351-100-4-001  
**Revision:** 1.1  
**Date:** 2026-10-14  
**Capture status:** **HAVE (OCR)** — PDF ingested 2026-10-14; **PART** table-level extract (expand from corpus)  
**RAG source_id:** `ccli-62351-100-4-extract`  
**OCR RAG id:** `ccli-62351-100-4-ocr-corpus` · index `ccli-62351-100-4-ocr-index`  
**Corpus (OCR):** `reference/iec62351/_extract_62351-100-4/full.txt` (115 pp., `--force-ocr`)  
**Source PDF:** `reference/iec62351/IEC_TS_62351-100-4-2023.pdf` (local copy from user Desktop; **gitignored** — doc88 export; prefer **licensed IEC** for legal submission)  
**Standard identity:** **IEC TS 62351-100-4:2023** — *Conformance test cases for **IEC 62351-4*** (MMS / OSI and XMPP environments).  

**Normative profile (HAVE):** `ccli-62351-4-extract` — **62351-4:2018+AMD1:2020**  
**Transport companion:** `ccli-62351-100-3-extract` — TLS cases for **62351-3** (Allegato **O §12** names **100-3** explicitly)  
**CEI grid parents:** `cei-0-16-allegato-o` · `cei-0-16-allegato-t-extract`  
**Other parents:** `ccli-61850-10-extract` (§6.3 cyber pointer) · `ccli-62351-9-extract` · `ccli-62351-3-extract` · `ccli-product-sw-freeze` (product MMS profile)  
**Programme:** Allegato **T** E2E / dual-cert claims · lab Q **7.3** · TSP **P3-07** / T3-05 · **separate PID** from D7 (62351-3)

---

## 1. What this document is (vs 62351-4 and CEI Allegato O/T)

| Document | Role |
|----------|------|
| **CEI 0-16 Allegato O** | §12 mandates accredited **62351-100-3** for **secure transport**. Does **not** name **100-4** in the extract — MMS/E2E formal suite is **optional / lab-dependent** unless product cert claims it |
| **CEI 0-16 Allegato T** | **T.3.3.4** — **E2E security mandatory** per **IEC 62351-4** (association + data-transfer); dual-cert / ACSE / SecPDU logging — *what* CCI must implement on DSO MMS |
| **IEC 62351-4** | **Requirements** — T-profile TLS + A-profile (§11) or **native E2E (§13)**; Annex **G** PKI (separate TLS vs E2E certs) |
| **IEC TS 62351-100-4** | **Abstract test cases** — how a lab verifies **62351-4** on a DUT (MMS security). Exact Tables TBD when PDF ingested |
| **IEC TS 62351-100-3** | **Transport only** (TLS / 62351-3) — **not** a substitute for 100-4 |
| **IEC 61850-10** | ACSI/server conformance; §6.3 points cyber device tests to **62351-100-4** and **100-6** (plus transport via **100-3**) |

```text
 Allegato O §12          Allegato T.3.3.4
      │                        │
      ▼                        ▼
 62351-100-3 (TLS)      62351-4 requirements
      │                 (A-profile and/or E2E)
      │                        │
      └──────────┬─────────────┘
                 ▼
         62351-100-4 (when lab runs it)
         = formal MMS security cases
```

**HiTEKS CCI (current lab claim):** MMS **server** on Eth_A **:3782** with **TLS (Cert A)** + **A-profile ACSE auth (Cert B)** + Annex **G.2** dual cert. Full **native §13** Clear/EncrTransfer E2E is a **later** product claim — do not mark full 62351-4 **M** in PID until implemented and 100-4 (or lab procedure) is scoped.

---

## 2. CEI Allegato O — relation to 100-4

From `CCI_Annex_O_Extract.md` §12:

| Test category | Named standard | Implication for 100-4 |
|---------------|----------------|------------------------|
| IEC 61850 conformance | UCA-accredited lab | **61850-10** — functional |
| **Secure transport (62351-3)** | **IEC 62351-100-3** by accredited body | **Mandatory** product path |
| MMS / E2E (62351-4) | *Not named as 100-4 in O §12 extract* | Driven by **Allegato T** + market/lab offer |

**Rule for quotes:** Book **100-3** for O §12. Ask lab (questionnaire **1.6** / **7.3**) whether they also execute **100-4** in the same visit. Prefer one lab for both when available.

---

## 3. CEI Allegato T — profile that 100-4 would exercise

Allegato **T** requires E2E per **62351-4**. Mapping to expected 100-4 themes (until PDF tables are available, use **62351-4** REQs + lab TSP as pre-evidence):

| Allegato T / 62351-4 topic | Clause | Product / lab claim | Formal 100-4 |
|----------------------------|--------|---------------------|--------------|
| E2E association + data transfer | T.3.3.4 · 62351-4 §11 / §13 | A-profile auth **PASS** (TSP Connect); native §13 **PART / later** | Lab cases when offered |
| Dual end-entity certs | Annex **G.2** | Cert A TLS ≠ Cert B MMS | Artefact + Connect |
| Cert B keyUsage | **G.5.2** | digitalSignature + keyAgreement | `verify_annex_pki.py` |
| Cert size / RSA-2048 / SHA-256 | **G.3–G.4** · T.3.3.4.1 | Lab PEMs | Pre-lab verify |
| Subject / AP-title binding | **G.6.2** | UTF8 OID string lab practice | Strict OID tag optional |
| AARQ/AARE auth value | §**11.2.2** | Cert + GT + RSA-SHA256 · raw GT sign | **P3-07 PASS** 2026-10-08 |
| Port under TLS | §6.3 · T | **3782** (with 100-3) | Same session as TLS |
| Security events / MMS handshake fail | T.3.3.4.6 | DUT log + event store | Lab logging check |
| RBAC roles | T Tables 97–102 · 62351-8 | viewer deny Operate | TSP T3-02/03 (not 100-4 body alone) |
| Native Clear/EncrTransfer | §13 | Not claimed in first PID | Defer until firmware + 100-4 PDF |

**Allegato O** functional clauses (O.8 / O.9 / O.14) remain **61850-10** / TSP — not 100-4.

---

## 4. Documents to submit (expected PID — separate from D7)

| Item | Content | CCLI status (2026-10-13) |
|------|---------|--------------------------|
| **62351-4 PICS** | Mode (compatibility A-profile vs native E2E), crypto tables | Derive from `ccli-62351-4-extract` §17 — **do not** mark full native **M** yet |
| **PIXIT** | Dual-cert paths, AP-title/AE-qual, time skew, mechanism OID | PART — TSP session start + EJBCA pack |
| **62351-3 PID (D7)** | Transport | **HAVE** draft — **prerequisite** for secure MMS, not a substitute |
| **DUT** | Firmware with ACSE auth + dual cert | Connect **PASS** · formal 100-4 **OPEN** |

D7 note (`D7_62351-3_PID_DRAFT.md`): **62351-4 / 100-4** is a **separate** PID.

---

## 5. Requirements (HiTEKS trace — planning)

| REQ-ID | Source | Requirement | Verification |
|--------|--------|-------------|--------------|
| **REQ-1004-PDF-001** | Capture | Ingest licensed **62351-100-4** PDF + OCR when available | New `_extract_62351-100-4/` |
| **REQ-1004-LAB-001** | Questionnaire 7.3 | Confirm lab executes 100-4 (or national scheme) | Meeting answer |
| **REQ-1004-CEI-T-001** | Allegato **T.3.3.4** | Product implements 62351-4 association security | TSP P3-07 / T3-05 |
| **REQ-1004-G2-001** | Annex G.2 | Separate TLS vs E2E end-entity certs | PEM serial/key differ |
| **REQ-1004-APROF-001** | §11.2.2 | AARQ/AARE auth value accepted | Connect Initiate Success |
| **REQ-1004-TLS-001** | 100-3 + §6.3 | MMS only on **:3782** with mutual TLS | T1-01 |
| **REQ-1004-SCOPE-001** | Product policy | First formal claim = **A-profile + TLS**; native §13 later | PID wording |
| **REQ-1004-O-001** | Allegato **O §12** | Do **not** treat 100-4 as replacing **100-3** transport cert | Separate quotes |

---

## 6. Interface matrix (lab)

| Interface | Source | Destination | Protocol |
|-----------|--------|-------------|----------|
| Eth_A DSO | TSP / lab client | CCI TG544 server | TLS **62351-3** (:3782) + MMS **62351-4** ACSE |
| Cert A | EJBCA / product TLS EE | TLS handshake | X.509 |
| Cert B | EJBCA / product MMS EE | ACSE authentication-value | X.509 + sig |
| Evidence | DUT log + pcap | Lab report | AARE / SecPDU diagnostics |

---

## 7. Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| **IEC TS 62351-100-4 PDF** | OCR corpus **HAVE** | Licensed PDF + table extract batches | **PART** — formal case IDs from OCR; verify vs IEC shop copy |
| **Lab execution** | Q7.3 unknown | Yes/no + procedure edition | Ask TUV / DNV |
| **Allegato O naming** | Only **100-3** in §12 | Whether DSO also expects 100-4 report | Confirm with market |
| **Native §13 E2E** | Not in first PID | Firmware + cases | Later phase |
| **A-profile dual-cert** | TSP **PASS** | Formal 100-4 (or lab) cases | Pre-evidence HAVE |

---

## 8. Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-1004-01 | Claim “62351-4 certified” without 100-4 / lab procedure | False market claim | Scope PID to A-profile + evidence only |
| R-1004-02 | Skip **100-3** because dual-cert Connect works | Allegato **O §12** fail | Always book transport cert |
| R-1004-03 | Confuse 61850-10 cyber pointer with free 100-4 | Wrong quote | Ask lab 7.3 explicitly |
| R-1004-04 | Use doc88 OCR as sole case list | Wrong IDs / chrome noise | Cross-check tables vs **licensed IEC PDF** before lab submit |
| R-1004-05 | Mark native E2E **M** in PICS early | Lab fail days | First pass A-profile only |

---

## 9. Verification (internal pre-lab — until 100-4 PDF)

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-1004-TLS-001 | TSP T1-01 | Mutual TLS Connect `:3782` |
| REQ-1004-G2-001 | PEM / `verify_annex_pki.py` | Cert A ≠ Cert B keys; KU on B |
| REQ-1004-APROF-001 | T3-05 / Connect | AARE accept · Initiate Success |
| REQ-1004-CEI-T-001 | Session logs | Auth cert = MMS Security client.pem |
| REQ-1004-LAB-001 | Questionnaire | Written lab answer on 100-4 availability |

Canonical TSP rows: `lab/conformance/LAB_REQUIREMENT_TEST_MATRIX.md` (P3-07 · T3-05) · evidence `TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08.md`.

---

## 10. Lab / TUV questions (100-4)

From `lab/meetings/2026-10-08_Lab_Conformance_Questionnaire.md` §7 — keep asking:

1. Is **IEC TS 62351-100-4** published and **executed** by you, or still draft / not offered?  
2. If not 100-4, which **national scheme** (if any) covers **62351-4** MMS security?  
3. Same visit as **61850-10** + **62351-100-3** on one firmware build?  
4. Which mode: **A-profile (§11)** only, **native E2E (§13)**, or both?  
5. Dual-cert (Annex G.2) mandatory in your procedure?  
6. PID template separate from 62351-3 D7?  
7. Certificate wording acceptable under Italian DSO / Allegato **T** E2E language?

---

## 11. Related corpus

| Doc | RAG / path | Role |
|-----|------------|------|
| Allegato O | `cei-0-16-allegato-o` | §12 → **100-3** mandatory; 100-4 not named |
| Allegato T | `cei-0-16-allegato-t-extract` | E2E / cyber profile → 62351-4 |
| 62351-4 requirements | `ccli-62351-4-extract` | Normative profile under test |
| 62351-100-3 | `ccli-62351-100-3-extract` | Transport companion |
| 61850-10 | `ccli-61850-10-extract` | §6.3 points to 100-4/6 |
| D7 (62351-3 only) | `lab/conformance/D7_62351-3_PID_DRAFT.md` | Separate from 100-4 PID |
| Lab matrix | `ccli-lab-requirement-test-matrix` | P3-07 / T3-05 pre-evidence |
| Questionnaire | `ccli-lab-conformance-questionnaire` | §1.6 · §7 |

---

## 12. Ingest / RAG (2026-10-14)

```powershell
# PDF at reference/iec62351/IEC_TS_62351-100-4-2023.pdf (copy from Desktop if missing)
python scripts/extract-62351-100-4.py --force-ocr
powershell -File scripts/ingest-62351-100-4.ps1
```

**RAG ids:** `ccli-62351-100-4-extract` · `ccli-62351-100-4-ocr-corpus` · `CCI_62351-100-4_OCR_Corpus.md`

Next engineering pass: batch-extract **Table 2 PIXIT** and **Tables 3–4** conformance rows into this extract (mirror `ccli-62351-100-3-extract`).

---

**RAG tags:** `62351-100-4`, `62351-4`, `MMS`, `ACSE`, `E2E`, `A-profile`, `Annex-G`, `dual-cert`, `Allegato-O`, `Allegato-T`, `CEI-0-16`, `P3-07`, `OCR`, `ccli-62351-100-4-extract`, `ccli-62351-100-4-ocr-corpus`

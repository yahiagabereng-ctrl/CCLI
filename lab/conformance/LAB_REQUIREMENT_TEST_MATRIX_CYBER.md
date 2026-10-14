# Lab requirement ↔ cybersecurity test matrix (62351 / Annex T transport)

**Document ID:** CCLI-LAB-REQ-TEST-MTX-CYB-001  
**Revision:** 1.0  
**Date:** 2026-10-14  
**RAG source_id:** `ccli-lab-requirement-test-matrix-cyber`  
**Purpose:** Checklist for **62351-3 transport (D7)**, **62351-4 / 100-4 MMS security**, **62351-8 RBAC**, and **product cyber profile** — separate from **61850-10 functional** tests in [LAB_REQUIREMENT_TEST_MATRIX.md](LAB_REQUIREMENT_TEST_MATRIX.md).  
**Field pack:** `scripts/stage-tsp-field-pack.ps1` → `01b_CHECKLIST_CYBER_REQUIREMENT_TEST_MATRIX.md`

**Companion PID:** [D7_62351-3_PID_DRAFT.md](D7_62351-3_PID_DRAFT.md) (**62351-3 only** — not 61850 D1 PICS).  
**62351-4 / 100-4:** separate PID — `ccli-62351-100-4-extract` · `ccli-62351-4-extract`.

**Normative RAG corpus:**

| source_id | Use |
|-----------|-----|
| `ccli-lab-d7-62351-3-pid` | D7 PICS §8 + TLS PIXIT |
| `ccli-62351-100-3-extract` | Accredited **100-3** Tables 2–3 themes |
| `ccli-62351-100-4-extract` · `ccli-62351-100-4-ocr-corpus` | **100-4** case planning |
| `ccli-62351-3-extract` · `ccli-62351-4-extract` · `ccli-62351-9-extract` | Requirements behind PID |
| `ccli-62351-mms-security-debug-map` | DUT log / config ↔ clause map |
| `ccli-tsp-test-identification` | Tier 3 step detail (T3-01…T3-05) |
| `cei-0-16-allegato-o-extract` | O §12 → **100-3** mandatory |
| `cei-0-16-allegato-t-extract` | T.3.3.4 TLS + E2E |
| `ccli-product-sw-freeze` | Product :3782 · no cleartext :102 |

**Scope decision (programme):** **62351-100-3** accredited report = gate **P7-07** (**M** for O §12). **62351-100-4** formal suite = **ask lab** (questionnaire 7.3) — Track C formal column **N/A** until lab confirms same visit.

---

## How to use

1. Run functional matrix **Tier 1** connect (T1-01) — cyber Track A reuses that TLS session where noted.  
2. Tick **□** in Track A (TSP Tier 3) before claiming transport ready for accredited PID submit.  
3. Complete Track B evidence (openssl / PKI scripts) for D7 paste.  
4. Track C internal rows = pre-test for **62351-4**; formal **100-4** only when lab executes TS.  
5. Product rows (Track D) on **product yaml** before `releases/product-cyber/` promote.

**Security pack exit (internal):** T3-01, T3-02, T3-03 **PASS**; T3-04 **PASS** if `tls_crl` loaded on DUT; T3-05 / P3-07 **PASS** for ACSE.

---

## §1 Programme requirements

| REQ-ID | Source | Requirement | Matrix track |
|--------|--------|-------------|--------------|
| REQ-CYB-SCOPE-001 | D7 | D7 = **62351-3 transport** only; ACSE in **62351-4 PID** | B |
| REQ-CYB-O-001 | Allegato **O §12** | Accredited **62351-100-3** for secure transport claim | E · **P7-07** |
| REQ-CYB-T-001 | Allegato **T.3.3.4** | MMS E2E per **62351-4** (A-profile + dual cert G.2 first pass) | A · C |
| REQ-CYB-1004-O-001 | Programme | **100-4** does **not** replace **100-3** | B vs C |
| REQ-PROD-SW-002 | Product freeze | Eth_A **3782** TLS only in product bundle | D |
| REQ-PROD-SW-007 | Product freeze | No cleartext TSP yaml in product release | D |

---

## §2 Track A — Internal TSP (Tier 3 + TLS base)

| □ | Cyber need | Standard | PID / PIXIT | CEI | Gate | TSP ID | Executor | Expect | Evidence | Debug map |
|---|------------|----------|-------------|-----|------|--------|----------|--------|----------|-----------|
| □ | Mutual TLS on secure port | 62351-3 §5.7 · 100-3 | D7 · port **3782** | T.3.3 | P3-06 | **T1-01** | TSP Connect | TLS 1.2 mutual · Initiate Success | `TSP_P3_01_CONNECT_*` | REQ-DBG-TLS-001 |
| □ | Application context MMS | 100-3 §4.3.1 | D7 PIXIT | T | P3-06 | T1-01 | TSP | 61850-8-1 over TLS | same | REQ-1003-APP-001 |
| □ | No client certificate | 62351-3 §5.6 | D7 mutual **m** | T security | P3-06 | **T3-01** | TSP | Handshake / assoc **fail** | `TSP_P3_06_TLS_FAIL_*` | REQ-D7-MUT-001 |
| □ | DSO profile ACSE accept | 62351-4 §11.2.2 | 62351-4 PID | T G.2 | **P3-07** | **T3-05** | TSP + DUT log | AARE auth OK | log + System Status | REQ-DBG-AARE-001 |
| □ | Dual cert (A ≠ B) | 62351-4 G.2 | PIXIT paths | T | P3-07 | T3-05 | PKI compare | TLS EE ≠ ACSE EE serial | `verify_annex_pki.py` | REQ-DBG-DUAL-001 |
| □ | Viewer read allowed | 62351-8 · T Tables 97–102 | — | T RBAC | P3-08 | **T3-02** | TSP | Browse/read OK | `TSP_P3_08_VIEWER_READ_*` | REQ-DBG-RBAC-001 |
| □ | Viewer Operate denied | 62351-8 | — | T | P3-08 | **T3-03** | TSP | Operate Wlim **deny** | same | REQ-DBG-RBAC-001 |
| □ | Revoked cert + CRL | 62351-3 §5.6.4.4 · 62351-9 | D7 Table 4 CRL **m** | T CRL | P3-09 | **T3-04** | TSP | Fail establish | `TSP_P3_09_CRL_*` | REQ-DBG-CRL-001 |
| □ | Optional wire capture | — | — | — | P3-07 | T3-05 | pcap | MMS after TLS | `pcap/TSP_P3_07_mms_*.pcapng` | `capture-mms-tsp.ps1` |

Detail: [TSP_TEST_IDENTIFICATION_POST_CONNECT.md](../evidence/testsuite-pro/TSP_TEST_IDENTIFICATION_POST_CONNECT.md) §5.

---

## §3 Track B — D7 / 62351-3 PICS → internal verification

| □ | REQ-ID | Test | Pass criteria | Evidence |
|---|--------|------|---------------|----------|
| □ | REQ-D7-ED-001 | Wire / stack config | **TLS 1.2** minimum | openssl s_client or TSP |
| □ | REQ-D7-PORT-001 | Port audit | **3782** secure; **102** lab-only | yaml + nmap |
| □ | REQ-D7-MUT-001 | T3-01 | No client cert → fail | T3-01 log |
| □ | REQ-D7-CRYPTO-001 | PKI verify | RSA ≥2048 · SHA-256 · cert ≤8192 B | `verify_annex_pki.py` |
| □ | REQ-D7-CRL-001 | T3-04 (CRL on) | Revoked → fail | T3-04 log |
| □ | REQ-D7-OCSP-001 | — | **Not claimed** (x) | N/A |
| □ | REQ-D7-REN-001 | Soak / PIXIT | Resumption/reneg intervals **PART** — agree with lab | PIXIT note in D7 |
| □ | REQ-D7-1003-001 | Accredited body | Tables 4–5 complete | **P7-07** report ID |
| □ | D7 Tables 1–5 | Submit snapshot | Cipher IANA list from **frozen** firmware | DUT `openssl ciphers` dump |

Full fill values: [D7_62351-3_PID_DRAFT.md](D7_62351-3_PID_DRAFT.md). Formal case mapping: `ccli-62351-100-3-extract` §5 (Table 2 normal · Table 3 resiliency).

---

## §4 Track C — 62351-4 / 62351-100-4 (MMS security PID)

**First pass:** A-profile + TLS + **G.2** dual cert — **not** full native §13 EncrTransfer **M**.

| □ | Theme | 62351-4 / T | Internal pre-test | Formal 100-4 | Evidence |
|---|-------|-------------|-------------------|--------------|----------|
| □ | E2E association | T.3.3.4 | T3-05 / Connect | Lab if offered | P3-07 live connect notes |
| □ | AARQ/AARE auth value | §11.2.2 | Initiate Success | Lab cases | DUT MMS log |
| □ | Subject / AP-title | G.6.2 | Lab PEM practice | Lab | PKI pack |
| □ | Cert B keyUsage G.5.2 | G.5.2 | verify script | Lab | PEM profile |
| □ | Security events on fail | T.3.3.4.6 | Force T3-01 · check event store | Lab | `ccli --event-dump` |
| □ | Native §13 SecPDU | §13 | **N/A** first PID | Defer | — |
| □ | Lab executes 100-4:2023 | Q 7.3 | Confirm yes/no | **N/A** until yes | meeting minutes |

Planning extract: `knowledge-base/08-engineering/CCI_62351-100-4_Extract.md` · OCR: `ccli-62351-100-4-ocr-corpus`.

---

## §5 Track D — Product cyber (5-LN or 31-LN — same transport)

| □ | REQ-ID | Test | Pass |
|---|--------|------|------|
| □ | REQ-PROD-SW-002 | Eth_A listen | **3782** TLS; **no** :102 with product config |
| □ | REQ-PROD-SW-007 | Release bundle audit | No `cleartext_tsp` / `tcp_port: 102` in shipped yaml |
| □ | REQ-PROD-SW-006 | Routing | No WAN → Eth_A path (`ccli-62443-zones-extract`) |

LN count does **not** change TLS/RBAC/ACSE rows — only functional matrix browse depth changes.

---

## §6 Track E — Accredited / documents (no TSP)

| □ | Deliverable | Standard | Gate | Owner |
|---|-------------|----------|------|-------|
| □ | **D7** PID submitted | 62351-3 §8 | Pre-quote | eng → lab |
| □ | **62351-100-3** test report | O §12 | **P7-07** | accredited lab |
| □ | **62351-4 PID** + optional **100-4** | T.3.3.4 | Second quote if split | lab |
| □ | Paste D7 markdown → lab form | — | When template received | eng |
| □ | 62443-4-1/4-2 product cert | O.15 stack | **OPEN** | programme · not TSP |

---

## Cross-reference — functional matrix

| Functional matrix row | Cyber matrix |
|----------------------|--------------|
| TLS transport (P3-06) | §2 T1-01 · §3 REQ-D7-PORT |
| No client cert | §2 T3-01 |
| ACSE / dual cert (P3-07) | §2 T3-05 · §4 |
| RBAC viewer (P3-08) | §2 T3-02 · T3-03 |
| CRL revoked (P3-09) | §2 T3-04 |

Tick cyber **□** here; functional matrix may reference this file instead of duplicating detail.

---

## RAG ingest

```powershell
powershell -File scripts\ingest-product-deploy-rag.ps1
# or full refresh:
powershell -File scripts\ingest-next-phases.ps1 -SkipDocker
```

**Keywords:** `cyber`, `62351-3`, `62351-4`, `62351-100-3`, `62351-100-4`, `D7`, `P3-06`, `P3-07`, `P3-08`, `P3-09`, `P7-07`, `3782`, `Tier 3`, `ccli-lab-requirement-test-matrix-cyber`

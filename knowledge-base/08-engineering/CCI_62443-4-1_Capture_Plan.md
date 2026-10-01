# IEC 62443-4-1 — Capture Plan (CCLI / TG-524)

**Document ID:** CCLI-SEC-62443-004  
**Revision:** 1.3  
**Date:** 2026-08-11  
**RAG source_id:** `ccli-62443-4-1-capture-plan` · Batch A → `ccli-62443-4-1-extract`  
**Normative basis:** IEC 62443-4-1:2018 — *Secure product development lifecycle requirements*  
**Parent:** `ccli-62443-3-3-extract` · `ccli-62443-zones-extract`  
**Programme link:** K6.3 · K6.7 · KR-012

---

## Summary

**62443-4-1** defines **8 Practices** (SM, SR, SD, SI, SVV, DM, SUM, SG) with **47 requirement clauses** for the **Secure Development Lifecycle (SDL)** of IACS products. This is the **process audit** basis (ISA SDLA / EDSA); **62443-4-2** is the **technical component** test.

**CCLI role:** HiTEKS as **product supplier** for TG-524 CCI (embedded + application per Figure 2). Integrator/asset-owner parts (3-2, 2-4) are DSO/site scope.

**Target maturity:** **ML 2–3** for first cert pass (Table 1); confirm with lab quote.

---

## Batch status

| Batch | Pages | Content | Status |
|-------|-------|---------|--------|
| **A** | 6, 7–10, 16–20 | Foreword, intro, scope, Figs 1–3, terms tail, §4, Table 1 | **CAPTURED** |
| **B** | 21–30 | Practice 1 SM-1…SM-13; Practice 2 SR-1…SR-5; SD-1 partial | **CAPTURED** |
| **C** | 31–37 | Practice 3 SD gap + SI + SVV + Table 3 | **PART** — pp. 33–37 (SD-2…4 gap pp. 31–32) |
| **D** | 38–47 | Practice 6–8 DM, SUM, SG | **CAPTURED** |
| **E** | 49, 50–51 | Annex A metrics (partial); **Annex B Table B.1** | **CAPTURED** |

---

## Batch A — captured pages

```
6, 7, 8, 9, 10, 16, 17, 18, 19, 20
```

| Page | Content |
|------|---------|
| **6** | Foreword — TC 65; FDIS 65/685 |
| **7** | §1 Scope (SDL); §2 Normative refs (62443-2-4); §3.1 start (abuse case) |
| **8** | Introduction — SDLA/ISCI lineage; OWASP CLASP; 61508; DO-178B |
| **9** | **Figure 1** — 62443 series map |
| **10** | **Figure 2** — Product lifecycle (supplier 4-1/4-2 vs integrator 3-2/3-3) |
| **16** | §3.1.36–41 (threat, threat modelling, trust boundary, zone) |
| **17** | §3.2 acronyms (SDL, SM, SR, SD, SI, SVV, DM, SUM, SG, STRIDE); §3.3 Conventions |
| **18** | §4.1 Concepts — defense in depth; link to 4-2 SL-C and 3-2 risk |
| **18–19** | **Figure 3** — SDL practices → defense-in-depth |
| **19** | §4.2 Maturity model; SM-5 applicability; notes on evolution |
| **20** | **Table 1** — Maturity levels 1–5; §5 Practice 1 header |

### Batch A gaps (optional)

```
11–15    ← §3.1 terms middle (zone/threat already on p.16)
```

---

## Batch B — captured pages

```
21, 22, 23, 24, 25, 26, 27, 28, 29, 30
```

| Page | Content |
|------|---------|
| **21–22** | SM-1 development process; SM-2 responsibilities; SM-3 applicability |
| **22–23** | SM-4 security expertise; SM-5 process scoping |
| **23–24** | SM-6 file integrity; SM-7 dev environment; **SM-8 private keys** |
| **24–25** | SM-9 external components; SM-10 custom third-party |
| **25–26** | SM-11 release gate; SM-12 verification; SM-13 continuous improvement; **Table 2** |
| **26–29** | Practice 2 — **SR-1…SR-5** (context, threat model, reqs, SL-C, review) |
| **30** | Practice 3 — **SD-1** secure design principles (start) |

---

## Session 4 — captured pages (pp. 33–40)

```
33, 34, 35, 36, 37, 38, 39, 40
```

| Page | Content |
|------|---------|
| **33–34** | **SI-1** implementation review + SCA; **SI-2** secure coding standards |
| **34–37** | **SVV-1…SVV-5**; **Table 3** tester independence |
| **38–40** | Practice 6 — **DM-1…DM-4** (receive, triage, assess, address) |

### Session 4 gaps (closed)

```
31, 32    ← SD-2 defence in depth · SD-3/4 design review (optional if zones doc suffices)
```

---

## Final batch — captured pages (pp. 41–51)

```
41, 42, 43, 44, 45, 46, 47, 49, 50, 51
```

| Page | Content |
|------|---------|
| **41** | **DM-5** disclose reportable issues (CVSS, resolution); resolution considerations |
| **42** | **DM-6** annual defect management review; Practice 7 — **SUM-1** qualification |
| **43** | **SUM-2…SUM-4** patch docs, OS compatibility, authentic delivery |
| **44** | **SUM-5** patch SLA policy; Practice 8 — **SG-1** defense-in-depth doc |
| **45–46** | **SG-2…SG-4** environment measures, hardening, secure disposal |
| **47** | **SG-5…SG-7** secure operation, accounts, documentation review |
| **49** | Annex A metrics examples (partial) |
| **50–51** | **Annex B Table B.1** — all 47 requirements |

### Optional remaining capture

```
31, 32, 48    ← SD-2…SD-4 body · Annex A start
```

---

## CCLI mapping template

| Practice | Prefix | CCLI artefact | Status |
|----------|--------|---------------|--------|
| 1 Security management | SM | SDL process doc + QMS reuse (ATEX) | **CAPTURED** — artefact GAP |
| 2 Security requirements | SR | Threat model + CRS from 3-3 | **CAPTURED** — artefact PART |
| 3 Secure by design | SD | `CCI_62443_Zones.md` + architecture | PART |
| 4 Secure implementation | SI | Coding standard + review gates | **CAPTURED** — artefact GAP |
| 5 V&V testing | SVV | Lab test plan (4-2 alignment) | **CAPTURED** — artefact GAP |
| 6 Defect management | DM | Vuln disclosure policy (K6.7 / CRA) | **CAPTURED** — artefact GAP |
| 7 Update management | SUM | Signed OTA process (K2.7) | **CAPTURED** — artefact GAP |
| 8 Security guidelines | SG | Operator hardening / disposal docs | **CAPTURED** — artefact GAP |

---

## Verification

| Item | Pass criteria |
|------|---------------|
| Batch A | Scope + Figs 1–3 + Table 1 in extract |
| Annex B | All 47 reqs mapped to CCLI artefacts |
| Cert quote | Lab confirms target ML + practice scope |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.2 | 2026-08-11 | Session 4 pp. 33–40 — SI, SVV, DM-1…4 |
| 1.3 | 2026-08-11 | Final batch pp. 41–51 — DM-5…6, SUM, SG, Annex B (47 clauses) |

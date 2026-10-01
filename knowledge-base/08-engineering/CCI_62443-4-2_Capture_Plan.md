# IEC 62443-4-2 — Capture Plan (CCLI / TG-524)

**Document ID:** CCLI-SEC-62443-006  
**Revision:** 1.8  
**Date:** 2026-08-11  
**RAG source_id:** `ccli-62443-4-2-capture-plan` · Batch A → `ccli-62443-4-2-extract`  
**Normative basis:** IEC 62443-4-2:2018 — *Technical security requirements for IACS components*  
**Capture status:** Normative + **Annex B HAVE**; gaps: p. 57 CR 6.1, p. 63 SAR, Annex A pp. 84–86  
**Parent:** `ccli-62443-3-3-extract` · `ccli-62443-zones-extract` · `ccli-62443-4-1-extract`  
**Programme link:** K6.4 · K6.5 · KR-012

---

## Summary

**62443-4-2** defines **Component Requirements (CR x.y)** and **Requirement Enhancements (RE)** for IACS components, derived from **62443-3-3** SRs. Four component types: **software application (SAR)**, **embedded device (EDR)**, **host device (HDR)**, **network device (NDR)**.

**TG-524 CCI lab scope:** primarily **EDR** (§13) + **SAR** (§12) + shared **CR 1–7** (§5–11); partial **NDR** (§15) for firewall/LTE/zone boundary.

**Target:** **SL-C 2** aligned with `SL-T(CCI Z0) = {2222222}` from zones + 3-3 work.

**Document length:** 98 pages (ToC footer).

---

## ToC captured (from screenshots)

| § | Title | Page |
|---|-------|------|
| — | Foreword | 12 |
| 0 | Introduction (0.1 Overview) | 14 |
| — | **Figure 1** — Parts of IEC 62443 series | 16 |
| 1 | Scope | 17 |
| 2 | Normative references | 17 |
| 3 | Terms, acronyms, conventions | 18–26 |
| 4 | Common component security constraints (CCSC 1–4) | 27 |
| **5** | **FR 1 — IAC** (CR 1.1–1.14) | 27–38 |
| **6** | **FR 2 — UC** (CR 2.1–2.13) | 38–45 |
| **7** | **FR 3 — SI** (CR 3.1–3.14) | 45–52 |
| **8** | **FR 4 — DC** (CR 4.1–4.3) | 52–54 |
| **9** | **FR 5 — RDF** (CR 5.1–5.4) | 55–56 |
| **10** | **FR 6 — TRE** (CR 6.1–6.2) | 56–58 |
| **11** | **FR 7 — RA** (CR 7.1–7.8) | 58–62 |
| **12** | Software application requirements (SAR) | 62–63 |
| **13** | Embedded device requirements (EDR) | 64–69 |
| **14** | Host device requirements (HDR) | 69–75 |
| **15** | Network device requirements (NDR) | 75–83 |
| **Annex A** | Device categories | 84–86 |
| **Annex B** | Table B.1 — CR/RE → SL 1–4 | 87–88+ |
| — | Bibliography | 93 |

Each CR follows: **Requirement → Rationale → Enhancements → Security levels (SL 1–4)**.

**4-2 extras vs 3-3:** CR **1.14** (symmetric key); CR **2.13** (physical diag); CR **3.10–3.14** (updates, tamper, RoT, boot).

---

## Capture batches — SL-C 2 TG-524 (P0)

Capture **`§x.y.4 Security levels`** for every CR **plus** device-type clauses for EDR/SAR/NDR.

### Batch A — Foundation — **CAPTURED**

```
12, 14, 16, 17, 19–27   ✓ Foreword · Intro · Fig 1 · Scope · Terms · CCSC · FR 1 start
```

Optional gap: **p. 18** (§3.1 terms start).

### Batch B — FR 1 IAC — **COMPLETE**

```
27–38   ✓ CR 1.1–1.14 + all SL tables (incl. 1.14 symmetric key)
```

Optional gap: **p. 31** (CR 1.4 SL confirm).

### Batch D — FR 3 SI + FR 4 start — **COMPLETE** (shared CRs)

```
45–52   ✓ CR 2.12–2.13 · FR 3 CR 3.1–3.9 · CR 3.10–3.14 deferred · FR 4 start
```

### Batch C — FR 2 UC — **COMPLETE**

```
39–44   ✓ CR 2.1 SL + RE 1–4 · CR 2.2–2.11 · CR 2.3 N/A · CR 2.4→§12–15
```

### Batch E — FR 4–7 start — **COMPLETE**

```
52–58   ✓ FR 4 CR 4.1–4.3 · FR 5 CR 5.1 (+ 5.2–5.3→NDR) · FR 6–7 start
```

Gap: **p. 57** (CR 6.1).

### Batch F — FR 6–7 + SAR start — **COMPLETE**

```
58–62   ✓ CR 6.2 SL · FR 7 CR 7.1–7.8 · §12 SAR 2.4 start
```

Gap: **p. 57** (CR 6.1).

### Batch G — §12 SAR + §13 EDR — **COMPLETE** *(p. 63 SAR gap)*

```
62      ✓ §12 SAR 2.4 start
63      ← SAR 2.4 SL + SAR 3.2 (not in batch)
64–69   ✓ §13 EDR 2.4 · 2.13 · 3.2 · 3.10–3.14 (P0 lab)
```

### Batch H — §14 HDR tail + §15 NDR — **COMPLETE**

```
75      ✓ HDR 3.14 SL tail
75–83   ✓ §15 NDR 1.6 · 1.13 · 2.4 · 2.13 · 3.2 · 3.10–3.14 · 5.2 · 5.3
```

Gap: **§14 HDR** body pp. 69–74 (optional for TG-524).

### Batch K — Annex B — **COMPLETE**

```
87–92   ✓ Annex B B.1–B.2 · Table B.1 FR 1–7 · Key
```

Gap: **Annex A** pp. 84–86 (device categories — optional).

---

## TG-524 minimum first pass (if batching slowly)

```
16, 17, 27          context
27–38               IAC
45–52               SI (boot/RoT/updates)
64–69               EDR
62–63               SAR
75–83               NDR (firewall, LTE, zones)
87–92               Annex B SL-2 checklist
```

---

## CCLI mapping template

| Type | Clause | SL-C target | CCLI control | Status |
|------|--------|-------------|--------------|--------|
| Shared | CR 1.8 | 2 | PKI / 62351 certs | MISS (K6.5) |
| Shared | CR 3.14 | 2 | Secure Boot chain | PART (K6.1) |
| EDR | EDR 3.12 | 2 | TPM supplier RoT | PART (K6.2) |
| EDR | EDR 3.10 | 2 | Signed OTA | PART (K2.7) |
| SAR | SAR 3.2 | 2 | Malware at app entry | PART |
| NDR | NDR 5.2 | 2 | Deny-by-default firewall | PART (K3.3) |

---

## Relationship to other parts

| Part | Role | Corpus |
|------|------|--------|
| **62443-3-3** | System SR/SL-C (design language) | `ccli-62443-3-3-extract` **HAVE** |
| **62443-4-1** | Product SDL process audit | `ccli-62443-4-1-extract` **HAVE** |
| **62443-4-2** | Component technical test | **this capture plan** |
| **62443-3-2** | Zones, SL-T | `ccli-62443-zones-extract` PART |

Lab quote cites **4-2**; design team traces **3-3 SR → 4-2 CR** IDs (same numbering).

---

## Verification

| Item | Pass criteria |
|------|---------------|
| 3-3 ↔ 4-2 trace | Every SL-C 2 SR row has matching CR row + device clause where applicable |
| EDR scope | TG-524 cert checklist covers §13 + shared CRs |
| K6.4 | Moves to **HAVE** when FR 1–7 + EDR/SAR/NDR SL-2 rows complete | **HAVE** (Annex B + normative body) |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-08-11 | ToC from user screenshots; Batch A captured pp. 12–27 |
| **1.1** | 2026-08-11 | Batch B pp. 27–38: FR 1 IAC complete |
| **1.2** | 2026-08-11 | Batch D pp. 45–52: FR 3 shared CRs + FR 4 start |
| **1.3** | 2026-08-11 | Batch E pp. 52–58: FR 4–5 complete; FR 6–7 start |
| **1.4** | 2026-08-11 | Batch F pp. 58–62: FR 7 complete; SAR 2.4 start |
| **1.5** | 2026-08-11 | Batch C pp. 39–44: FR 2 UC complete |
| **1.6** | 2026-08-11 | Batch G pp. 64–69: §13 EDR complete; SAR p.63 gap |
| **1.7** | 2026-08-11 | Batch H pp. 75–83: §15 NDR complete |
| **1.8** | 2026-08-11 | Batch K pp. 87–92: Annex B Table B.1 complete |

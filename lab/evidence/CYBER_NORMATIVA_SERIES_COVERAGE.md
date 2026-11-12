# Cyber normativa — full series & annex coverage index

**Document ID:** CCLI-LAB-EVD-CYBER-NORM-001  
**Revision:** 1.0  
**Date:** 2026-11-12  
**RAG source_id:** `ccli-cyber-normativa-series-coverage`  
**Parents:** `lab/conformance/Full Validate.md` (REQ-NCV-001) · `CCI_62351_Clause_Verification_Matrix.md` · `CCI_62443_TesPro_HiTEKS_Gap_Matrix.md` · `CCI_Implemented_Cyber_Profile.md`  
**Child (network layer only):** `lab/evidence/P2_fw4_NORMATIVA_COVERAGE_MATRIX.md`

This file **confirms programme coverage of all cyber-related normativa series and CEI annexes** referenced for CCLI O.15 / CSA / 62351. It is an **index and audit** — clause-level truth remains in **Full Validate** and **62351 Clause Matrix**.

---

## 1. Confirmation summary

| Question | Answer |
|----------|--------|
| Are **all** cyber norm families in `Full Validate.md` “Normative editions” listed here? | **Yes** (§2) |
| Are **CEI annexes** O / T / M cyber clauses mapped? | **Yes** (§3–5); **Annex U** = extract **MISSING** |
| Is **IEC 62351** full programme scope indexed? | **Yes** (§6); parts without extract = noted |
| Is **IEC 62443** (3-2 / 4-1 / 4-2 / 3-3) indexed? | **Yes** (§7) |
| Does **fw4 analysis** alone cover all of the above? | **No** — fw4 covers **§7 + O.13.1.1.1/.2 + T.3.3.4.10/.11** only (~15% of cyber rows); see §8 |
| Any **uncatalogued** cyber norm in repo grep? | **No** additional CEI/IEC families beyond §2 for CCI scope |

---

## 2. Normative families — corpus & primary evidence

| Family | Edition (claimed) | Extract / doc | Corpus | Primary evidence track | fw4? |
|--------|-------------------|---------------|--------|------------------------|------|
| **CEI 0-16 Allegato O** | 2025-12 V5 | `CCI_Annex_O_Extract.md` | **HAVE** | Full Validate §A; P2–P7 | **PART** (O.13.1.1.x) |
| **CEI 0-16 Allegato T** | 2025-12 | `CCI_Annex_T_Extract.md` | **HAVE** | Full Validate §C; P3/P4 | **PART** (T.3.3.4.10–11) |
| **CEI 0-16 Allegato M** | 2025-12 | `CCI_Annex_M_Extract.md` | **HAVE** | Full Validate §B; P6 | LTE path only |
| **CEI 0-16 Allegato U** | 2025-12 | — | **MISSING** | P3-05 timers | — |
| **IEC 61850-10** | Ed 2.1 (2025) | `CCI_61850-10_Extract.md` | **HAVE** | Full Validate §E; UCA **#5** | — |
| **IEC 62351-3** | 2014+AMD | `CCI_62351-3_Extract.md` | **HAVE** | Full Validate §F; D7; **#6** | port bind |
| **IEC TS 62351-100-3** | 2020 | `CCI_62351-100-3_Extract.md` | **HAVE** | P7-07; U-ACC-02 | — |
| **IEC 62351-4** | 2018+AMD | `CCI_62351-4_Extract.md` | **HAVE** | Full Validate §G; ACSE | — |
| **IEC TS 62351-100-4** | 2023 | `CCI_62351-100-4_Extract.md` | **HAVE** (OCR) | Optional lab | — |
| **IEC 62351-8** | 2011 | `CCI_62351-8_Extract.md` | **HAVE** | P3-08 RBAC | — |
| **IEC 62351-9** | 2023 | `CCI_62351-9_Extract.md` | **HAVE** | P3-09 CRL/PKI | — |
| **IEC 62351-14** | CDV 2025 | `CCI_62351-14_Extract.md` | **HAVE** (capture) | Syslog SIEM map | — |
| **IEC 62351-5** | — | `CCI_62351-5_Extract.md` | **HAVE** | Eth_B **104** + TLS P4-03 | — |
| **IEC 62351-1/6/7** | — | Capture plans | **PART** | Not first O.15 claim | — |
| **IEC TS 62351-100-6** | cited | — | **N/A** | GOOSE off | — |
| **IEC 62443-3-2** | 2020 | `CCI_62443_Zones.md` | **HAVE** | ZCR; P2-05 | **PART** |
| **IEC 62443-3-3** | — | `CCI_62443-3-3_Extract.md` | **HAVE** | Policy (site) | — |
| **IEC 62443-4-1** | 2018 | `CCI_62443-4-1_Extract.md` | **HAVE** | SDLA; **#4** | — |
| **IEC 62443-4-2** | 2018 | `CCI_62443-4-2_Extract.md` | **HAVE** | CSA SL-C; **#4** | NDR 5.x |
| **ISA CSA 311 / SDLA 312** | v1.5 | `CCI_ISASecure_*_Extract.md` | **HAVE** | FSA IDs; gap matrix | FSA-NDR 5.x |
| **FIPS 140-2 / CC** | O.15 **#3** | TesPro vendor | **PART** | P7-08; not L3 | — |
| **ISO 9001 / CE** | O.15 **#7–8** | Manufacturer | Outside repo | — | — |
| **CEI EN 61557-12** | O.15 **#2** | `CCI_61557-12_Extract.md` | **HAVE** | Metrology ACC | — |

---

## 3. CEI Allegato O — cyber clauses (beyond firewall)

| Clause | Topic | Covered in | Evidence | Status | fw4? |
|--------|--------|------------|----------|--------|------|
| **O.12** | Teledistacco predisposition | Full Validate §A.6 | P6-04 | PART | — |
| **O.13.1.1.1** | No bridge; Eth_A/B roles; stack | Full Validate · **P2_fw4** §3 | P2-01…03 | PASS/PART | **Yes** |
| **O.13.1.1.2** | Local config non-IP | Full Validate · P2_fw4 | P2-04 | GAP | INPUT policy |
| **O.13.1.2** | DSO comms-loss fallback | Full Validate | P3-05 | PASS | App |
| **O.13.1.3** | Eth_B fallback | Full Validate | P4-04 | PASS | App |
| **O.13.4** | Signed FW update | Full Validate · T.3.3.4.8 | TesPro + event store | GAP | Platform |
| **O.13.5** | UTC ±100 ms | Full Validate | P3-11 | PASS/PART | chrony |
| **O.13.7** | HW tamper | Full Validate · T | P7-08 | GAP | HW |
| **O.13.7.2** | Asset inventory | T.3.3.4.7 | LPHD1 | PART | — |
| **O.13.10** | Availability RAMS | Full Validate | — | OPEN | — |
| **O.14** | Security event logger | Full Validate §A.7 | P7-01…04 | PART | — |
| **O.15 §12** | Eight certificates | Full Validate §A.8 | P7 / U-ACC-* | PART/ACC | See §9 |

**O.13.2** (metrology accuracy) and **O.13.3** (UPS) are **#2 / HW**, not cyber profile — listed in Full Validate only.

---

## 4. CEI Allegato T — T.3.3.4 cybersecurity profile (all subclauses)

| Subclause | Topic | Full Validate | 62351 matrix | Typical test | Status | fw4? |
|-----------|--------|---------------|--------------|--------------|--------|------|
| **T.3.3.4.1** | Crypto algorithms (RSA, SHA, E2E) | §C.4 | §1 | U-D7-02 · U-T3-05 | PART | — |
| **T.3.3.4.2** | GOOSE security (informative) | §C.1 GOOSE off | — | N/A | N/A | L2 P2-G01 |
| **T.3.3.4.3** | Roles / 62351-8 | §C.3 | §1 | P3-08 | PASS lab | — |
| **T.3.3.4.5** | Time sync NTP/NTS/skew | §C.4 | — | P3-11 | PART/GAP NTS | — |
| **T.3.3.4.6** | Security event log + syslog TLS | §C.4 | §4–7 | P7-01/03 | PART | — |
| **T.3.3.4.7** | Asset inventory (LPHD) | §C.4 | — | U-T1-03 | PART | — |
| **T.3.3.4.8** | Secure boot + signed update | §C.4 | — | REQ-V-22/23 | GAP | Platform |
| **T.3.3.4.9** | PKI enrolment CRL OCSP SCEP/EST | §C.4 | §4–5 | P3-09 | PART/GAP | — |
| **T.3.3.4.10** | Traffic segregation FW/NAT/VPN | §C.4 · **P2_fw4** | — | P2 · U-CYB-PROD-01 | PART | **Yes** |
| **T.3.3.4.11** | Local + remote maint auth | §C.4 · **P2_fw4** | — | P2-04 · SSH | GAP/PART | **Yes** |

**T.3.3.4** TLS table rows (port 3782, ciphers, reneg, resume, CAs, CRL, OCSP) are in Full Validate §C.4 and `CCI_62351_Clause_Verification_Matrix.md` §1 — **not** fw4.

**T.3.3.2** ACSI (association, reporting, control) is **protocol** evidence — Full Validate §C.1–C.3 · UCA **#5**.

---

## 5. CEI Allegato M — cyber-relevant only

| Clause | Cyber aspect | Evidence | fw4? |
|--------|--------------|----------|------|
| **M.1–M.2** | LTE/GSM receiver path (untrusted radio) | P6-01; REQ-SEC-ZC-006 | **wan/usb0** zone |
| **M.4** | AT/SMS CLI whitelist | Site modem | — |
| **M ∩ O.11** | CCI must not fight teletrip | P6-03 | App + DIO |
| **M ∩ O.14** | Log defence-plan trip | P6-05 | O.14 |

Annex M is **not MMS**; isolation from Eth_A is **routing + policy**, not TLS.

---

## 6. IEC 62351 series — programme scope

| Part | Role for CCLI | In clause matrix? | First O.15 claim? | Status (programme) |
|------|---------------|-------------------|-------------------|-------------------|
| **62351-3** | TLS on MMS/104 transport | **Yes** Full Validate §F | **#6 M** | PART → ACC OPEN |
| **62351-100-3** | Accredited transport tests | **Yes** | **#6 M** | OPEN P7-07 |
| **62351-4** | MMS ACSE security (A-profile §11) | **Yes** §G | T / optional 100-4 | PART; §13 N/A |
| **62351-100-4** | ACSE conformance tests | **Yes** (planning) | Optional | PART |
| **62351-8** | RBAC roles | **Yes** | T (not #6) | PASS lab |
| **62351-9** | PKI, CRL, key mgmt | **Yes** | T + #4 | PART |
| **62351-14** | Syslog ↔ 62351 events | **Yes** | T logger | PART |
| **62351-5** | 104 security | **Yes** | P4-03 | PASS lab |
| **62351-1** | Overview | Capture plan | — | Reference |
| **62351-6** | GDOI | Capture / N/A | — | **N/A** |
| **62351-7** | Symmetric keys | Capture | — | Not first claim |

**Cross-link:** `CCI_62351_Clause_Verification_Matrix.md` is the **62351-only clause book** (REQ-NCV-001 companion for #6).

---

## 7. IEC 62443 — zones & component requirements

| Document | Requirement focus | Evidence doc | fw4 relevance |
|----------|-------------------|--------------|---------------|
| **62443-3-2** | ZCR 1–7; zones Z0–Z5; conduits C1–C5 | `CCI_62443_Zones.md` | **FULL** map |
| **62443-3-3** | System policy (site) | Extract | Integration |
| **62443-4-1** | SDLA / secure development | Extract · EVD packs | Process |
| **62443-4-2** | **CR 1–7** · **EDR/SAR/NDR/HDR** | Extract · gap matrix | **NDR 1.6/1.13/5.2/5.3** in **P2_fw4** |
| **ISA CSA 311** | FSA-CR/NDR/EDR IDs | `CCI_ISASecure_CSA_311_v1_5_Extract.md` | FSA mapping |

**Component split (TesPro vs HiTEKS):** `CCI_62443_TesPro_HiTEKS_RACI.md` — firewall engine = **TesPro NDR**; TLS/RBAC = **HiTEKS SAR**.

---

## 8. Layer model — what fw4 analysis covers

```text
Layer 0  Platform     Secure boot · TPM · FIPS/CC · signed image     → T.3.3.4.8 · O.13.4 · EDR 3.10–3.14
Layer 1  Network      nftables/fw4 · zones · LTE isolation            → O.13.1.1.1 · T.3.3.4.10–11 · NDR 5.x
Layer 2  Transport    TLS 1.2 · 62351-3 · ports 3782/2404             → O.15 #6 · 62351-100-3
Layer 3  Application  62351-4 ACSE · 62351-8 RBAC · 61850-10         → O.15 #5 · P3-07/08
Layer 4  PKI          62351-9 · CRL · enrolment                       → T.3.3.4.9 · CR 1.8
Layer 5  Logging      O.14 · T.3.3.4.6 · 62351-14                  → P7 · syslog TLS GAP
```

| Layer | Rows in **P2_fw4** matrix | Rows in **Full Validate** (cyber-related) |
|-------|---------------------------|---------------------------------------------|
| 1 only | **62** | ~15 |
| All cyber | — | **250+** (entire book) |

**Conclusion:** Confirming **all cyber normativa** requires **this index + Full Validate + 62351 matrix** — not fw4 alone.

---

## 9. O.15 certificates — cyber linkage

| O.15 # | Certificate | Cyber standards | Evidence status |
|--------|-------------|-----------------|-----------------|
| **1** | Functional CCI | O.6 + T + O.14 | PART (5-LN) |
| **2** | 61557-12 / 61010 | Not cyber transport | ACC OPEN |
| **3** | FIPS 140-2 L3 or no-claim | Crypto module | GAP (not L3) |
| **4** | CSA + SDLA | **62443-4-1/4-2** · ISASecure | PART pack |
| **5** | 61850 UCA | 61850-10 (+ 62351 via §6.3) | ACC OPEN |
| **6** | **62351-3 → 100-3** | **M** mandatory | ACC OPEN |
| **7** | ISO 9001 | QMS | Manufacturer |
| **8** | CE + DoC | Technical file incl. cyber | OPEN |

---

## 10. Programme REQ IDs — cyber cross-walk

| REQ family | IDs | Mapped in |
|------------|-----|-----------|
| Normative book | **REQ-NCV-001…007** | Full Validate |
| Network | **REQ-NET-001…005**, **REQ-LTE-001** | Architecture framework · **P2_fw4** |
| Vendor cyber | **REQ-V-16…21**, **V-36** | Vendor requirements · **P2_fw4** |
| Product SW | **REQ-PROD-SW-002**, **006** | Product freeze · Implemented profile |
| Implementation | **REQ-IMPL-CYB-001…015** | `CCI_Implemented_Cyber_Profile.md` |
| 62443 zones | **REQ-SEC-ZC-001…010** | `CCI_62443_Zones.md` · **P2_fw4** (subset) |
| Unified tests | **U-CYB-PROD-01**, **U-CYB-1003-***, **U-D7-*** | `LAB_UNIFIED_CERT_TEST_MATRIX.md` |

---

## 11. Known gaps / missing normativa (explicit)

| Item | Type | Impact |
|------|------|--------|
| **Annex U** extract | Corpus **MISSING** | Operating Rule timer values legal vs lab |
| **UCA Server TP Ed 2.1** | Corpus **MISSING** | Formal #5 case IDs |
| **62351-100-3** Table 2/3 case IDs | OCR garbled | Lab uses licensed PDF |
| **T.3.3.4 NTS / redundant NTP** | Norm > impl | T.3.3.4.5 GAP |
| **SCEP/EST enrolment** | T.3.3.4.9 | GAP |
| **OCSP** (T wants; D7 x) | Policy conflict | PART/N/A |
| **Native 62351-4 §13 E2E** | Deferred | N/A first PID |
| **TLS syslog** (T.3.3.4.6) | Implementation | PART |
| **VPN product** for LTE (T.3.3.4.10) | Product | OPEN |
| **DSO-specific firewall addendum** | Contract | Not in corpus |

---

## 12. Document map (where to drill down)

| Need | Read |
|------|------|
| **Firewall vs normativa only** | `P2_fw4_NORMATIVA_COVERAGE_MATRIX.md` |
| **Every O.15 clause row** | `lab/conformance/Full Validate.md` |
| **62351 clause-by-clause** | `knowledge-base/08-engineering/CCI_62351_Clause_Verification_Matrix.md` |
| **TesPro vs HiTEKS cyber RACI** | `CCI_62443_TesPro_HiTEKS_Gap_Matrix.md` |
| **As-built product cyber** | `CCI_Implemented_Cyber_Profile.md` |
| **Phase gates P2–P4 cyber** | `CCI_Phase_Regulation_Checklists.md` |
| **Vendor shalls** | `lab/conformance/VENDOR_CYBER_SW_REQUIREMENTS.md` |

---

## 13. Revision log

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-11-12 | Initial full series + annex coverage index; confirms fw4 is subset of cyber normativa |

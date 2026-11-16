# Essential security training — IEC 62443 (all HiTEKS staff on CCLI)

**Document ID:** CCLI-ORG-TRAIN-62443-001  
**Date:** 2026-11-16  
**Normative:** IEC 62443-4-1 **SM-4** (security expertise)  
**Process owner:** HiTEKS SDL / Security Manager  
**Evidence pack:** [`../evidence-packs/EVD-SM-4-Training.md`](../evidence-packs/EVD-SM-4-Training.md)  
**Assessment log (official):** [`templates/SM-4_Training.md`](templates/SM-4_Training.md) §3  

**Audience:** **Everyone** working on **CCLI** (firmware, lab, architecture, cert, project/product management, and collaborators with repo access). TesPro platform staff follow **their own** training under **SM-10** (not recorded in this log).

---

## 1. Why this mail exists

IECEE **IEC 62443-4-1** requires that:

1. **All** personnel in the secure development lifecycle have **security awareness**.  
2. Personnel in **named SM-2 roles** have **demonstrated expertise** for those duties (training, experience, certification, or equivalent).  
3. The organisation **records** who completed what and an assessor confirms **fit for role**.

This document is the **company-wide essentials list**. It matches Practice 1 / SM-4 in [`SDL-SM-Practice-1-Security-Management.md`](SDL-SM-Practice-1-Security-Management.md) and the auditor-facing template [`templates/SM-4_Training.md`](templates/SM-4_Training.md).

---

## 2. Mandatory for everyone (awareness)

Complete **before** committing to CCLI repos or cert artefacts:

| # | Topic | Minimum action | Internal reference |
|---|--------|----------------|---------------------|
| A1 | **62443 in one sitting** | Read § SM-4 + RACI overview (~45 min) | [`SDL-SM-Practice-1-Security-Management.md`](SDL-SM-Practice-1-Security-Management.md) (Roles + SM-4) |
| A2 | **CCLI security scope** | What is in scope: `ccli` on TG-544, zones, no Pi product | [`README.md`](README.md) · [`../CCI_62443_Zones.md`](../CCI_62443_Zones.md) |
| A3 | **No secrets in git** | Never commit keys, passwords, `.env`, live PEMs; lab PKI is documented + Trivy-accepted only | [`lab/conformance/LAB_ACCESS_FREEZE.md`](../../../lab/conformance/LAB_ACCESS_FREEZE.md) |
| A4 | **Separation of duties** | Know your role: do not sign off your own code as independent tester | [`templates/SM-2_RACI.md`](templates/SM-2_RACI.md) |

**Record:** line manager emails SDL lead with name + date of A1–A4 (or team meeting roll-call). SDL lead enters row in **SM-4 §3 log**.

---

## 3. Role-based essentials (SM-2)

Take the **awareness** block above **plus** the **training type(s)** for your role. One **accepted** course or **documented equivalent experience** per type is enough; use the **reference catalogue** below (§4), not an open-ended mandate to finish every vendor link.

| SM-2 role | Extra training (expertise) | Also applies when… |
|-----------|------------------------------|---------------------|
| **Security Manager / Security Expert** | Threat Modeling | SM-8 key custodian · SR/SD review · external security contact |
| **Project Manager** | Security Risk Assessment | SM-11 / SM-12 release gates |
| **Product Management** | Security Risk Assessment | SUM / SG after launch |
| **Product Development Lead** | None beyond awareness | Still must complete §2 |
| **Security Engineer** | Threat Modeling · Secure Coding · Security Testing | SI-2 · SM-6 / SM-7 |
| **Security Test Engineer** | Security Testing | **SVV-5 independence** — do not test code you wrote |
| **TesPro contacts** | Vendor SDL pack | **Not** logged on HiTEKS SM-4 sheet |

Role assignment: [`templates/SM_Product_Security_Development_Plan.md`](templates/SM_Product_Security_Development_Plan.md) §3 · [`templates/SM-2_RACI.md`](templates/SM-2_RACI.md).

---

## 4. Reference training catalogue (SM-4)

HiTEKS records what each person **actually** completed (course title, date, certificate, or equivalent experience). Listing a source here does **not** mean the company purchased it.

| Training type | Reference training material |
|---------------|----------------------------|
| **Threat modeling** | Microsoft — Threat Modeling Security Fundamentals · Udemy — Threat Modeling · Coursera — How to Threat Model (University of California, Irvine) |
| **Security risk assessment** | SANS — MGT415: Cyber Security Risk Assessment Training |
| **Secure coding** | Carnegie Mellon — Secure Coding in C and C++ |
| **Security testing** | TryHackMe — Cyber Security Training |

**CCLI lab correlate (optional, not a substitute for SM-4):** Trivy SCA runbook `lab/evidence/trivy/README.md` · zone/conduit evidence under `lab/evidence/` and ZCR docs in this folder.

---

## 5. Recruitment and hiring (security-related roles)

For roles that touch SDL, firmware cyber, or cert evidence, hiring should prefer:

- Bachelor’s or Master’s degree in **Computer Science** or **Cybersecurity**, and/or  
- Prior industry experience in **cybersecurity** roles, and/or  
- Recognised certifications such as **CISSP**, **CISM**, **CISA**, **CompTIA Security+**, or equivalent.

Equivalency is decided by the **Security Manager** and recorded on the SM-4 log.

---

## 6. Continuous learning

Everyone in §3 roles should maintain expertise. Recommended roadmaps (self-paced):

- **SANS** — Cyber Security Skills Roadmap / MGT415 follow-ons  
- **Carnegie Mellon** — secure coding refresh when touching `apps/ccli` C/C++  
- **Project-specific:** [`START_HERE_62443_TOOLCHAIN.md`](../START_HERE_62443_TOOLCHAIN.md) (register · Jira CSEC · evidence packs)

SM-13 retrospectives capture lessons learned: [`templates/SM_Retrospective.md`](templates/SM_Retrospective.md).

---

## 7. Responsibility separation (summary)

High-level rule: **accountable** vs **responsible** vs **independent test** are different people where 62443 requires it (especially **SVV** and **SM-4 assessment**).

Full matrix: [`templates/SM-2_RACI.md`](templates/SM-2_RACI.md) and SDL-SM-Practice-1 § SM-2 RACI table.

| Example | Rule |
|---------|------|
| Security Test Engineer | Must not be sole implementer of the feature under SVV-5 test |
| SM-4 “Fit for role?” | Assessor is not the trainee |
| Signing / SM-8 | Key custodian ≠ day-to-day developer without written process |

---

## 8. What you must send back (by role)

| Who | Send to SDL lead | Content |
|-----|------------------|---------|
| **All** | Within 2 weeks of this notice | Name · awareness date (§2) · role on CCLI |
| **SM-2 roles** | Within 90 days (or before release gate) | Course completed **or** equivalent experience · certificate ID if any |
| **Line managers** | With new hire on CCLI | Role assignment for template **1.1** |

SDL lead maintains **`templates/SM-4_Training.md` §3** and links evidence in **`EVD-SM-4-Training.md`** when auditors ask.

---

## 9. Related documents

| Doc | Use |
|-----|-----|
| [`SDL-SM-Practice-1-Security-Management.md`](SDL-SM-Practice-1-Security-Management.md) | Full SM-1…SM-13 process |
| [`templates/SM-4_Training.md`](templates/SM-4_Training.md) | Official assessment log |
| [`../START_HERE_62443_TOOLCHAIN.md`](../START_HERE_62443_TOOLCHAIN.md) | Jira · register · validation |
| [`company-broadcast/MESSAGE_62443_ESSENTIAL_TRAINING.txt`](company-broadcast/MESSAGE_62443_ESSENTIAL_TRAINING.txt) | Short text for email / Teams |

---

**Revision:** 1.0 · **Next review:** each major CCLI release (rN) or IECEE audit prep.
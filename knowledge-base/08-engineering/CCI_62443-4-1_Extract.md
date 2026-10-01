# IEC 62443-4-1 — CCLI Engineering Extract (Batch A)

**Document ID:** CCLI-SEC-62443-005  
**Revision:** 1.3  
**Date:** 2026-08-11  
**RAG source_id:** `ccli-62443-4-1-extract`  
**Normative basis:** IEC 62443-4-1:2018 — *Secure product development lifecycle requirements*  
**Capture status:** **HAVE** — all **47 clauses** (Annex B); SD-2…SD-4 body optional gap pp. 31–32  
**Parents:** `ccli-62443-3-3-extract` · `ccli-62443-zones-extract` · `ccli-62443-4-1-capture-plan`  
**Programme link:** K6.3 · K6.7 · KR-012

---

## Summary

**62443-4-1** specifies **process requirements** for the **Secure Development Lifecycle (SDL)** of IACS products — how the **product supplier** develops and maintains secure hardware, software, and firmware. It does **not** cover integrator installation or asset-owner operation (those are 3-2 / 2-4).

For **HiTEKS CCLI / TG-524:** you are the **product supplier** (Figure 2). The CCI is a **Product** under **62443-4-2** (embedded device + application). This extract covers **Practice 1** (SM-1…SM-13) and **Practice 2** (SR-1…SR-5) in full; **Practice 3** SD-1 started.

**Target SL-C:** SR-4 requires documenting **SL-C** for the product — align with **62443-4-2** + `{2222222}` from 3-3/3-2 zones work.

**Cert path:** ISA **SDLA** / **EDSA** process audit (4-1) + component technical test (4-2). Reuse **ATEX QMS** for SM-1, SM-12, SM-13 overlap.

---

## Requirements (from §1 Scope)

62443-4-1 defines an SDL covering:

| Lifecycle element | CCLI hook |
|-------------------|-----------|
| Security requirements definition | Trace to 62443-3-3 SR list |
| Secure design | `CCI_62443_Zones.md`, architecture freeze |
| Secure implementation | Coding standards, signed `.ipk` |
| Verification and validation | Lab + SVV practices |
| Defect management | K6.7 vuln process / CRA |
| Patch management | K2.7 signed OTA |
| Product end-of-life | Secure disposal (SG-4) |

**Applies to:** developer and **maintainer** of the product — **excludes** integrator and end user.

**Normative references (§2):** IEC 62443-2-4:2015 (+ AMD1:2017) — IACS service provider security program.

**Requirements summary:** Annex B (not yet captured).

---

## Architecture — 62443 series placement (Figure 1, §Introduction)

```
General        62443-1-1 Terminology · 1-2 Glossary · 1-3 Metrics · 1-4 Lifecycle
Policies       62443-2-1 Security program · 2-3 Patch · 2-4 Service providers
System         62443-3-1 Technologies · 3-2 Zones/SL-T · 3-3 SR/SL-C
Component      62443-4-1 Product SDL  ← this doc · 4-2 Component technical reqs
```

**Figure 2 — Product lifecycle scope:**

| Role | Standard | CCLI |
|------|----------|------|
| **Product supplier** | **4-1** (SDL) + **4-2** (product capabilities) | **HiTEKS** — TG-524 CCI |
| **System integrator** | 2-4, **3-2** | DSO / site integrator |
| **Automation Solution** | **3-3** SR/SL-C | SuC after integration |
| **Asset owner** | 2-1, 2-4 | DSO operates IACS |

**Product types (4-2):** applications · embedded devices · network components · host devices — TG-524 = **embedded + application**.

**Key boundary (p. 9):** 4-1 addresses **development process only** — not design/install/operation of the full Automation Solution. Product features or compensating mechanisms support **3-3** measures at integration time.

---

## Key concepts (§4.1–4.2)

| Concept | Definition / rule | CCLI implication |
|---------|-------------------|------------------|
| **Defense in depth** | Secure-by-design across full product lifecycle (Figure 3) | Zones + firewall + Secure Boot + TPM layers |
| **SL-C alignment** | Complying with 4-1 SDL supports meeting product **SL-C** | Pair with 4-2 + 3-3 SR traceability |
| **Threat modelling** | Design/analysis technique; early issue identification | Extend 3-2 zone threat table (SR-2) |
| **Impact analysis** | Severity from availability / integrity / confidentiality loss | PF2 curtailment = high impact |
| **SM-5 scoping** | All applicable reqs used per product regardless of org ML | Case-by-case exceptions documented |
| **Convention** | Requirements begin **"A process shall be employed…"** | Documented SDL procedures, not ad-hoc |

**Figure 3 — SDL practices → defense-in-depth (center):**

- Outer: **Security management** (spans all practices)
- Inner ring: Security guidelines · Spec of security reqs · Secure by design · Security V&V · Secure implementation
- Defect management + security update management → repairs under overall security management

**Lineage (Introduction):** Derived from ISA **SDLA**; sources include ISO/IEC 15408-3, OWASP CLASP, Howard/Lipner SDL, IEC 61508, RTCA DO-178B.

---

## Terms ingested (§3.1 — selected, pp. 7, 16)

| Term | Definition | CCLI usage |
|------|------------|------------|
| **Abuse case** | Negative test case / simulated attack from threat model | Pen-test / SVV scenarios |
| **Threat** | Event with potential to adversely affect ops, assets, IACS | Zone threat table |
| **Threat modelling** | Security design analysis (attack trees) | SR-2 deliverable |
| **Trust boundary** | Boundary where auth required or trust level changes | Z0–Z5 zone borders |
| **Zone** | Partition of SuC by functional/logical/physical relationship | `CCI_62443_Zones.md` |
| **User** | Person, org, or automated process accessing system | Console, MMS, services |

### §3.2 Acronyms (selected, p. 17)

| Acronym | Meaning | Practice prefix |
|---------|---------|-----------------|
| **SDL** | Security development life-cycle | Whole document |
| **SM** | Security management | Practice 1 |
| **SR** | Security requirements (spec) | Practice 2 — not 3-3 SR |
| **SD** | Secure design | Practice 3 |
| **SI** | Secure implementation | Practice 4 |
| **SVV** | Security verification and validation | Practice 5 |
| **DM** | Defect management | Practice 6 |
| **SUM** | Security update management | Practice 7 |
| **SG** | Security guidelines | Practice 8 |
| **STRIDE** | Spoofing, Tampering, Repudiation, Info disclosure, DoS, Elevation | Threat model |
| **SL-C** | Capability security level | Links to 4-2 / 3-3 |

---

## Maturity model (Table 1, §4.2, p. 20)

Based on **CMMI-DEV**; used by integrators/asset owners to assess supplier rigor.

| Level | CMMI-DEV | 62443-4-1 | Meaning |
|-------|----------|-----------|---------|
| **1** | Initial | Initial | Ad-hoc, undocumented; no repeatability |
| **2** | Managed | Managed | Written policies; trained staff; **not yet practiced on all procedures** |
| **3** | Defined | Defined (Practiced) | Repeatable org-wide; level-2 processes **practiced** on ≥1 product |
| **4–5** | Quant. Managed / Optimizing | **Improving** | Metrics-driven; continuous improvement (combined) |

**CCLI target for first audit:** **ML 2 minimum**, **ML 3** on critical practices (SM-8 keys, SUM patch, DM vuln).

**NOTE (p. 19):** Even at low ML, **SM-5** requires all **applicable** requirements in the product development lifecycle when building to **4-2** / **3-3**.

---

## Practice 1 — Security management (SM-1…SM-13) — **COMPLETE**

**Purpose (§5.1):** Security-related activities planned, documented, and executed across the product lifecycle. Misalignment with CM or IT policies can jeopardize the SDL.

### SM checklist — CCLI artefact mapping

| Clause | Requirement summary | CCLI deliverable | Status |
|--------|---------------------|------------------|--------|
| **SM-1** | Documented dev/maintenance process: CM + change control + audit log; requirements traceability; modular design; V&V; record review/approval; lifecycle support | SDL process doc — reuse **ATEX QMS** / ISO 9001 | GAP |
| **SM-2** | Identify org roles/personnel for each required process (RACI matrix) | SDL roles matrix | GAP |
| **SM-3** | Process to identify which products/parts 4-1 applies to | Product scope statement (TG-524 CCI in scope) | PART |
| **SM-4** | Security training/assessment for personnel in SM-2 roles | Role-specific security training records | GAP |
| **SM-5** | Scoping with **documented security analysis** justification; approved by SM-4 expertise | Scoping doc per product (e.g. fuzz test out if no external interfaces) | GAP |
| **SM-6** | Integrity verification for scripts, executables, important files (hashes / signatures) | Signed `.ipk` + publish checksums | PART |
| **SM-7** | Procedural + technical controls protecting product during dev/production/delivery | Secure CI, repo access, build environment (ISO 27001/27002) | GAP |
| **SM-8** | **Procedural + technical controls for code-signing private keys** | **Key ceremony SOP** (K6.3) | **MISS** |
| **SM-9** | Identify/manage security risks of **externally provided** components | Third-party BOM: OpenWrt feeds, libiec61850, TesPro BSP | PART |
| **SM-10** | Third-party **custom-developed** components must conform to 4-1 when security-relevant | TesPro BSP / custom carrier FW contract clauses | GAP |
| **SM-11** | No release until security issues tracked to closure (CVSS above residual risk) | Release gate checklist → Clauses 6–10 | GAP |
| **SM-12** | Verify all applicable SM-5-scoped processes completed with records before release | SDL audit record per release | GAP |
| **SM-13** | Continuous SDL improvement; analyse field-escaped defects | Table 2 activities; post-incident RCA | GAP |

### SM-1 — Development process (pp. 21–22)

**Requirement:** Documented and enforced product development/maintenance/support process, integrated with commonly accepted processes including:

- a) configuration management with change controls and audit logging  
- b) product description and requirements definition with **requirements traceability**  
- c) software/hardware design and implementation (e.g. modular design)  
- d) repeatable testing V&V  
- e) review and approval of all development process records  
- f) lifecycle support  

**Guidance:** ISO 9001 · ISO/IEC 27034 cited as compliant examples.

**CCLI:** Map to git + issue tracker + architecture DB traceability (`Architecture/architecture.db`).

### SM-8 — Controls for private keys (pp. 23–24) — **P0**

**Requirement:** Procedural and technical controls protecting **private keys used for code signing** from unauthorized access or modification.

**Guidance:** Private keys are the **root of trust** — extra protection against theft/modification.

**CCLI:** TPM-backed signing (K6.2); offline HSM or secure build agent; **K6.3 SOP** mandatory before cert.

### SM-9 — Externally provided components (pp. 24)

**Requirement:** Process to identify and manage security risks of all externally provided components.

**Assessment criteria:** alignment with security context; implementation rigor; V&V performed; vuln notification path; documentation sufficiency; supplier/community support.

**Examples:** CVE tracking for OpenWrt packages; COTS SDL evaluation; compensating controls (static analysis).

**CCLI:** SBOM + OpenWrt feed pinning (K6.7); ISO/IEC 27036-3 for supply chain.

### SR-2 — Threat model (pp. 27–28) — links K6.4

**Requirement:** Threat model per product scope including (where applicable):

- information flows, **trust boundaries**, processes, data stores  
- external entities, internal/external protocols  
- physical ports (debug, JTAG), attack vectors  
- threats + CVSS severity, mitigations, external dependencies (drivers, third-party code)  

**Maintenance:** Reviewed by dev team; **re-reviewed ≥ annually** for released products; issues → DM (10.4/10.5).

**CCLI:** Extend `CCI_62443_Zones.md` threat table; STRIDE per Eth_A/B/plant/USB/LTE.

### Table 2 — SDL continuous improvement (p. 26)

| Activity | Benefit |
|----------|---------|
| CVE DB → improve threat model | Keep TM current with field issues |
| OWASP / industry SDL groups | Emerging threats + best practices |
| Internal SDL sessions | Org-wide expertise |
| Root-cause analysis on field vulns | Corrective action across all practices |
| Manual + automated pen-test combo | Better SVV coverage |
| Custom fuzz tools for proprietary protocols | 104/MMS/Modbus fuzz before attackers |
| Dedicated security test experts | SVV-3 depth |

---

## Practice 2 — Specification of security requirements (SR-1…SR-5) — **COMPLETE**

**Purpose (§6.1):** Document **security capabilities** (auth, encryption, audit, …) and **product security context** (physical security, firewall environment).

### SR checklist — CCLI mapping

| Clause | Requirement summary | CCLI deliverable | Status |
|--------|---------------------|------------------|--------|
| **SR-1** | Document **product security context** (network location, env physical/cyber security, isolation, impact) | Lab platform + deployment assumptions doc | PART |
| **SR-2** | **Threat model** (see above) | Zone threat model | PART |
| **SR-3** | Document security requirements for install/operate/maintain/decommission | Product security reqs → trace to 3-3 SR IDs | PART |
| **SR-4** | Requirements include scope/boundaries + **target SL-C** | `{2222222}` + 4-2 component profile | PART |
| **SR-5** | Requirements reviewed by implementers, **independent** testers, customer advocate, security advisor | Architecture/security review minutes | GAP |

### SR-1 — Product security context (p. 27)

Document minimum environment assumptions: network location; physical/cyber security provided by deployment; isolation; potential impact (life, production, …).

**Examples:** No physical security → no pushbutton config; external firewall → product may not need own firewall.

**CCLI:** Document Z0–Z5 assumptions; DSO cabinet vs field cabinet; LTE as untrusted (Z5).

### SR-3 / SR-4 — Product security requirements + SL-C (pp. 28–29)

**SR-3:** Technical reqs (password policy) + business reqs (data handling, SoD) across full lifecycle including:

- a) security privileges for install/operate/maintain  
- b) security options; remove default passwords  
- c) decommissioning (purge sensitive data)  

**NOTE:** SL-C 1–4 defined in **62443-3-3** and **62443-4-2**.

**SR-4:** Requirements must include physical/logical scope and **required SL-C**.

**CCLI:** CRS from 3-3 extract + zones doc; target **SL-C = 2** per FR for TG-524 CCI.

### SR-5 — Security requirements review (p. 29)

Review for clarity, validity, alignment with threat model, verifiability. Participants: architects/developers, **independent testers**, customer advocate, security advisor.

---

## Practice 3 — Secure by design (partial — SD-1 started, p. 30)

### SD-1 — Secure design principles (partial)

**Requirement:** Process to develop secure design identifying **every physical and logical interface** including:

- a) external vs internal accessibility  
- b) security context implications on external interfaces  
- c) potential users and accessible assets  
- d) trust boundary crossings  
- e) security assumptions/constraints + threats  
- f) roles, privileges, access control  
- g) security capabilities / compensating mechanisms (input validation, error handling)  
- h) third-party products for interface  
- i) external interface usage documentation  
- j) threat mitigations from threat model  

**CCLI interfaces to characterize:** Eth_A (61850), Eth_B (104), plant (Modbus), USB (Z4), LTE (Z5), RS485, console.

**Gap:** SD-2…SD-4 (pp. 31–32) not yet captured.

---

## Practice 4 — Secure implementation (SI-1…SI-2) — **COMPLETE**

**Purpose (§8):** Ensure implementation matches secure design and follows coding best practices.

### SI checklist — CCLI mapping

| Clause | Requirement summary | CCLI deliverable | Status |
|--------|---------------------|------------------|--------|
| **SI-1** | Implementation reviews: missed SRs; coding standard compliance; **SCA** on all source changes; traceability to design; threat/exploit review at interfaces | Code review checklist + CI static analysis | GAP |
| **SI-2** | Maintain security coding standards (banned functions, trust-boundary input validation, error handling) | C/C++ secure coding guide for `apps/ccli/` | GAP |

### SI-1 — Security implementation review (pp. 33–34)

**Requirement:** Implementation reviews identify, characterize, and track security issues including:

- a) SRs from Clause 6 not adequately implemented  
- b) secure coding standard violations (banned functions, least privilege)  
- c) **Static Code Analysis (SCA)** on all source — tool-based if available; on every change  
- d) traceability of implementation to security capabilities (Clause 7)  
- e) examination of threats exploiting interfaces, trust boundaries, assets (7.2, 7.3)  

**Guidance:** Manual design review + manual code review + automated SCA for vulnerabilities and rule violations.

**CCLI:** clang-tidy / cppcheck in CI; review gates on `apps/ccli/` PRs; map to 3-3 SR trace matrix.

**Table 3 note:** SCA under SI-1 — **no independence** required (developer may run).

### SI-2 — Secure coding standards (p. 34)

**Requirement:** Implementation processes incorporate periodically reviewed standards including:

- a) avoid exploitable design patterns  
- b) avoid **banned functions** / weak constructs  
- c) automated tool settings (static analysis)  
- d) secure coding practices  
- e) **validate all inputs crossing trust boundaries**  
- f) error handling  

**CCLI:** CERT C / MISRA subset; ban `strcpy`, unchecked buffers; validate MMS/104/Modbus inputs at zone borders.

---

## Practice 5 — Security verification and validation (SVV-1…SVV-5) — **COMPLETE**

**Purpose (§9.1):** Document security testing to verify all security requirements met in product security context and defense-in-depth configuration. Issues → **Practice 6 (DM)**.

### SVV checklist — CCLI mapping

| Clause | Requirement summary | CCLI deliverable | Status |
|--------|---------------------|------------------|--------|
| **SVV-1** | Functional security testing; performance/scalability; boundary/stress/malformed input | Lab test plan vs 3-3/4-2 SR rows | GAP |
| **SVV-2** | Test each threat-model mitigation; attempt to thwart mitigations (STRIDE) | Abuse-case test matrix from SR-2 | GAP |
| **SVV-3** | Vulnerability testing: fuzz, attack surface, vuln scan, **SCA/SBOM**, runtime resource tests | Fuzz Eth_A/B/104/Modbus; SBOM scan (K6.7) | GAP |
| **SVV-4** | Penetration testing — exploit CIA via chained vulns | Subcontract pen-test or lab | GAP |
| **SVV-5** | Tester **independence** per **Table 3** | QA dept or external lab | GAP |

### SVV-1 — Security requirements testing (p. 35)

Functional testing of security capabilities including: general security features; API; permission delegation; anti-tamper/integrity; **signed image verification**; secure secret storage.

### SVV-2 — Threat mitigation testing (p. 35)

Test plans per threat-model mitigation; include attempts to **defeat** each mitigation. STRIDE example: test auth bypass for spoofing. Layered defense — test each layer.

### SVV-3 — Vulnerability testing (p. 36)

Based on industry-recognized public vulnerability sources:

- a) **abuse case / fuzz** on all external interfaces (104, MMS, Modbus, LTE)  
- b) **attack surface analysis** — ports, ACLs, privileged services  
- c) black-box known-vulnerability scanning  
- d) **software composition analysis (SCA)** — known CVEs in binaries, vulnerable libs, compiler flags  
- e) dynamic runtime testing — DoS, memory leaks, shared memory  

**CCLI:** OpenWrt package CVE scan + custom protocol fuzz; aligns with **SR 7.8** component inventory (3-3).

### SVV-4 — Penetration testing (p. 36)

Tests focused on **discovering and exploiting** vulnerabilities; defeat CIA; chained vulns (auth bypass → admin → break encryption).

### Table 3 — Tester independence (p. 37)

| Test type | Reference | Independence required |
|-----------|-----------|----------------------|
| Security requirements testing | SVV-1 | **Independent department** |
| Threat mitigation testing | SVV-2 | **Independent department** |
| Abuse case / fuzz | SVV-3 | **Independent person** |
| Static code analysis | SI-1 | **None** (dev may run) |
| Attack surface analysis | SVV-3 | **Independent person** |
| Known vulnerability scanning | SVV-3 | **Independent person** |
| Software composition analysis | SVV-3 | **None** |
| Penetration testing | SVV-4 | **Independent dept or org** |

**Levels:** None · Independent person · Independent department (QA) · Independent organization (external lab).

**CCLI:** Use external lab for SVV-4 pen-test; internal QA (not dev manager) for SVV-1/2.

---

## Practice 6 — Management of security-related issues (DM-1…DM-6) — **COMPLETE**

**Purpose (§10.1):** Handle security issues for product configured with defense-in-depth in its security context.

### DM checklist — CCLI mapping

| Clause | Requirement summary | CCLI deliverable | Status |
|--------|---------------------|------------------|--------|
| **DM-1** | Receive/track issues from testers, third parties, devs, users, researchers | Vuln disclosure page + ISO/IEC 29147 | GAP |
| **DM-2** | Timely investigation: applicability, verifiability, threats | Triage SOP (ISO/IEC 30111) | GAP |
| **DM-3** | CVSS + impact vs security context, defense-in-depth, root cause | CVSS + essential-function check (3-3) | GAP |
| **DM-4** | Fix / plan / defer / accept below residual risk | Release gate; link to SUM | GAP |
| **DM-5** | Disclose reportable issues to users (CVSS, affected versions, resolution) | Coordinated disclosure policy (CRA) | GAP |
| **DM-6** | **Annual** review of defect management process completeness | SDL audit record | GAP |

### DM-5 — Disclosing security-related issues (p. 41)

Inform users of **reportable** resolved issues including: issue description + **CVSS** (or similar); affected version(s); resolution description (may reference patches — Clause 11).

**Guidance:** IEC 62443-2-4 for service providers; IEC 29147 notification content; informational alerts for compensating mechanisms or non-susceptibility to publicized CVEs.

**CCLI:** security@hiteks.it (or equivalent); DSO notification path for Eth_A-facing vulns.

### DM-6 — Periodic review (p. 42)

Review defect management process **≥ annually** — completeness, efficiency, resolution of all issues since last review.

---

## Practice 7 — Security update management (SUM-1…SUM-5) — **COMPLETE**

**Purpose (§11.1):** Security updates tested for regressions and delivered to users timely.

### SUM checklist — CCLI mapping

| Clause | Requirement summary | CCLI deliverable | Status |
|--------|---------------------|------------------|--------|
| **SUM-1** | Qualify patches: fix intended vuln; no regressions; no safety/legal conflict | OTA regression test checklist | GAP |
| **SUM-2** | Document patches: apply manual/auto; reboot impact; verify applied; risk if not applied | Patch release notes + LuCI/sysupgrade guide | GAP |
| **SUM-3** | Document dependent OS/component update compatibility + mitigations | OpenWrt feed compatibility matrix | GAP |
| **SUM-4** | Deliver updates with **authenticity verification** | Signed `.ipk` / sysupgrade (K2.7) | PART |
| **SUM-5** | Patch delivery **SLA policy** (impact, public knowledge, exploits, volume, mitigations) | e.g. 30/60/90-day tiers | GAP |

### SUM-1 — Security update qualification (p. 42)

Verify patches from: product developer; component suppliers; platform dependencies. Confirm no conflict with operational, **safety**, or legal constraints.

**CCLI:** PF2 curtailment regression test on every security patch before release.

### SUM-4 / SUM-5 — Delivery + timeliness (pp. 43–44)

**SUM-4:** Updates available with mechanism to verify patch is **authentic** (signatures — links SM-6/SM-8).

**SUM-5:** Policy defining qualification + delivery timeframes considering: vuln impact; public knowledge; published exploits; deployed volume; effective mitigations. Example industry practice: **30 / 60 / 90 day** tiers.

**CCLI:** A/B OTA + Secure Boot rollback; document SLA in operator manual (SG-3).

---

## Practice 8 — Security guidelines (SG-1…SG-7) — **COMPLETE**

**Purpose (§12.1):** User documentation for integrating, configuring, and maintaining defense-in-depth per product security context. Complements **62443-2-4** service-provider hardening.

Covers: policies/procedures; architecture (firewall, compensating controls); security settings (firewall rules, certs, accounts); hardening tools. **Patching** addressed in Clause 11, not here.

### SG checklist — CCLI mapping

| Clause | Requirement summary | CCLI deliverable | Status |
|--------|---------------------|------------------|--------|
| **SG-1** | Document product defense-in-depth: capabilities, threats addressed, user mitigations | Operator security guide §1 | GAP |
| **SG-2** | Document **environment** measures expected (external firewall, physical security) | Deployment assumptions (SR-1) | PART |
| **SG-3** | Hardening guidelines: integration, APIs, defense-in-depth config, security options | LuCI hardening + UCI defaults doc | GAP |
| **SG-4** | Secure disposal: remove from env, purge config, wipe data, physical disposal | Factory reset + decommission SOP | GAP |
| **SG-5** | Secure operation: user/admin responsibilities and assumptions | Operator manual security chapter | GAP |
| **SG-6** | Account management: permissions, default accounts/password change | First-boot wizard + SG-6 checklist | PART |
| **SG-7** | Review all user manuals for security coverage and accuracy | Doc review gate before release | GAP |

### SG-3 — Security hardening guidelines (pp. 45–46)

Must include instructions for: integration with security context; API/protocol integration; maintaining defense-in-depth; each security option's role, defaults, configurable values, impact on work practices; security tools; periodic maintenance; incident reporting to supplier; admin best practices.

**CCLI:** Document Eth_A/B firewall rules, disable Wi‑Fi, LTE VPN-only, USB Z4 policy, 62351 TLS enablement path.

### SG-4 — Secure disposal (p. 46)

Remove from environment; purge config references; secure data wipe; physical disposal guidance for non-wipeable storage.

**CCLI:** Overlay wipe + TPM key clear procedure; ties to SR-4 decommissioning + 3-3 SR 4.2.

---

## Annex A — Possible metrics (p. 49, partial)

Informative SDL effectiveness metrics (examples):

| Metric | Formula (example) | Use |
|--------|---------------------|-----|
| Attack vectors mitigated | A/N × 100 | Threat model coverage |
| Current backlog | MIN((50·CSI)+(10·ISI), 100) | Critical/important open issues |
| Training | 1 − (CA/SE) × 100 | Engineer assessment completion |
| SDL violations | 1 − (CI/CT) × 100 | Secure coding checklist compliance |

**CCLI:** Use for SM-13 continuous improvement reporting; not mandatory for cert.

---

## Annex B — Table B.1 Summary of all requirements (pp. 50–51) — **COMPLETE**

**47 requirement clauses** across 8 practices:

| Practice | Clauses |
|----------|---------|
| **SM** | SM-1…SM-13 (13) |
| **SR** | SR-1…SR-5 (5) |
| **SD** | SD-1…SD-4 (4) |
| **SI** | SI-1…SI-2 (2) |
| **SVV** | SVV-1…SVV-5 (5) |
| **DM** | DM-1…DM-6 (6) |
| **SUM** | SUM-1…SUM-5 (5) |
| **SG** | SG-1…SG-7 (7) |

Use Table B.1 as **SDL audit checklist** for EDSA/SDLA; map each row to CCLI artefact in SDL evidence folder.

---

## Practices overview — capture status

| § | Practice | Clauses | Status |
|---|----------|---------|--------|
| **5** | Security management | SM-1…SM-13 | **COMPLETE** |
| **6** | Specification of security requirements | SR-1…SR-5 | **COMPLETE** |
| **7** | Secure by design | SD-1…SD-4 | SD-1 + **Annex B**; pp. 31–32 body optional |
| **8** | Secure implementation | SI-1…SI-2 | **COMPLETE** |
| **9** | Security V&V testing | SVV-1…SVV-5 | **COMPLETE** |
| **10** | Management of security-related issues | DM-1…DM-6 | **COMPLETE** |
| **11** | Security update management | SUM-1…SUM-5 | **COMPLETE** |
| **12** | Security guidelines | SG-1…SG-7 | **COMPLETE** |

---

## CCLI SDL artefact map (draft)

| Practice | Priority clause | CCLI deliverable | Node |
|----------|-----------------|------------------|------|
| SM | SM-7 | Secure dev environment (repo, CI, secrets) | K2 |
| SM | **SM-8** | **Private key / signing key controls** | **K6.3** |
| SM | SM-9/10 | Third-party BOM (OpenWrt, libiec61850, TesPro BSP) | K6.7 |
| SR | SR-2 | Threat model document | K6.4 |
| SR | SR-3/4 | Product security requirements ↔ 3-3 SR IDs | K6.4 |
| SD | SD-2 | Defence in depth / zone architecture | K3.3 |
| SI | SI-2 | Secure coding standard | K4 |
| SVV | SVV-1…4 | V&V + pen-test plan | K8 |
| DM | DM-1…5 | Vulnerability intake + disclosure | K6.7 / CRA |
| SUM | SUM-1…5 | Signed update + patch SLA | K2.7 |
| SG | SG-3…6 | Operator hardening + disposal guide | K7 |

---

## Interface matrix (4-1 → other corpus)

| 4-1 output | Consumer | Corpus link |
|------------|----------|-------------|
| Product security requirements | Design / CRS | `ccli-62443-3-3-extract` |
| Threat model | Zone/conduit design | `ccli-62443-zones-extract` |
| SDL evidence pack | Cert lab / EDSA | MISSING |
| Technical capabilities proof | Component test | `iec-62443-4-2` MISSING |

---

## BOM / standards matrix

| Block | Function | Norm | Status |
|-------|----------|------|--------|
| SDL process | How product is built | **62443-4-1** | **HAVE** — 47 clauses + Annex B (pp. 6–51); SD body gap pp. 31–32 optional |
| Component proof | What product must do | 62443-4-2 | MISSING |
| System SR/SL-C | Integration requirements | 62443-3-3 | PART (ingest pending) |
| Zones / SL-T | Risk / segmentation | 62443-3-2 | PART |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| §1–4 foundation | **HAVE** — pp. 6–10, 16–20 | — | — |
| Practice 1 (SM) | **HAVE** — SM-1…SM-13 | — | — |
| Practice 2 (SR) | **HAVE** — SR-1…SR-5 | — | — |
| Practice 3 (SD) | SD-1 + Annex B names | SD-2…SD-4 body (pp. 31–32) | Optional |
| Practice 4 (SI) | **HAVE** | — | — |
| Practice 5 (SVV) | **HAVE** | — | — |
| Practice 6 (DM) | **HAVE** — DM-1…DM-6 | — | — |
| Practice 7 (SUM) | **HAVE** — SUM-1…SUM-5 | — | — |
| Practice 8 (SG) | **HAVE** — SG-1…SG-7 | — | — |
| Annex A metrics | Partial (p. 49) | Full Annex A (p. 48) | Optional |
| Annex B Table B.1 | **HAVE** | — | — |
| SDL evidence folder | None | Living process docs | Engineering |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-641-001 | SDL docs written after product shipped | Cert fail | Start SM/SR artefacts now (reuse ATEX QMS) |
| R-641-002 | 4-1 audited without 4-2 lab scope | Mismatch | Single lab quote covering both |
| R-641-003 | ML 2 claimed but SM-8 keys ad-hoc | SM-8 fail | K6.3 key ceremony SOP first |
| R-641-004 | Pen-test by dev team (SVV-4) | Table 3 fail | External lab or independent org |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| Batch A | Page quality review | Scope, Figs 1–3, Table 1 PASS |
| Batch B | SM-1…SM-13 + SR-1…SR-5 in extract | All requirement clauses captured |
| Session 4 | SI, SVV, DM-1…4 in extract | Table 3 independence captured |
| Final batch | DM-5…6, SUM, SG, Annex B | All 47 clauses in extract |
| SM-8 / K6.3 | SDL audit prep | Key ceremony SOP exists |
| ML target | Cert scoping call | Lab confirms ML 2–3 acceptable |
| Traceability | Design review | Each 3-3 SR has SDL artefact owner |

---

## Source pages captured

| Page | Content | Quality |
|------|---------|---------|
| **6** | Foreword — TC 65 | PASS |
| **7** | §1 Scope; §2 Normative refs; §3.1.1 abuse case | PASS |
| **8** | Introduction — SDLA lineage | PASS |
| **9** | **Figure 1** — 62443 series | PASS |
| **10** | **Figure 2** — product lifecycle roles | PASS |
| **16** | §3.1.36–41 threat, zone, trust boundary | PASS |
| **17** | §3.2 acronyms; §3.3 Conventions | PASS |
| **18** | §4.1 Concepts; **Figure 3** start | PASS |
| **19** | Figure 3 cont.; §4.2 Maturity model | PASS |
| **20** | **Table 1** maturity levels; §5 Practice 1 header | PASS |
| **21–22** | SM-1 dev process; SM-2 roles; SM-3 applicability | PASS |
| **22–23** | SM-4 expertise; SM-5 scoping; SM-6 file integrity | PASS |
| **23–24** | SM-7 dev environment; **SM-8 private keys** | PASS |
| **24–25** | SM-9 external components; SM-10 custom third-party | PASS |
| **25–26** | SM-11 release gate; SM-12 process verification; SM-13 improvement; **Table 2** | PASS |
| **26–27** | Practice 2 purpose; **SR-1** security context | PASS |
| **27–28** | **SR-2** threat model | PASS |
| **28–29** | SR-3 product reqs; **SR-4** SL-C; **SR-5** review | PASS |
| **30** | Practice 3 start; **SD-1** interface characterization (partial) | PASS |
| **33–34** | **SI-1** implementation review + SCA; **SI-2** secure coding standards | PASS |
| **34–37** | **SVV-1…SVV-5**; **Table 3** tester independence | PASS |
| **38–40** | Practice 6 — **DM-1…DM-4** (receive, review, assess, address) | PASS |
| **41** | **DM-5** disclose reportable issues (CVSS, resolution) | PASS |
| **42–44** | **DM-6** annual review; **SUM-1…SUM-5** patch qualify/doc/deliver/SLA | PASS |
| **44–47** | **SG-1…SG-7** operator hardening, disposal, accounts, doc review | PASS |
| **49** | Annex A metrics examples (partial) | PASS |
| **50–51** | **Annex B Table B.1** — all 47 requirements | PASS |

### Optional remaining capture

```
31, 32, 48    ← SD-2…SD-4 body text · Annex A start
```

---

## Related RAG sources

| source_id | Role |
|-----------|------|
| `ccli-62443-4-1-extract` | This document |
| `ccli-62443-4-1-capture-plan` | Batch order |
| `ccli-62443-3-3-extract` | System SR/SL-C |
| `ccli-62443-zones-extract` | Zones / SL-T |
| `iec-62443-4-2` | Component cert target |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-08-11 | Batch A ingest pp. 6–10, 16–20 — scope, series Figs, maturity Table 1 |
| 1.1 | 2026-08-11 | Batch B pp. 21–30 — Practice 1 SM-1…SM-13; Practice 2 SR-1…SR-5; SD-1 partial |
| 1.2 | 2026-08-11 | Session 4 pp. 33–40 — Practices 4–5 complete; Practice 6 DM-1…DM-4 partial |
| 1.3 | 2026-08-11 | pp. 41–51 — Practices 6–8 complete; Annex B Table B.1 (47 clauses) |

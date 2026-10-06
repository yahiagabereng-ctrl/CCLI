# IEC 62443-3-3 — CCLI Engineering Extract

**Document ID:** CCLI-SEC-62443-003  
**Revision:** 2.0  
**Date:** 2026-08-11  
**RAG source_id:** `ccli-62443-3-3-extract`  
**Normative basis:** IEC 62443-3-3:2013 — *System security requirements and security levels*  
**Capture status:** **HAVE** — FR 1–7 body (pp. 24–66); Annex A/B (pp. 67–78)  
**Parents:** `ccli-62443-zones-extract` (SL-T 2) · `ccli-62443-3-3-capture-plan`  
**Programme link:** K6.4 · K6.5 · KR-012

---

## Summary

**62443-3-3** defines **System Requirements (SR)** and **Requirement Enhancements (RE)** for seven **Foundational Requirements (FR 1–7)**, mapped to **SL-C** (capability security level). Used with **62443-3-2** zone/conduit **SL-T** assignments to compile the control-system requirements list and verify **SL-A** (achieved).

For **HiTEKS CCLI / TG-524:** SL-T **2** is set in `CCI_62443_Zones.md`. This extract covers **FR 1–7** with **SL-C 2 checklists**, **Annex A** SL vector methodology, and **Annex B Table B.1** master mapping.

**Proposed SL vector (Z0 / C1 / C2):**

```
SL-T(CCI Z0) = { IAC  UC  SI  DC  RDF  TRE  RA } = { 2  2  2  2  2  2  2 }
Short form: {2222222}
```

**Component lab proof** still targets **62443-4-2**. Only FR 1 design gap at SL-C 2: **SR 1.8 PKI** (K6.5 / 62351).

---

## Requirements (from §1 Scope)

62443-3-3 provides technical **SRs** for seven FRs (from **62443-1-1**):

| FR | Foundational requirement | CCLI priority |
|----|-------------------------|---------------|
| **FR 1** | Identification and authentication control (IAC) | P0 — console, 62351, LTE |
| **FR 2** | Use control (UC) | P0 — RBAC, firewall |
| **FR 3** | System integrity (SI) | P0 — Secure Boot, signed images |
| **FR 4** | Data confidentiality (DC) | P0 — TLS on C1/C2 |
| **FR 5** | Restricted data flow (RDF) | P0 — zone segmentation |
| **FR 6** | Timely response to events (TRE) | P1 — audit / alerts |
| **FR 7** | Resource availability (RA) | P1 — service hardening |

**Scope boundary (§1):** This part defines **SL-C(control system)** requirements. **SL-T** and **SL-A** assignment methodology is in **62443-3-2**; 3-3 supplies the SR/RE checklist and SL-C mapping used after zones/conduits are defined.

**Normative references (§2):** IEC 62443-1-1:2009 · IEC 62443-2-1 (security program — non-technical SRs).

---

## Architecture — 62443 series placement (Figure 1, §0.3)

```
General        62443-1-1 Terminology · 1-2 Glossary · 1-3 Metrics · 1-4 Lifecycle
Policies       62443-2-1 Security program · 2-3 Patch management · 2-4 Supplier install
System         62443-3-1 Security technologies · 3-2 Zones/SL-T · 3-3 SR/SL-C  ← this doc
Component      62443-4-1 Product SDLC · 4-2 Component technical reqs  ← TG-524 cert
```

**Workflow (§0.3 + link to 3-2):**

1. **3-2** — Define SuC, zones/conduits, assign **SL-T** per zone/conduit  
2. **3-3** — Apply **SRs + REs**; map to **SL-C**; compile requirements list  
3. Design + compensating countermeasures where SR exceptions apply  
4. Verify completeness → document **SL-A**  

**4-2 relationship:** Vendor-centric derived requirements for **embedded / host / network / application** subsystems; refined using 3-2 SL-T and 1-3 metrics. TG-524 CCI = primarily **embedded + application** under 4-2.

---

## Key concepts (§0.1–0.2)

| Concept | Definition / rule | CCLI implication |
|---------|-------------------|------------------|
| **IACS vs IT** | IACS prioritises **availability**, plant protection, degraded mode, time-critical response | Do not apply IT-only controls that break PF2 / curtailment paths |
| **Essential function** | Required for HSE and equipment-under-control availability | PF2 observability + curtailment = essential — security must not disable without degraded mode |
| **SR / RE** | Baseline requirement + optional enhancement for higher SL | Trace each SR *.4 SL-2 row in design matrix |
| **Compensating countermeasure** | Alternative control when SR violates operational need | Document if e.g. USB console bypasses network SR — affects **SL-A** |
| **System-level compliance** | Not every component must meet every SR individually | Zone-level controls (firewall, TPM) can compensate |
| **Assumptions** | Security program per **2-1**; patch mgmt per **2-3** | Align with OpenWrt feed update policy (K6.7 SBOM) |

**Audience:** asset owner, integrator, product supplier, service provider, compliance authority.

---

## Terms ingested (§3.1 — selected)

| Term | Definition | CCLI usage |
|------|------------|------------|
| **Asset** | Physical/logical object of value to IACS | TG-524, firmware, keys, Eth_A/B ports |
| **Asset owner** | Responsible for IACS; includes operator | DSO / integrator at ZCR 7 |
| **Attack** | Intelligent threat; inside/outside; active/passive | Threat table in zones doc |
| **Authentication** | Assurance claimed identity is correct | Console, MMS client, VPN |
| **Authenticator** | Password, token, cert | TPM-backed keys (K6.2) |
| **Countermeasure** | Action/device/procedure reducing threat/vulnerability | Firewall, Secure Boot, 62351 TLS |
| **Degraded mode** | Continued essential function despite faults | Must preserve curtailment path design |
| **DMZ** | Limited network joining zones; controls **data flow** | Model LTE/WAN as separate zone Z5 — not L2 bridge |
| **Device** | Processor asset sending/receiving data | TG-524, analyzers, RTUs |
| **Essential function** | HSE + availability for equipment under control | PF2 FSM — do not break for security scans |
| **Event** | Change in circumstances — human or system | Audit log source (FR 6) |
| **Firecall** | Emergency access method | Define policy for locked-out maintainer (avoid ad-hoc bypass) |
| **Trust / untrusted** | Relied upon to behave as expected / not meeting trust reqs | LTE/WAN = untrusted (SR 1.13); DSO link policy TBD |
| **Zone** | Assets sharing common security requirements; clear border | Z0–Z5 in zones doc — aligns with 3-2 |

Terms from **62443-1-1** and **62443-2-1** also apply.

### §3.2 Acronyms (selected)

| Acronym | Meaning | CCLI |
|---------|---------|------|
| **FR** | Foundational requirement | FR 1–7 |
| **SR** | System requirement | Trace in design matrix |
| **RE** | Requirement enhancement | Higher SL bundles |
| **SL-T / SL-A / SL-C** | Target / Achieved / Capability security level | SL-T 2 from 3-2 |
| **IAC / UC / SI / DC / RDF / TRE / RA** | Seven FR abbreviations | Per-FR sections |
| **SuC** | System under consideration | CCI + connected paths |
| **PKI / CA / OCSP** | Public key infrastructure | 62351 TLS (K6.5) |
| **TPM** | Trusted platform module | K6.2 on TG-524 |

### §3.3 Conventions

- Each **FR** (from 62443-1-1) expands into **SRs**; each SR has baseline + optional **REs**.
- Requirements map to **SL-C(FR, control system) 1–4** — incremental per FR.
- **SL vector** (full composition): see **Annex A** (pp. 67–74) — `{2222222}` proposed for Z0.
- Example (**FR 4 DC**): SL 1 = casual disclosure; SL 2 = simple means/low resources; SL 3 = sophisticated/moderate; SL 4 = extended/high motivation.

---

## §4 Common control system security constraints

### §4.2 Support of essential functions

Security measures **shall not adversely affect essential functions** (HSE + equipment-under-control availability) unless supported by risk assessment (62443-2-1).

| Constraint | Rule | CCLI SR refs |
|------------|------|--------------|
| IAC / UC | Must not prevent essential functions | SR 1.3, 1.4, 1.11, 2.5 |
| Essential accounts | Must not be locked out | Account policy for curtailment path |
| PKI failure | CA failure must not interrupt essential functions | SR 1.8 — design 62351 accordingly |
| SIF initiation | IAC must not block SIF | Site-dependent PF2 DO |
| Audit delay | Non-repudiation must not add significant response delay | SR 2.12 |
| Zone boundary | Essential functions in fail-close / island mode | SR 5.2 |
| DoS | DoS on control/SIS network must not block SIF | SR 7.1 |

**CCLI:** PF2 observability + curtailment = essential — authentication on local emergency actions must not block degraded-mode operation.

### §4.3 Compensating countermeasures

- Phrasing **"control system shall provide the capability…"** — may be fulfilled by **external component** with interface to control system (per **62443-3-2**).
- Examples: centralized ID, password strength, signature checking, event correlation, decommissioning.
- **NOTE:** Higher SL is not always better — increases lockout risk and cost; external CM (physical security) may justify lower SL-C for IAC.

### §4.4 Least privilege

Control system shall enforce **least privilege**: granular permissions, flexible role mapping, individual accountability when required.

**CCLI:** RBAC in `apps/ccli/` + OpenWrt service separation; per-role MMS/104/Modbus access.

---

## FR 1 — Identification and authentication control (IAC)

### §5.1 SL-C(IAC) descriptions

| SL | Protection |
|----|------------|
| **1** | Casual / coincidental unauthenticated access |
| **2** | Intentional — **simple means, low resources, generic skills, low motivation** |
| **3** | Sophisticated — moderate resources, IACS-specific skills |
| **4** | Extended resources, high motivation |

**CCLI target:** **SL-C(IAC) = 2** for Z0 (matches SL-T 2).

### FR 1 — SL-C(IAC) 2 checklist (SR 1.1–1.13) — **COMPLETE**

| SR | SL-C 2 requires | Requirement summary | CCLI control | Status |
|----|-----------------|---------------------|--------------|--------|
| **1.1** | SR 1.1 + **RE 1** | Unique human user ID + auth | USB/SSH console; no shared account | PART |
| **1.2** | **SR 1.2** | Software process + device auth | MMS/Modbus peer auth | PART |
| **1.3** | **SR 1.3** | Account lifecycle mgmt | UCI users; remove defaults | PART |
| **1.4** | **SR 1.4** | Identifier mgmt | Role-based IDs | PART |
| **1.5** | **SR 1.5** | Authenticator init/change/protect | First-boot default purge | PART |
| **1.6** | **SR 1.6(1)** | Unique wireless user auth | Wi‑Fi off; LTE unique creds if used | PART |
| **1.7** | **SR 1.7** | Configurable password strength | Console password policy | PART |
| **1.8** | **SR 1.8** | PKI per best practice or external CA | 62351 cert chain (K6.5) | MISS |
| **1.9** | **SR 1.9** | Cert validate, revoke, private key control | TLS verify + TPM keys | PART |
| **1.10** | **SR 1.10** | Obscure auth feedback | Masked login; generic errors | PART |
| **1.11** | **SR 1.11** | Lockout after N failed attempts; no interactive logon for service accounts | Configurable lockout; balance §4.2 | PART |
| **1.12** | **SR 1.12** | Pre-login use notification banner | Configurable legal/audit banner | PART |
| **1.13** | **SR 1.13(1)** | Monitor/control untrusted network access + **explicit approval** | LTE/WAN VPN; role-gated remote access | PART |

### SR 1.1 — Human user identification and authentication

**Requirement:** Identify and authenticate all human users on all interfaces; support segregation of duties + least privilege.

**Enhancements:**

| RE | Capability | SL needed |
|----|------------|-----------|
| **RE 1** | Unique identification and authentication for every human user | **SL 2+** |
| **RE 2** | Multifactor authentication for **untrusted networks** | SL 3+ |
| **RE 3** | MFA for **all** networks | SL 4 |

**SL-C(IAC) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | SR 1.1 |
| **2** | **SR 1.1 + RE 1** |
| 3 | SR 1.1 + RE 1 + RE 2 |
| 4 | SR 1.1 + RE 1 + RE 2 + RE 3 |

**Guidance:** Emergency/essential functions must not be hampered (§4.2). Shared accounts only with compensating CM (physical control room). Links **SR 2.1** (authorization), **SR 1.13** (untrusted network).

**CCLI:** Eth_B operator login; USB maintainer (Z4) — unique accounts; MFA on LTE (Z5) if pursuing SL 3.

### SR 1.2 — Software process and device identification and authentication

**Requirement:** Identify and authenticate all software processes and devices on all interfaces; least privilege.

**RE 1:** Unique ID + auth for every process and device → **SL 3+**.

**SL-C(IAC) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **SR 1.2** |
| 3 | SR 1.2(1) = SR 1.2 + RE 1 |
| 4 | SR 1.2(1) |

**Guidance:** Protect against MITM/spoofing; mobile devices high risk; multicast protection may use group auth; must not block local emergency actions.

**CCLI:** DSO MMS peer authentication (62351); consider device certs for analyzer Modbus where supported.

### SR 1.3 — Account management

**Requirement:** Authorized users manage all accounts (add, activate, modify, disable, remove).

**RE 1:** Unified account management → **SL 3+**.

**SL-C(IAC) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | SR 1.3 |
| **2** | **SR 1.3** |
| 3 | SR 1.3(1) |
| 4 | SR 1.3(1) |

**Guidance:** Shared accounts acceptable with compensating CM; separate policies for **service accounts** (software-to-software); remove unused default accounts.

**CCLI:** Separate human vs service accounts for `ccli` daemons; document in K6.3 ceremony SOP.

### SR 1.4 — Identifier management

**Requirement:** Manage identifiers by user, group, role, or control system interface.

**Enhancements:** None.

**SL-C(IAC):** SR 1.4 at **all SL 1–4**.

**Guidance:** Wireless typically requires identifiers; wired may not; local emergency not hampered; policies per 62443-2-1.

**CCLI:** Role IDs: `maintainer`, `operator`, `service-ccli`, `service-61850`.

### SR 1.5 — Authenticator management

**Requirement:** Capability to:
- initialize authenticator content;
- **change all default authenticators** on installation;
- change/refresh authenticators;
- **protect authenticators** from unauthorized disclosure/modification (storage + transit).

**RE 1:** Hardware security for software-process/device credentials → **SL 3+** (e.g. **TPM**).

**SL-C(IAC) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | SR 1.5 |
| **2** | **SR 1.5** |
| 3 | SR 1.5(1) = SR 1.5 + RE 1 |
| 4 | SR 1.5(1) |

**Guidance:** Change default passwords; hash/encrypt passwords; **TPM** for key protection; firecall procedures; **lockout unacceptable** for high-availability — links SR 1.7, 1.8, 1.9.

**CCLI:** First-boot wizard changes root/console defaults; TPM 2.0 for key storage (K6.2) — positions for SL 3 on SR 1.5 RE 1 even at SL-T 2 zone (component capability headroom).

### SR 1.6 — Wireless access management

**Requirement:** Identify and authenticate all users (human, process, device) engaged in **wireless** communication.

**RE 1:** **Unique** identification and authentication for all wireless users → required at **SL 2+** (notation SR 1.6(1)).

**SL-C(IAC) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | SR 1.6 |
| **2** | **SR 1.6(1)** |
| 3 | SR 1.6(1) |
| 4 | SR 1.6(1) |

**Guidance:** Wireless = same IACS rules as wired but **physical CM less effective**; risk analysis may assign **higher SL-T** for wireless vs wired. Includes 802.11, LTE/tethering, Bluetooth, satellite, etc.

**CCLI:** Product profile **disables Wi‑Fi** (K3.7). If LTE (Z5) used: VPN + unique credentials per SR 1.6(1). Document "wireless not enabled" as scope exclusion or provide auth capability if lab enables radio.

### SR 1.7 — Strength of password-based authentication

**Requirement:** Enforce **configurable password strength** (minimum length + character variety) where passwords are used.

**RE 1:** Password reuse prevention + min/max lifetime for **human** users → **SL 3+**.  
**RE 2:** Password lifetime for **all** users → **SL 4**.

**SL-C(IAC) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | SR 1.7 |
| **2** | **SR 1.7** |
| 3 | SR 1.7(1) |
| 4 | SR 1.7(1)(2) |

**Guidance:** Dictionary/rainbow-table attacks; min password lifetime prevents rotate-back; links MFA in SR 1.1/1.2.

**CCLI:** OpenWrt/console password policy module; align with K6.3 key ceremony doc.

### SR 1.8 — Public key infrastructure (PKI) certificates

**Requirement:** Where PKI is used — operate PKI per best practices **or** obtain certs from existing PKI.

**Enhancements:** None.

**SL-C(IAC) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **SR 1.8** |
| 3 | SR 1.8 |
| 4 | SR 1.8 |

**Guidance:** Secure identity verification for cert issuance; PKI latency must not degrade control performance; RFC 3647; CA location per 62443-2-1.

**CCLI:** **Critical for K6.5** — IEC 62351 MMS/TLS on C1/C2. Use DSO/operator CA or enterprise PKI; document cert provisioning in K6.3 SOP.

### SR 1.9 — Strength of public key authentication

**Requirement:** Where public-key auth is used, capability to:
- validate cert signatures;
- build path to trusted CA (or deploy self-signed leaf to all peers securely);
- check **revocation** (CRL/OCSP) or compensate (short lifetime);
- establish user control of private key;
- map authenticated identity to user/process/device.

**RE 1:** Protect private keys via **hardware** (TPM) → **SL 3+**.

**SL-C(IAC) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **SR 1.9** |
| 3 | SR 1.9(1) |
| 4 | SR 1.9(1) |

**Guidance:** Trust chain validation essential; self-signed only with secure manual distribution; minimal trusted CA set; short-lived certs may compensate missing OCSP but create ops burden.

**CCLI:** libiec61850 + OpenSSL verify chain for MMS; TPM for server/client keys; OCSP optional if short-lived certs + documented risk acceptance.

### SR 1.10 — Authenticator feedback

**Requirement:** **Obscure** authentication feedback during login (e.g. password asterisks); no hints on failure reason ("unknown username").

**Enhancements:** None.

**SL-C(IAC):** SR 1.10 at **all SL 1–4**.

**CCLI:** Console/SSH/LuCI login — generic failure message; masked password entry.

### SR 1.11 — Unsuccessful login attempts

**Requirement:** Configurable limit on consecutive invalid access attempts within a configurable time window; deny access for a period or until admin unlock. **Service accounts** for critical services: capability to **disallow interactive logons**.

**Enhancements:** None.

**SL-C(IAC):** SR 1.11 at **all SL 1–4**.

**Guidance:** Prevents brute-force DoS; auto-reset counter after policy interval; **must not lock out operators in emergencies** (§4.2); consider continuous-operation requirements.

**CCLI:** `fail2ban`/PAM lockout on console — **exclude** or fast-reset accounts tied to essential functions; `ccli` service accounts non-interactive only.

### SR 1.12 — System use notification

**Requirement:** Display **configurable** system-use notification **before** authentication.

**Banner shall include (minimum):**
- a) Accessing a control system
- b) Usage may be monitored/recorded/audited
- c) Unauthorized use prohibited — penalties apply
- d) Use implies consent to monitoring

**Enhancements:** None.

**SL-C(IAC):** SR 1.12 at **all SL 1–4**.

**Guidance:** Legal/policy compliance; does not directly improve security; warning banner at login (physical notice insufficient for remote).

**CCLI:** LuCI/SSH pre-auth banner; Italian + English text for DSO sites.

### SR 1.13 — Access via untrusted networks

**Requirement:** Monitor and control **all** access methods via **untrusted networks** (remote dial-up, broadband, wireless, office LAN).

**RE 1:** Deny untrusted access unless **explicitly approved by assigned role** → **SL 2+** (notation SR 1.13(1)).

**SL-C(IAC) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | SR 1.13 |
| **2** | **SR 1.13(1)** |
| 3 | SR 1.13(1) |
| 4 | SR 1.13(1) |

**Guidance:** Restrict by source; VPN protection; enable remote access only when necessary + authenticated; MFA for remote users (links SR 1.1 RE 2 at SL 3).

**CCLI:** **Z5 LTE/WAN** = untrusted; VPN default-deny; role approval workflow for remote maintainer sessions; no bridge to Eth_A (K3.4).

---

## FR 2 — Use control (UC)

### §6.1 SL-C(UC) descriptions

| SL | Protection |
|----|------------|
| **1** | Casual / coincidental misuse |
| **2** | Intentional circumvention — **simple means, low resources, generic skills** |
| **3** | Sophisticated — moderate resources, IACS-specific skills |
| **4** | Extended resources, high motivation |

**CCLI target:** **SL-C(UC) = 2** for Z0.

### FR 2 — SL-C(UC) 2 checklist — **COMPLETE**

| SR | SL-C 2 requires | Requirement summary | CCLI control | Status |
|----|-----------------|---------------------|--------------|--------|
| **2.1** | SR 2.1 + **RE 1 + RE 2** | AuthZ for **all users**; role→permission mapping | RBAC in `apps/ccli/` + OpenWrt | PART |
| **2.2** | **SR 2.2** | Authorize/monitor/enforce wireless usage | Wi‑Fi off; LTE usage policy | PART |
| **2.3** | **SR 2.3** | Portable/mobile device restrictions | USB Z4 policy | PART |
| **2.4** | **SR 2.4** | Mobile code restrictions (JS/PDF/etc.) | Minimal LuCI; no consumer apps | PART |
| **2.5** | **SR 2.5** | Session lock after idle | SSH/LuCI timeout — exclude emergency WS (§4.2) | PART |
| **2.6** | **SR 2.6** | Remote session termination | Admin kill LTE/maintainer sessions | PART |
| **2.7** | *Not selected* | Concurrent session limits | SL 3+ only | N/A |
| **2.8** | **SR 2.8** | Auditable events (access, config, OS, …) | syslog/journald; UCI change log | PART |
| **2.9** | **SR 2.9** | Audit storage capacity | Log rotation + persistent store sizing | PART |
| **2.10** | **SR 2.10** | Alert on audit failure; no essential-service loss | Monitor log daemon health | PART |
| **2.11** | **SR 2.11** | Timestamps for audit records | GNSS/NTP sync (K3.5) | PART |
| **2.12** | *Not selected* | Non-repudiation | SL 3+ only | N/A |

### SR 2.1 — Authorization enforcement

**Requirement:** On all interfaces, enforce authorizations for **human users** — segregation of duties + least privilege.

**Enhancements:**

| RE | Capability | SL needed |
|----|------------|-----------|
| **RE 1** | Enforce authZ for **all users** (human, process, device) | **SL 2+** |
| **RE 2** | Authorized role can define/modify permission→role mapping | **SL 2+** |
| **RE 3** | Supervisor manual override (audited, configurable time/event) | SL 3+ |
| **RE 4** | **Dual approval** ("four-eyes") for serious process impact | SL 4 |

**SL-C(UC) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | SR 2.1 |
| **2** | **SR 2.1 + RE 1 + RE 2** |
| 3 | SR 2.1 + RE 1 + RE 2 + RE 3 |
| 4 | SR 2.1 + RE 1 + RE 2 + RE 3 + RE 4 |

**Guidance:** ACLs, matrices, crypto; verify after SR 1.1/1.2 auth; RBAC example; must not degrade operational performance; only qualified users may initiate changes.

**CCLI:** Operator vs maintainer vs service roles; curtailment commands may need dual approval at SL 4 — document PF2 policy at SL 2.

### SR 2.2 — Wireless use control

**Requirement:** Authorize, monitor, and enforce usage restrictions for **wireless connectivity** per industry practice.

**RE 1:** Identify and report **unauthorized wireless transmitters** in physical environment → **SL 3+**.

**SL-C(UC) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | SR 2.2 |
| **2** | **SR 2.2** |
| 3 | SR 2.2(1) |
| 4 | SR 2.2(1) |

**CCLI:** Wi‑Fi disabled; if LTE enabled — usage restrictions + logging; rogue AP detection out of scope unless site requires SL 3.

### SR 2.3 — Use control for portable and mobile devices (p. 39)

**Requirement:** Automatically enforce configurable restrictions:
- a) prevent use of portable/mobile devices;
- b) require context-specific authorization;
- c) restrict code/data transfer to/from portable devices.

**RE 1:** Verify connecting portable/mobile devices comply with zone security requirements → **SL 3+** SR 2.3(1).

**SL-C(UC) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | SR 2.3 |
| **2** | **SR 2.3** |
| 3 | SR 2.3(1) |
| 4 | SR 2.3(1) |

**CCLI:** USB maintainer laptop in **Z4** — context auth + restrict auto-mount; no arbitrary code load on Z0.

### SR 2.4 — Mobile code (p. 39)

**Requirement:** Enforce usage restrictions for mobile code (Java, JavaScript, ActiveX, PDF, Flash, VBScript, …): prevent execution; auth origin; restrict transfer; monitor use.

**RE 1:** Verify mobile code integrity before execution → **SL 3+** SR 2.4(1).

**SL-C(UC) mapping:** SL 1–2 = **SR 2.4**; SL 3–4 = SR 2.4(1).

**CCLI:** OpenWrt CCI image — no PDF viewers, Flash, or browser plugins on Z0.

### SR 2.5 — Session lock (p. 40)

**Requirement:** Session lock after configurable idle or manual initiation; re-auth to unlock.

**Guidance:** Not advised on operator emergency workstations (§4.2); use compensating controls if unsupported.

**SL-C(UC) mapping:** SL 1–4 = **SR 2.5** at all levels.

**CCLI:** SSH `ClientAliveInterval`; LuCI session timeout; exclude curtailment emergency path.

### SR 2.6 — Remote session termination (pp. 40–41)

**Requirement:** Terminate remote session after configurable idle or manually by session owner.

**Guidance:** Applies to sessions across zone boundaries; may limit to monitoring/maintenance (not critical ops).

**SL-C(UC) mapping:** SL 1 = Not Selected; **SL 2–4 = SR 2.6**.

**CCLI:** `kill` maintainer SSH sessions; LTE VPN session timeout.

### SR 2.7 — Concurrent session control (p. 41)

**Requirement:** Limit concurrent sessions per user (human/process/device) per interface.

**SL-C(UC) mapping:** SL 1–2 = Not Selected; SL 3–4 = SR 2.7.

### SR 2.8 — Auditable events (pp. 41–42)

**Requirement:** Generate audit records for: access control, request errors, OS events, control system events, backup/restore, configuration changes, reconnaissance, audit log events. Each record: timestamp, source, category, type, event ID, result.

**RE 1:** Centrally managed system-wide time-correlated audit trail; export to SIEM formats → **SL 3+** SR 2.8(1).

**SL-C(UC) mapping:** SL 1–2 = **SR 2.8**; SL 3–4 = SR 2.8(1).

**CCLI:** journald + remote syslog optional; UCI commit hooks for config audit.

### SR 2.9 — Audit storage capacity (pp. 42–43)

**Requirement:** Sufficient audit storage per log management best practice (NIST SP 800-92); mechanisms to reduce overflow likelihood.

**RE 1:** Warn when storage reaches configurable % of capacity → **SL 3+** SR 2.9(1).

**SL-C(UC) mapping:** SL 1–2 = **SR 2.9**; SL 3–4 = SR 2.9(1).

### SR 2.10 — Response to audit processing failures (p. 43)

**Requirement:** Alert personnel and prevent loss of essential services on audit processing failure.

**SL-C(UC) mapping:** SL 1–4 = **SR 2.10**.

**CCLI:** Monitor `logd`/journald; do not stop PF2 services if logging fails — alert only.

### SR 2.11 — Timestamps (pp. 43–44)

**Requirement:** Timestamps for audit record generation; sync with external sources (GPS/GLONASS/Galileo) where used.

**RE 1:** Internal clock sync at configurable frequency → **SL 3+** SR 2.11(1).

**RE 2:** Protect time source; audit event on alteration → SL 4 adds SR 2.11(2).

**SL-C(UC) mapping:** SL 1 = Not selected; **SL 2 = SR 2.11**; SL 3 = SR 2.11(1); SL 4 = SR 2.11(1)(2).

**CCLI:** GNSS/modem NTP (K3.5); protect NTP config in UCI.

### SR 2.12 — Non-repudiation (p. 44)

**Requirement:** Determine whether a given human user took a particular action.

**RE 1:** Non-repudiation for all users (human, process, device) → SL 3+ SR 2.12(1).

**SL-C(UC) mapping:** SL 1–2 = Not Selected; SL 3–4 = SR 2.12 / SR 2.12(1).

---

## FR 3 — System integrity (SI)

### §7.1 Purpose and SL-C(SI) descriptions (p. 45)

**Purpose:** Ensure IACS integrity to prevent unauthorized manipulation.

| SL | Protection |
|----|------------|
| **1** | Casual / coincidental manipulation |
| **2** | Intentional — **simple means, low resources, generic skills, low motivation** |
| **3** | Sophisticated — moderate resources, IACS-specific skills |
| **4** | Extended resources, high motivation |

**CCLI target:** **SL-C(SI) = 2** for Z0.

### FR 3 — SL-C(SI) 2 checklist — **COMPLETE**

| SR | SL-C 2 requires | Requirement summary | CCLI control | Status |
|----|-----------------|---------------------|--------------|--------|
| **3.1** | **SR 3.1** | Protect integrity of transmitted information | CRC/TLS on protocols; 62351 on C1/C2 | PART |
| **3.2** | **SR 3.2(1)** | Malicious code protection at **entry/exit points** | USB, Eth, LTE boundaries (K6.1) | PART |
| **3.3** | **SR 3.3** | Verify security functionality at FAT/SAT/maintenance | Secure Boot + firewall test checklist | PART |
| **3.4** | **SR 3.4** | Detect/protect unauthorized changes to **software at rest** | **Secure Boot + signed `.ipk`** (K6.1) | PART |
| **3.5** | **SR 3.5** | Input validation on control inputs / setpoints | API/UCI validation in `apps/ccli/` | PART |
| **3.6** | **SR 3.6** | Deterministic output on attack | Curtailment DO hold/fixed/unpowered policy | PART |
| **3.7** | **SR 3.7** | Error handling without adversary-exploitable info | Generic error responses | PART |
| **3.8** | **SR 3.8** | Session integrity; reject invalid session IDs | TLS/MMS session mgmt | PART |
| **3.9** | **SR 3.9** | Protect audit information from tamper/delete | Append-only / protected log store | PART |

### SR 3.1 — Communication integrity (pp. 45–46)

**Requirement:** Protect integrity of transmitted information.

**RE 1:** Cryptographic mechanisms to detect changes during communication → **SL 3+** SR 3.1(1).

**SL-C(SI) mapping:** SL 1–2 = **SR 3.1**; SL 3–4 = SR 3.1(1).

**Guidance:** Packet manipulation on switched/routed networks; sensor/actuator command tampering; physical access may suffice on small direct-link networks at lower SL.

**CCLI:** MMS/104 integrity via TLS (62351); Modbus CRC; no L2 bridge (K3.3).

### SR 3.2 — Malicious code protection (pp. 46–47)

**Requirement:** Prevent, detect, report, mitigate malicious/unauthorized software; capability to update protection mechanisms.

**RE 1:** Malicious code protection at **all entry and exit points** → **SL 2+** SR 3.2(1).

**RE 2:** Central management and reporting → **SL 3+** adds SR 3.2(2).

**SL-C(SI) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | SR 3.2 |
| **2** | **SR 3.2(1)** |
| 3 | SR 3.2(1)(2) |
| 4 | SR 3.2(1)(2) |

**CCLI:** Minimal OpenWrt image; USB mount policy; firewall on Eth/LTE; no email/browser on Z0.

### SR 3.3 — Security functionality verification (pp. 47–48)

**Requirement:** Verify intended operation of security functions at FAT, SAT, scheduled maintenance; report anomalies. Includes all security functions supporting this standard.

**RE 1:** Automated verification during FAT/SAT/maintenance → **SL 3+** SR 3.3(1).

**RE 2:** Verification during normal operation → **SL 4** SR 3.3(2).

**SL-C(SI) mapping:** SL 1–2 = **SR 3.3**; SL 3 = SR 3.3(1); SL 4 = SR 3.3(1)(2).

**Examples:** EICAR-style AV test; unauthorized login attempt; IDS rule trigger; confirm audit logging active.

**CCLI:** Lab checklist: Secure Boot verify, firewall deny test, default-account lockout test.

### SR 3.4 — Software and information integrity (pp. 48–49)

**Requirement:** Detect, record, report, protect against unauthorized changes to software and information **at rest**.

**RE 1:** Automated notification to configurable recipients on integrity discrepancy → **SL 3+** SR 3.4(1).

**SL-C(SI) mapping:** SL 1–2 = **SR 3.4**; SL 3–4 = SR 3.4(1).

**CCLI:** **Secure Boot + signed sysupgrade/`.ipk`** — primary SL-2 control (K6.1).

### SR 3.5 — Input validation (p. 49)

**Requirement:** Validate syntax and content of industrial process control inputs (and inputs directly impacting control system action).

**Guidance:** Pre-screen interpreter inputs; OWASP-style validation; does not address human error / out-of-range legitimate values.

**SL-C(SI) mapping:** SL 1–4 = **SR 3.5** at all levels.

**CCLI:** Validate setpoints, curtailment commands, UCI/API parameters in `apps/ccli/`.

### SR 3.6 — Deterministic output (p. 50)

**Requirement:** Set outputs to predetermined state if normal operation cannot be maintained due to attack.

**Options (asset-owner configurable):** **Unpowered** · **Hold** (last-known good) · **Fixed** (predetermined value).

**SL-C(SI) mapping:** SL 1–4 = **SR 3.6** at all levels.

**CCLI:** PF2 curtailment DO policy — document hold vs unpowered vs fixed for attack/degraded mode (§4.2 essential function).

### SR 3.7 — Error handling (pp. 50–51)

**Requirement:** Handle error conditions for effective remediation **without** information exploitable by adversaries unless necessary for timely troubleshooting.

**Guidance:** OWASP Code Review Guide; balance disclosure vs incident response needs.

**SL-C(SI) mapping:** SL 1 = Not Selected; **SL 2–4 = SR 3.7**.

**CCLI:** Generic API/SSH errors; detailed logs to protected audit store only (SR 2.8).

### SR 3.8 — Session integrity (p. 51)

**Requirement:** Protect session integrity; **reject invalid session IDs**.

**RE 1:** Invalidate session IDs on logout/termination → **SL 3+** SR 3.8(1).

**RE 2:** Unique session ID per session; treat unexpected IDs as invalid → SL 3+ adds SR 3.8(2).

**RE 3:** Randomness of session IDs from accepted sources → **SL 4** SR 3.8(3).

**SL-C(SI) mapping:** SL 1 = Not Selected; **SL 2 = SR 3.8**; SL 3 = SR 3.8(1)(2); SL 4 = SR 3.8(1)(2)(3).

**CCLI:** LuCI/SSH session tokens; MMS/TLS session binding.

### SR 3.9 — Protection of audit information (p. 52)

**Requirement:** Protect audit information and audit tools from unauthorized access, modification, deletion.

**RE 1:** Audit records on hardware-enforced write-once media → **SL 4** SR 3.9(1).

**SL-C(SI) mapping:** SL 1 = Not selected; **SL 2–3 = SR 3.9**; SL 4 = SR 3.9(1).

**CCLI:** journald append-only; restrict log file permissions; remote syslog with integrity.

---

## FR 4 — Data confidentiality (DC)

### §8.1 Purpose and SL-C(DC) descriptions (pp. 52–53)

**Purpose:** Ensure confidentiality of information on communication channels and in repositories.

| SL | Protection |
|----|------------|
| **1** | Casual / coincidental eavesdropping |
| **2** | Intentional search — **simple means, low resources, generic skills, low motivation** |
| **3** | Sophisticated — moderate resources, IACS-specific skills |
| **4** | Extended resources, high motivation |

**CCLI target:** **SL-C(DC) = 2** for Z0.

### FR 4 — SL-C(DC) 2 checklist — **COMPLETE**

| SR | SL-C 2 requires | Requirement summary | CCLI control | Status |
|----|-----------------|---------------------|--------------|--------|
| **4.1** | **SR 4.1(1)** | Confidentiality on **untrusted networks** + at rest where applicable | **62351 TLS** on LTE; disk encryption optional | PART |
| **4.2** | **SR 4.2** | Purge keys/credentials on decommission | Factory reset / key wipe SOP (K6.3) | PART |
| **4.3** | **SR 4.3** | Crypto algorithms/keys per industry practice | AES, SHA, NIST SP 800-57; OpenSSL policy | PART |

### SR 4.1 — Information confidentiality (pp. 53–54)

**Requirement:** Protect confidentiality of information with explicit read authorization, at rest or in transit.

**RE 1:** Protect confidentiality at rest and remote sessions on **untrusted networks** → **SL 2+** SR 4.1(1).

**RE 2:** Protect confidentiality **across zone boundaries** → **SL 3+** SR 4.1(2).

**SL-C(DC) mapping:** SL 1 = SR 4.1; **SL 2 = SR 4.1(1)**; SL 3–4 = SR 4.1(1)(2).

**Guidance:** Passwords never in clear (links SR 1.5); mobile device/USB risk; external comms may need compensating controls.

**CCLI:** **62351 TLS on C1/C2** (K6.5); VPN on LTE (Z5). At SL 2, zone-boundary TLS is SL 3 — document compensating firewall if MMS plaintext (V-01).

### SR 4.2 — Information persistence (p. 54)

**Requirement:** Purge all read-authorized information from components released from active service / decommissioned.

**RE 1:** Prevent unintended transfer via volatile shared memory → **SL 3+** SR 4.2(1).

**SL-C(DC) mapping:** SL 1 = Not Selected; **SL 2 = SR 4.2**; SL 3–4 = SR 4.2(1).

**CCLI:** Factory reset procedure; TPM key clear; overlay wipe (links 4-1 SG-4).

### SR 4.3 — Use of cryptography (pp. 54–55)

**Requirement:** If cryptography required — algorithms, key sizes, key establishment/management per industry practice.

**Guidance:** AES, SHA; NIST SP 800-57; ISO/IEC 19790; links **SR 1.8 PKI**.

**SL-C(DC) mapping:** SL 1–4 = **SR 4.3** at all levels.

**CCLI:** OpenSSL/TLS cipher suite policy; TPM-backed private keys (K6.2).

---

## FR 5 — Restricted data flow (RDF)

### §9.1 Purpose and SL-C(RDF) descriptions (pp. 55–56)

**Purpose:** Segment control system via zones and conduits to limit unnecessary data flow.

| SL | Protection |
|----|------------|
| **2** | Intentional circumvention — **simple means, low resources, generic skills, low motivation** |

**CCLI target:** **SL-C(RDF) = 2** for Z0 — **direct link to `CCI_62443_Zones.md`**.

### FR 5 — SL-C(RDF) 2 checklist — **COMPLETE**

| SR | SL-C 2 requires | Requirement summary | CCLI control | Status |
|----|-----------------|---------------------|--------------|--------|
| **5.1** | **SR 5.1(1)** | **Physical** network segmentation | Eth_A / Eth_B / plant / WAN separate (K3.3) | PART |
| **5.2** | **SR 5.2(1)** | **Deny-by-default** firewall at zone boundaries | OpenWrt `net_policy` / nftables | PART |
| **5.3** | **SR 5.3** | Block general P2P person-to-person comms | No email/social on Z0 | PART |
| **5.4** | **SR 5.4** | Application partitioning by criticality | Z0 app vs maintainer partition | PART |

### SR 5.1 — Network segmentation (pp. 56–57)

**Requirement:** Logically segment control system networks from non-control networks; segment critical from other control networks.

**RE 1:** **Physical** segmentation → **SL 2+** SR 5.1(1).

**RE 2:** Network services without connection to non-control networks → SL 3+ SR 5.1(2).

**RE 3:** Logical + physical isolation of critical networks → **SL 4** SR 5.1(3).

**SL-C(RDF) mapping:** SL 1 = SR 5.1; **SL 2 = SR 5.1(1)**; SL 3 = SR 5.1(1)(2); SL 4 = SR 5.1(1)(2)(3).

**CCLI:** TG-524 port isolation — **no L2 bridge** Eth_A ↔ plant (REQ-NET-LAB-001 / K3.3).

### SR 5.2 — Zone boundary protection (pp. 57–58)

**Requirement:** Monitor and control communications at zone boundaries per 62443-3-2 zones/conduits model.

**RE 1:** **Deny by default, allow by exception** → **SL 2+** SR 5.2(1).

**RE 2:** Island mode — prevent all boundary communication → SL 3+ SR 5.2(2).

**RE 3:** Fail close on boundary protection failure (must not interfere with SIS) → SL 3+ SR 5.2(3).

**SL-C(RDF) mapping:** SL 1 = SR 5.2; **SL 2 = SR 5.2(1)**; SL 3–4 = SR 5.2(1)(2)(3).

**CCLI:** OpenWrt default-deny firewall; island mode = drop all WAN/LTE except VPN.

### SR 5.3 — General purpose person-to-person communication restrictions (pp. 58–59)

**Requirement:** Prevent general-purpose P2P messages (email, social media, executable attachments) from external users/systems.

**RE 1:** Prohibit both transmission and receipt → **SL 3+** SR 5.3(1).

**SL-C(RDF) mapping:** SL 1–2 = **SR 5.3**; SL 3–4 = SR 5.3(1).

**CCLI:** No mail client, browser social apps on OpenWrt CCI image.

### SR 5.4 — Application partitioning (p. 59)

**Requirement:** Support partitioning of data, applications, services by criticality for zoning model.

**Guidance:** Physical or logical partition — separate CPUs, OS instances, network addresses.

**SL-C(RDF) mapping:** SL 1–4 = **SR 5.4** at all levels.

**CCLI:** Z0 (`apps/ccli/`) vs Z4 maintainer tools; procd service isolation.

---

## FR 6 — Timely response to events (TRE)

### §10.1 Purpose and SL-C(TRE) descriptions (p. 59)

**Purpose:** Respond to security violations — notify authority, report evidence, timely corrective action.

| SL | Protection |
|----|------------|
| **2** | Actively collect and **periodically report** forensic evidence |

**CCLI target:** **SL-C(TRE) = 2** for Z0.

### FR 6 — SL-C(TRE) 2 checklist — **COMPLETE**

| SR | SL-C 2 requires | Requirement summary | CCLI control | Status |
|----|-----------------|---------------------|--------------|--------|
| **6.1** | **SR 6.1** | Read-only audit log access (manual OK at SL 2) | `logread`, journald export | PART |
| **6.2** | **SR 6.2** | Continuous monitoring of security mechanisms | Firewall/service health alerts | PART |

### SR 6.1 — Audit log accessibility (pp. 60)

**Requirement:** Authorized humans/tools access audit logs on **read-only** basis.

**RE 1:** Programmatic access via API (SIEM) → **SL 3+** SR 6.1(1).

**SL-C(TRE) mapping:** SL 1–2 = **SR 6.1**; SL 3–4 = SR 6.1(1).

**CCLI:** `logread`, journalctl; restrict write to root; maintainer read-only role.

### SR 6.2 — Continuous monitoring (pp. 60–61)

**Requirement:** Continuously monitor all security mechanism performance; detect, characterize, report breaches timely.

**Guidance:** IDS, IPS, malicious code protection, network monitoring — must not adversely affect control system performance (links SR 3.3, SR 6.2).

**SL-C(TRE) mapping:** SL 1–4 = **SR 6.2** at all levels.

**CCLI (r32+):** procd `watchdog 90` + ubus ping every 30 s; crash tombstone → O.14 `security/crash_recovered`; fatal signal → best-effort safe DO. Still **PART** — firewall hit counters; SIEM/IDS at SL 3; HW WDT→DO (DRB) open.

---

## FR 7 — Resource availability (RA)

### §11.1 Purpose and SL-C(RA) descriptions (pp. 61–62)

**Purpose:** Maintain availability of control system against DoS, resource exhaustion, and failure.

| SL | Protection |
|----|------------|
| **2** | Intentional DoS — **simple means, low resources, generic skills, low motivation** |

**CCLI target:** **SL-C(RA) = 2** for Z0.

### FR 7 — SL-C(RA) 2 checklist — **COMPLETE**

| SR | SL-C 2 requires | Requirement summary | CCLI control | Status |
|----|-----------------|---------------------|--------------|--------|
| **7.1** | **SR 7.1(1)** | **RE 1** — manage communication loads / rate limiting | Per-interface rate limits on C3/C5 | PART |
| **7.2** | **SR 7.2** | Limit resource use by security functions | Cap scan/patch jobs; QoS on plant traffic | PART |
| **7.3** | **SR 7.3(1)** | **RE 1** — backup verification | Verified overlay/config backup before OTA | PART |
| **7.4** | **SR 7.4** | Recovery to known secure state | A/B partition + Secure Boot rollback (K2.7) | PART |
| **7.5** | **SR 7.5** | Emergency power without security state loss | PSU hold-up; degraded mode on brownout | PART |
| **7.6** | **SR 7.6** | View network/security settings per supplier guide | UCI + nft export; config drift checks | PART |
| **7.7** | **SR 7.7** | Prohibit unnecessary ports/protocols/services | Disable Wi‑Fi, unused daemons | PART |
| **7.8** | **SR 7.8** | Report installed components + properties | **SBOM / `.ipk` inventory** (K6.7) | PART |

### SR 7.1 — Denial of service protection (pp. 62–63)

**Requirement:** Protect against DoS; manage communication loads.

**RE 1:** Manage communication loads (rate limiting) → **SL 2+** SR 7.1(1).

**RE 2:** Limit DoS effects to other systems/networks → **SL 3+** SR 7.1(2).

**Guidance:** DoS on control/SIS network must not adversely impact safety-related systems (§4.2).

**SL-C(RA) mapping:** SL 1 = SR 7.1; **SL 2 = SR 7.1(1)**; SL 3–4 add SR 7.1(2).

**CCLI:** nftables rate limits on WAN/LTE; connection tracking limits on MMS/104 services.

### SR 7.2 — Resource management (pp. 63)

**Requirement:** Limit use of resources by security functions so essential functions are not starved.

**SL-C(RA) mapping:** SL 1–4 = **SR 7.2** at all levels.

**CCLI:** Schedule security scans off-peak; cap concurrent TLS handshakes.

### SR 7.3 — Control system backup (p. 63)

**Requirement:** Identify/locate critical files; backup user/system info + system state without affecting operations.

**RE 1:** Verify backup reliability → **SL 2+** SR 7.3(1).

**RE 2:** Automated backups at configurable frequency → **SL 3+** SR 7.3(2).

**SL-C(RA) mapping:** SL 1 = SR 7.3; **SL 2 = SR 7.3(1)**; SL 3–4 = SR 7.3(1)(2).

**CCLI:** `sysupgrade -b` before OTA; encrypted backup of UCI overlay (SR 4.3).

### SR 7.4 — Control system recovery and reconstitution (pp. 63–64)

**Requirement:** Recover and reconstitute to **known secure state** after disruption/failure.

**Guidance:** Reset security parameters; reinstall security-critical patches; restore security config.

**SL-C(RA) mapping:** SL 1–4 = **SR 7.4** at all levels.

**CCLI:** A/B sysupgrade + Secure Boot verified rollback (K2.7).

### SR 7.5 — Emergency power (p. 64)

**Requirement:** Switch to/from emergency power without affecting security state or documented degraded mode.

**Guidance:** Emergency power should cover compensating countermeasures (e.g. physical access control).

**SL-C(RA) mapping:** SL 1–4 = **SR 7.5** at all levels.

**CCLI:** Document brownout behaviour; security state preserved on brief power glitch.

### SR 7.6 — Network and security configuration settings (pp. 64–65)

**Requirement:** Configure per supplier hardening guide; interface to currently deployed settings.

**RE 1:** Machine-readable report of current security settings → **SL 3+** SR 7.6(1).

**SL-C(RA) mapping:** SL 1–2 = **SR 7.6**; SL 3–4 = SR 7.6(1).

**CCLI:** Export UCI + `nft list ruleset`; compare to golden config in lab (K8.6).

### SR 7.7 — Least functionality (p. 65)

**Requirement:** Prohibit/restrict unnecessary functions, ports, protocols, services.

**Guidance:** Disable COTS extras (email, VoIP, FTP, HTTP, file sharing) beyond baseline.

**SL-C(RA) mapping:** SL 1–4 = **SR 7.7** at all levels.

**CCLI:** Minimal OpenWrt image; disable Wi‑Fi, OPC, DLMS (K3.7).

### SR 7.8 — Control system component inventory (p. 66)

**Requirement:** Report current list of installed components and properties (ID, capability, revision).

**Guidance:** Align with SuC; formal configuration management (62443-2-1).

**SL-C(RA) mapping:** SL 1 = Not Selected; **SL 2–4 = SR 7.8**.

**CCLI:** **`opkg list-installed` / SBOM manifest** — primary K6.7 deliverable for CRA + 4-1 SM-9.

---

## Annex A — Security level vectors (pp. 67–74)

| Concept | Definition | CCLI usage |
|---------|------------|------------|
| **SL-T** | Target from risk assessment (3-2) | Z0/C1/C2 = 2 in `CCI_62443_Zones.md` |
| **SL-C** | Component/system **capability** | TG-524 + OpenWrt + `apps/ccli/` |
| **SL-A** | **Achieved** after design + operations | Cert verification matrix K8.6 |
| **SL vector** | Seven FR levels in fixed order | `{ IAC UC SI DC RDF TRE RA }` |
| **Notation** | Per-FR integer 0–4 | `{2222222}` for CCI Z0 |

**Lifecycle (Figures A.1–A.3):** SL-T (3-2 risk) → select SR/RE from 3-3 → design with SL-C components → verify → **SL-A**.

**SL 2 threat model (p. 73):** *simple means, low resources, generic skills, low motivation* — matches TG-524 lab assumptions.

**Vector format (p. 74):**

```
FORMAT → SL-?([FR,]domain)={IAC UC SI DC RDF TRE RA}
Example: SL-T(BPCS Zone)={2201313}
Example: SL-C(RA,FS-PLC)=4   ← single FR component of vector
```

**CCLI proposed vectors:**

| Domain | SL-T | Rationale |
|--------|------|-----------|
| **Z0 CCI core** | `{2222222}` | DSO-facing controller |
| **C1 DSO conduit** | `{2222222}` | 61850 MMS path |
| **C3 plant** | `{1111111}` | Modbus plant — lower exposure (zones doc) |

---

## Annex B — Table B.1 SL-C master map (pp. 75–78)

Annex B Table B.1 is the **cross-FR SL 1–4 checklist**. For **SL-C 2** at CCI Z0, notable bundles beyond base SR:

| FR | SL-2 extras (from Table B.1) |
|----|------------------------------|
| **1 IAC** | RE1 unique users (1.1); RE1 unique wireless (1.6); **SR 1.8 PKI**; SR 1.13 + verify §5.15.4 vs table for RE1 |
| **2 UC** | RE1+2 on 2.1; SR 2.6 remote terminate; SR 2.11 timestamps |
| **3 SI** | RE1 crypto comms (3.1) at SL 3; **SR 3.2(1)** malware at entry/exit; SR 3.7–3.9 |
| **4 DC** | **SR 4.1(1)** confidentiality on untrusted nets; SR 4.2 purge |
| **5 RDF** | **SR 5.1(1)** physical segmentation; **SR 5.2(1)** deny-by-default; SR 5.3 |
| **6 TRE** | SR 6.2 continuous monitoring |
| **7 RA** | **SR 7.1(1)** manage comm loads; **SR 7.3(1)** backup verify; **SR 7.8** component inventory |

Use Table B.1 for workshop CRS sign-off; normative SR *.4 sections win on any table discrepancy.

**Discrepancy note:** **SR 1.13 RE1** — §5.15.4 shows **SL 2 = SR 1.13(1)**; Table B p. 76 may show RE1 at SL 3–4 only — **normative §5.15.4 wins**.

---

## CCLI SL-T 2 → SL-C traceability (Z0) — `{2222222}`

### FR 1 — IAC

| SR | Design action | Owner | Gate |
|----|---------------|-------|------|
| 1.1 + RE1 | Per-user console/SSH; no shared `admin` | FW | Architecture freeze |
| 1.2 | Peer auth on MMS; document Modbus trust model | FW + protocols | K6.5 |
| 1.3 | Account lifecycle API; disable defaults | FW | K6.3 |
| 1.4 | Role/identifier schema in UCI | FW | D6 |
| 1.5 | Default password purge; TPM key wrap | FW | K6.2/K6.3 |
| 1.6(1) | Wi‑Fi off or LTE unique wireless auth | FW | K3.7 / K3.4 |
| 1.7 | Password strength policy | FW | K6.3 |
| 1.8 | PKI / cert provisioning for 62351 | FW + protocols | **K6.5** |
| 1.9 | TLS cert validation; TPM private keys | FW | K6.2/K6.5 |
| 1.10 | Masked login; generic auth errors | FW | D6 |
| 1.11 | Lockout policy; non-interactive service accounts | FW | §4.2 review |
| 1.12 | Pre-login banner (4 elements) | FW | Cert/legal |
| 1.13(1) | Untrusted network monitor + role approval | FW | K3.4 / Z5 |

### FR 2 — UC — **COMPLETE**

| SR | Design action | Owner | Gate |
|----|---------------|-------|------|
| 2.1 + RE1+2 | RBAC all users; editable role→permission map | FW | D6 |
| 2.2 | Wireless usage policy (LTE) | FW | K3.4 |
| 2.3 | USB/portable device restrictions | FW | Z4 policy |
| 2.4 | Block mobile code on Z0 | FW | Image hardening |
| 2.5 | Session lock (exclude emergency WS) | FW | §4.2 review |
| 2.6 | Remote session kill (LTE/maintainer) | FW | K3.4 |
| 2.8–2.11 | Audit events, storage, timestamps | FW | K3.5 GNSS/NTP |

### FR 3 — SI — **COMPLETE**

| SR | Design action | Owner | Gate |
|----|---------------|-------|------|
| 3.1 | Protocol integrity (TLS/CRC) | FW + protocols | K6.5 |
| 3.2(1) | Malware protection at entry/exit | FW | K6.1 |
| 3.3 | Security function verification checklist | QA | Lab bring-up |
| 3.4 | Secure Boot + signed images | FW | K6.1 |
| 3.5 | Input validation on control API | FW | D6 |
| 3.6 | Deterministic DO on attack | FW + HW | PF2 policy |
| 3.7 | Safe error messages | FW | D6 |
| 3.8 | Session integrity | FW | K6.5 |
| 3.9 | Protect audit logs | FW | SR 2.8 pair |

### FR 4 — DC — **COMPLETE**

| SR | Design action | Owner | Gate |
|----|---------------|-------|------|
| 4.1(1) | TLS on untrusted (LTE); 62351 C1/C2 | FW + protocols | **K6.5** |
| 4.2 | Decommission key/credential purge | FW | K6.3 |
| 4.3 | Crypto policy (AES/SHA/NIST) | FW | OpenSSL config |

### FR 5 — RDF — **COMPLETE**

| SR | Design action | Owner | Gate |
|----|---------------|-------|------|
| 5.1(1) | Physical port segmentation | FW + HW | K3.3 |
| 5.2(1) | Deny-by-default firewall | FW | `net_policy` |
| 5.3 | No P2P/email on Z0 | FW | Image hardening |
| 5.4 | App partition Z0 vs Z4 | FW | Architecture |

### FR 6 — TRE — **COMPLETE**

| SR | Design action | Owner | Gate |
|----|---------------|-------|------|
| 6.1 | Read-only audit access | FW | Ops runbook |
| 6.2 | Continuous security monitoring | FW | Alerts / SIEM |

### FR 7 — RA — **COMPLETE**

| SR | Design action | Owner | Gate |
|----|---------------|-------|------|
| 7.1(1) | Rate limits on WAN/LTE/MMS | FW | K3.3 |
| 7.3(1) | Verified config backup before OTA | FW | K2.7 |
| 7.4 | Secure rollback to known state | FW | K2.7 |
| 7.7 | Minimal attack surface (no Wi‑Fi/extras) | FW | K3.7 |
| 7.8 | SBOM / `.ipk` inventory | FW | **K6.7** |

### Design must-haves (cross-FR at SL-C 2)

| Requirement | SR | Artefact |
|-------------|-----|----------|
| Signed firmware / integrity at rest | 3.4 | Secure Boot + `.ipk` signing (K6.1) |
| Malware at entry points | 3.2(1) | USB policy, firewall, minimal services |
| TLS on untrusted paths | 4.1(1) | IEC 62351 on C1/C2 (K6.5) |
| Physical/logical segmentation | 5.1(1) | No L2 bridge Eth_A↔plant (K3.3) |
| Deny-by-default firewall | 5.2(1) | OpenWrt `net_policy` |
| Component inventory / SBOM | 7.8 | K6.7 `.ipk` manifest |
| PKI for TLS | 1.8 | **MISS** — K6.5 blocker |

**Compensating controls already identified** (`ccli-62443-zones-extract`):

| Gap | Compensating control | Document |
|-----|---------------------|----------|
| MMS without TLS (V-01) | Zone firewall + eventual 62351-4 | CRS + K6.5 |
| USB console (Z4) | Physical access + separate zone | ZCR 3.4 |
| LTE untrusted (SR 1.13) | VPN + no Eth_A bridge | K3.4 |

---

## Interface matrix (FR → CCLI artefact)

| FR | Primary interfaces | Protocol / control |
|----|-------------------|-------------------|
| FR 1 | USB, Eth_B, LTE, MMS | SSH/console, 104, VPN, 61850+TLS |
| FR 2 | All services | RBAC in `apps/ccli/` |
| FR 3 | Boot, update | Secure Boot, signed `.ipk` |
| FR 4 | C1, C2 | IEC 62351 |
| FR 5 | Eth_A/B/plant/WAN | `net_policy`, OpenWrt firewall |
| FR 6 | Z0 | Audit log, alerts |
| FR 7 | Z0 services | Rate limits, minimal attack surface |

---

## BOM / standards matrix

| Block | Function | Norm | Status |
|-------|----------|------|--------|
| SL-T | Zone targets | 62443-3-2 | PART — `ccli-62443-zones-extract` |
| SR / SL-C | System requirements | **62443-3-3** | **HAVE** — FR 1–7 + Annex A/B (pp. 24–78) |
| Component proof | Device test | 62443-4-2 | MISSING |
| SDLC | Process | 62443-4-1 | **HAVE** — `ccli-62443-4-1-extract` |
| Terminology | FR definitions | 62443-1-1 | MISSING |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| §4 Constraints | **HAVE** — pp. 22–24 | — | — |
| FR 1 (IAC) | **HAVE** — SR 1.1–1.13 | — | — |
| FR 2 (UC) | **HAVE** — SR 2.1–2.12 | — | — |
| FR 3 (SI) | **HAVE** — SR 3.1–3.9 | — | — |
| FR 4 (DC) | **HAVE** — SR 4.1–4.3 | — | — |
| FR 5 (RDF) | **HAVE** — SR 5.1–5.4 | — | — |
| FR 6 (TRE) | **HAVE** — SR 6.1–6.2 | — | — |
| FR 7 (RA) | **HAVE** — SR 7.1–7.8 | — | — |
| Annex A/B | **HAVE** — pp. 67–78 | — | — |
| SR 1.8 PKI | **MISS** | DSO CA / enterprise decision | K6.5 |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-633-001 | IT security control breaks essential function | HSE / curtailment loss | §0.1 IACS-first design review |
| R-633-002 | Undocumented compensating countermeasure | Wrong SL-A claim | CRS traceability per §0.2 |
| R-633-003 | 3-3 ingested without 4-2 | Lab scope mismatch | Quote 4-2 early; align SR IDs |
| R-633-004 | SR 1.13 RE1 table vs §5.15.4 mismatch | Wrong SL-C 2 claim | Normative text wins; workshop note |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| SL-T → SR trace | Design review | Every Z0 FR has assigned SR row or compensating doc |
| Essential function | FMEA / degraded mode | Security action does not remove curtailment without alternative |
| FR 1 SL-C 2 | All SR 1.1–1.13 rows in extract | Workshop sign-off | Internal review |
| FR 2 SL-C 2 | All SR 2.1–2.12 rows in extract | Workshop sign-off | Internal review |
| FR 3 SL-C 2 | All SR 3.1–3.9 rows in extract | Workshop sign-off | Internal review |
| FR 4–5 SL-C 2 | All SR 4.1–4.3, 5.1–5.4 rows | Workshop sign-off | Internal review |
| FR 6–7 SL-C 2 | All SR 6.1–6.2, 7.1–7.8 rows | Workshop sign-off | Internal review |
| SL vector | Workshop | `{2222222}` signed off for Z0; C3 `{1111111}` | Zones doc |
| Annex A/B | Corpus check | Vector methodology + Table B.1 ingested | Internal review |
| 62443-4-2 | Lab quote | Component SL-2 checklist aligned | P0 cert |

---

## Source pages captured

| Page | Content | Quality |
|------|---------|---------|
| **11** | §0.1 Overview | PASS |
| **12** | §0.2–0.3 Purpose, 3-2 link | PASS |
| **13** | Figure 1 — 62443 series | PASS |
| **14** | §1 Scope (7 FRs); §2–3 start | PASS |
| **15** | §3.1.1–3.1.7 | PASS |
| **17** | §3.1.17–3.1.24 | PASS |
| **20** | §3.1.45–47 trust, zone; §3.2 acronyms start | PASS |
| **21** | §3.2 acronyms (FR, SR, RE, SL-T/A/C, PKI, TPM, …) | PASS |
| **22** | §3.3 Conventions; §4.1 Overview | PASS |
| **23** | §4.2 Essential functions; §4.3 Compensating CM | PASS |
| **24** | §4.4 Least privilege; §5.1–5.3 SR 1.1 start | PASS |
| **25** | SR 1.1 RE 1–3; **§5.3.4 SL-C(IAC) table** | PASS |
| **26** | SR 1.2 requirement + RE 1 | PASS |
| **27** | SR 1.2 SL table; SR 1.3 + SL table | PASS |
| **28** | SR 1.4; SR 1.5 requirement | PASS |
| **29** | SR 1.5 RE 1 (TPM); **§5.7.4 SL table** | PASS |
| **30** | SR 1.6 wireless + **SL 1.6(1) at SL 2**; SR 1.7 start | PASS |
| **31** | SR 1.7 RE/lifetime; **§5.9.4 SL**; SR 1.8 PKI | PASS |
| **32** | SR 1.8 SL; SR 1.9 public key auth | PASS |
| **33** | SR 1.9 RE 1 TPM; SR 1.10 authenticator feedback | PASS |
| **34** | SR 1.11 lockout; SR 1.12 notification start | PASS |
| **35** | SR 1.12 banner elements; **SR 1.13(1) at SL 2** | PASS |
| **36** | **FR 2** purpose; SR 2.1 authorization enforcement | PASS |
| **37** | SR 2.1 RE 1–4; **SL-C(UC) 2 = 2.1+RE1+RE2**; SR 2.2 start | PASS |
| **38** | SR 2.2 wireless UC; SR 2.3 portable devices (partial) | PASS |
| **39** | SR 2.3 SL + RE1; SR 2.4 mobile code | PASS |
| **40** | SR 2.5 session lock; SR 2.6 remote termination start | PASS |
| **41** | SR 2.6 SL; SR 2.7 concurrent sessions; SR 2.8 auditable events | PASS |
| **42** | SR 2.8 RE1/SL; SR 2.9 audit storage capacity | PASS |
| **43** | SR 2.9 SL; SR 2.10 audit failures; SR 2.11 timestamps start | PASS |
| **44** | SR 2.11 RE/SL; SR 2.12 non-repudiation | PASS |
| **45** | **FR 3** purpose; SR 3.1 communication integrity start | PASS |
| **46** | SR 3.1 RE1/SL; SR 3.2 malicious code protection | PASS |
| **47** | SR 3.2 RE1/2/SL; SR 3.3 security verification start | PASS |
| **48** | SR 3.3 RE/SL; SR 3.4 software integrity at rest | PASS |
| **49** | SR 3.4 RE/SL; SR 3.5 input validation | PASS |
| **50** | SR 3.6 deterministic output; SR 3.7 error handling | PASS |
| **51** | SR 3.7 SL; SR 3.8 session integrity | PASS |
| **52** | SR 3.9 audit protection; **FR 4** purpose | PASS |
| **53** | SR 4.1 information confidentiality | PASS |
| **54** | SR 4.1 SL; SR 4.2 persistence; SR 4.3 crypto start | PASS |
| **55** | SR 4.3 SL; **FR 5** purpose | PASS |
| **56** | SR 5.1 network segmentation | PASS |
| **57** | SR 5.1 SL; SR 5.2 zone boundary protection | PASS |
| **58** | SR 5.2 SL; SR 5.3 P2P comm restrictions | PASS |
| **59** | SR 5.3 SL; SR 5.4 app partitioning; **FR 6** purpose | PASS |
| **60** | SR 6.1 audit access; SR 6.2 continuous monitoring | PASS |
| **61** | SR 6.2 SL; **FR 7** purpose + SL descriptions | PASS |
| **62** | SR 7.1 DoS protection; SR 7.2 resource management | PASS |
| **63** | SR 7.2 SL; SR 7.3 backup; SR 7.4 recovery start | PASS |
| **64** | SR 7.4–7.5 emergency power; SR 7.6 config start | PASS |
| **65** | SR 7.6 complete; SR 7.7 least functionality | PASS |
| **66** | **SR 7.8** component inventory — FR 7 complete | PASS |
| **67–74** | **Annex A** — SL-T/A/C lifecycle; vector format; SL 0–4 | PASS |
| **75–78** | **Annex B Table B.1** — master SL 1–4 map (all FR) | PASS |

### Optional remaining capture

```
16         ← §3.1 terms continuation
```

---

## Related RAG sources

| source_id | Role |
|-----------|------|
| `ccli-62443-3-3-extract` | This document |
| `ccli-62443-3-3-capture-plan` | ToC + batch order |
| `ccli-62443-zones-extract` | SL-T, zones Z0–Z5 |
| `iec-62443-4-2` | Component cert target |
| `ccli-tg500-lab-platform` | Port / zone map |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-08-11 | Batch A ingest pp. 11–15, 17 — scope, series, key terms |
| 1.1 | 2026-08-11 | pp. 20–29: §3.3, §4, FR 1 SR 1.1–1.5 + SL-C 2 checklist |
| 1.2 | 2026-08-11 | pp. 30–33: SR 1.6–1.10 (wireless, PKI, password, auth feedback) |
| 1.3 | 2026-08-11 | pp. 34–38: FR 1 complete (SR 1.11–1.13); FR 2 SR 2.1–2.3 partial |
| 1.4 | 2026-08-11 | **Patch 1** pp. 39–50: FR 2 complete (SR 2.3–2.12); FR 3 SR 3.1–3.7 partial |
| 1.5 | 2026-08-11 | **Patch 2** pp. 51–60: FR 3–5 complete; FR 6 SR 6.1–6.2 partial |
| **2.0** | 2026-08-11 | **Patch 3** pp. 61–78: FR 6–7 complete; Annex A/B; `{2222222}` vector |

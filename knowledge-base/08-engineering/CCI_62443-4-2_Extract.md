# IEC 62443-4-2 — CCLI Engineering Extract

**Document ID:** CCLI-SEC-62443-007  
**Revision:** 1.8  
**Date:** 2026-08-11  
**RAG source_id:** `ccli-62443-4-2-extract`  
**Normative basis:** IEC 62443-4-2:2018 — *Technical security requirements for IACS components*  
**Capture status:** **HAVE** — normative body + **Annex B Table B.1** (pp. 87–92); gaps: p. **57** CR 6.1, p. **63** SAR, §14 HDR pp. 69–74, Annex A pp. 84–86  
**Parents:** `ccli-62443-3-3-extract` · `ccli-62443-4-1-extract` · `ccli-62443-zones-extract` · `ccli-62443-4-2-capture-plan`  
**Programme link:** K6.4 · K6.5 · K6.1 · KR-012

---

## Summary

**62443-4-2** specifies **Component Requirements (CR)** and **Requirement Enhancements (RE)** for four IACS component types: **software application**, **host device**, **embedded device**, **network device**. Requirements are **derived from 62443-3-3** SRs — CR numbering matches SR IDs for traceability.

For **HiTEKS CCLI / TG-524:** lab certification targets **SL-C 2** on applicable CRs. Product classification: **embedded device (EDR)** + **software application (SAR)**; **network device (NDR)** clauses apply to OpenWrt firewall/routing; **host device (HDR)** partially relevant (USB console).

**Proposed component SL vector** (align with system `{2222222}`):

```
SL-C(TG-524 CCI) = { IAC  UC  SI  DC  RDF  TRE  RA } = { 2  2  2  2  2  2  2 }
```

**Process pairing:** **62443-4-1** SDL (HAVE) + **62443-4-2** technical proof (this doc). **FR 1 IAC + FR 2 UC complete** — only design gap at SL-C 2: **CR 1.8 PKI** (K6.5 / 62351).

---

## Requirements (from §1 Scope)

62443-4-2 provides detailed **CRs** for seven FRs (from **62443-1-1** / **62443-3-3**):

| FR | Foundational requirement | TG-524 clause focus |
|----|-------------------------|---------------------|
| **FR 1** | Identification and authentication control (IAC) | §5 CR 1.1–1.14 |
| **FR 2** | Use control (UC) | §6 CR 2.1–2.13 |
| **FR 3** | System integrity (SI) | §7 CR 3.1–3.14 + EDR 3.x |
| **FR 4** | Data confidentiality (DC) | §8 CR 4.1–4.3 |
| **FR 5** | Restricted data flow (RDF) | §9 CR 5.1–5.4 + NDR 5.x |
| **FR 6** | Timely response to events (TRE) | §10 CR 6.1–6.2 |
| **FR 7** | Resource availability (RA) | §11 CR 7.1–7.8 |

**Scope boundary (§1):** Defines **SL-C(component)** capability levels 0–4. **SL-T** and **SL-A** are out of scope (see **62443-3-2**). Non-technical program requirements → **62443-2-1**.

**Normative references (§2):**

| Reference | Role |
|-----------|------|
| **IEC TS 62443-1-1** | Terminology, FR definitions |
| **IEC 62443-3-3:2013** | Parent SR/RE set |
| **IEC 62443-4-1** | Secure product development lifecycle |

---

## Architecture — 62443 series placement (Figure 1, §0.1)

```
General        62443-1-1 · 1-2 Glossary · 1-3 Metrics · 1-4 Lifecycle
Policies       62443-2-1 · 2-2 · 2-3 Patch · 2-4 Service providers
System         62443-3-1 · 3-2 Zones/SL-T · 3-3 SR/SL-C
Component      62443-4-1 Product SDL · 62443-4-2 CR/SL-C  ← this doc · TG-524 cert
```

**Figure 1 status (2018 edition):** 3-3 **Published**; 4-1 **Published**; 4-2 **Approved**; 3-2 **Approved**.

**Component types (§0.1, §3.3):**

| Type | Abbrev | TG-524 mapping |
|------|--------|----------------|
| Embedded device | **EDR** | OpenWrt firmware on MT798X SoM |
| Software application | **SAR** | `apps/ccli/` CCI stack |
| Network device | **NDR** | OpenWrt `net_policy` / firewall / routing |
| Host device | **HDR** | Partial — USB commissioning console |

**Derivation rule (§3.3):** CRs expand 3-3 SRs; numbering matches SR IDs (gaps where SR not applicable to components). Type-specific requirements begin at **§12**.

**Annex A** — device category examples (PLC, IED, switch, VPN, HMI, historian) — pp. 84–86 *not captured*.  
**Annex B Table B.1** — master CR/RE → SL 1–4 map — **HAVE** (pp. 87–92).

---

## Key concepts (§0.1 Overview)

| Concept | Definition / rule | CCLI implication |
|---------|-------------------|------------------|
| **IACS vs IT** | IACS prioritises availability, plant protection, degraded mode, time-critical response | Same as 3-3 — do not break PF2 for IT-style lockdown |
| **Essential function** | HSE + equipment-under-control availability | Identify PF2 observability + curtailment as essential (2-1 RA) |
| **Component capability** | SL-C = mitigations **without compensating countermeasures** | Lab tests bare product; zone firewall is compensating at integration |
| **Compensating countermeasure** | External control when inherent capability insufficient | Document in integration guide (CCSC 2) |
| **Four component classes** | App · host · embedded · network | Multi-role product must meet **all applicable** type clauses |
| **3-3 derivation** | Same FR/SR/CR IDs; 4-2 adds component + device-type reqs | Trace 3-3 extract rows directly to 4-2 CR rows |

**Audience:** product suppliers (HiTEKS), integrators, asset owners, cert labs.

---

## Terms ingested (§3.1 — selected, pp. 19–23)

| Term | Definition | CCLI usage |
|------|------------|------------|
| **Authenticity** | Entity is what it claims — auth of origin + integrity | MMS peer identity, signed firmware |
| **Availability** | Timely reliable access to control system info/functions | DoS protection on plant ports |
| **Compensating countermeasure** | In lieu of / in addition to inherent capabilities | External firewall if PLC lacks UC |
| **Component** | Entity with host/network/app/**embedded** characteristics | TG-524 = multi-type |
| **Conduit** | Logical grouping of channels between zones | Align with zones doc |
| **Confidentiality** | Info not disclosed to unauthorised entities | 62351 TLS |
| **Countermeasure** | Action/device/procedure reducing threat/vulnerability | Secure Boot, TPM, firewall |
| **Degraded mode** | Continue essential functions despite faults | Brownout / LTE loss policy |
| **Device** | Discrete physical asset with capabilities | TG-524, analyzers, RTUs |
| **Embedded device** | Special-purpose process monitor/control; limited OS/firmware | **Primary TG-524 class** — OpenWrt on SoM |
| **Essential function** | HSE + availability for equipment under control | PF2 FSM paths |
| **Firecall** | Emergency access method | Maintainer lockout procedure |
| **Host device** | General-purpose OS hosting apps (Windows/Linux) | Pi dev mock; USB maintainer WS |
| **Mobile code** | Program transferred and executed without explicit install | Block on Z0 (CR 2.4 / SAR 2.4) |
| **Mobile device** | Portable intelligent device | Handheld programmer policy |
| **Network device** | Facilitates or restricts data flow | **TG-524 routing/firewall** |
| **Product supplier** | HW/SW manufacturer | **HiTEKS** |
| **Remote access** | User from outside zone perimeter | LTE / VPN (Z5) |
| **Role** | Privileges/obligations assigned to user/process/device | RBAC in `apps/ccli/` |
| **Security level** | Countermeasures + inherent properties for zone/conduit | SL-C 2 target |
| **Software application** | Programs interfacing with process or control system | **CCI stack** |
| **Threat** | Circumstances/events with potential adverse effect | Zone threat table |
| **Trust / untrusted** | Relied upon / not meeting trust requirements | WAN/LTE = untrusted |
| **Update / upgrade** | Security/fix change vs feature add | Signed OTA (CR 3.10) |
| **Zone** | Partition of SuC by functional/logical/physical relationship | Z0–Z5 |

*Note:* §3.1 starts p. **18** — partial gap; terms 3.1.1–3.1.5 not in screenshot batch.

### §3.2 Acronyms (selected, pp. 24–25)

| Acronym | Meaning | CCLI |
|---------|---------|------|
| **CR** | Component requirement | Lab checklist unit |
| **RE** | Requirement enhancement | Higher SL bundles |
| **FR** | Foundational requirement | FR 1–7 |
| **SR** | System requirement (3-3) | Traceability parent |
| **EDR / HDR / NDR / SAR** | Embedded / host / network / software app reqs | Device-type clauses §12–15 |
| **CCSC** | Common component security constraint | §4 |
| **SL / SL-C / SL-T / SL-A** | Security level / capability / target / achieved | SL-C 2 lab target |
| **IAC / UC / SI / DC / RDF / TRE / RA** | Seven FRs | Vector `{2222222}` |
| **PKI / CA / OCSP / TPM** | Crypto infrastructure | K6.2 · K6.5 |
| **TPM** | Trusted platform module | TG-424 Pro |

### §3.3 Conventions

- CRs/REs derived from **62443-3-3**; CR number = SR number (non-sequential gaps normal).
- Four component types; most CRs apply to all; **§12–15** add type-specific reqs.
- Notation: **SL-C(FR, component)** values 0–4; SL 0 = no requirements.
- **SL vector:** see 3-3 Annex A — `{2222222}` proposed for TG-524.
- Example (**FR 4 DC purpose**, p. 26): SL 1 casual exposure → SL 4 extended resources/high motivation.

---

## §4 Common component security constraints (CCSC)

| CCSC | Title | Requirement | CCLI hook |
|------|-------|-------------|-----------|
| **4.2 CCSC 1** | Support of essential functions | Adhere to **62443-3-3:2013 Clause 4** | PF2 must survive auth/audit/DoS controls |
| **4.3 CCSC 2** | Compensating countermeasures | Document external countermeasures when CR unmet alone | Integration guide: zone firewall, physical USB access |
| **4.4 CCSC 3** | Least privilege | Enforce least privilege; granularity per device type | RBAC + minimal OpenWrt services |
| **4.5 CCSC 4** | Software development process | Develop per **62443-4-1** SDL | Pair with `ccli-62443-4-1-extract` |

**CCLI:** CCSC 1 mirrors 3-3 §4 — curtailment and observability are essential; PKI/lockout must not block emergency paths without degraded mode.

---

## FR 1 — Identification and authentication control (IAC) — **COMPLETE**

### §5.1 Purpose and SL-C(IAC) descriptions (p. 27)

Identify and authenticate all users (humans, software processes, devices) before access.

| SL | Protection against |
|----|-------------------|
| **SL 1** | Casual / coincidental unauthenticated access |
| **SL 2** | Intentional access — **simple means**, low resources, generic skills, low motivation |
| **SL 3** | Sophisticated means, moderate resources, IACS-specific skills, moderate motivation |
| **SL 4** | Sophisticated means, extended resources, IACS-specific skills, high motivation |

**CCLI target:** **SL-C(IAC, component) = 2**.

### §5.2 Rationale (p. 28)

ID + auth implements access control with authorization (CR 2.1). Minimise multiple auth mechanisms per zone. Extend access control to **data at rest**.

### FR 1 — SL-C(IAC, component) 2 checklist — **COMPLETE**

| CR | SL-C 2 requires | Maps to 3-3 | CCLI control | Status |
|----|-----------------|-------------|--------------|--------|
| **1.1** | **CR 1.1(1)** | SR 1.1 + RE 1 | Unique console/SSH users | PART |
| **1.2** | **CR 1.2** | SR 1.2 | MMS/Modbus peer auth | PART |
| **1.3** | **CR 1.3** | SR 1.3 | Account lifecycle (UCI) | PART |
| **1.4** | **CR 1.4** | SR 1.4 | Role/identifier schema | PART |
| **1.5** | **CR 1.5** | SR 1.5 | Default password purge | PART |
| **1.6** | **NDR 1.6(1)** | SR 1.6(1) | LTE wireless auth (unique ID) | PART |
| **1.7** | **CR 1.7** | SR 1.7 | Password strength policy | PART |
| **1.8** | **CR 1.8** | SR 1.8 | PKI / 62351 certs | **MISS** |
| **1.9** | **CR 1.9** | SR 1.9 | TLS cert validation + TPM keys | PART |
| **1.10** | **CR 1.10** | SR 1.10 | Masked login; generic errors | PART |
| **1.11** | **CR 1.11** | SR 1.11 | Lockout; non-interactive services | PART |
| **1.12** | **CR 1.12** | SR 1.12 | Pre-login banner (4 elements) | PART |
| **1.13** | **NDR 1.13** | SR 1.13(1) | Untrusted-network access control (LTE) | PART |
| **1.14** | **CR 1.14** | *(4-2 only)* | Symmetric key auth (if used) | PART |

### CR 1.1 — Human user identification and authentication (pp. 28–29)

**Requirement:** Identify and authenticate all human users per **62443-3-3 SR 1.1** on all human-access interfaces; enforce on every such interface; support segregation of duties + least privilege; may be local or system-integrated.

**RE 1:** Unique ID + auth for every human user → **SL 2+** CR 1.1(1).  
**RE 2:** Multifactor authentication for **all** human access → **SL 3+** CR 1.1(2).

**SL-C(IAC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | CR 1.1 |
| **2** | **CR 1.1(1)** |
| 3 | CR 1.1(1)(2) |
| 4 | CR 1.1(1)(2) |

**Guidance:** Passwords, tokens, biometrics, physically keyed lids, MFA; local + remote; HTTP/HTTPS/FTP/SFTP/config tools; role/group auth OK; **must not hamper fast local emergency actions**; verify identity then enforce permissions (CR 2.1).

**CCLI:** Eth_B operator login; USB maintainer (Z4) — unique accounts; MFA deferred to SL 3.

### CR 1.2 — Software process and device identification and authentication (pp. 29–30)

**Requirement:** Component identifies/authenticates to any other component per **62443-3-3 SR 1.2**; human user auth (SR 1.1) may be part of process context.

**RE 1:** Unique ID + auth for component to any other component → **SL 3+** CR 1.2(1).

**SL-C(IAC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **CR 1.2** |
| 3 | CR 1.2(1) |
| 4 | CR 1.2(1) |

**Guidance:** Prevent rogue entities; mobile/portable devices high risk; group/role auth OK; essential functions not hampered; multicast may use group auth; compensating CM if unique ID infeasible.

**CCLI:** MMS client/server identity (62351); Modbus trust model documented.

### CR 1.3 — Account management (p. 30)

**Requirement:** Support account management directly or integrate into system per **62443-3-3 SR 1.3**.

**RE:** None.

**SL-C(IAC, component):** CR 1.3 at **all SL 1–4**.

**Guidance:** Native or LDAP/AD delegation; consider impact if directory unavailable.

**CCLI:** UCI/local account API; separate human vs service accounts (K6.3).

### CR 1.4 — Identifier management (pp. 30–31)

**Requirement:** Integrate into identifier management system and/or manage identifiers directly per **62443-3-3 SR 1.4**.

**RE:** None.

**SL-C(IAC, component):** CR 1.4 at **all SL 1–4** *(SL table p. 31 — not in screenshot batch; normative per 3-3)*.

**Guidance:** Identifiers unique and unambiguous — account names, UNIX UIDs, Windows GUIDs, bound X.509 certs.

**CCLI:** Roles: `maintainer`, `operator`, `service-ccli`, `service-61850`.

### CR 1.5 — Authenticator management (pp. 31–32)

**Requirement:** Per **62443-3-3 SR 1.5** — initialize, change defaults, refresh, protect authenticators (storage + transit).

**RE 1:** Authenticators protected via **hardware mechanisms** → **SL 3+** CR 1.5(1).  
*Example:* password-protected memory, OTP, HW integrity checks, secure boot.

**SL-C(IAC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | CR 1.5 |
| **2** | **CR 1.5** |
| 3 | CR 1.5(1) |
| 4 | CR 1.5(1) |

**Guidance:** HSM/TPM for crypto keys; policy for defaults, lifetime, recall; links CR 1.7, 1.8, 1.9.

**CCLI:** First-boot default purge; **TPM 2.0** (K6.2) — headroom for SL 3 RE 1.

### CR 1.6 — Wireless access management (p. 32)

→ **§15 NDR 1.6** — **HAVE** (pp. 75). **CCLI:** Wi‑Fi disabled (K3.7); LTE unique auth via **NDR 1.6(1)**.

### CR 1.7 — Strength of password-based authentication (pp. 32–33)

**Requirement:** Enforce **configurable password strength** per international guidelines where passwords used.

**RE 1:** Prevent password reuse + min/max lifetime for **human** users → **SL 3+** CR 1.7(1).  
**RE 2:** Password lifetime for **all** users → **SL 4** CR 1.7(2).

**SL-C(IAC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | CR 1.7 |
| **2** | **CR 1.7** |
| 3 | CR 1.7(1) |
| 4 | CR 1.7(1)(2) |

**CCLI:** Console/OpenWrt password policy (K6.3).

### CR 1.8 — Public key infrastructure certificates (p. 33)

**Requirement:** When PKI used — provide or integrate capability per **62443-3-3 SR 1.8**.

**RE:** None.

**SL-C(IAC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **CR 1.8** |
| 3 | CR 1.8 |
| 4 | CR 1.8 |

**Guidance:** Org certificate policy; RFC 3647; CA location per 62443-2-1.

**CCLI:** **K6.5 blocker** — IEC 62351 MMS/TLS on C1/C2; DSO/operator CA decision.

### CR 1.9 — Strength of public key-based authentication (pp. 34–35)

**Requirement:** When public-key auth used, capability to:
- a) validate cert signatures;
- b) validate chain or deploy self-signed leaf securely;
- c) check revocation (CRL/OCSP);
- d) establish user control of private key;
- e) map authenticated identity to user/process/device;
- f) algorithms/keys conform to **CR 8.5**.

**RE 1:** Protect long-lived private keys via **hardware** → **SL 3+** CR 1.9(1).

**SL-C(IAC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **CR 1.9** |
| 3 | CR 1.9(1) |
| 4 | CR 1.9(1) |

**Guidance:** Out-of-band CA OK; trust chain essential; self-signed with secure distribution; short lifetime may compensate missing OCSP.

**CCLI:** OpenSSL chain verify for MMS; TPM private keys.

### CR 1.10 — Authenticator feedback (p. 35)

**Requirement:** Obscure auth feedback during authentication; no failure hints.

**SL-C(IAC, component):** CR 1.10 at **all SL 1–4**.

**CCLI:** Masked password; generic "authentication failed".

### CR 1.11 — Unsuccessful login attempts (pp. 35–36)

**Requirement:** Configurable limit on consecutive invalid attempts; deny for period or until admin unlock.

**SL-C(IAC, component):** CR 1.11 at **all SL 1–4**.

**Guidance:** DoS risk from lockout; auto-reset after policy interval; **no auto lockout when immediate operator response required**; critical service accounts — disallow interactive logon.

**CCLI:** PAM/fail2ban on console — exclude emergency paths; service accounts non-interactive.

### CR 1.12 — System use notification (pp. 36–37)

**Requirement:** Configurable pre-auth notification on local human-access/HMI. Minimum four elements:
- a) accessing asset-owner system;
- b) usage may be monitored/recorded/audited;
- c) unauthorised use prohibited with penalties;
- d) use indicates consent to monitoring.

**SL-C(IAC, component):** CR 1.12 at **all SL 1–4**.

**CCLI:** SSH/console banner before login (legal/audit text).

### CR 1.13 — Access via untrusted networks (p. 37)

→ **§15 NDR 1.13** — **HAVE** (pp. 75–76). **CCLI:** LTE/VPN monitor/control; explicit approval at SL 3+ (K3.3).

### CR 1.14 — Strength of symmetric key-based authentication (pp. 37–38) — **4-2 only**

**Requirement:** For symmetric keys — capability to:
- a) establish mutual trust;
- b) store shared secret securely;
- c) restrict access to shared secret;
- d) algorithms/keys conform to **CR 8.5**.

**RE 1:** Protect long-lived symmetric keys via **hardware** → **SL 3+** CR 1.14(1).

**SL-C(IAC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **CR 1.14** |
| 3 | CR 1.14(1) |
| 4 | CR 1.14(1) |

**Guidance:** Out-of-band key install; Needham-Schröder/Kerberos; CMAC/GCM/GMAC modes; compromise = full system risk.

**CCLI:** Document if symmetric keys used (e.g. pre-shared VPN); prefer PKI/TLS path for 62351.

---

## FR 2 — Use control (UC) — **COMPLETE** (shared CRs)

### §6.1 Purpose and SL-C(UC) descriptions (p. 38)

Enforce privileges of authenticated users; monitor use.

| SL | Protection against |
|----|-------------------|
| **SL 1** | Casual / coincidental misuse |
| **SL 2** | Intentional circumvention — simple means, low resources, generic skills, low motivation |
| **SL 3** | Sophisticated means, moderate resources, IACS-specific skills |
| **SL 4** | Extended resources, high motivation |

**CCLI target:** **SL-C(UC, component) = 2**.

### §6.2 Rationale (p. 38)

Post-authentication privilege restriction; asset owner assigns privileges (CCSC 3); verify before read/write/download/config change; only qualified/authorized individuals initiate changes.

### FR 2 — SL-C(UC, component) 2 checklist — **COMPLETE**

| CR | SL-C 2 requires | Maps to 3-3 | CCLI control | Status |
|----|-----------------|-------------|--------------|--------|
| **2.1** | **CR 2.1(1)(2)** | SR 2.1 + RE 1 + RE 2 | RBAC in `apps/ccli/` + OpenWrt | PART |
| **2.2** | **CR 2.2** | SR 2.2 | Wi‑Fi off; LTE usage policy | PART |
| **2.3** | — | SR 2.3 | **No component CR** in 4-2 | N/A |
| **2.4** | **EDR 2.4(1)** / SAR gap | SR 2.4 | No mobile code on Z0 | PART |
| **2.5** | **CR 2.5** | SR 2.5 | SSH/LuCI session lock | PART |
| **2.6** | **CR 2.6** | SR 2.6 | Remote session kill/timeout | PART |
| **2.7** | — | SR 2.7 | Not selected at SL 2 | N/A |
| **2.8** | **CR 2.8** | SR 2.8 | journald + UCI change log | PART |
| **2.9** | **CR 2.9** | SR 2.9 | Log rotation + buffer sizing | PART |
| **2.10** | **CR 2.10** | SR 2.10 | Alert on log daemon failure | PART |
| **2.11** | **CR 2.11(1)** | SR 2.11 + RE 1 | GNSS/NTP sync (K3.5) | PART |
| **2.12** | **CR 2.12** | SR 2.12 *(not selected SL 2 system)* | Audit ties user to action | PART |
| **2.13** | **EDR 2.13** | SR 2.13 | JTAG lock; USB console auth | PART |

**4-2 vs 3-3 at SL-C 2:** Component **CR 2.12** applies (system SR 2.12 not selected); **CR 2.11(1)** time sync required (system SL 2 = base SR 2.11 only); **CR 2.3** has no component requirement.

### CR 2.1 — Authorization enforcement (pp. 38–39)

**Requirement:** Authorization enforcement for all identified/authenticated users per assigned responsibilities.

**Guidance:** RBAC; ACLs; segregation of duties + least privilege; must not adversely affect operational performance; only qualified users initiate changes/upgrades.

**RE 1:** AuthZ enforcement for **all users** (human, software process, device) per responsibilities + least privilege → **SL 2+** CR 2.1(1).

**RE 2:** Authorized role can define/modify **permission→role mapping** for all human users (not fixed nested hierarchies) → **SL 2+** CR 2.1(2). *NOTE 1:* Also applies to software processes and devices.

**RE 3:** **Supervisor manual override** for configurable time or event sequence → **SL 3+** CR 2.1(3). *NOTE 2:* Emergency reaction without new higher-privilege session.

**RE 4:** **Dual approval** when action can seriously impact industrial process → **SL 4** CR 2.1(4). Not for immediate HSE response (e.g. ESD).

**SL-C(UC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | CR 2.1 |
| **2** | **CR 2.1(1)(2)** |
| 3 | CR 2.1(1)(2)(3) |
| 4 | CR 2.1(1)(2)(3)(4) |

**CCLI:** Operator vs maintainer vs service roles; document PF2 dual-approval policy at SL 4.

### CR 2.2 — Wireless use control (p. 40)

**Requirement:** If component supports wireless interfaces — integrate into system supporting usage **authorization, monitoring, and restrictions** per industry practice.

**RE:** None.

**SL-C(UC, component):** CR 2.2 at **all SL 1–4**.

**Guidance:** Distinguish wireless vs wired interfaces; network devices may assist admission control; dedicated scanners for unauthorized wireless activity.

**CCLI:** Wi‑Fi disabled on TG-524; LTE usage restrictions + logging.

### CR 2.3 — Use control for portable and mobile devices (p. 40)

**No component-level requirement** associated with 62443-3-3 SR 2.3.

**CCLI:** Portable/mobile restrictions remain **system/zone** scope (Z4 USB policy per 3-3 SR 2.3).

### CR 2.4 — Mobile code (p. 40)

Use control for mobile code is **component-type-specific** → **Clauses 12–15** (SAR 2.4, etc.).

**CCLI:** See **§12 SAR 2.4** (Batch G).

### CR 2.5 — Session lock (pp. 40–41)

**Requirement:** If human user interface (local or network):
- **a)** session lock after configurable inactivity or manual initiation; and
- **b)** lock remains until session owner or another authorized user re-establishes access via I&A.

**RE:** None.

**SL-C(UC, component):** CR 2.5 at **all SL 1–4**.

**Guidance:** Auto-activate after configurable period; may be pre-empted by remote session termination (CR 2.6).

**CCLI:** SSH `ClientAliveInterval`; LuCI session timeout; exclude emergency curtailment path (CCSC / §4.2).

### CR 2.6 — Remote session termination (pp. 41)

**Requirement:** If remote sessions supported — terminate after configurable inactivity, manually by **local authority**, or manually by session initiator.

**RE:** None.

**SL-C(UC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **CR 2.6** |
| 3 | CR 2.6 |
| 4 | CR 2.6 |

**Guidance:** Remote = across zone boundary; may limit to monitoring/maintenance (not critical ops); some components cannot terminate essential-function sessions.

**CCLI:** Admin kill maintainer SSH/LTE sessions; VPN idle timeout.

### CR 2.7 — Concurrent session control (pp. 41–42)

**Requirement:** Limit **concurrent sessions per interface** for any given user (human, software process, device).

**RE:** None.

**SL-C(UC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| 2 | Not selected |
| 3 | CR 2.7 |
| 4 | CR 2.7 |

**Guidance:** Prevent resource-starvation DoS; trade-off between locking out one user vs all users; supplier/SI guidance on session limits.

**CCLI:** Not required at SL-C 2; document if implemented.

### CR 2.8 — Auditable events (pp. 42–43)

**Requirement:** Generate audit records for:
- a) access control;
- b) request errors;
- c) control system events;
- d) backup and restore event;
- e) configuration changes; and
- f) audit log events.

Each record includes: timestamp, source (device/process/user), category, type, event ID, event result.

**RE:** None.

**SL-C(UC, component):** CR 2.8 at **all SL 1–4**.

**Guidance:** Cover all events from above categories generatable by firmware or OS; categories only apply if functionality provided by component.

**CCLI:** journald; UCI commit hooks; remote syslog to SIEM (pairs CR 2.12).

### CR 2.9 — Audit storage capacity (pp. 43)

**Requirement:**
- **a)** allocate audit storage per commonly recognized log-management recommendations; and
- **b)** protect against component failure when reaching/exceeding audit storage capacity.

**RE 1:** **Warn** when allocated audit storage reaches configurable threshold → **SL 3+** CR 2.9(1).

**SL-C(UC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | CR 2.9 |
| **2** | **CR 2.9** |
| 3 | CR 2.9(1) |
| 4 | CR 2.9(1) |

**Guidance:** Consider retention policy, scope, online processing; local buffer until offload to system storage; NIST SP 800-92 [19].

**CCLI:** Log rotation; persistent store sizing; warn threshold at SL 3+.

### CR 2.10 — Response to audit processing failures (pp. 43–44)

**Requirement:**
- **a)** protect against loss of essential services/functions on audit processing failure; and
- **b)** support appropriate response actions per industry practice.

**RE:** None.

**SL-C(UC, component):** CR 2.10 at **all SL 1–4**.

**Guidance:** Failures include SW/HW errors, capture mechanism failures, capacity exceeded; overwriting oldest records or halting generation are possible but lose forensic data; alert personnel.

**CCLI:** Monitor `logd`/journald health; do not stop PF2 control on logging failure.

### CR 2.11 — Timestamps (p. 44)

**Requirement:** Create timestamps (date + time) for audit records.

**RE 1:** Timestamps **synchronized with system-wide time source** → **SL 2+** CR 2.11(1).

**RE 2:** Time sync mechanism detects **unauthorized alteration** and causes audit event → **SL 4** CR 2.11(2).

**SL-C(UC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | CR 2.11 |
| **2** | **CR 2.11(1)** |
| 3 | CR 2.11(1) |
| 4 | CR 2.11(1)(2) |

**Guidance:** ISO/IEC 8601:2004 format; consider daylight-saving time shifts.

**CCLI:** GNSS/modem NTP (K3.5); protect NTP config in UCI.

### CR 2.12 — Non-repudiation (p. 45)

**Requirement:** If human user interface provided — capability to determine whether a given **human user** took a particular action; list controls unable to support in component docs.

**RE 1:** Non-repudiation for **all** users (human, software process, device) → **SL 4** CR 2.12(1).

**SL-C(UC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | CR 2.12 |
| **2** | **CR 2.12** |
| 3 | CR 2.12 |
| 4 | CR 2.12(1) |

**Guidance:** Operator actions, config changes, messages, approvals; digital signatures, timestamps, receipts.

**CCLI:** Audit log ties user ID to config commits and operator commands (pairs **CR 2.8**).

### CR 2.13 — Use of physical diagnostic and test interfaces (p. 45)

**Requirement:** Component-type-specific → **Clauses 12–15**.

**CCLI:** USB/JTAG/console policy under **EDR 2.13 / HDR 2.13** (Batch G/H).

---

## FR 3 — System integrity (SI) — **COMPLETE** (shared CRs)

### §7.1 Purpose and SL-C(SI) descriptions (pp. 45–46)

Ensure component integrity against unauthorized manipulation.

| SL | Protection against |
|----|-------------------|
| **SL 1** | Casual / coincidental manipulation |
| **SL 2** | Intentional — **simple means**, low resources, generic skills, low motivation |
| **SL 3** | Sophisticated means, moderate resources, IACS-specific skills |
| **SL 4** | Extended resources, high motivation |

**CCLI target:** **SL-C(SI, component) = 2**.

### §7.2 Rationale (p. 46)

Asset owner maintains integrity post-production; physical + logical assets in transit and at rest; environmental factors (EMI, vibration) affect comms integrity.

### FR 3 — SL-C(SI, component) 2 checklist — **COMPLETE**

| CR | SL-C 2 requires | Maps to 3-3 | CCLI control | Status |
|----|-----------------|-------------|--------------|--------|
| **3.1** | **CR 3.1(1)** | SR 3.1 + RE 1 | TLS/CRC on C1/C2; auth of origin | PART |
| **3.2** | — | SR 3.2(1) | **→ Clauses 12–15** (malware) | DEFER |
| **3.3** | **CR 3.3** | SR 3.3 | Security function verify checklist | PART |
| **3.4** | **CR 3.4(1)** | SR 3.4 + RE 1 | Secure Boot + signed `.ipk` (K6.1) | PART |
| **3.5** | **CR 3.5** | SR 3.5 | Input validation on control API | PART |
| **3.6** | **CR 3.6** | SR 3.6 | Deterministic DO on attack | PART |
| **3.7** | **CR 3.7** | SR 3.7 | Safe error messages | PART |
| **3.8** | **CR 3.8** | SR 3.8 | Session integrity (TLS/MMS) | PART |
| **3.9** | **CR 3.9** | SR 3.9 | Protect audit logs | PART |
| **3.10–3.14** | **EDR 3.10(1)–3.14(1)** | SR 3.10+ | **§13 EDR** — boot/RoT/OTA | PART |

### CR 3.1 — Communication integrity (pp. 46–47)

**Requirement:** Protect integrity of transmitted information.

**RE 1:** Verify **authenticity** of received information during communication → **SL 2+** CR 3.1(1).  
*NOTE:* Integrity + authentication of origin achievable without confidentiality.

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | CR 3.1 |
| **2** | **CR 3.1(1)** |
| 3 | CR 3.1(1) |
| 4 | CR 3.1(1) |

**Guidance:** Packet manipulation; point-to-point physical access may suffice at lower SL; compensating CM or risk acceptance when comms service cannot assure controls; sealed RJ-45/M12, shielded cable for harsh environments.

**CCLI:** 62351 TLS on MMS; Modbus CRC; no L2 bridge (K3.3).

### CR 3.2 — Protection from malicious code (p. 47)

**Requirement:** Component-type-specific → **Clauses 12–15**.

**CCLI:** **SAR 3.2 / EDR 3.2** — USB/firewall entry points (K6.1).

### CR 3.3 — Security functionality verification (pp. 47–48)

**Requirement:** Support verification of security functions per **62443-3-3 SR 3.3**.

**RE 1:** Verification **during normal operation** → **SL 4** CR 3.3(1) *(implement carefully — not suitable for safety systems)*.

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | CR 3.3 |
| **2** | **CR 3.3** |
| 3 | CR 3.3 |
| 4 | CR 3.3(1) |

**Examples:** EICAR AV test; unauthorized login attempt; IDS rule trigger; audit logging active.

**CCLI:** Lab checklist at FAT: Secure Boot, firewall deny, lockout test.

### CR 3.4 — Software and information integrity (pp. 48–49)

**Requirement:** Perform/support integrity checks on software, configuration, information; record and report results.

**RE 1:** **Authenticity** checks on software/config/information → **SL 2+** CR 3.4(1).  
**RE 2:** **Automated notification** to configurable entity on unauthorized change → **SL 3+** CR 3.4(2).

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | CR 3.4 |
| **2** | **CR 3.4(1)** |
| 3 | CR 3.4(1)(2) |
| 4 | CR 3.4(1)(2) |

**Guidance:** Cryptographic hashes; monitor field device config for breaches.

**CCLI:** **Secure Boot + signed `.ipk`** (K6.1); notify on failed verify.

### CR 3.5 — Input validation (pp. 48–49)

**Requirement:** Validate syntax, length, and content of process control inputs and external inputs impacting component action.

**RE:** None.

**SL-C(SI, component):** CR 3.5 at **all SL 1–4**.

**Guidance:** Pre-screen interpreter inputs; OWASP; buffer overflow, SQL injection, malformed packets; not human error / out-of-range legitimate values.

**CCLI:** Validate setpoints, curtailment commands, UCI/API in `apps/ccli/`.

### CR 3.6 — Deterministic output (pp. 49–50)

**Requirement:** Outputs connected to automation process — set to predetermined state if normal operation cannot be maintained.

**Configurable fail modes:** **Unpowered** · **Hold** (last-known good) · **Fixed** · **Dynamic** (state-dependent).

**SL-C(SI, component):** CR 3.6 at **all SL 1–4**.

**CCLI:** PF2 curtailment DO policy — document hold vs unpowered for attack/degraded mode (§4.2).

### CR 3.7 — Error handling (p. 50)

**Requirement:** Handle errors without information exploitable by adversaries.

**SL-C(SI, component):** CR 3.7 at **all SL 1–4**.

**Guidance:** Do not reveal invalid user vs invalid password; OWASP Code Review Guide.

**CCLI:** Generic API/SSH errors; detail in protected audit log only.

### CR 3.8 — Session integrity (pp. 50–51)

**Requirement:** Protect session integrity:
- a) invalidate session IDs on logout/termination;
- b) unique system-generated session IDs only;
- c) randomness from accepted sources.

**RE:** None listed at SL 2.

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **CR 3.8** |
| 3 | CR 3.8 |
| 4 | CR 3.8 |

**Guidance:** MITM, hijacking, replay; overhead in real-time comms.

**CCLI:** TLS session management for MMS; LuCI/SSH session IDs.

### CR 3.9 — Protection of audit information (p. 51)

**Requirement:** Protect audit information, logs, and tools from unauthorized access, modification, deletion.

**RE 1:** Store audit records on **hardware-enforced write-once media** → **SL 4** CR 3.9(1).

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **CR 3.9** |
| 3 | CR 3.9 |
| 4 | CR 3.9(1) |

**CCLI:** Protected `/var/log`; remote syslog; append-only where feasible.

### CR 3.10 — Support for updates (p. 51)

→ **§13 EDR 3.10** — **HAVE** (pp. 66). **CCLI:** signed OTA (K2.7).

### CR 3.11 — Physical tamper resistance (p. 52)

→ **§13 EDR 3.11** — **HAVE** (pp. 66–67). **CCLI:** enclosure/seal policy.

### CR 3.12 — Provisioning product supplier roots of trust (p. 52)

→ **§13 EDR 3.12** — **HAVE** (pp. 67–68). **CCLI:** TPM supplier RoT (K6.2).

### CR 3.13 — Provisioning asset owner roots of trust (p. 52)

→ **§13 EDR 3.13** — **HAVE** (pp. 68–69). **CCLI:** owner key ceremony (K6.3).

### CR 3.14 — Integrity of the boot process (p. 52)

→ **§13 EDR 3.14** — **HAVE** (p. 69). **CCLI:** Secure Boot chain (K6.1) **P0 lab**.

---

## FR 4 — Data confidentiality (DC) — **COMPLETE** (shared CRs)

### §8.1 Purpose and SL-C(DC) descriptions (p. 52)

Ensure confidentiality of information on channels and in repositories.

| SL | Protection against |
|----|-------------------|
| **SL 1** | Eavesdropping / casual exposure |
| **SL 2** | Active search — simple means, low resources, generic skills, low motivation |
| **SL 3** | Sophisticated means, moderate resources, IACS-specific skills |
| **SL 4** | Extended resources, high motivation |

**CCLI target:** **SL-C(DC, component) = 2**.

### §8.2 Rationale (p. 52)

Component-generated information at rest or in transit may be confidential/sensitive.

### FR 4 — SL-C(DC, component) 2 checklist — **COMPLETE**

| CR | SL-C 2 requires | Maps to 3-3 | CCLI control | Status |
|----|-----------------|-------------|--------------|--------|
| **4.1** | **CR 4.1** | SR 4.1(1) at system | **62351 TLS** on C1/C2; at-rest where read-auth | PART |
| **4.2** | **CR 4.2** | SR 4.2 | Factory reset / key wipe (K6.3) | PART |
| **4.3** | **CR 4.3** | SR 4.3 | AES/SHA/NIST crypto policy | PART |

*Note:* 3-3 SL-2 adds SR 4.1(1) for untrusted networks at **system** level; component CR 4.1 applies at all SL 1–4 — integration + compensating controls for MMS plaintext (V-01).

### CR 4.1 — Information confidentiality (pp. 52–53)

**Requirement:**
- a) protect confidentiality of information **at rest** where explicit read authorization is supported;
- b) support protection of confidentiality **in transit** per **62443-3-3 SR 4.1**.

**RE:** None.

**SL-C(DC, component) mapping:** CR 4.1 at **all SL 1–4**.

**Guidance:** Protection need depends on deployment context; if component supports explicit read authorization, provide at-rest protection; in-transit needs system-level support from component.

**CCLI:** **62351 TLS** on MMS/104 (K6.5); optional disk encryption for credential stores.

### CR 4.2 — Information persistence (pp. 53–54)

**Requirement:** Erase all read-authorized information before release from active service or decommissioning.

**RE 1:** Protect against unauthorized/unintended transfer via **volatile shared memory** → **SL 3+** CR 4.2(1).  
**RE 2:** **Verify** information erasure → **SL 3+** CR 4.2(2).

**SL-C(DC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **CR 4.2** |
| 3 | CR 4.2(1)(2) |
| 4 | CR 4.2(1)(2) |

**Guidance:** Auth/network config in NVRAM; volatile memory attacks before overwrite.

**CCLI:** Factory reset SOP; TPM clear; overlay wipe (links 4-1 SG-4).

### CR 4.3 — Use of cryptography (p. 54)

**Requirement:** If cryptography required — use mechanisms per internationally recognized practices (AES, SHA, NIST SP 800-57, FIPS 140-2, ISO/IEC 19790).

**RE:** None.

**SL-C(DC, component):** CR 4.3 at **all SL 1–4**.

**Guidance:** Document key management; links CR 1.8 PKI, CR 5.10.

**CCLI:** OpenSSL cipher policy; TPM-backed keys (K6.2).

---

## FR 5 — Restricted data flow (RDF) — **COMPLETE** (shared CRs)

### §9.1 Purpose and SL-C(RDF) descriptions (p. 55)

Segment control system via zones and conduits to limit unnecessary data flow.

| SL | Protection against |
|----|-------------------|
| **SL 1** | Casual / coincidental circumvention of segmentation |
| **SL 2** | Intentional — simple means, low resources, generic skills, low motivation |
| **SL 3** | Sophisticated means, moderate resources, IACS-specific skills |
| **SL 4** | Extended resources, high motivation |

**CCLI target:** **SL-C(RDF, component) = 2** — links `CCI_62443_Zones.md`.

### §9.2 Rationale (p. 55)

Risk assessment per 62443-3-2; disconnect from business/public networks; unidirectional gateways; stateful firewalls; DMZ.

### FR 5 — SL-C(RDF, component) 2 checklist — **COMPLETE**

| CR | SL-C 2 requires | Maps to 3-3 | CCLI control | Status |
|----|-----------------|-------------|--------------|--------|
| **5.1** | **CR 5.1** | SR 5.1(1) at system | Port isolation — no L2 bridge (K3.3) | PART |
| **5.2** | **NDR 5.2(1)** | SR 5.2(1) | Deny-by-default nftables (`net_policy`) | PART |
| **5.3** | **NDR 5.3** | SR 5.3 | Block email/social/P2P on Z0 | PART |
| **5.4** | — | SR 5.4 | **No component CR** in 4-2 | N/A |

*Note:* 3-3 SL-2 requires **physical** segmentation SR 5.1(1) and **deny-by-default** SR 5.2(1) at system level — TG-524 delivers via **NDR 5.2** + hardware port map.

### CR 5.1 — Network segmentation (pp. 55–56)

**Requirement:** Support segmented network facilitating zones/conduits based on **logical segmentation** and criticality.

**RE:** None.

**SL-C(RDF, component):** CR 5.1 at **all SL 1–4**.

**Guidance:** Reduce ingress/egress; isolate critical/SIS; logical vs physical trade-off; incident response may break segment connections while preserving essential ops; DHCP/DNS/local CA may need isolation.

**CCLI:** Four Ethernet domains; **no Eth_A ↔ plant L2 bridge** (K3.3).

### CR 5.2 — Zone boundary protection (p. 56)

→ **§15 NDR 5.2** — **HAVE** (pp. 82). **CCLI:** OpenWrt deny-by-default firewall (`net_policy`); island/fail-close at SL 3+.

### CR 5.3 — General-purpose person-to-person communication restrictions (p. 56)

→ **§15 NDR 5.3** — **HAVE** (pp. 82–83). **CCLI:** No email/social/IM on Z0 image.

### CR 5.4 — Application partitioning (p. 56)

**No component-level requirement** associated with 62443-3-3 SR 5.4.

**CCLI:** Z0 app vs maintainer partition — architecture policy, not cert CR.

---

## FR 6 — Timely response to events (TRE) — started

### §10.1 Purpose and SL-C(TRE) descriptions (p. 56)

Respond to security violations — notify authorities, report evidence, corrective action.

| SL | Monitoring / reporting |
|----|-------------------------|
| **SL 1** | Collect and provide forensic evidence when queried |
| **SL 2** | **Periodically report** forensic evidence |
| **SL 3** | Actively collect and **push** evidence to authorities |
| **SL 4** | Push evidence in **near real-time** |

### §10.2 Rationale (p. 56)

Continuous monitoring maintains secure state; timely notification critical for risk mitigation.

### CR 6.1 — Audit log accessibility (p. 57 — *not in batch*)

**Gap:** Ingest p. **57** for CR 6.1 requirement + SL. Trace from 3-3 SR 6.1.

### CR 6.2 — Continuous monitoring (pp. 58)

**Requirement:** Continuously monitor security mechanisms; detect, characterize, report violations; reporting mechanisms for timely response.

**RE:** None.

**SL-C(TRE, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **CR 6.2** |
| 3 | CR 6.2 |
| 4 | CR 6.2 |

**Guidance:** SIEM correlates events into aggregate reports; must not adversely affect control performance.

**CCLI:** journald + remote syslog; firewall/auth failure alerts.

**Gap:** CR 6.1 still on p. **57** — FR 6 not fully **COMPLETE** until captured.

---

## FR 7 — Resource availability (RA) — **COMPLETE** (shared CRs)

### §11.1 Purpose and SL-C(RA) descriptions (pp. 58–59)

Ensure components available despite degradation or denial of essential services.

| SL | Protection |
|----|------------|
| **SL 1** | DoS casual/coincidental; normal production |
| **SL 2** | DoS — simple means; normal + abnormal conditions |
| **SL 3** | Sophisticated means; normal + abnormal + extreme |
| **SL 4** | Extended resources; normal + abnormal + extreme |

**CCLI target:** **SL-C(RA, component) = 2**.

### §11.2 Rationale (p. 58)

Resilience against partial/total DoS; security incidents must not affect essential/safety functions.

### FR 7 — SL-C(RA, component) 2 checklist — **COMPLETE**

| CR | SL-C 2 requires | Maps to 3-3 | CCLI control | Status |
|----|-----------------|-------------|--------------|--------|
| **7.1** | **CR 7.1(1)** | SR 7.1 + RE 1 | Rate limits on WAN/LTE/MMS | PART |
| **7.2** | **CR 7.2** | SR 7.2 | Cap security job resource use | PART |
| **7.3** | **CR 7.3(1)** | SR 7.3 + RE 1 | Verified config backup before OTA | PART |
| **7.4** | **CR 7.4** | SR 7.4 | Secure rollback to known state | PART |
| **7.5** | — | SR 7.5 | **No component CR** in 4-2 | HW PSU |
| **7.6** | **CR 7.6** | SR 7.6 | UCI + nft export | PART |
| **7.7** | **CR 7.7** | SR 7.7 | Minimal attack surface | PART |
| **7.8** | **CR 7.8** | SR 7.8 | **SBOM / `.ipk` inventory** (K6.7) | PART |

### CR 7.1 — Denial of service protection (pp. 58–59)

**Requirement:** Maintain **essential functions** in **degraded mode** after a DoS event.

**RE 1:** Mitigate information/message **flooding** DoS → **SL 2+** CR 7.1(1).

**SL-C(RA, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | CR 7.1 |
| **2** | **CR 7.1(1)** |
| 3 | CR 7.1(1) |
| 4 | CR 7.1(1) |

**Guidance:** Design for safe continued operation under DoS degradation.

**CCLI:** nftables rate limits on C3/C5; connection tracking caps.

### CR 7.2 — Resource management (p. 59)

**Requirement:** Limit resource use by **security functions** to protect against exhaustion.

**RE:** None.

**SL-C(RA, component):** CR 7.2 at **all SL 1–4**.

**Guidance:** Lower-priority scans/patching must not disrupt control processes; traffic rate limiting.

**CCLI:** Schedule maintenance off-peak; QoS on plant traffic.

### CR 7.3 — Control system backup (pp. 59–60)

**Requirement:** Participate in **system-level backup** of component state (user + system info) without affecting normal operation.

**RE 1:** **Validate backup integrity** before restoration → **SL 2+** CR 7.3(1).

**SL-C(RA, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | CR 7.3 |
| **2** | **CR 7.3(1)** |
| 3 | CR 7.3(1) |
| 4 | CR 7.3(1) |

**Guidance:** Crypto keys **not** stored in general backup — separate secure procedure.

**CCLI:** Verified overlay/config backup before OTA (K2.7).

### CR 7.4 — Control system recovery and reconstitution (pp. 60–61)

**Requirement:** Recover/reconstitute to **known secure state** after disruption or failure.

**RE:** None.

**SL-C(RA, component):** CR 7.4 at **all SL 1–4**.

**Guidance:** Secure parameter values, patches, config settings, known secure backups.

**CCLI:** A/B partition + Secure Boot rollback (K2.7).

### CR 7.5 — Emergency power (p. 61)

**No component-level requirement** associated with 62443-3-3 SR 7.5.

**CCLI:** PSU hold-up / brownout policy — hardware architecture (C10).

### CR 7.6 — Network and security configuration settings (pp. 61–62)

**Requirement:** Configure per supplier-recommended network/security settings; **interface to current deployed settings**.

**RE 1:** **Machine-readable report** of current security settings → **SL 3+** CR 7.6(1).

**SL-C(RA, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | CR 7.6 |
| **2** | **CR 7.6** |
| 3 | CR 7.6(1) |
| 4 | CR 7.6(1) |

**Guidance:** Monitor and control configuration changes per security policies.

**CCLI:** `uci export`; nftables ruleset dump; config drift checks.

### CR 7.7 — Least functionality (pp. 61–62)

**Requirement:** Restrict unnecessary functions, ports, protocols, and/or services.

**RE:** None.

**SL-C(RA, component):** CR 7.7 at **all SL 1–4**.

**CCLI:** Disable Wi‑Fi, unused daemons, minimal OpenWrt image (K3.7).

### CR 7.8 — Control system component inventory (p. 62)

**Requirement:** Support component inventory per **62443-3-3 SR 7.8**; augment overall inventory (62443-2-4 SP.06.02 compatible).

**RE:** None.

**SL-C(RA, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **CR 7.8** |
| 3 | CR 7.8 |
| 4 | CR 7.8 |

**CCLI:** **SBOM / `.ipk` manifest** (K6.7).

---

## §12 Software application requirements (SAR) — **PARTIAL**

### §12.1 Purpose (p. 62)

Document requirements **specific to software applications** (`apps/ccli/` CCI stack).

### SAR 2.4 — Mobile code (pp. 62 — partial; p. 63 gap)

**Requirement:** If a software application utilizes mobile code technologies, it shall enforce a security policy allowing, at minimum, for each mobile code technology:
- **a)** control execution of mobile code;
- **b)** control which users (human, software process, device) may upload mobile code; and
- **c)** control execution based on **integrity check** results prior to execution.

**Gap:** Rationale, **RE 1** (authenticity check), and **SL table** — p. **63** *(not in batch)*. Parallel text: **EDR 2.4** (p. 64).

**CCLI (expected SL-C 2):** No mobile code on Z0 CCI stack; block LuCI plugins/scripts.

### SAR 3.2 — Protection from malicious code (p. 63 — gap)

**Gap:** Full requirement + SL — p. **63** *(not in batch)*. Parallel: **EDR 3.2** (pp. 65–66).

**CCLI (expected SL-C 2):** Validate `.ipk`/app inputs at API boundary (K6.1).

---

## §13 Embedded device requirements (EDR) — **COMPLETE** *(P0 TG-524 lab scope)*

### §13.1 Purpose (p. 64)

Document requirements **specific to embedded devices** (OpenWrt firmware on MT798X SoM).

### EDR — SL-C(SI/UC, component) 2 checklist — **COMPLETE**

| Clause | SL-C 2 requires | Maps to CR | CCLI control | Status |
|--------|-----------------|------------|--------------|--------|
| **EDR 2.4(1)** | Mobile code authenticity | CR 2.4 | No mobile code on Z0 | PART |
| **EDR 2.13** | Protect JTAG/diag interfaces | CR 2.13 | JTAG lock; USB console auth | PART |
| **EDR 3.2** | Block unauthorized software | CR 3.2 | Signed `.ipk`; read-only root | PART |
| **EDR 3.10(1)** | Signed update verify | CR 3.10 | OTA signature check (K2.7) | PART |
| **EDR 3.11** | Tamper resistance/detection | CR 3.11 | Enclosure screws/seals | PART |
| **EDR 3.12** | Supplier RoT provisioning | CR 3.12 | TPM factory keys (K6.2) | PART |
| **EDR 3.13** | Asset-owner RoT provisioning | CR 3.13 | Owner cert ceremony (K6.3) | PART |
| **EDR 3.14(1)** | Boot authenticity via RoT | CR 3.14 | Secure Boot chain (K6.1) | PART |

### EDR 2.4 — Mobile code (pp. 64)

**Requirement:** If embedded device uses mobile code — enforce security policy with:
- **a)** control execution;
- **b)** control upload by user (human, process, device); and
- **c)** control execution based on **integrity check** prior to execution.

**RE 1:** Control execution based on **authenticity check** prior to execution → **SL 2+** EDR 2.4(1).

**SL-C(UC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | EDR 2.4 |
| **2** | **EDR 2.4(1)** |
| 3 | EDR 2.4(1) |
| 4 | EDR 2.4(1) |

**Guidance:** Java, JavaScript, ActiveX, PDF, Postscript, Flash, VBScript; disallow unacceptable mobile code in control system; controlled adjacent environment OK.

**CCLI:** Minimal OpenWrt image; no browser plugins on Z0.

### EDR 2.13 — Use of physical diagnostic and test interfaces (pp. 64–65)

**Requirement:** Protect against **unauthorized use** of physical factory diagnostic/test interfaces (e.g. **JTAG debugging**).

**RE 1:** **Active monitoring** of diag interfaces; **audit log** on access attempts → **SL 3+** EDR 2.13(1).

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **EDR 2.13** |
| 3 | EDR 2.13(1) |
| 4 | EDR 2.13(1) |

**Guidance:** JTAG control requires auth; boundary scan may not; networked diag interfaces subject to all CRs; threat/risk assessment determines auth need.

**CCLI:** Disable JTAG in production fuse; USB serial auth for commissioning (Z4).

### EDR 3.2 — Protection from malicious code (pp. 65–66)

**Requirement:** Protect from **installation and execution of unauthorized software**.

**RE:** None.

**SL-C(SI, component):** EDR 3.2 at **all SL 1–4**.

**Guidance:** Compensating controls in IACS may suffice per risk assessment; binary integrity monitoring, hashing, signatures; NX/DEP, ASLR, sandbox, restricted firmware update; see CR 6.2 monitoring.

**CCLI:** Secure Boot + signed `.ipk`; no arbitrary package install on Z0.

### EDR 3.10 — Support for updates (pp. 66)

**Requirement:** Support ability to be **updated and upgraded**.

**RE 1:** **Validate authenticity and integrity** of any software update/upgrade prior to installation → **SL 2+** EDR 3.10(1).

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | EDR 3.10 |
| **2** | **EDR 3.10(1)** |
| 3 | EDR 3.10(1) |
| 4 | EDR 3.10(1) |

**Guidance:** Patch without impacting essential functions on HA systems (CCSC / §4.2); redundancy example.

**CCLI:** Signed sysupgrade; A/B partition; essential PF2 functions maintained during OTA (K2.7).

### EDR 3.11 — Physical tamper resistance and detection (pp. 66–67)

**Requirement:** Tamper **resistance and detection** against unauthorized physical access.

**RE 1:** **Automatic notification** to configurable recipients on unauthorized physical access attempt; log all tamper notifications in audit log → **SL 3+** EDR 3.11(1).

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **EDR 3.11** |
| 3 | EDR 3.11(1) |
| 4 | EDR 3.11(1) |

**Guidance:** Hardened enclosures, locks, encapsulation, security screws, tight airflow paths; tamper-evident seals/tapes/switches.

**CCLI:** Enclosure security screws; tamper-evident seal on commissioning cover.

### EDR 3.12 — Provisioning product supplier roots of trust (pp. 67–68)

**Requirement:** Provision and protect confidentiality, integrity, and authenticity of **product supplier keys/data** as one or more **roots of trust** at manufacture.

**RE:** None.

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **EDR 3.12** |
| 3 | EDR 3.12 |
| 4 | EDR 3.12 |

**Guidance:** Trusted data for validating HW/SW/firmware before boot; crypto hashes or public key for signature validation; HW-protected; modification limited to supplier provisioning via trusted API (no exposure of protected data).

**CCLI:** TPM or SoC OTP stores supplier RoT; validates boot images (K6.2).

### EDR 3.13 — Provisioning asset owner roots of trust (pp. 68–69)

**Requirement:**
- **a)** provision and protect confidentiality, integrity, authenticity of **asset owner keys/data** as roots of trust; and
- **b)** support provisioning **without reliance on components outside the device's security zone**.

**RE:** None.

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **EDR 3.13** |
| 3 | EDR 3.13 |
| 4 | EDR 3.13 |

**Guidance:** Owner extensions/mobile code need validated origin; supplier provides secure owner RoT provisioning; protects comms integrity (CR 3.1) and confidentiality (CR 4.1); links **EDR 2.4** mobile code validation.

**CCLI:** Owner TLS/cert provisioning ceremony; no cloud dependency for key install (K6.3).

### EDR 3.14 — Integrity of the boot process (p. 69)

**Requirement:** Verify **integrity** of firmware, software, and configuration data needed for boot and runtime **prior to use**.

**RE 1:** Use **product supplier roots of trust** to verify **authenticity** of boot firmware/software/config prior to use → **SL 2+** EDR 3.14(1).

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | EDR 3.14 |
| **2** | **EDR 3.14(1)** |
| 3 | EDR 3.14(1) |
| 4 | EDR 3.14(1) |

**Guidance:** Prevent boot into insecure/invalid state; integrity checks during boot before execution.

**CCLI:** Verified boot chain MT798X + TPM quote (K6.1) — **P0 lab evidence**.

---

## CCLI traceability — EDR design actions (§13)

| Clause | Design action | Owner | Gate |
|--------|---------------|-------|------|
| EDR 2.4(1) | No mobile code; block unsigned scripts | FW | Image policy |
| EDR 2.13 | JTAG disabled; USB console auth | FW + HW | D3 |
| EDR 3.2 | Signed packages only | FW | K6.1 |
| EDR 3.10(1) | Signed OTA + non-disruptive update | FW | **K2.7** |
| EDR 3.11 | Tamper-evident enclosure | HW | C10 |
| EDR 3.12 | Supplier RoT in TPM/OTP | FW | **K6.2** |
| EDR 3.13 | Owner key provisioning (offline) | FW | **K6.3** |
| EDR 3.14(1) | Secure Boot + config integrity | FW | **K6.1** |

---

## §14 Host device requirements (HDR) — **PARTIAL**

### HDR 3.14 — Integrity of the boot process (p. 75 — tail only)

**RE 1:** Use **product supplier roots of trust** to verify **authenticity** of boot firmware/software/config prior to use.

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | HDR 3.14 |
| **2** | **HDR 3.14(1)** |
| 3 | HDR 3.14(1) |
| 4 | HDR 3.14(1) |

**Gap:** HDR 2.4, 2.13, 3.2, 3.10–3.14 body — pp. **69–74** *(not in batch)*. TG-524: partial relevance (USB commissioning console); primary evidence via **EDR 3.14**.

---

## §15 Network device requirements (NDR) — **COMPLETE**

### §15.1 Purpose (p. 75)

Document requirements **specific to network devices** (OpenWrt firewall/routing on TG-524).

### NDR — SL-C 2 checklist — **COMPLETE**

| Clause | SL-C 2 requires | Maps to CR | CCLI control | Status |
|--------|-----------------|------------|--------------|--------|
| **NDR 1.6(1)** | Unique wireless auth | CR 1.6 | LTE cert/auth (K3.3) | PART |
| **NDR 1.13** | Untrusted-network access control | CR 1.13 | LTE/VPN ACL + logging | PART |
| **NDR 2.4(1)** | Mobile code authenticity | CR 2.4 | No LuCI plugins/scripts | PART |
| **NDR 2.13** | Protect diag interfaces | CR 2.13 | JTAG lock; serial auth | PART |
| **NDR 3.2** | Malicious code protection | CR 3.2 | Packet filter + signed FW | PART |
| **NDR 3.10(1)** | Signed update verify | CR 3.10 | Signed OpenWrt OTA (K2.7) | PART |
| **NDR 3.11** | Tamper resistance/detection | CR 3.11 | Enclosure policy | PART |
| **NDR 3.12** | Supplier RoT | CR 3.12 | TPM keys (K6.2) | PART |
| **NDR 3.13** | Owner RoT | CR 3.13 | Owner cert ceremony (K6.3) | PART |
| **NDR 3.14(1)** | Boot authenticity via RoT | CR 3.14 | Secure Boot (K6.1) | PART |
| **NDR 5.2(1)** | Deny all, permit by exception | CR 5.2 | nftables default DROP (K3.3) | PART |
| **NDR 5.3** | Block P2P/email/social | CR 5.3 | Minimal image; no mail clients | PART |

### NDR 1.6 — Wireless access management (pp. 75)

**Requirement:** If supporting wireless access management — **identify and authenticate** all users (human, software process, device) in wireless communication.

**RE 1:** **Uniquely** identify and authenticate all wireless users → **SL 2+** NDR 1.6(1).

**SL-C(IAC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | NDR 1.6 |
| **2** | **NDR 1.6(1)** |
| 3 | NDR 1.6(1) |
| 4 | NDR 1.6(1) |

**Guidance:** Wireless = communication protocol option subject to same IACS security; physical countermeasures less effective on wireless.

**CCLI:** Wi‑Fi off; LTE modem — unique client cert or per-device credentials (K3.3).

### NDR 1.13 — Access via untrusted networks (pp. 75–76)

**Requirement:** Monitor and **control all methods of access** to the network device via **untrusted networks**.

**RE 1:** **Deny** access requests via untrusted networks unless **explicitly approved** by assigned role → **SL 3+** NDR 1.13(1).

**SL-C(IAC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | NDR 1.13 |
| **2** | **NDR 1.13** |
| 3 | NDR 1.13(1) |
| 4 | NDR 1.13(1) |

**Guidance:** Protect against unauthorized connections; remote access (dial-up, broadband, wireless); office network connections; ACL by MAC/VLAN (L2) or IP/port/protocol/VPN (L3).

**CCLI:** LTE WAN = untrusted; VPN + firewall rules; explicit maintainer approval workflow at SL 3+.

### NDR 2.4 — Mobile code (pp. 76–77)

**Requirement:** If mobile code used — enforce policy: control execution; control upload/transfer users; control execution based on **integrity check** prior to execution.

**RE 1:** Control execution based on **authenticity check** prior to execution → **SL 2+** NDR 2.4(1).

**SL-C(UC, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | NDR 2.4 |
| **2** | **NDR 2.4(1)** |
| 3 | NDR 2.4(1) |
| 4 | NDR 2.4(1) |

**Guidance:** Java, JS, ActiveX, PDF, Flash, VBScript; integrity/authenticity/authorization at app layer or secure tunnel for JIT execution.

**CCLI:** No mobile code on OpenWrt routing plane.

### NDR 2.13 — Use of physical diagnostic and test interfaces (pp. 77–78)

**Requirement:** Protect against **unauthorized use** of physical factory diagnostic/test interfaces (e.g. JTAG).

**RE 1:** **Active monitoring** of diag interfaces; **audit log** on access attempts → **SL 3+** NDR 2.13(1).

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **NDR 2.13** |
| 3 | NDR 2.13(1) |
| 4 | NDR 2.13(1) |

**CCLI:** Same as EDR 2.13 — JTAG fuse; USB serial auth.

### NDR 3.2 — Protection from malicious code (p. 78)

**Requirement:** Provide protection from **malicious code**.

**RE:** None.

**SL-C(SI, component):** NDR 3.2 at **all SL 1–4**.

**Guidance:** Compensating control OK (e.g. upstream packet filtering); evaluate local USB host access scenarios.

**CCLI:** nftables + signed firmware; no arbitrary package install.

### NDR 3.10 — Support for updates (pp. 78–79)

**Requirement:** Support ability to be **updated and upgraded**.

**RE 1:** **Validate authenticity and integrity** of updates prior to installation → **SL 2+** NDR 3.10(1).

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | NDR 3.10 |
| **2** | **NDR 3.10(1)** |
| 3 | NDR 3.10(1) |
| 4 | NDR 3.10(1) |

**Guidance:** Patch without impacting essential HA functions (§4.2); redundancy example.

**CCLI:** Signed sysupgrade on OpenWrt (K2.7).

### NDR 3.11 — Physical tamper resistance and detection (p. 79)

**Requirement:** Tamper **resistance and detection** against unauthorized physical access.

**RE 1:** **Automatic notification** on unauthorized physical access attempt; log in audit → **SL 3+** NDR 3.11(1).

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **NDR 3.11** |
| 3 | NDR 3.11(1) |
| 4 | NDR 3.11(1) |

**CCLI:** Enclosure tamper-evident seals (shared with EDR 3.11).

### NDR 3.12 — Provisioning product supplier roots of trust (pp. 79–80)

**Requirement:** Provision and protect supplier keys/data as **roots of trust** at manufacture.

**RE:** None.

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **NDR 3.12** |
| 3 | NDR 3.12 |
| 4 | NDR 3.12 |

**Guidance:** Validate HW/SW before boot; HW-protected; API returns validation results without exposing protected data.

**CCLI:** TPM supplier RoT (K6.2) — same chain as EDR.

### NDR 3.13 — Provisioning asset owner roots of trust (pp. 80–81)

**Requirement:**
- **a)** provision and protect owner keys/data as roots of trust; and
- **b)** provision **without reliance on components outside device security zone**.

**RE:** None.

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | Not selected |
| **2** | **NDR 3.13** |
| 3 | NDR 3.13 |
| 4 | NDR 3.13 |

**Guidance:** Owner extensions/mobile code need validated origin; links **NDR 2.4**, **CR 3.1**, **CR 4.1**.

**CCLI:** Offline owner TLS cert install (K6.3).

### NDR 3.14 — Integrity of the boot process (pp. 81–82)

**Requirement:** Verify **integrity** of firmware, software, and config for boot **prior to use**.

**RE 1:** Use **product supplier roots of trust** to verify **authenticity** prior to boot use → **SL 2+** NDR 3.14(1).

**SL-C(SI, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | NDR 3.14 |
| **2** | **NDR 3.14(1)** |
| 3 | NDR 3.14(1) |
| 4 | NDR 3.14(1) |

**CCLI:** Secure Boot chain (K6.1) — shared evidence with EDR 3.14.

### NDR 5.2 — Zone boundary protection (p. 82)

**Requirement:** At zone boundary — **monitor and control communications** to enforce zones/conduits compartmentalization.

**RE 1:** **Deny all, permit by exception** (default deny) → **SL 2+** NDR 5.2(1).

**RE 2:** **Island mode** — block all boundary communication → **SL 3+** NDR 5.2(2).

**RE 3:** **Fail close** — block boundary comms on protection mechanism failure → **SL 3+** NDR 5.2(3).

**SL-C(RDF, component) mapping:**

| SL | Requirements |
|----|--------------|
| 1 | NDR 5.2 |
| **2** | **NDR 5.2(1)** |
| 3 | NDR 5.2(1)(2)(3) |
| 4 | NDR 5.2(1)(2)(3) |

**Guidance:** Managed interfaces (firewalls, gateways, DMZ); alternate sites same protection; island mode for breach/enterprise attack; fail close on HW/power failure.

**CCLI:** nftables **default DROP** on C3/C5 WAN/LTE; explicit allow rules per conduit (`CCI_62443_Zones.md`); island mode script at SL 3+.

### NDR 5.3 — General-purpose person-to-person communication restrictions (pp. 82–83)

**Requirement:** At zone boundary — protect against **general-purpose P2P messages** (email, IM, social) from external users/systems.

**RE:** None.

**SL-C(RDF, component):** NDR 5.3 at **all SL 1–4**.

**Guidance:** Email, social media, executable attachments; risks outweigh benefits; block by port/address; application-layer firewalls.

**CCLI:** No mail/IM clients in OpenWrt image; block SMTP/IMAP/443 to social at boundary.

---

## CCLI traceability — NDR design actions (§15)

| Clause | Design action | Owner | Gate |
|--------|---------------|-------|------|
| NDR 1.6(1) | Unique LTE wireless auth | FW | **K3.3** |
| NDR 1.13 | Untrusted-network ACL + monitor | FW | K3.3 / zones |
| NDR 5.2(1) | Deny-by-default firewall rules | FW | **K3.3** |
| NDR 5.3 | No P2P/email on boundary | FW | Image policy |
| NDR 3.10(1) | Signed OpenWrt OTA | FW | K2.7 |
| NDR 3.14(1) | Boot authenticity (shared EDR) | FW | K6.1 |

---

## CCLI traceability — FR 6–7 design actions

| CR | Design action | Owner | Gate |
|----|---------------|-------|------|
| 6.2 | Continuous monitoring / syslog alerts | FW | Ops runbook |
| 6.1 | Read-only audit access | FW | p. 57 capture |
| 7.1(1) | Rate limits on WAN/LTE/MMS | FW | K3.3 |
| 7.3(1) | Verified backup before OTA | FW | K2.7 |
| 7.4 | Secure rollback | FW | K2.7 |
| 7.6 | UCI/nft config export | FW | Lab |
| 7.7 | Minimal attack surface | FW | K3.7 |
| 7.8 | SBOM / `.ipk` inventory | FW | **K6.7** |

---

## CCLI traceability — FR 4–5 design actions

| CR | Design action | Owner | Gate |
|----|---------------|-------|------|
| 4.1 | TLS on C1/C2; at-rest protection for cred stores | FW + protocols | K6.5 |
| 4.2 | Decommission key/credential purge | FW | K6.3 |
| 4.3 | Crypto policy (AES/SHA/NIST) | FW | OpenSSL config |
| 5.1 | Physical/logical port segmentation | FW + HW | K3.3 |
| 5.2 | Deny-by-default firewall | FW | **NDR 5.2** |
| 5.3 | No P2P/email on Z0 | FW | Image hardening |

---

## CCLI traceability — FR 3 design actions

| CR | Design action | Owner | Gate |
|----|---------------|-------|------|
| 3.1(1) | Authenticity/integrity on MMS/104; TLS on C1/C2 | FW + protocols | K6.5 |
| 3.3 | Security verify checklist at lab bring-up | QA | FAT |
| 3.4(1) | Secure Boot + signed images | FW | K6.1 |
| 3.5 | Input validation on control API | FW | D6 |
| 3.6 | Deterministic DO on attack | FW + HW | PF2 policy |
| 3.7 | Safe error messages | FW | D6 |
| 3.8 | Session integrity | FW | K6.5 |
| 3.9 | Protect audit logs | FW | CR 2.8 pair |
| 3.10–3.14 | Updates, RoT, boot | FW | **EDR §13 HAVE** |

---

## CCLI traceability — FR 1 design actions

| CR | Design action | Owner | Gate |
|----|---------------|-------|------|
| 1.1(1) | Per-user console/SSH; no shared `admin` | FW | Architecture freeze |
| 1.2 | Peer auth on MMS; document Modbus trust | FW + protocols | K6.5 |
| 1.3–1.5 | Account/credential lifecycle; TPM wrap | FW | K6.3 |
| 1.7 | Password strength policy | FW | K6.3 |
| 1.8 | PKI / cert provisioning for 62351 | FW + protocols | **K6.5** |
| 1.9 | TLS cert validation; TPM private keys | FW | K6.2/K6.5 |
| 1.10–1.12 | Masked login; lockout; banner | FW | D6 |
| 1.14 | Symmetric key policy (if used) | FW | Crypto policy |

**Deferred to device clauses:** CR 1.6 / 1.13 → **NDR HAVE**; CR 1.13 also SAR/EDR/HDR variants.

---

## CCLI traceability — FR 2 design actions

| CR | Design action | Owner | Gate |
|----|---------------|-------|------|
| 2.1(1)(2) | RBAC; role→permission mapping | FW | Architecture freeze |
| 2.2 | Wi‑Fi off; LTE usage policy | FW | K3.3 |
| 2.5 | SSH/LuCI session lock | FW | D6 |
| 2.6 | Remote session termination | FW | Ops runbook |
| 2.8 | Audit categories + record fields | FW | Lab |
| 2.9 | Audit storage + rotation | FW | Ops |
| 2.10 | No essential-service loss on log failure | FW | Lab |
| 2.11(1) | GNSS/NTP time sync | FW | **K3.5** |
| 2.12 | User→action audit trail | FW | Lab |
| 2.4 / 2.13 | Mobile code; physical diag | FW | §12–15 / EDR+NDR |

---

## Annex B — Table B.1 SL mapping — **HAVE** (pp. 87–92)

### B.1 Overview (p. 87)

Annex B (informative) maps **CRs and REs** to **SL-C(component) 1–4** per FR. Checkmarks in Table B.1 denote requirements for a given `SL-C(FR, component)` level. Cumulative: SL 2 = all SL 1 reqs + SL 2 additions.

**Example (FR 7 RA):** SL 1 = CR 7.1–7.7; SL 2 adds CR 7.1 RE(1), CR 7.3 RE(1), CR 7.8; SL 3 adds CR 7.6 RE(1); SL 4 = SL 3 (no FR 7 extras).

**Full SL vector:** see **62443-3-3 Annex A** — TG-524 target `{2222222}` system + component types below.

### B.2 Key (p. 92)

| Abbrev | Meaning |
|--------|---------|
| **CR** | Component requirement — all types |
| **SAR** | Software application |
| **EDR** | Embedded device |
| **HDR** | Host device |
| **NDR** | Network device |

### Table B.1 — TG-524 SL-C 2 master checklist

Product types in lab scope: **EDR** + **NDR** + **SAR** (`apps/ccli/`). Rows marked ✓ are **mandatory at SL-C 2** per Table B.1.

#### FR 1 — IAC (`SL-C(IAC) = 2`)

| Requirement | Type | SL 2 | CCLI |
|-------------|------|:----:|------|
| CR 1.1 | CR | ✓ | Unique human users |
| CR 1.1 RE(1) | RE | ✓ | Per-user console/SSH |
| CR 1.2 | CR | ✓ | MMS/Modbus peer auth |
| CR 1.3–1.5 | CR | ✓ | Account/credential lifecycle |
| NDR 1.6 + RE(1) | NDR | ✓ | LTE unique wireless auth |
| CR 1.7 | CR | ✓ | Password policy |
| CR 1.8 | CR | ✓ | PKI / 62351 (**MISS** K6.5) |
| CR 1.9 | CR | ✓ | TLS cert validation |
| CR 1.10–1.12 | CR | ✓ | Masked login; lockout; banner |
| NDR 1.13 | NDR | ✓ | Untrusted-network control |
| CR 1.14 | CR | ✓ | Symmetric key (if used) |

#### FR 2 — UC (`SL-C(UC) = 2`)

| Requirement | Type | SL 2 | CCLI |
|-------------|------|:----:|------|
| CR 2.1 + RE(1)(2) | CR | ✓ | RBAC; role mapping |
| CR 2.2 | CR | ✓ | Wi‑Fi off; LTE policy |
| CR 2.3 | — | — | No component CR |
| SAR/EDR/NDR 2.4 + RE(1) | Type | ✓ | No mobile code; authenticity |
| CR 2.5–2.6 | CR | ✓ | Session lock; remote kill |
| CR 2.8–2.10 | CR | ✓ | Audit log + storage + failure handling |
| CR 2.11 + RE(1) | CR | ✓ | GNSS/NTP sync (K3.5) |
| CR 2.12 | CR | ✓ | Non-repudiation *(component SL 2; 3-3 system N/A)* |
| EDR/NDR 2.13 | Type | ✓ | JTAG/console protection |

#### FR 3 — SI (`SL-C(SI) = 2`)

| Requirement | Type | SL 2 | CCLI |
|-------------|------|:----:|------|
| CR 3.1 + RE(1) | CR | ✓ | TLS/CRC; auth of origin |
| SAR/EDR/NDR 3.2 | Type | ✓ | Signed FW; input validation |
| CR 3.3–3.7 | CR | ✓ | Verify checklist; Secure Boot; API validation |
| CR 3.8–3.9 | CR | ✓ | Session integrity; protect audit logs |
| EDR/NDR 3.10 + RE(1) | Type | ✓ | Signed OTA (K2.7) |
| EDR/NDR 3.11 | Type | ✓ | Tamper-evident enclosure |
| EDR/NDR 3.12–3.13 | Type | ✓ | Supplier + owner RoT (K6.2/K6.3) |
| EDR/NDR 3.14 + RE(1) | Type | ✓ | Secure Boot chain (K6.1) |

#### FR 4 — DC (`SL-C(DC) = 2`)

| Requirement | Type | SL 2 | CCLI |
|-------------|------|:----:|------|
| CR 4.1 | CR | ✓ | TLS on C1/C2 |
| CR 4.2 | CR | ✓ | Decommission key purge |
| CR 4.3 | CR | ✓ | Crypto policy |

#### FR 5 — RDF (`SL-C(RDF) = 2`)

| Requirement | Type | SL 2 | CCLI |
|-------------|------|:----:|------|
| CR 5.1 | CR | ✓ | Four Ethernet domains (K3.3) |
| NDR 5.2 + RE(1) | NDR | ✓ | Deny-by-default firewall |
| NDR 5.3 | NDR | ✓ | No email/social on Z0 |

#### FR 6 — TRE (`SL-C(TRE) = 2`)

| Requirement | Type | SL 2 | CCLI |
|-------------|------|:----:|------|
| CR 6.1 | CR | ✓ | Audit log accessibility *(p. 57 body gap)* |
| CR 6.2 | CR | ✓ | Continuous monitoring / syslog |

#### FR 7 — RA (`SL-C(RA) = 2`)

| Requirement | Type | SL 2 | CCLI |
|-------------|------|:----:|------|
| CR 7.1 + RE(1) | CR | ✓ | Rate limits WAN/LTE |
| CR 7.2–7.4 | CR | ✓ | Resource caps; verified backup; rollback |
| CR 7.6–7.7 | CR | ✓ | UCI/nft export; least functionality |
| CR 7.8 | CR | ✓ | SBOM / `.ipk` inventory (K6.7) |
| CR 7.5 | — | — | No component CR |

### Table B.1 — SL 2 exclusions (not required)

| Requirement | Notes |
|-------------|-------|
| CR 1.1 RE(2) MFA | SL 3+ |
| CR 1.2 RE(1) unique process/device | SL 3+ |
| CR 1.5 RE(1) HW authenticators | SL 3+ |
| NDR 1.13 RE(1) explicit approval | SL 3+ |
| CR 2.1 RE(3)(4) override / dual approval | SL 3–4 |
| CR 2.7 concurrent sessions | SL 3+ |
| CR 2.9 RE(1) audit storage warn | SL 3+ |
| CR 2.11 RE(2) time-source integrity | SL 4 |
| CR 2.12 RE(1) non-repudiation all users | SL 4 |
| EDR/NDR 2.13 RE(1) active monitoring | SL 3+ |
| CR 3.4 RE(2) auto integrity notify | SL 3+ |
| EDR/NDR 3.11 RE(1) tamper notify | SL 3+ |
| NDR 5.2 RE(2)(3) island / fail-close | SL 3+ |
| CR 6.1 RE(1) programmatic audit access | SL 3+ |
| CR 7.6 RE(1) machine-readable config report | SL 3+ |

### Verification use

Table B.1 is the **authoritative SL-C 2 row** for lab quote and CRS traceability. Cross-check every ✓ against `ccli-62443-3-3-extract` SL-2 system rows + device-type clauses in this document.

**K6.4 corpus status:** **HAVE** for 62443-4-2 normative + Annex B; implementation gaps remain at **CR 1.8 PKI** and SAR p. 63.

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| FR 1 IAC | **HAVE** — CR 1.1–1.14 | — | — |
| FR 2 UC | **HAVE** — CR 2.1–2.13 (2.3 N/A; 2.4/2.13→§12–15) | — | — |
| FR 3 SI (shared) | **HAVE** — CR 3.1–3.9 + EDR 3.10–3.14 | — | — |
| FR 4 DC | **HAVE** — CR 4.1–4.3 | — | — |
| FR 5 RDF (shared) | **HAVE** — CR 5.1 + NDR 5.2(1)/5.3 | — | — |
| FR 6 TRE | CR 6.2 **HAVE**; CR 6.1 gap | p. **57** | — |
| FR 7 RA | **HAVE** — CR 7.1–7.8 | — | — |
| §12 SAR | SAR 2.4 partial; SAR 3.2 gap | p. **63** | Optional |
| §13 EDR | **HAVE** — EDR 2.4, 2.13, 3.2, 3.10–3.14 | — | — |
| §15 NDR | **HAVE** — NDR 1.6, 1.13, 2.4, 2.13, 3.x, 5.2, 5.3 | — | — |
| §14 HDR | HDR 3.14 SL tail only | pp. 69–74 | Optional |
| CR 1.6 / 1.13 | **NDR HAVE** | — | — |
| Annex B | **HAVE** — Table B.1 SL-2 master checklist | — | — |
| Annex A | Not captured | Device category examples | pp. 84–86 optional |
| §3.1 start | Partial (from 3.1.6) | Terms 3.1.1–3.1.5 | p. **18** optional |
| 62351 PKI | CR 1.8 captured | TLS cert provisioning implement | K6.5 |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-642-001 | Test 3-3 SR evidence instead of 4-2 CR + EDR | Lab rejection | Quote 4-2; include §13 EDR in scope |
| R-642-002 | Miss CR 3.10–3.14 (boot/RoT/updates) | SL-C 2 SI fail | **EDR §13 captured** — implement K6.1/K2.7 |
| R-642-003 | Multi-type product partial compliance | Cert gap | **EDR + NDR mapped**; SAR p.63 optional |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| Batch H NDR | NDR 1.6–5.3 SL-2 rows | CR 6.1 p.57; Annex B | Internal review |
| 3-3 trace | Matrix | Every 3-3 SL-2 SR → 4-2 CR row | CRS |
| EDR scope | Lab quote | §13 + shared CRs in test plan | **HAVE** — verify implementation |
| Annex B | **HAVE** | Table B.1 SL-2 ✓ rows mapped to CCLI | CRS / lab quote |

---

## Source pages — Batch A

| Page | Content | Status |
|------|---------|--------|
| **12** | Foreword — TC 65; conformity disclaimer | PASS |
| **14** | §0.1 Overview — IACS vs IT; derives from 3-3; Annex A/B refs | PASS |
| **16** | **Figure 1** — 62443 series map | PASS |
| **17** | §1 Scope (7 FRs, SL-C); §2 Normative refs (1-1, 3-3, 4-1) | PASS |
| **19–23** | §3.1 Terms 3.1.6–3.1.49 (partial) | PASS |
| **24–25** | §3.2 Acronyms (CR, EDR, HDR, NDR, SAR, SL-C, TPM…) | PASS |
| **26** | §3.3 Conventions — CR↔SR traceability; four component types | PASS |
| **27** | §4 CCSC 1–4; §5.1 FR 1 IAC purpose + SL descriptions | PASS |

### Batch B — FR 1 IAC (pp. 27–38)

| Page | Content | Status |
|------|---------|--------|
| **28** | §5.2 rationale; **CR 1.1** human user ID/auth + RE 1–2 | PASS |
| **29** | CR 1.1 SL; **CR 1.2** process/device auth | PASS |
| **30** | CR 1.2 SL; **CR 1.3** account mgmt; **CR 1.4** identifier start | PASS |
| **31** | *(not in batch)* CR 1.4 SL + CR 1.5 body | GAP |
| **32** | CR 1.5 RE(1) HW + SL; CR 1.6→§15; **CR 1.7** start | PASS |
| **33** | CR 1.7 SL; **CR 1.8** PKI | PASS |
| **34–35** | **CR 1.9** public key auth + RE(1) HW | PASS |
| **35–36** | **CR 1.10** feedback; **CR 1.11** lockout | PASS |
| **36–37** | **CR 1.12** use notification; CR 1.13→§12–15 | PASS |
| **37–38** | **CR 1.14** symmetric key + SL; **FR 2** start; CR 2.1 req | PASS |

### Batch C — FR 2 UC (pp. 39–44)

| Page | Content | Status |
|------|---------|--------|
| **39** | CR 2.1 RE 1–4 + SL; guidance RBAC/dual approval | PASS |
| **40** | **CR 2.2** wireless; CR 2.3 N/A; CR 2.4→§12–15; **CR 2.5** session lock | PASS |
| **41** | CR 2.5 SL; **CR 2.6** remote termination; **CR 2.7** concurrent sessions | PASS |
| **42** | CR 2.7 SL; **CR 2.8** auditable events | PASS |
| **43** | CR 2.8 SL; **CR 2.9** audit storage; **CR 2.10** audit failure start | PASS |
| **44** | CR 2.10 SL; **CR 2.11** timestamps + RE 1–2 + SL | PASS |

### Batch D — FR 3 SI + FR 4 start (pp. 45–52)

| Page | Content | Status |
|------|---------|--------|
| **45** | **CR 2.12** non-repudiation; CR 2.13→§12–15; **FR 3** purpose | PASS |
| **46** | §7.2 rationale; **CR 3.1** comm integrity | PASS |
| **47** | CR 3.1 SL; CR 3.2→§12–15; **CR 3.3** verify | PASS |
| **48** | CR 3.3 SL; **CR 3.4** software integrity; CR 3.5 start | PASS |
| **49** | CR 3.5 SL; **CR 3.6** deterministic output | PASS |
| **50** | CR 3.6 SL; **CR 3.7** errors; **CR 3.8** session start | PASS |
| **51** | CR 3.8 SL; **CR 3.9** audit protect; CR 3.10→§12–15 | PASS |
| **52** | CR 3.11–3.14→§12–15; **FR 4** start; CR 4.1 partial | PASS |

### Batch E — FR 4–7 start (pp. 52–58)

| Page | Content | Status |
|------|---------|--------|
| **52** | CR 3.11–3.14→§12–15; **FR 4** purpose; CR 4.1 req | PASS |
| **53** | CR 4.1 SL; **CR 4.2** persistence start | PASS |
| **54** | CR 4.2 SL; **CR 4.3** cryptography | PASS |
| **55** | **FR 5** purpose; **CR 5.1** segmentation | PASS |
| **56** | CR 5.1 SL; CR 5.2–5.3→§15; CR 5.4 N/A; **FR 6** start | PASS |
| **57** | *(not in batch)* **CR 6.1** audit access | GAP |
| **58** | CR 6.2 SL; **FR 7** start; CR 7.1 partial | PASS |

### Batch F — FR 6–7 + SAR start (pp. 58–62)

| Page | Content | Status |
|------|---------|--------|
| **58** | CR 6.2 SL + SIEM; **FR 7** purpose; **CR 7.1** req | PASS |
| **59** | CR 7.1 SL; **CR 7.2** resource mgmt; **CR 7.3** backup start | PASS |
| **60** | CR 7.3 SL; **CR 7.4** recovery | PASS |
| **61** | CR 7.4 SL; CR 7.5 N/A; **CR 7.6–7.7** | PASS |
| **62** | CR 7.7 SL; **CR 7.8** inventory; **§12 SAR 2.4** start | PASS |

### Batch G — §12 SAR + §13 EDR (pp. 62–69)

| Page | Content | Status |
|------|---------|--------|
| **62** | §12 SAR 2.4 requirement (partial) | PASS |
| **63** | SAR 2.4 SL + SAR 3.2 | **GAP** |
| **64** | §13 EDR purpose; **EDR 2.4** mobile code; **EDR 2.13** start | PASS |
| **65** | EDR 2.13 SL; **EDR 3.2** malicious code | PASS |
| **66** | EDR 3.2 SL; **EDR 3.10** updates; **EDR 3.11** tamper start | PASS |
| **67** | EDR 3.11 RE + SL; **EDR 3.12** supplier RoT | PASS |
| **68** | EDR 3.12 SL; **EDR 3.13** owner RoT | PASS |
| **69** | EDR 3.13 SL; **EDR 3.14** boot integrity; §14 HDR start | PASS |

### Batch H — §15 NDR (pp. 75–83)

| Page | Content | Status |
|------|---------|--------|
| **75** | HDR 3.14 SL tail; **§15** purpose; **NDR 1.6** wireless; **NDR 1.13** start | PASS |
| **76** | NDR 1.13 RE + SL; **NDR 2.4** mobile code | PASS |
| **77** | NDR 2.4 SL; **NDR 2.13** diag interfaces | PASS |
| **78** | NDR 2.13 SL; **NDR 3.2**; **NDR 3.10** updates start | PASS |
| **79** | NDR 3.10 SL; **NDR 3.11** tamper; **NDR 3.12** supplier RoT start | PASS |
| **80** | NDR 3.12 SL; **NDR 3.13** owner RoT | PASS |
| **81** | NDR 3.13 SL; **NDR 3.14** boot integrity | PASS |
| **82** | NDR 3.14 SL; **NDR 5.2** zone boundary; **NDR 5.3** start | PASS |
| **83** | NDR 5.3 rationale + SL | PASS |

### Batch K — Annex B Table B.1 (pp. 87–92)

| Page | Content | Status |
|------|---------|--------|
| **87** | Annex B B.1 overview; B.2 table intro; FR 7 SL example | PASS |
| **88** | Table B.1 **FR 1 IAC** rows | PASS |
| **89** | Table B.1 **FR 2 UC** rows | PASS |
| **90** | Table B.1 FR 2 tail + **FR 3 SI** start | PASS |
| **91** | Table B.1 FR 3 tail + **FR 4 DC** | PASS |
| **92** | Table B.1 **FR 5–7** + Key (CR/SAR/EDR/HDR/NDR) | PASS |

### Next capture

```
57      ← CR 6.1 audit access (body)
63      ← SAR 2.4 SL + SAR 3.2 (optional)
84–86   ← Annex A device categories (optional)
```

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-08-11 | Batch A ingest pp. 12–27 — foundation + CCSC + FR 1 start |
| **1.1** | 2026-08-11 | **Batch B** pp. 27–38: FR 1 complete (CR 1.1–1.14); FR 2 CR 2.1 partial |
| **1.2** | 2026-08-11 | **Batch D** pp. 45–52: FR 2 tail (2.12–2.13); FR 3 shared CRs; FR 4 start |
| **1.3** | 2026-08-11 | **Batch E** pp. 52–58: FR 4–5 complete; FR 6–7 started |
| **1.4** | 2026-08-11 | **Batch F** pp. 58–62: FR 7 complete; SAR 2.4 start |
| **1.5** | 2026-08-11 | **Batch C** pp. 39–44: FR 2 complete (CR 2.1–2.11) |
| **1.6** | 2026-08-11 | **Batch G** pp. 64–69: §13 EDR complete (P0 lab); SAR p.63 gap |
| **1.7** | 2026-08-11 | **Batch H** pp. 75–83: §15 NDR complete; HDR 3.14 tail |
| **1.8** | 2026-08-11 | **Batch K** pp. 87–92: Annex B Table B.1 SL-C master checklist |

---

## RAG cross-links

| source_id | Document |
|-----------|----------|
| `ccli-62443-4-2-extract` | This document |
| `ccli-62443-4-2-capture-plan` | ToC + batch order |
| `ccli-62443-3-3-extract` | Parent SR/SL-C (system) |
| `ccli-62443-4-1-extract` | SDL process (CCSC 4) |
| `ccli-62443-zones-extract` | SL-T 2 zones/conduits |

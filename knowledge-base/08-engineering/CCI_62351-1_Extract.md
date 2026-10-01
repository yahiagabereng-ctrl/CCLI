# IEC TS 62351-1 — CCLI Engineering Extract

**Document ID:** CCLI-SEC-62351-002  
**Revision:** 1.2  
**Date:** 2026-08-11  
**RAG source_id:** `ccli-62351-1-extract`  
**Normative basis:** IEC TS 62351-1:2007(E) — *Power systems management … Data and communications security — Part 1: Introduction to security issues*  
**Capture status:** **HAVE** (intro) — pp. **4, 6–7, 9–23, 24–34**; bibliography p. **35** optional  
**Parents:** `ccli-62351-1-capture-plan` · `ccli-62443-zones-extract` · `ccli-62443-4-2-extract`  
**Programme link:** K6.5 · KR-001

---

## Summary

**62351-1** introduces the **IEC 62351** series: security for **IEC TC 57** power-system communication protocols (**60870-5**, **60870-6**, **61850**, **61970/61968**). Normative profiles live in **62351-3 … -6**; **62351-7** covers network/system management.

For **HiTEKS CCLI / TG-524:** this doc establishes **why** protocol security is required and **which part** applies per conduit — it unblocks design for **62443 CR 1.8 PKI**, **CR 3.1(1)**, **CR 4.1** on **C1/C2**.

**Core design principle (§5.8):** For power operations, **authentication of control actions** is often **more important than encryption alone**. VPN/bump-in-the-wire alone is **insufficient** for end-to-end security.

---

## Requirements (from §1 Scope)

| REQ-ID | Source | Requirement | CCLI conduit |
|--------|--------|-------------|--------------|
| **REQ-62351-SCOPE-001** | §1.1 | Secure TC 57 protocols: 60870-5/6, 61850, 61970/68 + end-to-end issues | C1, C2 |
| **REQ-62351-SCOPE-002** | §1.2 | **62351-3…-6** = protocol profiles; backward compatible, phased deploy | Lab rollout |
| **REQ-62351-SCOPE-003** | §1.2 | **62351-7** = NSM / end-to-end monitoring | P1 ops |
| **REQ-62351-E2E-001** | §1, §5.3.5 | End-to-end = policies, enforcement, IDS, health, audit — not wire-only | 62443 zones |
| **REQ-62351-AUTH-001** | §5.8 | Auth of authorized actions **primary** over confidentiality hiding | MMS peer auth |
| **REQ-62351-RISK-001** | §5.5 | Risk-based degree of security — not 100% on all assets | SL-T 2 rationale |

---

## Architecture — 62351 series placement

```
62443-3-2/3-3     WHAT to protect (zones, SL-T, SR/CR)
62351-1           WHY + threat model + part map  ← this doc
62351-3           TCP/IP + TLS transport
62351-4           MMS / 61850 application security  → C1
62351-5           60870-5 security                    → C2
62351-6           61850 profiles (GOOSE/SMV)          → plant multicast
62351-7           Network/system management (NSM)
62443-4-2         Component proof CR 1.8 binds PKI to product
```

**Normative references (§2):**

| Standard | TG-524 role |
|----------|-------------|
| **IEC 60870-5** | **C2** — 104 on Eth_B |
| **IEC 60870-6** | ICCP/TASE.2 — out of scope unless DSO requires |
| **IEC 61850** | **C1** — MMS; GOOSE/SMV if plant-side |

**Terms:** → **62351-2** glossary (not in corpus).

---

## §1 Scope and object (pp. 6–7)

### §1.1 Scope

Information security for **power system control operations**. Develop security for TC 57 protocols listed above plus **end-to-end** security issues.

### §1.2 Object — part map

| Part | Role |
|------|------|
| **62351-1** | Introduction |
| **62351-3 … -6** | Security for TC 57 **communication profiles** — selectable levels, backward compatible |
| **62351-7** | End-to-end via **network/system management** |

**Threat drivers:** deregulated market intelligence; inadvertent + deliberate actions; terrorism; dual infrastructure (power + information).

**End-to-end definition:** Authenticated access, authorized market data, reliable equipment status, backup, audit reconstruction. Protocol parts (62351-3…-6) secure **between two systems** — not full internal/end-to-end alone.

**§1 tail:** Bump-in-the-wire / VPN **not adequate alone**; need policies, enforcement, IDS, internal health, audit.

---

## §4 Background (pp. 7–9 start)

### §4.1 Rationale

**Security by obscurity rejected.** Protocols had **no security** historically. Market gaming incentive. Threat range: carelessness → espionage → terrorism.

### §4.3 History (p. 9)

**IEC/TR 62210** → **IEC TC 57 WG 15** (1999). Rejected **ISO Common Criteria** TOE as too cumbersome; adopted **threat-mitigation analysis**.

---

## §5 Security issues (pp. 9–23) — **COMPLETE**

### §5.1 General

Context and scope for normative parts.

### §5.2 Threat types

**Assets:** physical equipment, HW, buildings, people, information, DBs, applications.

#### Inadvertent (§5.2.2)

| Threat | Notes | CCLI |
|--------|-------|------|
| Safety failures | Field crew HV environment | DO interlocks; procedural logging |
| Equipment failures | Most common reliability threat | PF2 monitoring |
| Carelessness | Tailgating, unlocked doors, exposed passwords | Maintainer training |
| Natural disasters | Storms, earthquakes | Enclosure; tamper |

#### Deliberate (§5.2.3)

| Threat | Notes |
|--------|-------|
| Disgruntled employee | Insider knowledge — high damage |
| Industrial espionage | Deregulated market — bid manipulation |
| Vandalism | Physical damage without gain |
| Cyber hackers | Internet entry; firewalls + isolation |
| Viruses/worms | Internet or infected laptops on “secure” links |
| Theft | Equipment/data for financial gain |
| Terrorism | Low probability; highest consequence |

### §5.3 Requirements, threats, attacks, countermeasures

#### §5.3.1 Four security requirements (+ authentication premise)

| Requirement | Definition |
|-------------|------------|
| **Confidentiality** | Prevent unauthorized **access** to information |
| **Integrity** | Prevent unauthorized **modification or theft** |
| **Availability** | Prevent **DoS**; ensure authorized access |
| **Non-repudiation** | Prevent denial/ false claim of actions |

#### §5.3.2 Four threat types (cyber)

Unauthorized access · unauthorized modification/theft · DoS · repudiation.

#### §5.3.3 Vulnerabilities

Weaknesses permitting deliberate or inadvertent unauthorized actions — bugs, design flaws, equipment failure, physical actions.

#### §5.3.4 Attacks

No single countermeasure; **chain of attacks** across assets over time.

#### §5.3.5 Four security categories (Figure 2)

| Layer | Typical attacks | Countermeasures |
|-------|-----------------|-----------------|
| **Human-machine interface** | Masquerade, bypass, theft, carelessness | RBAC, individual auth |
| **Software applications** | Trojan, virus, bugs | AuthZ, testing, patch mgmt |
| **Communications transport** | Eavesdrop, MITM, replay, DoS | TLS, encryption, ACLs, signatures |
| **Communications media** | RF intercept, path failure | Restricted access, redundancy |

**All four layers** needed for end-to-end security. **VPN alone** = transport only — does not stop masquerade or malicious host apps.

#### §5.3.6 Countermeasures (Figs 3–7)

Mesh of technologies; **not all countermeasures always** — overkill harms usability/performance. Select per need (Fig 7 overview).

**Key technologies cited:**

| Technology | Maps to |
|------------|---------|
| **PKI / Certificates** | CR 1.8, 1.9 |
| **TLS** | 62351-3/4 |
| **VPN** | C5 LTE (insufficient alone) |
| **IEC 62351 Security for DNP, IEC 61850** | **62351-4/5/6** |
| **RBAC** | CR 2.1 |
| **IDS / Firewalls / ACL** | NDR 5.2, CR 6.2 |
| **Digital signatures / HMAC / AES** | Integrity + non-repudiation |
| **Audit logging** | CR 2.8, 2.12 |
| **Certificate and key management** | K6.3 ceremony |

### §5.3.7 Problem decomposition

Two pitfalls: enterprise-wide one-size-fits-all vs technology-only (passwords/VPN everywhere).

**Three partition approaches:**

| Partition | Definition | CCLI mapping |
|-----------|------------|--------------|
| **Physical security perimeter** | Six-wall border around control rooms | Substation / cabinet |
| **Electronic security perimeter** | Logical network border | **62443 zone** boundary |
| **Security domain** | Org area with uniform policy + single manager | **Z0** CCI; DSO as separate domain |

Inter-domain comms need **special security services**.

### §5.4 Security policies

Living documents; training; disciplinary actions; adjunct docs for network config, firewalls, protocol security, password/cert assignment. Review **≥ annually**.

### §5.5 Security risk assessment

Not 100% on all assets — impractical. Assess damage vs cost of countermeasures **before** deployment.

### §5.6 Power system operations constraints (§5.6.1–5.6.2)

| Constraint | Implication for TG-524 |
|------------|------------------------|
| **DoS > IT priority** | Blocking dispatcher worse than blocking bank user — aligns **62443 FR 7** |
| **Narrowband / limited CPU** | Cannot always afford full TLS handshake overhead — profile selection |
| **Remote unmanned sites** | Key management, cert revocation difficult |
| **Multi-drop channels** | Standard IT network security may not apply |
| **Wireless in substations** | Noise + latency; security overhead affects throughput |
| **Key management** | Manual tech visit vs local key server |
| **Cert revocation** | No Internet — need secure distribution to field equipment |

### §5.6.3 System and network management

Fragmented view today; SCADA minimal comms monitoring. **2003 NE US blackout** — lack of timely information. Future: IED-resident software, peer field comms, self-healing — drives **62351-7**.

### §5.7 Five-step security process (Fig 8, p. 22)

Continuous cycle:

1. **Assessment** — assets, risks, policies, procurement  
2. **Policy** — domain policies, security plan  
3. **Deployment** — install, test, IDS/audit procedures  
4. **Training** — evolving threats/technologies  
5. **Audit** — constant monitoring, not post-event only  

Security designed in from start; retrofit costly; residual risk always remains.

### §5.8 Applying security to power system operations (p. 23)

**Defense in depth:** VPN + **62351-4** application security + RBAC + IDS + ACL + physical locks.

**Authentication priority:** For most power ops, **auth of control actions > hiding data via encryption**.

**Isolation:** Plant ops should not depend on Internet exposure — firewalls/isolation.

---

## CCLI conduit mapping — **Figure 9 correlation (p. 26)**

| 62351 part | TC 57 profile | TG-524 conduit | Priority |
|------------|---------------|----------------|----------|
| **62351-3** TLS/TCP | TASE.2, **61850 MMS**, **60870-5-104** | C1, C2, C5 | **P0** |
| **62351-4** MMS app-layer | TASE.2, **61850-8-1 MMS** | **C1** Eth_A | **P0** |
| **62351-5** 60870-5 auth | **60870-5-104**, -101/-102/-103, DNP3/TCP | **C2** Eth_B | **P0** |
| **62351-6** multicast auth | **61850 GOOSE**, **61850-9-2 SMV** | Plant multicast (P1) | P1 |
| **62351-7** NSM objects | All profiles + IDS/firewall health | Ops monitoring | P1 |

**Stack for C1 MMS:** **62351-3 (TLS)** + **62351-4 (MMS auth/MAC)** = end-to-end through application layer.

**Stack for C2 104:** **62351-3 (TLS)** + **62351-5 (app auth)** on networked path; serial -101 uses **62351-5 only** (no TLS).

---

## §6 Overview of IEC 62351 series (pp. 24–33) — **COMPLETE**

### §6.1 Scope of series (p. 24)

Security standards for three protocol families: **60870-5**, **60870-6 (TASE.2)**, **61850** — plus NSM/end-to-end via **62351-7**. Security varies by protocol and usage.

### §6.2 Authentication as key requirement (p. 24)

Authentication central to **confidentiality, integrity, non-repudiation**. Series supports **RBAC** at user and/or application level; **application-layer authentication minimum** for RBAC.

### §6.3 Objectives (pp. 24–25)

Digital signatures, authorized access, anti-eavesdrop, anti-playback/spoofing, intrusion detection. Constraints: field device compute, media speed, protective relaying response times.

**Out of scope:** security policies, employee training, system design decisions (asset owner).

### §6.4 Part map + PICS (pp. 25–26)

Each part addresses requirements with **PICS** (mandatory/optional per security level):

| Part | Title | Scope |
|------|-------|-------|
| **62351-1** | Introduction | This doc |
| **62351-2** | Glossary | Terms |
| **62351-3** | **TCP/IP profiles** | ICCP, **104**, DNP3/TCP, 61850/TCP |
| **62351-4** | **MMS profiles** | ICCP, **61850 MMS** |
| **62351-5** | **60870-5** + derivatives | Serial + networked |
| **62351-6** | **61850 non-TCP** | **GOOSE, GSSE, SMV** |
| **62351-7** | **NSM MIB** | End-to-end network/system mgmt |

**Security measures (§6.4 intro):** authentication, digital signatures, encryption (keys + messages where affordable), integrity/tamper detect, playback/spoof prevention, secure+non-secure coexistence (phased upgrade), infrastructure monitoring, unified identity management.

### §6.7 IEC 62351-3 — TCP/IP (pp. 26–27)

**Threats countered:** eavesdrop (TLS encrypt), MITM (message auth), spoof (certs), replay (TLS).

**Not countered by TLS alone:** **DoS** (implementation-specific); **application-layer** security (→ 62351-4/5).

**TLS vs IPSec:** **TLS selected** for end-to-end transport; IPSec for LAN segments (may combine).

**Interoperability levels:**

| TLS | App auth | Mode |
|-----|----------|------|
| No | No | Backward compatible |
| No | Yes | VPN / internal CC |
| Yes | No | Encrypt + node auth only |
| Yes | Yes | **Full security** |

**Technical measures (2007 edition — verify current TLS profile in normative -3):**

- Deprecate SSL 1.0/2.0; **TLS 1.0+** minimum
- Deprecate non-encrypting cipher suites
- Key renegotiation: **10 min** or **5 000 packets** (transparent)
- At least **AES** cipher suite
- Small certificates to reduce field burden
- **VPN / bump-in-the-wire (AGA 12-2):** out of 62351 scope but allowed in overall solution

**CCLI:** OpenWrt **TLS 1.2+** on C1/C2/C5; document cipher policy (K6.5).

### §6.8 IEC 62351-4 — MMS (pp. 28)

**Scope:** MMS (ISO 9506), TASE.2, **61850** application-layer security; uses **62351-3** for transport.

**Without encryption:** unauthorized access only.

**With 62351-3 + -4:** access control, tamper/theft protection via message auth + encryption.

**Attack countermeasures (62351-4 specific):**

| Attack | Countermeasure |
|--------|----------------|
| MITM | **MAC** mechanism |
| Tamper / integrity | Auth algorithm |
| Replay | State machines in **62351-3 + -4** |

**62351-3 + -4 together:** authentication, confidentiality, integrity, non-repudiation through **application layer**. Secure and non-secure profiles may coexist (phased upgrade).

**CCLI:** MMS on **C1** — lab evidence target **62351-4** + underlying **62351-3 TLS**.

### §6.9 IEC 62351-5 — 60870-5 (pp. 28–29)

**Serial (-101/-102/-103):** low bit rate / compute-constrained — **simple auth only** (TLS too heavy).

**Networked (-104, DNP3/TCP):** use **62351-3 TLS** + **62351-5 app auth** (spoof, replay, modify, some DoS).

**62351-5 alone:** app-layer auth — **no encryption** (no eavesdrop/traffic-analysis/repudiation protection). Needed because site VPN/TLS may not cover: intra-site security, serial-over-radio, terminal-server forwarding, rogue apps/malware on host, **RBAC chain to remote site**.

**Optional add-on:** VPN/bump-in-the-wire for encryption at transport layer.

**CCLI:** **C2** 104 path — **62351-3 + 62351-5**; RS485 Modbus out of 62351 scope (document trust model).

### §6.10 IEC 62351-6 — 61850 profiles (pp. 29–31)

**MMS over TCP:** uses **62351-3 + -4**.

**Multicast GOOSE/SMV:** non-routable; **4 ms** performance budget — **encryption excluded**; **authentication via HMAC/digital signature** (~**20 bytes** extension appended to PDU — Fig 10).

#### Table 1 — Multicast 61850 protocol characteristics (p. 30)

| Characteristic | SMV | GOOSE | MMS |
|----------------|-----|-------|-----|
| PDU size | ~1 500 | ~1 500 | >30 000 |
| Performance | Stream | **4 ms** | No requirement |
| Type | Multicast | Multicast | Connection-oriented |

#### Table 2 — Security measures (p. 30)

| Measure | SMV | GOOSE | MMS |
|---------|-----|-------|-----|
| X.509 certificates | No | No | **Yes** |
| Encryption | Not necessary | Only if >4 ms | **Yes** |
| Tamper detection | Yes | Yes | Yes |

**GOOSE/SMV auth:** extension at end of message; non-secure client ignores secure message; **SCL extended** for certificate exchange; **no in-band key renegotiation** (would break real-time flow).

**CCLI:** TG-524 **MMS on C1** (62351-4); GOOSE/SMV on plant bus — **P1** unless DSO mandates.

### §6.11 IEC 62351-7 — NSM (pp. 31–33)

**Purpose:** Standardized **NSM data objects** (MIB-like) for power industry — naming from **61850**; mappable to SNMP, 61850, 60870-5/6, web services.

**Fig 11:** NSM object models parallel **CIM / 61850** power-system object models across information vs power infrastructure.

**Fig 12 — Security monitoring architecture:** NSM objects + **IDS** at control center, WAN boundary, substation (firewalls between zones).

**NSM examples (§6.11.3 a, pp. 33–34):** network equipment failures/failover, protocol version mismatch, malformed messages, clock sync, **resource-exhaustion DoS**, buffer overflow DoS, physical access disruption, **invalid network access**, **invalid application object access**, **coordinated multi-system attacks**, **message delivery statistics**, **audit logs**.

#### §6.11.3 b — Network control (p. 34)

Manual on/off and switching commands; parameter/sequence setup for automated actions; automated reconfiguration on equipment failure.

#### §6.11.3 c — IED monitoring (p. 34)

Stop/start counts; application/module status (running, suspended, errors); connection failure history; keep-alive/heartbeat status; failover mechanism status; data reporting health; **unauthorized access attempts**; anomalous data access patterns.

#### §6.11.3 d — IED control actions (p. 34)

Start/stop reporting; IED restart; kill/restart application; re-establish peer connection; remote shutdown; event log; password change; failover option change; audit logs.

**CCLI:** Map to **62443 CR 6.2** continuous monitoring + remote syslog + OpenWrt health endpoints; **62351-7** P1 for ops dashboard. TG-524 as **security server / substation master** role in Fig 12 architecture.

---

## CCLI conduit mapping (summary)

| Conduit | Protocol | 62351 stack | 62443 rows |
|---------|----------|-------------|------------|
| **C1** | 61850 MMS | **62351-3 + 62351-4** | CR 1.8, 1.9, 3.1(1), 4.1 |
| **C2** | 60870-5-104 | **62351-3 + 62351-5** | CR 3.1(1), 4.1 |
| **C5** | LTE/IP | **62351-3** TLS (+ VPN compensating) | CR 1.13, 4.1(1) |
| **C3** | Modbus | *(no 62351)* | Document trust / CRC |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| 62351-1 §6 / Fig 9 | **HAVE** — part map + Tables 1–2 + §6.11.3 NSM | — | — |
| 62351-1 §7 / bib | Not captured | Conclusions + refs | p. **35** optional |
| 62351-4 | **PLAN** — capture plan created | MMS/E2E normative | **P0** — send ToC/first pages |
| 62351-3/5 | MISSING | TCP + 104 security | **P0** |
| 62351-2 | MISSING | Glossary | Optional |
| K6.3 ceremony | **HAVE** | `ccli-k63-ceremony-sop` | CR 1.8 implement |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-351-001 | VPN-only on C5 without 62351-4 on C1 | False sense of security | §5.3.5 + §5.8 |
| R-351-002 | Full TLS on narrowband RTU paths | Latency / availability fail | Profile + phased deploy |
| R-351-003 | No cert revocation model for field | PKI breaks at SL-C 2 | §5.6.2 offline ceremony |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| Batch B §5 | Figs 1–8 captured | Threat→countermeasure traceable | Internal review |
| Batch C §6 | Fig 9 + Tables 1–2 mapped to C1/C2 | 62351-4/-3 normative | Before lab |
| 62443 CR 1.8 | PKI design doc | Maps to 62351 cert model | K6.5 gate |

---

## Source pages

### Batch A — Foundation (pp. 4, 6–7)

| Page | Content | Status |
|------|---------|--------|
| **4** | Foreword | PASS |
| **6** | §1.1 Scope; §1.2 Object; threat/end-to-end rationale | PASS |
| **7** | §1 tail; §2 normative refs; §3→62351-2; §4.1 start | PASS |

### Batch B — §5 Security framework (pp. 9–23)

| Page | Content | Status |
|------|---------|--------|
| **9** | §4.3 history; §5.1–5.2.1 threats | PASS |
| **10** | §5.2.2 inadvertent (safety, equipment) | PASS |
| **11** | §5.2.2.3–4 carelessness, disaster; §5.2.3 deliberate start | PASS |
| **12** | §5.2.3.6–8 virus, theft, terror; §5.3.1 requirements start | PASS |
| **13** | §5.3.2–5.3.4 threats, vulns, attacks | PASS |
| **14** | **Fig 1–2** attack/threat map; category stack | PASS |
| **15** | §5.3.5 VPN insufficient; §5.3.6 countermeasures intro | PASS |
| **16–17** | **Figs 3–6** confidentiality, integrity, availability, non-repudiation | PASS |
| **18** | **Fig 7** overall mesh; §5.3.7 decomposition start | PASS |
| **19** | §5.3.7 domains; §5.4 security policies | PASS |
| **20** | §5.5 risk assessment; §5.6.1 power ops constraints | PASS |
| **21** | §5.6.2 key mgmt; §5.6.3 NSM; §5.7 start | PASS |
| **22** | **Fig 8** 5-step cycle; assessment detail | PASS |
| **23** | Policy/deploy/train/audit; §5.8 auth > encryption | PASS |

### Batch C — §6 Series map (pp. 24–33)

| Page | Content | Status |
|------|---------|--------|
| **24** | §5.8 threat tail; §6.1–6.3 scope, auth, objectives | PASS |
| **25** | §6.4 measures; parts 1–7 list; PICS | PASS |
| **26** | **Fig 9** correlation; §6.5–6.7; TLS over IPSec | PASS |
| **27** | §6.7.2–6.7.3 62351-3 TLS params; VPN out of scope | PASS |
| **28** | **§6.8** 62351-4 MMS; **§6.9** 62351-5 start | PASS |
| **29** | §6.9 serial vs 104; **§6.10** 62351-6 start | PASS |
| **30** | **Tables 1–2** GOOSE/SMV/MMS; §6.10 GOOSE auth | PASS |
| **31** | **Fig 10** GOOSE/SMV HMAC; **§6.11** 62351-7 start | PASS |
| **32** | **Fig 11** NSM vs CIM/61850; §6.11.2 requirements | PASS |
| **33** | **Fig 12** security monitoring; NSM object examples (a) | PASS |
| **34** | §6.11.3 a tail (12–16); **b–d** network control + IED monitor/control | PASS |

### Optional

```
35   ← §7 Conclusions · Bibliography
```

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-08-11 | Batch A pp. 4, 6–7 + Batch B pp. 9–23 |
| **1.1** | 2026-08-11 | **Batch C** pp. 24–33: §6 series map · Fig 9–12 · Tables 1–2 |
| **1.2** | 2026-08-11 | **Batch D** p. 34: §6.11.3 NSM monitor/control tail |

---

## RAG cross-links

| source_id | Document |
|-----------|----------|
| `ccli-62351-1-extract` | This document |
| `ccli-62351-1-capture-plan` | ToC + batch order |
| `ccli-62443-zones-extract` | C1–C5 conduits |
| `ccli-62443-4-2-extract` | CR 1.8 PKI requirement |
| `ccli-62443-3-3-extract` | SR 1.8 / 4.1 system rows |

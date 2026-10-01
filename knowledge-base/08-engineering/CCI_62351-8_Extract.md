# IEC TS 62351-8 — CCLI Engineering Extract

**Document ID:** CCLI-SEC-62351-008  
**Revision:** 1.0  
**Date:** 2026-09-16  
**RAG source_id:** `ccli-62351-8-extract`  
**Normative basis:** IEC TS 62351-8:2011(E) — *Role-based access control*  
**Capture status:** **HAVE (P1 core)** — §5.2 + §12 from `62351-8.pdf` OCR; §1 front matter **MISSING**  
**Parents:** `ccli-62351-8-capture-plan` · `ccli-62351-4-extract` · `cei-0-16-allegato-t`  
**Programme link:** K6.5 · Annex T §T.3.3.4

---

## Summary

**62351-8** specifies **role-based access control (RBAC)** for power-system protocols: **subjects** receive **roles** via **access tokens**; **roles** map to **rights**; **rights** gate **ACSI services** (61850) or protocol equivalents. Configuration is **out-of-band** and stored in **SCL** with a **revision number**. Audit events go to log **`SECAUD`**.

**Annex T (CEI 0-16)** mandates **two additional private roles** beyond the seven IEC roles: **`DSO_OPERATOR` (-1)** and **`AGGREGATOR_OPERATOR` (-2)**.

**Corpus status:** **PART** — core RBAC model captured; cert profile XSD detail (§9.5) **MISSING**.

---

## Requirements

| ID | Requirement | Source | CCLI implication |
|----|-------------|--------|------------------|
| REQ-3518-ROLE-001 | Support **7 mandatory pre-defined roles** (Table 3) | §5.2.1.4 | VIEWER…RBACMNT |
| REQ-3518-ROLE-002 | **Private roles** `-32768..-1` by external agreement | Table 1 | Annex T **-1/-2** DSO/Aggregator |
| REQ-3518-RIGHT-001 | **11 mandatory rights** (Table 2) | §5.2.1.3 | VIEW, READ, DATASET, REPORTING, … |
| REQ-3518-61850-001 | Access if required **right ∈ session roles** | §5.2.2 | 61850-7-2 association model |
| REQ-3518-61850-002 | **VIEW** denied ⇒ LD omitted from `GetLogicalDeviceDirectory` | §5.2.2.1.2.2 | Hide unauthorized LDs |
| REQ-3518-LOG-001 | Log subject + roles to **`SECAUD`** on successful association | §5.2.2.3 | 62351-14 / CR 6.2 |
| REQ-3518-SCL-001 | **Role-to-right assignment** defined in **SCL** object model | §5.2.2.4 | Commissioning artefact |
| REQ-3518-SCL-002 | Assignment includes **revision number** | §5.2.2.4 | Token must match config rev |
| REQ-3518-TOKEN-001 | **Profile A:** X.509 ID cert + PKI | §12.5 | TLS + MMS (62351-4) |
| REQ-3518-TOKEN-002 | **Profile B:** X.509 attribute certificate (PMI) | §12.5 | Short-lived privilege boost |
| REQ-3518-TOKEN-003 | **Profile C:** software token (Kerberos-like) | §12.5 | Optional |
| REQ-3518-TOKEN-004 | Supported tokens: ID cert, attr cert, software token | §12.2 | Annex T: A or B **mandatory** |
| REQ-3518-AOR-001 | Token **extension** carries **AoR** (area/segment) | §12.3 | Segregate RBAC vs legacy zones |
| REQ-3518-ANNEXT-001 | **DSO_OPERATOR** / **AGGREGATOR_OPERATOR** roles | Annex T T.3.3.4 | Table 98/99 object scope |

---

## Architecture

### Functional

```
Identity Provider (LDAP repo)
        │ access token (roles)
        ▼
Subject ──Associate──► ACSI Server (61850)
                           │
                           ├─ check role ∩ required right
                           ├─ SECAUD log
                           └─ SCL-stored role↔right map (revision)
```

### Security

| Mechanism | Purpose |
|-----------|---------|
| Access token | Binds subject → roles |
| Revision number | Prevents stale privilege after reconfig |
| AoR attribute | Limits token to network segment |
| CRL / short TTL | Revocation (Profile A/B/C variants) |

---

## Interface matrix

| Interface | Source | Destination | Protocol |
|-----------|--------|-------------|----------|
| Token issue | Identity provider | Subject / Server | LDAP / PKI |
| Association | Client LN | Server LN | 61850 ACSI (7-2) |
| RBAC config | Engineering | IED | **SCL** |
| Audit | Server | SIEM | **SECAUD** log → 62351-14 |

---

## 1. Pre-defined roles (Table 3)

| Role | ID | Summary |
|------|-----|---------|
| **VIEWER** | 0 | Discover object types in LD |
| **OPERATOR** | 1 | View values + **control** |
| **ENGINEER** | 2 | View + DataSets + files + configure |
| **INSTALLER** | 3 | View + write files + configure |
| **SECADM** | 4 | Subject↔role + role↔right + certs |
| **SECAUD** | 5 | View audit logs |
| **RBACMNT** | 6 | Change role↔right (sub-role of SECADM) |

**Table 1** maps each role to granted **rights** (packed list per Table 2).

**Private range:** `<-32768 .. -1>` — **Annex T** uses **-1** (DSO), **-2** (Aggregator).

---

## 2. Mandatory rights (Table 2)

| Right | Description |
|-------|-------------|
| **View** | Discover objects in LD |
| **Read** | Read values + types |
| **DataSet** | Manage permanent/non-permanent DataSets |
| **Reporting** | Buffered and unbuffered reports |
| **FileRead / FileWrite / FileMgt** | File access hierarchy |
| **Control** | Operate controllable objects |
| **Config** | Local/remote server configuration |
| **SettingGroup** | Remote setting groups |
| **Security** | Security functions at SAP/LD |

Conditional: e.g. no **Control** if no controllable objects.

---

## 3. IEC 61850 access model (§5.2.2)

- Access control on **virtual access views / services** (ACSI).
- On **Associate**, session bound to **subject roles**.
- Access granted if **required right** of the data object matches **any role** in session.
- **ALLOW** / **DENY** rights apply to **Associate / Release / Abort** (Tables 5–6).

### VIEW right — ACSI services (Table 7)

| Service | Comment |
|---------|---------|
| **GetLogicalNodeDirectory** | LN class object references |
| **GetDataDirectory** | DO listing |
| **GetDataDefinition** | DO/DA definition |
| **GetDataSetDirectory** | DataSet members |

---

## 4. Configuration & audit (§5.2.2.3–4)

- **SECAUD** log: subject + allowed roles after successful auth (SCSM defines encoding — 61850-7-2).
- Role↔right changes logged for **chain of custody**.
- Configuration: **out-of-band** (manual/tooling).
- **Role-to-right assignment in SCL** with **revision number** checked against token.

---

## 5. Access token profiles (§12)

| Profile | Mechanism | Notes |
|---------|-----------|-------|
| **A** | X.509 **ID certificate** | PKI; CRL recommended |
| **B** | X.509 **attribute certificate** | PMI + PKI; RBAC-only use |
| **C** | **Software token** | Kerberos-like; PKI may protect repo |
| **D** | RADIUS | Optional (Annex T) |

**§12.2 supported tokens:** ID cert (§9.5.1), attribute cert (§9.5.2), software token (§9.5.3).

**Backward compatibility:** legacy IEDs without RBAC — **network segmentation** (AoR) separates zones.

---

## Crosswalk — CEI Annex T

| Annex T | 62351-8 |
|---------|---------|
| 7 standard + 2 custom roles | Table 3 + private IDs **-1/-2** |
| Profile A or B mandatory | §12.2 / §12.5 |
| Attribute cert **< 24 h** | Short TTL vs replay (Annex T + §12.3) |
| DSO vs Aggregator DO scope | Tables 98–99 — **Control+Config** on reserved DOs |
| ACSI services list T.3.3.2 | Subset of rights (READ, REPORTING, …) |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| §1 Scope / §2 refs | **MISSING** | Edition baseline | Re-scan PDF front |
| §9.5 cert extensions (`IECUserRoles`) | **MISSING** | Wire format | Cross-ref 62351-9 |
| §6–7 LDAP flows | **PART** | Full PUSH/PULL | P2 |
| Table 1 full matrix | **PART** (OCR) | Bit-exact role×right | Re-scan p.20 |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-3518-001 | Token rev ≠ SCL rev | Privilege mismatch / lockout | Atomic commission procedure |
| R-3518-002 | Private role ID collision | Interop fail | Use Annex T **-1/-2** only |
| R-3518-003 | RBAC on legacy segment | Bypass | AoR + VLAN segregation §12.3 |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-3518-ANNEXT-001 | Map Annex T role table | -1/-2 privileges documented |
| REQ-3518-61850-002 | Simulator without VIEW | LD hidden from directory |
| REQ-3518-LOG-001 | Force successful Associate | SECAUD entry created |
| Phase 3 | libiec61850 + test token | Allowed/denied services match role |

---

## RAG routing

```
ccli-62351-8-extract, ccli-62351-4-extract, cei-0-16-allegato-t, ccli-62351-9-extract
```

# Lab requirement ↔ cybersecurity test matrix (62351 / Annex T transport)

**Document ID:** CCLI-LAB-REQ-TEST-MTX-CYB-001  
**Revision:** 1.4<br>
**Date:** 2026-10-14  
**RAG source_id:** `ccli-lab-requirement-test-matrix-cyber`  
**Purpose:** Checklist for **62351-3 transport (D7)**, **62351-4 / 100-4 MMS security**, **62351-8 RBAC**, and **product cyber profile** — separate from **61850-10 functional** tests in [LAB_REQUIREMENT_TEST_MATRIX.md](LAB_REQUIREMENT_TEST_MATRIX.md).  
**Field pack:** `scripts/stage-tsp-field-pack.ps1` → `01b_CHECKLIST_CYBER_REQUIREMENT_TEST_MATRIX.md`

**Programme □ checklist:** [LAB_UNIFIED_CERT_TEST_MATRIX.md](LAB_UNIFIED_CERT_TEST_MATRIX.md) rev **1.4** — **U-CYB-1003-*** / **U-CYB-1004-*** / **U-CYB-PROD-01** map 62351-**100-3** Tables 1–3 and **100-4** Table 3 mandatory themes; tick unified matrix for traceability, use **this** file for step detail.

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
| □ | Native §13 SecPDU | §13 + CEI 57-142 §5.1 | **GAP** — mandatory CEI product capability; not in current source | 100-4 if quoted | See §4A gap register |
| □ | Lab executes 100-4:2023 | Q 7.3 | Confirm yes/no | **N/A** until yes | meeting minutes |

Planning extract: `knowledge-base/08-engineering/CCI_62351-100-4_Extract.md` · OCR: `ccli-62351-100-4-ocr-corpus`.

---

## §4A Mandatory CEI E2E gap — bundled implementation scope

This is one product gap with several mandatory technical parts.  It is not a list of separate optional laboratory deliverables.  CEI 57-142:2026-07 §5.1 requires the IEC 62351-4 E2E application profile in addition to the TLS transport profile.

**Source audit basis (2026-10-14):** the current CCLI MMS path uses upstream `libiec61850` TLS/ACSE handling plus the project AARE responder-authentication hook.  It contains no native `e2eMmsAC`, `SecPDU`, `HandshakeReq`/`HandshakeAcc`, `ClearTransfer`, or E2E rekey state machine.  The separate Cert-B files and AARE signature therefore do not prove native E2E.

| Gap ID | Mandatory E2E part | CEI / IEC basis | Current repository status | Required closure / mandatory test evidence |
|--------|--------------------|-----------------|---------------------------|---------------------------------------------|
| **CYB-GAP-E2E** | **Native E2E association and data-transfer package** | CEI 57-142 §5.1 · IEC 62351-4 §§12–15 · IEC TS 62351-100-4 mandatory cases | **GAP** — native E2E is not implemented in the MMS server | Implement and test all parts below as one E2E work package |
| ↳ E2E-01 | Association handshake and SecPDU mapping | IEC 62351-4 §§12, 15 | No `HandshakeReq`/`HandshakeAcc`/`HandshakeRej`, SecPDU signature verification, or MMS/OSI SecPDU wrapping | E2E association succeeds with the required Cert-B/AP/AE binding and signed SecPDUs |
| ↳ E2E-02 | Clear data transfer | IEC 62351-4 §13.2 · IEC TS 62351-100-4 Table 41 | No `ClearTransfer` encoder/decoder or ICV validation | Mandatory ClearTransfer request/response passes with integrity, time, sequence, and replay checks |
| ↳ E2E-03 | E2E integrity and failure handling | IEC 62351-4 §§13–15 | No E2E ICV, timestamp-window, sequence/replay, or `DtSecAbort` diagnostic handling | Negative cases reject bad ICV, stale/replayed/invalid-sequence data and return the specified diagnostics |
| ↳ E2E-04 | Client-initiated rekey | IEC TS 62351-100-4 Table 24 | No CCI server request/response key-change state machine | DSO/client rekey request causes successful E2E key update |
| ↳ E2E-05 | Server-initiated rekey | IEC TS 62351-100-4 Table 25 | No CCI server-side rekey initiation or transition handling | CCI rekey request is accepted by the client and the new E2E keys are used |
| ↳ E2E-06 | Cert-B and cryptographic state | IEC 62351-4 Annex G · §13 | Cert-B files/AARE signing exist, but Cert B is not used by native E2E; no E2E DH/key state | Cert-B fields, DH/key derivation, current/changed-key state, and the mandatory wire evidence pass |

**Out of this mandatory E2E gap package:** `EncrTransfer`/confidentiality selection, OCSP, EST/SCEP automation, hardware-key certification, 62443 certification, and accredited-report administration remain separate scope or evidence items.  They are not added as mandatory E2E test gaps here.

**Exit rule:** do not mark `CYB-GAP-E2E` PASS from TLS, ACSE AARE signing, dual certificate files, or a successful normal MMS read.  The mandatory E2E package requires native SecPDU wire evidence, including ClearTransfer and both rekey directions.

---

## §4B Mandatory CEI PKI lifecycle gap — bundled implementation scope

This is one product gap with several mandatory technical parts.  EJBCA issuance, revocation, and CRL export demonstrate the lab CA workflow; they do not by themselves prove that the CCI implements the required device-side certificate and key lifecycle.

**Source basis:** Annex T `T.3.3.4.9`, PKI — Certificate & Key Lifecycle (§8.5 in the project extract).

| Gap ID | Mandatory PKI part | CEI / IEC basis | Current repository status | Required closure / mandatory evidence |
|--------|--------------------|-----------------|---------------------------|----------------------------------------|
| **CYB-GAP-PKI** | **Certificate and key lifecycle package** | Annex T `T.3.3.4.9` | **GAP** — current CCI uses provisioned PEM files and a static CRL; EJBCA is lab-side | Implement and test all parts below as one PKI work package |
| ↳ PKI-01 | New key-pair generation/reception triggers | New pair at startup when absent; controllership/ownership change; authorised command; suspected private-key compromise | No complete device-side lifecycle workflow found | Securely generate/receive and install a new pair for each trigger; retain audit evidence and protect the private key |
| ↳ PKI-02 | Certificate enrolment protocol | At least one of SCEP (RFC 8894) or EST (RFC 7030 over TLS 1.2+) | EJBCA EST server configuration exists, but the CCI EST/SCEP client is deferred; current flow is manual PEM export | Implement at least one supported device-side protocol and test successful, failed, and authenticated enrolment |
| ↳ PKI-03 | Certificate validity checking | Both CRL and OCSP (RFC 6960); OCSP nonce mandatory | CRL loading/checking is implemented; no OCSP client/nonce validation found | Validate CRL and signed OCSP responses, including nonce handling, revoked/unknown/error cases, and cache policy |
| ↳ PKI-04 | Minimum trust anchors | DSO CA, Administrative Domain CA, Manufacturer CA | Lab setup currently uses one EJBCA lab CA; three-anchor production configuration is not proven | Configure, protect, and verify all minimum anchors; test the correct issuer/domain decisions |
| ↳ PKI-05 | Revocation grounds and lifecycle audit | Compromise, changed affiliation, replaced certificate, decommissioning, withdrawn role, or Attribute Authority compromise | EJBCA can revoke a lab certificate, but device-side trigger handling and complete lifecycle logging are not proven | Execute representative revocations, reject affected certificates, replace keys/certs where required, and log each lifecycle event |

**Exit rule:** `CYB-GAP-PKI` is not PASS from EJBCA certificate issuance, a manually copied PEM, or a CRL-only negative test.  The mandatory package requires device-side lifecycle evidence, at least one enrolment protocol, both CRL and OCSP validation, minimum trust anchors, and revocation handling.

---

## §4C Mandatory CEI RBAC gap — bundled 62351-8 profile scope

This is one RBAC gap with several mandatory technical parts.  The current implementation proves only the `VIEWER` and `DSO_OPERATOR` paths.  The full IEC 62351-8 / Annex T profile also requires the seven predefined IEC roles and the Annex T private roles.  The final lab claim must be constrained by the agreed PICS/PIXIT if the DSO explicitly limits the role set.

**Source basis:** IEC 62351-8 §5.2.1.3 Table 2, §5.2.1.4 Table 3, §5.2.2, §5.2.2.3–4, §12.5; Annex T `T.3.3.4` and Tables 97–102.

| Gap ID | Mandatory RBAC part | IEC / CEI basis | Current repository status | Required closure / mandatory evidence |
|--------|---------------------|-----------------|---------------------------|-----------------------------------------|
| **CYB-GAP-RBAC** | **Complete role-based access-control package** | IEC 62351-8 + Annex T | **GAP** — code has `VIEWER` and `DSO_OPERATOR` only | Implement and test the claimed role/right profile as one RBAC work package |
| ↳ RBAC-01 | Seven predefined IEC roles | 62351-8 §5.2.1.4 Table 3: `VIEWER`, `OPERATOR`, `ENGINEER`, `INSTALLER`, `SECADM`, `SECAUD`, `RBACMNT` | Only `VIEWER` is present as a standard role; the other six are absent | Define role identities/tokens and test association and service permissions for every claimed role |
| ↳ RBAC-02 | Annex T private roles | Annex T / 62351-8 private-role range: `DSO_OPERATOR` (-1), `AGGREGATOR_OPERATOR` (-2) | `DSO_OPERATOR` exists; `AGGREGATOR_OPERATOR` is absent | Add and test DSO/Aggregator role separation and reserved-object scope |
| ↳ RBAC-03 | Role-to-right model | 62351-8 §5.2.1.3 Table 2: View, Read, DataSet, Reporting, FileRead, FileWrite, FileMgt, Control, Config, SettingGroup, Security | Current control gate checks mainly whether the token is DSO; no complete 11-right matrix | Define the role-to-right matrix and test positive/negative service access |
| ↳ RBAC-04 | ACSI/service enforcement and VIEW filtering | 62351-8 §5.2.2 and §5.2.2.1.2.2 | Viewer read/operate behavior exists; full service filtering and unauthorized logical-device hiding are not proven | Verify rights at association and service level; hide unauthorized logical devices from directory discovery |
| ↳ RBAC-05 | Security audit role/events | 62351-8 §5.2.2.3 (`SECAUD`) | Basic authentication/RBAC audit events exist; subject-plus-complete-role association evidence is not proven | Log subject, assigned roles, authorization result, and RBAC failures in the required audit path |
| ↳ RBAC-06 | SCL role-right configuration and revision | 62351-8 §5.2.2.4 | No complete SCL role-to-right mapping/revision check found | Provide SCL configuration, revision handling, and stale-revision rejection evidence |
| ↳ RBAC-07 | Token profile and area-of-responsibility binding | 62351-8 §12.2–§12.5; Annex T profile claim | Certificate mapping exists, but full token profile/AoR behavior is not proven | Document the selected token profile, bind roles to the token, and test scope/AoR restrictions |

**Exit rule:** `CYB-GAP-RBAC` is not PASS from one Viewer read test, one Viewer operate-denied test, or a DSO certificate allow-list.  The mandatory claimed profile requires role definitions, rights mapping, ACSI enforcement, audit evidence, and SCL/revision evidence.

---

## §4D Mandatory CEI security data-logger gap — bundled logging scope

This is one data-logger gap with several mandatory technical parts.  The current implementation has a 2048-event circular store and local event dump, but the complete Annex T logger profile is not closed.

**Source basis:** Annex T `T.3.3.4.6`, §8.7 in the project extract; CEI O.14 logger requirements.

| Gap ID | Mandatory logger part | Annex T basis | Current repository status | Required closure / mandatory evidence |
|--------|----------------------|---------------|---------------------------|----------------------------------------|
| **CYB-GAP-LOG** | **Protected security data-logger package** | `T.3.3.4.6` / IEEE 1686:2013 | **PARTIAL** — 2048-event FIFO, JSONL persistence, and event dump exist | Complete and test all parts below as one logger work package |
| ↳ LOG-01 | FIFO storage and protection | Sequential circular FIFO; not deletable/modifiable; minimum 2048 events | 2048-event ring and wrap test exist; `--event-clear` is rejected and file mode is `0640`, but privileged tamper resistance and retention evidence are not proven | Demonstrate 2048-event capacity, reboot/update persistence, ordinary-user denial, and protected storage behavior |
| ↳ LOG-02 | Secure remote transmission | Syslog RFC 5424/5425 over TLS, or SNMPv3 TSM over TLS | **GAP** — RFC 5424 formatting is sent through UDP `SOCK_DGRAM` to port 514; no TLS syslog or SNMP/TLS transport found | Implement and test RFC 5425 syslog-over-TLS or the accepted SNMPv3/TLS alternative, including peer authentication and failure handling |
| ↳ LOG-03 | Severity categories | `alarm`, `error`, `notice`, `warning` | **PARTIAL** — current formatter mainly distinguishes authentication warnings from notice; full severity mapping is not proven | Map every required event to the correct severity and produce evidence for authorised, abnormal, and unauthorised actions |
| ↳ LOG-04 | Security-event coverage | Authentication, user/role/permission changes, signature failures, update outcomes, TLS/certificate validation, E2E failures, certificate lifecycle, RBAC failures | **PARTIAL** — several authentication, RBAC, firmware-boot, interface, and protocol events exist; full Annex T mnemonic/category coverage is not closed | Generate and verify events for login/logout, certificate failures, CRL/OCSP status, E2E SecPDU failures, enrolment/renewal/revocation/CertAVL, RBAC, and update outcomes |
| ↳ LOG-05 | Remote/event evidence | Events must be readable/exportable without compromising the protected local record | Local `--event-dump` evidence exists; secure remote receipt, loss/retry behavior, and tamper/injection evidence are open | Capture receiver-side records, test transport failure/recovery, and prove local and remote event correlation |

**Exit rule:** `CYB-GAP-LOG` is not PASS from the 2048-event wrap test or local `--event-dump` alone.  The mandatory package requires protected FIFO behavior, secure remote transmission, correct severity classification, and the required security-event coverage.

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

### Unified matrix IDs (rev 1.4)

| This file | Unified U-ID |
|-----------|--------------|
| §2 T1-01 · REQ-1003-APP-001 | **U-T1-01**, **U-CYB-1003-02**, **U-CYB-1004-08** |
| §2 T3-01 | **U-T3-01**, **U-CYB-1003-07** |
| §2 T3-05 · A-profile | **U-T3-05**, **U-CYB-1004-01** |
| §2 dual cert | **U-D7-02**, **U-CYB-1004-02…04** |
| §2 T3-02/03/04 | **U-T3-02**, **U-T3-03**, **U-T3-04** |
| §3 REQ-D7-ED-001 · ciphers | **U-CYB-1003-01**, **U-CYB-1003-04** |
| §3 REQ-D7-REN / OCSP | **U-CYB-1003-03**, **U-CYB-1003-06** |
| §3 REQ-1003-ROLE-001 | **U-CYB-1003-08** |
| §3 REQ-D7-1003-001 | **U-D7-03**, **U-ACC-02**, **U-CYB-1003-05** |
| §4 Track C rows | **U-CYB-1004-01…08**, **U-DOC-09** |
| §5 REQ-PROD-SW-002/006/007 | **U-D7-01**, **U-CYB-PROD-01** |
| §6 D7 PID | **U-DOC-06** |

---

## RAG ingest

```powershell
powershell -File scripts\ingest-product-deploy-rag.ps1
# or full refresh:
powershell -File scripts\ingest-next-phases.ps1 -SkipDocker
```

**Keywords:** `cyber`, `62351-3`, `62351-4`, `62351-100-3`, `62351-100-4`, `D7`, `P3-06`, `P3-07`, `P3-08`, `P3-09`, `P7-07`, `3782`, `Tier 3`, `ccli-lab-requirement-test-matrix-cyber`

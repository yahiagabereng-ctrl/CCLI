# Native MMS E2E gap closure plan

**Superseded preparation outline:** use the [detailed implementation plan](e2e/IMPLEMENTATION_PLAN.md), [requirement/test map](e2e/REQUIREMENTS.md) and [source issue register](e2e/SOURCE_ISSUES.md) prepared from the user's supplied PDF on 2026-10-21. The outline below is retained as the earlier discussion record; the detailed map corrects its broad family references, source availability and role/condition assumptions.

Prepared: 2026-10-21 (Europe/Rome). Status: preparation and proposed gates; no implementation or new bench tests performed.

## Target

Implement and demonstrate IEC 62351-4 native end-to-end security for the CCI IEC 61850-8-1 MMS server on Eth_A, together with its TLS transport and existing role enforcement. Freeze the applicable CEI 0-16 edition and amendments with the certification lab before the formal claim.

CEI's Annex T requires support for the E2E application profile and TLS transport. Signed compatibility ACSE authentication is useful existing functionality but does not implement native E2E data protection. IEC TS 62351-100-4:2023 explicitly tests E2E and excludes the A-security profile; transport tests under 100-3 precede E2E tests.

Initial proposed transfer mode: native ClearTransfer with application integrity over TLS. Application encryption (EncrTransfer) is conditional/optional in the inspected test scope; decide it from the applicable Annex T profile, peer requirements and final PICS. ClearTransfer still carries application integrity protection. It must not mean ordinary unprotected MMS within TLS.

Primary external references:

- CEI Annex T English working translation, based on V2:2023-05: https://static.ceinorme.it/strumenti-online/doc/EstrattoAllegatoT.pdf
- CEI Italian consolidated text: https://static.ceinorme.it/strumenti-online/doc/0-16consolidata.pdf
- IEC 62351-4:2018+AMD1:2020: https://webstore.iec.ch/en/publication/67350
- IEC TS 62351-100-4:2023: https://webstore.iec.ch/en/publication/63323

The Italian applicable standard and agreed amendments govern the claim. Recheck editions at profile freeze. Do not use OCR summaries as the sole authority for encoding, algorithms or expected diagnostics.

## Existing assets and evidence

| Asset | Evidence and practical limit |
|---|---|
| Mutual TLS on MMS port 3782 | Existing source and captured sessions; a working session is not a complete 100-3 result. |
| Signed ACSE client/server authentication | Certificate proof, freshness/replay checks and AARE signing exist. This is compatibility functionality, not native SecPDU handling. |
| Separate TLS/application credentials and identity binding | Current configuration and dual-certificate fixtures exist. Audit Annex G profiles and actual key separation before reuse for native E2E. |
| Commissioned RBAC and security logging | Enrollment report records all 19 identities connecting, 143 live checks and 298 isolated checks. Preserve policy and authenticated identity semantics through the new E2E path. |
| MMS build integration | Vendored libiec61850 declares version 1.6.0. CMake generates ACSE/session/presentation overlays; native E2E requires additional stack-level integration. |
| Cross-build experience | Enrollment report records an ARM64 build with the existing SDK. Reproduce the exact SDK/sysroot/dependency versions; older SDK notes contain stale paths and missing-vendor-SDK statements. |
| Normative extracts and test-family audit | 62351-4 extract and 19 October 100-4 audit provide a starting requirements map. Confirm full source documents and amendment-matched machine-readable components. |
| Time synchronization | Later GNSS retry report records GNSS selected by chrony, correcting the earlier skew. Cold-boot readiness and sustained +/-100 ms UTC accuracy remain unproven. |

These are historical recorded results, not a fresh statement of the connected device's current state. The failed signed DSO association in the earlier 19 October audit was followed by successful dual-certificate enrollment checks; do not carry the earlier failure forward as an unresolved confirmed defect.

## Missing implementation and proof

| Workstream | Missing work | Required proof |
|---|---|---|
| Native wire format | Amendment-matched ASN.1 EnvPDU/SecPDU and ClearToken encoders/decoders; native application context and OSI mapping | Independent peer decodes and accepts the native handshake and protected transfer. |
| Association | Handshake request/accept/reject state machine, signed content, certificate and AP/AE identity binding, time and association-reference checks | Positive exchange and each applicable malformed-handshake diagnostic. |
| Protected MMS transfer | ClearTransfer integrity verification/creation in both directions; reports and controls included | Tampering/replay rejected before application dispatch; reads, reports and authorized controls work. |
| Session state | Directional keys, sequence/time handling, secure RNG, concurrent association isolation, bounded buffers and cleanup | Boundary/fault tests, restart/reconnect tests, memory/resource checks. |
| Key management | Specified DH/ECDH and HKDF behavior; client- and server-initiated application rekey and transition handling | Continued traffic across both rekey directions with correct old/new key acceptance rules. |
| Termination/errors | Native release/abort as required by the declared profile; standard diagnostics and security events | Exact expected SecPDU responses and logs, including malformed input paths. |
| PKI integration | Native application certificate chain, usage, identity binding, validity/revocation, provisioning/renewal and key handling | Valid, expired, revoked, wrong-identity and wrong-usage cases; independent TLS/application credentials. |
| Product policy | Explicit native/compatibility configuration and authenticated RBAC identity selection | Required native E2E cannot silently fall back to compatibility/TLS-only access. |
| Test equipment | Native E2E peer and fault injection support | Independent interoperability plus systematic negative cases. Existing ACSE probes alone cannot supply this. |
| Compliance package | Dedicated 62351-4 PICS/PIXIT, algorithms, mandatory/conditional test matrix, traceable evidence | Per-case status, firmware/tool hashes, traces, logs, timing and independent lab assessment. |

## Inputs needed

1. **Product owner:** target certification date, applicable CEI edition/amendments, intended DSO peer/profile, required transfer modes, and whether commercial stack licensing is an option.
2. **Test/lab owner:** installed TSP version and licensed capabilities, confirmation of native e2eMmsAC/EnvPDU/SecPDU and 100-4 support, and access to an independent E2E client. Successful existing ACSE Connect does not establish these capabilities.
3. **Standards access:** authoritative IEC 62351-4 plus applicable amendments, 62351-100-4, and matching ASN.1/code components. Local extracts/PDF captures exist; authoritative completeness and code-component availability still need checking.
4. **Bench owner:** available Tespro device, Ethernet and console access, stable power, time reference and recoverable known firmware/configuration. Resolve reproducible boot issues before long device campaigns. A Modbus/inverter simulator is needed later for physical control proof, not to begin E2E codec work.
5. **PKI owner:** CA/trust policy, application AP-title/AE-qualifier assignments for both peers, certificate profiles, renewal/revocation endpoints and test certificate issuance. Use local issuance/signing workflows; no private keys or passphrases need to be pasted into chat.
6. **Build owner:** reproducible host and ARM64 builds with exact SDK, sysroot, crypto library, compiler and package inputs. Existing builds are a starting asset, not a request to obtain all tooling again.
7. **Engineering ownership:** protocol/security implementation, independent test ownership and specialist review. No reliable calendar estimate until stack feasibility and test-peer availability are established.

## Ordered gates

### 1. Freeze requirements and test scope

- Reconcile older documents that defer E2E with the intended Annex T product claim.
- Draft dedicated 62351-4 PICS/PIXIT: OSI MMS server, edition/amendments, algorithms, identities, transfer modes, time behavior, limits, rekey and diagnostics.
- Convert the 19 October family-level audit into the complete applicable case list using authoritative tables and their conditions.
- Confirm lab procedure and native peer capabilities. A lab's packaging of its assessment does not waive Annex T implementation requirements.

Exit: agreed profile, traceable case matrix and usable native test peer.

### 2. Select implementation route

- Assess an E2E-capable extension/stack against a dedicated implementation integrated with the current libiec61850.
- Require demonstrated native handshake, ClearTransfer and application rekey; check ARM64/musl integration, licensing, source access, maintenance, crypto backend, callbacks and model/report/control compatibility.
- Commercial E2E options exist, but actual product coverage, price and suitability require evaluation. Do not assume the installed open-source stack exposes native E2E via a configuration flag.
- For an internal implementation, use matching standard code components and a maintained crypto library. Put protocol state/encoding in a dedicated module rather than expanding compatibility timestamp-signature handlers into a substitute E2E engine.

Exit: reproducible host prototype exchanges a valid native handshake and protected read with an independent peer; route and integration cost are understood.

### 3. Complete the host implementation

- Implement full association, transfer, identity validation, sequence/time/replay rules, errors, release/abort, both rekey directions and key cleanup.
- Connect the authenticated application identity to existing RBAC and audit events.
- Cover MMS responses, unsolicited reports and control traffic; enforce buffer/resource limits and isolation between sessions.
- Build codec/crypto vectors, malformed-input tests, fault injection and fuzzing. Include key transition, reconnect, process restart, concurrent sessions and disconnect during rekey.

Exit: positive and applicable negative host cases pass, with protocol/security review and independent peer results.

### 4. Integrate and qualify on Tespro

- Cross-build with recorded dependencies; verify crypto features, RNG, timing, memory and throughput on target.
- Verify time synchronization and commissioning credentials. Audit separate application keys/certificates and their runtime key interface; TPM-backed signing, if required by product policy, needs explicit integration and verification.
- Repeat existing role boundaries, MMS service/report/control behavior and TLS checks through the native path.
- Run continuous protected traffic, both rekey directions and recovery tests. Use the exact standard triggers, including the audited 15-minute rekey cases where applicable.
- Package any APK/USB release only with `apps/firmware-updater`, starting with its `docs/SIGNING_AND_USB_UPDATES.md`; follow canonical local signing and verification. A requested USB installation test must use USB.

Exit: target functional/security/performance results match the declared profile and the release is reproducible and recoverable.

### 5. Execute conformance and close the evidence

- Complete applicable 100-3 transport testing before 100-4 E2E testing.
- Execute 100-4 server association/transfer/rekey families: Tables 15/17/18/20/24/25 and all applicable Table 50/51 resilience cases from the audit. Include remaining applicable baseline/crypto declarations and conditional cases; this list is not a substitute for the full matrix.
- Add optional encryption families only if the frozen profile declares/requires them.
- Record expected and actual diagnostic codes, decoded exchanges, logs, firmware/source hashes, tool versions, fixtures and reproducible instructions. Protect keys and session secrets when storing/sharing captures.
- Have the lab review the agreed scope and evidence; update PICS/PIXIT, roadmap and release claims to match demonstrated capability.

Exit: every applicable requirement maps to implementation and passing evidence; any conditional exclusion is justified; formal claims have the agreed independent assessment.

## First implementation milestone

Native handshake followed by one integrity-protected MMS read and response over existing TLS, accepted by an independent E2E client, with a deliberately tampered transfer rejected using the specified diagnostic. This is the first technical milestone, not full E2E closure. Rekey, reports/controls, PKI negatives, full resilience, target qualification and formal evidence remain required.

## Repository evidence reviewed

- `lab/evidence/iec62351-100-4-2026-10-19/REPORT.md`
- `lab/evidence/iec62351-100-4-2026-10-19/mandatory-gaps.csv`
- `lab/evidence/mms-tls-enrollment-2026-10-19/REPORT.md`
- `lab/evidence/gnss-startup-fix-2026-10-19/REPORT.md`, `RETRY-FIX.md`, `BOOT-REREAD.md`
- `apps/ccli/adapters/iec61850_mms/mms_rbac_security.cpp`
- `apps/ccli/adapters/iec61850_mms/mms_acse_auth.hpp`
- `apps/ccli/adapters/iec61850_mms/mms_certificate_auth_bridge.h`
- `apps/ccli/cmake/mms_certificate_auth.cmake`
- `scripts/generate_mms_certificate_auth.py`
- `apps/ccli/third_party/libiec61850/CMakeLists.txt` and MMS sources
- `knowledge-base/08-engineering/CCI_62351-4_Extract.md`
- `knowledge-base/08-engineering/CCI_62351-100-4_Extract.md`
- `knowledge-base/08-engineering/CCI_K6.3_Key_Ceremony_SOP.md`

Source review and historical evidence establish preparation status. No device connection, firmware change, certificate issuance or new test execution was performed for this plan.

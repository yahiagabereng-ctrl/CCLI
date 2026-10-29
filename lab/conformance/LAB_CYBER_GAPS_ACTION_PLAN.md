# CCLI mandatory cybersecurity gaps and closure plan

**Document ID:** CCLI-LAB-CYBER-GAPS-001  
**Revision:** 1.0  
**Date:** 2026-10-14  
**Source matrix:** [LAB_REQUIREMENT_TEST_MATRIX_CYBER.md](LAB_REQUIREMENT_TEST_MATRIX_CYBER.md)  

## Purpose and scope

This document contains only the confirmed mandatory cybersecurity gaps currently tracked for the CCI. It explains what each gap means, what is already available, what must be implemented, how to test it before the laboratory visit, and what evidence closes the gap.

Passing TLS, CRL, local logging, or basic Viewer/DSO tests are mentioned only where needed to explain why they do not close a larger gap. Optional capabilities such as GOOSE security and E2E `EncrTransfer` are not included unless they become part of the agreed product claim.

## Gap summary

| Gap ID | Gap | Status | Main standards | Closure result |
|--------|-----|--------|----------------|----------------|
| `CYB-GAP-E2E` | Native MMS end-to-end security | **GAP** | CEI 57-142 §5.1; IEC 62351-4 §§12–15; IEC TS 62351-100-4 | Native SecPDU association, ClearTransfer, integrity/replay handling, and both rekey directions pass |
| `CYB-GAP-PKI` | Device-side certificate and key lifecycle | **GAP** | Annex T `T.3.3.4.9`; IEC 62351-9 | Device enrolment, key replacement, CRL and OCSP, required trust anchors, and revocation lifecycle pass |
| `CYB-GAP-RBAC` | Complete role/right access control | **GAP** | IEC 62351-8; Annex T Tables 97–102 | Claimed roles, rights, ACSI enforcement, audit, and SCL revision controls pass |
| `CYB-GAP-LOG` | Protected security logger and secure export | **PARTIAL** | Annex T `T.3.3.4.6`; CEI O.14 | Protected 2048-event FIFO, secure remote transmission, severity mapping, and event coverage pass |

## 1. `CYB-GAP-E2E` — native MMS end-to-end security

### What the gap means

TLS protects the TCP connection on port 3782. IEC 62351-4 E2E security additionally protects the MMS application messages inside that TLS connection. It uses a separate application certificate, SecPDUs, integrity checks, timestamps, sequence numbers, replay detection, and application-key refresh.

The current CCI has TLS, separate certificate files, and AARE responder authentication. Those functions do not implement native IEC 62351-4 E2E data transfer.

### Missing parts

| ID | Missing capability |
|----|--------------------|
| `E2E-01` | `e2eMmsAC` association and `HandshakeReq` / `HandshakeAcc` / `HandshakeRej` processing |
| `E2E-02` | `SecPDU` encoding, decoding, signing, and verification |
| `E2E-03` | Mandatory `ClearTransfer` request/response processing and ICV verification |
| `E2E-04` | Timestamp-window, sequence-number, replay, bad-ICV, and diagnostic handling |
| `E2E-05` | Client-initiated rekey handling |
| `E2E-06` | CCI/server-initiated rekey handling |
| `E2E-07` | Cert-B AP/AE binding, DH/HKDF key derivation, and current/changed-key state |

### What we should do

1. Select an IEC 62351-4 native-E2E implementation strategy: extend the current MMS stack or integrate a library that supports the required native profile.
2. Add the `e2eMmsAC` application context and the association handshake state machine.
3. Implement SecPDU wrapping/unwrapping and mandatory `ClearTransfer` integrity protection.
4. Use Cert B and its separate private key for E2E authentication and key agreement.
5. Add timestamp, sequence, replay, ICV, abort, and diagnostic validation.
6. Implement client-initiated and server-initiated E2E rekey transitions.
7. Add structured E2E security events to the protected logger.

### Required pre-lab tests and evidence

- Successful native E2E association with the correct Cert-B AP/AE identity.
- Successful MMS read and control carried in SecPDU `ClearTransfer` messages.
- Rejection of bad ICV, stale timestamp, replayed sequence, invalid sequence, wrong Cert B, and wrong AP/AE identity.
- Successful client-initiated and server-initiated rekey, followed by protected traffic using the changed keys.
- Packet capture showing the native application context, handshake, SecPDUs, ClearTransfer, and rekey exchange.
- DUT logs containing the expected success and failure diagnostics.

### Completion condition

Do not close this gap from a successful TLS connection, AARE signature, or normal MMS read. Close it only when the native SecPDU evidence, mandatory ClearTransfer behavior, negative security cases, and both rekey directions pass.

## 2. `CYB-GAP-PKI` — certificate and key lifecycle

### What the gap means

EJBCA currently acts as the laboratory CA. It can issue certificates, revoke them, and export a CRL. The CCI, however, currently consumes provisioned PEM files and a static CRL. A CA server does not by itself provide the required lifecycle inside the CCI.

### Missing parts

| ID | Missing or incomplete capability |
|----|----------------------------------|
| `PKI-01` | New key-pair generation/reception when no key exists, ownership changes, an authorised command is received, or compromise is suspected |
| `PKI-02` | Device-side SCEP RFC 8894 or EST RFC 7030 enrolment client |
| `PKI-03` | OCSP RFC 6960 checking with mandatory request nonce |
| `PKI-04` | Complete CRL update/distribution behavior and cache policy evidence |
| `PKI-05` | Minimum DSO, Administrative Domain, and Manufacturer CA trust anchors |
| `PKI-06` | Certificate replacement/revocation workflow for compromise, affiliation change, replacement, decommissioning, or withdrawn privileges |
| `PKI-07` | Complete lifecycle audit events for enrolment, renewal, replacement, and revocation |

### What we should do

1. Use EJBCA as the CA/RA backend, but implement at least one CCI enrolment client—preferably EST over TLS 1.2+.
2. Define where Cert A, Cert B, their private keys, and trust anchors are stored and protected.
3. Implement secure key generation/reception and atomic certificate/key replacement.
4. Implement CRL retrieval/update and OCSP request/response validation with a nonce.
5. Configure independent DSO, Administrative Domain, and Manufacturer trust anchors.
6. Define authorised commands and operational procedures for renewal, ownership change, compromise, and decommissioning.
7. Log every lifecycle action and failure in the security logger.

### Required pre-lab tests and evidence

- Successful authenticated EST or SCEP enrolment from the CCI to the PKI service.
- Failure with an untrusted PKI server, bad enrolment credentials, invalid response, or wrong profile.
- New key and certificate installation without leaving an inconsistent certificate/key pair.
- CRL rejection of a revoked certificate.
- OCSP `good`, `revoked`, `unknown`, bad-signature, stale-response, missing-nonce, and replayed-response cases.
- Successful validation against each required trust anchor and rejection under the wrong domain.
- Renewal/replacement and compromise workflow logs.

### Completion condition

Do not close this gap from EJBCA issuance, manually copied PEM files, or a CRL-only test. Close it only when the CCI performs the required device-side lifecycle, supports one enrolment protocol, validates both CRL and OCSP, holds the minimum trust anchors, and produces lifecycle audit evidence.

## 3. `CYB-GAP-RBAC` — complete role and right enforcement

### What the gap means

The current CCI maps certificates mainly to `VIEWER` and `DSO_OPERATOR`, and the control gate checks whether the session has the DSO token. IEC 62351-8 defines a larger role/right model, while Annex T adds the private DSO and Aggregator roles.

### Missing parts

| ID | Missing or incomplete capability |
|----|----------------------------------|
| `RBAC-01` | Standard roles `OPERATOR`, `ENGINEER`, `INSTALLER`, `SECADM`, `SECAUD`, and `RBACMNT`; only standard `VIEWER` is currently present |
| `RBAC-02` | Annex T private `AGGREGATOR_OPERATOR` (-2); `DSO_OPERATOR` (-1) exists |
| `RBAC-03` | Complete rights model: View, Read, DataSet, Reporting, FileRead, FileWrite, FileMgt, Control, Config, SettingGroup, Security |
| `RBAC-04` | Per-service ACSI enforcement and unauthorized logical-device hiding |
| `RBAC-05` | Subject and assigned-role logging in the `SECAUD` path |
| `RBAC-06` | SCL role-to-right assignment and revision validation |
| `RBAC-07` | Selected access-token profile and area-of-responsibility restrictions |

### What we should do

1. Agree the exact claimed role set and role/right matrix with the DSO and laboratory in the PICS/PIXIT.
2. Add the missing standard and private role identifiers required by that claim.
3. Replace the single DSO-token control check with a centralized role-to-right authorization engine.
4. Apply authorization to every claimed ACSI service, not only control operations.
5. Hide objects and logical devices when the session lacks the View right.
6. Store the role/right assignment and revision in the SCL configuration and reject stale revisions.
7. Log the authenticated subject, roles, access decision, and failures.

### Required pre-lab tests and evidence

- One certificate/token and positive/negative test set for every claimed role.
- A rights-versus-services test matrix covering browse, read, reports, data sets, files, control, configuration, settings, and security functions.
- Viewer read success and control denial.
- DSO and Aggregator separation on their reserved object scopes.
- Unauthorized logical devices absent from directory responses.
- Stale or mismatched role-map revision rejected.
- Successful association and denied-access events visible in the security logger.

### Completion condition

Do not close this gap from Viewer read/deny tests or a DSO certificate allow-list alone. Close it when every role and right in the agreed claim is implemented, service enforcement passes, SCL/revision handling works, and audit evidence is complete.

## 4. `CYB-GAP-LOG` — protected security data logger

### What the gap means

The current logger already has a 2048-event circular buffer, persistent JSONL storage, UTC timestamps, a read-only dump command, file mode `0640`, and a rejected user-clear command. The remaining gap is the complete Annex T logger profile, especially secure remote transmission and full security-event coverage.

### Missing parts

| ID | Missing or incomplete capability |
|----|----------------------------------|
| `LOG-01` | Evidence that ordinary users cannot delete/modify stored events and that logs survive reboot and firmware update |
| `LOG-02` | Syslog RFC 5425 over TLS or the accepted SNMPv3 TSM/TLS alternative; current forwarding is UDP |
| `LOG-03` | Complete `alarm`, `error`, `notice`, and `warning` severity mapping |
| `LOG-04` | Complete authentication, role, permission, signature, update, TLS/certificate, E2E, PKI lifecycle, and RBAC event coverage |
| `LOG-05` | Secure receiver evidence, transport-failure behavior, retry/loss policy, and local/remote correlation |

### What we should do

1. Keep the 2048-event FIFO and document its overwrite policy for the oldest event.
2. Prove that the operational user cannot clear, replace, or modify the event store.
3. Implement authenticated Syslog over TLS according to RFC 5425, or obtain approval for the secure SNMP alternative.
4. Add CA/certificate configuration for the log receiver and define behavior when the receiver or TLS validation fails.
5. Map every event to the required severity and Annex T mnemonic/category.
6. Add missing events from the E2E, PKI, RBAC, firmware, and certificate-validation work packages.
7. Test persistence and preservation across reboot and successful/failed firmware updates.

### Required pre-lab tests and evidence

- 2049-event wrap test proving exactly 2048 retained events and oldest-first replacement.
- User clear/overwrite attempt rejected; protected file ownership and permissions recorded.
- Reboot and firmware-update persistence captures.
- TLS-protected remote syslog capture with receiver identity validation.
- Rejection of an untrusted receiver and evidence of defined behavior during network/TLS failure.
- Representative `alarm`, `error`, `notice`, and `warning` events.
- Required security-event catalogue with one generated and verified example per event family.
- Correlation of local event ID/time with the securely received remote record.

### Completion condition

Do not close this gap from the wrap test or local event dump alone. Close it when storage protection and persistence are proven, remote transmission is authenticated and encrypted, severity mapping is complete, and all mandatory security-event families have verified evidence.

## Recommended closure order

1. Freeze the applicable CEI/IEC editions and the laboratory PICS/PIXIT claim.
2. Implement `CYB-GAP-E2E`, because it creates events, keys, certificates, and role interactions required by the other work packages.
3. Implement `CYB-GAP-PKI`, including Cert-B lifecycle support required by E2E.
4. Implement `CYB-GAP-RBAC` and finalize the exact service-permission matrix.
5. Complete `CYB-GAP-LOG` after the other features emit their final security events.
6. Run the combined negative-test campaign and freeze the evidence package for the laboratory.

## Final readiness rule

The product is not ready to claim the complete CEI cybersecurity profile while any of the four bundled gaps remains open. A successful TLS connection, normal MMS read, manual EJBCA provisioning, Viewer denial test, or 2048-event wrap test proves only a component of the corresponding requirement.

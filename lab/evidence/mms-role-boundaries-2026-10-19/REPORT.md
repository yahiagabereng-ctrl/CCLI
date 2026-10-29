# MMS role boundary check — 19 October 2026

**Later update:** the user authorized TLS enrollment. All 19 commissioned identities
now connect to Tespro; see the [enrollment verification](../mms-tls-enrollment-2026-10-19/REPORT.md).
The availability findings below describe the configuration before that change.

The tested role permissions behave as commissioned in isolation: **298 checks passed, 0 failed** across 19 identities, all nine roles, revision 0/1 certificates and the combined Viewer+Engineer identity. **Not all roles are usable on live Tespro:** only four tested identities pass its current TLS certificate allowlist. No live write, operate, report-enable or configuration change was sent.

The client used a persistent TLS/MMS connection for each identity. Full write/control checks ran against the MMS adapter on loopback port 3784 with synthetic measurements and no plant I/O. Each test certificate was explicitly pinned in its isolated configuration; the five signed commissioning files were unchanged and their SHA-256 hashes match Tespro's active package. Live tests used 192.168.0.1:3782 and only discovery/reads.

| Role | Measurement/state read | Static dataset | Reports | Configuration write | Operate |
|---|---|---|---|---|---|
| DSO_OPERATOR | Allowed in DSO scope | Allowed | Allowed; GI denied | Allowed | Allowed, termination and matching readback |
| AGGREGATOR_OPERATOR | Own scope allowed; DSO scope denied | DSO dataset denied | DSO report denied | DSO configuration denied | Own target not implemented |
| VIEWER | Allowed | Denied | Allowed | Denied | Denied |
| OPERATOR | Allowed | Denied | Allowed | Denied | Allowed, termination and matching readback |
| ENGINEER | Allowed | Allowed | Allowed | Allowed | Denied |
| INSTALLER | Allowed | Denied | Allowed | Allowed | Denied |
| SECADM | Allowed | Allowed | Denied | Allowed | Denied |
| SECAUD | Allowed | Denied | Allowed | Denied | Denied |
| RBACMNT | Allowed | Denied | Denied | Allowed | Denied |
| VIEWER + ENGINEER | Allowed | Allowed | Allowed | Allowed | Denied |

These are the permissions observed on the representative references below, rather than a claim of access to every model object. Both standard-role revisions behaved consistently. The combined role gained Engineer's configuration permission without gaining Operator's control permission. Aggregator's own dataset/report/configuration behavior could not be established from the available representative targets.

**Live availability**

| Identity | Tespro result |
|---|---|
| dso (role-bearing TLS certificate) | Connected; permitted reads and DSO scope separation passed |
| dual-dso (separate TLS and signed ACSE application certificates) | Connected; permitted reads and DSO scope separation passed |
| engineer-r0 | Connected; permitted measurement, dataset and RCB reads passed |
| viewer-r1 | Connected; permitted reads passed; dataset access denied |
| aggregator | Blocked before MMS association |
| viewer-r0 | Blocked before MMS association |
| operator-r0 / operator-r1 | Blocked before MMS association |
| engineer-r1 | Blocked before MMS association |
| installer-r0 / installer-r1 | Blocked before MMS association |
| secadm-r0 / secadm-r1 | Blocked before MMS association |
| secaud-r0 / secaud-r1 | Blocked before MMS association |
| rbacmnt-r0 / rbacmnt-r1 | Blocked before MMS association |
| multi-viewer-engineer | Blocked before MMS association |
| dual-viewer | Blocked before MMS association |

The live TLS pins reference `dso.pem`, `engineer-r0.pem`, `viewer-r1.pem`, `negative-revoked.pem` and `client_tls.pem`. The revoked pin remains rejected by the CRL. Security audit records report `trusted individual certificate not available` for unpinned peers. Commissioning an identity in the signed policy does not automatically enroll its TLS certificate. Making the remaining identities usable requires an explicit TLS enrollment/configuration change; none was applied during this check.

**Boundary checks and coverage**

- Allowed and denied configuration writes used `WlimDWMX1.WMaxSptPct.ctlModel[CF]` with value 3, with readback for permitted writes.
- Enhanced operations used `WlimDWMX1.Mod` with values 1 and 5. Authorized operations received command termination and matching `stVal[ST]` on the same association. Other roles received access denied.
- Raw writes to `PdCMMXU1.TotW.mag.f[MX]` and `WlimDWMX1.Mod.stVal[ST]` were denied for every role. These tests bypassed the menu's local guard and checked the server response.
- Report checks verified RCB read, enable, delivery and disable. GI passed for the applicable standard roles and was denied for DSO. Dynamic dataset creation was rejected globally and is not counted as evidence of per-role dataset-write permissions.
- DSO could not read Aggregator state; Aggregator could read its own state but could not read DSO measurements/state, write DSO configuration or access the DSO dataset/report.
- Six negative certificate specimens were rejected. Revocation was confirmed by the TLS audit. The other specimens were rejected as individually uncommissioned, so these checks do not independently prove each role-extension/AoR/revision validator.
- A signed DSO application certificate paired with the wrong TLS certificate established an association but could not read measurements/state, write configuration, initiate a control, access the dataset or read the RCB. The server recorded `ASSOCIATION_BINDING_FAIL` and denied access. Control initiation stopped at the denied control-model read.

The host harness uses existing compiled libraries whose adapter, authorization, certificate-authentication and policy-parser sources match the merged implementation after newline normalization. Its legacy ACSE bridge stores hooks in a separate shim; the merged build places that storage in libIEC61850. This is representative integration coverage, not on-device write/control or formal conformance certification. Aggregator operation remains unverified because the current model exposes no operable Aggregator target.

**Client changes made during verification**

The client now accepts the commissioned TLS-peer identity mode with `--auth-mode tls`; signed ACSE application-certificate mode remains the default. Mutual TLS, certificate-chain validation and server permissions remain enabled. Diagnostics preserve MMS access-denied results returned inside data values and control-model reads. Repeated report subscriptions can reuse a reservation acquired on the same persistent association, with server ownership checks still applied. No certificate, trust policy, role assignment, Tespro service or plant output was changed.

**Evidence**

- [Final combined results](final-results.json): 298 isolated checks passed, live availability and limitations.
- [Original role matrix](isolated/results.json): 19 identities, role-service checks and negative certificates. Its initial transport-pair assertion expected association rejection; the focused results below verify the implemented fail-closed access behavior instead.
- [Focused transport-binding results](binding-final/results.json): six checks passed.
- [Live read-only results](live/results.json): 37 successful read/negative checks and 15 blocked commissioned identities.
- [Live TLS rejection audit](live/tls-rejection-evidence.jsonl), [post-check policy hashes and service status](live/post-check.txt).
- [Negative certificate audit reasons](negative-audit-summary.json), [source comparison](source-equivalence.json).

Per-identity isolated directories retain the server configuration, server log and security event log. Credential paths are recorded; private-key material is not included.

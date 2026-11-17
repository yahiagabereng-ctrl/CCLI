# CCI security standards review

Review date: 26 October 2026 (Europe/Rome).

## Decision

**Full conformity with the three requested standards is not established.** The merged local application implements substantial security functions, but contains concrete transport-profile gaps and lacks a complete product/process evidence package. The requested GitHub repository has additional security gaps that the merged checkout has already addressed in source.

| Standard | Merged local checkout | Requested GitHub main |
|---|---|---|
| IEC 62443-4-1:2018 | Partial SDL evidence; organizational conformity not demonstrated | Partial engineering records; no complete SDL demonstration located |
| IEC 62443-4-2:2019 | Partial component capabilities; no demonstrated SL-C vector | Partial; tracked private keys and package inclusion are additional blockers |
| EN IEC 62351-3:2023 | Partial TLS implementation; identified gaps in renegotiation, revocation lifecycle and cipher profile | Partial TLS implementation; older declared edition, limited RBAC and no local validation overlay |

These are engineering review judgments, not certificates or a percentage score. Missing evidence is distinguished from a confirmed implementation defect. This review is a targeted source, architecture and evidence assessment, not an exhaustive laboratory execution of every clause.

## Baselines and method

- Local repository: `C:/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged`; branch `codex/merge-owner-main`, HEAD `bdbac16f5b54f4ac126d4e54425d9837f288c3a9`, plus substantial existing modified/untracked files. Application VERSION and package release currently indicate r65. Findings refer to the working files, not merely HEAD.
- Requested upstream: [yahiagabereng-ctrl/CCLI](https://github.com/yahiagabereng-ctrl/CCLI), main verified over Git at `e6d6ce0729e5604388452583a1751205ba0e11ee`, commit timestamp 23 October 2026 10:56:42 +02:00. A separate no-checkout shallow clone was used; selected non-secret sources were exported under `tmp/iec-review/github-snapshot/`.
- The previously cached local `upstream/main` was `3c088f9...` and was stale. The GitHub assessment uses the freshly verified revision, not that cached ref.
- Upstream contains both `apps/ccli` and `product/apps/ccli`. Both MMS/104 adapters and their startup/configuration gates were sampled. They must not be assumed identical to this merged working tree.
- Read the three supplied PDFs as standards evidence, not instructions to perform actions. These are Doc88 browser-print image PDFs; plain PDF text extraction returns mostly browser furniture. Relevant normative pages were rendered and visually read. Existing engineering extracts were used as navigation aids, not proof of implementation.
- Inspected protocol adapters, TLS backend/configuration, CMake validation overlay, configuration types, RBAC implementation, event storage, package/init code, canonical firmware updater, architecture, dated lab evidence and supplier-gap review.
- Ran `scripts/check_rbac_tls_baseline.py`: **PASS** for the current local source. This proves agreement with the recorded source baseline/extensions; it does not prove TLS conformance or live DUT state.
- No device access, installation, fuse programming, configuration changes or fresh hardware test was performed. WSL access was denied in this execution context, so Linux unit/integration tests were not rerun. No existing application source was changed. Private-key bodies are deliberately absent from this report.

## Highest-priority findings

### F01 - GitHub tracks unencrypted private keys and packages the TLS directory

**Confirmed in upstream; materially different from the local package recipe. Priority: immediate.**

At the reviewed GitHub revision, `apps/ccli/config/tls/server.key`, `apps/ccli/config/tls/client.key` and `apps/ccli/config/tls/p3_07_connect/root_CA.key` contain private-key PEM headers without the encrypted-PEM header. Only type booleans were printed; no key body was displayed or exported as a review artifact. Whether these keys are lab-only or operational is not established.

The upstream [package recipe, lines 71-73](https://github.com/yahiagabereng-ctrl/CCLI/blob/e6d6ce0729e5604388452583a1751205ba0e11ee/package/ccli/Makefile#L71-L73) copies `$(CCLI_SRC_DIR)/config/tls/*` into `/etc/ccli/tls/`. Thus builds using this recipe and the tracked inputs can ship server/client keys and the nested lab CA key. This is a source/packaging finding; the contents of every prebuilt APK were not unpacked and verified.

Relevant requirements: 62443-4-1 SM-7 and credential/supply-chain controls; 62443-4-2 CR 1.5 and CR 4.1. A CA private key distributed with device software undermines the trust model. SM-8 specifically concerns code-signing keys; the sampled TLS/CA keys are not automatically evidence of a code-signing-key leak.

**Close:** establish usage/exposure scope; replace affected trust and credentials where used; remove private inputs from tracked sources and all distributable artifacts; provision per-device keys securely; add package-content gates. Deleting current files alone does not invalidate copies or Git history. The local `package/ccli/Makefile` excludes live TLS inputs and preserves credentials during upgrade, but that fix is not present in upstream main.

### F02 - TLS 1.2 periodic renegotiation is not configured

**Confirmed local source gap; upstream adapters also lack configuration. Priority: high.**

EN IEC 62351-3:2023 clause 7.4.5, printed p.25 / supplied PDF p.33, requires configurable renegotiation, at most 24 hours for long-lived sessions; recommended default 12 hours, lower supported bound 10 minutes.

Local `apps/ccli/third_party/libiec61850/hal/tls/mbedtls3/tls_mbedtls.c:345` defaults `renegotiationTimeInMs` to -1, disabling automatic renegotiation. The MMS and 104 adapters do not call `TLSConfiguration_setRenegotiationTime`. The same adapters provide no configurable resumption/renegotiation intervals. The backend's six-hour session-cache default is not proof of periodic active-session key refresh.

Upstream MMS and 104 sources also make no interval-setting calls. The exact upstream pinned library build still requires independent verification; local backend findings must not be assigned to an uninspected upstream library binary.

**Close:** expose and validate the profile timers, configure the backend, align with revocation refresh, and test long-lived connections, client/server behavior and failures. ACSE/E2E rekey timers belong to a different layer and do not close this TLS requirement.

### F03 - Revocation handling exists, but lifecycle coverage is incomplete

**Confirmed partial implementation and historical release inconsistency. Priority: high.**

EN IEC 62351-3:2023 clauses 6.4.4.4.1-.3 require revocation-check capability at configurable intervals, specified CRL behavior and processing of stapled OCSP responses. Direct interaction with an OCSP responder is a 'should'; it must not be conflated with the mandatory stapled-response processing capability. CRL retrieval failure/expiry warnings and session continuity have specific rules.

- Local MMS loads a CRL when configured (`mms_adapter.cpp:1634` onward). Current Profile A additionally revalidates identity certificates (`mms_rbac_security.cpp:472`). This is positive evidence and is not a total absence of revocation checking.
- `Iec104TlsConfig` has no CRL field; `iec104_tls.cpp` never loads a CRL or implements an OCSP path. This applies to both local and sampled upstream 104 code.
- `lab/conformance/CCI_RBAC_CRL_AUTOMATION.md` explicitly records no enabled generation/distribution/reload automation and startup loading of TLS CRLs. It describes a newer r59 deployment with a CRL, superseding blanket claims that all deployments have CRLs off.
- Local archived r57 verification explicitly marks `crl=off` as PASS; its profile omits `tls_crl`. Upstream r64 profile also omits `tls_crl`, despite `PRODUCT_CYBER_MERGE.md` describing an intended CRL-on profile. Its TLS/measurement checklist does not close revocation behavior.
- The last seven-day CRL mentioned in the automation record expires 23 October 2026. That date is before this review, but does not establish the currently installed CRL is expired: no live state was inspected.

**Close:** support revocation on 104, implement verified refresh/reload and active-session handling, test stapled OCSP and all required security events, reconcile shipped profiles with release claims. Automation is an engineering means; the compliance issue is missing demonstrated required behavior, not merely the absence of a cron job.

### F04 - Cipher-suite profile needs correction and on-target verification

**Confirmed local source discrepancies; negotiated behavior untested. Priority: high.**

The supplied 2023 standard clause 7.2 Table 1, printed p.21 / PDF p.29, requires support for four TLS 1.2 suites, including `TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256`. NULL encryption is conditional and must be disabled by default with distinct authorized enabling.

The local mbedTLS3 backend default suite list at lines 372-387 omits that mandatory ECDSA AES-128 suite, while adding `TLS_RSA_WITH_NULL_SHA256`. The application does not clear/rebuild this list or expose a distinct NULL-suite enable policy. Compiled-library filtering may prevent NULL negotiation: this review does **not** claim a working NULL cipher was negotiated. Its default-list presence and missing explicit policy remain a source/profile concern.

**Close:** define the exact 2023 suite support and policy, provide suitable RSA/ECDSA test identities, ensure NULL is excluded by default, and capture positive/negative negotiation evidence from the frozen target build. Verify upstream's separately pinned backend before assuming its enabled suite set matches the local one.

### F05 - IEC 104 certificate validation can be disabled by incomplete configuration

**Confirmed local and upstream source behavior. Priority: high.**

`iec104_tls.cpp:31-47` disables chain validation when `ca_cert` is empty and disables pin restriction when `client_cert` is empty. The adapter requires only its own key/certificate. Combined omission creates a configuration path without the intended trusted-peer validation. Exact handshake consequences depend on the linked backend.

The sampled upstream product gates require TLS booleans and reject MMS port 102, but do not require trusted CA inputs, revocation controls or a complete secure profile. Positive startup guards are therefore only partial protection.

Relevant requirements: IEC 62351-3 clause 6.4.4; IEC 62443-4-2 CR 1.2, 1.8, 1.9.

**Close:** reject invalid product configurations before listener creation, require the chosen trust-validation model and test missing/empty/wrong trust material. Preserve explicit lab exceptions separately.

### F06 - Trusted boot and hardware-bound evidence remain open

**Evidence gap; not a claim that the silicon cannot support it. Priority: high.**

IEC 62443-4-2 EDR 3.14 requires pre-use boot integrity; RE(1), included at SL-C(SI)=2, requires authenticity against supplier roots of trust. Relevant companion requirements include EDR 3.12/3.13 and physical diagnostic/tamper controls where applicable.

`certification Tespro/CCLI_Tespro_Gap_Comparison_2026-10-22.md` records supplier draft secure-boot guides, but no demonstrated enforced chain on the actual board, fuse state, locked bypasses, authenticated recovery or full rootfs coverage. TPM-held update public trust and manufacturer APK verification are useful, but do not prove boot-time verification. No stronger final-product evidence was established in this review.

**Close:** obtain exact SKU/board/image/SDK identifiers and supplier proof, then demonstrate signed/altered/unsigned boot and recovery acceptance/refusal. Never infer whole-CCI conformity from a TPM component certificate.

### F07 - Secure development lifecycle conformity is not demonstrated

**Process evidence gap affecting both repositories. Priority: high.**

IEC 62443-4-1 addresses the supplier's documented and practiced processes, not just application features. The repository contains useful architecture, tests, change records, signing instructions and conformance planning. No complete approved package demonstrating the eight SDL practices was located. Older extracts explicitly label many organizational items GAP; those labels alone were not treated as fresh findings.

Needed evidence includes assigned roles/expertise, approved scope and target SL-C, maintained threat model, secure coding/review/static-analysis practice, independent verification where required, vulnerability intake/triage/disclosure, supplier-component risk management, release approval, patch qualification/timeliness and lifecycle/decommissioning guidance.

62443-4-2 clause 4.5 (CCSC 4) requires the secure development processes of 4-1. A technical-only 4-2 checklist cannot bypass that dependency.

**Close:** map each applicable 4-1 requirement to an approved process, named owner and executed release record. Organizational records may exist outside these repositories; they must be collected before an adverse organizational audit conclusion is final.

### F08 - Architecture isolation and logging are partial evidence

**Controls present; complete coverage unproven. Priority: medium/high by deployment.**

- `core/policy/net_policy.cpp` is explicitly a placeholder. Actual zone enforcement is delegated to OpenWrt, not that translation unit.
- Saved P2 cross-ping evidence supports inter-role forwarding separation. The same record uses INPUT accept and marks LuCI restriction PART. ICMP separation does not prove management-plane ACLs, IPv6, all protocols, boot/reload persistence or WAN/LTE isolation.
- Current event storage implements bounded retention, rotation, mode 0640, readout and degraded-persistence handling. These are useful CR 2.8-2.10/3.9/6.x contributions, but they do not prove protection from all unauthorized changes or complete event coverage.
- Remote EventStore sender uses UDP (`event_store.cpp:391`), without authenticated/confidential transport in that sender. Assess the actual deployment, including any localhost collector forwarding. IEC 62443 controls must be evaluated by selected requirements/context; this review does not assert that every 62443 deployment universally mandates RFC 5425.
- MMS TLS-event auditing is installed only with Profile A; upstream lacks that expanded event observer. Required 62351-3 security-event coverage needs a frozen-profile test.

**Close:** version and test the effective OS firewall/hardening baseline; demonstrate management isolation and resource-exhaustion behavior; test log access, persistence/failures and security-event generation/export.

## IEC 62443-4-1 practice screening

All eight practices were screened. This grouping is not an individual 47-requirement certification checklist.

| Practice | Evidence present locally | Remaining demonstration |
|---|---|---|
| SM, security management | Git/change records, modular source, canonical signing runbook, supplier reviews | Approved lifecycle process/RACI/training; security scoping; signing custody/rotation; supplier risk; release reviews; improvement records |
| SR, security requirements | Zones/conduits, security extracts and requirements/test plans | Approved security context, product threat model with maintained review, target SL-C and independent/customer/security review |
| SD, secure design | Layered architecture, interface separation, RBAC, safety gating | Current complete trust-boundary/interface analysis and reviewed mitigation traceability |
| SI, implementation | Security-specific code, validation overlay, regression tools | Enforced secure coding standard and source review/static-analysis evidence across changes |
| SVV, verification | Unit/integration suites, dated role/negative tests and protocol evidence | Complete applicable security-requirements/threat tests, vulnerability/penetration testing and independence records |
| DM, security issues | Technical gap lists and engineering fixes | Operational intake, triage, assessment, response, disclosure and periodic-review evidence |
| SUM, updates | Local operator-signed device-bound manifest, manufacturer verification, TPM trust/counter, replay tracking and upgrade safeguards | Qualified exact-release updates, dependent OS/component compatibility, failure/recovery evidence and patch-timeliness policy |
| SG, guidelines | Architecture, commissioning, deployment and signing guides | Controlled operator hardening, lifecycle responsibility, account management, decommissioning/data purge and reviewed manuals |

The upstream tree lacks the canonical `apps/firmware-updater` and expanded local RBAC/validation files. Do not transfer local positive evidence to upstream by implication.

## IEC 62443-4-2 component screening

The application can be assessed as SAR and the finished controller as an embedded device. NDR requirements also need an explicit applicability decision if the product provides network-boundary functions. A proposed SL-T 2 or an architectural `{2222222}` target is not demonstrated SL-C.

| Area | Status and evidence | Open coverage |
|---|---|---|
| Common constraints, clause 4 | Safe-output/supervision design and local least-privilege functions | Approved compensating measures, actual boundary and 4-1 process conformity |
| FR1, identification/authentication | TLS certificate authentication, local commissioned identities and PKI checks | All management interfaces/accounts/authenticator policies; 104 trust/revocation; lifecycle and key confidentiality; upstream F01 |
| FR2, use control | Local certificate-bound RBAC, signed policy, role/right tests; upstream has simpler DSO/Viewer control | Selected profile's complete service/role/admin/audit rights, session handling, wireless/diagnostic applicability and release-bound proof |
| FR3, integrity | TLS, local validation overlay, bounded parsers, safe-output logic, signed updater | Boot authenticity, complete update/recovery coverage, malformed-input/DoS and protected-audit proof |
| FR4, confidentiality | TLS and protected-file permissions | Upstream packaged keys; credential/data lifecycle; actual remote log path and purge behavior |
| FR5, restricted flow | Zone architecture and historical forwarding tests | Effective default-deny policy where required, management INPUT exposure, IPv6 and reload/reboot proof |
| FR6, event response | Local security events, persistent readout, supervision | Full event catalogue, authorized audit access, continuous monitoring and receiver-side delivery evidence |
| FR7, availability | Bounded event retention, watchdog/supervision, backup safeguards | Flood/handshake resource tests, verified backup/restore, exact-release recovery, least-functionality and component inventory |
| Device-specific requirements | TPM public update trust and supplier platform material | Diagnostic protection, boot/root-of-trust/tamper evidence according to selected type and SL; see F06 |

No SL-C level, including level 1, is declared achieved by this review. An SBOM is useful evidence for component inventory and dependency risk; it is not a substitute for the rest of FR7 or SDL.

## EN IEC 62351-3:2023 clause screening

| Clause/topic | Result |
|---|---|
| Edition/baseline | D7 explicitly declares 2014+AMD1:2018+AMD2:2020. Update and reconcile it to the supplied 2023 edition. IEC identifies 2023 as a technical revision with self-contained parameters. |
| TLS version | Both adapters force TLS 1.2. TLS 1.3 absence alone is not used here as an automatic failure; define selected-version conformance and its applicable tables. |
| 6.4.1, trust anchors | Bundle parsing supports more than one CA; later local records demonstrate combined trust. Capacity for at least five anchors and correct cross-domain decisions not demonstrated. A single file path does not prove single-anchor-only support. |
| 6.4.2, certificate size | Required handling through at least 8192 octets and oversize event/refusal not demonstrated against exact target build. |
| 6.4.3, mutual certificate exchange | Backend requires peer certificates; dated positive/no-client negative tests exist. Retest exact release and both protocol endpoints. |
| 6.4.4.1-.3, certificate validation | Local correction preserves pin/chain/CRL errors; upstream lacks that CMake overlay. 104 can disable validation. Verify backend and all certificate failure paths. |
| 6.4.4.4, revocation | Partial; see F03. Do not confuse CRL startup loading with complete live-session lifecycle. |
| 6.4.4.5, expired certificates | Library validation/local identity revalidation contribute; full frozen-release handshake/renegotiation/event tests remain needed. |
| 7.2, TLS 1.2 suites | Local mandatory ECDSA AES-128 suite missing from default list; NULL default-list issue; see F04. |
| 7.4.4-.5, resumption/renegotiation | Backend resumption cache exists; required application configuration/periodic behavior incomplete; see F02. |
| Security events / clause 10 conformance | Local observer and library events contribute. Full event mapping and updated conformance declaration/tests not established. |

The 2023 table treats certain algorithms/features differently from older D7 notes. Do not copy old mandatory/optional entries or older clause numbers into a 2023 declaration without reconciliation.

## Repository differences that change the answer

| Topic | GitHub main e6d6ce0 | Merged working tree |
|---|---|---|
| Entry architecture | `program/main.c` / `startup.c` product path, legacy option, parallel product source tree | Main reviewed runtime is service/orchestrator architecture; local E2E work is still modified/untracked |
| Product gate | Rejects cleartext MMS/104, but incomplete trust/profile validation | Secure defaults still need release-level enforcement and testing |
| RBAC | Basic DSO/Viewer logic in sampled adapter; no expanded `mms_rbac*` source files in tree inventory | Expanded roles, rights, commissioning, identity validation and later lab evidence |
| TLS pin-validation overlay | Not present in inspected CMake/tree inventory | Present and baseline-checked; do not flag the unpatched vendored callback as the actual local build without considering this overlay |
| Keys/package | Tracked PEM private keys; copies TLS directory | Package excludes live TLS inputs and preserves configuration/credentials |
| Updater | Canonical toolkit absent | Canonical manufacturer/operator-signed USB toolkit present |
| Latest archived product drop | r64 measurement/TLS checks | r57 archived product folder; newer local source/evidence elsewhere |

These are distinct code lines. A later-numbered upstream firmware does not automatically contain the local security work.

## Closure order and acceptance evidence

1. **Credential containment and packaging (HiTEKS/repository owner):** resolve F01, replace affected keys/trust according to actual use, scan resulting APK/USB payloads without disclosing secrets. Do not rewrite history or rotate device trust without a reviewed migration plan.
2. **Freeze one product baseline (HiTEKS + Tespro):** choose repository/branch/commit, board/SKU, platform image, SDK/feeds, dependency revisions, config and claimed component type/SL vector. Record source and binary hashes; retire contradictory profile claims.
3. **Close TLS profile gaps (HiTEKS):** timers, exact cipher policy, 104 trust/revocation, OCSP/stapling and event coverage; validate the five-anchor/certificate-size requirements.
4. **Demonstrate platform security (Tespro + HiTEKS):** enforced authenticated boot/rootfs/recovery and debug/physical controls appropriate to scope. Bind supplied documents to actual production settings and units.
5. **Complete SDL records (HiTEKS):** approved process and executed records for all applicable 4-1 requirements; trace each security requirement through implementation and independent testing as required.
6. **Execute final-product tests (HiTEKS + qualified lab):** frozen-build 62351 transport tests and applicable 62443 component tests, management/interface attack surface, invalid/revoked/expired identities, floods, power loss, logging failures, backup/recovery and signed USB update failure/replay cases. Preserve negative results and closure evidence.

## Source references

Supplied standards (printed page and PDF page differ):

- `Normativa/IEC 62443-4-1 2018.pdf`: visually checked selected contents/process pages, including SM-1/SM-2/SM-4/SM-5; used the repository extract to screen the remaining practices. This was not a visual reading of all 117 PDF pages.
- `Normativa/IEC 62443-4-2-2019.pdf`: visually checked selected contents, common constraints (printed 27/PDF29), supplier root-of-trust (printed67/PDF69) and boot requirements (printed69/PDF71); used the engineering extract to navigate CR/device families. This was not an exhaustive rereading of all 197 pages.
- `Normativa/EN IEC 62351-3-2023.pdf`: visually checked contents and certificate, revocation, cipher, resumption and renegotiation pages, particularly printed17-21 and24-25/PDF25-29 and32-33.

Official edition references: [IEC 62351-3:2023](https://webstore.iec.ch/en/publication/68410), [IEC 62443-4-1:2018](https://webstore.iec.ch/en/publication/33615), [IEC 62443-4-2:2019](https://webstore.iec.ch/en/publication/34421).

Key local evidence: `lab/conformance/D7_62351-3_PID_DRAFT.md`; `CCI_RBAC_CRL_AUTOMATION.md`; `CCI_RBAC_TESPRO_ACTIVATION_2026-10-16.md`; `CCI_RBAC_TESPRO_TEST_2026-10-16.md`; dated P2/P3 evidence under `lab/evidence/O13_1_isolation_2026-09-25/`; `certification Tespro/CCLI_Tespro_Gap_Comparison_2026-10-22.md`; architecture files; source paths cited above.

No competitor certificate under `Certificate/` was accepted as a certificate for HiTEKS CCI. No unrelated IEC 62351-4 E2E feature was treated as satisfying IEC 62351-3 transport requirements.

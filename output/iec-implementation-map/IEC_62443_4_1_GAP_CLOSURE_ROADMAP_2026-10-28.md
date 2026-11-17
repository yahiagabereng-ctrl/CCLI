# CCLI IEC 62443-4-1 gap closure roadmap

Prepared: 28 October 2026, Europe/Rome. Status: proposed execution plan; owners and dates require assignment at kickoff.

## Objective and starting point

Establish an approved, practiced secure development lifecycle for the selected CCLI product, demonstrate it on a controlled release, and assemble reviewable evidence for every applicable IEC 62443-4-1:2018 requirement.

The 28 October reassessment contains **47 requirement families and 139 engineering tasks** for 4-1. Its family assessments are **30 Partial, 15 Evidence missing, 1 Gap and 1 Scope pending**. None has delivery acceptance. These are engineering assessment counts, not compliance percentages. The 139 tasks are the project's breakdown of the 47 requirements, not additional IEC requirements.

The assessed baseline was source commit `15b0a9c51e368bdbad98e4e71d3784a6cfb7b6b5` and installed CCLI `0.1.0-r71`. This roadmap uses that dated assessment; it does not establish the present device configuration. Existing architecture, source, tests, deployment records and signing instructions are evidence inputs whose applicability must be checked against the release selected for qualification.

**Planning estimate: 16 calendar weeks from kickoff**, assuming a named SDL lead, firmware/platform engineering capacity, an available target controller, Tespro cooperation and independent testers booked early. This is an estimate for an executed lifecycle and release evidence package. Supplier delays, substantial product changes or test findings can extend it. Certification scheduling and the assessor's maturity/conformity decision are separate. A calendar deadline never closes a requirement automatically.

IEC describes 4-1 as lifecycle process requirements for the developer and maintainer, covering development, maintenance and retirement. See the [IEC scope](https://webstore.iec.ch/en/publication/33615). This roadmap's phase lengths, document organization and proposed operating controls are project choices.

## Sequence, owners and completion gates

| Phase | Target window | Work and accountable owner | Gate needed to proceed |
|---|---|---|---|
| A. Establish responsibility and scope | Weeks 1-2 | Product sponsor and SDL lead appoint owners, approve the SDL procedure, product boundary, applicability, intended security claims and release gates. Select independent testers and request supplier records. Open vulnerability intake. | Approved responsibilities and scope; qualified approvers; controlled evidence index; independence arrangements and supplier requests accepted. |
| B. Put engineering and response processes into operation | Weeks 2-4 | SDL lead, engineering lead and release custodian enforce coding/review controls, protect development and signing assets, inventory dependencies, train staff, and operate issue handling and patch-timing policies. | Controls are in use on real changes/issues; access, training, scan, supplier and key-custody evidence exists. Unresolved findings have owners and due dates. |
| C. Complete requirements and security design | Weeks 3-6 | Product architect leads the security context, threat model, requirements traceability, interface analysis and design review. Tespro supplies platform and custom-component evidence. | Approved requirements and design; each threat has a mitigation or justified disposition; security requirements have planned verification and operating guidance. |
| D. Complete the selected product and its support procedures | Weeks 5-10 | Engineering and release leads implement required changes, qualify patch delivery and dependency updates, and prepare hardening, operation, account and disposal guidance. | A reproducible release candidate, complete inventory, qualified update paths, usable guide drafts and implemented threat mitigations are ready for independent testing. |
| E. Independently verify and resolve findings | Weeks 9-15 | Independent QA/security testers execute requirements, mitigation, vulnerability and penetration tests. Engineering resolves findings and testers verify affected fixes. Documentation reviewers check the manuals. | Final candidate is identified by hashes; applicable tests and manual reviews pass, findings have approved dispositions, and changes after testing have impact analysis and necessary retests. |
| F. Approve a controlled release and audit the lifecycle | Weeks 14-16 | Release authority and QA verify issue and process gates, approve the release evidence, review process effectiveness and close audit findings. | Every applicable task has accepted evidence and a reviewer; a real release followed the approved lifecycle; future review/support responsibilities are active. |

Critical dependency: **scope -> security context and threat model -> requirements and design -> implementation and guides -> independently verified candidate -> release approval and lifecycle audit**. Issue handling, supplier work, training and evidence collection begin early and continue throughout.

### Role assignments

Roles below are proposed responsibilities. Assign actual people's names in week 1. People may hold several roles where appropriate; the tester independence rules still apply.

| Role used below | Responsibility |
|---|---|
| Sponsor | HiTEKS product/management authority: staffing, product scope, acceptable-risk criteria and release authorization. |
| SDL lead | Accountable for lifecycle processes, applicability, records, traceability, training coordination and the evidence index. |
| Architect | Product security context, threat model, security requirements and design. |
| Engineering lead | Firmware/platform implementation, coding standards, review, analysis and engineering remediation; coordinates Tespro. |
| Release custodian | Build/artifact integrity, signing operations, update qualification, packaging, patch release records and supported versions. |
| Security response lead | Vulnerability intake, investigation, assessment, resolution, disclosure and patch coordination. |
| Supplier owner | Requests and accepts Tespro/BSP/custom firmware and other supplier lifecycle/security evidence. |
| Independent QA | Security verification with the required independence, evidence quality and lifecycle audit. |
| Documentation owner | Product security, integration, hardening, operation, account and disposal instructions; coordinates practical review. |

### Decisions and actions in the first week

1. Name the sponsor, SDL lead, security response lead, release authority and independent test lead. Allocate engineering and documentation capacity.
2. Select the product boundary: application, finished controller, platform/BSP, updater, management software and any other supplied components. Identify responsibility for supplier-developed portions and supported versions.
3. Start the product-specific applicability and security-level decision. Record technical component requirements separately from the organizational process assessment. A low maturity target does not remove applicable lifecycle requirements.
4. Establish a controlled evidence index and assign the existing 139 task IDs to people. Keep the existing assessments until evidence supports an accepted change.
5. Request Tespro's custom-component SDL records, security support commitments, component/version inventory, patch compatibility information and platform interface/storage details. Set a week-4 response milestone and escalate missing material immediately.
6. Book independent verification and a penetration-test window. Record reporting lines before accepting tester independence.
7. Open security issue intake, inventory known issues from the existing assessments, assign owners, and preserve vulnerability and patch dates from the start.

## Closure plan for all 47 requirement families

The tables map each family to a responsible role, target and acceptance evidence. Weeks are relative to kickoff. An early procedure milestone and a later release milestone are both needed where shown. Detailed original tasks remain in the checklist at the end.

### Security management: 13 requirements

| Requirement | Current assessment | Owner and target | Work and evidence needed for closure |
|---|---|---|---|
| SM-1 Development process | Partial | SDL lead; procedure W2, release demonstration W16 | Approve one lifecycle procedure covering configuration/change control, product description and traceability, design/implementation, repeatable V&V, record approval and support. Retain executed change, review, test and release records from the qualification release. Reuse existing QMS controls where they actually cover these activities. |
| SM-2 Responsibilities | Evidence missing | Sponsor; W1 | Name an accountable person and authorized approver for every required activity. Approve a responsibility matrix, deputies and escalation path. Check that each of the 47 families has an owner and that verification assignments meet independence rules. |
| SM-3 Product applicability | Partial | Architect; W2 | Approve the products, parts, development locations and maintenance activities covered by the SDL. Explicitly include or assign supplied application, updater, platform/BSP and custom supplier firmware responsibilities. Retain the approved boundary and version list. |
| SM-4 Security expertise | Evidence missing | SDL lead; W3, then ongoing | Define role-specific competence needs; assess relevant staff, deliver required training and verify understanding. Retain role-to-training mapping, attendance/assessment evidence and a continuing-training schedule. A course list alone does not demonstrate competence. |
| SM-5 Process scoping | Evidence missing | SDL lead; W2 | Analyze applicability of all 47 requirements, record reasoned exclusions where valid, and obtain approval by people with appropriate security expertise. Link the decision to product scope and apply the selected processes throughout development. Retain the qualified reviewers and approved applicability record. |
| SM-6 File integrity | Partial | Release custodian; W4 | Inventory important delivered scripts, executables and other product files; define integrity mechanisms and verify their operation. Control related build/release manifests and evidence. Retain file coverage, public verification instructions, hashes/signatures and positive/negative verification results. |
| SM-7 Development environment security | Partial | Engineering lead; baseline W4, findings W10 | Document and enforce development, production and delivery protections: access control, protected changes, secret handling, build provenance, backups/recovery and artifact access. Assess historical credential exposure and execute any required controlled migration. Retain access reviews, control settings, restoration evidence and issue dispositions. |
| SM-8 Code-signing private keys | Partial | Release custodian; W4 | Approve key custody and authorized use, secure storage/access, backup/recovery and incident handling. Define supported rotation/revocation and trust-transition procedures. Retain access/operation records and exercises using safe test material. Follow the canonical updater workflow; evidence contains public identifiers and audit records, never private keys or passphrases. |
| SM-9 External components | Partial | Supplier owner; inventory W4, dispositions W10 | Inventory all externally provided hardware/software/firmware and maintain a release SBOM where applicable. Evaluate security context, supplier support, known vulnerabilities and notification paths. Retain component/version provenance, risk assessments, monitoring records and dispositions, not only an SBOM file. |
| SM-10 Custom supplier components | Evidence missing | Supplier owner; W6 | Identify security-relevant components developed specifically for CCLI, including applicable Tespro/BSP/custom work. Obtain lifecycle requirements in agreements and evidence of their execution. Review and accept supplier security, review, verification and support records. An unanswered questionnaire or purchase order does not close this family. |
| SM-11 Security issue release gate | Evidence missing | Sponsor; define W2, execute W16 | Define a gate covering issues from requirements, design, coding, verification and field reports. At release, review each issue, treatment and residual risk under DM-4 and record authorized decisions. Block release when required issue treatment is incomplete. Risk acceptance does not establish satisfaction of an unmet mandatory technical requirement. |
| SM-12 Process completion gate | Evidence missing | SDL lead; define W2, execute W16 | Define the required lifecycle records for a release. Verify every applicable process and its approvals before release and retain the completed checklist, evidence references and release authorization. Filling out a checklist retrospectively is not proof of a pre-release gate. |
| SM-13 SDL improvement | Evidence missing | SDL lead; first review W16, then ongoing | Review escaped defects and process effectiveness, identify causes and improvements, assign owners and verify corrective actions. Retain a management/engineering review with measurable actions and its next review date. Use actual issue history and the qualification release as inputs. |

### Security requirements: 5 requirements

| Requirement | Current assessment | Owner and target | Work and evidence needed for closure |
|---|---|---|---|
| SR-1 Security context | Partial | Architect; W3 | Approve intended users, assets, environments, product use, trust assumptions, physical access and deployment dependencies. Include management, Wi-Fi, USB, MMS/104, fieldbus and maintenance paths where present. Reconcile the intended production context with the assessed bench configuration. |
| SR-2 Threat model | Partial | Architect; W5 | Complete flows, stores, external entities, trust boundaries, internal/external protocols and physical/debug access. Assess hardware/software threats and severity; link mitigations, dependencies and residual issues. Retain development-team review and DM dispositions. Schedule released-product review at least annually and on relevant changes/new threats. |
| SR-3 Product requirements | Partial | Architect; W5 | Approve measurable requirements for installation, operation, maintenance and decommissioning. Map each requirement to threats/context, design, implementation, verification and guidance. Retain operating assumptions, acceptance criteria and versioned traceability; resolve ambiguous or untestable requirements. |
| SR-4 Requirements scope and level | Scope pending | Sponsor; W2 | Approve physical/logical scope and intended capability security level, including component types and the relevant FR vector/profile where used. Record optional-feature and interface-role decisions. State organizational maturity objectives separately. Retain explicit, reviewed claims rather than relying on an architectural target. |
| SR-5 Requirements review | Evidence missing | Architect; W6 | Review requirements for clarity, validity, threat coverage and verifiability with developers/architects, independent testers, customer advocate and security advisor. Retain attendance, required independence, comments, corrections and approval. Roles may overlap only where the applicable independence is preserved. |

### Secure design: 4 requirements

| Requirement | Current assessment | Owner and target | Work and evidence needed for closure |
|---|---|---|---|
| SD-1 Interface design | Partial | Architect; W5 | Document every internal/external physical and logical interface, accessible assets, users/roles/rights, boundary crossings, assumptions, third-party parts, safeguards and error/input/output behavior. Trace each mitigation to a modeled threat and describe intended use of external interfaces. Retain reviewed diagrams and interface specifications. |
| SD-2 Defense in depth | Partial | Architect; design W6, implementation W10 | Assign each defensive layer and its responsible party using the threat model. Resolve or explicitly substantiate network/management and platform assumptions against actual configuration. Retain design rationale, implementation/configuration evidence and verification of required independent layers. |
| SD-3 Design review | Evidence missing | Architect; W6 and each significant revision | Review unmet requirements, exploitable interfaces/assets/boundaries and design-rule violations. Retain a formal review with findings, responsible owners, dispositions and follow-up. Re-review affected design after substantial remediation or a changed product boundary. |
| SD-4 Design best practices | Partial | Engineering lead; standard W4, application W10 | Approve and apply secure design practices covering least privilege, simple proven mechanisms, attack-surface reduction and production debug controls. Review/update the practices periodically. Retain product-specific conformance reviews and Tespro evidence for protected or removed production debug access. |

### Secure implementation: 2 requirements

| Requirement | Current assessment | Owner and target | Work and evidence needed for closure |
|---|---|---|---|
| SI-1 Implementation review | Partial | Engineering lead; enforce W4, release evidence W15 | Define review criteria and run static analysis on each source change, using suitable tools where available. Review requirements/design traceability, coding violations and exploitable interfaces/assets. Inventory inherited delivered code and analyze the selected baseline as well as new changes. Retain reviews, tool versions/configuration, findings and disposition evidence. |
| SI-2 Secure coding | Partial | Engineering lead; standard W4, remediation W10 | Adopt language-specific coding standards, prohibited constructs/functions, trust-boundary validation, error handling and analyzer settings. Apply them to delivered source and scripts within scope; define treatment of supplier/third-party code. Retain the approved standard, enforcement configuration, code-review records and corrective changes. |

### Security verification and validation: 5 requirements

| Requirement | Current assessment | Owner and target | Work and evidence needed for closure |
|---|---|---|---|
| SVV-1 Security requirements tests | Partial | Independent QA; plan W8, results W13, retests W15 | Trace tests to every approved security requirement. Cover successful operation, errors/invalid input, performance/scalability, boundaries, stress and malformed/unexpected input. Identify test level and exact build/configuration/hardware, retain raw logs and dispositions, and satisfy departmental independence. Host regressions are useful inputs to this coverage. |
| SVV-2 Threat mitigation tests | Partial | Independent QA; plan W8, results W13, retests W15 | Test each threat mitigation and attempt to defeat it, including relevant authentication/replay, configuration, boundary, maintenance/update and failure behavior. Retain threat-to-test mapping, expected/actual results, raw evidence, findings and departmental independence. |
| SVV-3 Vulnerability tests | Partial | Independent QA; initial scans W4, complete campaign W13-15 | Use current vulnerability sources; test external protocols/interfaces with abuse/fuzz/flood/capacity tools where available. Analyze attack surface, ACLs/services, known vulnerabilities, delivered executable/firmware composition, compiler settings and runtime resources. Retain coverage, justified tool limitations, versions/dates, findings and resolution/retest evidence. Apply the independence appropriate to each method. |
| SVV-4 Penetration testing | Evidence missing | Independent QA; booking W2, tests W11-13, retests W15 | Arrange competent penetration testers with independent department/organization status. Approve scope against the entire selected product and threats; provide hardware and supported configuration. Retain exploitation attempts, findings, risk treatment and independent verification of fixes. Expand time if findings require redesign. |
| SVV-5 Tester independence | Evidence missing | SDL lead; arrangements W2, demonstrated W15 | Approve a category-to-tester matrix with names, product development involvement, reporting lines and provider contracts. Verify it against Table 3; retain actual authors/executors/reviewers with test records. A different job title or a second developer under the same development manager is insufficient for departmental independence. |

### Security issue management: 6 requirements

| Requirement | Current assessment | Owner and target | Work and evidence needed for closure |
|---|---|---|---|
| DM-1 Issue intake | Partial | Security response lead; active W2, demonstrated W4 | Establish a monitored reporting route and controlled issue tracker for users/integrators, suppliers, developers and testers. Define responsible staff, escalation and records through closure. Import known security findings with provenance and retain intake/handling records. |
| DM-2 Issue investigation | Partial | Security response lead; W4, then ongoing | Approve prompt applicability, reproduction/verification and triggering-threat investigation. Define escalation for urgent reports. Retain executed investigations showing affected configuration, reproduction evidence or justified inability to reproduce, and next action. |
| DM-3 Issue assessment | Partial | Security response lead; W4, then ongoing | Score severity with a defined vulnerability-scoring method and assess the discovery context, intended use and defensive layers. Identify affected products/releases, root causes and related issues. Retain reviewed assessments, including supplied-component impact. |
| DM-4 Issue resolution | Partial | Sponsor; criteria W4, release decisions W16 | Approve acceptable residual-risk criteria and decision authority. Record fixes, remediation plans, justified deferrals or acceptable risks; inform affected products/processes and suppliers. Review open issues each release/iteration and verify recurrence-prevention actions. Mandatory conformance obligations remain visible even when deployment risk is accepted. |
| DM-5 Vulnerability disclosure | Evidence missing | Security response lead; operational W4, exercised before W16 | Approve the reporting/disclosure policy and supported-product notification process. Prepare advisories with description, severity, affected versions, resolution and mitigations; retain evidence of actual required notifications. Exercise readiness when no real reportable case exists; an exercise does not replace notifications owed for known issues. Publishing or sending advisories is a separate authorized operational action. |
| DM-6 Issue-process review | Evidence missing | Security response lead; W16, then at least annually | Review the available issue history for completeness, efficiency and resolution. Retain minutes, metrics, corrective actions and verification of closure. Schedule and assign the next review; do not wait a year to perform the initial review of existing issues. |

### Security update management: 5 requirements

| Requirement | Current assessment | Owner and target | Work and evidence needed for closure |
|---|---|---|---|
| SUM-1 Patch qualification | Partial | Release custodian; W10, final retests W15 | Show that patches resolve their intended security issues without unacceptable regressions. Include own, supplier and dependent OS/component updates, supported source versions and the target controller. Retain functional/security regression, configuration preservation, failure/recovery and dependency-compatibility results for the exact candidate. |
| SUM-2 Patch documentation | Partial | Documentation owner; draft W10, approved W14 | Issue version-specific patch instructions for supported manual/automated paths, affected versions, restart/operational impacts, application verification, risks of nondeployment and mitigations. Validate instructions through an operator walkthrough and retain release approval. |
| SUM-3 Dependency patch guidance | Partial | Supplier owner; W10 | Obtain dependent platform/OS/component patch information and qualify compatibility. Publish tested combinations and mitigations for updates not approved for use. Retain supplier input, compatibility decisions, tests and customer-facing guidance. |
| SUM-4 Authentic patch delivery | Partial | Release custodian; W10, retests W15 | Qualify authentic update availability for supported versions and normal USB/device verification using the canonical toolkit. Resolve the assessed temporary-versus-permanent trust discrepancy through an approved commissioning/trust transition. Retain authentic/tampered/wrong-identity/replay outcomes and recovery evidence. SSH installation does not qualify the requested USB workflow. |
| SUM-5 Patch timeliness | Evidence missing | Security response lead; W4, demonstrated by W16 | Approve and operate risk-based qualification/delivery targets considering impact, public knowledge/exploits, affected installed population and mitigations. Retain timestamps, escalation/exception approvals and evidence of following the policy. These response targets are supplier commitments; 4-1 does not supply one universal patch deadline. |

### Security guidelines: 7 requirements

| Requirement | Current assessment | Owner and target | Work and evidence needed for closure |
|---|---|---|---|
| SG-1 Product security guide | Partial | Documentation owner; draft W10, approved W14 | Describe implemented capabilities, their defensive purpose, addressed threats and user mitigations for known/residual risks, including inherited-code limitations. Trace statements to the qualified product and reviewed issue records. Avoid describing planned features as delivered capabilities. |
| SG-2 Environment security guide | Partial | Architect; assumptions W6, verified guide W14 | Specify defensive measures required from the installation environment and identify who provides them. Reconcile zone/management/Wi-Fi/plaintext-protocol assumptions with supported deployment. Retain integration checks demonstrating the documented environment can support the approved product context. |
| SG-3 Hardening guide | Partial | Documentation owner; draft W10, approved W14 | Document dependency/API/protocol integration, defense in depth, every security setting and its default/range/security and operational effects, and how to set/change/delete it. Include administration/monitoring/incident tools, routine maintenance and incident reporting. Walk through instructions on the candidate configuration and retain corrections. |
| SG-4 Disposal guide | Gap | Documentation owner; method qualified W10, approved W14 | Inventory keys, logs, protected data, backups, storage and external references. Document removing the device, cleaning deployment references and securely deleting stored data. Validate the supported deletion method on representative hardware; where erasure cannot be assured, document secure physical disposal and its conditions. The 4-1 obligation is accurate disposal guidance; any selected 4-2 deletion capability needs its own technical evidence. |
| SG-5 Secure operation guide | Partial | Documentation owner; approved W14 | Define operator/administrator responsibilities, required recurring actions and security-relevant assumptions. Cover commissioning, account/PKI maintenance, monitoring, updates, backups and incident reporting as applicable. Retain an operator review demonstrating instructions are understandable and executable. |
| SG-6 Account guide | Partial | Documentation owner; approved W14 | Inventory every supplied/default/service account and its privileges across application, OS, management and other components. Explain changing applicable default identifiers/passwords and supported account lifecycle. Verify the guide against the candidate; reconcile bench root access and exposed management assumptions with the supported production setup. |
| SG-7 Manual review | Evidence missing | Documentation owner; review W14, corrections W15 | Review completeness and accuracy of security capabilities, integration and documented practices. Perform practical commissioning/hardening/update/disposal walkthroughs, record errors/omissions, correct them and obtain approvals. Freeze guide revisions with the release evidence. |

## Execution details that determine success

### Independent verification

The supplied standard's Table 3 was visually checked in its French text (PDF page 96, printed page 94); the English scan pages containing these provisions are incomplete. The following summarizes the independence assignments used by the existing register:

| Activity | Required independence from product developers |
|---|---|
| SVV-1 requirements testing; SVV-2 mitigation testing | Independent department. Record a reporting structure that satisfies the standard's definition, rather than merely allocating another developer. |
| SVV-3 abuse-case, attack-surface and known-vulnerability analysis | Independent person who was not a product developer. |
| SI-1 static analysis; SVV-3 software composition analysis | No tester independence required for these analyses. Keep their findings and dispositions. |
| SVV-4 penetration testing | Independent department or organization. |

If internal staffing cannot satisfy an assignment, engage a suitable external provider. Developers can support setup and investigation while the designated independent tester controls execution, results and the required retests. Confirm the documented arrangements with the selected assessor before the campaign.

### Product work that feeds the lifecycle evidence

4-1 requires an operating process for requirements, threats, design, implementation and issue treatment. The existing 4-2 and TLS findings are inputs to that process. Their actual technical closure depends on the approved product requirements and claims; this roadmap does not automatically add every 4-2 enhancement or optional feature to the product scope.

For the assessed CCLI baseline, resolve these dependencies early:

- **Deployment and interfaces:** establish the approved production treatment of management services, OS accounts, bridged LAN/Wi-Fi and fieldbus/plaintext paths. Reflect the result consistently in context, threat model, design, tests and guides.
- **Supplier/platform:** accept evidence for the actual supplied BSP/custom firmware, production/debug controls, storage/disposal behavior, supported component updates and any boot/recovery capabilities relied on by the design. Assign supplier remediation explicitly.
- **Signing and USB updates:** use only `apps/firmware-updater`, its `docs/SIGNING_AND_USB_UPDATES.md`, `scripts/new-bundle.sh`, local ignored `keys/`, `tools/bin/apk`, `artifacts/` and `bundles/`. Perform local manufacturer verification before bundling. Enter the encrypted operator-key passphrase only at the visible local prompt; private keys stay out of evidence exports, APKs and USB bundles.
- **Target acceptance:** qualify legitimate USB installation, trust handling, tampered/wrong-device/replayed or otherwise unauthorized bundles, supported-version updates, interrupted-update/recovery behavior and retained configuration as required by the approved design. Use a controlled target and a recovery plan. Do not substitute the previous SSH installation for USB evidence.
- **Known deferred features:** preserve deferred OCSP and other open technical obligations in requirements/issue records. An accepted project deferral does not satisfy a mandatory obligation in an unrestricted technical conformance claim. Resolve the feature or adjust the declared claim transparently with appropriate review.
- **Disposal:** choose a method the supplied storage and platform can actually support. Include retained backups and infrastructure references; simple file removal is not proof that protected data is unrecoverable.

### Issue, patch and review cadence

Activate intake and escalation in week 2. By week 4, management approves measurable investigation, mitigation, qualification and delivery targets and an exception process. Use the actual available team and product support obligations when selecting those targets.

Review open security issues at every release or iteration. Review the released-product threat model and issue-management process at least annually; perform earlier reviews for relevant changes or new threats. Update design/coding practices periodically. Schedule access, key-custody, component-vulnerability and process-effectiveness reviews at defined intervals; use initial monthly operational reviews as a project starting point, adjusted for risk.

Use existing issues and security changes as truthful evidence inputs. Label reconstructed historical records with their actual preparation dates and provenance. Future procedures require current execution records. Where no real vulnerability notification or incident exists, use a clearly labeled exercise to establish readiness and retain actual records when an event occurs.

## Evidence organization and final acceptance

Maintain one controlled evidence index rather than disconnected compliance documents. A practical organization is: governance/scope/training; product context/requirements/threat/design; secure engineering/suppliers; issue/patch operations; verification; product guides; release approval and review. Existing QMS and repository systems may own the authoritative records; the index should point to their controlled revisions.

For each task record: existing task ID, named owner, applicability decision, required output, target milestone, source clause, evidence link/revision, product/build/configuration where applicable, issue dispositions, reviewer and review date. Keep sensitive evidence access-controlled and use public identifiers or redacted records for keys/credentials.

**A family is ready for accepted closure only when:**

1. All its task obligations are accounted for and completed, or a qualified applicability decision justifies exclusion.
2. Required procedures and technical specifications are approved and versioned.
3. Records demonstrate execution on the selected product/release; applicable inherited evidence is justified and missing coverage is completed.
4. Requirement/threat/design/change/test/manual traceability is complete where relevant, with source, binaries, configuration and target identification sufficient to reproduce the result.
5. Tester independence meets the applicable category and is evidenced by actual people/reporting relationships.
6. Findings have verified fixes or permitted, authorized dispositions. Risk acceptance is not used to claim an unmet mandatory requirement as satisfied.
7. A reviewer accepts the evidence and the release authority verifies both SM-11 and SM-12 before release. An internal audit checks all 47 families and 139 task records; remaining audit findings are resolved before claiming the package complete.
8. Lifecycle support, monitoring, patch response and recurring reviews have assigned owners and active schedules. The process continues after the initial release.

At week 16, the proposed outcome is an accepted evidence package for an executed lifecycle, a controlled product release and a reconciled matrix. Formal conformity and organizational maturity claims require their own supported assessment; one release and this roadmap alone do not establish them.

## Sources and relationship to existing records

- [Latest 28 October assessment](<C:/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged/output/iec-implementation-map/COMPLIANCE_STATUS_2026-10-28.md>): current family assessments, evidence and remaining work used as the starting point.
- [Assessed requirement/task register](<C:/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged/output/iec-implementation-map/register-assessed-2026-10-28.json>): all 47 4-1 families and 139 task IDs/definitions; engineering paraphrases, with family assessments and individual acceptance open.
- [Original implementation map](<C:/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged/output/iec-implementation-map/IMPLEMENTATION_MAP.md>): requirement inventory, applicability and source interpretation decisions. Its older implementation statuses are superseded by the reassessment.
- [Supplied IEC 62443-4-1:2018 PDF](<C:/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged/Normativa/IEC 62443-4-1 2018.pdf>): normative reference; Table 3 independence checked visually for this roadmap. The roadmap does not reproduce a complete normative interpretation audit.
- [Canonical signing and USB procedure](<C:/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged/apps/firmware-updater/docs/SIGNING_AND_USB_UPDATES.md>): existing update workflow and required key-handling constraints.
- [IEC publication and scope](https://webstore.iec.ch/en/publication/33615).

The original workbook and assessed JSON are unchanged by preparation of this roadmap. Empty checklist boxes below are planning items and do not alter the original assessment or acceptance states.

## Original task checklist

All 139 original task IDs are retained below, grouped under the 47 families. Apply the corresponding owner, milestone and closure evidence in the tables above. Source: the assessed register linked above. These are the existing project task paraphrases.

### SM-1 - Development process (clause 5.2.1)

- [ ] `SDL-SM-1-01`: Document and enforce configuration/change control with audit records
- [ ] `SDL-SM-1-02`: Maintain product description and traceable requirements
- [ ] `SDL-SM-1-03`: Use defined hardware/software design and implementation practices
- [ ] `SDL-SM-1-04`: Maintain repeatable verification and validation
- [ ] `SDL-SM-1-05`: Review and approve development-process records
- [ ] `SDL-SM-1-06`: Provide lifecycle support

### SM-2 - Responsibilities (clause 5.4.1)

- [ ] `SDL-SM-2-01`: Identify organizational roles and responsible personnel for every required process

### SM-3 - Product applicability (clause 5.5.1)

- [ ] `SDL-SM-3-01`: Identify which products and product parts are covered by this SDL

### SM-4 - Security expertise (clause 5.6.1)

- [ ] `SDL-SM-4-01`: Identify role-specific security training and assessment needs
- [ ] `SDL-SM-4-02`: Provide training and retain evidence that responsible personnel have appropriate expertise

### SM-5 - Process scoping (clause 5.7.1)

- [ ] `SDL-SM-5-01`: Determine applicable SDL requirements using documented security analysis
- [ ] `SDL-SM-5-02`: Have scoping and justification reviewed and approved by appropriately qualified personnel
- [ ] `SDL-SM-5-03`: Follow every applicable requirement throughout product development regardless of organizational maturity level, as required by4.2

### SM-6 - File integrity (clause 5.8.1)

- [ ] `SDL-SM-6-01`: Provide an integrity-verification mechanism for all product scripts, executables and other important files

### SM-7 - Development environment security (clause 5.9.1)

- [ ] `SDL-SM-7-01`: Apply procedural and technical protection during design, implementation, testing and release
- [ ] `SDL-SM-7-02`: Protect product and updates during development, production and delivery

### SM-8 - Code-signing private keys (clause 5.10.1)

- [ ] `SDL-SM-8-01`: Protect signing private keys against unauthorized access
- [ ] `SDL-SM-8-02`: Protect signing private keys against unauthorized modification

### SM-9 - External components (clause 5.11.1)

- [ ] `SDL-SM-9-01`: Identify and manage security risks of every externally provided component

### SM-10 - Custom supplier components (clause 5.12.1)

- [ ] `SDL-SM-10-01`: Require compliant development lifecycle processes for security-relevant third-party components developed specifically for this product

### SM-11 - Security issue release gate (clause 5.13.1)

- [ ] `SDL-SM-11-01`: Do not release product or patch until security issues from requirements, design, implementation, verification and defect management have been addressed and tracked to closure

### SM-12 - Process completion gate (clause 5.14.1)

- [ ] `SDL-SM-12-01`: Before release verify completion of every applicable SDL process
- [ ] `SDL-SM-12-02`: Retain records showing that completion

### SM-13 - SDL improvement (clause 5.15.1)

- [ ] `SDL-SM-13-01`: Continuously improve the SDL
- [ ] `SDL-SM-13-02`: Analyze security defects in component, subsystem and system technologies that escape into the field

### SR-1 - Security context (clause 6.2.1)

- [ ] `SDL-SR-1-01`: Document the intended product security context and environment assumptions

### SR-2 - Threat model (clause 6.3.1)

- [ ] `SDL-SR-2-01`: Maintain a threat model for the current development scope
- [ ] `SDL-SR-2-02`: Include categorized information flows, trust boundaries, processes, data stores and external entities where applicable
- [ ] `SDL-SR-2-03`: Include internal/external protocols, exposed physical ports, debug headers and board connections where applicable
- [ ] `SDL-SR-2-04`: Identify hardware/software attack vectors, threats and severity using a vulnerability scoring system
- [ ] `SDL-SR-2-05`: Record mitigations or dispositions, security issues and external dependencies
- [ ] `SDL-SR-2-06`: Have the development team review and verify the model
- [ ] `SDL-SR-2-07`: Review released-product models at least annually and update for new threats
- [ ] `SDL-SR-2-08`: Assess and resolve identified issues through DM-3 and DM-4

### SR-3 - Product requirements (clause 6.4.1)

- [ ] `SDL-SR-3-01`: Document product/feature security capabilities for installation, operation, maintenance and decommissioning

### SR-4 - Requirements scope and level (clause 6.5.1)

- [ ] `SDL-SR-4-01`: State physical and logical product scope/boundaries
- [ ] `SDL-SR-4-02`: State required product capability security level SL-C

### SR-5 - Requirements review (clause 6.6.1)

- [ ] `SDL-SR-5-01`: Review, update and approve requirements for clarity, validity, threat-model alignment and verifiability
- [ ] `SDL-SR-5-02`: Include architects/developers, independent testers, customer advocate and security advisor
- [ ] `SDL-SR-5-03`: Keep tester independence even when other disciplines share personnel

### SD-1 - Interface design (clause 7.2.1)

- [ ] `SDL-SD-1-01`: Identify and characterize every physical/logical interface as internal, external or both
- [ ] `SDL-SD-1-02`: Document external-interface security-context implications
- [ ] `SDL-SD-1-03`: Identify users and directly/indirectly accessible assets
- [ ] `SDL-SD-1-04`: Identify trust-boundary crossings
- [ ] `SDL-SD-1-05`: Document security assumptions, constraints and threats
- [ ] `SDL-SD-1-06`: Define roles, privileges and access permissions
- [ ] `SDL-SD-1-07`: Define safeguards including input validation, output and error handling
- [ ] `SDL-SD-1-08`: Identify third-party interface components and their security capabilities
- [ ] `SDL-SD-1-09`: Document use of externally accessible interfaces
- [ ] `SDL-SD-1-10`: Trace design mitigations to threat-model threats

### SD-2 - Defense in depth (clause 7.3.1)

- [ ] `SDL-SD-2-01`: Implement risk-based multiple defensive layers from the threat model
- [ ] `SDL-SD-2-02`: Assign responsibility to each layer

### SD-3 - Design review (clause 7.4.1)

- [ ] `SDL-SD-3-01`: Review each significant design revision for unmet security requirements
- [ ] `SDL-SD-3-02`: Review interface, trust-boundary and asset exploitability
- [ ] `SDL-SD-3-03`: Review violations of secure design practices
- [ ] `SDL-SD-3-04`: Characterize and track discovered security issues to closure

### SD-4 - Design best practices (clause 7.5.1)

- [ ] `SDL-SD-4-01`: Document, apply and periodically update secure design practices
- [ ] `SDL-SD-4-02`: Apply least privilege and use proven secure components/designs where possible
- [ ] `SDL-SD-4-03`: Use simple mechanisms and secure design patterns
- [ ] `SDL-SD-4-04`: Reduce attack surface and document trust boundaries
- [ ] `SDL-SD-4-05`: Remove development debug ports/headers/traces from production boards or document and protect them

### SI-1 - Implementation review (clause 8.3.1)

- [ ] `SDL-SI-1-01`: Review unmet security requirements and secure coding violations
- [ ] `SDL-SI-1-02`: Perform static code analysis on every source change including new code
- [ ] `SDL-SI-1-03`: Use a static-analysis tool where one exists for the language
- [ ] `SDL-SI-1-04`: Review implementation traceability to the security design
- [ ] `SDL-SI-1-05`: Examine exploitable interfaces, trust boundaries and assets
- [ ] `SDL-SI-1-06`: Characterize and track implementation security issues to closure

### SI-2 - Secure coding (clause 8.4.1)

- [ ] `SDL-SI-2-01`: Maintain periodically reviewed security coding standards
- [ ] `SDL-SI-2-02`: Avoid exploitable constructs/patterns and banned functions
- [ ] `SDL-SI-2-03`: Define automated-tool settings and secure coding practices
- [ ] `SDL-SI-2-04`: Validate all trust-boundary inputs
- [ ] `SDL-SI-2-05`: Define and apply error handling

### SVV-1 - Security requirements tests (clause 9.2.1)

- [ ] `SDL-SVV-1-01`: Test security functions against requirements
- [ ] `SDL-SVV-1-02`: Test error scenarios and invalid input
- [ ] `SDL-SVV-1-03`: Include performance and scalability tests
- [ ] `SDL-SVV-1-04`: Include boundary, stress, malformed and unexpected input tests

### SVV-2 - Threat mitigation tests (clause 9.3.1)

- [ ] `SDL-SVV-2-01`: Create and execute tests proving each threat mitigation works
- [ ] `SDL-SVV-2-02`: Create and execute attempts to defeat each mitigation

### SVV-3 - Vulnerability tests (clause 9.4.1)

- [ ] `SDL-SVV-3-01`: Identify and characterize potential vulnerabilities using current recognized public vulnerability sources
- [ ] `SDL-SVV-3-02`: Run security abuse/malformed-input tests on all external interfaces/protocols where tools exist, including fuzz/flood/capacity testing
- [ ] `SDL-SVV-3-03`: Analyze attack surface, weak ACLs, exposed ports and privileged services
- [ ] `SDL-SVV-3-04`: Run black-box known-vulnerability scans for hardware/host/software components
- [ ] `SDL-SVV-3-05`: Perform software/composition analysis on all delivered binary executables including embedded firmware
- [ ] `SDL-SVV-3-06`: Detect known component vulnerabilities, vulnerable library links, security-rule violations and vulnerable compiler settings
- [ ] `SDL-SVV-3-07`: Perform dynamic runtime resource tests where tools exist, including timing-related DoS, memory leaks and shared-memory access

### SVV-4 - Penetration testing (clause 9.5.1)

- [ ] `SDL-SVV-4-01`: Identify and characterize security issues by attempting to exploit product vulnerabilities

### SVV-5 - Tester independence (clause 9.6.1)

- [ ] `SDL-SVV-5-01`: Use an independent department for security-requirements and threat-mitigation tests
- [ ] `SDL-SVV-5-02`: Use an independent person for abuse-case, attack-surface and known-vulnerability analysis
- [ ] `SDL-SVV-5-03`: Use an independent department or organization for penetration tests
- [ ] `SDL-SVV-5-04`: Static code analysis and software composition analysis require no tester independence
- [ ] `SDL-SVV-5-05`: Retain independence evidence against Table 3

### DM-1 - Issue intake (clause 10.2.1)

- [ ] `SDL-DM-1-01`: Receive and track to closure issues from security testers, suppliers, developers/testers and users including integrators, owners and maintainers

### DM-2 - Issue investigation (clause 10.3.1)

- [ ] `SDL-DM-2-01`: Investigate reported issues promptly for applicability, verifiability and triggering threats

### DM-3 - Issue assessment (clause 10.4.1)

- [ ] `SDL-DM-3-01`: Assess impact in actual discovery context, intended product context and defense-in-depth design
- [ ] `SDL-DM-3-02`: Assign severity using a vulnerability scoring system
- [ ] `SDL-DM-3-03`: Identify affected products/versions, root causes and related issues

### DM-4 - Issue resolution (clause 10.5.1)

- [ ] `SDL-DM-4-01`: Define an acceptable residual-risk level
- [ ] `SDL-DM-4-02`: Choose and record fix, remediation plan, justified deferral or acceptance below that threshold
- [ ] `SDL-DM-4-03`: Inform related processes/products/revisions and affected third-party source suppliers
- [ ] `SDL-DM-4-04`: Evaluate recommendations that prevent recurrence
- [ ] `SDL-DM-4-05`: Review open issues at least at every release or iteration

### DM-5 - Vulnerability disclosure (clause 10.6.1)

- [ ] `SDL-DM-5-01`: Inform supported-product users promptly of reportable issues
- [ ] `SDL-DM-5-02`: Include issue description, score, affected versions and resolution

### DM-6 - Issue-process review (clause 10.7.1)

- [ ] `SDL-DM-6-01`: Review issue management at least annually
- [ ] `SDL-DM-6-02`: Examine all issues since the last review for completeness, efficiency and resolution

### SUM-1 - Patch qualification (clause 11.2.1)

- [ ] `SDL-SUM-1-01`: Verify own updates fix the intended vulnerabilities
- [ ] `SDL-SUM-1-02`: Verify no regressions from own patches, supplier patches or dependent platform/component patches

### SUM-2 - Patch documentation (clause 11.3.1)

- [ ] `SDL-SUM-2-01`: Publish affected product versions
- [ ] `SDL-SUM-2-02`: Provide manual and automated approved-patch instructions
- [ ] `SDL-SUM-2-03`: Describe impacts including restart
- [ ] `SDL-SUM-2-04`: Explain how to verify application
- [ ] `SDL-SUM-2-05`: Document risks of not applying and mitigations for non-approved/non-deployed patches

### SUM-3 - Dependency patch guidance (clause 11.4.1)

- [ ] `SDL-SUM-3-01`: State product compatibility with component/OS security updates
- [ ] `SDL-SUM-3-02`: Provide mitigations for updates not approved by the supplier

### SUM-4 - Authentic patch delivery (clause 11.5.1)

- [ ] `SDL-SUM-4-01`: Make patches available for all supported products/versions in a way users can verify authenticity

### SUM-5 - Patch timeliness (clause 11.6.1)

- [ ] `SDL-SUM-5-01`: Define and follow qualification/delivery timing policy
- [ ] `SDL-SUM-5-02`: Consider vulnerability impact, public knowledge, published exploits, affected installed population and available mitigation

### SG-1 - Product security guide (clause 12.2.1)

- [ ] `SDL-SG-1-01`: Document implemented security capabilities and their defensive roles
- [ ] `SDL-SG-1-02`: Document addressed threats
- [ ] `SDL-SG-1-03`: Document user mitigations for known risks including legacy-code risks

### SG-2 - Environment security guide (clause 12.3.1)

- [ ] `SDL-SG-2-01`: Document defensive measures expected from the external deployment environment

### SG-3 - Hardening guide (clause 12.4.1)

- [ ] `SDL-SG-3-01`: Explain integration of product and dependencies with its security context
- [ ] `SDL-SG-3-02`: Explain integration of APIs/protocols with user applications
- [ ] `SDL-SG-3-03`: Explain applying and maintaining defense in depth
- [ ] `SDL-SG-3-04`: Explain each security setting, defaults, allowed values, security effects and operational effects
- [ ] `SDL-SG-3-05`: Explain setting, changing and deleting each security-option value
- [ ] `SDL-SG-3-06`: Document security tools/utilities for administration, monitoring, incident handling and evaluation
- [ ] `SDL-SG-3-07`: Document periodic security maintenance
- [ ] `SDL-SG-3-08`: Explain reporting product security incidents to the supplier
- [ ] `SDL-SG-3-09`: Document secure maintenance/administration practices

### SG-4 - Disposal guide (clause 12.5.1)

- [ ] `SDL-SG-4-01`: Explain removal from deployment and cleanup of references/configuration in that environment
- [ ] `SDL-SG-4-02`: Explain secure deletion of stored data
- [ ] `SDL-SG-4-03`: Explain secure physical disposal where data cannot be erased

### SG-5 - Secure operation guide (clause 12.6.1)

- [ ] `SDL-SG-5-01`: Document user/administrator responsibilities and required actions
- [ ] `SDL-SG-5-02`: Document assumptions about user/administrator behavior affecting security

### SG-6 - Account guide (clause 12.7.1)

- [ ] `SDL-SG-6-01`: Document permissions/privileges for OS, control-system, database and other product accounts
- [ ] `SDL-SG-6-02`: Document default/service accounts and how to change default names/passwords

### SG-7 - Manual review (clause 12.8.1)

- [ ] `SDL-SG-7-01`: Identify, characterize and close manual errors/omissions
- [ ] `SDL-SG-7-02`: Verify security-capability coverage, integration guidance and security of documented practices

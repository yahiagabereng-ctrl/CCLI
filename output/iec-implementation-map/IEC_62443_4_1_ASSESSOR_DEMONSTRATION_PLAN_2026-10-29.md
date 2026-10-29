# IEC 62443-4-1 assessor demonstration plan

Prepared 29 October 2026, Europe/Rome. This is a proposed walkthrough and evidence checklist, not evidence that the process has already been executed or accepted.

## What the demonstration needs to establish

Show that the organization has an approved secure development lifecycle, that people followed it on a real, identified CCLI release, and that the lifecycle continues through support and issue handling. The main demonstration should follow records created as the work happened, from approved scope and requirements through engineering, independent verification, release approval and delivery.

A policy document, a presentation, source code, a successful build, or a signed package by itself does not demonstrate the lifecycle. The assessor should be able to follow the links between these records and verify that they all refer to the same product version and release candidate.

IEC 62443-4-1 is the development and maintenance process assessment. Keep it distinct from technical product conformity under IEC 62443-4-2 and from any certification or maturity decision. The product boundary and claims need to be consistent across those workstreams.

## Start from the present evidence baseline

The 28 October assessment used source commit `15b0a9c51e368bdbad98e4e71d3784a6cfb7b6b5` and installed CCLI `0.1.0-r71`. It found 47 requirement families: 30 Partial, 15 Evidence missing, 1 Gap and 1 Scope pending, with no family yet accepted for delivery. These counts describe that dated assessment; they are not a compliance percentage or a statement about today's device state.

The r71 deployment record is useful historical evidence for the specific facts it records, including hashes and verification steps. It says installation was performed over SSH with a temporary key directory. Do not present it as proof of the normal target USB update path or as proof that r71 followed a newly approved, complete release lifecycle. Demonstrate those controls on a future controlled release and label earlier records accurately.

## Assemble one release evidence pack

Choose a release candidate that has not already been released. Freeze its identity before qualification. Create an index that maps every applicable 4-1 task in the saved gap-closure roadmap to a record, record owner, date, reviewer and disposition. Use stable references to the controlled originals; do not rely on a folder of unlabelled screenshots.

The pack should contain these connected records:

1. **Scope and ownership:** approved product boundary and supported versions; applicability decisions; security claims and assumptions; named accountable owners, approvers and independent verification roles; supplier responsibilities.
2. **Security basis:** security context, threat model, security requirements, interface/design review, and traceability from threats to mitigations, requirements, implementation and verification.
3. **Engineering history:** a real change request linked to issue IDs, source commit(s), human review and approval, secure coding/static-analysis results, dependency inventory/SBOM and supplier evidence. Record tool versions, configuration and finding dispositions.
4. **Verification:** test plan and traceability; exact firmware/software hashes, device model/revision and configuration; raw results for requirements tests, threat-mitigation tests, vulnerability testing and independent security testing; findings, approved dispositions, fixes and retest results. Retain failed results as well as passes.
5. **Release decision:** completed pre-release gates, unresolved-issue review, approved residual-risk decisions, required manual review, release authorization, build provenance, artifact hashes, public signature-verification result and package contents.
6. **Delivery and operation:** installation/update record for the selected target and supported delivery method; before/after identity and health checks; recovery or failure-test records; release notes, hardening/operation instructions, support period and vulnerability-reporting route.
7. **Lifecycle follow-through:** vulnerability intake and triage procedure, patch decision/timing records, supported-version tracking, and a completed example or controlled exercise showing an issue moving through assessment, fix, verification and communication. Include a process-effectiveness review and assigned improvements.

The records must be authentic and controlled. Preserve dates, authors, approvals, versions and audit history. If a required record is being reconstructed, mark it as retrospective and do not use it to claim that a pre-release gate operated at the time. Never put private signing keys or passphrases in the evidence pack.

## Walkthrough sequence for the assessor

Allow about 60–90 minutes, with the evidence index and candidate device ready beforehand.

1. **Set the boundary (5–10 min).** Show the approved product/version scope, intended users and environments, security claims, applicability decisions and named roles. Explain what comes from CCLI, the platform supplier and other suppliers.
2. **Trace one security requirement end to end (15–20 min).** Pick a real requirement from the approved register. Follow it to its threat/context, design decision, code change and human review, test case/result on the identified candidate, user guidance and release disposition. Let the assessor choose a second sample if time permits.
3. **Trace one defect or vulnerability (10–15 min).** Show its intake date, severity/risk assessment, owner, affected versions, decision, code fix, review, regression/retest, release note and any customer/supplier communication. If no live case is suitable, use a clearly labelled exercise; do not represent an exercise as a real field incident.
4. **Show independent verification (10–15 min).** Explain tester competence and reporting independence. Open the actual test report and raw evidence, identify the exact candidate hashes/configuration, then show findings and retests. Be candid about limits and unresolved results.
5. **Show release authorization and delivery (10–15 min).** Open the pre-release checklist and approvals, confirm unresolved security issues were reviewed, verify the artifact hash/signature, then demonstrate the documented installation/update process on the intended target. Capture device identity, installed version, health and recovery evidence.
6. **Show continued operation (5–10 min).** Show the support and vulnerability-reporting route, who monitors and triages reports, how patches are qualified, the support end date, and how product retirement/disposal is handled.
7. **Close with the evidence map (5 min).** Show status and reviewer for all applicable roadmap task IDs. Explain each exclusion or open item with its approved rationale, owner and due date. Do not turn an open item into a pass through verbal explanation.

## How to handle AI-generated code in the demonstration

AI assistance is not itself proof that the lifecycle was followed, and its use does not replace accountable engineering review. Treat generated output like any other proposed change: identify the human owner, preserve the resulting diff and source revision, review it against requirements/design and secure-coding rules, run the applicable analysis and tests, record findings, and require an authorized human to approve the merge and release.

As a practical internal control, record which AI tool/model was used and the relevant use context when policy permits; also record the human reviewer and checks performed. Do not send secrets or sensitive customer/device data to an AI service. These are recommended ways to make the work auditable; do not describe a particular prompt log or AI form as an IEC-mandated artifact unless the approved requirement interpretation says so.

The assessor should be able to select a changed line and ask: who owns it, what requirement or defect led to it, who reviewed it, which checks ran, what the results were, and which released binary contains it? The evidence should answer that from records, not memory.

## Release demonstration conditions

Before scheduling the walkthrough, use the roadmap's planned gates: approved scope and process; implemented and reviewed candidate; independent tests and dispositions; release checklist and authorization; controlled evidence index. Run the exact package and update procedure intended for customers on the intended target, and record any failure/recovery trials needed for qualification.

For any APK signing or USB bundle work, follow the repository's canonical `apps/firmware-updater/docs/SIGNING_AND_USB_UPDATES.md` workflow and its signing/bundling controls. Do not treat an SSH install as a substitute for a requested USB installation test. Keep signing secrets outside the evidence pack; retain only safe public verification data and audit records.

## Definition of a convincing result

An assessor can choose a requirement, change, test or release artifact and follow the references in both directions: from the requirement to the delivered candidate, and from the candidate back to its approved requirements, reviews, tests and release decision. The records are consistent about product version, source revision, configuration, hardware and hashes; exceptions and failures have explicit owners and decisions; and the same process is shown to work in practice.

Passing this walkthrough supports an evidence-based assessment. It does not by itself guarantee conformity, certification or closure of technical 4-2 requirements; the assessor evaluates applicability, adequacy and implementation of the lifecycle evidence.

## Related project record

The full 47-family gap plan and 139 engineering tasks are in `IEC_62443_4_1_GAP_CLOSURE_ROADMAP_2026-10-28.md`. Use that roadmap's task IDs and acceptance evidence as the detailed evidence index.

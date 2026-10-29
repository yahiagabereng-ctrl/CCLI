"""Build a source/evidence reassessment without changing normative requirements.

Engineering assessments are separate from task acceptance: no row is certified
or automatically closed by this report. Run from the repository root.
"""
from pathlib import Path
from collections import Counter
from copy import deepcopy
import datetime as dt
import hashlib
import json
import subprocess

ROOT = Path.cwd()
OUT = ROOT / 'output/iec-implementation-map'
ORIGINAL = json.loads((OUT / 'register.json').read_text(encoding='utf-8'))
GIT = ['git', '-c', 'safe.directory=' + ROOT.as_posix()]
HEAD = subprocess.check_output(GIT + ['rev-parse', 'HEAD'], text=True).strip()
DATE = '2026-10-28'

# Evidence describes scope and limits as well as a file location. Earlier
# campaigns are not represented as tests executed again on the installed r71.
E = {
 'TLS': ('Strict transport policy and shared HAL', [
  'apps/ccli/adapters/security/iec62351_backend.inc',
  'apps/ccli/adapters/security/mbedtls_config.h',
  'apps/ccli/cmake/iec62351_tls_overlay.py',
  'apps/ccli/cmake/tls_idle_tick.py']),
 'ADAPTER': ('MMS and IEC 104 server configuration/enforcement', [
  'apps/ccli/adapters/iec61850_mms/mms_adapter.cpp',
  'apps/ccli/adapters/iec60870_104/iec104_tls.cpp',
  'apps/ccli/core/config/ccli_config.hpp']),
 'TLS-TEST': ('Host regression source plus earlier isolated ARM64 campaign results', [
  'apps/ccli/tests/integration/iec62351_tls_test.py',
  'output/iec62351-tls/device/README.md',
  'output/iec62351-tls/device/results.json']),
 'MERGE': ('Merged-source validation record: 19 selected host tests; historical installation statements in this record predate r71 deployment', [
  'lab/conformance/OWNER_BRANCH_MERGE_2026-10-27.md']),
 'CLOSURE': ('Current TLS behavior, configuration and remaining limitations', [
  'lab/conformance/IEC62351_TLS_CLOSURE.md']),
 'DEPLOY': ('r71 installation, package/binary hashes, preserved configuration and later meter configuration', [
  'lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r71_2026-10-27.md']),
 'RBAC': ('Owner-signed policy, identity binding, roles, access decisions and replay checks', [
  'apps/ccli/adapters/iec61850_mms/mms_rbac_security.cpp',
  'apps/ccli/adapters/iec61850_mms/mms_rbac.cpp',
  'apps/ccli/adapters/iec61850_mms/mms_xacml.cpp',
  'apps/ccli/tests/integration/mms_rbac_tls_test.cpp']),
 'LOG': ('Bounded audit retention, permissions, readout, persistence-error handling and optional UDP syslog', [
  'apps/ccli/core/event/event_store.cpp',
  'apps/ccli/core/event/event_store.hpp',
  'apps/ccli/tests/unit/event_store_rotation_test.cpp']),
 'SAFE': ('Deterministic-output and supervision mechanisms; whole-product failure tests remain open', [
  'apps/ccli/core/pf2/pf2_fsm.cpp',
  'apps/ccli/services/svc_io.c',
  'apps/ccli/core/service/service_supervision.cpp',
  'apps/ccli/tests/unit/io_safe_state_test.c']),
 'UPDATE': ('Manufacturer verification and device-bound operator authorization with TPM trust/counters; complete target USB/recovery qualification is separate', [
  'apps/firmware-updater/src/firmware_updater.cpp',
  'apps/firmware-updater/docs/SIGNING_AND_USB_UPDATES.md',
  'apps/firmware-updater/tests/test_authorization.py']),
 'PACKAGE': ('Actual package recipe excludes TLS private inputs; current release is r71', [
  'package/ccli/Makefile', 'apps/ccli/VERSION', 'apps/ccli/CMakeLists.txt']),
 'ARCH': ('Architecture and zone intent; some older statements do not match the current device', [
  'Architecture/CCI_Runtime_Dataflow_Architecture.md',
  'Architecture/Software_Architecture.md',
  'apps/ccli/core/policy/net_policy.cpp']),
 'GUIDE': ('Engineering guides exist; controlled release manuals and approval evidence remain open', [
  'lab/CCLI_DUT_ADMIN_HARDENING.md',
  'apps/firmware-updater/docs/SIGNING_AND_USB_UPDATES.md',
  'apps/ccli/config/README.md']),
 'SUPPLIER': ('Supplier-document review records unresolved exact-unit boot/TPM/tamper evidence', [
  'certification Tespro/CCLI_Tespro_Gap_Comparison_2026-10-22.md']),
 'DECL': ('Existing PID is a draft with older-edition/profile statements; needs reconciliation to 2023', [
  'lab/conformance/D7_62351-3_PID_DRAFT.md']),
 'LIVE': ('Read-only Tespro observations on 28 October: listeners, root password auth, LAN/Wi-Fi bridge, GNSS/chrony, hashes and CRL expiry', [
  'output/iec-implementation-map/reassessment-2026-10-28/tespro-runtime.txt']),
 'KEYS': ('Current tracked PEM/key-candidate scan found no private-key PEM markers; historical exposure/rotation not discharged', [
  'output/iec-implementation-map/reassessment-2026-10-28/source-checks.json']),
 'INVENTORY': ('Original supplied-standard requirement inventory, conditions and SL enhancements retained', [
  'output/iec-implementation-map/register.json']),
}

A = {}
def put(key, state, implemented, gap, evidence):
    assert key not in A, key
    A[key] = dict(status=state, implemented=implemented, remaining=gap,
                  evidence=evidence.split(','), acceptance='Open')

# IEC 62443-4-1: an engineering artifact does not demonstrate an approved,
# executed organizational process. Every named requirement is assessed.
SDL = [
('SM-1','Partial','Git history, build recipes, tests and release/deployment records exist.','Approve the lifecycle procedure and retain executed change-control, review and support records.','PACKAGE,DEPLOY,MERGE'),
('SM-2','Evidence missing','Technical roles appear in plans.','Provide approved responsibility/authority assignments and named accountable people.','INVENTORY'),
('SM-3','Partial','Application and Tespro product boundaries are described in architecture.','Approve which products/components and lifecycle activities are in scope.','ARCH,SUPPLIER'),
('SM-4','Evidence missing','Security implementation work is visible.','Provide competence criteria, training and continuing-expertise records.','INVENTORY'),
('SM-5','Evidence missing','Requirement conditions are inventoried.','Approve SDL scoping and justified exclusions for this product.','INVENTORY'),
('SM-6','Partial','Source fingerprints, package signatures and deployment hashes exist.','Control integrity and access for all lifecycle artifacts, not just selected source and releases.','PACKAGE,DEPLOY,UPDATE'),
('SM-7','Partial','Current tracked private-key candidates are clean and package preparation excludes TLS secrets.','Close historical credential exposure, developer environment controls, access reviews, backups and supply-chain risk.','KEYS,PACKAGE,UPDATE'),
('SM-8','Partial','Canonical signing toolkit keeps local ignored keys and prompts for operator passphrase.','Demonstrate approved signing-key custody, authorization, rotation, revocation and incident handling.','UPDATE,KEYS'),
('SM-9','Partial','Dependencies and build settings are recorded.','Complete release SBOM, external-component inventory, vulnerability monitoring and supplier qualification.','PACKAGE,SUPPLIER'),
('SM-10','Evidence missing','Supplier package and integration material exist.','Provide contractual and executed SDL/security records for custom supplier components.','SUPPLIER'),
('SM-11','Evidence missing','Technical gaps and test failures have been tracked.','Show a release gate assessing every open security issue and recording approved disposition.','DEPLOY,MERGE'),
('SM-12','Evidence missing','Build and installation gates exist.','Prove required SDL activities and approvals completed before release.','UPDATE,DEPLOY'),
('SM-13','Evidence missing','Engineering corrections are recorded.','Provide periodic SDL effectiveness review and improvement records.','INVENTORY'),
('SR-1','Partial','Architecture describes users, protocols and zones.','Approve the full security context, environmental assumptions and product trust boundaries.','ARCH'),
('SR-2','Partial','Architecture and risk material provide inputs.','Provide a maintained product threat model with reviewed threats, mitigations and residual risks.','ARCH'),
('SR-3','Partial','The 289-family inventory and protocol/security requirements exist.','Approve product-specific requirements and trace them to tests and operating assumptions.','INVENTORY,ARCH'),
('SR-4','Scope pending','All component enhancements and TLS profile conditions remain inventoried.','Approve component types, security-level vector, interface roles and feature applicability.','INVENTORY,SUPPLIER'),
('SR-5','Evidence missing','Engineering reviews exist.','Provide required stakeholder/security/customer review and approval records.','MERGE,INVENTORY'),
('SD-1','Partial','Modular interfaces, TLS, RBAC and trust-policy structures exist.','Complete current trust-boundary/interface documentation including management, Wi-Fi and fieldbus paths.','ARCH,TLS,RBAC,LIVE'),
('SD-2','Partial','Layered TLS, signed policy, signed updates and output safety exist.','Reconcile actual LAN/Wi-Fi bridge and exposed management services with defense-in-depth assumptions.','TLS,RBAC,UPDATE,SAFE,LIVE'),
('SD-3','Evidence missing','Merge and code review records are available.','Provide an approved security-design review with full mitigation coverage and tracked findings.','MERGE,ARCH'),
('SD-4','Partial','Recognized crypto, bounded parsers and safe defaults exist in application paths.','Establish and apply reviewed security-design best practices across the complete product.','TLS,RBAC,SAFE'),
('SI-1','Partial','Source review fixes, validation overlays and regression evidence exist.','Provide controlled implementation-review criteria and complete review/static-analysis records.','MERGE,TLS-TEST'),
('SI-2','Partial','Input checking and crypto policy are implemented in inspected paths.','Enforce a secure coding standard, banned-pattern rules and automated analysis across all delivered software.','TLS,RBAC,PACKAGE'),
('SVV-1','Partial','Host and isolated ARM64 positive/negative security campaigns passed; installed MMS reads succeeded.','Complete exact-release requirements coverage, stress/scalability and boundary tests with required independence.','TLS-TEST,MERGE,DEPLOY'),
('SVV-2','Partial','Replay, identity binding, revoked/expired peers and malformed authentication are exercised.','Trace and test every approved threat mitigation and defeat attempt independently.','RBAC,TLS-TEST,MERGE'),
('SVV-3','Partial','Malformed-input, protocol resilience and native security campaigns exist.','Complete current vulnerability/SCA/SAST scans, all-interface fuzz/flood/resource testing and disposition.','TLS-TEST,MERGE'),
('SVV-4','Evidence missing','Internal negative/regression tests exist.','Provide qualified independent product penetration-test results and issue closure.','MERGE'),
('SVV-5','Evidence missing','Test authors and engineering execution are recorded in some artifacts.','Prove the required independent person/department/organization for each test category.','INVENTORY,MERGE'),
('DM-1','Partial','Technical gap reports and Git fixes provide engineering issue records.','Establish operational security issue intake, tracking and closure across suppliers/users/testers.','MERGE,SUPPLIER'),
('DM-2','Partial','Specific TLS, CRL and interoperability issues were investigated.','Provide the approved repeatable investigation process and executed security issue records.','CLOSURE,DEPLOY'),
('DM-3','Partial','Technical impact and root causes are discussed in reviews.','Record severity scoring, affected releases/products, context and related issues.','CLOSURE,SUPPLIER'),
('DM-4','Partial','Fixes and explicit remaining gaps are recorded.','Approve residual-risk criteria, deferred issues, stakeholder notification and periodic release review.','CLOSURE,MERGE'),
('DM-5','Evidence missing','No accepted user vulnerability-disclosure record was located.','Provide supported-product disclosure policy, advisories and executed notification evidence.','INVENTORY'),
('DM-6','Evidence missing','No accepted periodic issue-process review was located.','Retain annual/comparable required issue-process review and corrective actions.','INVENTORY'),
('SUM-1','Partial','r71 package signature, preservation checks and installed MMS read/browse passed.','Qualify all update paths, supplier dependency updates, regressions, power loss and recovery on exact product.','UPDATE,DEPLOY'),
('SUM-2','Partial','Signing, update and deployment instructions exist.','Publish controlled release patch instructions, impacts, verification and nondeployment mitigations.','UPDATE,GUIDE,DEPLOY'),
('SUM-3','Partial','SDK/dependency baselines and integration constraints exist.','Publish tested OS/component patch compatibility and mitigations for unapproved updates.','PACKAGE,SUPPLIER'),
('SUM-4','Partial','Manufacturer-signed APKs and operator-signed device-bound bundles are implemented.','Qualify normal target delivery/USB trust and recovery; r71 SSH install used a temporary verified key directory because permanent APK trust differed.','UPDATE,DEPLOY'),
('SUM-5','Evidence missing','No approved patch response-time policy was located.','Define and demonstrate risk-based patch qualification and delivery deadlines.','INVENTORY'),
('SG-1','Partial','Architecture, TLS closure and security guides explain implemented features.','Issue a controlled product security manual with threats, residual risks and mitigations.','ARCH,CLOSURE,GUIDE'),
('SG-2','Partial','Zones and environmental assumptions appear in architecture.','Approve external countermeasures and reconcile them with current bridge/management exposure.','ARCH,LIVE,GUIDE'),
('SG-3','Partial','Hardening and PKI/update runbooks exist.','Publish consistent current defaults, settings, lifecycle maintenance and secure integration instructions.','GUIDE,DECL'),
('SG-4','Gap','Backup and update retention exist.','Implement/document validated decommissioning and secure deletion of keys, logs and protected data.','GUIDE,UPDATE'),
('SG-5','Partial','Operator commissioning and deployment instructions exist.','Approve explicit operator/administrator responsibilities and secure-operation assumptions.','GUIDE'),
('SG-6','Partial','Role permissions, key provisioning and SSH administration are described.','Complete every default/service/OS account guide; current root-password and management exposure are not hardened.','RBAC,GUIDE,LIVE'),
('SG-7','Evidence missing','Engineering guides have been updated.','Provide formal review of manual completeness, accuracy, integration and security practices.','GUIDE,DECL'),
]
for k,*v in SDL: put('SDL-'+k,*v)

# Whole-controller assessment: application mechanisms are explicitly identified
# without assuming they cover unrelated OpenWrt management interfaces.
COMP = [
('CCSC1','Partial','Safe-output logic and watchdog/supervision exist.','Demonstrate essential-function preservation under all applicable security failures and DoS.','SAFE,LIVE'),
('CCSC2','Partial','Architecture and supplier reviews describe external dependencies.','Approve compensating measures and demonstrate they are deployed.','ARCH,SUPPLIER,LIVE'),
('CCSC3','Partial','Certificate-bound application roles and signed permission scopes exist.','Cover OS/management accounts and full permission administration; shared root access remains.','RBAC,LIVE'),
('CCSC4','Partial','Engineering artifacts support portions of the SDL.','Complete the approved/executed 62443-4-1 process evidence.','INVENTORY,MERGE'),
('CR1-1','Partial','MMS users/processes have certificate authentication; SSH/management authentication exists.','Demonstrate authentication and segregation of duties on every human-access interface; harden root access.','RBAC,TLS,LIVE'),
('CR1-2','Partial','MMS/104 use mutual certificates and ACSE identity binding.','Cover all claimed process/device interfaces and per-device provisioning; fieldbus/management scope remains.','TLS,ADAPTER,RBAC,LIVE'),
('CR1-3','Partial','Signed identity/role policy supports application account provisioning.','Qualify complete create/change/disable/delete lifecycle and all OS/web accounts.','RBAC,GUIDE,LIVE'),
('CR1-4','Partial','Certificate fingerprints, role identifiers and signed bindings are checked.','Demonstrate identifier uniqueness, administration and lifecycle across full product.','RBAC,LIVE'),
('CR1-5','Partial','Key pairing, certificate validity, CRLs and controlled local signing inputs exist.','Close historical exposure, rotation/default credential handling, all-interface authenticator management and key custody.','TLS,UPDATE,KEYS,LIVE'),
('CR1-7','Gap','Certificate-based application authentication avoids application passwords.','Management password-strength enforcement is not demonstrated; weak shared root credential remains usable on this bench.','LIVE,GUIDE'),
('CR1-8','Partial','CA bundles, chains, CRLs, pinning and signed commissioning policy are implemented.','Qualify the complete PKI lifecycle, renewal, unique credentials and all product interfaces.','TLS,ADAPTER,RBAC,KEYS'),
('CR1-9','Partial','Application certificate proof-of-possession, chain/revocation and identity binding are implemented.','Demonstrate the requirement across every claimed public-key interface and exact release.','TLS,ADAPTER,TLS-TEST,RBAC'),
('CR1-10','Partial','Operator key passphrases use local prompts; GUI masks key-passphrase input.','Verify feedback obscuring on every SSH/web/local authenticator interface.','UPDATE,GUIDE,LIVE'),
('CR1-11','Gap','Replay rejection and authentication denial exist.','Implement/qualify configurable failed-attempt thresholds, time windows and deny/reset behavior; replay caches do not provide this.','RBAC,TLS,LIVE'),
('CR1-12','Evidence missing','Management services are present.','Show a configurable preauthentication system-use notice on every applicable local HMI/access interface.','LIVE,GUIDE'),
('CR1-14','Scope pending','Current TLS authentication uses asymmetric certificates.','Confirm whether any PSK/symmetric authentication is claimed; TLS traffic encryption alone does not activate this authentication feature.','TLS,INVENTORY'),
('CR2-1','Partial','Application read/write/control decisions use signed role/permission policy.','Qualify OS/web/FTP permissions and signed scopes for the 23-node model; default denial of new nodes is not an allow-all fix.','RBAC,DEPLOY,LIVE'),
('CR2-2','Partial','Two enabled Wi-Fi APs use PSK2 on the LAN bridge.','Approve and test wireless access restrictions, authorization, monitoring and zone isolation.','LIVE,ARCH'),
('CR2-5','Evidence missing','GUI/SSH sessions exist.','Demonstrate configurable inactivity/manual session lock and authenticated unlock on applicable human interfaces.','LIVE,GUIDE'),
('CR2-6','Partial','Application disconnects and TLS failure/deadline termination exist.','Qualify configurable human/remote inactivity termination and local-authority termination across interfaces.','TLS,ADAPTER,LIVE'),
('CR2-7','Gap','Protocol stack resource limits exist.','Implement/qualify configurable per-user concurrent-session limits for all claimed interfaces.','ADAPTER,LIVE'),
('CR2-8','Partial','Security authentication/authorization/TLS events carry timestamp, source and result information.','Complete required platform, backup/restore, configuration and audit-event catalogue and actor attribution.','RBAC,LOG,ADAPTER,LIVE'),
('CR2-9','Partial','Latest 2048 events are retained with bounded rotating segments.','Qualify allocated disk capacity, exhaustion behavior and product retention policy.','LOG,LIVE'),
('CR2-10','Partial','Persistence errors preserve in-memory events and report failure; safe-output paths exist.','Exercise logger faults, full/read-only storage and essential functions on the final device.','LOG,SAFE'),
('CR2-11','Implemented','Application audit records carry UTC date/time timestamps.','Final acceptance remains open; synchronization and time-source integrity are separate enhancement rows.','LOG,LIVE'),
('CR2-12','Partial','Application actions can be attributed to signed identities and recorded actors.','Shared root and mutable audit storage prevent full human accountability/non-repudiation demonstration.','RBAC,LOG,LIVE'),
('CR3-1','Partial','MMS/104 TLS and signed ACSE protect their communications.','Close plaintext Modbus/HTTP/FTP conduit assumptions and all remaining integrity-protected interface coverage.','TLS,RBAC,LIVE,ARCH'),
('CR3-3','Partial','Baseline checking, tests, health/watchdog and event readout support verification.','Document and qualify complete product security-function verification procedures.','MERGE,TLS-TEST,SAFE'),
('CR3-4','Partial','Signed APK/update policy and reviewed source/configuration checks exist.','Demonstrate boot/rootfs/runtime integrity and full protected-configuration coverage.','UPDATE,PACKAGE,SUPPLIER'),
('CR3-5','Partial','Bounded ASN.1/BER, policy-schema and TLS input validation exist.','Qualify every external industrial and management input, including malformed/flood campaigns.','TLS,RBAC,TLS-TEST,LIVE'),
('CR3-6','Partial','GPIO safe state, permissive gating, PF2 and watchdog mechanisms exist.','Demonstrate all defined process failure states on actual final hardware under relevant security faults.','SAFE,MERGE'),
('CR3-7','Partial','Inspected security paths deny invalid inputs without private-key disclosure.','Audit all management/error paths and confirm no exploitable information or secret leakage.','TLS,RBAC,LIVE'),
('CR3-8','Partial','TLS session integrity and ACSE anti-replay/binding are implemented; unsafe TLS cache resumption is disabled.','Qualify web/SSH session identifiers, logout invalidation and all-interface session behavior.','TLS,RBAC,LIVE'),
('CR3-9','Partial','Audit files use 0640 and application clearing is forbidden.','Protect against privileged/offline modification/deletion and qualify access controls; root-owned append files are not immutable.','LOG,LIVE'),
('CR4-1','Partial','TLS protects application traffic; package exclusions and restricted signing storage protect selected secrets.','Close plaintext management/conduit exposure, private-key lifecycle and all protected data-at-rest requirements.','TLS,UPDATE,KEYS,LIVE'),
('CR4-2','Gap','Credential and backup retention mechanisms exist.','Implement verified secure erasure/decommissioning of retained keys, logs, backups and protected data.','UPDATE,GUIDE'),
('CR4-3','Implemented','Selected application crypto uses TLS 1.2, the four mandatory AES128 suites, SHA256 and strong RSA/P256 policy.','Implementation assessment is scoped to inspected CCLI crypto; key custody and full device claims remain separate/open.','TLS,TLS-TEST,CLOSURE'),
('CR5-1','Gap','Zone architecture and OS firewall configuration interfaces exist.','Deployment gap: live LAN1/LAN2/LAN3 and two Wi-Fi APs share br-lan. Demonstrate intended DSO/operator/plant segmentation and effective IPv4/IPv6 policy; this finding does not mean OpenWrt cannot support segmentation.','ARCH,LIVE'),
('CR6-1','Partial','CLI event readout is available and application audit access is designed.','Demonstrate authorized read-only audit-reader access across complete device interfaces.','LOG,RBAC,LIVE'),
('CR6-2','Partial','Security events, supervision and health monitoring are implemented.','Complete breach monitoring, protected collection, delivery/alerting and response proof across product.','LOG,SAFE,ADAPTER'),
('CR7-1','Partial','Bounded logs, TLS deadlines, parser limits and supervision mitigate selected failures.','Prove essential functions under all-interface floods, handshake storms and degraded resource conditions.','TLS,SAFE,LOG,LIVE'),
('CR7-2','Partial','Queues, event retention and protocol buffers have bounds.','Qualify CPU, memory, thread and flash limits across concurrent security/management operations.','TLS,LOG,SAFE'),
('CR7-3','Partial','Controlled upgrades back up configuration; r71 preservation hashes and restricted recovery archives exist.','Demonstrate complete recoverable product-state backups without disrupting essential operation.','UPDATE,DEPLOY'),
('CR7-4','Partial','Upgrade holds, preservation and recovery snapshots exist.','Prove secure restore/reconstitution after power loss, failed update and platform failure.','UPDATE,DEPLOY,SUPPLIER'),
('CR7-6','Partial','YAML/UCI expose security configuration and strict TLS inputs are validated.','Provide approved whole-device policy, consistent secure defaults and qualified configuration access.','ADAPTER,GUIDE,LIVE'),
('CR7-7','Gap','Application MMS binds to 192.168.0.1:3782; IEC104 is disabled in active profile.','Remove/restrict unnecessary FTP/HTTP/HTTPS/SSH/Wi-Fi and default management access in production; current bench remains broadly exposed.','LIVE,GUIDE,PACKAGE'),
('CR7-8','Partial','Software version, SDK/dependency records and device metadata exist.','Deliver a release-bound complete hardware/software inventory/SBOM; installed binary reports git unknown.','PACKAGE,DEPLOY,LIVE'),
]
for k,*v in COMP: put('COMP-'+k,*v)

ENH = [
('CR1-1-RE1','Gap','Application certificates distinguish identities.','Shared root access does not provide unique identification of every human.','RBAC,LIVE'),
('CR1-1-RE2','Gap','Certificate and password interfaces exist.','No qualified MFA for every human-access interface was located.','LIVE'),
('CR1-2-RE1','Partial','Signed application identities and TLS certificates exist.','Prove unique per-component/device credentials and lifecycle instead of shared lab identities.','RBAC,TLS,KEYS'),
('CR1-5-RE1','Partial','TPM-backed update public trust/counters are supported.','Hardware protection of all relied-upon authenticators is not demonstrated; TLS keys use files.','UPDATE,TLS,SUPPLIER'),
('CR1-7-RE1','Gap','Password access exists on management services.','Password history and configurable minimum/maximum human-password lifetimes are not qualified.','LIVE,GUIDE'),
('CR1-7-RE2','Gap','Password access exists on management services.','All-user/process/device password lifetime policy is not qualified.','LIVE,GUIDE'),
('CR1-9-RE1','Gap','TPM public update trust exists.','Long-lived TLS/application private keys are file inputs; demonstrate required hardware private-key protection if this SL enhancement is claimed.','TLS,UPDATE,SUPPLIER'),
('CR1-14-RE1','Scope pending','PSK authentication is not selected for inspected TLS endpoints.','Decide symmetric-authenticator applicability before judging hardware protection.','TLS,INVENTORY'),
('CR2-1-RE1','Partial','Signed policy authorizes application people/process identities.','Demonstrate least privilege on every OS/management/device interface.','RBAC,LIVE'),
('CR2-1-RE2','Partial','Authorized owner-signed packages define roles and scopes with activation sequencing.','Qualify authorized administrative workflow and complete all-interface role changes.','RBAC'),
('CR2-1-RE3','Gap','DSO control priorities and permissive gating exist.','These do not demonstrate a timed/configurable supervisor security override.','RBAC,SAFE'),
('CR2-1-RE4','Gap','Update authorization uses manufacturer/operator signatures.','General dual-human approval for serious plant actions is not implemented/qualified.','UPDATE,RBAC'),
('CR2-9-RE1','Gap','Bounded log rotation exists.','No configurable approaching-capacity audit warning was demonstrated; wrap notices alone do not close it.','LOG'),
('CR2-11-RE1','Partial','GNSS boot enabling and chrony GNSS source were verified live.','Prove sustained required accuracy, loss/recovery, reboot behavior and clock-daemon coordination; latest CCLI offset sample was -106 ms and still marked unsynchronized.','LIVE,LOG'),
('CR2-11-RE2','Evidence missing','GNSS/chrony provide a time source.','Demonstrate detection/audit of unauthorized source alteration and applicable spoofing/tampering protections.','LIVE,SUPPLIER'),
('CR2-12-RE1','Partial','Application certificate identity is attached to audit actions.','Provide full people/process/device attribution with protected records and unique management identities.','RBAC,LOG,LIVE'),
('CR3-1-RE1','Partial','Mutual TLS and signed ACSE authenticate inspected application traffic.','Close other conduit/management authenticity requirements and final-product coverage.','TLS,RBAC,LIVE'),
('CR3-3-RE1','Partial','Watchdog and health monitoring run online.','Demonstrate verification of every required security function during operation.','SAFE,ADAPTER'),
('CR3-4-RE1','Partial','Manufacturer/operator signatures and signed role policy provide selected authenticity checks.','Demonstrate enforced authenticated boot/rootfs/recovery and all relevant information authenticity.','UPDATE,RBAC,SUPPLIER'),
('CR3-4-RE2','Partial','Integrity/authentication failures generate security events.','Qualify configurable recipient notification and delivery for unauthorized change attempts.','LOG,UPDATE,RBAC'),
('CR3-9-RE1','Gap','Application append-only semantics and bounded rotation exist.','Hardware-enforced write-once audit storage is not demonstrated.','LOG'),
('CR4-2-RE1','Gap','Crypto providers may clear selected internal buffers.','No complete volatile/shared-memory purge implementation/evidence for protected data was located.','TLS'),
('CR4-2-RE2','Gap','No secure-erasure acceptance record exists.','Implement and demonstrate erase verification for all protected persistence.','GUIDE,UPDATE'),
('CR6-1-RE1','Partial','JSONL/CLI readout and optional RFC5424 UDP syslog exist.','Qualify authorized API/centralized collection and protected receiver delivery; active remote syslog is disabled.','LOG,LIVE'),
('CR7-1-RE1','Partial','Bounded buffers and TLS deadlines mitigate selected flood effects.','Provide real all-interface flood mitigation and essential-function measurements.','TLS,LOG,SAFE,LIVE'),
('CR7-3-RE1','Partial','Recovery archives and r71 configuration-preservation hashes were checked.','Prove backup-integrity validation before an actual complete restore, including failed/corrupt backups.','DEPLOY,UPDATE'),
('CR7-6-RE1','Partial','Deployed YAML/UCI settings are machine readable.','Provide authorized complete security-settings reporting with safe secret handling and release consistency.','ADAPTER,LIVE,GUIDE'),
]
for k,*v in ENH: put('COMP-'+k,*v)

# Type-dependent rows keep their source applicability. EDR is the proposed
# controller classification; HDR/NDR classification is not silently asserted.
for p in ORIGINAL['parents']:
    key = p['id']
    if not key.startswith('COMP-') or key in A: continue
    ref = p['reference']
    if '2.4' in ref:
        put(key,'Scope pending','Signed updates are supported; no runtime mobile-code feature is declared.','Document whether mobile code is used and select type/SL; OTA firmware is not automatically the same feature.','UPDATE,INVENTORY')
    elif ref.startswith('SAR3.2'):
        put(key,'Evidence missing','Application runs on OpenWrt and supports signed delivery.','Supplier-qualified compatible malicious-code protection and required configuration documentation were not demonstrated.','PACKAGE,SUPPLIER')
    elif ref.startswith(('HDR','NDR')):
        if ref.startswith('NDR5.'):
            put(key,'Gap','Boundary-control intent and WAN rejection rules exist.','If NDR is claimed, qualify this boundary requirement; the current shared LAN/Wi-Fi bridge and accepting LAN zone do not establish it.','ARCH,LIVE')
        elif ref.startswith('NDR1.6'):
            put(key,'Scope pending','Live Wi-Fi APs use PSK2 and share the LAN bridge.','Decide NDR applicability and demonstrate required wireless/unique-identity authentication; shared PSK is not unique user identity.','LIVE,INVENTORY')
        else:
            put(key,'Scope pending','Platform/application mechanisms may contribute; EDR/SAR scope is currently proposed.','Approve HDR/NDR classification and SL, then qualify this type-specific requirement; no blanket N/A or inherited compliance is assigned.','INVENTORY,SUPPLIER,LIVE')
    elif ref.startswith('EDR2.13'):
        put(key,'Evidence missing','Supplier platform material discusses hardware/debug features.','Provide exact-unit diagnostic interface controls, exceptions/risk assessment, monitoring and access-attempt audit where required.','SUPPLIER')
    elif ref.startswith('EDR3.2'):
        put(key,'Partial','Signed APK/update checks constrain selected installation paths.','Prove prevention of unauthorized installation/execution across rootfs, boot, recovery and privileged management paths.','UPDATE,SUPPLIER,LIVE')
    elif ref == 'EDR3.10':
        put(key,'Implemented','CCLI APK upgrades are supported and r71 installation was observed.','Formal acceptance and full firmware/OS update scope remain open; do not extend APK evidence to BootROM/rootfs.','UPDATE,DEPLOY')
    elif ref == 'EDR3.10-RE1':
        put(key,'Partial','Manufacturer APK and device-bound operator-manifest verification are implemented.','Qualify every allowed software/firmware update path; r71 used explicit temporary manufacturer trust over SSH, not USB/TPM acceptance.','UPDATE,DEPLOY,SUPPLIER')
    elif ref.startswith('EDR3.11'):
        put(key,'Evidence missing','Supplier hardware documents and review exist.','Provide installed tamper resistance/detection, logging/notification and fault/bypass tests appropriate to selected SL.','SUPPLIER')
    elif ref.startswith('EDR3.12'):
        put(key,'Partial','Manufacturer public update trust can be provisioned in TPM NV.','Prove manufacturing provisioning/protection, active-unit root policy and boot-chain enforcement; public-root storage is not proof of the complete trust chain.','UPDATE,SUPPLIER')
    elif ref.startswith('EDR3.13'):
        put(key,'Partial','TPM operator CA/CRL and owner-signed role-policy mechanisms exist.','Qualify owner root provisioning, authorization, lifecycle and hardware/file trust boundaries on actual units.','UPDATE,RBAC,SUPPLIER')
    elif ref.startswith('EDR3.14'):
        put(key,'Evidence missing','Supplier secure-boot descriptions exist; CCLI APK signatures are checked.','Provide exact-unit enforced boot/kernel/rootfs/recovery integrity/authenticity evidence, fuse state and rejection tests.','SUPPLIER,UPDATE')
    else: raise RuntimeError('Unassessed component row: '+key)

# IEC 62351-3 scoped server profile. Normative requirements and task acceptance
# are retained even when project work is deferred or a feature is not selected.
TLS = [
('GEN-01','Implemented','Successful TLS handshakes emit events through MMS/104 observers.','Complete independent event catalogue/receiver acceptance remains open.','TLS,ADAPTER,TLS-TEST'),
('GEN-02','Partial','Active secure MMS uses 3782; IEC104 TLS has an explicit configurable port.','Reconcile each referencing-protocol secure-port declaration and default with the release PICS/PIXIT. Installed IEC104 is disabled, so its shipped-port profile is not accepted by this observation.','ADAPTER,DECL,DEPLOY,LIVE'),
('VER-01','Implemented','Both application server adapters enforce TLS 1.2 min/max.','Formal frozen-product acceptance remains open.','TLS,ADAPTER,TLS-TEST'),
('VER-02','Not selected','The released CCLI MMS/104 paths under review are servers.','A product TLS-client claim needs separate client negotiation/behavior validation; regression clients are not that claim.','ADAPTER,INVENTORY'),
('VER-03','Not selected','TLS 1.3 is disabled in the selected TLS 1.2 profile.','Reassess if TLS 1.3 is selected.','TLS,ADAPTER'),
('VER-04','Implemented','TLS 1.0/1.1 are excluded by the version lock; no legacy compatibility mode is provided.','Do not claim an authorized legacy mode; qualify all clause-specific legacy rejection events.','TLS,ADAPTER'),
('VER-05','Partial','TLS versions below 1.2 cannot negotiate.','Verify complete clause-specific alarm behavior for each prohibited-version proposal on exact target.','TLS,ADAPTER,TLS-TEST'),
('VER-06','Implemented','Initial negotiated version is retained and a renegotiation version change raises an alarm and aborts.','Version transition is unreachable under the current 1.2 lock; independent acceptance remains open.','TLS,CLOSURE'),
('NULL-01','Implemented','The strict configured four-suite list contains no NULL encryption.','Maintain exclusion for the selected encrypted profile and qualify final product.','TLS,TLS-TEST'),
('NULL-02','Not selected','No NULL/integrity-only TLS mode is configurable in this endpoint.','Explicit policy enabling would be required if a future profile adds it.','TLS,CLOSURE'),
('CERT-01','Implemented','PEM CA bundles support at least five roots; host regression authenticates through the fifth root.','Retain independent exact-build acceptance; a single bundle path is not a single-anchor limitation.','TLS-TEST,ADAPTER,MERGE'),
('CERT-02','Implemented','8192-byte DER local/peer certificates are accepted and 8193-byte certificates rejected with size alarm; limit is documented.','Publish consistent final release manuals and independent exact-target boundary acceptance.','TLS,TLS-TEST,CLOSURE'),
('CERT-03','Implemented','Mutual certificates, local key/cert pairing and missing endpoint/peer failure events are enforced.','Final all-role/product acceptance remains open.','TLS,ADAPTER,TLS-TEST'),
('CERT-04','Implemented','Mandatory CA-chain validation plus optional exact-certificate filtering and local identity validation are configured.','PKI lifecycle and exact-release acceptance remain open.','TLS,ADAPTER,TLS-TEST'),
('CERT-05','Implemented','With no leaf allowlist, adapters can require CA validation alone; with pins they add restriction to chain validation.','Qualify all chosen trust profiles and failure-event wording on final release.','ADAPTER,TLS-TEST'),
('CERT-06','Implemented','Configured exact-certificate pins reject unlisted peers and raise the authorized-CA individual-certificate alarm.','Retain independent acceptance for deployed allowlists.','TLS,ADAPTER,TLS-TEST'),
('REV-01','Partial','Configurable periodic CRL checking and certificate revalidation are implemented on both servers.','Complete referenced ISO/X.509 procedural coverage; OCSP processing is deferred and distribution/issuer lifecycle still needs qualification.','TLS,ADAPTER,CLOSURE'),
('REV-02','Implemented','Connection-owned certificate/CRL state is rechecked; revoked identities are disconnected without a positive grace period.','Qualify long-duration/exact-target behavior independently.','TLS,TLS-TEST,CLOSURE'),
('REV-03','Implemented','Revoked peers are refused and active revocations terminate sessions with separate revocation/termination alarms.','Retain exact-release independent acceptance.','TLS,TLS-TEST'),
('REV-04','Implemented','Authenticated local-file refresh, at-least-daily configurable evaluation, warnings and established-session continuity are implemented.','CRL delivery/renewal is manual; startup needs the applicable current CRL for new handshakes. Persistent global rollback protection and independent acceptance remain open.','TLS,TLS-TEST,CLOSURE'),
('REV-05','Deferred','CRL path works; stapled OCSP response processing is absent.','User deferred OCSP. Keep the standard obligation open where applicable; project deferral is not standards N/A.','CLOSURE,INVENTORY'),
('REV-06','Deferred','No OCSP cache/responder operation is implemented.','User deferred OCSP; if used later, implement expiry/nextUpdate, <=24h cache and failure warnings.','CLOSURE,INVENTORY'),
('CERT-07','Implemented','Expired local/peer certificates are rejected and active certificate validation covers renegotiation.','Qualify final product and all required event paths independently.','TLS,TLS-TEST'),
('CERT-08','Partial','Signature/key-policy checks and certificate validation failures raise security events.','Verify exhaustive algorithm-mismatch and signature-verification alarm mapping on exact target, not merely handshake rejection.','TLS,ADAPTER,TLS-TEST'),
('12-DISALLOWED','Partial','Only the four approved suites can negotiate; MD5/DES/NULL choices are excluded.','Qualify all mixed/only-disallowed offers and required clause-specific alarms; product client obligations are not claimed.','TLS,TLS-TEST'),
('12-KEX','Implemented','RSA, DHE_RSA and ECDHE_RSA/ECDSA mandatory key-exchange suites are built/configured.','Final independent conformance remains open.','TLS,TLS-TEST'),
('12-ALG-01','Implemented','RSA >=2048, P256 ECDSA/SHA256 and required SHA256 signature policy are implemented.','Retain independent final-build certificate/signature algorithm coverage.','TLS,TLS-TEST'),
('12-ALG-02','Implemented','All RSA below 2048 is rejected by default with required alarm; no RSA1024 compatibility mode is selected.','Do not claim a legacy enabling mode; additional legacy warnings apply only if later implemented.','TLS,TLS-TEST'),
('12-ALG-03','Implemented','RSA below 1024 is excluded by stricter minimum key policy and length alarms.','Independent exact-target acceptance remains open.','TLS,TLS-TEST'),
('12-ALG-04','Implemented','SHA256 stays enabled; SHA384/SHA512 are policy-controlled; SHA1 warns and is rejected.','Qualify every selected hash/signature combination and formal declaration.','TLS,TLS-TEST,CLOSURE'),
('12-RES-01','Gap','A resumption timer is parsed/validated, but cfg->useSessionResumption is forced false.','Implement safe peer identity/revocation restoration and required active-session resumption; timer presence is not feature implementation.','TLS,CLOSURE'),
('12-RES-02','Not selected','Ended-session resumption is disabled; connections use full handshakes.','Implement expiry/full-handshake notices if resumption is enabled; active resumption remains a separate gap.','TLS,CLOSURE'),
('12-RES-03','Not selected','No ticket-based resumption is selected.','If tickets are enabled later, enforce <=24h lifetime and test expiry.','TLS,CLOSURE'),
('12-RES-04','Not selected','Same-connection resumption is disabled; full secure renegotiation is supported.','Reassess cached-session/verify_data binding when resumption is implemented.','TLS,CLOSURE'),
('12-REN-01','Implemented','600..86400-second configurable full renegotiation, 12h default, monotonic idle ticks and certificate revalidation are implemented.','Accelerated tests do not replace actual 12h/24h independent final-product endurance acceptance.','TLS,TLS-TEST,CLOSURE'),
('12-REN-02','Implemented','Server requests renegotiation and aborts incomplete requests after 30s; RFC5746 strict behavior is configured.','Scope is released servers; qualify client initiator obligations if client capability is claimed.','TLS,TLS-TEST,ADAPTER'),
('12-EXT-01','Partial','RFC5746 secure renegotiation and legacy renegotiation rejection use provider enforcement.','Complete initial-extension-warning and all missing/changed verify_data alarm tests against the exact released provider.','TLS,TLS-TEST'),
('12-EXT-02','Not selected','CA bundles work; optional trusted_ca_keys extension is not selected.','Do not equate CA validation with this extension; implement encoding/selection events if selected.','TLS,ADAPTER'),
('12-EXT-03','Implemented','Strict ClientHello signature_algorithms is checked before server flight; SHA1 fallback is warned/rejected.','Released endpoints are servers; client offer behavior requires separate product-client qualification if claimed.','TLS,TLS-TEST'),
('12-EXT-04','Deferred','No status_request/CertificateStatus stapling path is implemented.','User deferred OCSP. The source inventory marks this TLS1.2 requirement mandatory; it remains a gap to an unrestricted 2023 conformance claim.','CLOSURE,INVENTORY'),
('12-EXT-05','Deferred','No OCSP multistapling is implemented or selected.','User deferred OCSP; conditional multistapling obligations activate if selected.','CLOSURE,INVENTORY'),
('12-EXT-06','Implemented','CBC AES128 requires RFC7366 EtM; non-EtM negotiation is refused before application data.','Retain independent final-build negative acceptance.','TLS,TLS-TEST'),
('PIN-01','Not selected','Exact-certificate pins and CA validation exist.','Standardized IEC62351-9 serial+issuer CertAVL support is not demonstrated/claimed; exact DER pinning alone does not prove it.','ADAPTER,CLOSURE'),
('CONF-01','Partial','TLS closure and draft PID enumerate features.','Publish a consistent 2023 PICS/PIXIT/client-server declaration; older D7 edition, CRL optional and provider statements are stale.','CLOSURE,DECL,DEPLOY'),
('OPT-13-ENABLE','Not selected','Current TLS profile deliberately permits only 1.2.','TLS1.3 is optional overall; select it explicitly before its conditional profile is judged.','TLS,ADAPTER'),
('OPT-OCSP-QUERY','Deferred','Direct online OCSP retrieval is absent.','User deferred it; this optional querying decision does not remove mandatory stapled-response gaps.','CLOSURE,INVENTORY'),
('OPT-TRUSTED-CA','Not selected','CA-bundle validation works without trusted_ca_keys indication.','Optional extension selection and multi-domain tests remain a future feature decision.','ADAPTER'),
('OPT-SNI12','Not selected','Current server profile uses a fixed local identity.','SNI product support/selection is not claimed or qualified.','ADAPTER'),
('OPT-COOKIE13','Not selected','TLS1.3 is not selected.','Reassess optional cookie behavior if TLS1.3 is selected.','TLS'),
('OPT-SNI13','Not selected','TLS1.3 is not selected.','Reassess optional SNI behavior if TLS1.3 is selected.','TLS'),
('OPT-CA13','Not selected','TLS1.3 is not selected.','Reassess optional CA indication if TLS1.3 is selected.','TLS'),
('OPT-TIMER-DEFAULTS','Partial','12h full-renegotiation default and 1h CRL checking default are configured; 6h resumption value is retained.','Six-hour resumption is not operational because safe resumption is disabled.','TLS,ADAPTER,CLOSURE'),
('OPT-CRL-REFRESH','Partial','Local file reload/check occurs at a configurable interval and checks validity.','Authenticated network distribution and automatic renewal are absent; operator must atomically replace current issuer bundles before nextUpdate.','TLS,CLOSURE,LIVE'),
('OPT-EVENT-EXPORT','Partial','Local events and optional generic RFC5424 UDP syslog are implemented.','IEC62351-14/-7 export is not demonstrated; remote syslog is disabled in active profile.','LOG,ADAPTER,LIVE'),
]
for k,*v in TLS: put('TLS-'+k,*v)

for p in ORIGINAL['parents']:
    key=p['id']
    if key in A: continue
    if key.startswith('TLS-13-'):
        put(key,'Not selected','TLS1.3 and its suites/groups/signature/extension profile are disabled.','Conditional TLS1.3 obligations remain inventoried and must be implemented/tested if selected; no TLS1.3 conformance is claimed.','TLS,INVENTORY')
    elif key.startswith('TLS-12-SUITE-'):
        if key.endswith(('0x003C','0x009E','0xC02F','0xC02B')):
            put(key,'Implemented','This mandatory suite is in the strict configured list and positive negotiation campaigns.','Required support is demonstrated by engineering tests; independent exact-product acceptance remains open.','TLS,TLS-TEST')
        else:
            put(key,'Not selected','Optional/conditional NULL or AES256 suite is not in the four-suite profile.','No support claim is made; preserve explicit policy selection and required conditions if added later.','TLS,INVENTORY')
    else: raise RuntimeError('Unassessed family: '+key)

IDS={p['id'] for p in ORIGINAL['parents']}
assert set(A)==IDS, (set(A)-IDS, IDS-set(A))
assert len(A)==289
for a in A.values():
    assert all(e in E for e in a['evidence'])

assessed=deepcopy(ORIGINAL)
assessed['reassessment']={
 'date':DATE,'source_commit':HEAD,'branch':'codex/owner-sync-20261027',
 'method':'Source and saved engineering evidence review, current protected-source baseline check and read-only Tespro inspection. No full conformance campaign was rerun.',
 'application_scope':'apps/ccli personal legacy entry, r71; optional owner program/ and product/apps/ccli are not presumed equivalent.',
 'runtime_binary_sha256':'76f8ba487ec2acb62a509fa9bbcd6befa8c289ee274ac7a5ddfa83a7da77a6cf',
 'runtime_profile_sha256':'0eadbbde0e33343403305a26a3c3ad485c74f41c98199f8d13d4b59e93954482',
 'ocsp':'Deferred by user; standards obligations retained, mandatory stapling not declared compliant.',
 'task_acceptance':'Original delivery states retained. Parent engineering assessment inherited by tasks for triage; it does not independently close every subtask.',
 'sl_and_type':'SL-C vector unapproved; SAR/EDR proposed; HDR/NDR and feature conditions require applicability decisions.'}
for p in assessed['parents']:
    p['previous_local_assessment']=p['local']
    p['previous_owner_only_assessment']=p['upstream']
    p['upstream']='Historical owner-only comparison superseded for the personal merged baseline; see reassessment metadata.'
    p['engineering_assessment']=A[p['id']]
    p['local']=A[p['id']]['status']+': '+A[p['id']]['implemented']
for task in assessed['tasks']:
    task['previous_local_assessment']=task['local']
    task['previous_owner_only_assessment']=task['upstream']
    task['upstream']='Historical owner-only comparison superseded for the personal merged baseline; see reassessment metadata.'
    task['engineering_assessment']=deepcopy(A[task['parent']])
    task['local']=A[task['parent']]['status']+': '+A[task['parent']]['implemented']
    task['assessment_granularity']='Family-level triage; individual task acceptance remains open.'
assessed['evidence_catalog']={k:dict(description=v[0],paths=v[1]) for k,v in E.items()}

states=['Implemented','Partial','Gap','Evidence missing','Deferred','Scope pending','Not selected']
by_standard={s:Counter(A[p['id']]['status'] for p in ORIGINAL['parents'] if p['standard']==s) for s in ORIGINAL['summary']['by_standard']}
assessed['assessment_counts']={s:dict(c) for s,c in by_standard.items()}
target=OUT/'register-assessed-2026-10-28.json'
target.write_text(json.dumps(assessed,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')

def cell(value): return str(value).replace('|','/').replace('\n',' ')
lines=[
 '# CCLI compliance reassessment — 28 October 2026', '',
 f'Source commit: `{HEAD}`. Installed CCLI: **0.1.0-r71** at **192.168.0.1**. This review supersedes the old implementation-status statements in the 26 October register; its normative requirement inventory is retained.', '',
 '**Result:** the selected TLS1.2 server mechanisms have substantial implementation and engineering-test evidence. Safe session resumption and deferred OCSP remain gaps. IEC62443-4-1 organizational conformity and IEC62443-4-2 whole-controller/SL conformity are not demonstrated. No requirement is certified or automatically closed by this reassessment.', '',
 'The report assesses all **289 families** and preserves all **531 original tasks**, all conditions, SL enhancements and original task delivery states. Family assessments are engineering triage; a partial family does not mean every subtask is missing or implemented. The JSON retains every task definition with the family evidence and remaining work.', '',
 '## Current baseline and evidence limits', '',
 '- Current source includes owner merge `bbff9a4` and I1-profile correction `15b0a9c`. The earlier owner-only revision is no longer the personal product baseline.',
 '- Installed r71 binary hash matches its deployment record. Its build metadata says git unknown and uses the SDK SOURCE_DATE_EPOCH; use the recorded artifact/binary/source hashes rather than the embedded build date as traceability.',
 '- Installed active profile hash matches the committed personal profile. TLS/Profile A and the 23-node CFG remain the deployed application profile; IEC104 is implemented/tested as a component but disabled in this installed profile.',
 '- Source baseline validation was executed now and passed. Existing 19-test merged-host results, earlier isolated ARM64 TLS campaigns and r71 installed MMS evidence were reviewed; they were not rerun as a new exhaustive product conformance campaign.',
 '- The optional owner program entry and product mirror are separate qualification targets; evidence for apps/ccli is not automatically assigned to them.',
 '- Scope/type and target FR security-level vector still need approval. An enhancement gap matters when its condition/SL is selected; an unselected optional feature is not counted as a mandatory failure.',
 '- OCSP is deferred under the user instruction. The source inventory identifies TLS1.2 stapling as mandatory; project deferral does not remove the resulting gap from an unrestricted standard conformance claim.',
 '- Native IEC62351-4 application E2E remains disabled on Tespro and is a separate layer; its activation state is not used to decide IEC62351-3 transport compliance.', '',
 '## Assessment meanings', '',
 '| State | Meaning |','|---|---|',
 '| Implemented | Inspected scoped code implements the capability, with referenced evidence. Independent final-product acceptance remains open. |',
 '| Partial | Some mechanism or evidence exists; a specific behavior, interface, deployment or approval gap remains. |',
 '| Gap | Functionality is absent in the inspected path or live configuration conflicts with the requirement/assumption. |',
 '| Evidence missing | No accepted organizational or exact-platform proof was located. This does not establish the hardware is incapable. |',
 '| Deferred | User has deferred this implementation. Applicable mandatory obligations remain open. |',
 '| Scope pending | Component type, SL or feature applicability needs an approved decision. |',
 '| Not selected | Optional feature/unclaimed role is disabled or not claimed in the selected profile; reassess when selected. |', '',
 '## Family counts — not compliance percentages', '',
 '| Standard | '+' | '.join(states)+' | Total |',
 '|---|'+'---:|'*(len(states)+1),
 ]
for standard,c in by_standard.items():
    lines.append('| '+standard+' | '+' | '.join(str(c[s]) for s in states)+f' | {sum(c.values())} |')
lines += ['',
 '## Confirmed progress since the old review', '',
 '- Required four TLS1.2 suites, strict encryption/key/signature policy, CA bundles, mutual certificates, 8192-byte certificate limits and individual certificate filtering now have explicit code/regression evidence.',
 '- Both MMS and IEC104 now validate local/peer identities and configured CRLs. Periodic authenticated local-file refresh, active revocation checks, failure warnings and established-session continuity are implemented.',
 '- Configurable 10-minute-to-24-hour full renegotiation, a monotonic idle tick and a 30-second completion deadline replace the older disabled-timer gap.',
 '- Unsafe cached TLS resumption is deliberately disabled; this contains an identity/revocation bypass but leaves the resumption feature unimplemented.',
 '- r71 has been signed, verified and installed, with 236 preserved non-event-log configuration/credential checks and successful installed signed-ACSE/mutual-TLS reads/browse. This is not a complete USB/failure/recovery qualification.',
 '- Current tracked PEM/key candidates have no private-key PEM markers; package preparation excludes live TLS private inputs. No claim is made that historical copies/credentials were revoked or removed from old commits.',
 '- GNSS is now enabled persistently and chrony uses GNSS. At the read-only snapshot, CCLI still flagged its -106 ms sample outside the configured +/-100 ms quality threshold; sustained accuracy and reboot/source integrity remain open.', '',
 '## Priority closure work', '',
 '| Priority | Concrete finding | Required closure | Related families |',
 '|---|---|---|---|',
 '| P1 operations | Installed root-CA CRL nextUpdate is **29 October 2026 19:36:10 UTC (20:36:10 Europe/Rome)**. RBAC-CA CRL nextUpdate is **2 November 2026 09:04:57 UTC**. Both were read from device public CRLs. | Publish authenticated replacements atomically before expiry and qualify renewal/distribution. Freshness is required on new handshakes; established-session continuity does not extend new-handshake acceptance. | TLS-REV-01/04; TLS-OPT-CRL-REFRESH; CR1.8/1.9 |',
 '| P1 deployment | FTP21, HTTP80, HTTPS443 and root-password SSH22 listen broadly; two PSK2 Wi-Fi APs are enabled. These are bench observations, not an approved production hardening baseline. | Restrict management, remove unnecessary services, use unique least-privilege accounts/approved authentication and qualify remaining attack surface. | CR1.1/1.7/1.11; CR2.2; CR7.7 |',
 '| P1 network | LAN1/LAN2/LAN3 and two Wi-Fi APs share br-lan; LAN INPUT/FORWARD accept. Separate IP aliases do not create separate security zones. | Enforce tested DSO/operator/plant/management conduits and effective IPv4/IPv6 controls. Isolate or protect plaintext Modbus and other allowed protocols. | CR3.1/4.1/5.1; conditional NDR5.2 |',
 '| P1 TLS feature | Safe session resumption is not implemented although its timer is validated. | Restore/revalidate cached peer identity and revocation state safely, then test required resumption behavior. | TLS-12-RES-01 |',
 '| Deferred TLS feature | OCSP stapling/response processing/cache are absent under user deferral. | Retain as deferred obligations; resolve before an unrestricted 62351-3:2023 conformance claim. Direct online querying remains a separate optional recommendation. | TLS-REV-05/06; TLS-12-EXT-04/05 |',
 '| P1 platform evidence | APK signatures and TPM update trust do not prove authenticated boot/kernel/rootfs/recovery, fuse enforcement, protected debug or installed tamper sensing. | Obtain exact-unit supplier evidence and positive/negative target tests for the selected type/SL. | EDR3.11-3.14 and relevant REs |',
 '| P1 lifecycle | No complete approved/executed SDL, independence or disclosure/patch policy evidence. | Assign owners, approve processes and retain actual lifecycle/release records, not only code and templates. | All 47 SDL families |',
 '| P2 PKI/account lifecycle | Shared bench root authentication and historical private-key exposure are not closed by current-tree cleanup. | Complete exposure assessment, controlled credential/trust migration where needed and per-device/user lifecycle evidence. | SM7/SM8; CR1.3-1.5/1.8/1.9 |',
 '| P2 declaration | Older D7 PID includes older edition/provider/profile statements that disagree with the current strict mbedTLS policy. | Publish consistent 2023 PICS/PIXIT with selected server roles, suites, extensions, disabled features and evidence. | TLS-CONF-01; SG1/SG3/SG7 |',
 '| P2 validation | Logs are bounded/root-owned but mutable by root; complete event coverage, protected export, storage faults, all-interface DoS, erase and recovery evidence remain incomplete. | Execute a frozen-product security test/independent review plan and retain results for each applicable requirement. | CR2.8-2.12/3.9/4.2/6.x/7.x; SVV |', '',
 '## Detailed reassessment', '',
 'Every family below retains the source clause and applicability condition. Evidence keys refer to the catalog at the end. Delivery acceptance remains open in every row.', '',
 ]
for standard in ORIGINAL['summary']['by_standard']:
    lines += ['### '+standard,'','| ID / clause | Requirement | Applicability / minimum SL | Assessment | Implemented/evidence | Remaining gap or acceptance | Evidence |','|---|---|---|---|---|---|---|']
    for p in ORIGINAL['parents']:
        if p['standard']!=standard:continue
        a=A[p['id']]
        condition=p['condition']+(('; min SL '+str(p['min_sl'])) if p.get('min_sl') else '')
        lines.append('| '+' | '.join(map(cell,[p['id']+' / '+p['clause'],p['title'],condition,a['status'],a['implemented'],a['remaining'],', '.join(a['evidence'])]))+' |')
    lines.append('')
lines += ['## Evidence catalog','','Paths are repository-relative. Saved campaign and supplier records retain their own dates and limits; a historical PASS is not represented as a newly executed test.','']
for key,(description,paths) in E.items():
    lines += ['**'+key+' — '+description+'**','']
    for path in paths: lines.append('- `'+path+'`')
    lines.append('')
lines += ['## Review limits and acceptance','','No fuse programming, trust rotation, service hardening, firmware change, protocol control/write or new all-clause conformance campaign was performed during this reassessment. Read-only device observations support configuration findings. The source-standard interpretation inventory was reused rather than re-OCRing every normative page. Applicable referenced-standard dependencies, approved scope/SL decisions, exact frozen-release tests and organizational/supplier approvals remain necessary before closing acceptance.','']
(OUT/'COMPLIANCE_STATUS_2026-10-28.md').write_text('\n'.join(lines),encoding='utf-8')

missing=[path for _,paths in E.values() for path in paths if not (ROOT/path).exists()]
# LIVE/KEYS are written as a final evidence step after collecting safe output.
missing=[x for x in missing if '/reassessment-2026-10-28/' not in x]
assert not missing,missing
print(json.dumps({'source_commit':HEAD,'families':len(A),'tasks':len(assessed['tasks']),'counts':assessed['assessment_counts'],'outputs':['COMPLIANCE_STATUS_2026-10-28.md',target.name]},indent=2))

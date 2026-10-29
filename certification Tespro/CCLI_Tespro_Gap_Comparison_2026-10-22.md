# Tespro evidence compared with the CCLI product

Review date: 22 October 2026, Europe/Rome.

## Decision

The supplied material is useful supplier input, but it does not close CCLI's product certification or cybersecurity gaps. Our application has substantially more demonstrated functionality than the supplier's draft documents prove about the platform. Continue software development, but keep production acceptance open until the exact hardware and firmware baseline, secure boot, identity lifecycle and final-product test evidence are established.

Separate three acceptance tracks: Tespro platform evidence; HiTEKS/CCLI implementation and integration; independent testing of the finished CCI. Neither TPM certificates, laboratory accreditation nor a successful TSP connection substitutes for all three.

This is a read-only document/source/evidence review. No DUT connection, reboot, flash, fuse programming, provisioning, installation or fresh conformance test was performed. Recorded device results are dated evidence, not a live-state assertion. No private keys were read for this review. The original supplier documents are unchanged.

## Evidence baseline

- Reviewed all 7 PDFs (123 pages in total), the ISO 9001 certificate image, all 11 DOCX files, the ZIP inventory/integrity, and the saved CNAS scope HTML and relevant embedded scope page. Text was extracted from all PDFs; the scanned CNAS certificate and selected certificate/datasheet pages were visually inspected. This is a technical/content audit, not a visual inspection of every page.
- ZIP CRC check passed. All 11 extracted DOCX files match their ZIP copies byte-for-byte. `__MACOSX`, `.DS_Store` and the Word lock file are packaging debris, not evidence.
- Ten technical DOCX documents are v0.1 drafts/templates. Only the overview is v1.0. The ABI letter and TPM statement are not completed signed commitments; release hashes, exact versions and signatures remain blank.
- Latest installation evidence found: CCLI **0.1.0-r64**, 21 October, APK SHA-256 `2694837eed4cdd59e00335e31e6b5508b3b050a4a4cac02906de82a9af380efb`. It was installed over **SSH**. This does not establish a USB installation test for r64.
- Current source includes a 22 October Aggregator follow-up beyond that installed r64. Keep deployed behavior and source-only fixes separate.
- Recorded platform: TesproOS `32-2.1.0 r601-af70fcaaa2`, Linux `6.12.68`, TR500/MT7987A SPI-NOR lab board; project paths also use TG544/TG500 and older planning uses TG-524. Obtain a signed cross-reference to the final SKU and hardware revision.
- Current local application baseline uses CEI 57-142:2026-07, with references to CEI 0-16:2026. Older Annex O/T extracts and planning matrices require edition reconciliation before they become a final contractual test list.

## Comparison and ownership

| Area | What Tespro supplies | What we have | Remaining acceptance gap and owner |
|---|---|---|---|
| Final-product certification | Unsigned scope allocation, lab accreditation, TPM component records | CCI application, conformance plans, lab evidence | No TG500 safety/EMC CB certificate or associated product reports were found in this package. Final CCI certificate/report baseline, CE documentation and applicable tests remain to be assembled. **Tespro + HiTEKS + lab.** |
| Exact image and SDK | Draft image naming/version lock and matching SDK promises | Signed/hash-identified r64 APK; SDK-based builds and deployment receipts | No delivered platform image, recovery image, SDK archive, full manifest, SBOM or buildinfo in the supplier ZIP. Bind them to the actual board/OS revision, not just the branch name. **Tespro; HiTEKS integrates.** |
| ABI stability | Promises kmods work across all patch releases | CCLI mainly uses user-space APIs and platform ubus/HAL integration | Same source commit is insufficient without matching config, patches, feeds and build inputs. SDK document also says cross-release installs fail, conflicting with the ABI letter. Require separate user-space/kmod policies and demonstrated compatibility. **Tespro.** |
| IEC 61850 coexistence | Draft priority/yield/restart policy around TCP 3782 | Our own MMS server owns 3782; r64 TLS, signed association, control/read/report checks | Verify installed vendor packages/services and actual init-script behavior. No service code or test logs accompany the policy. Add boot/start/runtime/release/IPv4/IPv6/resource-contention tests. **Tespro implements; HiTEKS verifies.** |
| Secure boot | Planned eFuse production state; special vendor U-Boot build required | Manufacturer-signed APKs and TPM-held public update trust | No evidence of enforced BootROM-to-rootfs verification on our board, production fuse state, locked bypasses or authenticated recovery. Signed application installation does not establish trusted boot. **Tespro/module vendor, with HiTEKS acceptance.** |
| Updates and rollback | Generic FIT/usign instructions | Canonical `apps/firmware-updater`: operator-signed device-bound manifest, manufacturer APK verification, TPM trust/counter, replay tracking, preservation checks | Validate full USB path for the exact accepted release and failures/recovery. Transaction replay protection and APK upgrade-only policy do not prove bootloader/OS anti-rollback. Define signed version policy across all layers. **Shared.** |
| TPM certification | SLB9672/SLB9673 CC report; FIPS policy for specific versions | Public updater trust stored in TPM NV | Read out exact TPM firmware and part on accepted units; map to certified configuration and approved-mode requirements. Overall FIPS Level 2 is not overall Level 3. Component boundary is not whole gateway/CCI. **Tespro + lab.** |
| IDevID and private-key use | Explicitly says IDevID is not factory-provisioned; proposes enrollment | EJBCA-issued role/transport certificates and public trust in TPM; recorded server key remains PEM-based | Choose whether IDevID is required for our declared onboarding/product profile. If required, implement manufacturing CA/profile, TPM signing, uniqueness, lifetime/rework rules and proof of possession. Independently define hardware protection of operational TLS/ACSE keys. **Shared.** |
| TLS and RBAC | No CCLI conformance evidence | 19 commissioned identities connected; 143 live read/discovery/auth checks and 298 isolated role checks in 19 October evidence; r64 adds Aggregator live control/dataset/report checks | Re-run full applicable matrix against one frozen product build. Complete security administration/audit rights and applicable SCL/PICS mapping. Older statement that only Viewer/DSO exist is superseded. **HiTEKS + lab.** |
| Native IEC 62351-4 E2E | Not supplied | TLS and signed ACSE identity authentication | SecPDU/ClearTransfer, native handshake/release, integrity/sequence/replay and both rekey directions remain open in the recorded audit. These are distinct from TLS and signed association. Freeze required server scope; optional EncrTransfer must not be labeled unconditionally mandatory. **HiTEKS + lab.** |
| PKI lifecycle | No operational lifecycle implementation | EJBCA issuance, explicit leaf pins, CRL validation and negative tests | Device enrollment/renewal/key replacement, required OCSP/nonce behavior, validated CRL generation/distribution/rollback prevention and safe reload remain open. No automated lifecycle closure shown. **HiTEKS/operator infrastructure.** |
| Logging | No integrated CCI evidence | Persistent 2048-event view, bounded rotation/performance fix, update/security events | Remote sender still uses UDP `SOCK_DGRAM`, not syslog TLS. Complete event/severity catalogue, secure export, access/tamper protection and retention/power-failure evidence. **HiTEKS.** |
| Tamper detection | TI DRV5032 datasheet | Enclosure/seal planning; no integrated Hall-sensor path found in reviewed active CCLI source | Freeze sensor ordering code, GPIO/wiring, magnet/enclosure geometry, bypass/fault behavior and logging/notification tests if required by the selected profile/security level. A datasheet proves no installed detector. **Tespro/mechanical integrator + HiTEKS.** |
| Functional CCI behavior | Generic gateway platform | PF2/DSO paths, MMS model, r64 Aggregator interface; physical outputs intentionally disabled | Healthy telemetry, complete signed P/Q regulation, allocation/priority, communications-loss behavior, real feedback and settling tests remain open. r64 accepted commands/readback do not prove physical inverter response. **HiTEKS + plant/test team.** |
| Platform reliability and product hardware | No closure evidence in this package | GNSS startup repair and lab I/O experience | Close intermittent pre-Linux BL2 failures with supplier root cause; verified recovery/backup; cold-boot timing, holdover, required I/O, power autonomy, environmental/EMC/insulation and network-zone evidence for exact SKU. **Shared.** |

## Specific problems in the supplier package

### Certification scope and standards

The allocation statement correctly separates platform and finished-product responsibility, but it asserts platform conformity without identifying the underlying certificate number, reports, hardware revision or firmware hash. Request those documents before treating that baseline as certified. Its signature/confirmation lines are blank.

Its standards table is not adequate for our CCI path. It categorically excludes the IEC 61557 series for some measurement configurations and contains `IEC 62053-12` references that need correction/identification. It also introduces an EV charging-controller route; our CCI is a plant controller. IEC's published IEC 61557-12 scope explicitly addresses power metering and monitoring devices. Agree applicability using the finished CCI's functions and controlled CEI references with the lab; do not infer that a gateway's 62368-1 path discharges the finished CCI requirements.

The provided A2LA scope 4674.01 lists relevant EMC methods and 62368-1 safety with exclusions, but no 61557-12, 61010-1, 61010-2-201, IEC 61850 or IEC 62443 entry was found in that supplied scope. The saved CNAS testing-scope page likewise contains no 61557/61010 match. A saved dynamic page may be incomplete. Obtain a complete current scope covering each proposed test, its edition, site and limitations; otherwise arrange another qualified laboratory. These findings do not claim the laboratory lacks other accreditations or that the certificates are forged.

The ISO 9001 image is Tespro's management-system certificate 45524Q0809R0S, issued 19 August 2024, nominal validity to 18 August 2027. Its stated scope is design of smart-city energy-management solutions and development/sales of electrical-energy-metering test instruments. Obtain registry/surveillance confirmation and clarify coverage of gateway manufacturing and the actual factory. It is not a TG500 product certificate.

### TPM certification limits

The CC report is BSI-DSZ-CC-1278-2026, dated 23 February 2026, EAL4 augmented by ALC_FLR.1 and AVA_VAN.4. SLB9673 firmware `26.25.19812.00` is among its evaluated versions. The official Common Criteria portal publishes a matching report. This supports the identified component/configuration only.

NIST CMVP record **4467** matches the supplied FIPS policy/module and firmware family. On this review date the record is **Historical**, reason **sunsetting**. Its **overall level is 2**; Physical Security and EMI/EMC are Level 3 exceptions. It is a single-chip module, conditional on the prescribed FIPS mode/initialization. This is not an overall Level 3 gateway certificate. Historical status does not by itself determine Italian CCI acceptance; obtain the laboratory's explicit decision against the current hardware-cybersecurity requirement, required boundary and accepted validation scheme. The older project Annex O extract expects at least FIPS 140-2 Level 3: that interpretation/edition must be resolved rather than silently equating a Level 3 exception with overall Level 3.

### Secure boot instructions need correction and demonstration

- The secure-boot guide identifies `/etc/uhttpd.key etc.` as runtime update trust. A web-server private key path is not an adequate definition of the firmware verification trust anchor. Require the actual verifier, algorithm, trusted public-key location, update format and rejection path.
- The supplied `fit_check_sign -f ... -s <pubkey>` examples do not match the documented U-Boot example, which uses `-k` with the trusted control DTB. Ask for commands tested against the exact delivered tool versions and actual signed images.
- `sysupgrade -T` is not standalone proof of signature enforcement. Prove metadata checking, cryptographic verification and refusal of unsigned/altered images on this target.
- Rootfs is described as external squashfs. Prove its actual authenticated coverage at boot and, if necessary, verified reads; signing only a kernel/DTB does not establish rootfs integrity.
- One document says failure halts boot/signed recovery; another permits a serial prompt. Resolve production console, alternate boot, recovery and download policies, and demonstrate that they cannot bypass authentication.
- Anti-rollback is described as supported with policy to agree; it is not demonstrated. The fuse status is explicitly planned and vendor-dependent. NXP-HAB terminology mapping does not prove equivalent achieved assurance on MediaTek.

Do not execute these draft instructions on the current lab device. Production fuse programming requires a validated vendor procedure and a separately authorized manufacturing operation.

### IDevID procedure is not executable acceptance evidence

- `tpm2_readpublic -C e ...` is not the documented object-read syntax (`-c` for an actual loaded/persistent object), and does not retrieve an EK certificate. Correct EK creation/readout and certificate NV/chain validation are needed.
- The shown hierarchy-auth command does not match documented `tpm2_changeauth -c ...` usage; subsequent commands also omit handling the owner/key authorization that the procedure says to set.
- `tpm2_sign ... -t plain` misuses `-t`, which is a validation-ticket input. Restricted-key signing requires proper digest/ticket handling and a defined signature output format.
- A persistent handle and NV index with `ownerwrite` are not inherently write-once. Define authorization, lock/undefine/evict controls and rework ownership; reconcile with our existing updater trust indexes before commissioning.
- Key attributes, OpenSSL TPM-provider key URI/authentication, certificate profile/OIDs and device-unique identity need a tested end-to-end example. The text says `IEEE 8126 MUD`; MUD is RFC 8520, and BRSKI is RFC 8995. Neither is established as a mandatory CCLI feature by the supplied draft.

The positive point is that Tespro explicitly admits standard units lack factory IDevID. Choose a production enrollment strategy if required; do not relabel an EK certificate or role certificate as an IDevID.

### Coexistence and ABI promises need measurable acceptance

The policy says Tespro's service does not listen on 3782, yet stops that separate service whenever our listener appears. This is an intentional priority/exclusion rule, not demonstrated concurrent operation. It can be acceptable if agreed, but document the functionality lost while CCLI runs. A 60-second patrol also needs an agreed transition delay. Require the actual service scripts, persistence behavior, logs and repeatable tests, rather than accepting the draft pseudocode as implementation.

The ABI letter guarantees every patch kmod works, while SDK matching says cross-release installation fails. Resolve this conflict. Kernel vermagic, symbol versions and package ABI dependency hashes must match; branch names and matching commit strings alone do not prove that. Pin feeds to actual commits rather than relying on an unqualified `feeds update -a` quick start. Our main application currently needs user-space compatibility, but the vendor's TPM/network drivers add separate kernel acceptance obligations.

The drafts also mix ipk/opkg/GPG/usign/apk terminology while our release uses APK. Require the exact package manager and signature format for the accepted platform release; do not apply generic opkg instructions to the current device without checking.

## Current CCLI gaps corrected for recent work

1. **Do not reopen already demonstrated role enrollment as wholly absent.** The 19 October enrollment report records 19 identities, 143 live checks and 298 isolated checks. r64 subsequently adds positive Aggregator model/control/dataset/report coverage. These are representative engineering results, not a complete accredited RBAC campaign.
2. **Separate installed r64 from 22 October source.** Current source adds Aggregator namespace/nameplate/support objects, concurrent report instances, narrower configuration rights and Good/Stale/Invalid measurement-quality mapping. The follow-up records 2/2 CTest entries passing on synthetic loopback. Those changes are not in the installed r64 receipt.
3. **Quality remains partially open even in source.** `mms_adapter.cpp` function `update_aggregator_measurement_quality` correctly maps source state; `ccli_main.cpp` calls it. The DSO `totw_q`/`totvar_q` path in time synchronization still sets Good when time is synchronized. Thus completing Aggregator quality does not close DSO measurement quality. Verify both roles with stale/invalid telemetry independent of GNSS state, then package/deploy the reviewed fix and repeat on the DUT.
4. **Telemetry evidence is mixed, not universally absent.** The 19 October enrollment records 10 kW/1 kvar with good quality. Later diagnostics record read failures; the r64 validation deliberately had no simulator running. Re-establish fresh, continuous RTU readings on the agreed bench before attributing later zero readings to a fixed or permanently broken link.
5. **Physical Aggregator control remains intentionally unavailable.** The source refuses enabling the unvalidated plant-output backend. Complete signed import/export P/Q tracking, capability allocation, priority and loss-of-comms behavior before proposing enablement.
6. **Security and release evidence need one frozen specimen.** Reconcile source changes, installed binary, model/CID/CFG, signed policy, certificate profile, platform image, SDK, SBOM and report versions. Resolve the inherited protected TLS-baseline hash discrepancy documented in the Aggregator audit. Do not carry the old 5-LN plan forward without accounting for the newer Aggregator model.

## Closure order

| Priority | Action | Acceptance evidence | Owner |
|---|---|---|---|
| Immediate | Check actual CRL validity and prepare controlled renewal/reload | The lab CRL documented in the repo expires **23 October 2026 at 10:59:49 Europe/Rome**; verify the actual deployed trust bundle before relying on that date | HiTEKS/operator |
| P0 | Freeze final SKU and controlled standards/editions; resolve FIPS boundary/level/status and lab scope | Signed product baseline and lab-approved requirement/test matrix | HiTEKS + Tespro + lab |
| P0 | Obtain real platform release evidence | Signed image/recovery hashes, exact SDK/config/feeds, full SBOM/manifest, completed ABI commitment, exact board/TPM versions | Tespro |
| P0 | Resolve BL2 boot reliability and secure-boot chain/recovery | Supplier root cause, validated recovery, safe test-board secure-boot/fuse-state and negative-test records | Tespro/module vendor |
| P1 | Complete measurement quality across DSO/Aggregator and deploy source follow-up | Frozen signed package, stale/invalid/clock-state tests, healthy RTU and role read/report captures on DUT | HiTEKS |
| P1 | Complete PKI automation and operational key-protection decision | Enrollment/renewal/revocation/reload and outage/rollback tests; explicit key boundary | HiTEKS + Tespro as needed |
| P1 | Implement required native E2E and secure logging | Applicable 100-4 server cases, both rekeys, replay/integrity negatives, TLS syslog receiver and complete event checks | HiTEKS + lab |
| P1 | Prove vendor-service priority and exact-release USB update | Real scripts and coexistence tests; USB authorization/install/preservation/replay/failure receipts | Shared |
| P1 | Finish physical CCI regulation and hardware integration | Fresh metering, real P/Q response/settling/priority/loss-of-comms, I/O/power/time/environment evidence | HiTEKS + test team |
| P2 | Run accredited campaigns and complete manufacturing/CE file | Reports/certificates for the declared final specimen and standards, approved production configuration and manufacturing scope | HiTEKS + accredited bodies |

P0/P1/P2 are engineering priorities, not claims about statutory deadlines. Optional onboarding, encryption modes and higher-security-level tamper features must be selected explicitly rather than added as universal requirements.

## Supplier file disposition

| Supplied file | Review outcome |
|---|---|
| `01 CNAS.pdf` | Lab accreditation L5868, nominal 19 February 2025 to 18 February 2031; does not certify TG500. Full applicable scope and current status still needed. |
| `03 A2LA.pdf` | Lab certificate 4674.01, nominal expiry 31 December 2027; scope required. |
| `TO BE VERIFY/4674-01.pdf` | Six scope pages plus certificate; useful for listed tests and exclusions, insufficient for the entire CCI programme. |
| `TO BE VERIFY/L5868 Accredited Scope.html` and assets | Saved accreditation context/scope, not a TG500 test report; dynamic/offline completeness limitation applies. |
| `CC certificate.pdf` | Identified TPM component/version certification; map actual unit before reuse. |
| `FIPS 140 2 Level 2 Non-Proprietary Security Policy.pdf` | Component operational policy; NIST record required for level/status; not finished-device certification. |
| `Hall switch datasheet.pdf` | TI DRV5032 component data; no integration or tamper acceptance proof. |
| `Image_20261021151610_217_4.png` | ISO 9001 management-system certificate image; factory/scope/surveillance clarification required. |
| `TesPro-Certification-Scope-Allocation-Statement-EN.pdf` | Unsigned responsibility proposal, with standards-applicability issues; underlying product certificates absent. |
| `00-TG500-Deliverables-Overview.docx` | Useful index, but listed release artifacts are not included; does not promote accompanying drafts to released evidence. |
| `TG500-Firmware-Image-Identity-and-Version-Lock.docx` | Draft with unfilled release register; exact r601 match required. |
| `TG500-SDK-Version-Matching.docx` | Draft; provide actual SDK/build inputs and resolve cross-release guarantee. |
| `TG500-ABI-Compatibility-Declaration-Letter.docx` | Unfilled unsigned template; not an effective ABI commitment. |
| `TG500-IEC61850-Coexistence-Written-Policy.docx` | Draft design/pseudocode; implementation and measured behavior missing. |
| `TG500-SKU-Software-Package-List.docx` | Summary only; full authoritative manifest/SBOM absent. |
| `TG500-TPM-IDevID-Provisioning-Statement.docx` | Admits no factory IDevID; unsigned options need decision and proof. |
| `TG500-IDevID-Enrollment-Procedure.docx` | Draft with command/profile/persistence defects; must be corrected and tested before use. |
| `TG500-Secure-Boot-Enablement-Guide.docx` | Vendor-dependent plan with trust/command issues; no enabled-device proof. |
| `TG500-Signed-Image-Flow.docx` | Generic draft; exact tool/package/verification flow must be demonstrated. |
| `TG500-Fuse-based-Secure-Boot-Policy-and-HAB-Equivalence.docx` | Terminology mapping and planned fuse policy; no achieved equivalence/anti-rollback evidence. |
| `TG500-20261021.zip` | Intact duplicate of 11 DOCX files, not a firmware or certification evidence bundle. |

## Traceable references

Repository paths are relative to `CCLI Merged` unless indicated otherwise.

- `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r64_2026-10-21_093408.md` and `lab/evidence/aggregator-live-install-2026-10-21/RESULTS.md`: deployed specimen and limits.
- `lab/conformance/CCI_AGGREGATOR_INTERFACE_2026-10-21.md`: r64 interface and 22 October source-only follow-up/test results.
- `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp`: Aggregator quality around line 2080; DSO time-derived quality around line 2220.
- `apps/ccli/services/ccli_main.cpp`: source measurement-quality propagation around line 856.
- `lab/evidence/mms-tls-enrollment-2026-10-19/REPORT.md`: role coverage, server PEM-key limitation and historical healthy readings.
- `lab/evidence/iec62351-100-4-2026-10-19/REPORT.md`: native E2E scope and missing mandatory families within that scope; later handoff supersedes its historical clock/association failure.
- `lab/conformance/CCI_RBAC_CRL_AUTOMATION.md`: actual lab CRL lifetime, correction to older expiry claim, unfinished distribution/reload.
- `apps/ccli/core/event/event_store.cpp`: UDP sender around line 383.
- `apps/firmware-updater/docs/SIGNING_AND_USB_UPDATES.md`, `README.md`, `src/firmware_updater.cpp`: canonical update architecture and implementation.
- `handoff/chatgpt-project-2026-10-20/CCLI_PROJECT_HANDOFF.md`: platform identity, BL2 fault, GNSS, normative conflicts and prior evidence. r64 and 22 October records supersede its older release status.
- `knowledge-base/08-engineering/CCI_Annex_O_Extract.md`: project certification/hardware/power/time requirements; reconcile with controlled current original.
- `lab/conformance/dnv/DNV_UCA_PRODUCT_READINESS.md`: separate formal transport/UCA/product claims; its earlier model plan needs updating.
- `../CCLI/tmp/rbac-review/cei57142.txt`: current local CEI 57-142 extraction, especially section 5.8 (printed p.70) and PKI section 5.9. Use controlled original `../CCLI/Normativa/CEI 57-142-2026-07.pdf` for final lab agreement.
- [NIST CMVP 4467](https://csrc.nist.gov/projects/cryptographic-module-validation-program/certificate/4467): current Historical status, overall Level 2, Level 3 exceptions and covered firmware.
- [Official CC portal report 1278](https://www.commoncriteriaportal.org/nfs/ccpfiles/files/epfiles/1278a_pdf.pdf): matching component certificate/report.
- [IEC 61557-12 scope](https://webstore.iec.ch/en/publication/30065): PMD scope.
- [U-Boot signature verification](https://docs.u-boot-project.org/en/v2024.01/usage/fit/signature.html): FIT trusted control DTB and image-hash verification.
- [tpm2_readpublic](https://tpm2-tools.readthedocs.io/en/latest/man/tpm2_readpublic.1/), [tpm2_changeauth](https://tpm2-tools.readthedocs.io/en/latest/man/tpm2_changeauth.1/), [tpm2_sign](https://tpm2-tools.readthedocs.io/en/stable/man/tpm2_sign.1/): syntax and restricted-signing ticket requirements; pin actual installed tool versions before producing a corrected procedure.

External checks confirm selected records and technical references, not independent authenticity of every supplied file. Full current CNAS registry verification, ISO 9001 registry/surveillance verification and a byte-for-byte official A2LA scope comparison were not completed; keep those acceptance items open.

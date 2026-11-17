# Aggregator interface implementation — 21 October 2026

## Scope and applicable document

Implemented in the merged checkout from the local source checkpoint fac93a9 plus its working-tree changes. Release r64 was built with the canonical firmware toolkit, manufacturer-signature verified, and installed on Tespro on 21 October 2026. This task preserved all 27 configured TLS/RBAC files and did not change TLS/ACSE/session/presentation source.

The newer **local** normative baseline is CEI 57-142:2026-07, referenced by CEI 0-16:2026, rather than the user's CEI 0-16:2023 Annex T screenshots. Local source PDF: sibling `CCLI/Normativa/CEI 57-142-2026-07.pdf`; reviewed extraction `CCLI/tmp/rbac-review/cei57142.txt`. Relevant clauses are 4.3.1.3.1–.3, Table 21 (function states), Table 23 (Aggregator allocation), 4.3.2 (ACSI) and 5.3 (RBAC). This does not assert that no later published document exists.

User decisions: implement in the merged repository; keep plant outputs disabled; use the newer document when the existing implementation differs. Existing DSO mode conventions were not changed.

## Implemented and verified

- Signed Aggregator P and Q requests: `WSaDAGC1.WSptPct` and `VArSaDVAR1.VArTgtSptPct`, both −100…+100% Smax. Non-finite/out-of-range values are rejected before initial control acceptance, with defensive validation during execution. Signed values are verified by readback.
- Aggregator activation values **−1 / −5**, verified by control and status readback. The former +1/+5 Aggregator values are rejected. Existing DSO controls retain their existing values.
- `Beh`, `Health` and `FctOpSt` on both Aggregator control nodes. With physical actuation unavailable, Beh is +5/off, Health is +1/OFF and FctOpSt is 0/not available. Mod and target readback represent accepted in-memory requests, not proof of physical regulation. Health use is described as under study in the document.
- `PdCiMMXU1.TotW` and `.TotVAr`, including magnitude, timestamp and quality wiring. They use the same POC measurement updates as the DSO meter, while retaining separate role-protected object references.
- Static dataset `LLN0.DS_R_Aggregator_Mis4sec` containing the two Aggregator measurement objects.
- Both unbuffered `LLN0.RP.urcb_Aggregator_Mis4sec` and buffered `LLN0.BR.brcb_Aggregator_Mis4sec`, with a 4000 ms integrity period. Generated-CID instance names append `01`. Tests receive real reports from both variants.
- Explicit dataset/report scopes only in the Aggregator binding. The generated `01` report instances have explicit scopes; no wildcard is used. DSO reads/controls of these Aggregator objects/containers remain denied, and Aggregator access to DSO measurements remains denied.
- C1: Aggregator cannot delete its read-only dataset. C2: report enable/disable works, while changing IntgPd is denied. C3: allocated `ctlVal` control operations work, `ctlModel` stays protected, and measurements/Beh are read-only.
- A separate reproducible `lab_tg544_aggregator.cid` and `.cfg`; the existing DSO CID/CFG are not replaced. Generator: `scripts/generate_aggregator_model.py`. It clones the existing model, adds distinct Aggregator enum/types and preserves SCL type ordering.

## Plant output boundary — deliberately not complete plant regulation

`mms.aggregator_plant_outputs_enabled` defaults to **false**. The main service filters Aggregator active flags and deltas before plant calculation/write paths, preserving DSO state and changes. It does not turn negative P into a PF2 export ceiling. The adapter refuses startup if this reserved setting is set true, because the existing backend has no validated signed active-power tracking path.

Thus the **missing RBAC/model/report interface is implemented and tested in isolation**, but full physical Aggregator regulation is **not complete**. Physical enablement requires a proper signed P/Q backend, plant capability/allocation and priority handling, per-actor communications-loss behavior, real function feedback and settling-time tests. The existing r61 export-ceiling approximation must not be presented as signed import/export tracking. User approval is required before enabling plant outputs, after that work is concrete and reviewable.

Legacy `tools/mms-client/verify_aggregator.py` and its historical live variant exercise the older +1/+5 export-ceiling bench semantics. They are not verification of this new interface. Use the updated automated TLS test below. No existing client-tool changes were overwritten.

## Live installation and validation (21 October 2026)

Release 0.1.0-r64, codename CEI57-142-AGGREGATOR, was installed on Tespro at 192.168.0.1. APK SHA-256: 2694837eed4cdd59e00335e31e6b5508b3b050a4a4cac02906de82a9af380efb. Manufacturer signature verification passed before install. Profile A is active at owner-signed sequence 4 with policy digest 5f4b7dde602f7ce20bc5004de633f8098404a9f5aeca000ebc4db1e337dfba8b. CCLI was running as PID 20894 and listening on 192.168.0.1:3782. All 27 configured TLS/RBAC files matched the root-only pre-upgrade backup.

Using the repository's IEC 61850 client with the commissioned AGGREGATOR_OPERATOR identity:

- Mutual TLS and Profile A association passed; Aggregator status, dataset members and dataset values were readable.
- P=30% and Q=20% setpoint operations and P/Q mode -1 transitions were accepted with successful termination and readback. Values were restored to P=100%, Q=0%, both modes -5/off.
- Reading DSO-only PdCMMXU1.TotW was denied as required.
- BRCB replayed 93 buffered reports, some with old or epoch measurement timestamps. This matches buffered delivery while unsubscribed, not 93 new simulator events; buffer overflow was not checked. The first URCB listen was 3 seconds, shorter than its 4-second integrity period, so no event was expected with static inputs. A 6-second retest received one periodic report and passed. Both RCBs were disabled after testing.
- A Q setpoint request inside the configured 3-second spacing interval was rejected; retry after the interval passed.

The status endpoint confirms outputs remained disabled: wsa_active=false, varsa_active=false, do_curtail_on=false and p_wsa_kw=0. No simulator or COM8 action was performed. The user confirmed the inverter simulator was not running, so zero/stale Modbus values were expected during this validation. MMS returned Good quality flags with zero values while device status was stale; verify quality propagation when the simulator is running. Successful protocol checks do not prove physical P/Q actuation. The device continues to use its handbuilt model; the generated CID/CFG was not selected.

Detailed live evidence and the deployment manifest are in lab/evidence/aggregator-live-install-2026-10-21/RESULTS.md and lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r64_2026-10-21_093408.md.

## Tests and evidence

Independent WSL snapshot/build: `/home/adminavz/ccli-aggregator-gap-20261021/{source,build}`. Native build of the full `ccli` executable passed; the executable was not run against hardware. Its libmodbus/IEC104/GOOSE backends are disabled in this isolated native build.

Passed:

- `aggregator_power_arbitration`, including default-off filtering and preservation of DSO state.
- `mms_rbac_generate_certificates`, with new disposable Aggregator credentials and signed fixture packages.
- `mms_rbac_tls_authorization`, including both the handbuilt and generated Aggregator CID paths, real loopback TLS/MMS, signed value/mode readback, buffered/unbuffered report reception and role-denial checks. Existing legacy TLS, certificate rejection and live identity revocation tests in the same suite passed.
- `mms_acse_p307_acse_auth` and `mms_rbac_signed_security` regression suites.

Final focused network/arbitration run: 3/3 CTest entries passed, 17.58 s (including fixture setup). Separate parser/security run: 3/3 entries passed, 2.96 s (including fixture setup). These counts are suites/setup entries, not a full CEI conformance case count.

The protected TLS baseline checker reports a pre-existing mismatch for committed scripts/generate_mms_certificate_auth.py against the older recorded extension hash. The script has no working-tree diff and was not edited; the baseline was not rewritten. This did not block the verified r64 build. Investigate the inherited discrepancy before the next release. Existing TLS/ACSE/session/presentation sources, TLS credentials and trust were not edited by this implementation.

## Build and commissioning handoff

1. Release r64 has been built with the canonical firmware toolkit, manufacturer signature verified, installed and validated on Tespro. The deployment manifest and live client evidence are linked above.
2. The running device uses the handbuilt MMS model. The generated CID/CFG remains available in source but was not selected on the device.
3. Owner-signed Profile A policy sequence 4 is active with the Aggregator dataset/report scopes.
4. TLS credentials, trust and CRLs were preserved byte-for-byte across installation. No TLS or ACSE protocol source was changed for this release.
5. Aggregator plant outputs remain disabled by default; the service refuses startup if this unvalidated backend is explicitly enabled.
6. Restore healthy Modbus measurements and fix MMS quality propagation before treating measurements as valid or attempting physical P/Q regulation. Current telemetry is stale/read_fail while MMS quality is reported Good.
7. Physical Aggregator regulation still needs a validated signed P/Q plant backend, feedback and settling tests. Keep outputs disabled until that work and its review are complete.

## Source-tree audit follow-up — 22 October 2026

After reviewing the Aggregator implementation against CEI 57-142:2026-07, the source-tree model was extended to include the Aggregator 7-420 nameplate and support objects (`NamPlt`, `ReqW`/`ReqVAr`, `RmpRteUse`, `InEcpRef`), and the CEI namespace is populated in both generated and hand-built models. The generated model now provides two instances of each Aggregator buffered and unbuffered report, as required for concurrent clients. Aggregator RBAC no longer grants broad configuration access that could permit changing `ctlModel`; the allocated control operations remain available through their authorized `ctlVal` paths. Aggregator P/Q measurement quality now follows the source meter state: Good maps to IEC quality Good, Stale to Questionable/OldData, and Invalid to Invalid/Failure. Clock synchronization remains represented by timestamp time quality and no longer promotes stale source measurements to Good.

The focused verification was rerun from `apps/ccli/build-aggregator-audit`: `mms_rbac_tls_test` built successfully, then `ctest -R mms_rbac_tls_authorization --output-on-failure` passed both the certificate fixture and TLS/MMS authorization test (2/2, zero failures). The test covers the generated and hand-built models, namespace, support objects, signed P/Q boundaries, mode values, Good/Stale/Invalid measurement quality, status/measurement access, simultaneous report clients, protected report configuration, and role separation. It uses loopback and synthetic measurements; it does not exercise a physical plant or establish full CEI conformance.

These source-tree follow-up changes were not part of the already-installed r64 build. Full signed P/Q regulation remains open pending the validated plant backend, capability/allocation and priority handling, communications-loss behavior, and real feedback/settling validation. Keep plant outputs disabled.

Actual TSP tests against the added generated model, complete ACSI conformance, healthy live telemetry and physical regulation remain outstanding.

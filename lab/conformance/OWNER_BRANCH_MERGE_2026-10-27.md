# Owner synchronization — 2026-10-27

Personal starting commit: 06992ff (r70 TLS/CRL work and 23-node CID).
Owner source: yahiagabereng-ctrl/CCLI, upstream/main e6d6ce0.
Merge base: ffb0f19. Recovery branch: backup/pre-owner-sync-20261027.

## Integration decisions

- Retain the personal r70 version and validated services/ccli_main.cpp application entry by default (CCLI_LEGACY_MAIN=ON). The owner's optional program/ entry remains available for separate qualification.
- Merge the owner's EMT432 current, multi-phase voltage/current magnitude and phase-angle decoding, commissioning helpers, meter defaults and documentation.
- Preserve personal TLS/CRL validation, signed ACSE/Profile A identity binding, native E2E implementation and Aggregator behavior. OCSP remains outside the claimed scope; native E2E remains disabled on the device.
- Combine live Aggregator limits and owner Operating Rule fallback using separate arguments. Update both application entry paths and regression checks so a fallback boolean cannot accidentally become a WSa enable flag.
- Fix an incoming startup diagnostic that dereferenced a dynamic-model MMS value before IedServer initialized it. Native product-adapter tests caught this failure.
- Retain the 23-node CID/CFG from 06992ff without model edits. Preserve the owner's revised 5-node specimen separately.
- Add lab_tespro_192_168_0_1_23ln.yaml from the actual Tespro configuration. Package default remains /etc/ccli/lab.yaml and installs this profile plus the matching CID/CFG under /etc/ccli/icd. The application-config push script selects this profile and defaults to 192.168.0.1.
- Keep the meter RTU configuration snapshot unchanged in this merge. The separate verified EMT432 endpoint is 192.168.178.10:502, but persistent subnet and meter TCP configuration have not been deployed. Older owner profiles use other bench addresses; they are retained as explicit alternatives.
- Exclude newly introduced product-mirror private-key and combined-key files from the final tracked tree. Add ignore rules; credentials stay local. The owner's existing public history is not rewritten.
- Leave local captures, binary build outputs and temporary office files outside this merge commit.

## Validation

The merged personal application, DSO and security test targets compile on the local Linux host. Host build uses the simulator Modbus backend because libmodbus is absent; this is not a hardware/TCP-meter acceptance test. Focused regression outcomes are recorded below after execution. No APK was built, signed or installed and Tespro was not changed by this merge.

Validation result: personal application and optional owner ccli_program both compile. All 18 focused CTests passed (139.66 s), and the full native field campaign passed separately (284.77 s): 19 selected tests total. The narrowly reviewed source-integrity fingerprints retain their prior values in owner_merge_review metadata; all protected TLS sources outside the five reviewed integration files and the complete MMS security startup were compared with the personal starting commit. IP/model guard verified 192.168.0.1:3782, TLS/Profile A, r70 and the unchanged 23-node CID/CFG.

User requested local-only integration: changes remain staged on codex/owner-sync-20261027, ready for a later merge commit. No commit or push to GitHub was performed, and no firmware/configuration was deployed to Tespro.


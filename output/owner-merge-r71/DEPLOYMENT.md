# r71 local build/deployment preparation

Source: local unresolved-commit merge working tree on codex/owner-sync-20261027, personal HEAD 06992ff plus owner e6d6ce0, reviewed integration fixes and release bump to 71. No GitHub commit/push requested or performed.

Host validation: 18 focused regressions passed plus full native field campaign (19 selected tests total). ARM64 APK build uses canonical firmware-updater/scripts/build-ccli.sh and existing manufacturer key, OpenWrt SDK 25.12.5, platform_tg500, libmodbus forced ON and personal legacy application entry.

Device: 192.168.0.1. Confirmed SSH Ed25519 SHA256:f9nqNAmE4INT0UIhGfKk0waU8BGCxN/FVkN0VbhNo/0.

Recovery archive held locally under restricted ACL in ignored tmp/tespro-recovery-r71/before-r71.tgz. SHA256 255d9e4707c7b3eea2bff20bedf0d6adeb91d7e0674b5ffb3a8839657f1f472f matches device copy. Contains credentials: never commit or distribute it. Older recovery directories moved to /tmp/ccli-r71-old-snapshots after verifying this durable backup; /etc/ccli-upgrade.DiAoaP and /etc/ccli-upgrade.lnDbBF retained on flash. Free flash increased from 4.3 MiB to 14.4 MiB.

Pre-install hashes:
- lab.yaml: a09da95f8b6146d7cc338db1d075b7f0af9067ba10187e236cb7f02b610dd6ee
- 23-node runtime CFG: 5b055efbfde98f188cbdb874121bef41f0485dd1ff1a1bcddfdd4a68b3cf42b5
- fresh RBAC CRL: 05c51835aa6ce45dd2a9ce260317be63e8f505babc8c1a16530bf6a5f25ea9cf

At APK install, the existing RTU meter section was preserved. After the install, the EMT432 TCP configuration was commissioned as recorded below. Native E2E remains disabled. The 23-node CID is a structural model, not a claim that all new nodes have live backend mappings.

## Installed and verified

- Signed artifact: apps/firmware-updater/artifacts/ccli-0.1.0-r71.apk, SHA256 30634e552141a811a1c24acd0764ff90a2882ec03af08273f6e80b8c02c68619. Uploaded hash matched. Canonical manufacturer verification passed locally and on Tespro.
- The wrapper built r71 successfully but stopped while verifying an unrelated historical r61 artifact. The new r71 was signed and verified separately through canonical scripts/sign-apk.sh and scripts/verify-apk.sh; no unsigned installation was used.
- Tespro's permanent APK public key differs from the canonical workstation manufacturer public key. r70 and r71 both verified against the same canonical key. Used only a temporary public-key directory for this exact-hash SSH installation. Permanent APK and TPM trust stores were not modified; temporary public-key copy removed afterward. This is not USB/TPM update validation.
- Upgraded ccli 0.1.0-r70 to r71 using apk --no-network add --upgrade and the explicit temporary canonical public-key directory.
- Upgrade recovery snapshot retained on flash: /etc/ccli-upgrade.KpPcDO. All 236 saved non-event-log configuration/credential hashes passed. Upgrade hold cleared only after those checks.
- Running process PID 13360. Root-owned init/binary; /etc and credential parent directories remain root-owned. Listening 192.168.0.1:3782 with TLS, CRL and Profile A enabled, full_cid_cfg model loaded.
- Main-service mutual TLS / signed ACSE association and authorized read passed. Independently checked server AARE certificate, issuer, AP/AE, signature, mechanism, timestamp and CONNECT/ACCEPT formatting.
- Authenticated directory browse returned all 23 logical nodes. Evidence in mms-read/ and browse.json alongside this record.
- Free flash after install: 10.3 MiB. Current UTC checked before/after upgrade; GNSS remains unsynchronized.
- No GitHub commit or push performed. Source remains the local merged worktree with the r71 release bump.

## Post-install EMT432 TCP setup

- Updated `/etc/ccli/lab.yaml` to match the local personal profile: Modbus TCP `192.168.178.10:502`, unit 1; active power register 932, reactive power 940, phase-A current register 922 (scale 1.0 A), float word swap enabled, and line voltage register 906. The CCLI service was restarted to load the profile.
- Added persistent OpenWrt interface `network.meter_lan3` on `br-lan` with `192.168.178.1/24`. The Tespro's existing `192.168.0.1/24` address and MMS listener remain active.
- Verified route and ping to the meter, then observed repeated successful CCLI FC3 polls. After I1 was mapped, logs reported about 5.9 A, 1.35 kW, 229 V, and `pf2=ok`. Earlier zero power/current samples changed as the meter values changed; compare live readings with the plant's operating state.

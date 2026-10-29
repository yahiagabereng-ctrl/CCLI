# Development deployment over SSH

This is the project default until the user explicitly changes it. Applies to all chats and agents working in this repository.

## Standing authorization

Use SSH for development deployment to the user-confirmed Tespro. Unsigned APKs and `apk add --allow-untrusted` are explicitly authorized. Do not ask for signing approval or an operator signing passphrase for routine development installs. No USB bundle, manufacturer signature, operator certificate or TPM provisioning is required for this development path.

## Fast routine

1. Coordinate with any other chat using the device before stopping or replacing CCLI. Only one chat deploys at a time. Include all agreed shared-source changes in the build.
2. Increment `apps/ccli/VERSION`, synchronize `package/ccli/Makefile` with `scripts/sync-ccli-version.ps1`, and record the changes. Build the exact release for the target ARM64 SDK with libmodbus enabled. Reuse SDK dependencies and incremental build output where valid; avoid downloading feeds or rebuilding unchanged dependencies on every install. Do not force a clean build unless required by changed build settings or stale output.
3. An existing unsigned development build entry is `lab/tg544-openwrt/build-ccli-local-untrusted.sh --untrusted-lab-build`. It currently contains a workstation-specific SDK path and forces a clean build: inspect/adapt those assumptions before using it. It is not the signed release build entry.
4. Upload the exact versioned APK over SSH/SCP to `/tmp`. Use legacy `scp -O` when the target lacks SFTP. Verify its SHA-256 after transfer. Never select an artifact through an ambiguous wildcard or assume the old `ccli-bin` is current.
5. Preserve the current `/etc/ccli`, `/etc/config/ccli`, credentials and device-specific settings in a recoverable backup. Check free flash. Stop CCLI, then install the APK with `apk --no-network add --upgrade --allow-untrusted /tmp/<exact-package>.apk`. Resolve missing dependencies explicitly if needed. Prefer APK package installation so package registration matches the executable.
6. Respect the package's upgrade preservation mechanism: confirm configuration and credentials were preserved and no `.pending` marker remains before clearing the newly created `/etc/ccli-upgrade.hold`. Investigate pre-existing holds rather than clearing them blindly. Start CCLI explicitly after preservation checks.
7. Check installed version, service process and required listener, then the affected functionality (for this bench, TCP meter and inverter RTU status). Record the artifact hash, deployed version, configuration changes and result in `lab/tg544-openwrt/deploy-manifests/`. Keep checks proportional to the change.

Do not replace the whole device configuration with a historical lab profile during a routine code update. The inverter simulator was verified on `/dev/rs485_2_uart` (COM8 route), unit 1, 9600 8N1; preserve the live settings unless the user changes the wiring. TCP meter and inverter RTU acquisition are separate.

Direct executable replacement is an exceptional development shortcut and must be reported as such; it is not an APK install and does not update package metadata. Verify the exact executable version/hash before replacement.

## Signed releases and USB updates remain available

Keep the existing signing tools, keys, documentation and USB workflow intact. When the user requests signed packages, release delivery or USB testing, follow `apps/firmware-updater/docs/SIGNING_AND_USB_UPDATES.md` using only the canonical toolkit, local ignored keys, `tools/bin/apk`, `artifacts/`, `bundles/` and `scripts/new-bundle.sh`. Never use old Desktop signing inputs or `.ccli-signing` defaults. Private keys stay confidential and out of packages/bundles. Enter the encrypted operator-key passphrase only at the visible local prompt. Verify manufacturer signatures before bundling. A requested USB installation test must use USB.

Verified bench setting: serialservice.port3.enabled=0 gives CCI exclusive RS485-2 access. Preserve this setting during deployment; enabling seriald sharing on this port interferes with inverter polling. Backup and verification record: lab/tg544-openwrt/deploy-manifests/CCLI_RTU_EXCLUSIVE_2026-10-29.md.

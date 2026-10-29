# Default development deployment

Until the user explicitly says otherwise, use the SSH development routine in [DEVELOPMENT_SSH_DEPLOYMENT.md](DEVELOPMENT_SSH_DEPLOYMENT.md). All chats and agents must read it before building or deploying CCLI. Unsigned development APKs and `--allow-untrusted` installation are authorized; signing and USB bundling are not required for routine development deployment. Coordinate concurrent deployments, preserve live configuration/credentials, verify the exact artifact and keep deployment checks focused. Preserve the signed-release workflow below for later use.
# Firmware update workflow

For all future APK signing and USB bundle tasks, use only the canonical toolkit
under `apps/firmware-updater`. Start by reading its
`docs/SIGNING_AND_USB_UPDATES.md`. Use `scripts/new-bundle.sh`, local `keys/`,
`tools/bin/apk`, `artifacts/` and `bundles/` there. Do not use old Desktop
Firmware Update inputs or `.ccli-signing` keys as alternate defaults.

Treat private keys as confidential, but track CCLI TLS, lab PKI, root CA, and
manufacturer signing key material in this repository. Synchronize it only to the
private GitHub remote. Keep keys out of APKs and USB bundles. Request
the encrypted operator-key passphrase only at the user's visible local prompt.
Use the toolkit's local manufacturer signature verification before bundling.
Building application binaries still requires repository sources and an SDK;
bundling an already built APK does not. Never substitute an SSH installation
for a requested USB installation test.

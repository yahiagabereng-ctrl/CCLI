# firmware-updater 1.1

Start with [the consolidated signing and USB procedure](docs/SIGNING_AND_USB_UPDATES.md).
Canonical tools are in `scripts/` here. Private keys in `keys/` are tracked only
in the private GitHub repository; signed APKs in `artifacts/` and generated bundles
in `bundles/` stay local. Do not copy this whole toolkit onto USB. Historical paths
below describe the original lab setup; use the consolidated procedure for current
local commands.

A clean clone from the private GitHub repository includes the key material but
intentionally lacks APK artifacts, bundles, and the local `tools/bin/apk` executable.
Use the consolidated guide's “Preparing a fresh clone” section before bundling.
The end-to-end bundle test is
`wsl bash ./scripts/test-bundle-self-contained.sh` from this directory.

Offline TesproOS USB update engine for the Annex T firmware-update controls.
The TesPro remains the USB host. An authorized laptop creates a one-time
bundle and an operator carries that bundle to the device on removable media.

Release 1.1.0-r8 verifies running procd instances instead of trusting init-script
status (which may report "active with no instances"). For a CCLI APK transaction,
it hashes existing files under `/etc/ccli` and `/etc/config/ccli` before installation,
checks their preservation afterward, and only then clears `/etc/ccli-upgrade.hold`.
An unfinished `.pending` marker or a pre-existing hold aborts the transaction for
review. CCLI must have a running instance and listen on MMS port 3782 for three
consecutive checks before startup is reported successful. A failed preservation
check retains the hold. Already installed transactions remain consumed even if
startup fails, preventing replay; investigate and recover that installed state.

## Required USB bundle

Each `update-*` directory must contain:

```text
operator-certificate.pem
update-manifest.json
update-manifest.json.sig
one-or-more-signed-packages.apk
```

The certificate is public. The operator private key stays on the authorized
laptop, TPM, smart card, or HSM and must never be copied to the USB drive.

The signed manifest binds the authorization to:

- one transaction ID;
- one provisioned TesPro device ID;
- the `firmware-update` operator role;
- a short validity window;
- the exact filename and SHA-256 digest of every APK.

The updater rejects copied/expired transactions, wrong-device manifests,
unlisted APKs, hash changes, revoked or unauthorized operator certificates,
and APKs not signed by the dedicated manufacturer release key.

## Annex T update sequence

1. Validate the EJBCA operator certificate chain, time, CRL, non-CA status,
   `digitalSignature` key usage, and `OU=Firmware Update Operators` authority.
2. Verify `update-manifest.json.sig` and enforce its device, role, expiry,
   package-hash, and one-time transaction constraints.
3. Read the CA certificate, CRL, and manufacturer APK public key from dedicated
   TPM NV indexes into a temporary, private staging directory. Verify each APK
   using only that manufacturer key; network repositories are disabled.
4. Stop the `ccli` service and confirm it is inactive before consuming the
   authorization. A stop-script exit code alone is not treated as failure when
   the service is confirmed inactive.
5. Perform an upgrade-only APK transaction. Once the APK manager reports a
   successful installation, atomically record the transaction ID so it cannot
   replay. Pre-install failures leave the authorization retryable.
6. Increment TPM NV counter
   `0x1500016`, restart `ccli`, and verify that it is running again.
7. Append security events to the persistent eMMC audit file and system log.
   The update procedure never truncates or deletes the persistent audit file.

## One-time device provisioning

The commissioning inputs are public and are copied to the device temporarily:

```text
operator-ca.pem
operator-crl.pem
ccli-release.pub.pem
```

Run the provisioning command on the TesPro with the three inputs and its
unique device ID:

```sh
firmware-updater-provision-trust operator-ca.pem operator-crl.pem \
  ccli-release.pub.pem CCI-0008CCFD041F
```

This writes and reads back three dedicated TPM NV indexes: `0x1500017` for
the CA certificate, `0x1500018` for its CRL, and `0x1500019` for the APK
release public key. It refuses to overwrite an occupied index. Each input
must fit the TesPro TPM's 2160-byte maximum NV entry size. If a later CRL
exceeds that limit, commissioning needs a different revocation storage design.
The updater reads these entries for every bundle check. The old general APK
key store is not used for update authorization.

This TPM uses owner authorization for NV writes. Protect that authorization in
production; possession of root with access to the owner hierarchy can change
these entries. TPM NV storage does not by itself prove an authorized operator
commissioned the device.

The default persistent audit location is:

```text
/mnt/mmcblk0/firmware-updater/security-audit.log
```

Updates fail closed if the eMMC audit file cannot be appended. The device UTC
clock must also be maintained offline (for example during commissioning or by
an approved GNSS/time source), because certificate and authorization validity
checks fail when the clock is wrong.

## EJBCA operator certificate

Create a dedicated EJBCA certificate/end-entity profile for update operators:

```text
Basic Constraints: CA=FALSE
Key Usage: Digital Signature
Subject OU: Firmware Update Operators
Validity: organization policy (for example 90–365 days)
```

Issue individual certificates so the audit identifies a person. Do not issue
these certificates from EJBCA's Management CA and do not reuse the firmware
release-signing key as an operator key.

## Build and sign the updater package

From WSL, with the OpenWrt 25.12.2 mediatek-filogic SDK installed at the path
configured in `build-signed.sh` and the protected release key pair under
`apps/firmware-updater/keys/manufacturer`:

```sh
bash apps/firmware-updater/build-signed.sh
```

The script checks that the release private and public keys match, cross-builds
the package, signs it, verifies the resulting signature, and copies the final
APK to `apps/firmware-updater/dist`.

## Create a USB bundle

Use the canonical local-key command in
[`docs/SIGNING_AND_USB_UPDATES.md`](docs/SIGNING_AND_USB_UPDATES.md). Do not use
the historical external `C:\PKI` example or old PowerShell command below this
point; bundle creation now goes through `scripts/new-bundle.sh` in WSL.

## Device commands

```sh
/usr/sbin/firmware-updater --status
/usr/sbin/firmware-updater --provision-tpm
/usr/sbin/firmware-updater --verify-bundle /mnt/usb/update-TRANSACTION-ID
/usr/sbin/firmware-updater --once
logread -e firmware-updater
tail -n 50 /mnt/mmcblk0/firmware-updater/security-audit.log
```

`--verify-bundle` performs all authorization, certificate, CRL, APK-signature,
hash, device, role, and expiry checks without consuming the transaction,
stopping CCLI, or installing anything.

## Remaining product evidence

This implementation supplies the software controls, but Annex compliance also
requires controlled provisioning records, EJBCA profile/issuance evidence,
operator-key protection evidence, time-source evidence, negative/replay tests,
and proof that the persistent security log is excluded from every update and
factory-reset procedure.

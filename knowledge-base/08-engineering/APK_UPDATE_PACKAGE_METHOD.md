# APK Generation Method for the TesPro USB Updater

Current toolkit and procedure: [apps/firmware-updater](../../apps/firmware-updater/docs/SIGNING_AND_USB_UPDATES.md).
Use its scripts and local keys for current bundle creation. Its r8 completion
checks supersede the older direct-start advice below.

This document describes how to produce an OpenWrt APK that can be installed by the TesPro offline USB updater.

## Trust model

There are two independent signatures:

1. The APK is signed with the manufacturer release key. The corresponding public key is provisioned in the TesPro TPM. The updater verifies every APK against that TPM-provided key.
2. The USB update manifest is signed with an authorized operator private key. The operator certificate and CRL are trusted through TPM NV storage.

Never copy either private key to the USB drive.

## 1. Prepare the package

The package must use the correct name and a newer release when upgrading, target `aarch64_cortex-a53`, declare runtime dependencies, set executable permissions, and include an OpenWrt init script for services. Init scripts and system startup files must be owned by `root:root`.

For the controller, the APK includes:

```text
/usr/sbin/ccli
/etc/init.d/ccli
/etc/config/ccli
/etc/ccli/lab.yaml
```

The CCLI init script uses `START=99`, so it starts late in boot after networking and required services. Current CCLI post-install preserves configuration and sets an upgrade hold; updater r8 verifies preservation, clears the hold, and confirms startup.

For the independent watchdog, build a second `ccli-watchdog-<version>-r<release>.apk` for the
same target. It must contain `/usr/sbin/ccli-watchdog` and `/etc/init.d/ccli-watchdog`, declare
`ccli` as a runtime dependency, and have its own root-owned executable init script and post-install
enable/start action. The watchdog APK must be listed as a separate package entry in the same signed
update manifest as CCLI.

## 2. Build and sign the APK

Build with the TesPro/OpenWrt SDK and refuse to produce an unsigned package when the manufacturer key is missing. Example for CCLI from WSL:

```bash
export CCLI_APK_SIGN_KEY="$PWD/apps/firmware-updater/keys/manufacturer/ccli-release.pem"
export CCLI_APK_SIGN_PUB="$PWD/apps/firmware-updater/keys/manufacturer/ccli-release.pub.pem"
bash /mnt/c/Users/Administrator/Desktop/CCLI\ Alireza/CCLI/lab/tg544-openwrt/build-ccli-ipk-sdk.sh
```

Build the watchdog separately with `lab/tg544-openwrt/build-ccli-watchdog-apk.sh`. The two
scripts emit `lab/tg544-openwrt/ccli-<version>-r<release>.apk` and
`lab/tg544-openwrt/ccli-watchdog-<version>-r<release>.apk`. Do not use the SDK/default key;
TesPro will reject either package.

Verify the final APK using the same public key provisioned in the TPM, with network access disabled:

```bash
apk --keys-dir /path/to/manufacturer-keys --no-network verify ccli-0.1.0-r55.apk
apk --keys-dir /path/to/manufacturer-keys --no-network verify ccli-watchdog-0.1.0-r55.apk
sha256sum ccli-0.1.0-r55.apk ccli-watchdog-0.1.0-r55.apk
```

Inspect the package and confirm `/etc/init.d/ccli` is present, executable, and installed as `root:root`. For packages without a controlled upgrade hold, a post-install script can run:

```sh
chown root:root /etc/init.d/ccli
/etc/init.d/ccli enable
/etc/init.d/ccli restart || /etc/init.d/ccli start
```

## 3. Create the signed USB bundle

The updater expects one complete directory:

```text
update-<transaction-id>/
  ccli-0.1.0-r55.apk
  ccli-watchdog-0.1.0-r55.apk
  operator-certificate.pem
  update-manifest.json
  update-manifest.json.sig
```

Create a multi-package bundle with the PowerShell script:

```powershell
scripts/new-firmware-update-bundle.ps1 `
  -Package ccli-0.1.0-r55.apk,ccli-watchdog-0.1.0-r55.apk `
  -OperatorCertificate operator-certificate.pem `
  -OperatorPrivateKey operator-private-key.pem `
  -DeviceId TG544-UNIQUE-SERIAL `
  -OutputDirectory C:\FirmwareBundles `
  -UsbDriveLetter E
```

Each listed APK receives its own filename and SHA-256 entry in the same signed manifest. The
PowerShell helper is app- and device-agnostic: it accepts one to 32 APKs, an explicit device ID,
an optional operator role, and an optional removable USB drive. The shell helper accepts one APK
only; use the PowerShell helper for this paired CCLI/watchdog update.
The remaining arguments are:

```text
APK  operator-certificate.pem  operator-private-key.pem
DEVICE_ID  OUTPUT_DIRECTORY  VALID_FOR_MINUTES
```

The operator private-key passphrase is entered only at the local OpenSSL prompt. It is never stored in the bundle.

The manifest records the device ID, operator role, transaction ID, validity window, APK filename, and APK SHA-256 hash. The bundle script verifies that the operator certificate is non-CA, allows digital signatures, and has the firmware-update-operator identity.

## 4. Copy to USB

Copy the entire directory to the USB root or the updater’s expected update directory:

```text
USB:\update-<transaction-id>\
```

Do not rename files or copy the operator private key.

## 5. Automatic installation

When the USB is mounted, the running `firmware-updater` detects a valid bundle automatically. It checks the manifest, certificate, CRL, device, role, expiry, replay state, manifest signature, APK hash, and TPM-trusted manufacturer APK signature before installing.

It then performs an upgrade-only APK transaction, records the transaction as consumed, restores/restarts the service, and writes audit events such as:

```text
BUNDLE_VERIFIED
AUTHORIZATION_ACCEPTED
INSTALL_STARTED
CCLI_STARTED
UPDATE_SUCCEEDED
```

Every transaction is one-time-use. Reusing a processed directory is rejected.

## 6. Verify after installation

On TesPro:

```sh
apk list -I | grep '^ccli-'
/etc/init.d/ccli enabled
/etc/init.d/ccli status
ls -l /etc/init.d/ccli
ubus call rc list '{"name":"ccli"}'
```

The startup entry should show `ccli` with priority `99`, enabled and running. LuCI’s Startup page uses the `rc` backend; a non-root-owned init script can be hidden even when the service is running.

## Common failures

- **Manufacturer APK signature failed:** wrong signing key or APK modified after signing.
- **Transaction already consumed:** create a new bundle.
- **Authorization expired/premature:** check device time and bundle validity.
- **Wrong device:** manifest device ID does not match the TesPro device.
- **No startup entry:** check `/etc/init.d/ccli` ownership, executable permission, and `/etc/rc.d/S99ccli`.
- **Bundle not detected:** confirm the complete update directory is on the mounted USB and `firmware-updater` is running.

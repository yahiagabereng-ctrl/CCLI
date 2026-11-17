# TesPro secure offline USB update runbook

This runbook documents the working offline update process built and tested on
13 October 2026. It uses EJBCA to identify the human update operator, TPM NV
storage on the TesPro as the update trust store, and an APK manufacturer key to
authorize the actual software package.

It is a lab/operator procedure. Do not put passwords, private keys, P12 files,
or TPM owner authorization in this document, a USB bundle, source control, or
an unencrypted text file.

## Current tested state

| Component | Tested version/state |
| --- | --- |
| TesPro device ID | `CCI-0008CCFD041F` |
| Firmware updater installed on TesPro | `firmware-updater 1.1.0-r6` |
| Firmware updater release built for next USB update | `firmware-updater 1.1.0-r7` |
| CCLI after update test | `ccli 0.1.0-r53` |
| Operator CA | `CCI-Firmware-Operator-CA` |
| Operator role | `firmware-update` |
| Required subject OU | `Firmware Update Operators` |
| TPM transaction counter | `0x1500016` |
| TPM operator CA | `0x1500017` |
| TPM operator CRL | `0x1500018` |
| TPM APK manufacturer public key | `0x1500019` |

The updater is a daemon and scans removable media every 15 seconds. It is the
TesPro, not the laptop, that hosts the USB drive and performs installation.

## What each trust item does

```text
Operator private key ── signs manifest ────────────────┐
EJBCA operator cert ── identifies authorized operator ──┼─ USB authorization bundle
Manufacturer APK key ── signs the software APK ─────────┘
                                                        │
TesPro TPM: CA + CRL + manufacturer public key ──────── verifies bundle and APK
```

The private operator key signs a short-lived permission to update. It does not
sign the CCLI or updater APK. The dedicated manufacturer release key signs the
APK. The TesPro reads the corresponding public key directly from TPM NV for
every verification.

## Prerequisites

- A trusted Windows laptop with OpenSSL and WSL/OpenWrt SDK build environment.
- EJBCA Administration access for the dedicated update CA and operator profile.
- A unique TesPro device ID.
- The public CA certificate, current CRL, and manufacturer APK release public
  key available for initial TPM provisioning.
- A removable USB drive. The private operator key must remain on the laptop.
- Correct TesPro UTC time. Certificate and bundle validity checks fail closed
  if its clock is incorrect.

## 1. Create the TPM-backed update trust

Do this once during controlled commissioning, using public inputs only:

```sh
firmware-updater-provision-trust \
  operator-ca.pem operator-crl.pem ccli-release.pub.pem \
  CCI-0008CCFD041F
```

The command writes and reads back the three trust values into TPM NV. It refuses
to overwrite an existing index. Confirm the result with:

```sh
/usr/sbin/firmware-updater --status
```

Expected essentials:

```text
TPM: available
Operator CA TPM NV: 0x1500017
Operator CRL TPM NV: 0x1500018
Manufacturer key TPM NV: 0x1500019
```

The CA certificate, CRL, and release key can be public; the TPM owner hierarchy
authorization must be protected because it controls who can change NV content.

## 2. Configure EJBCA

### 2.1 Create a dedicated crypto token and CA

In EJBCA Administration:

1. Create a software crypto token, for example
   `CCI-Firmware-Operator-Token`.
2. Generate an RSA 4096 key pair inside that token.
3. Create a dedicated X.509 CA, for example
   `CCI-Firmware-Operator-CA`.
4. Use the generated token key for the CA signing/default/signing key.
5. Use a CA subject such as `CN=CCI Firmware Operator CA,O=HiTEKS,C=IT`.
6. Save the CA, then download its PEM certificate and current CRL for TPM
   commissioning.

Do not use EJBCA `ManagementCA` as the firmware-update operator CA.

### 2.2 Create the operator certificate profile

Create an **End Entity** certificate profile with these minimum properties:

| Property | Value |
| --- | --- |
| Type | End Entity |
| Allowed key algorithm | RSA |
| Allowed key size | 2048 or 4096 bits; 2048 was used for the operator cert |
| Basic Constraints | `CA=FALSE`, critical |
| Key Usage | `digitalSignature` |
| Subject Key Identifier | enabled |
| Authority Key Identifier | enabled |
| Certificate storage | enabled |
| Validity | appropriate organizational policy; 90–365 days is typical |

No CA or certificate-signing key usages should be present. The profile must be
issued by `CCI-Firmware-Operator-CA` and must allow that CA.

### 2.3 Create the End Entity profile and operator identity

Create an End Entity profile that:

- permits subject DN attributes `CN`, `O`, `OU`, and `C`;
- requires `CN` and makes the selected DN values editable as needed;
- defaults/selects the firmware-update certificate profile and dedicated CA;
- uses a generated PKCS#12 (`P12`) token for this lab workflow.

Add an End Entity with a unique username and enrollment password. Use a subject
such as:

```text
CN=<operator name>,OU=Firmware Update Operators,O=HiTEKS,C=IT
```

The exact `OU=Firmware Update Operators` is required: the updater rejects
operator certificates without that subject OU.

## 3. Enroll and protect the operator certificate

Use EJBCA RA Web to make the request with the selected End Entity profile:

1. Select **By the CA** for key-pair generation when using the P12 lab path.
2. Select the P12 token type.
3. Enter the existing End Entity username and enrollment password.
4. Download the generated `.p12` file.

Extract the public certificate and encrypted private key in WSL or another
trusted environment. Example paths must be adjusted:

```sh
openssl pkcs12 \
  -in "/mnt/c/Path/Operator.p12" \
  -clcerts -nokeys \
  -out "/mnt/c/Path/operator-certificate.pem"

openssl pkcs12 \
  -in "/mnt/c/Path/Operator.p12" \
  -nocerts \
  -out "/mnt/c/Path/operator-private-key.pem"
```

When OpenSSL asks for a PEM passphrase, choose and retain one securely. It is
needed every time a USB authorization bundle is signed.

Confirm the certificate and private key match without revealing either secret:

```sh
openssl x509 -in operator-certificate.pem -pubkey -noout |
  openssl pkey -pubin -outform DER | sha256sum

openssl pkey -in operator-private-key.pem -pubout |
  openssl pkey -pubin -outform DER | sha256sum
```

The two hashes must be identical.

## 4. Build and sign an APK

Every update APK is signed with the dedicated manufacturer release private key.
The matching public key is already in TPM NV `0x1500019`.

### Build CCLI

1. Bump `apps/ccli/VERSION` release number and sync `package/ccli/Makefile`.
2. Add a changelog entry.
3. Build with the OpenWrt SDK script:

```sh
export CCLI_APK_SIGN_KEY=/mnt/c/secure/ccli-release.pem
export CCLI_APK_SIGN_PUB=/mnt/c/secure/ccli-release.pub.pem
bash lab/tg544-openwrt/build-ccli-ipk-sdk.sh
```

The output used in the successful test was:

```text
lab/tg544-openwrt/ccli-0.1.0-r53.apk
```

### Build the updater itself

```sh
export CCLI_APK_SIGN_KEY=/mnt/c/secure/ccli-release.pem
export CCLI_APK_SIGN_PUB=/mnt/c/secure/ccli-release.pub.pem
bash apps/firmware-updater/build-signed.sh
```

The latest signed updater output is:

```text
apps/firmware-updater/dist/firmware-updater-1.1.0-r7.apk
```

The scripts reset APK signatures, apply the release signature, and verify it
with the matching public key before copying the final artifact to `dist/`.

## 5. Create a one-time USB authorization bundle

The bundle creator does all of the following:

1. Checks the operator certificate is a non-CA certificate with
   `digitalSignature` and the required subject OU.
2. Generates a UUID transaction ID.
3. Binds it to one TesPro device ID, an operator role, expiry window, exact APK
   filename, and SHA-256 hash.
4. Signs `update-manifest.json` using the encrypted operator private key.
5. Verifies the signature against the operator certificate.

Bundle layout:

```text
update-<transaction-id>/
├── ccli-<version>.apk                 # or firmware-updater-<version>.apk
├── operator-certificate.pem           # public
├── update-manifest.json
└── update-manifest.json.sig
```

Never copy `operator-private-key.pem`, the P12 file, an enrollment password,
or a manufacturer release private key to the USB drive.

### Generic bundle command

In a visible PowerShell window, run:

```powershell
powershell.exe -ExecutionPolicy Bypass -File .\scripts\new-firmware-update-bundle.ps1 `
  -Package <signed-apk-path> `
  -OperatorCertificate <operator-certificate.pem> `
  -OperatorPrivateKey <operator-private-key.pem> `
  -DeviceId CCI-0008CCFD041F `
  -OutputDirectory <bundle-output-directory> `
  -ValidForMinutes 240
```

Enter the private-key passphrase only at the local OpenSSL prompt. Nothing may
appear while it is typed; that is normal.

### CCLI helper command used in the test

```powershell
cd "C:\Users\Administrator\Desktop\CCLI Alireza\CCLI"
powershell.exe -ExecutionPolicy Bypass -File ".\scripts\new-ccli-usb-bundle.ps1" `
  -UsbDriveLetter D -ValidForMinutes 240
```

It copies a hash-verified bundle to `D:\CCI-CCLI-TEST\update-<UUID>`.

## 6. Install from USB and monitor

Before insertion, confirm baseline state on the TesPro:

```sh
date -u
/usr/sbin/firmware-updater --status
/etc/init.d/firmware-updater status
/etc/init.d/ccli status
apk list --installed | grep -E '^(ccli|firmware-updater)-'
```

Insert the USB drive. The updater scans it automatically. Monitor with:

```sh
logread -f -e firmware-updater
tail -f /mnt/mmcblk0/firmware-updater/security-audit.log
```

Expected successful event sequence:

```text
BUNDLE_VERIFIED
CCLI_DEACTIVATED
AUTHORIZATION_ACCEPTED
INSTALL_STARTED
CCLI_RESTORED
UPDATE_SUCCEEDED
```

If CCLI is absent and the bundle contains a `ccli-*.apk`, updater r6 emits
`CCLI_NOT_INSTALLED`, installs the package, then emits `CCLI_STARTED`. This is
the supported initial-install/recovery path. A bundle containing another signed
package (for example, a future watchdog) does not require CCLI to be installed
and does not try to start it.

Then confirm:

```sh
apk list --installed | grep -E '^(ccli|firmware-updater)-'
/etc/init.d/ccli status
pgrep -af 'ccli|firmware-updater'
```

After success, remove the USB. The transaction is deliberately consumed and
remaining scans will log an expected replay rejection for that same bundle.

## 7. Important failure behavior and updater fixes

The earlier updater (`1.1.0-r3`) consumed the transaction before attempting to
deactivate CCLI. Its CCLI init script could return non-zero despite CCLI having
stopped, causing this failure:

```text
UPDATE_ABORTED: CCLI deactivation failed
```

No APK was installed, but the authorization was incorrectly consumed. This was
fixed in `firmware-updater 1.1.0-r5`:

- it judges CCLI shutdown by confirmed inactive state, not just the stop command
  exit code;
- it does not consume the bundle if CCLI cannot be stopped;
- it does not consume the bundle on APK simulation or installation failure;
- it records the transaction only after the APK manager reports installation
  success;
- it restarts CCLI after the update attempt.

`firmware-updater 1.1.0-r6` additionally supports an absent CCLI init service:

- it treats a missing `/etc/init.d/ccli` as an initial installation or recovery
  condition, rather than an update failure;
- after a successful bundle containing `ccli-*.apk`, it starts the newly
  installed CCLI service;
- bundles for unrelated signed APKs, such as a future watchdog, continue
  without requiring or starting CCLI.

`firmware-updater 1.1.0-r7` resolves the asynchronous-stop edge case observed
during the r6 update:

- it checks CCLI status once per second for up to five seconds after requesting
  shutdown before declaring deactivation failed;
- a failed deactivation attempt explicitly ensures CCLI is running before it
  returns;
- after every completed installation, it ensures the CCLI service is running
  whenever the service exists;
- its package post-install hook also starts CCLI, protecting the r6-to-r7
  self-update path where the older updater owns the transaction.

The fixed updater was bootstrapped once over controlled SSH after its signature
was verified using the TPM-held manufacturer public key. Future updater updates
can use the regular signed USB bundle process.

## 8. Troubleshooting

| Symptom | Meaning / action |
| --- | --- |
| `one-time transaction was already consumed` | The bundle already installed successfully or was consumed by old updater r3. Create a fresh bundle with a new UUID; do not reuse it. |
| `authorization is expired` | Correct TesPro UTC time and create a fresh short-lived bundle. |
| Operator certificate rejection | Check CA/CRL in TPM, `CA=FALSE`, `digitalSignature`, and `OU=Firmware Update Operators`. |
| APK signature rejection | Rebuild/sign with the manufacturer release key whose public half is in TPM NV `0x1500019`. |
| CCLI fails to restore | Do not continue with another update. Inspect `/etc/init.d/ccli status`, CCLI logs, and updater audit log first. |
| Repeated replay rejections | A successfully used USB bundle is still connected; remove it. |
| Device has no USB mounted but old messages appear | Check `mount` and `/mnt/sda1`; log messages may be historical. |

## 9. Evidence to retain

For each update, retain in controlled storage:

- APK SHA-256 and manufacturer signing-key identity;
- the public operator certificate fingerprint and EJBCA issuance/revocation
  evidence;
- bundle transaction ID, device ID, creation/expiry time, and operator role;
- TPM provisioning readback record and NV index references;
- TesPro persistent audit extract around the update;
- installed version and service-health confirmation after reboot when required.

Do not retain private keys, P12 passwords, enrollment passwords, or TPM owner
authorization in ordinary evidence folders.

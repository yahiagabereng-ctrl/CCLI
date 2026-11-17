# Signed APK and USB update procedure

The canonical local toolkit is `apps/firmware-updater`. Private signing and PKI
keys are committed only to the private CCLI GitHub repository; generated artifacts
and `tools/bin/apk` remain local. WSL with Bash, OpenSSL, Python 3, and standard
Linux utilities is required. The encrypted operator-key passphrase must be entered
locally. Building CCLI itself requires the full repository source tree and OpenWrt
SDK. A fresh clone from the private GitHub repository includes keys but not the APK
tool binary, local artifacts, or bundles; see “Preparing a fresh clone.”

## Files

| Location inside firmware-updater | Purpose |
| --- | --- |
| `keys/manufacturer/ccli-release.pem` | Existing manufacturer private signing key |
| `keys/manufacturer/ccli-release.pub.pem` | Matching APK verification key |
| `keys/operator/operator-private-key.pem` | Encrypted operator manifest-signing key |
| `keys/operator/operator-certificate.pem` | Public operator identity |
| `keys/operator/Alireza.p12` | Original password-protected operator keystore |
| `keys/provisioning/operator-ca.pem` | Public operator CA certificate |
| `keys/provisioning/operator-crl.pem` | Public CRL; refresh before it expires |
| `artifacts/` | Local signed CCLI APKs and build metadata |
| `bundles/` | Newly signed one-time bundles |
| `scripts/` | Build entry point, signature verification, bundle creation |
| `tools/bin/apk` | Local Linux APK signer/verifier; no SDK lookup |
| `files/firmware-updater-provision-trust` | Device TPM commissioning tool |
| `local-documents/` | Historical workstation runbooks, retained as reference |

Private signing and PKI keys are committed to the private GitHub repository and
remain protected by its access controls. Generated artifacts, bundles, and the local
APK tool are Git-ignored. The keys directory has restricted Windows ACLs. Do not
copy the entire toolkit to USB: copy only a generated `update-*` directory. Neither
private key belongs on TesPro. The current workstation keys were copied, not moved
or regenerated.

### Preparing a fresh clone

Install WSL 2 with a supported Linux distribution and install `bash`, `openssl`,
`python3`, `coreutils`, `findutils`, and `grep`. Clone the private repository and
open WSL at `apps/firmware-updater`. The clone includes `keys/`; keep it restricted
to authorized users. It does not contain `artifacts/`, `bundles/`, or `tools/bin/apk`.
Create those folders and provision local artifacts and the APK tool through the
organization's approved process; do not add generated artifacts or tools to GitHub:

```sh
mkdir -p artifacts bundles tools/bin
```

For bundling an APK that was already signed by this manufacturer, the minimum
inputs are the signed APK, `keys/manufacturer/ccli-release.pub.pem`,
`keys/operator/operator-certificate.pem`, and the matching encrypted
`keys/operator/operator-private-key.pem`. The manufacturer private key is not
needed to bundle an already signed APK. It is needed to sign a new APK. The CA,
CRL and P12 are not read during bundle creation: CA/CRL are device provisioning
inputs, and P12 is only the enrollment container from which PEM material may be
exported. Keep them in the local toolkit if the local operator is responsible
for renewals or device provisioning.

The device must already trust the operator's issuing CA and current CRL in TPM
NV `0x1500017`/`0x1500018`, and its manufacturer key in `0x1500019`. A different
manufacturer key cannot update that device until an authorized commissioning
process changes TPM trust. An operator certificate from a different CA likewise
requires an authorized trust update. Do not generate replacement keys casually.

The APK verification executable is deliberately Git-ignored. Bootstrap it from
an extracted OpenWrt SDK once:

```sh
bash ./scripts/bootstrap-apk-tool.sh
```

It copies `staging_dir/host/bin/.apk.bin` to this folder. `OPENWRT_SDK` may point
to the SDK directory. Once copied, bundling does not need the SDK. Provision the
signed APK locally in `artifacts/` by secure transfer or rebuild it as described
below. Confirm all four bundle inputs are present before proceeding.
Passwords are not included in scripts or documentation. A lost P12 password does
not invalidate a usable extracted PEM key; PEM and P12 passwords can differ.

## Certificate issuance and public trust

In EJBCA, use the dedicated firmware operator CA, not ManagementCA. Create an
end-entity certificate profile with CA=FALSE, Digital Signature key usage, and
the required operator identity OU `Firmware Update Operators`. Restrict available
CA/profile choices in the end-entity profile. Create the actual end entity and
enroll it as P12; creating a profile alone does not issue a certificate. Resetting
an end-entity enrollment password does not change an existing P12 password.

Extract the public certificate with `openssl pkcs12 -clcerts -nokeys`, and the
encrypted private key with `openssl pkcs12 -nocerts`. Keep the PEM private key
encrypted. Confirm that public keys derived from certificate and private key match.
Download the dedicated CA PEM and current CRL from **CA Functions → CA Structure
& CRLs**. CA settings View is not the certificate download page.

For a newly enrolled operator P12 (substitute its actual filename for
`operator.p12` below), run these from the toolkit directory in WSL;
OpenSSL prompts for the P12 import password and then for a new PEM encryption
passphrase. Do not put either password on the command line:

```sh
openssl pkcs12 -in keys/operator/operator.p12 -clcerts -nokeys \
  -out keys/operator/operator-certificate.pem
openssl pkcs12 -in keys/operator/operator.p12 -nocerts \
  -out keys/operator/operator-private-key.pem
openssl x509 -in keys/operator/operator-certificate.pem -pubkey -noout \
  | openssl pkey -pubin -outform DER | sha256sum
openssl pkey -in keys/operator/operator-private-key.pem -pubout -outform DER \
  | sha256sum
```

The two SHA-256 public-key fingerprints must match. Verify the certificate against
the issuing CA and current CRL before use:

```sh
openssl verify -CAfile keys/provisioning/operator-ca.pem \
  -CRLfile keys/provisioning/operator-crl.pem -crl_check \
  keys/operator/operator-certificate.pem
```

At commissioning, copy only public provisioning inputs to TesPro and run:

```sh
firmware-updater-provision-trust operator-ca.pem operator-crl.pem \
  ccli-release.pub.pem CCI-0008CCFD041F
```

The CA, CRL and manufacturer public key occupy TPM NV `0x1500017`, `0x1500018`,
and `0x1500019`. Existing indexes must not be overwritten casually. Ordinary
bundle creation does not require provisioning again. Device UTC, certificate
validity and CRL validity must all be correct.

## Build and verify

Run from the `CCLI Merged` repository root in PowerShell:

```powershell
wsl bash ./apps/firmware-updater/scripts/build-ccli.sh
wsl bash ./apps/firmware-updater/build-signed.sh
```

CCLI uses the 25.12.5 mediatek/filogic SDK and source tree in this checkout; the
updater uses the 25.12.2 SDK. Both target `aarch64_cortex-a53`. The CCLI wrapper
uses `keys/manufacturer/ccli-release.pem` and its public partner, verifies the
result, and copies it to `artifacts/`. The updater build uses the same local key
pair and writes `dist/firmware-updater-1.1.0-r8.apk`. SDKs are not included in Git;
install/extract them separately and set `OPENWRT_SDK_HOME` (CCLI) or `OPENWRT_SDK`
(updater) if they are not under the WSL home directory. Use a higher package
release for normal upgrades. Same-version bundles do not reinstall an already
installed package. A recovery install works when CCLI is absent.

The current local `ccli-0.1.0-r60.apk` metadata records source commit
`76139dbbee893405c40cd14c4a1f82b326a4fa6f`, SDK 25.12.5, and SHA-256
`97cc15959b1040dffbe78618d6b62d853ddde30474d9c7329a4b069284545889`. The APK
and metadata are ignored local artifacts, so they will not come from GitHub. To
recreate an identical binary, use that exact source revision and SDK plus the
same build environment; otherwise the current checkout's `PKG_RELEASE` produces
a different release. Always record new source commit, SDK, APK hash and signature
verification alongside a release artifact.
Other service APKs require matching architecture, declared dependencies and
root-owned executable init scripts. The updater's readiness check is CCLI-specific;
it does not prove an unrelated application's health.

To manufacturer-sign another already built APK, run
`wsl bash ./apps/firmware-updater/scripts/sign-apk.sh PATH_TO_APK`.
It verifies the signature and writes `artifacts/<original-filename>`; it refuses
to overwrite an existing artifact. This does not build or validate app behavior.

## Create a bundle

From the `CCLI Merged` repository root, change into this toolkit, then run the
read-only preflight and canonical command:

```powershell
Set-Location .\apps\firmware-updater
wsl bash ./scripts/check-bundle-inputs.sh ./artifacts/ccli-0.1.0-r60.apk
wsl bash ./scripts/new-bundle.sh ./artifacts/ccli-0.1.0-r60.apk CCI-0008CCFD041F 60
```

The r60 filename is the current workstation example. A fresh clone has no APK
artifact; build or securely transfer the approved signed APK, then substitute its
actual filename in both commands. After a source build, the package release number
in the filename may differ from r60.

Use only this toolkit for future bundles. Preflight checks the local APK signature,
operator certificate constraints and required runtime/files. It does not unlock
the encrypted private key; final signing and key/certificate matching are verified
when the passphrase is entered during bundle creation.

The canonical wrapper defaults all certificate, private-key and output paths to
this toolkit. To use a different APK, pass its path as the first argument; the
wrapper still takes operator keys and bundle output from here. The low-level shell
script accepts explicit paths for automation, but normal use should go through
`scripts/new-bundle.sh`.

```powershell
wsl bash ./scripts/new-bundle.sh `
  ./artifacts/ccli-0.1.0-r60.apk `
  CCI-0008CCFD041F 60
```

Run this yourself in a visible PowerShell window. OpenSSL asks for the operator
PEM key passphrase there. Do not paste it into chat. Default validity is 60 minutes;
the script allows 1–1440 minutes, subject to device authorization policy. A fresh
transaction ID is generated every time. APK signatures are checked before signing
the manifest. Only printout `USB update bundle created` confirms completion.

The output is `apps/firmware-updater/bundles/update-<UUID>/` containing the APK,
operator certificate, manifest and manifest signature. Copy the complete directory
to the USB root or `updates/` directory, verify copied hashes, then eject the USB
from the PC and connect it to TesPro. Never copy `keys/` or password files.

`scripts/new-firmware-update-bundle.ps1` supports 1–32 APKs and USB hash verification,
but requires modern PowerShell/.NET PEM support and WSL for APK verification.
Windows PowerShell 5.1 lacks the required PEM API; use the WSL shell workflow above.
The shell helper accepts one APK. For multiple APKs use PowerShell 7 with explicit
Package, OperatorCertificate, OperatorPrivateKey, DeviceId and OutputDirectory.

## Updater r8 completion behavior

The device verifies the operator, manifest, device ID, time, replay state, package
hashes and every manufacturer APK signature before installation. It quiesces CCLI,
installs via apk, verifies existing configuration/credential file hashes, checks
that no `.pending` marker remains, then clears the new `.hold` marker. Existing
holds abort a new transaction for investigation. Preservation failures retain the
hold. A healthy CCLI instance and MMS port 3782 must pass three consecutive checks;
`active with no instances` no longer counts as running.

Transactions are consumed after successful package installation, even if startup
later fails. Investigate the installed state rather than replaying authorization.
`UPDATE_SUCCEEDED` requires the additional startup checks. Logs are available via
`logread` and `/mnt/mmcblk0/firmware-updater/security-audit.log`. A running instance
and port listener prove startup, not full application conformance.

For a test, first confirm the intended APK/version and device. Removing CCLI is a
separate explicit action; back up `/etc/ccli` and `/etc/config/ccli` before removal.
Let USB insertion trigger installation. Do not substitute an SSH APK installation
when testing USB behavior. Inspect recent timestamps and transaction IDs to avoid
mistaking historical logs for the current test.

The device scans removable-media mount points recursively (up to the configured
depth) for `update-manifest.json`; placing the complete generated folder at the
USB root or under `updates/` works. Keep each bundle directory intact. Do not
manually edit the manifest or reuse a consumed transaction. The installer uses
the package filename/hash listed in the signed manifest.

## Verification record

Updater r8 was installed over SSH with its signature checked against the TPM key.
Sixteen regression tests passed, including an empty-procd-instance regression.
CCLI preservation was checked, its held installation recovered, and port 3782
confirmed on three checks. The operator subsequently reported the USB test worked.
CCLI r59 was then explicitly uninstalled for a recovery test. A newer signed r60
artifact is present in this merged toolkit; verify the desired artifact before
authorizing a new USB update. The current merged source tree's CCLI package release
is higher than r60, so a rebuild from this checkout creates a newer package.

The toolkit's end-to-end test can be run from this directory:

```powershell
wsl bash ./scripts/test-bundle-self-contained.sh
```

It makes a temporary copy of the local APK, signs it with the local manufacturer
key, creates a disposable test operator certificate/key under `/tmp`, builds and
verifies a bundle, checks its manifest hash, and confirms no private key is in
the bundle. It does not use the real operator key, create a deployable bundle,
touch TesPro, or require the OpenWrt SDK. This test passed on 2026-10-20. A real
bundle still requires the real encrypted operator key and its local passphrase.

## GitHub publication checklist

Before pushing, confirm `keys/`, `artifacts/`, `bundles/`, `local-documents/`,
and `tools/bin/` are ignored and no secret or release APK is staged. Publish the
scripts, package source, docs, tests, and license/provenance notices for any
redistributed third-party tools. Do not commit manufacturer/operator private
keys, P12 containers, passwords, signed customer bundles, or device-specific
credentials. A GitHub collaborator must obtain authorized keys/certificates
through a secure channel and must use the manufacturer key already trusted by
the target device, unless the device is deliberately recommissioned.

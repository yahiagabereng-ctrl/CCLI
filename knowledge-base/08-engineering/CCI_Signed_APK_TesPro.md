# Creating a TesPro-Trusted Signed APK

This runbook describes the tested process for building and installing a CCLI APK accepted by TesProOS (`apk-tools 3.0.2`).

## Signing model

- Package format: APK v3 (OpenWrt APK package format).
- Signing key: an EC P-256 private key in PEM format.
- Trust anchor: the matching public key installed in `/etc/apk/keys/` on TesPro.
- The private key must never be copied to the device or committed to Git.
- The final package must verify with the matching public key before installation.

The device’s existing OpenWrt/vendor keys should be retained. Add or replace only the CCLI-specific trust key unless the firmware vendor explicitly instructs otherwise.

## 1. Generate a fresh key pair

Run in WSL/Linux. Store the keys outside the repository:

```sh
umask 077
mkdir -p /mnt/c/Users/Administrator/.ccli-signing
openssl ecparam -name prime256v1 -genkey -noout \
  -out /mnt/c/Users/Administrator/.ccli-signing/ccli-release.pem
openssl ec -in /mnt/c/Users/Administrator/.ccli-signing/ccli-release.pem \
  -pubout > /mnt/c/Users/Administrator/.ccli-signing/ccli-release.pub.pem
```

Check that the pair matches:

```sh
openssl ec -in ccli-release.pem -pubout -outform DER 2>/dev/null | sha256sum
openssl pkey -pubin -in ccli-release.pub.pem -pubout -outform DER 2>/dev/null | sha256sum
```

The two hashes must be identical. Keep the private key in a protected location and maintain an offline backup.

## 2. Build and sign the APK

From the repository root, set the SDK and signing-key paths, then run the checked-in build script:

```sh
export OPENWRT_SDK_HOME=/home/adminavz
export CCLI_APK_SIGN_KEY=/mnt/c/Users/Administrator/.ccli-signing/ccli-release.pem
export CCLI_APK_SIGN_PUB=/mnt/c/Users/Administrator/.ccli-signing/ccli-release.pub.pem

bash lab/tg544-openwrt/build-ccli-ipk-sdk.sh
```

The script refuses to build if the key pair is missing. It signs the final APK using `apk adbsign` and writes the result to:

```text
lab/tg544-openwrt/ccli-0.1.0-r34.apk
```

`--allow-untrusted` in the build script applies only to the unsigned intermediate artifact while signing. It must not be used for device installation.

## 3. Verify the package locally

Create a temporary trust directory containing only the public key:

```sh
rm -rf /tmp/ccli-public-key
mkdir -p /tmp/ccli-public-key
cp /mnt/c/Users/Administrator/.ccli-signing/ccli-release.pub.pem \
  /tmp/ccli-public-key/ccli-release.pem

apk --keys-dir /tmp/ccli-public-key \
  verify lab/tg544-openwrt/ccli-0.1.0-r34.apk
```

The result must be `OK`. Do not continue if verification reports `UNTRUSTED signature`, `ADB block error`, or another error.

Record the artifact hash for transport verification:

```sh
sha256sum lab/tg544-openwrt/ccli-0.1.0-r34.apk
```

## 4. Install the public trust key on TesPro

Connect to the TesPro as root over SSH and back up the current key directory first:

```sh
mkdir -p /root/apk-keys-backup
cp -a /etc/apk/keys/. /root/apk-keys-backup/
```

Install the CCLI public key as `/etc/apk/keys/ccli-release.pem`. Transfer it using the approved SSH file-transfer method for the device. If `pscp`/SFTP is unavailable, stream the binary file through `plink` and verify its hash afterward.

On the device:

```sh
chmod 0644 /etc/apk/keys/ccli-release.pem
sha256sum /etc/apk/keys/ccli-release.pem
```

The device hash must match the public-key hash recorded on the build host. Never copy the private `.pem` key to TesPro.

## 5. Transfer and verify the APK

Copy the APK to a temporary device location, for example:

```text
/tmp/ccli-install/ccli-0.1.0-r34.apk
```

Check the device-side hash:

```sh
sha256sum /tmp/ccli-install/ccli-0.1.0-r34.apk
apk verify /tmp/ccli-install/ccli-0.1.0-r34.apk
```

Both the hash and verification must match the build host. The verification result must be `OK`.

## 6. Install without bypassing trust

Only after successful verification:

```sh
apk add /tmp/ccli-install/ccli-0.1.0-r34.apk
```

Do **not** use `--allow-untrusted`. That option disables the trust check and is not an acceptable production installation method.

Confirm installation:

```sh
apk info -e ccli
apk info -L ccli
```

The executable is installed at `/usr/sbin/ccli`; the service definition is `/etc/init.d/ccli`.

## 7. Post-install checks

```sh
apk info ccli
/etc/init.d/ccli status
ubus call dido_v2 status
uci show ccli
```

If the application is intended to start automatically, enable it explicitly after reviewing `/etc/config/ccli`:

```sh
/etc/init.d/ccli enable
/etc/init.d/ccli start
```

## Key rotation and recovery

For a future key rotation:

1. Generate a new pair outside the repository.
2. Install the new public key on TesPro while retaining the old CCLI public key.
3. Build and verify packages with the new private key.
4. Install and test the new package.
5. Remove the old CCLI public key only after all rollback packages using it are retired.

Keep the firmware/vendor keys (`openwrt-snapshots.pem`, `public-key.pem`, or vendor-provided equivalents) unless TesPro documentation explicitly says they may be removed.

## Troubleshooting

- `UNTRUSTED signature`: the device does not have the matching public key, the wrong public key filename/content was copied, or the package was signed with a different private key.
- `ADB block error`: the APK was malformed or was processed through an incompatible signing step; rebuild from source and verify locally before transfer.
- Package hash mismatch: the transfer was corrupted or the wrong artifact was copied; do not install it.
- Installation succeeds but the service has no instance: inspect `/etc/config/ccli`; installation and service enablement are separate operations.

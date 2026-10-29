# TESPRO USB APK updater

This service automatically installs or upgrades signed TesproOS `.apk`
packages found on mounted USB storage. It is intended for Alpine/OpenWrt APK
packages, not Android application APKs.

## Behavior

- Scans mounted `/dev/sd*` partitions every 15 seconds.
- Finds `.apk` files up to eight directory levels below each mount point.
- Copies the full package set to temporary local storage before installation.
- Simulates the transaction first, then installs the package set together so
  dependencies can be resolved between packages on the USB device.
- Disables network package fetching during the transaction.
- Requires packages to be signed by a key already trusted in
  `/etc/apk/keys`. Network package fetching remains disabled.
- Records a SHA-256 hash of successful or failed package sets so unchanged
  media is not processed repeatedly.

Logs are written to `/tmp/tespro-usb-apk-update.log` and the system log.

## TESPRO commands

```sh
/etc/init.d/tespro-usb-apk-update status
/usr/sbin/tespro-usb-apk-update status
/usr/sbin/tespro-usb-apk-update retry
logread -e tespro-usb-apk-update
```

To retry a corrected package set without unplugging the USB device, run the
`retry` command. The updater can be put into an explicitly unsafe lab mode by
setting `allow_untrusted` to `1` in `/etc/config/tespro-usb-apk-update`, but
that means physical access to the USB port is sufficient to install code as
root.

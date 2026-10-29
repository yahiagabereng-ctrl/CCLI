# usb-test

Harmless TesproOS APK package for testing the USB updater. Version 1.0.2-r1
starts a small background service at boot and reports its boot ID and start
time at:

`http://TESPRO-IP/cgi-bin/usb-test`

Installation creates marker files at `/usr/share/usb-test/README.txt` and
`/etc/usb-test/installed.txt`.

The private signing key is kept outside the repository. The signed artifact is
generated in `dist/` and verified against the TESPRO CCLI public key before
deployment.

# Local APK tool

`bin/apk` is a local copy of the actual Linux x86-64 host APK binary from the
OpenWrt 25.12.2 mediatek-filogic SDK's `staging_dir/host/bin/.apk.bin`.
It uses the WSL system C library and loader; it is not the SDK launcher script.
It verifies and signs APK v3 packages through WSL without referencing that SDK.
The executable is a local Git-ignored dependency; preserve it when copying this
toolkit to another workstation. OpenSSL, Python 3 and Bash must be installed in WSL.

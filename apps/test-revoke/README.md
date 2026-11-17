# test-revoke

An intentionally unsigned TesproOS/OpenWrt APK used to test firmware updater
signature rejection. It contains an init script named `test-revoke`; if the APK
is manually installed, the post-install hook enables and starts it so it is
listed by LuCI at **System -> Startup**.

The build output is deliberately not passed through `adbsign`.

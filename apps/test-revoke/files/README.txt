test-revoke 1.0.0-r1

This package is intentionally unsigned. It is for testing signature-revocation
and updater rejection behavior. Do not use it as production firmware.

If it is manually accepted and installed, postinst enables and starts
/etc/init.d/test-revoke. The service should then appear in LuCI under
System -> Startup and can be controlled there.

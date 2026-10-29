# Tespro r64 Aggregator live validation

Date: 21 October 2026, 09:34 UTC  
Target: Tespro at 192.168.0.1  
Installed package: ccli-0.1.0-r64, release 64  
APK: apps/firmware-updater/artifacts/ccli-0.1.0-r64.apk  
APK SHA-256: 2694837eed4cdd59e00335e31e6b5508b3b050a4a4cac02906de82a9af380efb  
APK size: 1,087,028 bytes

## Installation and active policy

The APK was built with the canonical firmware toolkit and passed its manufacturer-signature verification. It was installed over SSH with the user's authorization. No reboot was performed.

The CCLI process was running as PID 20894 and listening on 192.168.0.1:3782 at the final check. The active Profile A activation record is sequence 4, digest 5f4b7dde602f7ce20bc5004de633f8098404a9f5aeca000ebc4db1e337dfba8b. The Aggregator commissioning policy includes the read-only dataset and report scopes added for this release.

All 27 configured TLS/RBAC certificate, key-reference, trust and CRL files matched the root-only pre-upgrade backup byte-for-byte. No TLS/ACSE/session/presentation implementation or credential was changed. Private keys were not copied to the device by this task.

## IEC 61850 client checks

- Mutual TLS 1.2 and Profile A role authentication: passed.
- Aggregator status reads: passed. P and Q modes read back as -5/off; Beh=5, Health=1 and FctOpSt=0 for both.
- Static dataset membership and dataset read: passed.
- P setpoint operation at 30% and Q setpoint operation at 20%: accepted with successful enhanced-control termination and matching readback. Restored to P=100% and Q=0%.
- P and Q mode transitions to -1/on and back to -5/off: accepted with successful termination and matching readback.
- DSO-only PdCMMXU1.TotW read under the Aggregator identity: correctly denied with access-denied.
- BRCB subscription replayed 93 buffered reports; some carried old or epoch measurement timestamps. This is consistent with buffered reports being retained while no client is subscribed, and is not evidence of 93 new simulator events. The buffer-overflow flag was not checked; treat old entries as historical and check timestamp/quality before using them.
- URCB first listened for 3 seconds, shorter than its 4-second integrity period, with no data changes; zero reports was expected. The client nevertheless labels zero events as failure. A 6-second retest received one periodic report and passed; the client then disabled the RCB.
- Unbuffered Aggregator report subscription: enable/disable succeeded, but no report arrived while values were static. The client therefore returned failure for the no-report result. Both report control blocks were confirmed disabled afterward.

One Q setpoint request sent less than the configured 3-second minimum interval after another setpoint was rejected. The retry after waiting passed; this is the intended spacing gate.

## Plant output and telemetry state

Plant outputs remained disabled. Final state read from the device: P mode -5, Q mode -5, P target 100%, Q target 0%, wsa_active=false, varsa_active=false, do_curtail_on=false, p_wsa_kw=0, and p_kw/q_kvar=0. No simulator or COM8 action was performed.

The user confirmed the inverter simulator was not running. Therefore zero/stale Modbus measurements were expected for this validation and are not an Aggregator control failure. MMS still returned Good quality flags with zero values while device status was stale; verify quality propagation when the simulator is running before treating live values as valid.

The live tests validate the installed Profile A/MMS role boundary, Aggregator model, controls, dataset and report services. They do not prove physical P/Q actuation. The current device continues to use its existing handbuilt model; the new generated CID/CFG was not selected.
# Tespro native E2E activation attempt — 2026-10-27

Attempted native-only E2E using the existing application certificate/key on a separate on-device component harness (MMS 3783, synthetic measurements, no controller or plant I/O). The working CCLI process and configuration on MMS 3782 were preserved.

Firmware: installed CCLI 0.1.0-r70. Diagnostic harness: output/iec62351-tls/device/mms-rbac-device-server.

Candidate configuration: /tmp/ccli-native-e2e-attempt.yaml. Candidate policy and activation state: /etc/ccli/rbac-test-20261016/native-e2e-attempt/. The complete existing signed XML package was copied to preserve relative references and signatures; no owner policy changes were made.

Native candidate used the existing server application certificate /etc/ccli/tls/server.pem, its separate application key /etc/ccli/tls/server_acse.key, explicit trust/CRL bundle, AP-title 1.1.1.999.1, AE qualifier 12, RSA-SHA256, 900-second rekey interval, compatibility disabled.

## Observed result

Exit status 1. The owner-signed commissioning policy sequence 4 activated successfully; TLS CRL loaded, responder authentication was ready, and native configuration then failed with:

    mms: native E2E startup refused: server application certificate does not satisfy Annex G/trust/CRL/identity policy

No native listener started. Existing compatibility certificate subject encoding is unsuitable for the strict native validator: native subject requires a single 2.5.4.106 attribute with ASN.1 OBJECT IDENTIFIER value containing AP-title plus AE qualifier, rather than the existing textual value and extra subject fields. Do not weaken validation to admit these credentials.

Native E2E is NOT active. No protected native read/rekey/release or external interoperability result is claimed. Correct native server and peer application credentials, current application revocation coverage and a signed peer identity binding remain necessary. Application confidentiality support remains a separate incomplete implementation item.

EJBCA container ccli-ejbca-ce is running. Public CA inventory includes CCLI-RBAC-Lab-CA, ManagementCA and CCI-Firmware-Operator-CA; the old CCLI-Lab-CA name is absent. No CA, profile, enrollment, key or signed commissioning policy was changed by this attempt.

Production MMS 3782 remained listening with CCLI PID 8784 after the attempt. The isolated harness terminated on validation failure.

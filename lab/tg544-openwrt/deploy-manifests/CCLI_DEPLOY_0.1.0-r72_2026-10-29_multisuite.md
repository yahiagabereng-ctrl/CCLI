# Tespro MMS TLS multiple identities — 2026-10-29

Device: 192.168.0.1:3782. Deployed application: 0.1.0-r72.
Binary SHA256: 37e00e14af4297b86a90094eb7fb24c13dbcb7b4b5301186b50572f0402b7305.
Deployment: direct ARM64 binary installation via SSH; no APK or USB bundle was produced.

The listener has ECDSA P-256 and RSA-3072 server identities. Additional identities undergo certificate/key, chain, revocation, EKU and profile validation. Client cipher ordering is respected.

Post-deployment live OpenSSL probes completed TLS 1.2 mutual authentication with server chain and IP verification OK:
- AES128-SHA256: RSA server certificate.
- DHE-RSA-AES128-GCM-SHA256: RSA server certificate, DH 2048.
- ECDHE-ECDSA-AES128-GCM-SHA256: ECDSA P-256 server certificate.
- Combined offer with AES128-SHA256 first: AES128-SHA256 selected.

Read-only MMS association/browse/read passed on the same implementation before the r72 version-header regeneration. The final r72 check above verifies TLS negotiation; it does not constitute full IEC conformance testing.

Device rollback: /root/ccli-multisuite-r71-20261029-0959 (working multi-suite r71 binary and configuration). Original pre-change rollback: /root/ccli-before-multisuite-20261029-0847.
Device configuration: /etc/ccli/lab.yaml. Additional RSA identity: /etc/ccli/tls/annex-rsa-20261029/. Existing ECDSA identity and RBAC policy retained.

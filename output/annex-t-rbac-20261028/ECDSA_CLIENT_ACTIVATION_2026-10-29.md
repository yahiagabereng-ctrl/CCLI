# Tespro ECDSA client activation and read-only MMS verification

Verified on 29 October 2026 at approximately 08:15 UTC (09:15 Europe/Rome).

## Device activation

- Device: `192.168.0.1`; this PC Ethernet address: `192.168.0.137`.
- Signed RBAC commissioning sequence 6 installed at `/etc/ccli/rbac-dual/package-seq6`.
- Active state: `6 afe667bd7fa0109a6de812199553f4d2880b0a14a422e61b8bf86537f2f6c055`.
- All four package file hashes and the owner manifest signature passed local verification. Remote file hashes matched after transfer.
- All 20 previous identity bindings are preserved. Binding 21 enrolls the ECDSA DSO identity with its ECDSA TLS transport certificate and role `-1`.
- The configuration change affects only `mms.rbac_policy`, selecting sequence 6. The ECDSA TLS peer certificate pin was already present.
- CCLI restarted successfully and remains running with TCP 3782 listening.
- The original sequence 5 package remains on the device. The prior configuration and activation state are retained at `/root/ccli-before-rbac-seq6-20261029-0814` as evidence; do not restore an old activation state to bypass sequence protection.

## Credential verification

Both client keys match their certificates; both public keys use `secp256r1` (P-256). Issuer signatures, validity and revocation checks passed.

| Credential | Certificate SHA-256 | Valid until UTC | DER bytes |
|---|---|---|---:|
| TLS ECDSA client | `0d8b53b4e803b308bc7561302dab1743252bb71f6c500f3da22cc2b7b00c9642` | 2027-10-28 14:01:52 | 817 |
| MMS ECDSA DSO identity | `83994fcbf97d12fcf8f752fcddd6d943f5f212523a46f91985fb44a95179f969` | 2027-10-28 14:38:25 | 840 |

## Results

1. OpenSSL mutual TLS probe: PASS, exit 0. TLS 1.2 negotiated `ECDHE-ECDSA-AES128-GCM-SHA256`; peer `CN=CCLI_Tespro_MMS_ECDSA_P256_Server`; SHA-256 and ECDSA P-256 handshake signature; chain, IP address and CRL verification OK.
2. Custom IEC 61850 client's read-only `check`, using the ECDSA TLS client and separate ECDSA DSO identity: PASS, exit 0.
3. MMS server certificate chain, signed time and authentication signature verified.
4. Mutual TLS and ACSE certificate association succeeded. Server log at 09:14:56 device local time records `mms: ACSE auth ACCEPT — Profile A role token`.
5. Directory returned `CCI016_01LD_Plant`. Reads of `PdCMMXU1.TotW.mag.f`, `TotVAr.mag.f`, `TotW.q` and `TotW.t` succeeded. The values returned were zero, quality `good`, `oldData=0`, `inaccurate=0`, and timestamp `20261029081452.876Z` with `clockNotSynchronized=0`.

This verifies the ECDSA client TLS and MMS DSO authentication path and the requested read access. No MMS writes or control operations were performed. It does not establish the correctness of the external meter readings or full standards conformance.

## CRL freshness at verification

The installed TLS CRL bundle `/etc/ccli/rbac-dual/trust/tls-crl-bundle-20261028.pem` contains:

| Issuer | nextUpdate UTC |
|---|---|
| CCLI_Lab_Root_CA | 2026-10-29 19:36:10 |
| CCLI RBAC Lab CA | 2026-11-02 09:04:57 |
| CCLI_Lab_Root_CA_2026 | 2026-10-29 11:21:01 |

The separate installed RBAC identity CRL expires 2026-11-04 11:21:19 UTC. All were current during testing. The new TLS CA's installed CRL needs refresh before **29 October 2026 12:21:01 Europe/Rome**. The local probe's CRL is an earlier valid issue, expiring at 10:35:43 UTC (11:35:43 Europe/Rome). No CRL was replaced during this activation.

# Tespro validation request: separate IEC 62351-3 and IEC 62351-4 requirements

## Request

Please confirm whether the Tespro test program can operate as an external IEC 61850-8-1 MMS client and validate the CCLI MMS server over TCP. The supplied integration guide shows the opposite direction; for this request, **Tespro is the client and CCLI is the server**.

Please provide a testing application that lets us validate two separate mandatory scopes: (1) IEC 62351-3 TLS transport security and (2) IEC 62351-4 native MMS/E2E application security. It should also let us run a combined TLS plus E2E interoperability check, with output kept separate from each scope's individual results. This document lists the required test capabilities; it is not a request for Tespro to execute the tests or issue a conformance certificate. Use only requirements mandatory for the CEI 0-16 Allegato T profile and the selected IEC profiles. Use TLS 1.2 only. The compatibility A-security profile and optional modes are out of scope. Where IEC 62351-4 marks an algorithm conditional, include it only where the CEI profile below makes it required. Identify any requested test the application cannot perform.

The test endpoint and test credentials can be agreed separately. No IP address or private key belongs in this request or in the test report.

## Shared test context

- **Application protocol:** IEC 61850-8-1 MMS.
- **Stack:** MMS/ACSE over the OSI Session, Presentation and COTP layers, carried over ISO-on-TCP (RFC 1006) and TCP.
- **DUT and test roles:** CCLI is the MMS server; Tespro must initiate the connection as the MMS client.
- **Transport protection:** TLS 1.2, with mutual X.509 certificate authentication and chain validation. Reject lower protocol versions. TLS transport validation is Part A (IEC 62351-3).
- **Application protection:** native IEC 62351-4 E2E for OSI/MMS, using the `e2eMmsAC` application context and `e2eSecurityAS` presentation abstract syntax. The protected protocol is `IEC 61850-8-1`. E2E validation is Part B (IEC 62351-4).
- **Separate certificates:** when TLS and E2E are both used, test and report the TLS certificate and the E2E application-layer certificate separately. The supplied Tespro guide describes TLS certificate handling but does not establish support for a separate IEC 62351-4 application certificate. Please confirm whether the test program can configure, present and validate that E2E certificate.

<!-- pagebreak -->

## Part A - IEC 62351-3: TLS transport requirements

IEC 62351-3 defines the TLS transport requirements and generic TLS behavior. The specific mandatory suite set for this CCLI profile comes from CEI 0-16 Allegato T and, for the native profile, IEC 62351-4. All suites below are tested as TLS transport suites in Part A; the source column identifies which standard makes each requirement mandatory.

### TLS 1.2 cipher suites and mandatory CEI behavior

The application must let us offer each listed suite separately and record whether CCLI negotiates or rejects it. Use the exact IANA name and ID in its configuration and results.

| TLS cipher suite and IANA ID | Mandatory basis | CCLI evidence / requested check |
|---|---|---|
| `TLS_RSA_WITH_AES_128_CBC_SHA256` (`0x003C`) | CEI 0-16 Allegato T and IEC 62351-4 native profile | Negotiated in a live CCLI trace; map the guide's AES-128-CBC-SHA256 label to this IANA name and ID. |
| `TLS_DHE_RSA_WITH_AES_128_GCM_SHA256` (`0x009E`) | CEI 0-16 Allegato T and IEC 62351-4 native profile | Present in the TLS backend suite list; live negotiation has not been confirmed. |
| `TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256` (`0xC02B`) | CEI 0-16 Allegato T and IEC 62351-4 native profile | Missing from the current configured suite list. Include the ECDSA certificate case and report this required gap if Tespro can offer it. |
| `TLS_DH_RSA_WITH_AES_128_GCM_SHA256` (`0x00A0`) | IEC 62351-4 native profile | Required by the native profile; current CCLI negotiation status is unconfirmed. |
| `TLS_RSA_WITH_NULL_SHA256` (`0x003B`) | CEI 0-16 Allegato T; disabled-by-default behavior is required | Verify it is rejected in the default profile. This suite provides no TLS encryption. |

Do not add other TLS 1.2 suites. In particular, do not substitute optional suites for a required suite that cannot be offered or negotiated.

### IEC 62351-3 mandatory TLS configuration and conformance checks

The application must support the applicable mandatory CCLI server-role normal and resiliency cases in IEC TS 62351-100-3:2020. Its configuration and results must cover at least:

- Mutual certificate authentication and chain validation; at least five distinct trusted root CAs; minimum RSA key length 2048 bits and ECDSA key length 256 bits; certificate sizes handled through 8192 octets.
- CRL and OCSP revocation checking. OCSP requests must include a nonce. Test revoked and expired certificates; inability to retrieve revocation information alone must not terminate an established session.
- CRL updates at least every 24 hours; OCSP response cache at most 24 hours. Do not terminate an active session solely because an OCSP cache entry aged past that limit.
- CEI TLS timing: renegotiation aligned with the CRL update period, no shorter than half the CRL update period and never less than 10 minutes; session resumption at least every two hours, shorter than the renegotiation interval and aligned with CRL updates. Verify secure renegotiation and resumption behavior.
- Mandatory rejection cases: version below TLS 1.2, no common required cipher, missing or untrusted certificate, oversized certificate, expired or revoked certificate, below-minimum public key, unsupported certificate-signature algorithm, invalid certificate signature, and invalid renegotiation/resumption behavior.
- Record handshake, renegotiation, resumption, certificate-validation and cipher-failure results, including required security events. If the DUT log is unavailable, provide equivalent verifiable evidence.

<!-- pagebreak -->

## Part B - IEC 62351-4: native MMS/E2E application requirements

Test native E2E separately from TLS, then test the combined TLS plus E2E profile. Use only the CEI-mandatory algorithm set and IEC 62351-4 requirements that apply to the selected native profile. IEC 62351-4 Table 11 marks some encryption and ICV algorithms conditional; include those below because CEI Allegato T requires them for this CCLI profile, and identify CEI as their mandatory basis. Do not describe a conditional Table 11 item as unconditionally mandatory under IEC 62351-4 alone.

| E2E item | Required profile |
|---|---|
| Public-key algorithms and keys | RSA, minimum 2048 bits; EC on secp256r1 and brainpoolP256r1, minimum 256 bits. |
| Signatures | RSA with SHA-256 and ECDSA with SHA-256. |
| Key agreement and derivation | DH Group 14 (2048-bit MODP), mandatory in IEC 62351-4; ECDH Groups 23 (secp256r1) and 28 (brainpoolP256r1) as required by CEI 0-16 Allegato T; HKDF-SHA-256. |
| Integrity and encryption | HMAC-SHA-256 for the ICV; CEI-listed AES-128-CBC, AES-256-CBC, AES-128-GCM and AES-256-GCM. AES-GCM must use authenticated-encryption integrity protection and required nonce handling. IEC 62351-4 marks these modes conditional; they are included here because CEI Allegato T requires this algorithm set. |
| Key refresh | Configurable refresh interval from 15 minutes to 24 hours; exercise the request and key-change handshake. |
| E2E certificate | Separate from the TLS end-entity certificate; X.509 v3, non-negative 128-bit UUID serial number, no issuerUniqueID or subjectUniqueID, `digitalSignature` and `keyAgreement` key-usage bits, required key sizes and SHA-256-or-stronger certificate signatures; certificate size up to 8192 octets. Support CEI-required CRL and OCSP validation, including an OCSP nonce. Verify the OSI certificate identity binding to the MMS AP-title and AE-qualifier. |

The application must let us test each CEI-required key group and algorithm independently where negotiation permits, and show whether CCLI accepts or rejects it.

The application must support test actions and show results for these mandatory E2E behaviors:

1. Establish and release the native E2E MMS association using `e2eMmsAC`, `e2eSecurityAS`, signed handshake tokens and protected protocol `IEC 61850-8-1`.
2. Transfer protected MMS data with integrity/authentication and the CEI-required encrypted profile (`applProtected=1`); report selected key group and algorithms.
3. Exercise mandatory E2E key refresh at the configured interval, including the key-refresh request and key-change handshake.
4. Reject invalid, expired, revoked, untrusted, wrongly bound or incorrectly signed E2E certificates, and reject timestamps outside the 10-minute peer tolerance.
5. Reject modified protected data, invalid ICV or GCM authentication tag, missing CBC IV or GCM nonce, duplicate/replayed association data and invalid or repeated sequence numbers; report the security result/event.
6. Report the E2E result independently from TLS, then report the result when both layers are active.

The Tespro guide documents TLS certificates only. Please state whether it can use a separate MMS/ACSE E2E application certificate and execute these E2E cases. Do not treat a successful TLS handshake as evidence of E2E support.

<!-- pagebreak -->

## Testing application deliverables and capability confirmation

Please provide:

- A capability matrix mapping the requested Part A IEC 62351-3 cases, Part B IEC 62351-4 cases and combined check to the application's supported, partially supported or unsupported functions. Include the applicable standard/test-case references where available and identify any gaps.
- The application and its user/configuration guide, including how to configure it as an IEC 61850-8-1 MMS client connecting to the CCLI server, select TLS suites and certificates, and run each separate profile and the combined check.
- The application's result/log format. It should report negotiated TLS version and IANA suite, TLS certificate outcome, E2E certificate outcome, E2E context/key group/algorithms, MMS/ACSE outcome and relevant security events, while keeping Part A, Part B and combined results distinguishable.
- The CCLI device PICS/PIXIT fields or values the application needs us to provide for setup. PICS/PIXIT describes the CCLI device under test; Tespro is not being asked to create or supply CCLI's PICS/PIXIT.
- An explicit statement of whether the application supports the separate MMS/ACSE E2E application certificate and the E2E test actions listed in Part B. The supplied guide currently documents TLS certificate handling only.

The application must not require us to provide private keys to Tespro. Test endpoint details and credentials can be exchanged separately.

## Standards basis

- CEI 0-16, Allegato T, especially T.3.3.4 and T.3.3.4.1.
- IEC 62351-3:2014 with AMD1:2018 and AMD2:2020 defines the TLS transport requirements; IEC TS 62351-100-3:2020 supplies the applicable TLS conformance cases (Part A).
- IEC 62351-4:2018 with AMD1:2020 defines the native MMS/E2E application requirements and its native-profile TLS suite addition; IEC TS 62351-100-4:2023 supplies applicable MMS/E2E conformance cases (Part B).
- IANA TLS Cipher Suites Registry for exact cipher suite names and identifiers.

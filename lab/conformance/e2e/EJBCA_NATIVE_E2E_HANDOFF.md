# Resume native E2E credential issuance through EJBCA

Updated: 26 October 2026. **Paused at the user's request.** This is a continuation plan, not an issuance or deployment record.

## State when paused

- The user confirmed that EJBCA is the certificate authority. Continue with the existing EJBCA instance and its retained database/CA keys.
- No new certificates, private keys, CSRs, profiles or commissioning packages were created in this continuation. No CA settings, Tespro files or application code were changed. Only this handoff and its links were added.
- Docker Desktop launch was requested with a hidden window. Both normal and outside-sandbox `docker ps` failed because `dockerDesktopLinuxEngine` was unavailable. A running EJBCA instance and its CA inventory have **not** been verified. Do not initialize a replacement CA/database to work around this.
- Windows `python`/`py` resolve to WindowsApps aliases. The bundled Python executable works and has `cryptography 50.0.2`; `pyasn1` was unavailable. No dependencies were installed.
- The fast localhost native stack campaign completed with exit code 0. It exercised RSA/ECDSA, HMAC/GMAC-128/GMAC-256, protected reads/reports, rekey/release, malformed/authentication/abort cases and the product adapter using disposable PKI. Its output was observed in the chat; no new standalone evidence report was archived. This was **not a Tespro probe run**, not the separate real 900-second renewal campaign, and not external conformance evidence.
- The GUI screenshot showed working compatibility MMS reads. That connection does not establish that native E2E is enabled on Tespro.

## Findings to preserve

### Existing EJBCA credentials and issuance code

The repository already contains EJBCA-issued credentials in `apps/ccli/config/tls/ejbca/`, synchronized compatibility/TLS files in `apps/ccli/config/tls/`, and ignored RBAC staging material. The GUI's saved compatibility application identity points into the sibling `CCLI/lab/ejbca-staging/rbac/dual-certificate/` tree. Do not assume the files in the merged repository's TLS directory represent every credential installed on Tespro.

`scripts/issue_ejbca_cli_pki.py` issues through EJBCA `addendentity` and `createcert`. Its `_build_csr` currently:

- builds an application subject with C/O/OU plus `2.5.4.106`;
- passes the AP/AE OID digits to `cryptography.x509.NameAttribute` as a string, producing UTF8String;
- therefore does not create the strict native subject expected by CCLI and the probe;
- removes old enrollment records in `_ensure_end_entity`. Do not run the old issuer unchanged against working identities for this continuation.

The existing EJBCA issuer generates a new key per leaf, with `dual_key=True` for both server and client application identities. **Do not describe all EJBCA credentials as sharing TLS/application keys.** The separate legacy `gen_lab_pki.py` path shares the client key. Verify actual public keys and certificate fingerprints before deciding what needs replacement.

`apps/ccli/config/tls/gen_lab_pki.py` deliberately emits the compatibility UTF8String subject and writes into the existing TLS directory. Its strict ASN.1 helper is not used by its current application-certificate issuance path. It is not the native EJBCA issuance workflow.

### Required application subject

Both `e2e_identity.cpp::matches_subject_identity` and `e2e_probe.py` require the complete application subject to contain **one RDN with one attribute**:

```text
Subject ::= SEQUENCE {
  SET {
    SEQUENCE {
      type  OBJECT IDENTIFIER 2.5.4.106,
      value OBJECT IDENTIFIER <AP-title arcs followed by AE-qualifier>
    }
  }
}
```

The value tag must be `0x06`, not UTF8String `0x0C`. Extra CN/O/OU/C subject fields also fail the current native subject checks. A textual display of the same digits does not prove the required DER encoding.

Application leaves also require X.509 v3, supported strong keys/signatures, `digitalSignature` and `keyAgreement`, valid chain/time, applicable CRL coverage and AP/AE identity matching. Use independent TLS and application key pairs and certificates for each endpoint. The dedicated application profile should not add TLS EKUs.

The project probe currently verifies application certificates directly issued by its configured application CA. For the first bench campaign, select a compatible direct-issuance chain, or explicitly extend and qualify the probe before using an intermediate application chain.

### Trust anchor choice

The existing lab documentation names `CCLI-Lab-CA` and exports its public certificate as `root_CA.pem`; the RBAC workflow separately uses `CCLI-RBAC-Lab-CA`. The actual CA to use for native E2E has **not** been selected or verified against live device configuration.

Use the intended existing EJBCA authority and compare its exported public certificate fingerprint with the intended trust anchor. TLS and native E2E have explicit, separate trust settings (`tls_ca_cert` and `e2e_trust_anchors`). The same public root can be configured for both roles where commissioned policy permits; installation in the TLS trust store alone does not configure native application trust. Never export/install a CA private key on Tespro or in the probe bundle.

## Resume steps, in order

1. **Recover the existing EJBCA service.** Check Docker Desktop/WSL engine health, then inventory existing containers and retained volumes. Start the existing instance without pulling a new image, replacing its database or importing a new CA. Inspect public CA names, status and fingerprints. Current scripts use `https://localhost:9443/ejbca/adminweb/`; older `lab/EJBCA_LAB_SETUP.md` passages still mention port 8443. Confirm the actual port and authentication mode.

2. **Record current commissioning inputs.** Read the actual Tespro configuration, running firmware version, public CA/leaf fingerprints, CRL dates, policy sequence and AP/AE addresses. Confirm the probe peer identity and allowed read object. Preserve rollback material. Do not invent AP/AE identities or assume a root just from its filename. Existing lab files use AP `1.1.1.999.1` and AE `12`, but these must be confirmed for both endpoint roles.

3. **Prepare a dedicated native issuance script and staging directory.** Prefer a new native path over changing the working compatibility issuer. Use a fresh child of the Git-ignored `lab/ejbca-staging/` tree, with current-user/SYSTEM ACLs on Windows or mode 0700 on Linux before writing keys/passwords. Keep CA keys in EJBCA, redact enrollment secrets from error output, use unique new enrollment names and resumable issuance, and do not delete/revoke working enrollments. Private files must stay out of Git, APKs and evidence exports.

4. **Build and verify correctly encoded CSRs.** Generate independent native application keys for server and probe peer. Construct a PKCS#10 request whose complete subject is the single OID-valued RDN above, then sign the final CertificationRequestInfo with the corresponding leaf key. Verify the CSR signature, exact subject DER and public-key match before submission. The existing test fixture helpers demonstrate DER encoding, but disposable fixture CAs/keys must not become deployed identities. Do not patch a certificate after EJBCA has signed it.

5. **Configure an isolated EJBCA native application profile.** Clone/configure a dedicated profile and restricted User Generated end-entity profile; preserve working TLS and compatibility profiles. Enable **Allow Subject DN Override by CSR** for the native profile so EJBCA preserves the CSR subject encoding. Restrict this issuance path to a trusted RA because CSR subject override does not validate the actual DN contents. Set CA=false, mandatory application key usages, supported algorithm/size/signature, validity, certificate storage and applicable CRL distribution points. Keep unrelated extension/key-usage override disabled. Read settings back and stop if unsupported instead of treating CLI warnings as success.

6. **Issue through EJBCA and export public revocation data.** Submit the verified server/peer CSRs through the existing CA's CLI `createcert` or authenticated enrollment interface. Export the public issuing/root certificates and a current EJBCA-signed CRL covering the application leaves. Check certificate distribution-point and CRL issuing-point/reason scope; matching issuer names alone are insufficient. Preserve the existing CA and its revocation database.

7. **Validate before commissioning.** Verify the issued subject bytes equal the requested strict subject, CA signature, leaf/key match, distinct TLS/application public keys, key usages, algorithm/size, validity, certificate size, chain, AP/AE and revocation coverage. Record public fingerprints/serials/expiry in a manifest without secrets. Exercise positive and malformed/wrong-identity/text-valued/extra-subject CSR and certificate cases with meaningful offline checks. The old command below is a useful additional check, but not the complete native gate:

   ```powershell
   python scripts/verify_annex_pki.py --dir <NEW_STAGING_DIRECTORY> --strict-g62
   ```

   Its current G.6.2 check scans for an OID marker and uses a fixed lab identity. It must not be the sole proof of exact subject shape, configurable identities or CRL scope. Extend strict validation or add a dedicated native validator; include the real native C++ identity validator and probe path in the acceptance checks where possible.

8. **Prepare the owner-signed commissioning candidate.** Bind the new application fingerprint to the actual peer TLS fingerprint, actor and permitted roles in the existing XML policy/identity bindings. Preserve existing authorized bindings and advance the actual policy sequence correctly. Authorization remains defined by the commissioned XML. Prepare explicit `e2e_certificate`, `e2e_private_key`, `e2e_trust_anchors`, `e2e_crls`, `e2e_ap_title` and `e2e_ae_qualifier` settings. Choose compatibility/native-only behavior deliberately; the existing GUI cannot connect in native-only mode.

9. **Provision and run the live probe when device work resumes.** Follow [Tespro readiness](TESPRO_LAB_READINESS.md), the canonical firmware toolkit if packaging/signing is required, and the requested installation method. Protect the current bench state, verify UTC/GNSS discipline, then run `e2e_probe.py` against Tespro with the commissioned TLS and application credentials. Capture handshake, protected read/rekey/read/release for HMAC and both GMAC sizes, followed by real timed renewal and device negative/reconnect cases. Distinguish local host results, actual Tespro results and external interoperability evidence. The mandatory application-encryption implementation gap remains separate from testing the existing integrity-only implementation.

## Documentation to correct during implementation

- `lab/EJBCA_LAB_SETUP.md`: its Cert B subject examples and `openssl x509 -subject` comparison are compatibility guidance, not sufficient native Annex G validation. Update the single-attribute DER requirement, trusted CSR-override method, actual admin port and strict release checks.
- `scripts/issue_ejbca_cli_pki.py` and `scripts/issue_ejbca_rest_pki.py`: inspect both CSR builders before any native reuse; do not assume adding `--strict-g62` changes CSR encoding. The flag is a verification switch.
- `scripts/verify_annex_pki.py`: replace marker-only subject checks for a native release gate with structured DER parsing and explicit expected AP/AE inputs.
- Keep the [implementation plan](IMPLEMENTATION_PLAN.md) and [readiness guide](TESPRO_LAB_READINESS.md) linked to this handoff until native credentials and live results are actually recorded.

## Official EJBCA references checked

- [Custom Subject DN and altName OIDs](https://docs.keyfactor.com/ejbca/9.4.2/custom-subject-dn-and-altname-oids): recommends CSR subject override for preserving subject attributes exactly as encoded in a trusted CSR.
- [Certificate Profile Fields](https://docs.keyfactor.com/ejbca/9.3.4/certificate-profile-fields): describes Allow Subject DN Override by CSR, the remaining end-entity registration requirements and the lack of content validation for the overridden subject.

These document the supported route; issuing a strict native certificate successfully on the user's installed EJBCA version remains unverified.

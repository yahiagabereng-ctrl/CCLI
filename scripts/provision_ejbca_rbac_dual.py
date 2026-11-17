#!/usr/bin/env python3
"""Prepare separate MMS role identities bound to the retained lab TLS clients.

No device deployment. Existing certificates/keys and profile settings are retained.
The dedicated MMS profile permits a trusted local CSR to preserve the existing
application subject and only the IECUserRoles extension override.
"""
from pathlib import Path
import argparse
import datetime as dt
import hashlib
import json
import re
import shutil
import sys

import provision_ejbca_rbac as p
from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding, rsa

PROFILE = 'CCLI-RBAC-MMS-ACSE'
ENROLLMENT = 'CCLI-RBAC-MMS-EE'
DUAL = p.OUT / 'dual-certificate'
TLS = p.ROOT / 'apps/ccli/config/tls'
SPECS = [('dso-mms', [-1], 'DSO', 'IEC 62351-8-CEI016:2021', 'client_tls.pem', 'client.pem'),
         ('viewer-mms', [0], 'CCI', 'IEC62351-8', 'viewer_tls.pem', 'client.pem')]

def profiles():
    admin = p.Admin()
    admin.clone(PROFILE)
    marker = DUAL / 'profile-v1.json'
    if marker.exists():
        print('Dedicated MMS profile already configured', flush=True)
        return
    admin.edit_profile(PROFILE)
    caid = re.search(r'CA Name: ' + p.CA + r'\s+.*?Id: (-?\d+)', p.cli('ca', 'listcas'), re.S).group(1)
    settings = {
        'availableCAs': caid, 'availableBitLengths': '2048,3072',
        'availableKeyAlgorithms': 'RSA', 'signatureAlgorithm': 'SHA256WithRSA',
        'encodedValidity': '365d', 'keyUsage': 'true,false,false,false,true,false,false,false,false',
        'useKeyUsage': 'true', 'keyUsageCritical': 'true', 'useBasicConstraints': 'true',
        'basicConstraintsCritical': 'true', 'useSubjectKeyIdentifier': 'true',
        'useAuthorityKeyIdentifier': 'true', 'useCertificateStorage': 'true',
        'storeCertificateData': 'true', 'useExtendedKeyUsage': 'false',
        'allowDNOverride': 'true', 'allowKeyUsageOverride': 'false',
    }
    for field, value in settings.items():
        p.cli('ca', 'editcertificateprofile', PROFILE, '--field', field, '--value', value)
        print('Configured MMS profile: ' + field, flush=True)
    old_ee, old_role = p.EE, p.ROLE
    try:
        p.EE, p.ROLE = ENROLLMENT, PROFILE
        admin.enrollment_profile(certificate_profiles=(PROFILE,))
    finally:
        p.EE, p.ROLE = old_ee, old_role
    marker.write_text(json.dumps(settings, indent=2) + '\n')

def verify(name, roles, aor, definition, transport, original):
    cert = x509.load_pem_x509_certificate((DUAL / (name + '.pem')).read_bytes())
    key = serialization.load_pem_private_key((DUAL / (name + '.key')).read_bytes(), password=None)
    ca = x509.load_pem_x509_certificate((p.OUT / 'rbac_ca.pem').read_bytes())
    baseline = x509.load_pem_x509_certificate((TLS / original).read_bytes())
    tls = x509.load_pem_x509_certificate((TLS / transport).read_bytes())
    assert cert.issuer == ca.subject
    application_oid = x509.ObjectIdentifier('2.5.4.106')
    actual_ap = cert.subject.get_attributes_for_oid(application_oid)
    expected_ap = baseline.subject.get_attributes_for_oid(application_oid)
    assert len(actual_ap) == len(expected_ap) == 1 and actual_ap[0].value == expected_ap[0].value
    ca.public_key().verify(cert.signature, cert.tbs_certificate_bytes, padding.PKCS1v15(), cert.signature_hash_algorithm)
    fmt = (serialization.Encoding.DER, serialization.PublicFormat.SubjectPublicKeyInfo)
    assert cert.public_key().public_bytes(*fmt) == key.public_key().public_bytes(*fmt)
    assert cert.public_key().public_bytes(*fmt) != tls.public_key().public_bytes(*fmt)
    usage = cert.extensions.get_extension_for_class(x509.KeyUsage).value
    assert usage.digital_signature and usage.key_agreement and not usage.key_cert_sign and not usage.crl_sign
    assert not cert.extensions.get_extension_for_class(x509.BasicConstraints).value.ca
    extension = cert.extensions.get_extension_for_oid(x509.ObjectIdentifier(p.OID))
    assert not extension.critical and extension.value.value == p.role_der(roles, aor, definition, 1)
    assert all(not isinstance(e.value, x509.ExtendedKeyUsage) for e in cert.extensions)
    now = dt.datetime.now(dt.timezone.utc)
    assert cert.not_valid_before_utc <= now < cert.not_valid_after_utc
    (DUAL / (name + '.der')).write_bytes(cert.public_bytes(serialization.Encoding.DER))
    return dict(name=name, role_ids=roles, aor=aor, role_definition=definition, revision=1,
                identity_sha256=cert.fingerprint(hashes.SHA256()).hex(),
                transport_sha256=tls.fingerprint(hashes.SHA256()).hex(),
                subject=cert.subject.rfc4514_string(), transport_file=transport,
                not_after=cert.not_valid_after_utc.isoformat())

def issue():
    records = []
    for name, roles, aor, definition, transport, original in SPECS:
        keypath, csrpath, certpath = [DUAL / (name + suffix) for suffix in ('.key', '.csr.pem', '.pem')]
        if not keypath.exists():
            key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
            keypath.write_bytes(key.private_bytes(serialization.Encoding.PEM, serialization.PrivateFormat.PKCS8, serialization.NoEncryption()))
        key = serialization.load_pem_private_key(keypath.read_bytes(), password=None)
        if not csrpath.exists():
            subject = x509.load_pem_x509_certificate((TLS / original).read_bytes()).subject
            csr = x509.CertificateSigningRequestBuilder().subject_name(subject).add_extension(
                x509.UnrecognizedExtension(x509.ObjectIdentifier(p.OID), p.role_der(roles, aor, definition, 1)), critical=False)
            csrpath.write_bytes(csr.sign(key, hashes.SHA256()).public_bytes(serialization.Encoding.PEM))
        if not certpath.exists():
            username = 'ccli-rbac-' + name
            try: existing = p.cli('ra', 'findendentity', username)
            except RuntimeError as exc:
                if 'does not exist' not in str(exc): raise
                existing = 'does not exist'
            if 'does not exist' in existing or 'No end entity' in existing:
                p.cli('ra', 'addendentity', '--username', username, '--dn', 'CN=CCLI-RBAC-' + name,
                      '--caname', p.CA, '--type', '1', '--token', 'USERGENERATED', '--certprofile', PROFILE,
                      '--eeprofile', ENROLLMENT, '--password', p.secret_data()['enrollment_password'])
            remote = '/tmp/ccli-rbac-' + name
            p.run(['docker', 'cp', str(csrpath), p.CONTAINER + ':' + remote + '.csr'])
            p.cli('createcert', '--username', username, '--password', p.secret_data()['enrollment_password'],
                  '-c', remote + '.csr', '-f', remote + '.pem')
            p.run(['docker', 'cp', p.CONTAINER + ':' + remote + '.pem', str(certpath)])
            p.run(['docker', 'exec', '--user', '0', p.CONTAINER, 'rm', '-f', remote + '.csr', remote + '.pem'])
        records.append(verify(name, roles, aor, definition, transport, original))
        print('PASS: ' + name + ' verified, separate key, AP/AE subject, role extension and issuer', flush=True)
    (DUAL / 'public-manifest.json').write_text(json.dumps(records, indent=2) + '\n')

def package():
    from commission_rbac import FILES
    target = DUAL / 'commissioning-sequence3'
    target.mkdir(exist_ok=True)
    for name in FILES: shutil.copyfile(p.OUT / 'commissioning-candidate' / name, target / name)
    for name, roles, aor, definition, transport, original in SPECS:
        p.run([sys.executable, str(p.ROOT / 'scripts/commission_rbac.py'), 'bind', '--package', str(target),
               '--identity', str(DUAL / (name + '.pem')), '--transport', str(TLS / transport),
               '--actor', 'DSO' if name == 'dso-mms' else 'OWNER', '--roles=' + ','.join(map(str, roles))])
    p.run([sys.executable, str(p.ROOT / 'scripts/commission_rbac.py'), 'sign', '--package', str(target),
           '--sequence', '3', '--owner-key', str(p.OUT / 'owner-signer/owner-signer.key')])
    print('Prepared owner-signed sequence-3 package; existing 17 bindings retained, 2 separate-key pairs added', flush=True)

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('stage', choices=('profiles', 'issue', 'package', 'all'), nargs='?')
    parser.add_argument('--plan', action='store_true')
    args = parser.parse_args()
    if args.plan:
        print('Retain original TLS cert/key; issue DSO/Viewer MMS certificates with independent keys.')
        print('Dedicated MMS-only profile/EE; preserve existing AP/AE subject encoding; 365-day leaves.')
        print('Owner-signed sequence 3 retains original 17 bindings and adds two pairs. No device deployment.')
        return
    if not args.stage: parser.error('choose a stage or --plan')
    p.protect_output()
    DUAL.mkdir(exist_ok=True)
    baseline = {str(f): hashlib.sha256(f.read_bytes()).hexdigest() for f in TLS.iterdir() if f.is_file()}
    for stage in ('profiles', 'issue', 'package') if args.stage == 'all' else (args.stage,):
        globals()[stage]()
    assert all(hashlib.sha256(Path(path).read_bytes()).hexdigest() == digest for path, digest in baseline.items())
    print('PASS: all existing local TLS files unchanged', flush=True)

if __name__ == '__main__': main()

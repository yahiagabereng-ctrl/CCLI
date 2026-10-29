#!/usr/bin/env python3
"""Provision an isolated, administrator-controlled EJBCA Profile A lab PKI.

Never copies material to Tespro or changes working TLS files. Secrets and keys
are kept in ignored lab/ejbca-staging/rbac; protect that folder with OS ACLs.
The CA private key remains in EJBCA. Run stages in order; issuance resumes from
saved CSRs/certificates rather than deleting or replacing existing identities.
"""
from __future__ import annotations

import argparse
import csv
from concurrent.futures import ThreadPoolExecutor
import datetime as dt
import hashlib
import http.cookiejar
import json
import os
from pathlib import Path
import re
import secrets
import shutil
import ssl
import subprocess
import sys
import urllib.parse
import urllib.request

from lxml import html, etree
from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding, rsa
from cryptography.hazmat.primitives.serialization import pkcs12
from cryptography.x509.oid import NameOID, ObjectIdentifier

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'lab/ejbca-staging/rbac'
CA = 'CCLI-RBAC-Lab-CA'
ROLE = 'CCLI-RBAC-ProfileA-TLS'
OWNER = 'CCLI-RBAC-Owner-Signer'
EE = 'CCLI-RBAC-Lab-EE'
OID = '1.0.62351.8.1'
BASE = 'https://localhost:9443/ejbca/adminweb/'
CONTAINER = 'ccli-ejbca-ce'
CLI = '/opt/keyfactor/bin/ejbca.sh'
CRL_VALIDITY = dt.timedelta(days=30)  # User-selected development policy, not production policy.
CRL_VALIDITY_MS = int(CRL_VALIDITY.total_seconds() * 1000)


def configure_crl_validity():
    """Apply the lab lifetime to new or already provisioned CAs (milliseconds)."""
    cli('ca', 'editca', '--caname', CA, '--field', 'CRLPeriod',
        '--value', str(CRL_VALIDITY_MS))
    print(f'Configured RBAC CA CRL validity: {CRL_VALIDITY.days} days (development); no device update performed', flush=True)


def verify_crl_lifetime(crl):
    # EJBCA can backdate thisUpdate by one second relative to nextUpdate.
    if crl.next_update_utc is None:
        raise RuntimeError('Generated CRL has no nextUpdate')
    lifetime = crl.next_update_utc - crl.last_update_utc
    if abs(lifetime - CRL_VALIDITY) > dt.timedelta(seconds=5):
        raise RuntimeError(f'Expected a {CRL_VALIDITY.days}-day development CRL; EJBCA issued lifetime {lifetime}')


def run(args):
    cp = subprocess.run(args, capture_output=True, text=True, encoding='utf-8', errors='replace')
    if cp.returncode:
        # CLI arguments can contain enrollment/token passwords. Never print them.
        message = cp.stdout + cp.stderr
        for value in secret_data().values():
            message = message.replace(value, '<redacted>')
        raise RuntimeError(message[-5000:])
    return cp.stdout + cp.stderr


def cli(*args):
    return run(['docker', 'exec', CONTAINER, CLI, *args])


def secret_data():
    path = OUT / 'secrets.json'
    if not path.exists():
        values = {k: secrets.token_urlsafe(32) for k in ('token_password', 'enrollment_password', 'pkcs12_password')}
        path.write_text(json.dumps(values, indent=2) + '\n')
    return json.loads(path.read_text())


def export_file(command, remote, local):
    cli(*command, '-f', remote)
    run(['docker', 'cp', f'{CONTAINER}:{remote}', str(local)])
    run(['docker', 'exec', CONTAINER, 'rm', '-f', remote])


def initialize():
    if f'CA Name: {CA}\n' not in cli('ca', 'listcas'):
        cli('ca', 'init', '--caname', CA, '--dn', 'CN=CCLI RBAC Lab CA,O=HiTEKS,C=IT',
            '--tokenName', 'CCLI-RBAC-Lab-Token', '--tokenType', 'soft',
            '--tokenPass', secret_data()['token_password'], '--keyspec', '3072',
            '-v', '1825', '--policy', 'null', '-s', 'SHA256WithRSA')
        print('Created dedicated RBAC lab CA', flush=True)
    configure_crl_validity()
    export_file(['ca', 'getcacert', '--caname', CA], '/tmp/ccli-rbac-ca.pem', OUT / 'rbac_ca.pem')
    print('CA public certificate exported; CA private key remains in EJBCA', flush=True)


class Admin:
    def __init__(self):
        # Local quickstart has its own self-signed HTTPS certificate. This
        # exception is restricted to this fixed localhost administration URL.
        self.opener = urllib.request.build_opener(
            urllib.request.HTTPCookieProcessor(http.cookiejar.CookieJar()),
            urllib.request.HTTPSHandler(context=ssl._create_unverified_context()))

    def request(self, route, fields=None, partial=False):
        data = None if fields is None else urllib.parse.urlencode(fields, doseq=True).encode()
        request = urllib.request.Request(BASE + route, data=data,
            headers={'Faces-Request': 'partial/ajax'} if partial else {})
        with self.opener.open(request, timeout=90) as response:
            if not response.url.startswith(BASE):
                raise RuntimeError('Unexpected administration redirect')
            return etree.fromstring(response.read()) if partial else html.fromstring(response.read())

    @staticmethod
    def fields(form):
        fields = []
        for node in form.xpath('.//input[@name] | .//select[@name] | .//textarea[@name]'):
            name = node.get('name')
            kind = node.get('type', '')
            if kind in ('submit', 'button', 'image') or node.get('disabled') is not None:
                continue
            if kind in ('checkbox', 'radio') and node.get('checked') is None:
                continue
            if node.tag == 'select':
                options = node.xpath('./option[@selected]')
                if not options and node.get('multiple') is None:
                    options = node.xpath('./option')[:1]
                fields.extend((name, o.get('value', '')) for o in options)
            else:
                default = 'on' if kind in ('checkbox', 'radio') else ''
                fields.append((name, node.get('value', default) if node.tag == 'input' else node.text or ''))
        return fields

    def clone(self, name):
        if name in cli('ra', 'addendentity', '--help'):
            return
        route = 'ca/editcertificateprofiles/editcertificateprofiles.xhtml'
        doc = self.request(route)
        rows = doc.xpath('//tr[.//td[normalize-space(.)="ENDUSER"]]')
        if len(rows) != 1:
            raise RuntimeError('Cannot uniquely locate ENDUSER template')
        buttons = rows[0].xpath('.//input[@value="Clone"] | .//button[contains(.,"Clone")]')
        if len(buttons) != 1:
            raise RuntimeError('Cannot uniquely locate clone control')
        form = rows[0].xpath('ancestor::form')[0]
        fields = self.fields(form) + [(buttons[0].get('name'), buttons[0].get('value', 'Clone'))]
        doc = self.request(route, fields)
        form = doc.xpath('//form[.//input[contains(@name,"addFromTemplateProfileNew")]]')[0]
        fields = self.fields(form)
        target = form.xpath('.//input[contains(@name,"addFromTemplateProfileNew")]')[0].get('name')
        fields = [(k, name if k == target else v) for k, v in fields]
        button = form.xpath('.//input[contains(@name,"cloneConfirmButton")] | .//button[contains(@name,"cloneConfirmButton")]')[0]
        fields.append((button.get('name'), button.get('value', 'Create from template')))
        self.request('ca/editcertificateprofiles/clonecertificateprofile.xhtml', fields)
        if name not in cli('ra', 'addendentity', '--help'):
            raise RuntimeError(f'Profile clone failed: {name}')
        print(f'Created certificate profile {name}', flush=True)

    def enrollment_profile(self, certificate_profiles=None):
        if EE in cli('ra', 'addendentity', '--help'):
            return
        route = 'ra/editendentityprofiles/editendentityprofiles.xhtml'
        doc = self.request(route)
        form = doc.xpath('//form[@id="manageEndEntityProfiles"]')[0]
        fields = self.fields(form) + [('manageEndEntityProfiles:addButton', 'Add')]
        doc = self.request(route, fields)
        form = doc.xpath('//form[@id="eeProfiles"]')[0]
        fields = self.fields(form)
        def option(suffix, text):
            found = form.xpath('.//select[contains(@name, $suffix)]/option[normalize-space(.)=$text]', suffix=suffix, text=text)
            if len(found) != 1:
                raise RuntimeError(f'Missing enrollment option {suffix}: {text}')
            return found[0].get('value')
        changes = {
            'endEntityProfileName': EE,
            'textFieldPasswordUseCheckBox_input': 'on',
            'textFieldPasswordModifiableCheckBox_input': 'on',
            'subjectDnComponents:0:dnModifiableCheckBox_input': 'on',
            'defaultTokenMenu': '1', 'availableToken': ['1'],
            'defaultCertificateProfile': option('defaultCertificateProfile', ROLE),
            'availableCertProfiles': [option('defaultCertificateProfile', name)
                                     for name in (certificate_profiles or (ROLE, OWNER))],
            'defaultCAMenu': option('defaultCAMenu', CA),
            'availableCA': [option('defaultCAMenu', CA)],
        }
        remove = {'eeProfiles:' + k for k in changes}
        fields = [(k, v) for k, v in fields if k not in remove]
        for k, v in changes.items():
            fields.extend(('eeProfiles:' + k, value) for value in (v if isinstance(v, list) else [v]))
        fields.append(('eeProfiles:saveButton', 'Save'))
        self.request('ra/editendentityprofiles/endentityprofilepage.xhtml', fields)
        if EE not in cli('ra', 'addendentity', '--help'):
            raise RuntimeError('Enrollment profile creation failed')
        print(f'Created restricted enrollment profile {EE}', flush=True)

    def edit_profile(self, name, inspect=False):
        route = 'ca/editcertificateprofiles/editcertificateprofiles.xhtml'
        doc = self.request(route)
        row = doc.xpath('//tr[.//td[normalize-space(.)=$name]]', name=name)[0]
        button = row.xpath('.//input[@value="Edit"] | .//button[contains(.,"Edit")]')[0]
        script = '\n'.join(doc.xpath('//script/text()'))
        match = re.search(re.escape(button.get('name')) + r"':'[^']*','id':'(-?\d+)'", script)
        if not match:
            raise RuntimeError('Missing profile edit ID')
        edit_route = 'ca/editcertificateprofiles/editcertificateprofile.xhtml?id=' + match.group(1)
        doc = self.request(edit_route)
        assert doc.xpath('//input[@name="cpf:profileName"]/@value') == [name]
        if inspect:
            (OUT / 'role-profile-edit.html').write_bytes(html.tostring(doc))
            print('\n'.join(f'{n.tag} {n.get("name")} {n.get("value", "")}' for n in doc.xpath('//input[@name] | //textarea[@name]') if any(s in n.get('name', '').lower() for s in ('overrid', 'save', 'extension'))))
        form = doc.xpath('//form[@id="content"]')[0]
        original_fields = [(k, v) for k, v in self.fields(form) if k not in (
            'content:checkallowextensionoverride_input', 'content:textfieldcustomoverridableextension',
            'content:checkcustomnonoverridableextension_input')]
        fields = original_fields + [('content:checkallowextensionoverride_input', 'on'),
                ('jakarta.faces.partial.ajax', 'true'), ('jakarta.faces.source', 'content:checkallowextensionoverride'),
                ('jakarta.faces.partial.execute', 'content'), ('jakarta.faces.partial.render', 'content:allowextensionoverridegrid'),
                ('jakarta.faces.behavior.event', 'valueChange'), ('jakarta.faces.partial.event', 'change')]
        partial = self.request(edit_route, fields, partial=True)
        if inspect:
            (OUT / 'role-profile-partial.xml').write_bytes(etree.tostring(partial))
            for update in partial.xpath('//update[@id="content:allowextensionoverridegrid"]'):
                fragment = html.fromstring(update.text)
                print(html.tostring(fragment, encoding='unicode'))
            return
        if not partial.xpath('//update[@id="content:allowextensionoverridegrid"]'):
            raise RuntimeError('Extension settings AJAX refresh failed')
        states = partial.xpath('//update[contains(@id,"ViewState")]/text()')
        if len(states) != 1:
            raise RuntimeError('Missing refreshed form state')
        fields = [(k, states[0] if k == 'jakarta.faces.ViewState' else v) for k, v in original_fields]
        fields += [('content:checkallowextensionoverride_input', 'on'),
                   ('content:textfieldcustomoverridableextension', OID), ('content:saveProfileButton', 'Save')]
        response = self.request(edit_route, fields)
        (OUT / 'role-profile-save.html').write_bytes(html.tostring(response))
        assert "returned value '[1.0.62351.8.1]'" in cli('ca', 'editcertificateprofile', name, '-getValue', 'overridableExtensionOIDs')
        assert "returned value '[]'" in cli('ca', 'editcertificateprofile', name, '-getValue', 'nonOverridableExtensionOIDs')
        assert "returned value 'true'" in cli('ca', 'editcertificateprofile', name, '-getValue', 'allowExtensionOverride')
        print('Saved and verified IECUserRoles-only extension override', flush=True)
        (OUT / (name + '.configured')).write_text('verified\n')


def configure():
    admin = Admin()
    caid = re.search(r'CA Name: ' + CA + r'\s+.*?Id: (-?\d+)', cli('ca', 'listcas'), re.S).group(1)
    for name in (ROLE, OWNER):
        if (OUT / (name + '.v3-configured')).exists():
            continue
        if (OUT / (name + '.v2-configured')).exists():
            for field in ('useCertificateStorage', 'storeCertificateData'):
                cli('ca', 'editcertificateprofile', name, '--field', field, '--value', 'true')
                print(f'Configured {name}.{field}', flush=True)
            (OUT / (name + '.v3-configured')).write_text('verified\n')
            continue
        admin.clone(name)
        if name == ROLE and not (OUT / (name + '.configured')).exists():
            admin.edit_profile(name)
        settings = {
            'availableCAs': caid, 'availableBitLengths': '2048,3072',
            'availableKeyAlgorithms': 'RSA', 'signatureAlgorithm': 'SHA256WithRSA',
            'encodedValidity': '365d', 'keyUsage': 'true,false,false,false,false,false,false,false,false',
            'useKeyUsage': 'true', 'keyUsageCritical': 'true',
            'useBasicConstraints': 'true', 'basicConstraintsCritical': 'true',
            'useSubjectKeyIdentifier': 'true', 'useAuthorityKeyIdentifier': 'true',
            'useCertificateStorage': 'true', 'storeCertificateData': 'true',
            'useExtendedKeyUsage': 'true' if name == ROLE else 'false',
            'allowDNOverride': 'false', 'allowKeyUsageOverride': 'false',
        }
        if name == ROLE:
            settings.update(extendedKeyUsageOids='1.3.6.1.5.5.7.3.2')
        else:
            settings.update(allowExtensionOverride='false')
        for field, value in settings.items():
            cli('ca', 'editcertificateprofile', name, '--field', field, '--value', value)
            print(f'Configured {name}.{field}', flush=True)
        (OUT / (name + '.v2-configured')).write_text('verified\n')
        (OUT / (name + '.v3-configured')).write_text('verified\n')
    admin.enrollment_profile()


def tlv(tag, value):
    size = len(value)
    length = bytes([size]) if size < 128 else bytes([0x81, size])
    return bytes([tag]) + length + value


def role_der(ids, aor, definition, revision):
    role_ids = tlv(0x30, b''.join(tlv(2, rid.to_bytes(1, 'big', signed=True)) for rid in ids))
    record = role_ids + tlv(12, aor.encode()) + tlv(2, bytes([revision])) + tlv(12, definition.encode())
    return tlv(0x30, tlv(0x30, record))


def specimens():
    for revision in (0, 1):
        for rid, name in enumerate(('viewer', 'operator', 'engineer', 'installer', 'secadm', 'secaud', 'rbacmnt')):
            yield f'{name}-r{revision}', [rid], 'CCI', 'IEC62351-8', revision
    yield 'dso', [-1], 'DSO', 'IEC 62351-8-CEI016:2021', 1
    yield 'aggregator', [-2], 'AGGREGATOR', 'IEC 62351-8-CEI016:2021', 1
    yield 'multi-viewer-engineer', [0, 2], 'CCI', 'IEC62351-8', 1
    yield 'negative-unknown-role', [99], 'CCI', 'IEC62351-8', 1
    yield 'negative-wrong-aor', [2], 'OTHER', 'IEC62351-8', 1
    yield 'negative-wrong-definition', [2], 'CCI', 'WRONG-DEFINITION', 1
    yield 'negative-wrong-revision', [2], 'CCI', 'IEC62351-8', 2
    yield 'negative-revoked', [2], 'CCI', 'IEC62351-8', 1
    yield 'negative-no-roles', None, None, None, None
    yield 'owner-signer', None, None, None, None


def issue():
    baseline = OUT / 'working-tls-sha256.json'
    if not baseline.exists():
        baseline.write_text(json.dumps({str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest()
            for p in (ROOT / 'apps/ccli/config/tls').iterdir() if p.is_file()}, indent=2) + '\n')
    def issue_one(spec):
        name, ids, aor, definition, revision = spec
        folder = OUT / name
        folder.mkdir(exist_ok=True)
        keypath, csrpath, certpath = (folder / f'{name}.{ext}' for ext in ('key', 'csr.pem', 'pem'))
        if not keypath.exists():
            key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
            keypath.write_bytes(key.private_bytes(serialization.Encoding.PEM, serialization.PrivateFormat.PKCS8, serialization.NoEncryption()))
        key = serialization.load_pem_private_key(keypath.read_bytes(), password=None)
        if not csrpath.exists():
            csr = x509.CertificateSigningRequestBuilder().subject_name(x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, 'CCLI-RBAC-' + name)]))
            if ids is not None:
                csr = csr.add_extension(x509.UnrecognizedExtension(ObjectIdentifier(OID), role_der(ids, aor, definition, revision)), critical=False)
            csrpath.write_bytes(csr.sign(key, hashes.SHA256()).public_bytes(serialization.Encoding.PEM))
        if not certpath.exists():
            retired_path = OUT / 'retired-initial.json'
            retired = json.loads(retired_path.read_text()) if retired_path.exists() else []
            username = 'ccli-rbac-' + name + ('-v2' if name in retired else '')
            # Only register if absent; never delete an existing enrollment.
            try:
                cp = cli('ra', 'findendentity', username)
            except RuntimeError as exc:
                if f"End entity '{username}' does not exist." not in str(exc):
                    raise
                cp = 'does not exist'
            if 'No end entity' in cp or 'does not exist' in cp or 'Could not find' in cp:
                cli('ra', 'addendentity', '--username', username, '--dn', 'CN=CCLI-RBAC-' + name,
                    '--caname', CA, '--type', '1', '--token', 'USERGENERATED',
                    '--certprofile', OWNER if name == 'owner-signer' else ROLE, '--eeprofile', EE,
                    '--password', secret_data()['enrollment_password'])
            remote = '/tmp/ccli-rbac-' + name
            run(['docker', 'cp', str(csrpath), CONTAINER + ':' + remote + '.csr'])
            cli('createcert', '--username', username, '--password', secret_data()['enrollment_password'],
                '-c', remote + '.csr', '-f', remote + '.pem')
            run(['docker', 'cp', CONTAINER + ':' + remote + '.pem', str(certpath)])
        # docker cp creates a root-owned public CSR in sticky /tmp. Remove only
        # this identity's two exact staging paths with container root privileges.
        remote = '/tmp/ccli-rbac-' + name
        run(['docker', 'exec', '--user', '0', CONTAINER, 'rm', '-f', remote + '.csr', remote + '.pem'])
        verify_leaf(name, ids, aor, definition, revision)
        cert = x509.load_pem_x509_certificate(certpath.read_bytes())
        p12path = folder / (name + '.p12')
        if not p12path.exists():
            p12path.write_bytes(pkcs12.serialize_key_and_certificates(name.encode(), key, cert,
                [x509.load_pem_x509_certificate((OUT / 'rbac_ca.pem').read_bytes())],
                serialization.BestAvailableEncryption(secret_data()['pkcs12_password'].encode())))
        print(f'Issued and verified {name}', flush=True)
    # Independent identities have separate keys, end entities and remote files.
    # Keep concurrency low to limit Java CLI memory use in the local container.
    with ThreadPoolExecutor(max_workers=2) as pool:
        list(pool.map(issue_one, specimens()))


def verify_leaf(name, ids, aor, definition, revision):
    cert = x509.load_pem_x509_certificate((OUT / name / (name + '.pem')).read_bytes())
    ca = x509.load_pem_x509_certificate((OUT / 'rbac_ca.pem').read_bytes())
    assert cert.issuer == ca.subject
    ca.public_key().verify(cert.signature, cert.tbs_certificate_bytes, padding.PKCS1v15(), cert.signature_hash_algorithm)
    key = serialization.load_pem_private_key((OUT / name / (name + '.key')).read_bytes(), password=None)
    fmt = (serialization.Encoding.DER, serialization.PublicFormat.SubjectPublicKeyInfo)
    assert key.public_key().public_bytes(*fmt) == cert.public_key().public_bytes(*fmt)
    assert not cert.extensions.get_extension_for_class(x509.BasicConstraints).value.ca
    usage = cert.extensions.get_extension_for_class(x509.KeyUsage).value
    assert usage.digital_signature and not usage.key_cert_sign and not usage.crl_sign
    assert not usage.content_commitment and not usage.key_encipherment and not usage.data_encipherment and not usage.key_agreement
    now = dt.datetime.now(dt.timezone.utc)
    assert cert.not_valid_before_utc <= now < cert.not_valid_after_utc
    if ids is not None:
        extension = cert.extensions.get_extension_for_oid(ObjectIdentifier(OID))
        assert not extension.critical and extension.value.value == role_der(ids, aor, definition, revision)
    else:
        assert all(e.oid.dotted_string != OID for e in cert.extensions)
    if name != 'owner-signer':
        eku = cert.extensions.get_extension_for_class(x509.ExtendedKeyUsage).value
        assert [o.dotted_string for o in eku] == ['1.3.6.1.5.5.7.3.2']
    else:
        assert all(not isinstance(e.value, x509.ExtendedKeyUsage) for e in cert.extensions)
    assert len(cert.public_bytes(serialization.Encoding.DER)) <= 8192


def finish():
    revoked = x509.load_pem_x509_certificate((OUT / 'negative-revoked/negative-revoked.pem').read_bytes())
    ca = x509.load_pem_x509_certificate((OUT / 'rbac_ca.pem').read_bytes())
    previous = OUT / 'rbac_crl.pem'
    already_revoked = False
    if previous.exists():
        old_crl = x509.load_pem_x509_crl(previous.read_bytes())
        already_revoked = old_crl.is_signature_valid(ca.public_key()) and old_crl.get_revoked_certificate_by_serial_number(revoked.serial_number) is not None
    if not already_revoked:
        cli('ra', 'revokeendentity', '--username', 'ccli-rbac-negative-revoked', '-r', '1')
    # Also update existing lab CAs when callers refresh without rerunning init.
    configure_crl_validity()
    cli('ca', 'createcrl', '--caname', CA)
    derpath = OUT / 'rbac_crl.der'
    export_file(['ca', 'getcrl', '--caname', CA], '/tmp/ccli-rbac.crl', derpath)
    crl = x509.load_der_x509_crl(derpath.read_bytes())
    ca = x509.load_pem_x509_certificate((OUT / 'rbac_ca.pem').read_bytes())
    ca.public_key().verify(ca.signature, ca.tbs_certificate_bytes, padding.PKCS1v15(), ca.signature_hash_algorithm)
    assert ca.extensions.get_extension_for_class(x509.BasicConstraints).value.ca
    ca_usage = ca.extensions.get_extension_for_class(x509.KeyUsage).value
    assert ca_usage.key_cert_sign and ca_usage.crl_sign
    assert crl.is_signature_valid(ca.public_key())
    verify_crl_lifetime(crl)
    (OUT / 'rbac_crl.pem').write_bytes(crl.public_bytes(serialization.Encoding.PEM))
    revoked = x509.load_pem_x509_certificate((OUT / 'negative-revoked/negative-revoked.pem').read_bytes())
    assert crl.get_revoked_certificate_by_serial_number(revoked.serial_number) is not None
    result = {'ca': CA, 'ca_sha256': ca.fingerprint(hashes.SHA256()).hex(),
              'crl_this_update_utc': crl.last_update_utc.isoformat(),
              'crl_next_update_utc': crl.next_update_utc.isoformat(),
              'created_utc': dt.datetime.now(dt.timezone.utc).isoformat(), 'identities': []}
    for name, ids, aor, definition, revision in specimens():
        verify_leaf(name, ids, aor, definition, revision)
        cert = x509.load_pem_x509_certificate((OUT / name / (name + '.pem')).read_bytes())
        (OUT / name / (name + '.der')).write_bytes(cert.public_bytes(serialization.Encoding.DER))
        p12key, p12cert, p12chain = pkcs12.load_key_and_certificates(
            (OUT / name / (name + '.p12')).read_bytes(), secret_data()['pkcs12_password'].encode())
        assert p12key is not None and p12cert == cert and p12chain == [ca]
        fmt = (serialization.Encoding.DER, serialization.PublicFormat.SubjectPublicKeyInfo)
        assert p12key.public_key().public_bytes(*fmt) == cert.public_key().public_bytes(*fmt)
        assert (crl.get_revoked_certificate_by_serial_number(cert.serial_number) is not None) == (name == 'negative-revoked')
        result['identities'].append(dict(name=name, roles=ids, aor=aor, definition=definition, revision=revision,
            sha256=cert.fingerprint(hashes.SHA256()).hex(), serial=hex(cert.serial_number),
            not_after=cert.not_valid_after_utc.isoformat(), revoked=name == 'negative-revoked'))
    (OUT / 'public-manifest.json').write_text(json.dumps(result, indent=2) + '\n')
    for path, digest in json.loads((OUT / 'working-tls-sha256.json').read_text()).items():
        assert hashlib.sha256((ROOT / path).read_bytes()).hexdigest() == digest, path
    (OUT / 'README.txt').write_text(
        'CCLI EJBCA RBAC LAB TEST MATERIAL\n'
        'rbac_ca.pem: public identity issuer. rbac_crl.pem: signed test CRL.\n'
        'public-manifest.json: fingerprints, serials and expected roles.\n'
        'Each identity folder: PEM/DER certificate, protected local PEM key, CSR and encrypted PKCS#12.\n'
        'PKCS#12 import password: pkcs12_password in local secrets.json; do not publish it.\n'
        'negative-* are deliberate negative fixtures; owner-signer is not an MMS user.\n'
        'retired-initial contains superseded/revoked certificates; never use them.\n'
        'Working TLS certificates were not replaced. New clients require explicit test trust/pins.\n'
        'Do not deploy verification-only. No device activation has been performed.\n')
    print(f'PASS: {len(result["identities"])} certificates verified; CRL signature and revocation verified', flush=True)
    print(f'CRL nextUpdate: {crl.next_update_utc.isoformat()} (7-day validity); '
          'deploy separately to Tespro', flush=True)


def package():
    """Prepare a local lab candidate; no device trust or activation changes."""
    from commission_rbac import FILES
    folder = OUT / 'commissioning-candidate'
    folder.mkdir(exist_ok=True)
    for filename in FILES:
        if not (folder / filename).exists():
            shutil.copyfile(ROOT / 'apps/ccli/config/rbac' / filename, folder / filename)
    for name, ids, aor, definition, revision in specimens():
        if name.startswith('negative-') or name == 'owner-signer':
            continue
        certpath = OUT / name / (name + '.pem')
        actor = 'DSO' if name == 'dso' else 'AGGREGATOR' if name == 'aggregator' else 'OWNER'
        run([sys.executable, str(ROOT / 'scripts/commission_rbac.py'), 'bind',
            '--package', str(folder), '--identity', str(certpath), '--transport', str(certpath),
            '--actor', actor, '--roles=' + ','.join(map(str, ids))])
    run([sys.executable, str(ROOT / 'scripts/commission_rbac.py'), 'sign', '--package', str(folder),
         '--sequence', '1', '--owner-key', str(OUT / 'owner-signer/owner-signer.key')])
    manifest = etree.parse(str(folder / 'commissioning-manifest.xml')).getroot()
    entries = []
    for entry in manifest:
        digest = hashlib.sha256((folder / entry.get('Name')).read_bytes()).hexdigest()
        assert digest == entry.get('Sha256')
        entries.append((entry.get('Name'), digest))
    payload = ('CCLI-RBAC-PACKAGE-1\n1\n' + ''.join(f'{name} {digest}\n' for name, digest in entries)).encode('ascii')
    owner = x509.load_pem_x509_certificate((OUT / 'owner-signer/owner-signer.pem').read_bytes())
    owner.public_key().verify(bytes.fromhex(manifest.get('Signature')), payload, padding.PKCS1v15(), hashes.SHA256())
    bindings = etree.parse(str(folder / 'identity-bindings.xml')).getroot()
    assert len(bindings) == 17
    (folder / 'README.txt').write_text(
        'LAB CANDIDATE ONLY. Sequence 1; 17 positive test identities.\n'
        'Uses the repository scopes, including functions absent in the running model.\n'
        'No device trust, TLS pins or activation state have been changed.\n'
        'Review model scope and client route before controlled device activation.\n'
        'Keep owner-signer.key on this protected PC; deploy only public signer certificate.\n')
    print('PASS: staged candidate, 17 positive identity bindings, four file hashes and owner signature verified', flush=True)


def retire_initial():
    archive = OUT / 'retired-initial'
    archive.mkdir(exist_ok=True)
    record = OUT / 'retired-initial.json'
    retired = json.loads(record.read_text()) if record.exists() else []
    for name, *_ in specimens():
        path = OUT / name / (name + '.pem')
        if not path.exists() or name in retired:
            continue
        cert = x509.load_pem_x509_certificate(path.read_bytes())
        # Only retire the first batch that lacks the required key usage.
        try:
            cert.extensions.get_extension_for_class(x509.KeyUsage)
            continue
        except x509.ExtensionNotFound:
            pass
        cli('ra', 'revokeendentity', '--username', 'ccli-rbac-' + name, '-r', '4')
        path.rename(archive / (name + '-' + format(cert.serial_number, 'x') + '.pem'))
        retired.append(name)
        record.write_text(json.dumps(retired, indent=2) + '\n')
        print(f'Revoked and archived initial {name}; replacement uses a new enrollment', flush=True)


def verification_package():
    """Bind negatives only in an isolated, never-deployed test package."""
    source = OUT / 'commissioning-candidate'
    target = OUT / 'verification-only'
    target.mkdir(exist_ok=True)
    from commission_rbac import FILES
    for filename in FILES:
        shutil.copyfile(source / filename, target / filename)
    for name, *_ in specimens():
        if not name.startswith('negative-'):
            continue
        certpath = OUT / name / (name + '.pem')
        run([sys.executable, str(ROOT / 'scripts/commission_rbac.py'), 'bind',
             '--package', str(target), '--identity', str(certpath), '--transport', str(certpath),
             '--actor', 'OWNER', '--roles=2'])
    run([sys.executable, str(ROOT / 'scripts/commission_rbac.py'), 'sign', '--package', str(target),
         '--sequence', '1', '--owner-key', str(OUT / 'owner-signer/owner-signer.key')])
    (target / 'README.txt').write_text('VERIFICATION ONLY. Deliberately binds negative fixtures for rejection tests. Never deploy.\n')
    print('Prepared isolated negative-fixture verification package; never deploy this folder', flush=True)


def register_issued():
    """Recover records from the first role batch issued with storage disabled.

    Strict import retains original public certificates, keys and serials. Only
    this tool's dedicated lab EEs are updated; owner metadata already existed.
    """
    record_path = OUT / 'db-registered.json'
    records = json.loads(record_path.read_text()) if record_path.exists() else {}
    retired = json.loads((OUT / 'retired-initial.json').read_text())
    tasks = []
    for name, *_ in specimens():
        if name == 'owner-signer':
            continue
        username = 'ccli-rbac-' + name + ('-v2' if name in retired else '')
        tasks.append((name, username, OUT / name / (name + '.pem'),
                      'REVOKED' if name == 'negative-revoked' else 'ACTIVE', 'KEY_COMPROMISE'))
    for path in (OUT / 'retired-initial').glob('*.pem'):
        name = next(name for name in retired if path.name.startswith(name + '-'))
        tasks.append(('retired-' + name, 'ccli-rbac-' + name, path, 'REVOKED', 'SUPERSEDED'))
    def import_one(task):
        name, username, path, status, reason = task
        cert = x509.load_pem_x509_certificate(path.read_bytes())
        fingerprint = cert.fingerprint(hashes.SHA256()).hex()
        if records.get(name) == fingerprint:
            return name, fingerprint
        remote = '/tmp/ccli-rbac-register-' + name + '.pem'
        run(['docker', 'cp', str(path), CONTAINER + ':' + remote])
        args = ['ca', 'importcert', '--username', username, '--password', secret_data()['enrollment_password'],
                '--caname', CA, '-a', status, '-f', remote, '--certprofile', ROLE, '--eeprofile', EE,
                '--strict', '--overwrite']
        if status == 'REVOKED':
            args += ['--revocation-reason', reason]
        cli(*args)
        run(['docker', 'exec', '--user', '0', CONTAINER, 'rm', '-f', remote])
        print(f'Registered original certificate {name} as {status}', flush=True)
        return name, fingerprint
    with ThreadPoolExecutor(max_workers=2) as pool:
        for name, fingerprint in pool.map(import_one, tasks):
            records[name] = fingerprint
            record_path.write_text(json.dumps(records, indent=2) + '\n')


def protect_output():
    """Protect the output before creating or reading enrollment secrets."""
    OUT.mkdir(parents=True, exist_ok=True)
    if os.name == 'nt':
        identity = subprocess.run(['whoami', '/user', '/fo', 'csv', '/nh'],
            capture_output=True, text=True, check=True)
        sid = next(csv.reader(identity.stdout.strip().splitlines()))[1]
        subprocess.run(['icacls', str(OUT), '/inheritance:r', '/grant:r',
            f'*{sid}:(OI)(CI)F', '*S-1-5-18:(OI)(CI)F'],
            capture_output=True, text=True, check=True)
    else:
        OUT.chmod(0o700)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('stage', nargs='?', choices=('all', 'init', 'crl-settings', 'profiles', 'issue', 'finish', 'inspect', 'restrict', 'package', 'retire-initial', 'verification-package', 'register-issued'))
    parser.add_argument('--plan', action='store_true', help='Show steps and identities without contacting EJBCA or writing files')
    args = parser.parse_args()
    if args.plan:
        print(f'EJBCA: {BASE}\nContainer: {CONTAINER}\nCA: {CA}\nOutput: {OUT}')
        print('Normal sequence: init -> profiles -> issue -> finish -> package')
        print('CRL validity: 7 days (604800000 ms); applied by init/finish, including existing CA.')
        print('crl-settings applies only the CA lifetime; finish issues and verifies a replacement CRL.')
        print('Automatic generation/delivery and device reload are not configured by this tool.')
        print('all runs this sequence; recovery/negative-package stages are explicit only.')
        for name, ids, aor, definition, revision in specimens():
            print(f'  {name}: roles={ids}, AoR={aor}, definition={definition}, revision={revision}')
        print('No Tespro deployment or Profile A activation is performed.')
        return
    if args.stage is None:
        parser.error('choose a stage, all, or --plan (see scripts/README_EJBCA_RBAC.md)')
    protect_output()
    if args.stage == 'all':
        for name, action in [('init', initialize), ('profiles', configure), ('issue', issue),
                             ('finish', finish), ('package', package)]:
            print(f'=== {name} ===', flush=True)
            action()
        return
    if args.stage == 'inspect':
        admin = Admin()
        doc = admin.request('ca/editcertificateprofiles/editcertificateprofiles.xhtml')
        (OUT / 'certificate-profile-page.html').write_bytes(html.tostring(doc))
        print('\n'.join(' '.join(r.itertext()).strip() for r in doc.xpath('//tr') if 'ENDUSER' in ' '.join(r.itertext())))
        admin.edit_profile(ROLE, inspect=True)
    else:
        {'init': initialize, 'crl-settings': configure_crl_validity,
         'profiles': configure, 'issue': issue, 'finish': finish,
         'restrict': lambda: Admin().edit_profile(ROLE), 'package': package, 'retire-initial': retire_initial,
         'verification-package': verification_package, 'register-issued': register_issued}[args.stage]()


if __name__ == '__main__':
    main()

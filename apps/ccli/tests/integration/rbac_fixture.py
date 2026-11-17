#!/usr/bin/env python3
"""Generate isolated RBAC test certificates/packages. Never touches product TLS files."""
from __future__ import annotations
import argparse
import datetime as dt
import hashlib
import ipaddress
from pathlib import Path
import shutil
import subprocess
import sys
import uuid
import xml.etree.ElementTree as ET
from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding, rsa
from cryptography.x509.oid import ExtendedKeyUsageOID, NameOID, ObjectIdentifier

NOW = dt.datetime.now(dt.timezone.utc).replace(microsecond=0)

def tlv(tag, value):
    n = len(value)
    length = bytes([n]) if n < 128 else bytes([0x80 | ((n.bit_length() + 7) // 8)]) + n.to_bytes((n.bit_length() + 7) // 8, 'big')
    return bytes([tag]) + length + value

def roles(rid, aor, definition, revision=1, operation=None):
    value = tlv(0x30, tlv(2, rid.to_bytes(1, 'big', signed=True))) + tlv(12, aor.encode()) + tlv(2, bytes([revision])) + tlv(12, definition.encode())
    if operation is not None: value += tlv(10, bytes([operation]))
    return tlv(0x30, tlv(0x30, value))

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--source', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    folder = args.out / ('run-' + uuid.uuid4().hex)
    folder.mkdir(mode=0o700)
    def key(): return rsa.generate_private_key(public_exponent=65537, key_size=2048)
    def save_key(name, private):
        (folder / (name + '.key')).write_bytes(private.private_bytes(serialization.Encoding.PEM, serialization.PrivateFormat.PKCS8, serialization.NoEncryption()))
        (folder / (name + '.key')).chmod(0o600)
    def cert(name, private, issuer=None, signing=None, ca=False, role=None, expired=False, server=False):
        subject = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, name)])
        builder = (x509.CertificateBuilder().subject_name(subject).issuer_name(issuer.subject if issuer else subject)
                   .public_key(private.public_key()).serial_number(x509.random_serial_number())
                   .not_valid_before(NOW - dt.timedelta(days=3))
                   .not_valid_after(NOW - dt.timedelta(days=1) if expired else NOW + dt.timedelta(days=30))
                   .add_extension(x509.BasicConstraints(ca=ca, path_length=None), critical=True)
                   .add_extension(x509.KeyUsage(digital_signature=True, content_commitment=False, key_encipherment=not ca,
                       data_encipherment=False, key_agreement=False, key_cert_sign=ca, crl_sign=ca,
                       encipher_only=False, decipher_only=False), critical=True))
        if role is not None: builder = builder.add_extension(x509.UnrecognizedExtension(ObjectIdentifier('1.0.62351.8.1'), role), critical=False)
        if not ca:
            builder = builder.add_extension(x509.ExtendedKeyUsage([ExtendedKeyUsageOID.SERVER_AUTH if server else ExtendedKeyUsageOID.CLIENT_AUTH]), critical=False)
        if server:
            builder = builder.add_extension(x509.SubjectAlternativeName([x509.IPAddress(ipaddress.ip_address('127.0.0.1')), x509.DNSName('localhost')]), critical=False)
        certificate = builder.sign(signing or private, hashes.SHA256())
        (folder / (name + '.pem')).write_bytes(certificate.public_bytes(serialization.Encoding.PEM))
        (folder / (name + '.der')).write_bytes(certificate.public_bytes(serialization.Encoding.DER))
        save_key(name, private)
        return certificate
    ca_key = key(); ca = cert('ca', ca_key, ca=True)
    rogue_key = key(); rogue_ca = cert('rogue_ca', rogue_key, ca=True)
    owner_key = key(); cert('owner', owner_key)
    wrong_owner_key = key(); cert('wrong_owner', wrong_owner_key)
    transport_key = key(); transport = cert('transport', transport_key, ca, ca_key)
    role = roles(-1, 'DSO', 'IEC 62351-8-CEI016:2021')
    identities = {}
    def identity(name, rid=-1, aor='DSO', definition='IEC 62351-8-CEI016:2021', revision=1,
                 actor='DSO', allowed='-1,0', private=None, issuer=None, signing=None, expired=False, operation=None, same_transport=False):
        private = private or key()
        certificate = cert(name, private, issuer or ca, signing or ca_key,
                           role=roles(rid, aor, definition, revision, operation), expired=expired)
        pin = hashlib.sha256(certificate.public_bytes(serialization.Encoding.DER)).hexdigest()
        transport_pin = pin if same_transport else hashlib.sha256(transport.public_bytes(serialization.Encoding.DER)).hexdigest()
        identities[name] = dict(Sha256=pin, TransportSha256=transport_pin, Actor=actor, Roles=allowed)
        return certificate, private
    dso, _ = identity('dso', private=transport_key)
    identity('dso_admin', rid=4, aor='CCI', definition='IEC62351-8')
    identity('wrong_issuer', issuer=rogue_ca, signing=rogue_key)
    identity('expired', expired=True)
    revoked, _ = identity('revoked')
    unconfigured, _ = identity('unconfigured'); del identities['unconfigured']
    identity('admin_operation', operation=2)
    identity('owner_rev0', rid=2, aor='CCI', definition='IEC62351-8', revision=0, actor='OWNER', allowed='2', same_transport=True)
    proof_cert, proof_key = identity('proof_identity')
    unified_dso, _ = identity('unified_dso', same_transport=True)
    identity('unified_viewer', rid=0, aor='CCI', definition='IEC62351-8', actor='OWNER', allowed='0', same_transport=True)
    identity('unified_aggregator', rid=-2, aor='AGGREGATOR', actor='AGGREGATOR', allowed='-2,0', same_transport=True)
    cert('server', key(), ca, ca_key, server=True)
    def crl(name, serials, stale=False, bad_signature=False):
        builder = (x509.CertificateRevocationListBuilder().issuer_name(ca.subject)
                   .last_update(NOW - dt.timedelta(days=1)).next_update(NOW - dt.timedelta(minutes=1) if stale else NOW + dt.timedelta(days=1)))
        for serial in serials:
            builder = builder.add_revoked_certificate(x509.RevokedCertificateBuilder().serial_number(serial).revocation_date(NOW - dt.timedelta(minutes=1)).build())
        (folder / name).write_bytes(builder.sign(rogue_key if bad_signature else ca_key, hashes.SHA256()).public_bytes(serialization.Encoding.PEM))
    crl('base.crl.pem', [revoked.serial_number])
    crl('stale.crl.pem', [], stale=True)
    crl('bad_signature.crl.pem', [], bad_signature=True)
    crl('revoked_dso.crl.pem', [revoked.serial_number, dso.serial_number])
    crl('revoked_unified.crl.pem', [revoked.serial_number, unified_dso.serial_number])
    shutil.copyfile(folder / 'base.crl.pem', folder / 'active.crl.pem')
    shutil.copyfile(folder / 'base.crl.pem', folder / 'integration.crl.pem')
    model = (args.source / 'config/icd/lab_tg544_eth_a.cfg').read_text()
    model = model.replace('DS(DS_R_Stato_Allarmi_Segnali){',
        'DS(DS_R_Evil){\nDE(LLN0$DC$NamPlt);\n}\n'
        'RC(urcb_Evil CCI016_01LD_Plant/LLN0.urcb_Evil 0 DS_R_Evil 1 24 159 0 1000);\n'
        'DS(DS_R_Stato_Allarmi_Segnali){', 1)
    (folder / 'model.cfg').write_text(model)
    shutil.copyfile(args.source / 'config/icd/lab_tg544_aggregator.cfg', folder / 'aggregator-model.cfg')
    def proof(name, timestamp, corrupt=False):
        stamp = timestamp.strftime('%Y%m%d%H%M%S.').encode()
        signature = proof_key.sign(stamp, padding.PKCS1v15(), hashes.SHA256())
        if corrupt: signature = signature[:-1] + bytes([signature[-1] ^ 1])
        body = tlv(0xa0, tlv(0x80, proof_cert.public_bytes(serialization.Encoding.DER)) + tlv(0x81, stamp) + tlv(0x82, signature))
        (folder / name).write_bytes(tlv(0xa2, body))
        if name == 'signed_auth.bin':
            # Alternate valid BER length encoding must not bypass proof replay detection.
            cert_der = proof_cert.public_bytes(serialization.Encoding.DER)
            cert_field = bytes([0x80, 0x83, 0]) + len(cert_der).to_bytes(2, 'big') + cert_der
            alternate = tlv(0xa0, cert_field + tlv(0x81, stamp) + tlv(0x82, signature))
            (folder / 'rewrapped_auth.bin').write_bytes(tlv(0xa2, alternate))
    proof('signed_auth.bin', NOW)
    proof('bad_auth.bin', NOW, True)
    proof('stale_auth.bin', NOW - dt.timedelta(hours=1))
    tool = args.source.resolve().parents[1] / 'scripts/commission_rbac.py'
    def package(name, sequence, owner='owner', mutation=None):
        destination = folder / name; destination.mkdir()
        for item in ('commissioning-policy.xml', 'permission-library.xml', 'commissioning-scopes.xml'):
            shutil.copyfile(args.source / 'config/rbac' / item, destination / item)
        scope_file = destination / 'commissioning-scopes.xml'
        scope_root = ET.parse(scope_file).getroot()
        for binding in scope_root:
            if mutation == 'aggregator_config_only' and binding.attrib['RoleId'] == '-2':
                for scope in list(binding):
                    if scope.attrib['Permission'] == 'CONTROL': binding.remove(scope)
            if binding.attrib['RoleId'] == '-1':
                for scope in binding:
                    if scope.attrib['Permission'] == 'DATASET': ET.SubElement(scope, 'Object').text = 'LD_Plant/LLN0.DS_R_Evil'
                    if scope.attrib['Permission'] == 'REPORTING': ET.SubElement(scope, 'Object').text = 'LD_Plant/LLN0.urcb_Evil'
        ET.ElementTree(scope_root).write(scope_file, encoding='utf-8', xml_declaration=True)
        bindings = ET.Element('IdentityBindings', Version='1.0')
        for values in identities.values(): ET.SubElement(bindings, 'Identity', values)
        if mutation == 'actor': bindings[0].set('Roles', '4')
        ET.ElementTree(bindings).write(destination / 'identity-bindings.xml', encoding='utf-8', xml_declaration=True)
        if mutation == 'policy':
            file = destination / 'commissioning-policy.xml'
            file.write_text(file.read_text().replace('permit-overrides', 'deny-overrides', 1))
        if mutation == 'changed':
            file = destination / 'commissioning-scopes.xml'; file.write_bytes(file.read_bytes() + b'\n')
        subprocess.run([sys.executable, str(tool), 'sign', '--package', str(destination), '--sequence', str(sequence),
                        '--owner-key', str(folder / (owner + '.key'))], check=True, capture_output=True)
        return destination
    package('package1', 1)
    package('package2', 2)
    package('package_config_only', 3, mutation='aggregator_config_only')
    tampered = package('tampered', 3)
    target = tampered / 'commissioning-scopes.xml'; target.write_bytes(target.read_bytes() + b'\n')
    package('wrong_owner', 3, owner='wrong_owner')
    package('same_sequence_changed', 2, mutation='changed')
    package('bad_actor', 3, mutation='actor')
    package('bad_policy', 3, mutation='policy')
    (args.out / 'fixture-directory.txt').write_text(folder.name)
    print('Generated isolated certificate and signed-package fixtures:', folder)
    return 0

if __name__ == '__main__': raise SystemExit(main())

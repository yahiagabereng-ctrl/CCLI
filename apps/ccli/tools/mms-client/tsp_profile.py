"""Validate the certificate-authenticated AARE before allowing client operations."""
import datetime as dt
import hashlib
from pathlib import Path
import re
import subprocess
import tempfile

from cryptography import x509
from cryptography.exceptions import InvalidSignature
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding


def fields(data):
    result, pos = [], 0
    while pos < len(data):
        if len(data) - pos < 2:
            raise ValueError('Truncated AARE field')
        tag, size = data[pos:pos + 2]
        pos += 2
        if size & 128:
            count = size & 127
            if not 0 < count <= 4 or pos + count > len(data):
                raise ValueError('Invalid AARE length')
            size = int.from_bytes(data[pos:pos + count], 'big')
            pos += count
        if size > len(data) - pos:
            raise ValueError('AARE field exceeds container')
        result.append((tag, data[pos:pos + size]))
        pos += size
    return result


def one(data, tag):
    parsed = fields(data)
    if len(parsed) != 1 or parsed[0][0] != tag:
        raise ValueError('Unexpected AARE authentication structure')
    return parsed[0][1]


def verify_aare(data, ca):
    if not 0 < len(data) <= 16384:
        raise ValueError('Invalid AARE size')
    parsed = fields(data)
    if len({tag for tag, _ in parsed}) != len(parsed):
        raise ValueError('Duplicate AARE field')
    values = dict(parsed)
    if one(values.get(0xa2, b''), 2) != b'\x00':
        raise ValueError('Server rejected association')
    if one(values.get(0xa1, b''), 6) != bytes.fromhex('28ca220203'):
        raise ValueError('Unexpected MMS application context')
    if values.get(0x89) != bytes.fromhex('2886480001000101'):
        raise ValueError('Server did not return certificate authentication')
    if one(values.get(0xa4, b''), 6) != bytes.fromhex('2901876701') or one(values.get(0xa5, b''), 2) != b'\x0c':
        raise ValueError('Unexpected responding AP-title/AE qualifier')
    external = fields(one(values.get(0xaa, b''), 0xa2))
    if len(external) != 2 or external[0] != (2, b'\x05') or external[1][0] != 0xa0:
        raise ValueError('Invalid server authentication context')
    proof = fields(one(external[1][1], 0xa0))
    if [tag for tag, _ in proof] != [0x80, 0x81, 0x82]:
        raise ValueError('Missing certificate/time/signature in AARE')
    cert_bytes, stamp, signature = [value for _, value in proof]
    if not re.fullmatch(rb'\d{14}(?:\.\d*)?Z?', stamp):
        raise ValueError('Invalid server authentication timestamp')
    signed_at = dt.datetime.strptime(stamp[:14].decode(), '%Y%m%d%H%M%S').replace(tzinfo=dt.timezone.utc)
    if abs((dt.datetime.now(dt.timezone.utc) - signed_at).total_seconds()) > 120:
        raise ValueError('Stale server authentication proof')
    cert = x509.load_der_x509_certificate(cert_bytes)
    try:
        cert.public_key().verify(signature, stamp, padding.PKCS1v15(), hashes.SHA256())
    except InvalidSignature as error:
        raise ValueError('MMS server authentication signature is invalid') from error
    with tempfile.TemporaryDirectory(prefix='ccli-peer-auth-') as temporary:
        peer = Path(temporary) / 'peer.pem'
        peer.write_bytes(cert.public_bytes(serialization.Encoding.PEM))
        verification = subprocess.run(['openssl', 'verify', '-purpose', 'any', '-CAfile', str(ca), str(peer)],
                                      capture_output=True, text=True, timeout=3)
        if verification.returncode:
            raise ValueError('MMS server certificate is not valid under the configured MMS CA')
    try:
        subject = cert.subject.rfc4514_string()
    except ValueError:
        subject = 'Certificate with an ASN.1 distinguished-name attribute; see SHA-256'
    return {'sha256': hashlib.sha256(cert_bytes).hexdigest(), 'serial': format(cert.serial_number, 'X'),
            'subject': subject, 'signed_time': stamp.decode(), 'signature_verified': True,
            'chain_verified': True, 'ca': str(ca)}


def approve_record(record, args, stream):
    try:
        peer = verify_aare(bytes.fromhex(record['value']), getattr(args, 'mms_ca', None) or args.ca)
    except Exception:
        stream.write('REJECT\n')
        stream.flush()
        raise
    stream.write('ACCEPT\n')
    stream.flush()
    record['value'] = 'MMS server certificate chain, signed time and signature verified; SHA-256=' + peer['sha256']
    return peer

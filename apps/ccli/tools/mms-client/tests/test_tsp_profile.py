import datetime as dt
import io
from pathlib import Path
import sys
import tempfile
from types import SimpleNamespace
import unittest

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa, padding
from cryptography.x509.oid import NameOID

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tsp_profile import verify_aare, approve_record


def tlv(tag, value):
    size = len(value)
    raw = size.to_bytes(max(1, (size.bit_length() + 7) // 8), 'big')
    return bytes([tag]) + (bytes([size]) if size < 128 else bytes([128 | len(raw)]) + raw) + value


class ServerAuthenticationTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temporary = tempfile.TemporaryDirectory(prefix='ccli-aare-tests-')
        cls.key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
        now = dt.datetime.now(dt.timezone.utc)
        name = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, 'temporary-test-root')])
        root = (x509.CertificateBuilder().subject_name(name).issuer_name(name).public_key(cls.key.public_key())
                .serial_number(1).not_valid_before(now - dt.timedelta(days=1)).not_valid_after(now + dt.timedelta(days=1))
                .add_extension(x509.BasicConstraints(ca=True, path_length=None), True).sign(cls.key, hashes.SHA256()))
        cls.ca = Path(cls.temporary.name) / 'root.pem'
        cls.ca.write_bytes(root.public_bytes(serialization.Encoding.PEM))
        cls.cert = (x509.CertificateBuilder().subject_name(x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, 'test-server')]))
                    .issuer_name(name).public_key(cls.key.public_key()).serial_number(2)
                    .not_valid_before(now - dt.timedelta(days=1)).not_valid_after(now + dt.timedelta(days=1))
                    .add_extension(x509.BasicConstraints(ca=False, path_length=None), True).sign(cls.key, hashes.SHA256()))

    @classmethod
    def tearDownClass(cls):
        cls.temporary.cleanup()

    def response(self, age=0, corrupt=False, result=0):
        stamp = (dt.datetime.now(dt.timezone.utc) + dt.timedelta(seconds=age)).strftime('%Y%m%d%H%M%S.').encode()
        signature = self.key.sign(stamp, padding.PKCS1v15(), hashes.SHA256())
        if corrupt:
            signature = bytes([signature[0] ^ 1]) + signature[1:]
        proof = tlv(0x80, self.cert.public_bytes(serialization.Encoding.DER)) + tlv(0x81, stamp) + tlv(0x82, signature)
        return (tlv(0xa1, tlv(6, bytes.fromhex('28ca220203'))) + tlv(0xa2, tlv(2, bytes([result])))
                + tlv(0xa4, tlv(6, bytes.fromhex('2901876701'))) + tlv(0xa5, tlv(2, b'\x0c'))
                + tlv(0x89, bytes.fromhex('2886480001000101'))
                + tlv(0xaa, tlv(0xa2, tlv(2, b'\x05') + tlv(0xa0, tlv(0xa0, proof)))))

    def test_valid_chain_and_signature(self):
        result = verify_aare(self.response(), self.ca)
        self.assertTrue(result['signature_verified'] and result['chain_verified'])

    def test_corrupt_signature(self):
        with self.assertRaisesRegex(ValueError, 'signature is invalid'): verify_aare(self.response(corrupt=True), self.ca)

    def test_stale_proof(self):
        with self.assertRaisesRegex(ValueError, 'Stale'): verify_aare(self.response(age=-600), self.ca)

    def test_future_proof(self):
        with self.assertRaisesRegex(ValueError, 'Stale'): verify_aare(self.response(age=600), self.ca)

    def test_rejected_association(self):
        with self.assertRaisesRegex(ValueError, 'rejected'): verify_aare(self.response(result=1), self.ca)

    def test_missing_certificate_auth(self):
        with self.assertRaises(ValueError): verify_aare(tlv(0xa2, tlv(2, b'\x00')), self.ca)

    def test_duplicate_field(self):
        with self.assertRaisesRegex(ValueError, 'Duplicate'): verify_aare(self.response() + tlv(0xa2, tlv(2, b'\x00')), self.ca)

    def test_wrong_ca(self):
        with self.assertRaisesRegex(ValueError, 'configured MMS CA'):
            verify_aare(self.response(), Path('/nonexistent/ccli-test-ca.pem'))

    def test_ack_only_after_verification(self):
        output = io.StringIO()
        approve_record({'value': self.response().hex()}, SimpleNamespace(ca=self.ca), output)
        self.assertEqual(output.getvalue(), 'ACCEPT\n')
        output = io.StringIO()
        with self.assertRaises(Exception):
            approve_record({'value': self.response(corrupt=True).hex()}, SimpleNamespace(ca=self.ca), output)
        self.assertEqual(output.getvalue(), 'REJECT\n')


if __name__ == '__main__':
    unittest.main()

"""Exercise the actual OpenWrt CRL fetch script against signed HTTP fixtures."""
import datetime as dt
import functools
import http.server
import os
import pathlib
import subprocess
import tempfile
import threading
import unittest

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.x509.oid import NameOID


class CRLFetchTest(unittest.TestCase):
    def test_signed_updates_and_rejections(self):
        script = pathlib.Path(__file__).resolve().parents[2] / 'package/ccli/files/ccli-crl-fetch'
        key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
        wrong_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
        name = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, 'fixture-ca')])
        now = dt.datetime.now(dt.timezone.utc).replace(microsecond=0)
        cert = (x509.CertificateBuilder().subject_name(name).issuer_name(name)
                .public_key(key.public_key()).serial_number(1)
                .not_valid_before(now-dt.timedelta(days=1))
                .not_valid_after(now+dt.timedelta(days=2))
                .add_extension(x509.BasicConstraints(ca=True, path_length=None), True)
                .sign(key, hashes.SHA256()))
        with tempfile.TemporaryDirectory() as folder:
            root = pathlib.Path(folder)
            (root/'ca.pem').write_bytes(cert.public_bytes(serialization.Encoding.PEM))
            handler = functools.partial(http.server.SimpleHTTPRequestHandler, directory=folder)
            server = http.server.ThreadingHTTPServer(('127.0.0.1', 0), handler)
            thread = threading.Thread(target=server.serve_forever, daemon=True)
            thread.start()
            try:
                def run(number, start=-60, end=3600, signing_key=key):
                    crl = (x509.CertificateRevocationListBuilder().issuer_name(name)
                           .last_update(now+dt.timedelta(seconds=start))
                           .next_update(now+dt.timedelta(seconds=end))
                           .add_extension(x509.CRLNumber(number), False)
                           .sign(signing_key, hashes.SHA256()))
                    (root/'download.crl').write_bytes(crl.public_bytes(serialization.Encoding.DER))
                    return subprocess.run(['sh', str(script),
                        f'http://127.0.0.1:{server.server_port}/download.crl',
                        str(root/'ca.pem'), str(root/'cache')], capture_output=True, text=True)

                result = run(10)
                self.assertEqual(result.returncode, 0, result.stderr)
                baseline = (root/'cache/crl.pem').read_bytes()
                self.assertEqual(run(10).returncode, 0)  # identical response is idempotent
                for description, arguments in [
                    ('rollback', dict(number=9)),
                    ('same number with changed content', dict(number=10, end=3700)),
                    ('bad signature', dict(number=11, signing_key=wrong_key)),
                    ('expired', dict(number=11, start=-7200, end=-60)),
                    ('future', dict(number=11, start=3600, end=7200)),
                    ('issue time rollback', dict(number=11, start=-120)),
                ]:
                    with self.subTest(description=description):
                        self.assertNotEqual(run(**arguments).returncode, 0)
                        self.assertEqual((root/'cache/crl.pem').read_bytes(), baseline)
                result = run(11, start=-30)
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertNotEqual((root/'cache/crl.pem').read_bytes(), baseline)
                # CEI's 24-hour refresh guidance is not a maximum signed lifetime.
                result = run(12, start=-20, end=7*86400)
                self.assertEqual(result.returncode, 0, result.stderr)
                result = run(13, start=-10, end=30*86400)
                self.assertEqual(result.returncode, 0, result.stderr)
                publisher = script.with_name('ccli-crl-refresh')
                config = root/'issuers.conf'
                url = f'http://127.0.0.1:{server.server_port}/download.crl'
                config.write_text(f'root both {url} {root}/ca.pem\n')
                def publish():
                    return subprocess.run(['sh', str(publisher), str(config), str(root/'state')],
                        env=dict(os.environ, CCLI_CRL_FETCH=str(script)), capture_output=True, text=True)
                result = publish()
                self.assertEqual(result.returncode, 0, result.stderr)
                current = (root/'state/current').readlink()
                self.assertEqual(publish().returncode, 0)
                self.assertEqual((root/'state/current').readlink(), current)
                config.write_text(f'root both {url} {root}/ca.pem\nmissing tls http://127.0.0.1:{server.server_port}/absent {root}/ca.pem\n')
                self.assertNotEqual(publish().returncode, 0)
                self.assertEqual((root/'state/current').readlink(), current)
                self.assertEqual((root/'state/current/rbac_crl.pem').read_bytes(),
                                 (root/'state/current/tls-crl-bundle.pem').read_bytes())
            finally:
                server.shutdown()
                server.server_close()
                thread.join()


if __name__ == '__main__':
    unittest.main()

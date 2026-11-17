#!/usr/bin/env python3
"""Host-side security tests for the offline USB authorization gate."""

import hashlib
import json
import os
from pathlib import Path
import shutil
import stat
import subprocess
import tempfile
import time
import unittest
from datetime import datetime, timedelta, timezone

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "apps" / "firmware-updater" / "src" / "firmware_updater.cpp"


def run(command, *, env=None, check=True):
    return subprocess.run(command, env=env, check=check, text=True,
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT)


class AuthorizationTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = Path(tempfile.mkdtemp(prefix="firmware-updater-test-"))
        cls.binary = cls.temp / "firmware-updater"
        run(["g++", "-std=c++17", "-O2", "-Wall", "-Wextra", "-Wpedantic",
             str(SOURCE), "-o", str(cls.binary)])

        cls.ca_key = cls.temp / "operator-ca.key"
        cls.ca_cert = cls.temp / "operator-ca.pem"
        cls.operator_key = cls.temp / "operator.key"
        cls.operator_csr = cls.temp / "operator.csr"
        cls.operator_cert = cls.temp / "operator-certificate.pem"
        cls.extensions = cls.temp / "operator-extensions.cnf"
        cls.extensions.write_text(
            "basicConstraints=critical,CA:FALSE\n"
            "keyUsage=critical,digitalSignature\n"
            "subjectKeyIdentifier=hash\n"
            "authorityKeyIdentifier=keyid,issuer\n", encoding="ascii")
        run(["openssl", "req", "-x509", "-newkey", "rsa:2048", "-nodes",
             "-subj", "/CN=Test Operator CA/O=HiTEKS/C=IT",
             "-keyout", str(cls.ca_key), "-out", str(cls.ca_cert), "-days", "2",
             "-addext", "basicConstraints=critical,CA:TRUE",
             "-addext", "keyUsage=critical,keyCertSign,cRLSign"])
        run(["openssl", "req", "-newkey", "rsa:2048", "-nodes",
             "-subj", "/CN=Test Operator/OU=Firmware Update Operators/O=HiTEKS/C=IT",
             "-keyout", str(cls.operator_key), "-out", str(cls.operator_csr)])
        run(["openssl", "x509", "-req", "-in", str(cls.operator_csr),
             "-CA", str(cls.ca_cert), "-CAkey", str(cls.ca_key), "-CAcreateserial",
             "-out", str(cls.operator_cert), "-days", "2", "-sha256",
             "-extfile", str(cls.extensions)])
        ca_certificate = x509.load_pem_x509_certificate(cls.ca_cert.read_bytes())
        ca_private_key = serialization.load_pem_private_key(cls.ca_key.read_bytes(), None)
        now = datetime.now(timezone.utc)
        crl = (x509.CertificateRevocationListBuilder()
               .issuer_name(ca_certificate.subject)
               .last_update(now - timedelta(minutes=1))
               .next_update(now + timedelta(days=1))
               .sign(ca_private_key, hashes.SHA256()))
        cls.crl = cls.temp / "operator-crl.pem"
        cls.crl.write_bytes(crl.public_bytes(serialization.Encoding.PEM))

        cls.spoof_key = cls.temp / "role-spoof.key"
        cls.spoof_csr = cls.temp / "role-spoof.csr"
        cls.spoof_cert = cls.temp / "role-spoof-certificate.pem"
        run(["openssl", "req", "-newkey", "rsa:2048", "-nodes",
             "-subj", "/CN=OU=Firmware Update Operators/OU=Unauthorized/O=HiTEKS/C=IT",
             "-keyout", str(cls.spoof_key), "-out", str(cls.spoof_csr)])
        run(["openssl", "x509", "-req", "-in", str(cls.spoof_csr),
             "-CA", str(cls.ca_cert), "-CAkey", str(cls.ca_key), "-CAcreateserial",
             "-out", str(cls.spoof_cert), "-days", "2", "-sha256",
             "-extfile", str(cls.extensions)])

        cls.keys = cls.temp / "manufacturer-keys"
        cls.keys.mkdir()
        (cls.keys / "release.pem").write_text("test release public key\n", encoding="ascii")
        cls.audit_log = cls.temp / "security-audit.log"
        cls.fake_bin = cls.temp / "bin"
        cls.fake_bin.mkdir()
        cls._write_helpers()
        cls.env = os.environ.copy()
        cls.env["PATH"] = str(cls.fake_bin) + os.pathsep + cls.env["PATH"]
        cls.env["TEST_OPERATOR_CA"] = str(cls.ca_cert)
        cls.env["TEST_OPERATOR_CRL"] = str(cls.crl)
        cls.env["TEST_MANUFACTURER_KEY"] = str(cls.keys / "release.pem")

    @classmethod
    def tearDownClass(cls):
        shutil.rmtree(cls.temp, ignore_errors=True)

    @classmethod
    def _executable(cls, name, content):
        path = cls.fake_bin / name
        path.write_text(content, encoding="utf-8")
        path.chmod(path.stat().st_mode | stat.S_IXUSR)

    @classmethod
    def _write_helpers(cls):
        cls._executable("uci", f"""#!/bin/sh
key="${{3##*.}}"
case "$key" in
  operator_role_subject) printf '%s\\n' 'OU=Firmware Update Operators' ;;
  operator_role) printf '%s\\n' 'firmware-update' ;;
  device_id) printf '%s\\n' 'TG544-TEST-001' ;;
  max_authorization_seconds) printf '%s\\n' '86400' ;;
  manufacturer_keys_dir) printf '%s\\n' '{cls.keys}' ;;
  audit_log) printf '%s\\n' '{cls.audit_log}' ;;
  require_persistent_audit) printf '%s\\n' '1' ;;
  *) exit 1 ;;
esac
""")
        cls._executable("apk", "#!/bin/sh\nexit 0\n")
        cls._executable("tpm2_nvread", """#!/bin/sh
test "$1" = '-C' && test "$2" = 'o' && test "$3" = '-o' || exit 1
case "$5" in
  0x1500017) cp "$TEST_OPERATOR_CA" "$4" ;;
  0x1500018) cp "$TEST_OPERATOR_CRL" "$4" ;;
  0x1500019) cp "$TEST_MANUFACTURER_KEY" "$4" ;;
  *) exit 1 ;;
esac
""")
        cls._executable("jsonfilter", """#!/usr/bin/env python3
import json, sys
args = sys.argv[1:]
path = args[args.index('-i') + 1]
expression = args[args.index('-e') + 1]
with open(path, 'r', encoding='utf-8') as stream:
    value = json.load(stream)
mapping = {
    '@.schema_version': lambda: [value['schema_version']],
    '@.transaction_id': lambda: [value['transaction_id']],
    '@.device_id': lambda: [value['device_id']],
    '@.operator_role': lambda: [value['operator_role']],
    '@.not_before_epoch': lambda: [value['not_before_epoch']],
    '@.expires_epoch': lambda: [value['expires_epoch']],
    '@.packages[*].file': lambda: [item['file'] for item in value['packages']],
    '@.packages[*].sha256': lambda: [item['sha256'] for item in value['packages']],
}
for item in mapping[expression]():
    print(item)
""")

    def make_bundle(self, *, device_id="TG544-TEST-001", expired=False,
                    operator_cert=None, operator_key=None):
        bundle = Path(tempfile.mkdtemp(prefix="bundle-", dir=self.temp))
        package = bundle / "firmware-test-1.0.0-r1.apk"
        package.write_bytes(b"signed APK fixture")
        now = int(time.time())
        manifest = {
            "schema_version": 1,
            "transaction_id": "11111111-2222-4333-8444-555555555555",
            "device_id": device_id,
            "operator_role": "firmware-update",
            "not_before_epoch": now - (7200 if expired else 10),
            "expires_epoch": now - 3600 if expired else now + 3600,
            "packages": [{
                "file": package.name,
                "sha256": hashlib.sha256(package.read_bytes()).hexdigest(),
            }],
        }
        manifest_path = bundle / "update-manifest.json"
        manifest_path.write_text(json.dumps(manifest, indent=2), encoding="utf-8")
        shutil.copy2(operator_cert or self.operator_cert,
                     bundle / "operator-certificate.pem")
        run(["openssl", "dgst", "-sha256", "-sign",
             str(operator_key or self.operator_key),
             "-out", str(bundle / "update-manifest.json.sig"), str(manifest_path)])
        return bundle, package

    def verify(self, bundle):
        return run([str(self.binary), "--verify-bundle", str(bundle)],
                   env=self.env, check=False)

    def test_valid_bundle_is_accepted(self):
        bundle, _ = self.make_bundle()
        result = self.verify(bundle)
        self.assertEqual(result.returncode, 0, result.stdout)

    def test_service_health_requires_running_instance(self):
        harness = self.temp / "health.cpp"
        binary = self.temp / "health"
        harness.write_text(
            '#define main updater_main\n#include "' + str(SOURCE) +
            '"\n#undef main\nint main() { return service_running("ccli") ? 0 : 1; }\n',
            encoding="utf-8")
        run(["g++", "-std=c++17", str(harness), "-o", str(binary)])
        helpers = self.temp / "health-bin"
        helpers.mkdir(exist_ok=True)
        for name, text in {"ubus": '#!/bin/sh\nprintf "%s" "$HEALTH_JSON"\n',
                           "jsonfilter": '#!/usr/bin/env python3\nimport json,sys\nv=json.load(sys.stdin)\nfor s in v.values():\n for i in s.get("instances",{}).values():\n  print(str(i.get("running",False)).lower())\n'}.items():
            path = helpers / name
            path.write_text(text)
            path.chmod(0o700)
        env = self.env.copy()
        env["PATH"] = str(helpers) + os.pathsep + env["PATH"]
        for state, expected in [("{}", 1), ('{"ccli":{"instances":{}}}', 1),
                                ('{"ccli":{"instances":{"main":{"running":false}}}}', 1),
                                ('{"ccli":{"instances":{"main":{"running":true}}}}', 0)]:
            env["HEALTH_JSON"] = state
            self.assertEqual(run([str(binary)], env=env, check=False).returncode, expected)

    def test_modified_apk_is_rejected(self):
        bundle, package = self.make_bundle()
        package.write_bytes(b"tampered")
        result = self.verify(bundle)
        self.assertNotEqual(result.returncode, 0, result.stdout)

    def test_wrong_device_is_rejected(self):
        bundle, _ = self.make_bundle(device_id="TG544-OTHER-999")
        result = self.verify(bundle)
        self.assertNotEqual(result.returncode, 0, result.stdout)

    def test_expired_authorization_is_rejected(self):
        bundle, _ = self.make_bundle(expired=True)
        result = self.verify(bundle)
        self.assertNotEqual(result.returncode, 0, result.stdout)

    def test_modified_manifest_is_rejected(self):
        bundle, _ = self.make_bundle()
        manifest = bundle / "update-manifest.json"
        manifest.write_text(manifest.read_text(encoding="utf-8") + " ", encoding="utf-8")
        result = self.verify(bundle)
        self.assertNotEqual(result.returncode, 0, result.stdout)

    def test_role_text_in_wrong_subject_field_is_rejected(self):
        bundle, _ = self.make_bundle(operator_cert=self.spoof_cert,
                                     operator_key=self.spoof_key)
        result = self.verify(bundle)
        self.assertNotEqual(result.returncode, 0, result.stdout)

    def test_missing_tpm_ca_is_rejected(self):
        bundle, _ = self.make_bundle()
        env = self.env.copy()
        env["TEST_OPERATOR_CA"] = str(self.temp / "missing-ca.pem")
        result = run([str(self.binary), "--verify-bundle", str(bundle)],
                     env=env, check=False)
        self.assertNotEqual(result.returncode, 0, result.stdout)

    def test_tampered_tpm_crl_is_rejected(self):
        bundle, _ = self.make_bundle()
        tampered = self.temp / "tampered-crl.pem"
        tampered.write_bytes(self.crl.read_bytes().replace(b"A", b"B", 1))
        env = self.env.copy()
        env["TEST_OPERATOR_CRL"] = str(tampered)
        result = run([str(self.binary), "--verify-bundle", str(bundle)],
                     env=env, check=False)
        self.assertNotEqual(result.returncode, 0, result.stdout)

    def test_ccli_stop_uses_confirmed_inactive_state(self):
        source = SOURCE.read_text(encoding="utf-8")
        start = source.index("bool deactivate_ccli(")
        end = source.index("bool ensure_ccli_running(", start)
        function = source[start:end]
        self.assertIn('(void)run("/etc/init.d/" + service + " stop', function)
        self.assertIn("kCcliStopPolls", source)
        self.assertIn("for (int poll = 0; poll < kCcliStopPolls; ++poll)", function)
        self.assertIn("if (!service_running(service)) return true;", function)

    def test_transaction_is_recorded_only_after_apk_install(self):
        source = SOURCE.read_text(encoding="utf-8")
        start = source.index("bool install(")
        end = source.index("int process(", start)
        function = source[start:end]
        self.assertLess(function.index("apk_installed = run(command);"),
                        function.index("if (apk_installed && !consume_transaction(bundle))"))

    def test_missing_ccli_service_allows_initial_or_recovery_install(self):
        source = SOURCE.read_text(encoding="utf-8")
        start = source.index("bool deactivate_ccli(")
        end = source.index("bool ensure_ccli_running(", start)
        function = source[start:end]
        self.assertIn("service_present = fs::is_regular_file", function)
        self.assertIn("if (!service_present)", function)
        self.assertIn("was_running = false;", function)
        self.assertIn("return true;", function)

    def test_ccli_is_started_after_initial_install(self):
        source = SOURCE.read_text(encoding="utf-8")
        start = source.index("bool bundle_contains_ccli(")
        end = source.index("int process(", start)
        function = source[start:end]
        self.assertIn('name.rfind("ccli-", 0) == 0', function)
        self.assertIn("CCLI_NOT_INSTALLED", function)
        self.assertIn("CCLI_STARTED", function)
        self.assertIn("ensure_ccli_running(service)", function)

    def test_failed_deactivation_restores_ccli(self):
        source = SOURCE.read_text(encoding="utf-8")
        start = source.index("bool install(")
        end = source.index("int process(", start)
        function = source[start:end]
        abort = function.index('" CCLI deactivation failed"')
        self.assertLess(function.index("ensure_ccli_running(service)"), abort)

    def test_completed_install_always_starts_present_ccli(self):
        source = SOURCE.read_text(encoding="utf-8")
        start = source.index("bool install(")
        end = source.index("int process(", start)
        function = source[start:end]
        self.assertIn("const bool ccli_present_after_install", function)
        self.assertIn("bool restored = preservation_ok && ensure_ccli_running(service);", function)

    def test_r7_postinstall_restores_ccli_during_self_update(self):
        makefile = (ROOT / "apps" / "firmware-updater" / "Makefile").read_text(
            encoding="utf-8")
        self.assertIn("/etc/init.d/ccli start", makefile)


if __name__ == "__main__":
    unittest.main(verbosity=2)

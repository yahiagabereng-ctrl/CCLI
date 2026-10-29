"""Validate the canonical signed r63 APK without executing its install scripts."""
from pathlib import Path
import datetime as dt
import hashlib
import json
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[4]
TOOLKIT = ROOT / 'apps/firmware-updater'
APK = TOOLKIT / 'artifacts/ccli-0.1.0-r63.apk'
TOOL = TOOLKIT / 'tools/bin/apk'


def main():
    signature = subprocess.run(['bash', str(TOOLKIT / 'scripts/verify-apk.sh'), str(APK)],
                               check=True, capture_output=True, text=True)
    APK.with_suffix('.apk.signature.log').write_text(signature.stdout + signature.stderr)
    metadata = subprocess.check_output([str(TOOL), 'adbdump', str(APK)], text=True)
    APK.with_suffix('.apk.metadata.txt').write_text(metadata)
    assert '\n  name: ccli\n' in metadata and '\n  version: 0.1.0-r63\n' in metadata
    assert '\n  arch: aarch64_cortex-a53\n' in metadata
    with tempfile.TemporaryDirectory(prefix='ccli-r63-apk-check-') as temporary:
        work = Path(temporary)
        trust, payload = work / 'trust', work / 'payload'
        trust.mkdir()
        payload.mkdir()
        (trust / 'release.pem').write_bytes((TOOLKIT / 'keys/manufacturer/ccli-release.pub.pem').read_bytes())
        subprocess.run([str(TOOL), '--keys-dir', str(trust), '--no-network', 'extract',
                        '--no-chown', '--destination', str(payload), str(APK)], check=True)
        files = [path for path in payload.rglob('*') if path.is_file()]
        marker = re.compile(br'-----BEGIN (?:RSA |EC |DSA |OPENSSH |ENCRYPTED )?PRIVATE KEY-----')
        for path in files:
            relative = path.relative_to(payload)
            assert path.suffix.lower() not in ('.key', '.p12', '.pfx'), str(relative)
            contents = path.read_bytes()
            # The executable contains PEM parser marker strings, not key data.
            # Require a real encoded key body when inspecting ELF bytes.
            if contents.startswith(b'\x7fELF'):
                assert not re.search(marker.pattern + br'[\r\n]+[A-Za-z0-9+/=\r\n]{64,}', contents), str(relative)
            else:
                assert not marker.search(contents), str(relative)
            assert 'EJBCA RBAC' not in str(relative) and '/config/tls/' not in '/' + str(relative), str(relative)
        init = payload / 'etc/init.d/ccli'
        assert b'\r' not in init.read_bytes()
        assert init.stat().st_mode & 0o111
        binary_hash = hashlib.sha256((payload / 'usr/sbin/ccli').read_bytes()).hexdigest()
        record = {
            'package': 'ccli', 'version': '0.1.0-r63', 'codename': 'TSP-PRESENTATION-CONTEXT',
            'architecture': 'aarch64_cortex-a53',
            'source_commit': subprocess.check_output(['git', '-C', str(ROOT), 'rev-parse', 'HEAD'], text=True).strip(),
            'sdk': 'OpenWrt 25.12.5 mediatek/filogic GCC 14.3.0 musl',
            'verified_at_utc': dt.datetime.now(dt.timezone.utc).isoformat(),
            'apk_sha256': hashlib.sha256(APK.read_bytes()).hexdigest(), 'apk_bytes': APK.stat().st_size,
            'payload_binary_sha256': binary_hash, 'manufacturer_signature_verified': True,
            'payload_files': len(files), 'credential_files_or_text_pem_private_keys_found': False,
            'init_script_line_endings': 'LF', 'operator_confirmed_actual_tsp_connection': True,
            'device_installation_performed': False, 'usb_bundle_created': False,
        }
    APK.with_suffix('.build.json').write_text(json.dumps(record, indent=2))
    APK.with_suffix('.apk.sha256').write_text(record['apk_sha256'] + '  ' + APK.name + '\n')
    print(json.dumps(record, indent=2))


if __name__ == '__main__':
    main()

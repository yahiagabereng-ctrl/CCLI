#!/usr/bin/env python3
"""Exercise the actual package hooks against a temporary filesystem (Linux)."""
import os
from pathlib import Path
import re
import subprocess
import tempfile

REPO = Path(__file__).resolve().parents[1]
recipe = (REPO / 'package/ccli/Makefile').read_text()
with tempfile.TemporaryDirectory(prefix='ccli-upgrade-test-') as directory:
    root = Path(directory)
    (root / 'etc/ccli/tls').mkdir(parents=True)
    (root / 'etc/config').mkdir()
    (root / 'etc/init.d').mkdir()
    originals = {'etc/ccli/tls/server.key': b'private-test-fixture\n',
                 'etc/ccli/tls/server_tls.pem': b'certificate-test-fixture\n',
                 'etc/ccli/lab.yaml': b'mms:\n  rbac_profile_a: false\n  tls_enabled: true\n',
                 'etc/config/ccli': b"config ccli 'main'\n option enabled '1'\n"}
    for name, data in originals.items():
        (root / name).write_bytes(data)
        (root / name).chmod(0o600)
    def hook(name):
        body = re.search(r'^define Package/ccli/' + name + r'\n(.*?)^endef', recipe, re.M | re.S).group(1)
        body = body.replace('$$', '$').replace('/etc', str(root / 'etc')).replace('/usr', str(root / 'usr'))
        result = subprocess.run(['/bin/sh', '-c', body], capture_output=True, text=True,
                                env={**os.environ, 'IPKG_INSTROOT': ''})
        if result.returncode:
            raise RuntimeError(f'{name}: {result.stderr}')
        return result.stdout
    hook('preinst')
    assert (root / 'etc/ccli-upgrade.pending').exists()
    # Simulate apk deleting credentials formerly owned by the old package and
    # introducing a default YAML during an upgrade.
    (root / 'etc/ccli/tls/server.key').unlink()
    (root / 'etc/ccli/tls/server_tls.pem').unlink()
    (root / 'etc/ccli/lab.yaml').write_text('new-default\n')
    hook('postinst')
    for name, data in originals.items():
        assert (root / name).read_bytes() == data, name
        assert (root / name).stat().st_mode & 0o777 == 0o600, name
    assert not (root / 'etc/ccli-upgrade.pending').exists()
    assert (root / 'etc/ccli-upgrade.hold').exists()
    backups = list((root / 'etc').glob('ccli-upgrade.??????'))
    assert len(backups) == 1 and backups[0].stat().st_mode & 0o777 == 0o700
    init = (REPO / 'package/ccli/files/ccli.init').read_text().replace('/etc', str(root / 'etc'))
    # Override logger so this isolated test never writes to the host's syslog.
    blocked = subprocess.run(['/bin/sh', '-c', init + '\nlogger() { :; }; start_service'], capture_output=True)
    assert blocked.returncode == 1, 'service must remain held after restoration'
    assert 'config/tls/*' not in recipe, 'APK must not contain live TLS credentials'
print('PASS: obsolete TLS files restored, active configuration preserved, private backup retained, automatic start held')

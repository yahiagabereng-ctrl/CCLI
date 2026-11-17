import os
from pathlib import Path
import subprocess
import device_r63

BASE = Path(__file__).resolve().parent
ROOT = BASE.parents[3]
artifact = ROOT / 'apps/firmware-updater/artifacts/ccli-0.1.0-r63.apk'
destination = 'root@192.168.0.1:/tmp/ccli-apk-r63-20261020/ccli-0.1.0-r63.apk'
result = subprocess.run([str(device_r63.PUTTY / 'pscp.exe'), *device_r63.COMMON,
                         '-scp', str(artifact), destination], capture_output=True, text=True, timeout=120)
if result.returncode:
    raise RuntimeError('APK upload failed: ' + result.stderr)
output = device_r63.ssh(script='install_release_apk.sh', timeout=240)
(ROOT / 'apps/firmware-updater/artifacts/ccli-0.1.0-r63.install.txt').write_text(output)

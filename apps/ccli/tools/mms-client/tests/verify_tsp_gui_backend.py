"""Exercise the GUI's JSON bridge with the real client, using read-only services."""
import json
from pathlib import Path
import subprocess
import sys

HERE = Path(__file__).resolve().parents[1]
ROOT = HERE.parents[3]
tls = ROOT / 'apps/ccli/config/tls'
identity = ROOT / 'apps/ccli/config/EJBCA RBAC/dual-certificate/dso-mms.pem'
config = {'host': '192.168.0.1', 'port': 3782, 'auth_mode': 'certificate',
          'ca': str(tls / 'root_CA.pem'), 'tls_cert': str(tls / 'client_tls.pem'), 'tls_key': str(tls / 'client_tsp.key'),
          'identity_cert': str(identity), 'identity_key': str(identity.with_suffix('.key')),
          'binary': str(ROOT / 'apps/ccli/build-mms-client/ccli-mms-client')}
messages = [{'action': 'connect', 'config': config},
            {'action': 'request', 'id': 1, 'command': 'read', 'reference': 'CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f', 'fc': 'MX'},
            {'action': 'request', 'id': 2, 'command': 'control-info', 'reference': 'CCI016_01LD_Plant/WlimDWMX1.Mod'},
            {'action': 'disconnect'}]
result = subprocess.run([sys.executable, str(HERE / 'gui_backend.py')],
                        input=''.join(json.dumps(message) + '\n' for message in messages),
                        capture_output=True, text=True, timeout=35)
assert result.returncode == 0, result.stderr
responses = [json.loads(line) for line in result.stdout.splitlines()]
assert [response['type'] for response in responses] == ['connected', 'result', 'result', 'disconnected'], responses
assert responses[0]['server_certificate']['signature_verified'] and 'TSP' in responses[0]['wire_profile']
assert all(response['code'] == 0 for response in responses[1:3]), responses
print('PASS: GUI connect exposes TSP profile and verified server certificate; read/control-info/disconnect succeed')

bad_config = dict(config, mms_ca=str(ROOT / 'apps/ccli/config/EJBCA RBAC/rbac_ca.pem'))
negative = subprocess.run([sys.executable, str(HERE / 'gui_backend.py')],
                         input=json.dumps({'action': 'connect', 'config': bad_config}) + '\n'
                         + json.dumps({'action': 'disconnect'}) + '\n',
                         capture_output=True, text=True, timeout=35)
denied = [json.loads(line) for line in negative.stdout.splitlines()]
assert negative.returncode == 0 and [item['type'] for item in denied] == ['error', 'disconnected'], denied
assert 'configured MMS CA' in denied[0]['error'] and denied[0]['status'].startswith('DISCONNECTED'), denied
print('PASS: Wrong MMS CA prevents CONNECTED and the native association is rejected before operations')

"""Control/write regression restricted to the separate, synthetic port 3783 harness."""
import argparse
import copy
import json
from pathlib import Path
import sys

from cryptography import x509
from cryptography.hazmat.primitives import serialization

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import client


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    options = parser.parse_args()
    config = client.ROOT / 'apps/ccli/config'
    tls = config / 'tls'
    base = argparse.Namespace(host='192.168.0.1', port=3783, ca=tls / 'root_CA.pem',
        binary=client.ROOT / 'apps/ccli/build-mms-client/ccli-mms-client', auth_mode='certificate',
        ap_title='1.1.1.999.1', ae_qualifier=12, verbose=False, command='read', reference='',
        fc='ST', type='int32', count=1, value=None, members=None, yes=True, report=None)
    results = []
    for role, transport_key in [('dso', 'client_tsp.key'), ('viewer', 'viewer.key')]:
        args = copy.copy(base)
        args.tls_cert = tls / ('client_tls.pem' if role == 'dso' else 'viewer_tls.pem')
        args.tls_key = tls / transport_key
        identity = config / ('EJBCA RBAC/dual-certificate/' + role + '-mms.pem')
        cert = x509.load_pem_x509_certificate(identity.read_bytes())
        key = serialization.load_pem_private_key(identity.with_suffix('.key').read_bytes(), password=None)
        with client.PersistentSession(args, cert, key) as session:
            def request(name, command, reference, allowed=True, **kwargs):
                operation = copy.copy(args)
                operation.command, operation.reference = command, reference
                for field, value in kwargs.items(): setattr(operation, field, value)
                code, events = session.execute(operation, quiet=True)
                passed = code == 0 if allowed else code != 0 and any(event['error'] == 21 for event in events)
                results.append({'role': role, 'case': name, 'passed': passed, 'events': events})
                assert passed, results[-1]
                return events
            root = 'CCI016_01LD_Plant/WlimDWMX1'
            request('read', 'read', root + '.Mod.stVal')
            request('configuration-write', 'write', root + '.WMaxSptPct.ctlModel', role == 'dso', fc='CF', value='3')
            events = request('operate', 'operate', root + '.Mod', role == 'dso', value='1')
            if role == 'dso':
                assert any(event['kind'] == 'readback_match' and event['error'] == 0 for event in events)
                request('operate-off', 'operate', root + '.Mod', value='5')
            request('read-after-control-or-denial', 'read', root + '.Mod.stVal')
    options.output.parent.mkdir(parents=True, exist_ok=True)
    options.output.write_text(json.dumps({'passed': True, 'plant_io': False, 'port': 3783, 'checks': results}, indent=2))
    print('PASS: DSO read/write/operate/readback; Viewer read and write/operate denial; connection survives denied actions')


if __name__ == '__main__':
    main()

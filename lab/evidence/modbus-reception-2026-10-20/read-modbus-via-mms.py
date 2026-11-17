"""Read-only bench check through the existing MMS client, one association."""
import argparse
import json
from pathlib import Path
import sys
from cryptography import x509
from cryptography.hazmat.primitives import serialization

root = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(root / 'apps/ccli/tools/mms-client'))
import client

pki = root.parent / 'CCLI/lab/ejbca-staging/rbac/viewer-r0'
cert_path = pki / 'viewer-r0.pem'
key_path = pki / 'viewer-r0.key'
cert = x509.load_pem_x509_certificate(cert_path.read_bytes())
key = serialization.load_pem_private_key(key_path.read_bytes(), password=None)
args = argparse.Namespace(host='192.168.0.1', port=3782,
    ca=root / 'apps/ccli/config/tls/root_CA.pem', tls_cert=cert_path, tls_key=key_path,
    auth_mode='tls', ap_title='1.1.1.999.1', ae_qualifier=12,
    binary=root / 'apps/ccli/build-mms-client/ccli-mms-client', verbose=False,
    command='read', reference='', fc='MX', count=1, type='float', value=None,
    members=None, yes=False, report=None)
results = []
session = client.PersistentSession(args, cert, key)
try:
    for suffix in ('TotW.mag.f', 'TotVAr.mag.f', 'TotW.q', 'TotVAr.q', 'TotW.t'):
        args.reference = client.PLANT + '/PdCMMXU1.' + suffix
        code, events = client.run_operation(args, cert, key, session=session)
        results.append(dict(reference=args.reference, exit_code=code, events=events))
    args.command = 'watch'
    args.reference = client.PLANT + '/PdCMMXU1.TotW.mag.f'
    args.count = 3
    code, events = client.run_operation(args, cert, key, session=session)
    results.append(dict(reference=args.reference, command='watch', exit_code=code, events=events))
finally:
    session.close()
    Path(__file__).with_name('readings.json').write_text(
        json.dumps({'identity':'viewer-r0','read_only':True,'checks':results}, indent=2)+'\n')


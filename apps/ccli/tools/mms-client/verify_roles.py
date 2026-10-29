#!/usr/bin/env python3
"""Exercise commissioned identities through this client's persistent MMS connection.

Full mode is restricted to the loopback harness (3784), which has no plant I/O.
Live mode sends only discovery/read/dataset-read/RCB-read requests, never writes.
Certificate keys are loaded from the supplied PKI directory and never exported.
"""
import argparse
import contextlib
import copy
import datetime as dt
import hashlib
import io
import json
from pathlib import Path
import subprocess
import time
import xml.etree.ElementTree as ET

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
import client

P = client.PLANT + '/'
MEAS = P + 'PdCMMXU1.TotW.mag.f'
CONTROL = P + 'WlimDWMX1.Mod'
CONFIG = P + 'WlimDWMX1.WMaxSptPct.ctlModel'
AGG = P + 'WSaDAGC1.Mod.stVal'
DATASET = P + 'LLN0.DS_R_PdC_Mis4sec'
RCB = P + 'LLN0.RP.urcb_PdC_Mis4sec'
READ_ONLY = {'browse', 'read', 'dataset-members', 'dataset-read', 'rcb-read', 'control-info'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--pki', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--live', action='store_true')
    parser.add_argument('--host', default='192.168.0.1')
    parser.add_argument('--server', type=Path)
    parser.add_argument('--config', type=Path)
    parser.add_argument('--fixed-tls-pins', action='store_true',
                        help='Use the supplied isolated TLS pins unchanged for every identity')
    parser.add_argument('--only', nargs='+', help='Optional identity labels for focused investigation')
    options = parser.parse_args()
    if not options.live and (not options.server or not options.config):
        parser.error('isolated mode requires --server and --config')
    options.output.mkdir(parents=True, exist_ok=True)
    package = options.pki / 'dual-certificate/commissioning-sequence3'
    bindings = {e.attrib['Sha256']: e.attrib for e in ET.parse(package / 'identity-bindings.xml').getroot()}
    names = ['dso', 'aggregator'] + [f'{r}-r{v}' for v in (0, 1)
        for r in ('viewer', 'operator', 'engineer', 'installer', 'secadm', 'secaud', 'rbacmnt')]
    names += ['multi-viewer-engineer', 'dual-dso', 'dual-viewer']
    negatives = ['negative-' + n for n in ('no-roles', 'revoked', 'unknown-role', 'wrong-aor', 'wrong-definition', 'wrong-revision')]
    if options.only:
        names = [n for n in names if n in options.only]
        negatives = [n for n in negatives if n in options.only]
    results, identities = [], []
    metadata = {'started_utc': dt.datetime.now(dt.timezone.utc).isoformat(),
        'mode': 'live-read-only' if options.live else 'isolated-no-plant-io',
        'host': options.host if options.live else '127.0.0.1', 'port': 3782 if options.live else 3784,
        'policy_sha256': {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in package.glob('*.xml')},
        'limitations': ['Aggregator P is an export ceiling, not signed import/export tracking; own datasets/reports are not allocated.',
            'Representative service/object checks; this is not exhaustive certification.',
            'Dynamic dataset services are globally disabled and do not establish per-role permissions.',
            ('Isolated TLS pins are reused unchanged for every identity.' if options.fixed_tls_pins else
             'Isolated mode rotates the explicit TLS peer pin per identity; live TLS pins remain unchanged.')]}
    base = argparse.Namespace(host=metadata['host'], port=metadata['port'],
        ca=client.ROOT / 'apps/ccli/config/tls/root_CA.pem',
        binary=client.ROOT / 'apps/ccli/build-mms-client/ccli-mms-client',
        ap_title='1.1.1.999.1', ae_qualifier=12, verbose=False,
        command='read', reference='', fc='ST', type='int32', count=5, value=None,
        members=None, yes=True, report=None)

    def save():
        document = dict(metadata, identities=identities, checks=results,
            passed=sum(r['passed'] for r in results), failed=sum(not r['passed'] for r in results))
        (options.output / 'results.json').write_text(json.dumps(document, indent=2) + '\n')

    def record(label, case, expected, passed, **details):
        result = dict(identity=label, case=case, expected=expected, passed=bool(passed), **details)
        results.append(result)
        print(f'{"PASS" if passed else "FAIL"}: {label} {case} expected={expected}', flush=True)
        save()

    def credentials(label):
        a = copy.copy(base)
        if label.startswith('dual-'):
            name = label[5:]
            a.auth_mode = 'certificate'
            cert_path = options.pki / f'dual-certificate/{name}-mms.pem'
            key_path = options.pki / f'dual-certificate/{name}-mms.key'
            if name == 'dso':
                a.tls_cert = client.ROOT / 'apps/ccli/config/tls/client_tls.pem'
                a.tls_key = client.ROOT / 'apps/ccli/config/tls/client_tsp.key'
            else:
                a.tls_cert = client.ROOT / 'apps/ccli/config/tls/viewer_tls.pem'
                a.tls_key = client.ROOT / 'apps/ccli/config/tls/viewer.key'
        else:
            a.auth_mode = 'tls'
            cert_path, key_path = options.pki / label / (label + '.pem'), options.pki / label / (label + '.key')
            a.tls_cert, a.tls_key = cert_path, key_path
        cert = x509.load_pem_x509_certificate(cert_path.read_bytes())
        key = serialization.load_pem_private_key(key_path.read_bytes(), password=None)
        return a, cert, key

    def request(label, session, a, cert, key, case, cmd, ref, allowed, raw=False, **kwargs):
        if options.live and cmd not in READ_ONLY:
            raise RuntimeError('refusing a state-changing request in live mode')
        req = copy.copy(a)
        req.command, req.reference = cmd, ref
        for k, v in kwargs.items(): setattr(req, k, v)
        # Raw calls deliberately bypass the UI's MX-write guard to verify server enforcement.
        if raw: code, events = session.execute(req, quiet=True)
        else: code, events = client.run_operation(req, cert, key, quiet=True, session=session)
        denied = any(e['error'] == 21 for e in events)
        passed = code == 0 if allowed else code != 0 and denied
        if cmd == 'browse' and allowed: passed = passed and any(e['kind'] == 'entry' for e in events)
        if cmd == 'operate' and allowed:
            passed = passed and any(e['kind'] == 'readback_match' and e['error'] == 0 for e in events)
        record(label, case, 'allow' if allowed else 'access-denied', passed,
               command=cmd, reference=ref, fc=req.fc, exit_code=code, events=events, bypass_ui_guard=raw)
        return code, events

    server = log = None
    def stop_server():
        nonlocal server, log
        if server and server.poll() is None:
            server.terminate()
            try: server.wait(timeout=15)
            except subprocess.TimeoutExpired: server.kill(); server.wait()
        if log: log.close()
        server = log = None

    def start_server(label, transport_cert):
        nonlocal server, log
        stop_server()
        folder = options.output / label
        folder.mkdir(parents=True, exist_ok=True)
        replacements = {'bind_address': '127.0.0.1', 'tcp_port': '3784', 'comms_loss_fallback_s': '0',
            'rbac_activation_state': str(folder / 'activation.state'),
            'path': str(folder / 'events.jsonl')}
        if not options.fixed_tls_pins: replacements['tls_client_cert'] = str(transport_cert)
        lines = []
        for line in options.config.read_text().splitlines():
            k = line.strip().split(':', 1)[0]
            if k in replacements: line = '  ' + k + ': ' + replacements[k]
            lines.append(line)
        config = folder / 'server.yaml'
        config.write_text('\n'.join(lines) + '\n')
        log_path = folder / 'server.log'
        log = log_path.open('w')
        server = subprocess.Popen([str(options.server), str(config), '900'], stdout=log, stderr=log)
        for _ in range(100):
            if server.poll() is not None: raise RuntimeError(log_path.read_text()[-3000:])
            if 'ISOLATED_RBAC_TEST_READY' in log_path.read_text(): break
            time.sleep(.2)
        else: raise RuntimeError('isolated server did not become ready')

    try:
        for label in names:
            a, cert, key = credentials(label)
            fp = cert.fingerprint(hashes.SHA256()).hex()
            binding = bindings.get(fp)
            if not binding: raise RuntimeError(f'{label} is not commissioned')
            roles = {int(x) for x in binding['Roles'].split(',')}
            peer = x509.load_pem_x509_certificate(a.tls_cert.read_bytes())
            if peer.fingerprint(hashes.SHA256()).hex() != binding['TransportSha256']:
                raise RuntimeError(f'{label} transport identity does not match commissioning binding')
            identities.append(dict(label=label, roles=sorted(roles), authentication=a.auth_mode,
                certificate_sha256=fp, transport_sha256=binding['TransportSha256']))
            agg, dso = -2 in roles, -1 in roles
            config = bool(roles & {-1, 2, 3, 4, 6})
            control = bool(roles & {-1, 1})
            dataset = bool(roles & {-1, 2, 4})
            reports = bool(roles & {-1, 0, 1, 2, 3, 5})
            if not options.live: start_server(label, a.tls_cert)
            try:
                with contextlib.redirect_stdout(io.StringIO()): session = client.PersistentSession(a, cert, key)
            except OSError as exc:
                record(label, 'association', 'allow', False, error=str(exc)); continue
            with session:
                record(label, 'association', 'allow', True)
                def run(case, cmd, ref, allow, **kw):
                    return request(label, session, a, cert, key, case, cmd, ref, allow, **kw)
                run('measurement-read', 'read', MEAS, not agg, fc='MX')
                run('dso-state-read', 'read', CONTROL + '.stVal', not agg)
                if agg or dso: run('aggregator-scope-read', 'read', AGG, agg)
                run('discovery-in-scope', 'browse', P + ('WSaDAGC1' if agg else 'WlimDWMX1'), True)
                run('dataset-directory', 'dataset-members', DATASET, dataset)
                run('dataset-values', 'dataset-read', DATASET, dataset)
                run('report-block-read', 'rcb-read', RCB, reports)
                if not options.live:
                    aggregator_control = bool(roles & {-2, 1})
                    for name, ref in [('active', P + 'WSaDAGC1.Mod'), ('reactive', P + 'VArSaDVAR1.Mod')]:
                        run('aggregator-' + name + '-operate', 'operate', ref, aggregator_control, value='1')
                        if aggregator_control:
                            run('aggregator-' + name + '-off', 'operate', ref, True, value='5')
                    run('report-enable-delivery-disable', 'reports', RCB, reports)
                    if reports:
                        run('report-general-interrogation', 'reports-gi', RCB, not dso)
                    run('configuration-write', 'write', CONFIG, config, fc='CF', value='3')
                    if not agg:
                        run('operate-with-readback', 'operate', CONTROL, control, value='1')
                        if control: run('operate-off-with-readback', 'operate', CONTROL, True, value='5')
                    # Access must be denied by the server even if a custom client bypasses UI checks.
                    run('measurement-write-rejected', 'write', MEAS, False, raw=True,
                        fc='MX', type='float', value='999')
                    run('state-write-rejected', 'write', CONTROL + '.stVal', False,
                        raw=True, fc='ST', value='1')
                    code, events = run('read-after-denied-requests', 'read', AGG if agg else CONTROL + '.stVal', True)
                    dynamic = copy.copy(a)
                    dynamic.command, dynamic.reference = 'dataset-create', P + 'LLN0.RoleProbe'
                    dynamic.members = [P + 'PdCMMXU1.TotW[MX]']
                    code, events = client.run_operation(dynamic, cert, key, quiet=True, session=session)
                    record(label, 'dynamic-datasets-disabled', 'service-disabled', code != 0,
                           exit_code=code, events=events)

        for label in negatives:
            a, cert, key = credentials(label)
            if not options.live: start_server(label, a.tls_cert)
            a.command, a.reference, a.fc = 'read', MEAS, 'MX'
            code, events = client.run_operation(a, cert, key, quiet=True)
            association = [e for e in events if e['kind'] == 'association']
            record(label, 'invalid-identity-rejected', 'association-rejected',
                bool(association) and association[0]['error'] != 0 and code != 0, events=events)

        if not options.only or 'unbound-transport' in options.only:
            # A valid application certificate is still forbidden with an unbound transport certificate.
            a, cert, key = credentials('dual-dso')
            a.tls_cert = client.ROOT / 'apps/ccli/config/tls/viewer_tls.pem'
            a.tls_key = client.ROOT / 'apps/ccli/config/tls/viewer.key'
            if not options.live: start_server('unbound-transport', a.tls_cert)
            a.command, a.reference, a.fc = 'read', MEAS, 'MX'
            code, events = client.run_operation(a, cert, key, quiet=True)
            association = [e for e in events if e['kind'] == 'association']
            rejected = bool(association) and association[0]['error'] != 0
            denied = any(e['kind'] == 'read' and e['error'] == 21 for e in events)
            record('dual-dso-with-viewer-transport', 'unbound-transport-data-blocked', 'fail-closed',
                code != 0 and (rejected or denied), events=events)
            if not options.live and not rejected:
                with contextlib.redirect_stdout(io.StringIO()): session = client.PersistentSession(a, cert, key)
                with session:
                    label = 'dual-dso-with-viewer-transport'
                    for case, cmd, ref, kw in [
                        ('unbound-state-read', 'read', CONTROL + '.stVal', {'fc': 'ST'}),
                        ('unbound-config-write', 'write', CONFIG, {'fc': 'CF', 'value': '3'}),
                        ('unbound-control-initiation', 'operate', CONTROL, {'value': '1'}),
                        ('unbound-dataset', 'dataset-members', DATASET, {}),
                        ('unbound-report-read', 'rcb-read', RCB, {})]:
                        request(label, session, a, cert, key, case, cmd, ref, False, **kw)
    finally:
        stop_server()
        metadata['finished_utc'] = dt.datetime.now(dt.timezone.utc).isoformat()
        save()
    failures = [r for r in results if not r['passed']]
    print(f'RESULT: {len(results)-len(failures)} passed; {len(failures)} failed', flush=True)
    return bool(failures)


if __name__ == '__main__': raise SystemExit(main())

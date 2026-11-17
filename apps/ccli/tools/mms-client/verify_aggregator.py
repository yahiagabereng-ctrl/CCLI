#!/usr/bin/env python3
"""Aggregator controls on the loopback-only server; never sends plant I/O."""
import argparse
import contextlib
import copy
import io
import json
from pathlib import Path
import re
import subprocess
import time
from cryptography import x509
from cryptography.hazmat.primitives import serialization
import client

P = client.PLANT + '/'
AP = P + 'WSaDAGC1'
AQ = P + 'VArSaDVAR1'


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--pki', type=Path, required=True)
    p.add_argument('--server', type=Path, required=True)
    p.add_argument('--config', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    o = p.parse_args()
    o.output.mkdir(parents=True, exist_ok=True)
    results = []
    replacements = {'bind_address': '127.0.0.1', 'tcp_port': '3784', 'comms_loss_fallback_s': '2',
        'path': str(o.output/'events.jsonl'), 'rbac_activation_state': str(o.output/'activation.state')}
    lines = []
    for line in o.config.read_text().splitlines():
        k = line.strip().split(':', 1)[0]
        if k in replacements: line = '  ' + k + ': ' + replacements[k]
        lines.append(line)
    lines += ['dso:', '  enabled: true', '  profile: tr57126_table1', '  release_delta_kw: 5']
    cfg = o.output/'server.yaml'
    cfg.write_text('\n'.join(lines)+'\n')
    log_path = o.output/'server.log'
    log = log_path.open('w')
    server = subprocess.Popen([str(o.server), str(cfg), '300'], stdout=log, stderr=log)
    base = argparse.Namespace(host='127.0.0.1', port=3784, auth_mode='tls',
        ca=client.ROOT/'apps/ccli/config/tls/root_CA.pem',
        binary=client.ROOT/'apps/ccli/build-mms-client/ccli-mms-client',
        ap_title='1.1.1.999.1', ae_qualifier=12, verbose=False,
        command='read', reference='', fc='ST', type='int32', count=1,
        value=None, members=None, yes=True, report=None)
    def record(name, passed, **details):
        results.append(dict(case=name, passed=bool(passed), **details))
        (o.output/'results.json').write_text(json.dumps(results, indent=2)+'\n')
        print(('PASS: ' if passed else 'FAIL: ')+name, flush=True)
        if not passed: raise AssertionError(name)
    def session(label):
        a = copy.copy(base)
        a.tls_cert, a.tls_key = o.pki/label/(label+'.pem'), o.pki/label/(label+'.key')
        cert = x509.load_pem_x509_certificate(a.tls_cert.read_bytes())
        key = serialization.load_pem_private_key(a.tls_key.read_bytes(), password=None)
        with contextlib.redirect_stdout(io.StringIO()): s = client.PersistentSession(a, cert, key)
        s.test_args, s.test_cert, s.test_key = a, cert, key
        return s
    def request(s, name, command, ref, expected='allow', **kw):
        a = copy.copy(s.test_args)
        a.command, a.reference = command, ref
        for k,v in kw.items(): setattr(a,k,v)
        code, events = client.run_operation(a, s.test_cert, s.test_key, quiet=True, session=s)
        ok = code == 0 if expected == 'allow' else code != 0
        if expected == 'access-denied': ok = ok and any(e['error'] == 21 for e in events)
        record(name, ok, command=command, reference=ref, expected=expected, events=events)
        return events
    def effect(name, p_kw, q_kvar, source):
        deadline = time.monotonic()+3
        values = []
        while time.monotonic() < deadline:
            values = re.findall(r'POWER_COMMAND p_eff_kw=([-\d.]+) q_target_kvar=([-\d.]+) q_source=(\S+)', log_path.read_text())
            if values and abs(float(values[-1][0])-p_kw)<.01 and abs(float(values[-1][1])-q_kvar)<.01 and values[-1][2] == source:
                record(name, True, derived=values[-1]); return
            time.sleep(.1)
        record(name, False, observed=values[-1:] or [])
    try:
        for _ in range(100):
            if server.poll() is not None: raise RuntimeError(log_path.read_text()[-3000:])
            if 'ISOLATED_RBAC_TEST_READY' in log_path.read_text(): break
            time.sleep(.1)
        else: raise RuntimeError('isolated harness not ready')
        with session('aggregator') as agg:
            request(agg, 'P-setpoint', 'operate', AP+'.WSptPct', value='10', type='float')
            request(agg, 'setpoint-spacing-enforced', 'operate', AP+'.WSptPct', 'rejected', value='11', type='float')
            request(agg, 'P-mode-on', 'operate', AP+'.Mod', value='1')
            effect('P-affects-export-ceiling', 21, 0, 'none')
            for value in ('-1', '101'):
                request(agg, 'P-reject-'+value, 'operate', AP+'.WSptPct', 'rejected', value=value, type='float')
            for value in ('0', '2', '7'):
                request(agg, 'mode-reject-'+value, 'operate', AP+'.Mod', 'rejected', value=value)
            readback = request(agg, 'P-unchanged-after-invalid-values', 'read', AP+'.WSptPct.mxVal.f', fc='MX')
            record('invalid-P-writes-do-not-change-state', any(e['kind']=='read' and float(e['value'])==10 for e in readback))
            time.sleep(3.1)
            request(agg, 'Q-setpoint', 'operate', AQ+'.VArTgtSptPct', value='-10', type='float')
            request(agg, 'Q-mode-on', 'operate', AQ+'.Mod', value='1')
            effect('Q-affects-reactive-selection', 21, -21, 'AGGREGATOR_VArSa')
            for value in ('-101', '101'):
                request(agg, 'Q-reject-'+value, 'operate', AQ+'.VArTgtSptPct', 'rejected', value=value, type='float')
            request(agg, 'aggregator-cannot-control-DSO', 'operate', P+'WlimDWMX1.Mod', 'access-denied', value='1')
            with session('dso') as dso:
                request(dso, 'DSO-cannot-control-aggregator', 'operate', AP+'.Mod', 'access-denied', value='1')
                time.sleep(3.1)
                request(dso, 'DSO-Q-setpoint', 'operate', P+'VArSdDVAR1.VArTgtSptPct', value='20', type='float')
                request(dso, 'DSO-Q-on', 'operate', P+'VArSdDVAR1.Mod', value='1')
                effect('DSO-Q-takes-priority', 21, 42, 'DSO_VArSd')
                time.sleep(3.1)
                request(agg, 'aggregator-Q-request-under-DSO', 'operate', AQ+'.VArTgtSptPct', value='-15', type='float')
                effect('aggregator-cannot-override-DSO-Q', 21, 42, 'DSO_VArSd')
                request(dso, 'DSO-Q-off', 'operate', P+'VArSdDVAR1.Mod', value='5')
                effect('DSO-release-restores-aggregator-Q', 21, -31.5, 'AGGREGATOR_VArSa')
            with session('viewer-r1') as viewer:
                request(viewer, 'viewer-cannot-control-aggregator', 'operate', AQ+'.Mod', 'access-denied', value='1')
        time.sleep(2.8)
        with session('aggregator') as agg:
            for name, ref, fc, expected in [('P-mode', AP+'.Mod.stVal','ST',5), ('Q-mode',AQ+'.Mod.stVal','ST',5),
                ('P-value',AP+'.WSptPct.mxVal.f','MX',100), ('Q-value',AQ+'.VArTgtSptPct.mxVal.f','MX',0)]:
                events = request(agg, 'fallback-'+name, 'read', ref, fc=fc)
                record('fallback-'+name+'-matches', any(e['kind']=='read' and float(e['value'])==expected for e in events))
        print(f'RESULT: {len(results)} checks passed', flush=True)
    finally:
        if server.poll() is None:
            server.terminate()
            try: server.wait(timeout=15)
            except subprocess.TimeoutExpired: server.kill(); server.wait()
        log.close()


if __name__ == '__main__': main()

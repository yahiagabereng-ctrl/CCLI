#!/usr/bin/env python3
"""Explicit integration regression on a localhost harness with no plant I/O."""
import argparse
import copy
import json
from pathlib import Path
import signal
import subprocess
import threading
import time

from cryptography import x509
from cryptography.hazmat.primitives import serialization
import client


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--server', type=Path, required=True, help='Fallback-capable mms-rbac-dual-host-server')
    p.add_argument('--config', type=Path, required=True, help='Existing isolated config with credential paths')
    p.add_argument('--identity-cert', type=Path, required=True)
    p.add_argument('--identity-key', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    opts = p.parse_args()
    opts.output.mkdir(parents=True, exist_ok=True)
    text = opts.config.read_text()
    # Match the host harness's enforced loopback-only port and separate state/log.
    replacements = {'bind_address': '127.0.0.1', 'tcp_port': '3784', 'comms_loss_fallback_s': '15',
                    'rbac_activation_state': str(opts.output / 'activation.state'), 'path': str(opts.output / 'events.jsonl')}
    lines = []
    for line in text.splitlines():
        key = line.strip().split(':', 1)[0]
        if key in replacements: line = '  ' + key + ': ' + replacements[key]
        lines.append(line)
    config = opts.output / 'server.yaml'
    config.write_text('\n'.join(lines) + '\n')
    args = argparse.Namespace(host='127.0.0.1', port=3784,
        ca=client.ROOT / 'apps/ccli/config/tls/root_CA.pem',
        tls_cert=client.ROOT / 'apps/ccli/config/tls/client_tls.pem',
        tls_key=client.ROOT / 'apps/ccli/config/tls/client_tsp.key',
        binary=client.ROOT / 'apps/ccli/build-mms-client/ccli-mms-client',
        ap_title='1.1.1.999.1', ae_qualifier=12, verbose=False,
        command='read', reference='', fc='ST', type='int32', count=1, value=None, members=None, yes=True, report=None)
    cert = x509.load_pem_x509_certificate(opts.identity_cert.read_bytes())
    key = serialization.load_pem_private_key(opts.identity_key.read_bytes(), password=None)
    results = []

    def record(name, passed):
        results.append({'case': name, 'passed': bool(passed)})
        print(('PASS: ' if passed else 'FAIL: ') + name, flush=True)
        if not passed: raise AssertionError(name)

    def request(session, name, command, reference, expected=0, **kwargs):
        a = copy.copy(args)
        a.command, a.reference = command, reference
        a.report = opts.output / (name + '.json')
        for k, v in kwargs.items(): setattr(a, k, v)
        code, events = client.run_operation(a, cert, key, quiet=True, session=session)
        record(name, code == expected)
        return events

    log_path = opts.output / 'server.log'
    log = log_path.open('w')
    server = subprocess.Popen([str(opts.server), str(config), '180'], stdout=log, stderr=log)
    try:
        for _ in range(80):
            if server.poll() is not None: raise RuntimeError(log_path.read_text()[-3000:])
            if 'ISOLATED_RBAC_TEST_READY' in log_path.read_text(): break
            time.sleep(.25)
        else: raise RuntimeError('harness did not start')
        ref = 'CCI016_01LD_Plant/WlimDWMX1.Mod'
        with client.PersistentSession(args, cert, key) as session:
            events = request(session, 'operate-with-readback', 'operate', ref, value='1')
            record('immediate-status-matches', any(e['kind'] == 'readback_match' and e['error'] == 0 for e in events))
            record('immediate-status-is-on', any(e['kind'] == 'read' and e['reference'] == ref + '.stVal' and e['value'] == '1' for e in events))
            request(session, 'browse-same-association', 'browse', 'CCI016_01LD_Plant')
            print('Idling 20 seconds with no MMS requests...', flush=True)
            time.sleep(20)
            events = request(session, 'read-after-idle', 'read', ref + '.stVal')
            record('mode-stays-on-past-fallback', any(e['kind'] == 'read' and e['value'] == '1' for e in events))
            request(session, 'bad-read', 'read', ref + '.DoesNotExist', expected=1)
            request(session, 'read-after-error', 'read', ref + '.stVal')
            cancel = threading.Timer(.5, lambda: session.process.send_signal(signal.SIGUSR1))
            cancel.start()
            try: request(session, 'cancel-watch', 'watch', ref + '.stVal', expected=130, count=10)
            finally: cancel.cancel(); cancel.join()
            request(session, 'read-after-cancel', 'read', ref + '.stVal')
            before = log_path.read_text().count('mms: client CONNECT count=')
            record('one-connection-for-all-actions', before == 1)
        print('Closed session; waiting 17 seconds for the configured fallback...', flush=True)
        time.sleep(17)
        with client.PersistentSession(args, cert, key) as session:
            events = request(session, 'read-after-disconnect', 'read', ref + '.stVal')
            record('fallback-still-works-after-exit', any(e['kind'] == 'read' and e['value'] == '5' for e in events))
            server.terminate(); server.wait(timeout=12)
            time.sleep(.5)
            record('connection-loss-detected', session.status().startswith('DISCONNECTED'))
            try:
                request(session, 'no-resend', 'operate', ref, value='1')
            except OSError: record('lost-connection-does-not-resend', True)
            else: record('lost-connection-does-not-resend', False)
    finally:
        if server.poll() is None:
            server.terminate()
            try: server.wait(timeout=12)
            except subprocess.TimeoutExpired: server.kill(); server.wait()
        log.close()
        (opts.output / 'results.json').write_text(json.dumps(results, indent=2) + '\n')


if __name__ == '__main__': main()

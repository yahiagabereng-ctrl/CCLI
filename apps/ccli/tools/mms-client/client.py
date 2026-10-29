#!/usr/bin/env python3
"""Interactive TLS/MMS commissioning client. The server enforces your identity's permissions."""
import argparse
import copy
import datetime as dt
import getpass
import json
import math
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import queue
import signal
import threading
import time

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ec, padding, rsa
from tsp_profile import approve_record

ROOT = Path(__file__).resolve().parents[4]
PLANT = 'CCI016_01LD_Plant'
COMMANDS = ['menu', 'identity', 'check', 'browse', 'datasets', 'read', 'watch', 'write', 'operate',
            'control-info', 'select-cancel', 'dataset-members', 'dataset-read', 'dataset-create',
            'dataset-delete', 'report-blocks', 'rcb-read', 'reports', 'reports-gi']
CHANGES = {'write', 'operate', 'select-cancel', 'dataset-create', 'dataset-delete', 'reports', 'reports-gi'}
TYPES = ['bool', 'int32', 'int64', 'uint32', 'float', 'double', 'string']
FCS = ['ST', 'MX', 'SP', 'SV', 'CF', 'DC', 'SG', 'SE', 'SR', 'OR', 'BL', 'EX', 'CO']


def tlv(tag, content):
    size = len(content)
    n = (size.bit_length() + 7) // 8
    length = bytes([size]) if size < 128 else bytes([128 + n]) + size.to_bytes(n, 'big')
    return bytes([tag]) + length + content


def der_fields(data):
    result, pos = [], 0
    while pos < len(data):
        tag = data[pos]; pos += 1
        if pos >= len(data): raise ValueError('truncated DER')
        size = data[pos]; pos += 1
        if size & 128:
            n = size & 127
            if not n or pos + n > len(data): raise ValueError('invalid DER length')
            size = int.from_bytes(data[pos:pos+n], 'big'); pos += n
        if pos + size > len(data): raise ValueError('truncated DER value')
        result.append((tag, data[pos:pos+size])); pos += size
    return result


def identity(cert):
    print(f'Application identity: {cert.subject.rfc4514_string()}')
    names = {-1: 'DSO_OPERATOR', -2: 'AGGREGATOR_OPERATOR', 0: 'VIEWER', 1: 'OPERATOR',
             2: 'ENGINEER', 3: 'INSTALLER', 4: 'SECADM', 5: 'SECAUD', 6: 'RBACMNT'}
    try:
        ext = cert.extensions.get_extension_for_oid(x509.ObjectIdentifier('1.0.62351.8.1'))
        outer = der_fields(ext.value.value)
        if len(outer) != 1 or outer[0][0] != 0x30: raise ValueError('role sequence missing')
        for tag, record in der_fields(outer[0][1]):
            if tag != 0x30: raise ValueError('role record missing')
            fields = der_fields(record)
            ids = [int.from_bytes(v, 'big', signed=True) for t, v in der_fields(fields[0][1]) if t == 2]
            print('Role: ' + ', '.join(names.get(i, f'ID {i}') for i in ids))
            print('Area of responsibility: ' + fields[1][1].decode())
    except (x509.ExtensionNotFound, AttributeError, ValueError, IndexError) as exc:
        print(f'Role token unavailable: {exc}')
    print('Available tools: browse/read, datasets, reports, configuration writes, operate.')
    print('Actual access depends on the server policy, object scope and current control state.')


def validate(args):
    if args.fc not in FCS: raise ValueError('invalid functional constraint')
    if not 1 <= args.port <= 65535 or not 1 <= args.count <= 3600:
        raise ValueError('port must be 1..65535 and count 1..3600')
    if args.command not in ('menu', 'identity', 'check', 'browse') and not args.reference:
        raise ValueError('this command requires an object or logical-node reference')
    if args.command in ('write', 'operate', 'select-cancel'):
        if args.value is None: raise ValueError('provide --value and --type')
        if args.type not in TYPES: raise ValueError('unsupported value type')
        if args.type == 'bool' and args.value not in ('0', '1', 'true', 'false'):
            raise ValueError('boolean value must be true, false, 0 or 1')
        if args.type in ('int32', 'int64', 'uint32'):
            if not re.fullmatch(r'[+-]?\d+', args.value): raise ValueError('invalid integer')
            n = int(args.value)
            lo, hi = {'int32': (-2**31, 2**31-1), 'int64': (-2**63, 2**63-1), 'uint32': (0, 2**32-1)}[args.type]
            if not lo <= n <= hi: raise ValueError(f'value outside {args.type} range')
        if args.type in ('float', 'double'):
            n = float(args.value)
            if not math.isfinite(n) or (args.type == 'float' and abs(n) > 3.402823466e38):
                raise ValueError('non-finite/out-of-range float')
        if args.command == 'write' and args.fc in ('MX', 'ST', 'CO'):
            raise ValueError('write uses configuration/setpoint attributes (e.g. CF/SP); use operate for controls')
    if args.command == 'dataset-create':
        if args.reference.startswith('@'): raise ValueError('association-specific datasets would disappear on disconnect; use LD/LN.name')
        if not args.members: raise ValueError('provide --members with FC-qualified references such as LD/LN.TotW[MX]')
        if any(not re.fullmatch(r'[^;\s]+\[[A-Z]{2}\]', r) for r in args.members):
            raise ValueError('each dataset member must end in [FC] and contain no spaces/semicolon')


def authentication_proof(cert, key):
    stamp = dt.datetime.now(dt.timezone.utc).strftime('%Y%m%d%H%M%S.%fZ').encode()
    if isinstance(key, rsa.RSAPrivateKey):
        signature = key.sign(stamp, padding.PKCS1v15(), hashes.SHA256())
    elif isinstance(key, ec.EllipticCurvePrivateKey) and key.curve.key_size >= 256:
        signature = key.sign(stamp, ec.ECDSA(hashes.SHA256()))
    else:
        raise ValueError('application identity key must be RSA or ECDSA P-256 or stronger')
    payload = tlv(0x80, cert.public_bytes(serialization.Encoding.DER)) + tlv(0x81, stamp) + tlv(0x82, signature)
    return tlv(0xa2, tlv(2, b'\x05') + tlv(0xa0, tlv(0xa0, payload)))


def authentication_file(args, cert, key, directory):
    if getattr(args, 'auth_mode', 'certificate') == 'tls':
        peer = x509.load_pem_x509_certificate(Path(args.tls_cert).read_bytes())
        if peer.fingerprint(hashes.SHA256()) != cert.fingerprint(hashes.SHA256()):
            raise ValueError('TLS identity mode requires the identity and TLS certificates to match')
        return '-'
    path = Path(directory) / 'proof.der'
    path.write_bytes(authentication_proof(cert, key))
    return str(path)


def show_record(record):
    status = 'OK' if record['error'] == 0 else f"ERROR {record['error']}"
    explanation = ' ' + record.get('error_text', '') if record['error'] else ''
    print(f"{status:10} {record['kind']:14} {record['reference']} {record['value']}{explanation}", flush=True)


class PersistentSession:
    """One native connection for the entire menu. Never retries a control/write."""
    def __init__(self, args, cert, key):
        self.cert, self.key = cert, key
        self.process = self.reader = self.temp = self.errors = None
        self.connected = False
        self.reconnect(args)

    def __enter__(self): return self

    def __exit__(self, *_): self.close()

    @staticmethod
    def _pump(process, inbox):
        try:
            for line in process.stdout:
                try: inbox.put(json.loads(line))
                except json.JSONDecodeError: pass
        finally: inbox.put(None)

    def _receive(self, deadline):
        while time.monotonic() < deadline:
            try: record = self.inbox.get(timeout=min(1, max(.01, deadline-time.monotonic())))
            except queue.Empty: continue
            if record is None:
                self.connected = False
                raise OSError('MMS session ended. Choose 12 to reconnect; no operation will be resent.')
            record['observed_utc'] = dt.datetime.now(dt.timezone.utc).isoformat()
            if record['kind'] == 'session_state' and record['value'] == 'disconnected': self.connected = False
            return record
        raise OSError('Timed out waiting for the MMS client; verify server state before retrying a change.')

    def _send(self, fields):
        chunks = []
        for field in fields:
            encoded_size = len(field.encode('utf-8'))
            if encoded_size > 65536: raise ValueError('command field exceeds 65536 bytes')
            chunks.append(f'{encoded_size}\n{field}\n')
        self.process.stdin.write(''.join(chunks))
        self.process.stdin.flush()

    def reconnect(self, args):
        self.close()
        self.peer_certificate = None
        self.args = copy.copy(args)
        self.inbox = queue.Queue()
        self.temp = tempfile.TemporaryDirectory(prefix='ccli-mms-session-')
        self.errors = tempfile.TemporaryFile(mode='w+t')
        try:
            proof_path = authentication_file(args, self.cert, self.key, self.temp.name)
            command = [str(args.binary), args.host, str(args.port), str(args.ca), str(args.tls_cert), str(args.tls_key),
                       str(proof_path), args.ap_title, str(args.ae_qualifier), 'session', '', 'MX', '1', 'int32', '']
            self.process = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                stderr=self.errors, text=True, encoding='utf-8', start_new_session=True)
            self.reader = threading.Thread(target=self._pump, args=(self.process, self.inbox), daemon=True)
            self.reader.start()
            deadline = time.monotonic() + 20
            self.wire_profile = 'Legacy client (no profile marker)'
            while True:
                record = self._receive(deadline)
                if record['kind'] == 'wire_profile':
                    self.wire_profile = record['value']
                if record['kind'] == 'server_auth':
                    self.peer_certificate = approve_record(record, args, self.process.stdin)
                show_record(record)
                if record['kind'] == 'session_ready':
                    self.connected = record['error'] == 0
                    if not self.connected: raise OSError('MMS association failed; use --verbose for diagnostics.')
                    break
        except BaseException:
            self.close()
            raise

    def status(self):
        if not self.process or self.process.poll() is not None: self.connected = False
        while True:
            try: record = self.inbox.get_nowait()
            except queue.Empty: break
            if record is None or (record['kind'] == 'session_state' and record['value'] == 'disconnected'):
                self.connected = False
        return 'CONNECTED' if self.connected else 'DISCONNECTED - choose 12 to reconnect'

    def _collect(self, deadline, events, quiet):
        while True:
            record = self._receive(deadline)
            if record['kind'] == 'complete': return record['error']
            events.append(record)
            if not quiet: show_record(record)

    def execute(self, args, quiet=False):
        self.status()
        if not self.connected:
            raise OSError('MMS connection lost. Choose 12 to reconnect. No operation was sent or resent.')
        value = ';'.join(args.members) if args.command == 'dataset-create' else (args.value or '')
        events = []
        self._send([args.command, args.reference, args.fc, str(args.count), args.type, value])
        try:
            result = self._collect(time.monotonic() + max(60, args.count * 6 + 30), events, quiet)
        except KeyboardInterrupt:
            print('\nCancelling current request; checking completion before returning to the menu.')
            self.process.send_signal(signal.SIGUSR1)
            try: self._collect(time.monotonic() + 20, events, quiet)
            except OSError:
                self.close()
                raise
            result = 130
        except OSError:
            # Losing completion makes the state uncertain; never reuse or retry it.
            self.close()
            raise
        return result, events

    def close(self):
        self.connected = False
        process = self.process
        if process is not None:
            if process.poll() is None:
                try:
                    self._send(['quit', '', 'MX', '1', 'int32', ''])
                    process.stdin.close()
                except (OSError, ValueError): pass
                try: process.wait(timeout=10)
                except subprocess.TimeoutExpired:
                    process.kill(); process.wait()
            if self.reader: self.reader.join(timeout=2)
            if process.stdout: process.stdout.close()
            if process.stdin and not process.stdin.closed: process.stdin.close()
        self.process = self.reader = None
        if self.errors:
            if getattr(self, 'args', None) and self.args.verbose:
                self.errors.seek(0); print(self.errors.read(), file=sys.stderr)
            self.errors.close(); self.errors = None
        if self.temp: self.temp.cleanup(); self.temp = None


def finish_operation(args, result, events, quiet=False, persistent=False):
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps({'host': args.host, 'port': args.port, 'command': args.command,
            'reference': args.reference, 'connection_mode': 'persistent' if persistent else 'single-command',
            'passed': result == 0, 'events': events}, indent=2) + '\n')
    if not quiet: print('PASS: requested operation completed' if result == 0 else 'FAILED: check the error above; use --verbose for TLS diagnostics')
    return result, events


def run_operation(args, cert, key, quiet=False, session=None):
    validate(args)
    if args.command in CHANGES and not args.yes:
        detail = args.value if args.value is not None else ';'.join(args.members or [])
        print(f'\nTarget: {args.host}:{args.port}\nAction: {args.command}\nObject: {args.reference}\nValue: {detail or "(service operation)"}')
        if not sys.stdin.isatty():
            raise ValueError('state-changing command requires confirmation; pass --yes only for the intended operation')
        if input('Type APPLY to send, or Enter to cancel: ').strip() != 'APPLY':
            print('Cancelled.'); return 130, []
    if session is not None:
        result, events = session.execute(args, quiet=quiet)
        return finish_operation(args, result, events, quiet=quiet, persistent=True)
    events = []
    with tempfile.TemporaryDirectory(prefix='ccli-mms-') as temp, tempfile.TemporaryFile(mode='w+t') as errors:
        auth_path = authentication_file(args, cert, key, temp)
        value = ';'.join(args.members) if args.command == 'dataset-create' else (args.value or '')
        command = [str(args.binary), args.host, str(args.port), str(args.ca), str(args.tls_cert), str(args.tls_key),
                   str(auth_path), args.ap_title, str(args.ae_qualifier), args.command, args.reference, args.fc,
                   str(args.count), args.type, value]
        process = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=errors, text=True)
        try:
            for line in process.stdout:
                try: record = json.loads(line)
                except json.JSONDecodeError:
                    if not quiet: print(line.rstrip())
                    continue
                record['observed_utc'] = dt.datetime.now(dt.timezone.utc).isoformat()
                if record['kind'] == 'server_auth':
                    approve_record(record, args, process.stdin)
                events.append(record)
                if not quiet:
                    status = 'OK' if record['error'] == 0 else f"ERROR {record['error']}"
                    explanation = ' ' + record.get('error_text', '') if record['error'] else ''
                    print(f"{status:10} {record['kind']:14} {record['reference']} {record['value']}{explanation}", flush=True)
            result = process.wait()
            errors.seek(0)
            if args.verbose: print(errors.read(), file=sys.stderr)
        except KeyboardInterrupt:
            process.terminate()
            try: process.wait(timeout=10)
            except subprocess.TimeoutExpired: process.kill(); process.wait()
            result = 130
        finally:
            if process.poll() is None:
                process.terminate()
                try: process.wait(timeout=5)
                except subprocess.TimeoutExpired: process.kill(); process.wait()
    return finish_operation(args, result, events, quiet=quiet)


def ask(label, default=''):
    value = input(f'{label}' + (f' [{default}]' if default else '') + ': ').strip()
    return value or default


def choose_reference(label, items):
    for i, (name, ref) in enumerate(items, 1): print(f'  {i}. {name}  ({ref})')
    answer = ask(label + ' - number or full reference', '1')
    if answer.isdigit():
        if not 1 <= int(answer) <= len(items): raise ValueError('number outside list')
        return items[int(answer)-1][1]
    return answer


def interactive(args, cert, key):
    with PersistentSession(args, cert, key) as session:
        return interactive_menu(args, cert, key, session)


def interactive_menu(args, cert, key, session):
    def action(command, reference='', quiet=False, **options):
        request = copy.copy(args)
        request.command, request.reference = command, reference
        request.value, request.members, request.yes = None, None, False
        for k, v in options.items(): setattr(request, k, v)
        try: return run_operation(request, cert, key, quiet=quiet, session=session)
        except (OSError, ValueError) as exc: print(f'ERROR: {exc}'); return 2, []

    def browse_tree():
        ref, history = '', []
        while True:
            code, records = action('browse', ref, quiet=True)
            entries = [r['value'] for r in records if r['kind'] == 'entry']
            print(f'\nModel: {ref or "logical devices"}')
            if code:
                for r in records:
                    if r['error']: print(f"ERROR {r['error']}: {r['value']}")
            for i, name in enumerate(entries, 1): print(f'  {i:3}  {name}')
            print('Number = open | r = read | w = write | o = operate | c = control info | b = back | q = menu')
            choice = input('Browse> ').strip().lower()
            if choice == 'q': return
            if choice == 'b':
                if history: ref = history.pop()
                else: return
            elif choice.isdigit() and 1 <= int(choice) <= len(entries):
                raw = entries[int(choice)-1]
                match = re.search(r'\[([A-Z]{2})\]$', raw)
                if match: args.fc = match.group(1)
                name = re.sub(r'\s*\[[A-Z]{2}\]$', '', raw)
                history.append(ref)
                ref = name if not ref else ref + ('/' if '/' not in ref else '.') + name
            elif choice == 'r': action('read', ref, fc=ask('Functional constraint', args.fc))
            elif choice == 'w':
                action('write', ref, fc=ask('Functional constraint', 'CF'), type=ask('Type (' + '/'.join(TYPES) + ')', 'float'), value=ask('New value'))
            elif choice == 'c': action('control-info', ref)
            elif choice == 'o':
                code, records = action('control-info', ref)
                if code: continue
                default_type = next((r['value'] for r in records if r['kind'] == 'control_type'), 'int32')
                action('operate', ref, type=ask('Type (' + '/'.join(TYPES) + ')', default_type), value=ask('Control value'))
            else: print('Choose a listed number or action.')

    while True:
        print(f'\nCCLI MMS CLIENT  |  {args.host}:{args.port}  |  {session.status()}')
        print('1 Browse model       2 Quick check       3 Read value        4 Watch value')
        print('5 Write attribute    6 Operate control   7 Datasets          8 Reports')
        print('9 Identity/access    10 Change endpoint  11 Save reports     12 Reconnect    0 Exit')
        try:
            choice = input('Choose> ').strip()
            if choice in ('0', 'q', 'quit', 'exit'): return 0
            if choice == '1': browse_tree()
            elif choice == '2': action('check')
            elif choice in ('3', '4'):
                ref = choose_reference('Read object', [(name, PLANT + '/PdCMMXU1.' + suffix) for name, suffix in
                    [('Active power', 'TotW.mag.f'), ('Reactive power', 'TotVAr.mag.f'), ('Voltage', 'PPV.phsAB.cVal.mag.f'), ('Quality', 'TotW.q'), ('Timestamp', 'TotW.t')]])
                fc = ask('Functional constraint', 'MX')
                action('watch' if choice == '4' else 'read', ref, fc=fc, count=int(ask('Readings', '10')) if choice == '4' else 1)
            elif choice == '5':
                print('Write the attribute reference (e.g. .../WlimDWMX1.WMaxSptPct.ctlModel), not the control object.')
                ref = choose_reference('Attribute', [(label, PLANT + '/' + obj) for label, obj in
                    [('Power-limit control model', 'WlimDWMX1.WMaxSptPct.ctlModel'),
                     ('Power-limit mode control model', 'WlimDWMX1.Mod.ctlModel'),
                     ('Active-power target control model', 'WSdDAGC1.WSptPct.ctlModel')]])
                fc = ask('Functional constraint', 'CF')
                action('write', ref, fc=fc, type=ask('Type (' + '/'.join(TYPES) + ')', 'int32'), value=ask('New value'))
            elif choice == '6':
                ref = choose_reference('Control object', [(name, PLANT + '/' + obj) for name, obj in
                    [('Power limit mode', 'WlimDWMX1.Mod'), ('Power limit percent', 'WlimDWMX1.WMaxSptPct'),
                     ('DSO active-power mode', 'WSdDAGC1.Mod'), ('DSO active-power percent', 'WSdDAGC1.WSptPct'),
                     ('Reactive-power mode', 'VArSdDVAR1.Mod'), ('Reactive-power target', 'VArSdDVAR1.VArTgtSptPct'),
                     ('Power-factor mode', 'PFSPDFPF1.Mod'),
                     ('Aggregator active-power mode', 'WSaDAGC1.Mod'),
                     ('Aggregator export percent', 'WSaDAGC1.WSptPct'),
                     ('Aggregator reactive-power mode', 'VArSaDVAR1.Mod'),
                     ('Aggregator reactive-power percent', 'VArSaDVAR1.VArTgtSptPct')]])
                status, records = action('control-info', ref)
                if status: continue
                default_type = next((r['value'] for r in records if r['kind'] == 'control_type'), 'int32')
                mode = ask('i = info only, o = operate, s = select then cancel', 'i')
                if mode in ('o', 's'):
                    action('operate' if mode == 'o' else 'select-cancel', ref,
                           type=ask('Type (' + '/'.join(TYPES) + ')', default_type), value=ask('Control value'))
            elif choice == '7':
                mode = ask('l = list, m = members, r = read, c = create, d = delete', 'l')
                commands = {'l': 'datasets', 'm': 'dataset-members', 'r': 'dataset-read', 'c': 'dataset-create', 'd': 'dataset-delete'}
                if mode not in commands: continue
                default = PLANT + '/LLN0' if mode == 'l' else (PLANT + '/LLN0.DS_R_PdC_Mis4sec' if mode in ('m', 'r') else '')
                if mode in ('c', 'd'): print('CCI Profile A currently disables dynamic dataset create/delete; the server can reject these services.')
                ref = ask('Logical node' if mode == 'l' else 'Dataset reference', default)
                action(commands[mode], ref, members=ask('Members separated by ; (include [FC])').split(';') if mode == 'c' else None)
            elif choice == '8':
                mode = ask('l = list, r = read block, s = subscribe, g = subscribe + GI', 'l')
                if mode == 'l': action('report-blocks', ask('Logical node', PLANT + '/LLN0'))
                elif mode in ('r', 's', 'g'):
                    ref = ask('RCB reference', PLANT + '/LLN0.RP.urcb_PdC_Mis4sec')
                    action({'r': 'rcb-read', 's': 'reports', 'g': 'reports-gi'}[mode], ref,
                           count=int(ask('Listen seconds', '10')) if mode != 'r' else 1)
            elif choice == '9': identity(cert)
            elif choice == '10':
                host, port = ask('Host', args.host), int(ask('Port', str(args.port)))
                if not 1 <= port <= 65535: raise ValueError('invalid port')
                args.host, args.port = host, port
                session.reconnect(args)
            elif choice == '11':
                value = ask('JSON report path; Enter disables reports')
                args.report = Path(value) if value else None
                print('Each action replaces that report file.' if value else 'Report saving off.')
            elif choice == '12': session.reconnect(args)
            else: print('Choose 0..12.')
        except EOFError: return 0
        except KeyboardInterrupt: print('\nReturned to menu; operation may already have reached the server.')
        except ValueError as exc: print(f'ERROR: {exc}')
        except OSError as exc: print(f'ERROR: {exc}')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=COMMANDS, nargs='?', default='menu')
    parser.add_argument('reference', nargs='?', default='')
    parser.add_argument('--host', default='192.168.0.1')
    parser.add_argument('--port', type=int, default=3782)
    parser.add_argument('--fc', choices=['ST', 'MX', 'SP', 'SV', 'CF', 'DC', 'SG', 'SE', 'SR', 'OR', 'BL', 'EX', 'CO'], default='MX')
    parser.add_argument('--count', type=int, default=10, help='Watch readings or report listening seconds (1..3600)')
    parser.add_argument('--type', choices=TYPES, default='int32')
    parser.add_argument('--value', help='Typed write/operate value')
    parser.add_argument('--members', nargs='+', help='Dataset members ending in [FC]')
    parser.add_argument('--yes', action='store_true', help='Send the explicitly requested change without the interactive APPLY prompt')
    parser.add_argument('--ca', type=Path, default=ROOT / 'apps/ccli/config/tls/root_CA.pem')
    parser.add_argument('--mms-ca', type=Path, help='CA for the signed MMS server AARE; defaults to --ca')
    parser.add_argument('--tls-cert', type=Path, default=ROOT / 'apps/ccli/config/tls/client_tls.pem')
    parser.add_argument('--tls-key', type=Path, default=ROOT / 'apps/ccli/config/tls/client_tsp.key')
    parser.add_argument('--identity-cert', type=Path, required=True)
    parser.add_argument('--identity-key', type=Path, required=True)
    parser.add_argument('--auth-mode', choices=['certificate', 'tls'], default='certificate',
                        help='Signed ACSE application certificate (default), or the commissioned TLS peer identity')
    parser.add_argument('--ap-title', default='1.1.1.999.1')
    parser.add_argument('--ae-qualifier', type=int, default=12)
    parser.add_argument('--binary', type=Path, default=ROOT / 'apps/ccli/build-mms-client/ccli-mms-client')
    parser.add_argument('--report', type=Path)
    parser.add_argument('--verbose', action='store_true')
    args = parser.parse_args()
    if args.mms_ca and not args.mms_ca.is_file():
        parser.error('MMS server CA file does not exist')
    validate(args)
    for path in (args.ca, args.tls_cert, args.tls_key, args.identity_cert, args.identity_key, args.binary):
        if not path.is_file(): parser.error(f'Missing file: {path}')
    cert = x509.load_pem_x509_certificate(args.identity_cert.read_bytes())
    key_data = args.identity_key.read_bytes()
    try: key = serialization.load_pem_private_key(key_data, password=None)
    except TypeError:
        key = serialization.load_pem_private_key(key_data, password=getpass.getpass('Application identity key passphrase: ').encode())
    if key.public_key().public_bytes(serialization.Encoding.DER, serialization.PublicFormat.SubjectPublicKeyInfo) != cert.public_key().public_bytes(serialization.Encoding.DER, serialization.PublicFormat.SubjectPublicKeyInfo):
        parser.error('Application identity certificate and key do not match')
    if args.command == 'identity': identity(cert); return 0
    if args.command == 'menu': identity(cert); return interactive(args, cert, key)
    return run_operation(args, cert, key)[0]


if __name__ == '__main__':
    try: sys.exit(main())
    except (OSError, ValueError) as exc:
        print(f'ERROR: {exc}', file=sys.stderr); sys.exit(2)

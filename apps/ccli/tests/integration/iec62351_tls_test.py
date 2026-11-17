#!/usr/bin/env python3
"""Loopback interoperability/negative tests of the actual generated TLS HAL."""
import argparse
import atexit
import datetime as dt
import os
from pathlib import Path
import shutil
import signal
import socket
import ssl
import subprocess
import tempfile
import time
from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa, ec
from cryptography.x509.oid import NameOID, ExtendedKeyUsageOID, ObjectIdentifier


def hello_without_signature_algorithms(cipher):
    """Build a TLS 1.2 ClientHello, then omit extension 13 on the wire."""
    incoming, outgoing = ssl.MemoryBIO(), ssl.MemoryBIO()
    ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    ctx.minimum_version = ctx.maximum_version = ssl.TLSVersion.TLSv1_2
    ctx.set_ciphers(cipher)
    client = ctx.wrap_bio(incoming, outgoing, server_hostname='test')
    try:
        client.do_handshake()
    except ssl.SSLWantReadError:
        pass
    record = outgoing.read()
    assert record[0] == 22 and record[5] == 1
    assert len(record) == 5 + int.from_bytes(record[3:5], 'big')
    body = record[9:]
    offset = 34
    offset += 1 + body[offset]  # session ID
    offset += 2 + int.from_bytes(body[offset:offset+2], 'big')  # cipher suites
    offset += 1 + body[offset]  # compression methods
    extensions = body[offset+2:]
    retained = bytearray()
    removed = False
    while extensions:
        kind = int.from_bytes(extensions[:2], 'big')
        size = int.from_bytes(extensions[2:4], 'big')
        assert len(extensions) >= 4 + size
        if kind == 13:
            removed = True
        else:
            retained += extensions[:4+size]
        extensions = extensions[4+size:]
    assert removed
    body = body[:offset] + len(retained).to_bytes(2, 'big') + retained
    hello = b'\x01' + len(body).to_bytes(3, 'big') + body
    return record[:3] + len(hello).to_bytes(2, 'big') + hello


def first_server_flight(host, port, hello):
    """Collect handshake types or a fatal alert, without completing TLS."""
    messages, pending = [], bytearray()
    with socket.create_connection((host, port), timeout=5) as raw:
        raw.sendall(hello)
        def read_exact(length):
            result = b''
            while len(result) < length:
                chunk = raw.recv(length - len(result))
                if not chunk:
                    raise AssertionError('TLS server closed before an alert or ServerHelloDone')
                result += chunk
            return result
        while True:
            header = read_exact(5)
            payload = read_exact(int.from_bytes(header[3:5], 'big'))
            if header[0] == 21:
                return messages, payload
            assert header[0] == 22, header.hex()
            pending += payload
            while len(pending) >= 4:
                length = int.from_bytes(pending[1:4], 'big')
                if len(pending) < 4 + length:
                    break
                kind = pending[0]
                messages.append(kind)
                del pending[:4+length]
                if kind == 14:
                    return messages, None


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--server', required=True)
    parser.add_argument('--iec104', action='store_true')
    parser.add_argument('--remote', help='Explicit Tespro test address; temporary probe, port 3793')
    parser.add_argument('--ssh-hostkey', help='Required pinned SSH host key for remote tests')
    parser.add_argument('--event-log', type=Path)
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='ccli-tls-policy-') as tmp:
        root = Path(tmp)
        now = dt.datetime.now(dt.timezone.utc)
        ca_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
        ca_name = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, 'ephemeral test CA')])
        ca = (x509.CertificateBuilder().subject_name(ca_name).issuer_name(ca_name)
              .public_key(ca_key.public_key()).serial_number(1)
              .not_valid_before(now-dt.timedelta(days=2)).not_valid_after(now+dt.timedelta(days=2))
              .add_extension(x509.BasicConstraints(ca=True, path_length=None), True)
              .add_extension(x509.KeyUsage(True, False, False, False, False, True, True, False, False), True)
              .sign(ca_key, hashes.SHA256()))
        (root/'ca.pem').write_bytes(ca.public_bytes(serialization.Encoding.PEM))
        (root/'ca.key').write_bytes(ca_key.private_bytes(serialization.Encoding.PEM,
            serialization.PrivateFormat.TraditionalOpenSSL, serialization.NoEncryption()))
        trust_anchors = [ca.public_bytes(serialization.Encoding.PEM)]
        extra_root = extra_key = None
        for index in range(2, 6):
            extra_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
            extra_name = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME,
                f'additional test root {index}')])
            extra = (x509.CertificateBuilder().subject_name(extra_name).issuer_name(extra_name)
                .public_key(extra_key.public_key()).serial_number(index)
                .not_valid_before(now-dt.timedelta(days=2)).not_valid_after(now+dt.timedelta(days=2))
                .add_extension(x509.BasicConstraints(ca=True, path_length=None), True)
                .add_extension(x509.KeyUsage(True, False, False, False, False, True, True, False, False), True)
                .sign(extra_key, hashes.SHA256()))
            trust_anchors.append(extra.public_bytes(serialization.Encoding.PEM))
            if index == 5:
                extra_root, extra_root_key = extra, extra_key
        (root/'ca-five.pem').write_bytes(b''.join(trust_anchors))

        fifth_client_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
        fifth_client_name = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, 'client rooted at CA 5')])
        fifth_client_cert = (x509.CertificateBuilder().subject_name(fifth_client_name)
            .issuer_name(extra_root.subject).public_key(fifth_client_key.public_key())
            .serial_number(x509.random_serial_number())
            .not_valid_before(now-dt.timedelta(days=2)).not_valid_after(now+dt.timedelta(days=1))
            .add_extension(x509.BasicConstraints(ca=False, path_length=None), True)
            .add_extension(x509.KeyUsage(True, False, True, False, False, False, False, False, False), True)
            .add_extension(x509.ExtendedKeyUsage([ExtendedKeyUsageOID.CLIENT_AUTH]), False)
            .sign(extra_root_key, hashes.SHA256()))
        (root/'client_five_root.pem').write_bytes(fifth_client_cert.public_bytes(serialization.Encoding.PEM))
        (root/'client_five_root.key').write_bytes(fifth_client_key.private_bytes(
            serialization.Encoding.PEM, serialization.PrivateFormat.PKCS8, serialization.NoEncryption()))
        (root/'client_five_root.key').chmod(0o600)

        def cert(name, server=False, expired=False, ecc=False, weak=False, wrong_eku=False,
                 der_size=None, rsa_bits=2048, signature_hash=hashes.SHA256()):
            key = ec.generate_private_key(ec.SECP256R1()) if ecc else rsa.generate_private_key(
                public_exponent=65537, key_size=1024 if weak else rsa_bits)
            subject = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, name)])
            builder = (x509.CertificateBuilder().subject_name(subject).issuer_name(ca_name)
                .public_key(key.public_key()).serial_number(x509.random_serial_number())
                .not_valid_before(now-dt.timedelta(days=2))
                .not_valid_after(now-dt.timedelta(days=1) if expired else now+dt.timedelta(days=1))
                .add_extension(x509.BasicConstraints(ca=False, path_length=None), True)
                .add_extension(x509.KeyUsage(True, False, True, False, False, False, False, False, False), True)
                .add_extension(x509.ExtendedKeyUsage([ExtendedKeyUsageOID.SERVER_AUTH if server != wrong_eku else ExtendedKeyUsageOID.CLIENT_AUTH]), False))
            def sign(padding):
                current = builder
                if padding:
                    current = current.add_extension(x509.UnrecognizedExtension(
                        ObjectIdentifier('1.3.6.1.4.1.55555.62351.3'), b'\x5a' * padding), False)
                return current.sign(ca_key, signature_hash)
            if isinstance(signature_hash, hashes.SHA1):
                csr = (x509.CertificateSigningRequestBuilder().subject_name(subject)
                    .add_extension(x509.BasicConstraints(ca=False, path_length=None), True)
                    .add_extension(x509.KeyUsage(True, False, True, False, False, False, False, False, False), True)
                    .add_extension(x509.ExtendedKeyUsage([ExtendedKeyUsageOID.CLIENT_AUTH]), False)
                    .sign(key, hashes.SHA256()))
                csr_path = root/(name+'.csr.pem')
                cert_path = root/(name+'.sha1.pem')
                ext_path = root/(name+'.ext')
                csr_path.write_bytes(csr.public_bytes(serialization.Encoding.PEM))
                ext_path.write_text('basicConstraints=critical,CA:FALSE\n'
                    'keyUsage=critical,digitalSignature,keyEncipherment\n'
                    'extendedKeyUsage=clientAuth\n')
                subprocess.run(['openssl', 'x509', '-req', '-in', str(csr_path),
                    '-CA', str(root/'ca.pem'), '-CAkey', str(root/'ca.key'), '-set_serial', '987654321',
                    '-days', '2', '-sha1', '-extfile', str(ext_path), '-out', str(cert_path)],
                    check=True, capture_output=True)
                certificate = x509.load_pem_x509_certificate(cert_path.read_bytes())
            else:
                certificate = sign(0)
            if der_size is not None:
                padding = max(1, der_size - len(certificate.public_bytes(serialization.Encoding.DER)))
                for _ in range(12):
                    certificate = sign(padding)
                    actual_size = len(certificate.public_bytes(serialization.Encoding.DER))
                    if actual_size == der_size:
                        break
                    padding += der_size - actual_size
                if len(certificate.public_bytes(serialization.Encoding.DER)) != der_size:
                    raise AssertionError(f'could not create {der_size}-octet DER certificate')
            (root/(name+'.pem')).write_bytes(certificate.public_bytes(serialization.Encoding.PEM))
            (root/(name+'.key')).write_bytes(key.private_bytes(serialization.Encoding.PEM,
                serialization.PrivateFormat.PKCS8, serialization.NoEncryption()))
            (root/(name+'.key')).chmod(0o600)
            return certificate

        server_cert = cert('server', server=True); cert('ec_server', server=True, ecc=True)
        client = cert('client'); cert('other'); cert('expired', expired=True)
        cert('expired_server', server=True, expired=True); cert('wrong_eku', wrong_eku=True)
        cert('weak_server', server=True, weak=True); cert('weak_client', weak=True)
        cert('server_3072', server=True, rsa_bits=3072)
        cert('sha1_client', signature_hash=hashes.SHA1())
        cert('sha384_client', signature_hash=hashes.SHA384())
        cert('server_8192', server=True, der_size=8192)
        cert('client_8192', der_size=8192)
        cert('server_8193', server=True, der_size=8193)
        cert('client_8193', der_size=8193)

        def crl(path, revoke=False, stale=False, bad=False, future=False, expires=None):
            builder = (x509.CertificateRevocationListBuilder().issuer_name(ca_name)
                .last_update(now+dt.timedelta(hours=1) if future else now-dt.timedelta(minutes=1))
                .next_update(expires if expires is not None else
                    now-dt.timedelta(seconds=1) if stale else now+dt.timedelta(days=1)))
            if revoke:
                builder = builder.add_revoked_certificate(x509.RevokedCertificateBuilder()
                    .serial_number(server_cert.serial_number if revoke == 'server' else client.serial_number)
                    .revocation_date(now-dt.timedelta(minutes=2)).build())
            signing = rsa.generate_private_key(public_exponent=65537, key_size=2048) if bad else ca_key
            (root/path).write_bytes(builder.sign(signing, hashes.SHA256()).public_bytes(serialization.Encoding.PEM))
        crl('base.crl'); crl('revoked.crl', revoke=True); crl('stale.crl', stale=True)
        crl('bad.crl', bad=True); crl('future.crl', future=True)
        crl('stale-bad.crl', stale=True, bad=True)
        crl('stale-local-revoked.crl', stale=True, revoke='server')
        fifth_crl = (x509.CertificateRevocationListBuilder().issuer_name(extra_root.subject)
            .last_update(now-dt.timedelta(minutes=1)).next_update(now+dt.timedelta(days=1))
            .sign(extra_root_key, hashes.SHA256()).public_bytes(serialization.Encoding.PEM))
        (root/'base-five.crl').write_bytes((root/'base.crl').read_bytes()+fifth_crl)

        logs = []
        if args.event_log:
            atexit.register(lambda: args.event_log.write_text('\n\n'.join(logs)))
        remote = None
        if args.remote:
            from tls_remote import Remote
            remote = Remote(args.remote, args.ssh_hostkey, args.server, root)
        def start(identity='server', crl_name='base.crl', pin='-', key=None, timer=43200,
                  extra_pin=None, hash_policy=None, ca_name='ca.pem'):
            if remote:
                if crl_name != '-': remote.replace(root/crl_name)
                command = ['3793', remote.path((key or identity)+'.key'), remote.path(identity+'.pem'),
                    remote.path(ca_name), remote.path(crl_name) if crl_name != '-' else '-',
                    remote.path(pin+'.pem') if pin != '-' else '-',
                    str(timer)]
                if args.iec104:
                    command.append('iec104')
                    if extra_pin:
                        command.append(remote.path(extra_pin+'.pem'))
                if hash_policy:
                    command.append(hash_policy)
                p = remote.start(command)
                port = 3793
            else:
                p, port = start_local(identity, crl_name, pin, key, timer, extra_pin, hash_policy, ca_name)
            before = ''
            while True:
                line = p.stdout.readline(); before += line
                if line.strip() == 'READY' or not line: break
            return p, port, before

        def start_local(identity, crl_name, pin, key, timer, extra_pin, hash_policy, ca_name):
            with socket.socket() as s:
                s.bind(('127.0.0.1', 0)); port = s.getsockname()[1]
            command = [args.server, str(port), str(root/((key or identity)+'.key')),
                str(root/(identity+'.pem')), str(root/ca_name),
                str(root/crl_name) if crl_name != '-' else '-',
                str(root/(pin+'.pem')) if pin != '-' else '-', str(timer)]
            if args.iec104:
                command.append('iec104')
                if extra_pin:
                    command.append(str(root/(extra_pin+'.pem')))
            if hash_policy:
                command.append(hash_policy)
            p = subprocess.Popen(command,
                stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)
            return p, port

        def finish(p, before):
            if p.poll() is None: p.terminate()
            rest, _ = p.communicate(timeout=5)
            logs.append(before+rest)
            return before+rest

        def connect(port, identity='client', cipher='ECDHE-RSA-AES128-GCM-SHA256', allow_weak=False):
            ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
            ctx.minimum_version = ctx.maximum_version = ssl.TLSVersion.TLSv1_2
            ctx.check_hostname = False; ctx.load_verify_locations(str(root/'ca.pem'))
            ctx.set_ciphers(cipher + (':@SECLEVEL=0' if allow_weak else ''))
            if identity: ctx.load_cert_chain(str(root/(identity+'.pem')), str(root/(identity+'.key')))
            raw = socket.create_connection((args.remote or '127.0.0.1', port), timeout=5)
            return ctx.wrap_socket(raw, server_hostname='test')

        for cipher, identity in [('AES128-SHA256','server'), ('DHE-RSA-AES128-GCM-SHA256','server'),
                ('ECDHE-RSA-AES128-GCM-SHA256','server'), ('ECDHE-ECDSA-AES128-GCM-SHA256','ec_server')]:
            p, port, before = start(identity)
            try:
                with connect(port, cipher=cipher) as s:
                    s.sendall(b'hello'); assert s.recv(5) == b'hello', cipher
                    assert s.version() == 'TLSv1.2'
            finally: log = finish(p, before)
            assert 'EVENT 0' in log, log
            print('PASS required suite', cipher)

        p, port, before = start(crl_name='base-five.crl', pin='-', ca_name='ca-five.pem')
        try:
            with connect(port, 'client_five_root') as s:
                s.sendall(b'roots'); assert s.recv(5) == b'roots'
                assert s.version() == 'TLSv1.2'
        finally: log = finish(p, before)
        assert 'EVENT 0' in log, log
        print('PASS trust bundle with five root CA certificates')

        for cipher, identity in [('AES128-SHA256', 'server'),
                ('DHE-RSA-AES128-GCM-SHA256', 'server'),
                ('ECDHE-RSA-AES128-GCM-SHA256', 'server'),
                ('ECDHE-ECDSA-AES128-GCM-SHA256', 'ec_server')]:
            p, port, before = start(identity)
            try:
                messages, alert = first_server_flight(args.remote or '127.0.0.1', port,
                    hello_without_signature_algorithms(cipher))
                assert messages == [] and alert == b'\x02\x28', (cipher, messages, alert)
                p.wait(timeout=5)
            finally:
                log = finish(p, before)
            assert 'warning: deprecated hash algorithm used' in log, log
            assert 'alarm: signature_algorithms extension unavailable; SHA-1 fallback prohibited' in log, log
            assert 'EVENT 0' not in log, log
            print('PASS missing signature_algorithms rejected before server flight', cipher)

        p, port, before = start('server_3072')
        try:
            with connect(port) as s:
                s.sendall(b'key'); assert s.recv(3) == b'key'
        finally: log = finish(p, before)
        assert 'EVENT 0' in log, log
        print('PASS RSA 3072 endpoint certificate')

        p, port, before = start('server_8192', pin='client_8192')
        try:
            with connect(port, 'client_8192') as s:
                s.sendall(b'size'); assert s.recv(4) == b'size'
        finally: log = finish(p, before)
        assert 'EVENT 0' in log, log
        print('PASS peer and endpoint certificates at 8192 DER octets')

        if args.iec104:
            p, port, before = start(pin='client', extra_pin='other')
            try:
                with connect(port, 'other') as s:
                    s.sendall(b'list'); assert s.recv(4) == b'list'
            finally: log = finish(p, before)
            assert 'EVENT 0' in log, log
            print('PASS repeatable IEC 104 individual-certificate allowlist')

        p, port, before = start(pin='client_8193')
        accepted = False
        try:
            with connect(port, 'client_8193') as s:
                s.sendall(b'x'); accepted = s.recv(1) == b'x'
        except (ssl.SSLError, OSError): pass
        finally: log = finish(p, before)
        assert not accepted and 'alarm: TLS certificate size exceeded' in log, log
        print('PASS terminated peer certificate at 8193 DER octets')

        p, _, before = start('server_8193')
        log = finish(p, before)
        assert 'REJECTED' in log and 'alarm: TLS certificate size exceeded' in log, log
        print('PASS rejected endpoint certificate at 8193 DER octets')

        for peer, crl_name, pin in [(None,'base.crl','-'), ('expired','base.crl','expired'),
                ('client','revoked.crl','client'), ('wrong_eku','base.crl','-'), ('other','base.crl','client')]:
            p, port, before = start(crl_name=crl_name, pin=pin)
            accepted = False
            try:
                with connect(port, peer) as s:
                    s.sendall(b'x'); accepted = s.recv(1) == b'x'
            except (ssl.SSLError, OSError): pass
            finally: log = finish(p, before)
            assert not accepted, (peer, crl_name, pin)
            assert 'EVENT 2' in log, log
            if peer is None:
                assert 'alarm: peer certificate unavailable' in log, log
            if peer == 'other':
                assert 'alarm: certificate validation: trusted individual certificate from authorized CA not available' in log, log
            if peer == 'client' and crl_name == 'revoked.crl':
                assert 'alarm: TLS session terminated because the peer certificate was revoked' in log, log
            print('PASS rejected peer', peer, crl_name, pin)

        p, port, before = start(pin='weak_client')
        accepted = False
        try:
            with connect(port, 'weak_client', allow_weak=True) as s:
                s.sendall(b'x'); accepted = s.recv(1) == b'x'
        except (ssl.SSLError, OSError): pass
        finally: log = finish(p, before)
        assert not accepted and 'alarm: insufficient RSA key length (<2048 bit)' in log, log
        print('PASS rejected peer RSA 1024 key with IEC alarm')

        p, port, before = start(pin='sha1_client')
        accepted = False
        try:
            with connect(port, 'sha1_client', allow_weak=True) as s:
                s.sendall(b'x'); accepted = s.recv(1) == b'x'
        except (ssl.SSLError, OSError): pass
        finally: log = finish(p, before)
        assert not accepted and 'warning: deprecated hash algorithm used' in log, log
        print('PASS SHA-1 certificate rejection with warning event')

        if not args.remote:
            p, port, before = start(pin='sha384_client')
            accepted = False
            try:
                with connect(port, 'sha384_client') as s:
                    s.sendall(b'x'); accepted = s.recv(1) == b'x'
            except (ssl.SSLError, OSError): pass
            finally: log = finish(p, before)
            assert not accepted, log
            print('PASS optional SHA-384 certificate signature disabled by default')

            p, port, before = start(pin='sha384_client', hash_policy='sha384')
            try:
                with connect(port, 'sha384_client') as s:
                    s.sendall(b'hash'); assert s.recv(4) == b'hash'
            finally: log = finish(p, before)
            assert 'EVENT 0' in log, log
            print('PASS optional SHA-384 certificate signature enabled by policy')

        p, port, before = start()
        try:
            result = subprocess.run(['openssl', 's_client', '-connect', f'{args.remote or "127.0.0.1"}:{port}',
                '-tls1_2', '-cipher', 'AES128-SHA256', '-no_etm', '-cert', str(root/'client.pem'),
                '-key', str(root/'client.key'), '-CAfile', str(root/'ca.pem')],
                input=b'x', capture_output=True, timeout=10)
            p.wait(timeout=5)
        finally: log = finish(p, before)
        assert 'CBC negotiated without encrypt-then-MAC' in log, log
        print('PASS rejected CBC without encrypt-then-MAC')

        if hasattr(signal, 'SIGUSR1'):
            p, port, before = start(timer=600)
            try:
                with connect(port) as s:
                    s.sendall(b'a'); assert s.recv(1) == b'a'
                    p.send_signal(signal.SIGUSR1)
                    time.sleep(0.15)
                    # Leave the peer idle without processing the HelloRequest.
                    p.send_signal(signal.SIGUSR2)
                    p.wait(timeout=5)
            finally: log = finish(p, before)
            assert 'Peer did not complete TLS renegotiation within 30 seconds' in log, log
            print('PASS idle renegotiation deadline (accelerated monotonic clock)')

        for identity, crl_name, key, timer in [('expired_server','base.crl',None,43200),
                ('weak_server','base.crl',None,43200), ('server','base.crl','other',43200),
                ('expired_server','stale.crl',None,43200),
                ('server','stale-local-revoked.crl',None,43200),
                ('server','stale-bad.crl',None,43200), ('server','bad.crl',None,43200),
                ('server','future.crl',None,43200), ('server','base.crl',None,599),
                ('server','base.crl',None,86401), ('server','-',None,43200)]:
            p, _, before = start(identity, crl_name, key=key, timer=timer)
            log = finish(p, before)
            assert 'REJECTED' in log and 'EVENT 2' in log, log
            if identity == 'weak_server':
                assert 'alarm: insufficient RSA key length (<2048 bit)' in log, log
            if crl_name == '-':
                assert 'TLS requires a configured CRL' in log, log
            print('PASS rejected local configuration', identity, crl_name, timer)

        p, port, before = start(crl_name='stale.crl')
        accepted = False
        try:
            assert 'READY' in before and 'warning: CRL expired' in before, before
            with connect(port) as s:
                s.sendall(b'x'); accepted = s.recv(1) == b'x'
        except (ssl.SSLError, OSError): pass
        finally: log = finish(p, before)
        assert not accepted and 'REJECTED' not in log, log
        print('PASS expired CRL warns without blocking listener; new handshake remains strict')

        shutil.copyfile(root/'stale.crl', root/'recover.crl')
        p, port, before = start(crl_name='recover.crl')
        try:
            assert 'READY' in before and 'warning: CRL expired' in before, before
            shutil.copyfile(root/'base.crl', root/'recover.next')
            os.replace(root/'recover.next', root/'recover.crl')
            if remote: remote.replace(root/'recover.crl')
            with connect(port) as s:
                s.sendall(b'fresh'); assert s.recv(5) == b'fresh'
        finally: log = finish(p, before)
        assert 'EVENT 0' in log, log
        print('PASS fresh CRL restores authentication without restarting listener')

        crl('expiring.crl', expires=dt.datetime.now(dt.timezone.utc)+dt.timedelta(seconds=3))
        p, port, before = start(crl_name='expiring.crl')
        try:
            with connect(port) as s:
                s.sendall(b'a'); assert s.recv(1) == b'a'
                # Drive policy ticks across the whole-second X.509 expiry
                # boundary; this echo probe does not have a protocol idle loop.
                until = time.monotonic()+5
                while time.monotonic() < until:
                    time.sleep(.2)
                    s.sendall(b'b'); assert s.recv(1) == b'b'
                time.sleep(0.15)
        finally: log = finish(p, before)
        assert 'warning: CRL expired' in log, log
        print('PASS CRL expiry warns and preserves an established session')

        if args.iec104:
            for identity, key in [('missing_endpoint', 'server'), ('server', 'missing_endpoint')]:
                p, _, before = start(identity, key=key)
                log = finish(p, before)
                assert 'REJECTED' in log and 'alarm: endpoint certificate unavailable' in log, log
                print('PASS endpoint identity unavailable alarm', identity, key)

        for replacement in ['bad.crl','stale.crl','missing','revoked.crl']:
            shutil.copyfile(root/'base.crl', root/'active.crl')
            p, port, before = start(crl_name='active.crl')
            try:
                with connect(port) as s:
                    s.sendall(b'a'); assert s.recv(1) == b'a'
                    if replacement == 'missing': (root/'active.crl').unlink()
                    else:
                        shutil.copyfile(root/replacement, root/'next.crl')
                        os.replace(root/'next.crl', root/'active.crl')
                    if remote: remote.replace(root/'active.crl')
                    time.sleep(1.3)
                    live = False
                    try: s.sendall(b'b'); live = s.recv(1) == b'b'
                    except (ssl.SSLError, OSError): pass
                    assert live == (replacement != 'revoked.crl'), replacement
            finally: log = finish(p, before)
            assert ('EVENT 2' if replacement == 'revoked.crl' else 'EVENT 1 1001') in log, log
            if replacement == 'revoked.crl':
                assert 'alarm: TLS session terminated because the peer certificate was revoked' in log, log
            print('PASS active CRL replacement', replacement)
        print('PASS IEC 62351-3 TLS HAL campaign (OCSP not covered)')


if __name__ == '__main__': main()

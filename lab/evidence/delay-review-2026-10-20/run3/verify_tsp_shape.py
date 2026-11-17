"""Read-only network probe using TSP's captured framing and a fresh signed proof.

This is disposable server-validation evidence, not a change to our client app.
Only the authentication timestamp/signature and dependent lengths are refreshed.
"""
import argparse
import datetime as dt
import importlib.util
import json
from pathlib import Path
import socket
import ssl
import time

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding

BASE = Path(__file__).resolve().parent
ROOT = BASE.parents[3]
spec = importlib.util.spec_from_file_location('ber', ROOT / 'apps/ccli/tests/unit/mms_presentation_context_test.py')
ber = importlib.util.module_from_spec(spec)
spec.loader.exec_module(ber)


def walk(nodes):
    for tag, value in nodes:
        yield tag, value
        if isinstance(value, list):
            yield from walk(value)


def proof_node(nodes):
    return next(value for tag, value in walk(nodes) if tag == 0xa0 and isinstance(value, list)
                and [child[0] for child in value] == [0x80, 0x81, 0x82])


def slen(length):
    return bytes([length]) if length < 255 else b'\xff' + length.to_bytes(2, 'big')


def read_slen(data, pos):
    length = data[pos]
    return (int.from_bytes(data[pos + 1:pos + 3], 'big'), pos + 3) if length == 255 else (length, pos + 1)


def session_payload(data):
    size, pos = read_slen(data, 1)
    assert size == len(data) - pos
    while pos < len(data):
        tag = data[pos]
        size, start = read_slen(data, pos + 1)
        if tag in (0xc1, 0xc2):
            assert start + size == len(data)
            return data[start:]
        pos = start + size
    raise AssertionError('No presentation payload')


def rebuild_session(presentation):
    original = (BASE / 'captured-session.bin').read_bytes()
    _, pos = read_slen(original, 1)
    start_parameters = pos
    while original[pos] not in (0xc1, 0xc2):
        size, start = read_slen(original, pos + 1)
        pos = start + size
    contents = original[start_parameters:pos] + b'\xc2' + slen(len(presentation)) + presentation
    return original[:1] + slen(len(contents)) + contents


def receive_exact(conn, size):
    data = b''
    while len(data) < size:
        chunk = conn.recv(size - len(data))
        if not chunk:
            raise RuntimeError('Server closed before a complete response')
        data += chunk
    return data


def receive_tpkt(conn):
    header = receive_exact(conn, 4)
    assert header[:2] == b'\x03\x00'
    return header + receive_exact(conn, int.from_bytes(header[2:], 'big') - 4)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', type=int, default=3783)
    parser.add_argument('--label', default='isolated')
    parser.add_argument('--captured-stale-proof', action='store_true')
    args = parser.parse_args()
    tree = ber.decode((BASE / 'captured-presentation.bin').read_bytes())
    proof = proof_node(tree)
    cert = x509.load_der_x509_certificate(proof[0][1])
    config = ROOT / 'apps/ccli/config'
    certificate_paths = list((config / 'tls').glob('*.pem')) + list((config / 'EJBCA RBAC').rglob('*.pem'))
    matches = []
    for path in certificate_paths:
        try:
            candidate = x509.load_pem_x509_certificate(path.read_bytes())
            if candidate.fingerprint(hashes.SHA256()) == cert.fingerprint(hashes.SHA256()):
                matches.append(str(path.relative_to(ROOT)))
        except ValueError:
            pass
    assert matches, 'Captured identity must match a known local certificate'
    if not args.captured_stale_proof:
        expected = cert.public_key().public_bytes(serialization.Encoding.DER, serialization.PublicFormat.SubjectPublicKeyInfo)
        candidates = [config / 'tls/client_tsp.key', config / 'EJBCA RBAC/dual-certificate/dso-mms.key']
        candidates += list((config / 'EJBCA RBAC/TSP DSO Viewer Credentials').rglob('*.key'))
        for path in candidates:
            if not path.is_file():
                continue
            try:
                key = serialization.load_pem_private_key(path.read_bytes(), password=None)
            except (ValueError, TypeError):
                continue
            if key.public_key().public_bytes(serialization.Encoding.DER, serialization.PublicFormat.SubjectPublicKeyInfo) == expected:
                break
        else:
            raise AssertionError('No matching existing identity key')
        stamp = dt.datetime.now(dt.timezone.utc).strftime('%Y%m%d%H%M%S.').encode()
        proof[1] = (0x81, stamp)
        proof[2] = (0x82, key.sign(stamp, padding.PKCS1v15(), hashes.SHA256()))
    else:
        stamp = proof[1][1]
    presentation = ber.encode(tree)
    session = rebuild_session(presentation)
    assert len(session) == 1586 and len(presentation) == 1558, 'Preserve exact TSP shape'
    ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    ctx.check_hostname = False
    ctx.load_verify_locations(str(config / 'tls/root_CA.pem'))
    ctx.minimum_version = ctx.maximum_version = ssl.TLSVersion.TLSv1_2
    ctx.set_ciphers('AES128-SHA256')
    ctx.load_cert_chain(str(config / 'tls/client_tls.pem'), str(config / 'tls/client_tsp.key'))
    row = {'port': args.port, 'identity_serial': format(cert.serial_number, 'X'),
           'matching_public_certificates': matches, 'signed_time': stamp.decode(),
           'captured_stale_proof': args.captured_stale_proof, 'session_bytes': len(session),
           'presentation_bytes': len(presentation), 'fragments': [1021, 565]}
    with socket.create_connection(('192.168.0.1', args.port), timeout=5) as raw:
        with ctx.wrap_socket(raw, server_hostname='192.168.0.1') as conn:
            start = time.perf_counter()
            conn.sendall(bytes.fromhex('0300001611e00000000400c0010ac2020001c1020001'))
            assert receive_tpkt(conn) == bytes.fromhex('0300001611d00004000100c0010ac2020001c1020001')
            row['cotp_ms'] = (time.perf_counter() - start) * 1000
            start = time.perf_counter()
            for index, fragment in enumerate([session[:1021], session[1021:]]):
                packet = b'\x02\xf0' + bytes([0x80 if index else 0]) + fragment
                conn.sendall(b'\x03\x00' + (len(packet) + 4).to_bytes(2, 'big') + packet)
            response = b''
            while True:
                try:
                    packet = receive_tpkt(conn)
                except RuntimeError:
                    if not args.captured_stale_proof:
                        raise
                    row['closed_without_accepted_association'] = True
                    row['passed'] = True
                    (BASE / ('tsp-shape-' + args.label + '.json')).write_text(json.dumps(row, indent=2))
                    print(json.dumps(row, indent=2))
                    return
                assert packet[5] == 0xf0
                response += packet[7:]
                if packet[6] & 0x80:
                    break
            row['association_ms'] = (time.perf_counter() - start) * 1000
    response_tree = ber.decode(session_payload(response))
    aare = next(value for tag, value in walk(response_tree) if tag == 0x61 and isinstance(value, list)
                and any(child[0] == 0xa1 for child in value))
    result = next(value for tag, value in aare if tag == 0xa2)
    row['aare_result'] = int.from_bytes(result[0][1], 'big')
    row['mms_initiate_response'] = any(tag == 0xa9 for tag, value in walk(aare))
    if not args.captured_stale_proof:
        assert row['aare_result'] == 0 and row['mms_initiate_response'], row
        server_proof = proof_node(aare)
        server_cert = x509.load_der_x509_certificate(server_proof[0][1])
        server_cert.public_key().verify(server_proof[2][1], server_proof[1][1], padding.PKCS1v15(), hashes.SHA256())
        row['server_proof_signature_verified'] = True
    else:
        assert row['aare_result'] != 0, 'Stale proof must not be accepted'
    row['passed'] = True
    (BASE / ('tsp-shape-' + args.label + '-request.bin')).write_bytes(session)
    (BASE / ('tsp-shape-' + args.label + '-response.bin')).write_bytes(response)
    (BASE / ('tsp-shape-' + args.label + '.json')).write_text(json.dumps(row, indent=2))
    print(json.dumps(row, indent=2))


if __name__ == '__main__':
    main()

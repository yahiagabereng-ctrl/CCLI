#!/usr/bin/env python3
"""Observe a read-only client session through a local, mutually authenticated TLS relay.

The relay passes the actual OSI bytes unchanged to Tespro. No device configuration,
controls or server binaries are changed. All keys are existing local lab inputs.
"""
import argparse
import copy
import json
from pathlib import Path
import select
import socket
import ssl
import sys
import threading

from cryptography import x509
from cryptography.hazmat.primitives import serialization

HERE = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(HERE))
import client
from tsp_profile import fields


def tree(data):
    return [(tag, tree(value) if tag & 32 else value) for tag, value in fields(data)]


def presentation(session):
    def length(data, pos):
        size = data[pos]
        return (int.from_bytes(data[pos + 1:pos + 3], 'big'), pos + 3) if size == 255 else (size, pos + 1)
    size, pos = length(session, 1)
    assert session[0] in (13, 14) and size == len(session) - pos
    while pos < len(session):
        tag = session[pos]
        size, start = length(session, pos + 1)
        if tag == 0xc2:
            assert start + size == len(session)
            return session[start:]
        pos = start + size
    raise AssertionError('Missing extended session user data')


def packets(data):
    result = []
    while data:
        assert len(data) >= 4 and data[:2] == b'\x03\x00'
        size = int.from_bytes(data[2:4], 'big')
        assert 7 <= size <= len(data)
        result.append(data[:size])
        data = data[size:]
    return result


def association(frames):
    chunks = []
    for frame in frames:
        if frame[5] == 0xf0:
            chunks.append(frame[7:])
            if frame[6] & 128:
                return b''.join(chunks), [len(chunk) for chunk in chunks]
    raise AssertionError('No complete fragmented association')


def normalize(nodes):
    if [tag for tag, _ in nodes] == [0x80, 0x81, 0x82] and nodes[0][1][:1] == b'\x30':
        return [(0x80, nodes[0][1]), (0x81, b'<fresh time>'), (0x82, b'<fresh signature>')]
    return [(tag, normalize(value) if isinstance(value, list) else value) for tag, value in nodes]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--host', default='192.168.0.1')
    parser.add_argument('--port', type=int, default=3782)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--reference-session', type=Path)
    options = parser.parse_args()
    options.output.mkdir(parents=True, exist_ok=True)
    config = client.ROOT / 'apps/ccli/config'
    tls = config / 'tls'
    cert_path = config / 'EJBCA RBAC/dual-certificate/dso-mms.pem'
    key_path = cert_path.with_suffix('.key')
    front_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    front_ctx.minimum_version = front_ctx.maximum_version = ssl.TLSVersion.TLSv1_2
    front_ctx.set_ciphers('AES128-SHA256')
    front_ctx.load_cert_chain(str(tls / 'server_tls.pem'), str(tls / 'server.key'))
    front_ctx.load_verify_locations(str(tls / 'root_CA.pem'))
    front_ctx.verify_mode = ssl.CERT_REQUIRED
    back_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    back_ctx.check_hostname = False
    back_ctx.minimum_version = back_ctx.maximum_version = ssl.TLSVersion.TLSv1_2
    back_ctx.set_ciphers('AES128-SHA256')
    back_ctx.load_verify_locations(str(tls / 'root_CA.pem'))
    back_ctx.load_cert_chain(str(tls / 'client_tls.pem'), str(tls / 'client_tsp.key'))
    listener = socket.socket()
    listener.bind(('127.0.0.1', 0))
    listener.listen(1)
    listener.settimeout(20)
    incoming, outgoing, failures = bytearray(), bytearray(), []
    stop = threading.Event()

    def relay():
        try:
            raw, _ = listener.accept()
            with front_ctx.wrap_socket(raw, server_side=True) as front:
                with socket.create_connection((options.host, options.port), timeout=5) as remote:
                    with back_ctx.wrap_socket(remote, server_hostname=options.host) as back:
                        while not stop.is_set():
                            ready = [sock for sock in (front, back) if sock.pending()]
                            if not ready:
                                ready, _, _ = select.select([front, back], [], [], .2)
                            for source in ready:
                                data = source.recv(65536)
                                if not data:
                                    return
                                target, capture = (back, incoming) if source is front else (front, outgoing)
                                capture.extend(data)
                                target.sendall(data)
        except Exception as error:
            failures.append(repr(error))
        finally:
            listener.close()

    thread = threading.Thread(target=relay, daemon=True)
    thread.start()
    args = argparse.Namespace(host='127.0.0.1', port=listener.getsockname()[1],
        ca=tls / 'root_CA.pem', mms_ca=tls / 'root_CA.pem', tls_cert=tls / 'client_tls.pem', tls_key=tls / 'client_tsp.key',
        identity_cert=cert_path, identity_key=key_path, auth_mode='certificate', ap_title='1.1.1.999.1', ae_qualifier=12,
        binary=client.ROOT / 'apps/ccli/build-mms-client/ccli-mms-client', verbose=False,
        command='read', reference='', fc='MX', type='int32', count=1, value=None, members=None, yes=True, report=None)
    cert = x509.load_pem_x509_certificate(cert_path.read_bytes())
    key = serialization.load_pem_private_key(key_path.read_bytes(), password=None)
    operations = []
    try:
        with client.PersistentSession(args, cert, key) as session:
            for command, reference in [('read', 'CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f'),
                                       ('browse', 'CCI016_01LD_Plant'),
                                       ('control-info', 'CCI016_01LD_Plant/WlimDWMX1.Mod'),
                                       ('read', 'CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f')]:
                request = copy.copy(args)
                request.command, request.reference = command, reference
                code, events = session.execute(request, quiet=True)
                assert code == 0, (command, events)
                operations.append({'command': command, 'reference': reference, 'events': events})
            peer = session.peer_certificate
    finally:
        stop.set()
        thread.join(timeout=3)
        (options.output / 'client-tpkt.bin').write_bytes(incoming)
        (options.output / 'server-tpkt.bin').write_bytes(outgoing)
    assert not thread.is_alive() and not failures, failures
    client_frames, server_frames = packets(bytes(incoming)), packets(bytes(outgoing))
    assert client_frames[0] == bytes.fromhex('0300001611e00000000400c0010ac2020001c1020001')
    assert sum(frame[5] == 0xe0 for frame in client_frames) == 1, 'Actions must reuse one association'
    request, request_chunks = association(client_frames)
    response, response_chunks = association(server_frames)
    assert len(request_chunks) >= 2 and request_chunks[0] == 1021
    assert len(response_chunks) >= 2, 'Exercise fragmented AARE reception'
    cp = tree(presentation(request))
    normal = next(value for tag, value in cp[0][1] if tag == 0xa2)
    contexts = next(value for tag, value in normal if tag == 0xa4)
    context_ids = [int.from_bytes(value[0][1], 'big') for _, value in contexts]
    assert context_ids == [3, 1, 5]
    assert contexts[2][1][1] == (6, bytes.fromhex('2886480001010401'))
    captured_match = None
    if options.reference_session:
        captured_match = normalize(cp) == normalize(tree(presentation(options.reference_session.read_bytes())))
        assert captured_match, 'Generated request differs from TSP beyond fresh time/signature'
    result = {'passed': True, 'association_count': 1, 'operations': operations,
              'cotp_request_matches_tsp': True, 'context_ids': context_ids,
              'request_session_bytes': len(request), 'request_fragments': request_chunks,
              'response_fragments': response_chunks, 'server_certificate': peer,
              'captured_presentation_matches_except_fresh_time_and_signature': captured_match}
    (options.output / 'client-session.bin').write_bytes(request)
    (options.output / 'server-session.bin').write_bytes(response)
    (options.output / 'result.json').write_text(json.dumps(result, indent=2))
    print(json.dumps({key: value for key, value in result.items() if key != 'operations'}, indent=2))


if __name__ == '__main__':
    main()

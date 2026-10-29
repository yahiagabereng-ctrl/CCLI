import hashlib
import json
from pathlib import Path

base = Path(__file__).resolve().parent
chunks = []
for line in (base / 'tls-stream0.txt').read_text(encoding='utf-8-sig').splitlines():
    if line.startswith('\t'):
        continue
    value = line.strip()
    if value and all(c in '0123456789abcdef' for c in value):
        data = bytes.fromhex(value)
        if data[:2] == b'\x03\x00' and len(data) >= 7 and data[5] == 0xf0:
            chunks.append(data[7:])
session = b''.join(chunks)
assert len(session) == 1586, len(session)
(base / 'captured-session.bin').write_bytes(session)

def slen(data, pos):
    n = data[pos]
    if n == 255:
        return int.from_bytes(data[pos+1:pos+3], 'big'), pos+3
    return n, pos+1

length, pos = slen(session, 1)
while pos < len(session):
    tag = session[pos]
    n, start = slen(session, pos+1)
    if tag == 0xc2:
        presentation = session[start:start+n]
        break
    pos = start + n
else:
    raise ValueError('No session extended user data')
(base / 'captured-presentation.bin').write_bytes(presentation)

def oid(data):
    arcs = [data[0] // 40, data[0] % 40]
    value = 0
    for c in data[1:]:
        value = (value << 7) | (c & 127)
        if not c & 128:
            arcs.append(value)
            value = 0
    return '.'.join(map(str, arcs))

tsp_oid = bytes.fromhex('2886480001010401')
server_oid = bytes.fromhex('288648000001010401')
assert b'\x06' + bytes([len(tsp_oid)]) + tsp_oid in presentation
assert b'\x06' + bytes([len(server_oid)]) + server_oid not in presentation
summary = {
    'captured_session_length': len(session),
    'captured_presentation_length': len(presentation),
    'tsp_authentication_abstract_syntax': oid(tsp_oid),
    'server_generated_authentication_abstract_syntax': oid(server_oid),
    'tsp_abstract_syntax_hex': tsp_oid.hex(),
    'server_abstract_syntax_hex': server_oid.hex(),
    'pcap_sha256': hashlib.sha256((base/'TSP_P3_01_CONNECT_2026-10-20.pcapng').read_bytes()).hexdigest(),
}
(base / 'request-analysis.json').write_text(json.dumps(summary, indent=2))
print(json.dumps(summary, indent=2))

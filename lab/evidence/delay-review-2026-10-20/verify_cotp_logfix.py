"""Check the TSP COTP request against a two-second response budget; no MMS controls."""
import argparse
import datetime
import json
from pathlib import Path
import socket
import ssl
import time

BASE = Path(__file__).resolve().parent
ROOT = BASE.parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--port', type=int, default=3782)
parser.add_argument('--count', type=int, default=5)
parser.add_argument('--output', default='verified-cotp.json')
args = parser.parse_args()
ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
ctx.check_hostname = False
ctx.load_verify_locations(str(ROOT / 'apps/ccli/config/tls/root_CA.pem'))
ctx.minimum_version = ctx.maximum_version = ssl.TLSVersion.TLSv1_2
ctx.set_ciphers('AES128-SHA256')
ctx.load_cert_chain(str(ROOT / 'apps/ccli/config/tls/client_tls.pem'),
                    str(ROOT / 'apps/ccli/config/tls/client_tsp.key'))
results = []
for attempt in range(args.count):
    row = {'attempt': attempt + 1, 'port': args.port,
           'utc': datetime.datetime.now(datetime.timezone.utc).isoformat()}
    start = time.perf_counter()
    try:
        with socket.create_connection(('192.168.0.1', args.port), timeout=2) as raw:
            with ctx.wrap_socket(raw, server_hostname='192.168.0.1') as conn:
                row['tls_ms'] = (time.perf_counter() - start) * 1000
                request = bytes.fromhex('0300001611e00000000400c0010ac2020001c1020001')
                sent = time.perf_counter()
                conn.sendall(request)
                response = conn.recv(22)
                row['cotp_ms'] = (time.perf_counter() - sent) * 1000
                row['response'] = response.hex()
                row['passed'] = response == bytes.fromhex('0300001611d00004000100c0010ac2020001c1020001')
    except Exception as exc:
        row.update(passed=False, error=repr(exc))
    results.append(row)
    print(json.dumps(row), flush=True)
    time.sleep(0.3)
(BASE / args.output).write_text(json.dumps(results, indent=2), encoding='utf-8')
raise SystemExit(0 if all(row['passed'] for row in results) else 1)

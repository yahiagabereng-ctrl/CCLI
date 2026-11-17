"""Read-only transport probe and /proc observer. Sends no MMS controls."""
import datetime
import json
import os
from pathlib import Path
import socket
import ssl
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[3]
OUT = Path(__file__).resolve().parent
sample = r'''
pid=$(pidof ccli)
i=0
while [ "$i" -lt 12 ]; do
    read up rest < /proc/uptime
    printf 'SAMPLE %s ' "$up"
    for t in /proc/$pid/task/*; do
        printf '%s=' "${t##*/}"
        cat "$t/wchan"
        printf ' '
    done
    printf '\n'
    ls -l /proc/$pid/fd 2>/dev/null | grep 'events.jsonl'
    for t in /proc/$pid/task/*; do
        printf 'CPU %s ' "${t##*/}"
        awk '{print "utime=" $14, "stime=" $15}' "$t/stat"
    done
    wc -c /etc/ccli/security-events/events.jsonl
    i=$((i+1))
    sleep 1
done
tail -8 /etc/ccli/security-events/events.jsonl
'''
observe = subprocess.Popen([
    r'C:\Program Files\PuTTY\plink.exe', '-batch', '-ssh',
    '-hostkey', 'SHA256:fy0pZiLnyHHY6hKpcR7ukqBAb22qfNxU72Qpv/hseJM',
    '-pw', os.environ['TESPRO_SSH_PASSWORD'], 'root@192.168.0.1', sample,
], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
ctx.check_hostname = False
ctx.load_verify_locations(str(ROOT / 'apps/ccli/config/tls/root_CA.pem'))
ctx.minimum_version = ctx.maximum_version = ssl.TLSVersion.TLSv1_2
ctx.set_ciphers('AES128-SHA256')
ctx.load_cert_chain(str(ROOT / 'apps/ccli/config/tls/client_tls.pem'),
                    str(ROOT / 'apps/ccli/config/tls/client_tsp.key'))
time.sleep(0.6)
result = {'utc_start': datetime.datetime.now(datetime.timezone.utc).isoformat()}
t0 = time.perf_counter()
try:
    with socket.create_connection(('192.168.0.1', 3782), timeout=15) as raw:
        with ctx.wrap_socket(raw, server_hostname='192.168.0.1') as conn:
            result['tls_handshake_seconds'] = time.perf_counter() - t0
            result['cipher'] = conn.cipher()
            result['local_address'] = conn.getsockname()
            # Identical COTP CR to run2 stream 0; no ACSE or MMS request.
            request = bytes.fromhex('0300001611e00000000400c0010ac2020001c1020001')
            start = time.perf_counter()
            result['utc_cotp_send'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
            conn.sendall(request)
            result['response'] = conn.recv(4096).hex()
            result['cotp_reply_seconds'] = time.perf_counter() - start
            result['utc_cotp_receive'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
except Exception as exc:
    result['error'] = repr(exc)
finally:
    stdout, stderr = observe.communicate(timeout=30)
    suffix = sys.argv[1] if len(sys.argv) > 1 else ''
    (OUT / f'live-thread-observations{suffix}.txt').write_text(stdout, encoding='utf-8')
    if stderr:
        result['observer_stderr'] = stderr
    (OUT / f'live-probe{suffix}.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
    print(json.dumps(result, indent=2))

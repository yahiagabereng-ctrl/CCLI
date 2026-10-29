"""Extract reproducible TLS/COTP timings; never exports private key material."""
import csv
import hashlib
import json
from pathlib import Path
import subprocess

BASE = Path(__file__).resolve().parent
ROOT = BASE.parents[2]
TSHARK = r'C:\Program Files\Wireshark\tshark.exe'
FIELDS = ['frame.number', 'frame.time_relative', 'tcp.stream', 'ip.src',
          'tcp.srcport', 'tls.handshake.type', 'tls.record.content_type',
          'cotp.type', '_ws.col.Info']
result = []
for run in ['run1', 'run2']:
    capture = BASE / run / 'TSP_P3_01_CONNECT_2026-10-20.pcapng'
    args = [TSHARK, '-r', str(capture), '-o',
            'tls.keys_list:192.168.0.1,3782,tpkt,' +
            str(ROOT / 'apps/ccli/config/tls/server.key'),
            '-Y', 'tcp.port == 3782', '-T', 'fields']
    for field in FIELDS:
        args.extend(['-e', field])
    out = subprocess.run(args, check=True, capture_output=True, text=True).stdout
    rows = [dict(zip(FIELDS, row)) for row in csv.reader(out.splitlines(), delimiter='\t')]
    with (BASE / f'{run}-packet-timeline.csv').open('w', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=FIELDS)
        writer.writeheader()
        writer.writerows(rows)
    streams = {}
    for row in rows:
        stream = streams.setdefault(row['tcp.stream'], {'run': run, 'stream': int(row['tcp.stream'])})
        if row['cotp.type'] == '0x0e':
            stream['request_frame'] = int(row['frame.number'])
            stream['request_s'] = float(row['frame.time_relative'])
            stream['client_port'] = int(row['tcp.srcport'])
        if row['cotp.type'] == '0x0d':
            stream['confirm_frame'] = int(row['frame.number'])
            stream['confirm_s'] = float(row['frame.time_relative'])
        if 'Close Notify' in row['_ws.col.Info'] and row['ip.src'] != '192.168.0.1':
            stream['client_close_frame'] = int(row['frame.number'])
            stream['client_close_s'] = float(row['frame.time_relative'])
    for stream in streams.values():
        if 'request_s' in stream:
            stream['reply_delay_s'] = stream['confirm_s'] - stream['request_s']
            stream['client_close_after_request_s'] = stream['client_close_s'] - stream['request_s']
            result.append(stream)
    count_args = [TSHARK, '-r', str(capture), '-o', args[4],
                  '-Y', 'acse || mms', '-T', 'fields', '-e', 'frame.number']
    protocol_frames = subprocess.run(count_args, check=True, capture_output=True, text=True).stdout
    assert not protocol_frames.strip(), f'Unexpected ACSE/MMS traffic in {run}'
    print(run, 'sha256=' + hashlib.sha256(capture.read_bytes()).hexdigest(), 'ACSE/MMS frames=0')
(BASE / 'packet-timings.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
for s in result:
    print(f"{s['run']} stream {s['stream']} frames {s['request_frame']} -> {s['confirm_frame']}: "
          f"reply {s['reply_delay_s']:.6f}s; client close {s['client_close_after_request_s']:.6f}s")

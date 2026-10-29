import hashlib
import json
from pathlib import Path

base = Path(__file__).resolve().parent
root = base.parents[2]
rows = json.loads((base / 'live-retained-events-r62.json').read_text(encoding='utf-8-sig'))
fresh = [row for row in rows if row['ts_o14'] >= '2026/10/20 13:10:00']
audits = [json.loads(row['detail']) for row in fresh if row['detail'].startswith('{"rbac"')]
assert len(rows) == 2048
assert audits
timings = json.loads((base / 'live-cotp-after-verified-restart.json').read_text())
assert len(timings) == 10 and all(row['passed'] for row in timings)
mms = json.loads((base / 'mms-live-verified-restart-read.json').read_text())
assert mms['passed']
for line in (base / 'built-source-sha256.txt').read_text().splitlines():
    digest, path = line.split(maxsplit=1)
    assert hashlib.sha256((root / path).read_bytes()).hexdigest() == digest, path
summary = {
    'live_connections_passed': len(timings),
    'cotp_min_ms': min(row['cotp_ms'] for row in timings),
    'cotp_max_ms': max(row['cotp_ms'] for row in timings),
    'tls_min_ms': min(row['tls_ms'] for row in timings),
    'tls_max_ms': max(row['tls_ms'] for row in timings),
    'signed_mms_read_passed': mms['passed'],
    'retained_events': len(rows),
    'fresh_events': len(fresh),
    'fresh_rbac_records_roundtrip_valid': len(audits),
    'built_sources_match_current_workspace': True,
}
(base / 'validation-summary-r62.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')
print(json.dumps(summary, indent=2))

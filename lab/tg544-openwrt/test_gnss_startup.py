"""Exercise the real shell startup functions with fake modem responses."""
import os
from pathlib import Path
import subprocess
import tempfile

worker = Path(__file__).with_name('gnss-start-worker').read_text()
definitions = worker.rsplit('gnss_main "$@"', 1)[0]
with tempfile.TemporaryDirectory() as temp:
    directory = Path(temp)
    (directory / 'jsonfilter').write_text('''#!/usr/bin/env python3
import json,sys
a=sys.argv
try: print(str(json.loads(a[a.index('-s')+1])[a[a.index('-e')+1][2:]]).lower())
except Exception: sys.exit(1)
''')
    (directory / 'ubus').write_text('''#!/usr/bin/env python3
import os,sys,pathlib,json
p=pathlib.Path(os.environ['CASE_DIR'])
method=sys.argv[-1]
with (p/'calls').open('a') as f: f.write(method+'\\n')
if os.environ['SCENARIO']=='transport-fail': sys.exit(1)
if method=='gnss_start':
 print(json.dumps({'success':os.environ['SCENARIO']!='start-fail'}))
else:
 calls=(p/'calls').read_text().splitlines()
 on=os.environ['SCENARIO']=='already-on' or (os.environ['SCENARIO']=='start-ok' and 'gnss_start' in calls)
 print(json.dumps({'supported':True,'powered_on':on}))
''')
    for name in ('jsonfilter', 'ubus'):
        (directory / name).chmod(0o755)
    for scenario, ready, expected in [('not-ready', False, 1), ('already-on', True, 0),
            ('start-fail', True, 1), ('verify-off', True, 1),
            ('start-ok', True, 0), ('transport-fail', True, 1)]:
        calls = directory / 'calls'
        calls.unlink(missing_ok=True)
        script = definitions + '\nlogger() { :; }; sleep() { :; };\n'
        script += 'gnss_broker_ready() { return %d; }; gnss_ensure_on\n' % (0 if ready else 1)
        result = subprocess.run(['sh', '-c', script], env=dict(os.environ,
            PATH=temp+':'+os.environ['PATH'], CASE_DIR=temp, SCENARIO=scenario), timeout=5)
        assert result.returncode == expected, (scenario, result.returncode)
        methods=calls.read_text().splitlines() if calls.exists() else []
        if not ready: assert methods == []
        if scenario == 'already-on': assert methods == ['gnss_check']
        if scenario == 'start-ok': assert methods == ['gnss_check','gnss_start','gnss_check']
        print('PASS:',scenario)

# The init callback must register a worker, without issuing AT calls or sleeping.
init = Path(__file__).with_name('gnss.init').read_text()
script = init + '''
uci() { echo 1; }
logger() { :; }
ubus() { echo 'unexpected synchronous ubus' >&2; exit 91; }
sleep() { echo 'unexpected synchronous sleep' >&2; exit 92; }
procd_open_instance() { :; }
procd_set_param() { echo "$*"; }
procd_close_instance() { :; }
start_service
'''
result = subprocess.run(['sh','-c',script],capture_output=True,text=True,timeout=2)
assert result.returncode == 0, result.stderr
assert 'command /usr/sbin/gnss-start-worker' in result.stdout
print('PASS: boot callback returns without waiting or AT traffic')

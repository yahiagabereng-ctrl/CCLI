"""Stage/test/apply the reviewed fix. Credentials come only from the environment."""
import argparse
import os
from pathlib import Path
import subprocess

BASE = Path(__file__).resolve().parent
PUTTY = Path(r'C:\Program Files\PuTTY')
COMMON = ['-batch', '-hostkey', 'SHA256:fy0pZiLnyHHY6hKpcR7ukqBAb22qfNxU72Qpv/hseJM',
          '-pw', os.environ['TESPRO_SSH_PASSWORD']]
STAGE = '/tmp/ccli-logfix-r62-20261020'

def ssh(command=None, script=None, timeout=180):
    args = [str(PUTTY / 'plink.exe'), *COMMON, '-ssh']
    if script:
        args += ['-m', str(BASE / script)]
    args += ['root@192.168.0.1']
    if command:
        args += [command]
    result = subprocess.run(args, capture_output=True, text=True, timeout=timeout)
    if result.stderr:
        print(result.stderr, end='')
    if result.returncode:
        print(result.stdout, end='')
        raise RuntimeError(f'Remote operation failed: exit {result.returncode}')
    return result.stdout

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('action', choices=['stage', 'isolated', 'serve-isolated', 'isolated-status', 'stop-isolated', 'deploy', 'restart', 'status'])
    action = parser.parse_args().action
    if action == 'stage':
        print(ssh(f'test ! -e {STAGE} && umask 077 && mkdir {STAGE}'), end='')
        files = ['ccli-r62', 'mms-rbac-device-server-r62', 'event-store-rotation-test']
        args = [str(PUTTY / 'pscp.exe'), *COMMON, '-scp', *[str(BASE / name) for name in files],
                f'root@192.168.0.1:{STAGE}/']
        result = subprocess.run(args, capture_output=True, text=True, timeout=120)
        if result.returncode:
            print(result.stderr)
            raise RuntimeError('Staging upload failed')
        print('Three binaries staged in device /tmp; live CCI unchanged.')
    elif action == 'isolated':
        output = ssh(script='setup_isolated_logfix.sh')
        (BASE / 'isolated-setup-receipt.txt').write_text(output, encoding='utf-8')
        print(output, end='')
    elif action == 'serve-isolated':
        output = ssh(f'echo $$ > {STAGE}/isolated-server.pid; '
                     f'exec {STAGE}/mms-rbac-device-server-r62 '
                     '/etc/ccli/rbac-test-20261016/logfix-r62-20261020/server.yaml 300 '
                     f'> {STAGE}/isolated-server.log 2>&1', timeout=330)
        print(output, end='')
    elif action == 'isolated-status':
        output = ssh(f'wc -l /etc/ccli/rbac-test-20261016/logfix-r62-20261020/events.jsonl*; '
                     f'tail -10 {STAGE}/isolated-server.log; cat {STAGE}/rotation-test.log')
        (BASE / 'isolated-test-status.txt').write_text(output, encoding='utf-8')
        print(output, end='')
    elif action == 'stop-isolated':
        print(ssh(f'pid=$(cat {STAGE}/isolated-server.pid); '
                  f'if test -r /proc/$pid/cmdline && tr "\\000" " " < /proc/$pid/cmdline | grep "{STAGE}/mms-rbac-device-server-r62" >/dev/null; '
                  f'then kill -TERM "$pid"; fi'), end='')
    elif action == 'deploy':
        output = ssh(script='apply_logfix_r62.sh')
        (BASE / 'deployment-receipt-r62.txt').write_text(output, encoding='utf-8')
        print(output, end='')
    elif action == 'status':
        print(ssh('date; /usr/sbin/ccli --version; pidof ccli; '
                  'wc -l /etc/ccli/security-events/events.jsonl*; '
                  'logread -e ccli | tail -12'), end='')
    elif action == 'restart':
        output = ssh(script='restart_logfix_r62.sh')
        (BASE / 'verified-restart-receipt-r62.txt').write_text(output, encoding='utf-8')
        print(output, end='')

if __name__ == '__main__':
    main()

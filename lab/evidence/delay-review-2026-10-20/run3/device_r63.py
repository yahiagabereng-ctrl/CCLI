"""Staging and isolated tests; never print or store credentials in receipts."""
import argparse
import os
from pathlib import Path
import subprocess

BASE = Path(__file__).resolve().parent
PUTTY = Path(r'C:\Program Files\PuTTY')
COMMON = ['-batch', '-hostkey', 'SHA256:fy0pZiLnyHHY6hKpcR7ukqBAb22qfNxU72Qpv/hseJM',
          '-pw', os.environ['TESPRO_SSH_PASSWORD']]
STAGE = '/tmp/ccli-tsp-r63-20261020'
TESTROOT = '/etc/ccli/rbac-test-20261016/tsp-r63-20261020'


def ssh(command=None, script=None, timeout=180):
    args = [str(PUTTY / 'plink.exe'), *COMMON, '-ssh']
    if script:
        args += ['-m', str(BASE / script)]
    args += ['root@192.168.0.1']
    if command:
        args += [command]
    result = subprocess.run(args, capture_output=True, text=True, timeout=timeout)
    print(result.stdout, end='')
    if result.stderr:
        print(result.stderr, end='')
    if result.returncode:
        raise RuntimeError(f'Remote operation failed: exit {result.returncode}')
    return result.stdout


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('action', choices=['stage', 'setup', 'serve', 'status', 'stop', 'deploy'])
    action = parser.parse_args().action
    if action == 'stage':
        ssh(f'test ! -e {STAGE} && umask 077 && mkdir {STAGE}')
        files = ['ccli-r63', 'mms-rbac-device-server-r63']
        result = subprocess.run([str(PUTTY / 'pscp.exe'), *COMMON, '-scp',
                                 *[str(BASE / name) for name in files], f'root@192.168.0.1:{STAGE}/'],
                                capture_output=True, text=True, timeout=120)
        if result.returncode:
            raise RuntimeError('Staging upload failed: ' + result.stderr)
        print('r63 server binaries staged; live service unchanged.')
    elif action == 'setup':
        (BASE / 'r63-isolated-setup.txt').write_text(ssh(script='setup_isolated_r63.sh'))
    elif action == 'serve':
        ssh(f'echo $$ > {STAGE}/isolated.pid; exec {STAGE}/mms-rbac-device-server-r63 '
            f'{TESTROOT}/server.yaml 600 > {STAGE}/isolated.log 2>&1', timeout=630)
    elif action == 'status':
        (BASE / 'r63-isolated-status.txt').write_text(ssh(f'tail -35 {STAGE}/isolated.log; '
                                                         f'tail -15 {TESTROOT}/events.jsonl'))
    elif action == 'stop':
        ssh(f'pid=$(cat {STAGE}/isolated.pid); '
            f'if test -r /proc/$pid/cmdline && tr "\\000" " " < /proc/$pid/cmdline | '
            f'grep "{STAGE}/mms-rbac-device-server-r63" >/dev/null; then kill -TERM "$pid"; fi')
    elif action == 'deploy':
        (BASE / 'r63-deployment-receipt.txt').write_text(ssh(script='apply_r63.sh'))


if __name__ == '__main__':
    main()

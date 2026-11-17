"""Explicit Tespro loopback-campaign transport. No installed service is changed."""
import atexit
import io
import os
from pathlib import Path
import shlex
import signal
import subprocess
import tarfile
import uuid


class RemoteProcess:
    def __init__(self, remote, arguments):
        self.remote = remote
        command = 'echo REMOTE_PID $$; export CCLI_TLS_TEST_BIND=192.168.0.1; exec ' + shlex.join(arguments)
        self.process = subprocess.Popen(remote.command(command), stdout=subprocess.PIPE,
                                        stderr=subprocess.DEVNULL, text=True)
        line = self.process.stdout.readline()
        if not line.startswith('REMOTE_PID '):
            raise RuntimeError('Remote test process did not return its PID: ' + line)
        self.pid = int(line.split()[1])
        self.stdout = self.process.stdout
        remote.processes.append(self)

    def poll(self): return self.process.poll()
    def wait(self, timeout=None): return self.process.wait(timeout=timeout)
    def communicate(self, timeout=None): return self.process.communicate(timeout=timeout)

    def terminate(self):
        if self.poll() is None:
            self.remote.run(f'kill -TERM {self.pid} 2>/dev/null || true')

    def send_signal(self, number):
        names = {signal.SIGUSR1: 'USR1', signal.SIGUSR2: 'USR2'}
        self.remote.run(f'kill -{names[number]} {self.pid}')


class Remote:
    def __init__(self, host, hostkey, executable, fixtures):
        password = os.environ.get('CCLI_TLS_DUT_PASSWORD')
        if not password or not hostkey:
            raise RuntimeError('Remote tests require CCLI_TLS_DUT_PASSWORD and a pinned --ssh-hostkey')
        self.ssh = os.environ.get('CCLI_TLS_PLINK', '/mnt/c/Program Files/PuTTY/plink.exe')
        self.host, self.hostkey, self.password = host, hostkey, password
        self.root = '/tmp/ccli-tls-' + uuid.uuid4().hex
        self.processes = []
        self.run('umask 077; mkdir ' + shlex.quote(self.root))
        atexit.register(self.close)
        self.run('umask 077; cat > ' + shlex.quote(self.root+'/probe') +
                 '; chmod 700 ' + shlex.quote(self.root+'/probe'), Path(executable).read_bytes())
        stream = io.BytesIO()
        with tarfile.open(fileobj=stream, mode='w:gz') as archive:
            for file in fixtures.iterdir():
                if file.is_file(): archive.add(file, arcname=file.name)
        self.run('umask 077; tar -xz -C ' + shlex.quote(self.root), stream.getvalue())

    def command(self, command):
        return [self.ssh, '-batch', '-T', '-ssh', 'root@'+self.host,
                '-hostkey', self.hostkey, '-pw', self.password, command]

    def run(self, command, data=None):
        result = subprocess.run(self.command(command), input=data, capture_output=True, timeout=20)
        if result.returncode:
            raise RuntimeError('Remote test operation failed: ' + result.stderr.decode(errors='replace'))
        return result.stdout

    def path(self, file): return self.root+'/'+Path(file).name

    def replace(self, file):
        target = shlex.quote(self.path(file))
        if Path(file).exists():
            self.run('umask 077; cat > '+target+'.next && mv -f '+target+'.next '+target, Path(file).read_bytes())
        else: self.run('rm -f '+target)

    def start(self, arguments): return RemoteProcess(self, [self.root+'/probe']+arguments)

    def close(self):
        for process in self.processes:
            try: process.terminate()
            except Exception: pass
        # Generated UUID path only; never remove an existing operator directory.
        if self.root.startswith('/tmp/ccli-tls-') and len(self.root) == len('/tmp/ccli-tls-')+32:
            try: self.run('rm -rf -- '+shlex.quote(self.root))
            except Exception: pass

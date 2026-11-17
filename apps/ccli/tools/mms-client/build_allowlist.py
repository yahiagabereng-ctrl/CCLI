#!/usr/bin/env python3
"""Stage current merged sources without credentials and build host/target verification artifacts."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess

p = argparse.ArgumentParser()
p.add_argument('--workspace', type=Path, required=True)
p.add_argument('--sdk', type=Path, required=True)
p.add_argument('--host', action='store_true')
a = p.parse_args()
repo = Path(__file__).resolve().parents[4]
source = a.workspace / 'source'
if not source.exists():
    def ignore(directory, entries):
        ignored = set(shutil.ignore_patterns('.git', '__pycache__', '*.pem', '*.key', '*.p12', '*.pfx')(directory, entries))
        ignored.update(n for n in entries if n.startswith('build') and (Path(directory) / n).is_dir())
        if Path(directory).name == 'config': ignored.add('tls')
        return ignored
    shutil.copytree(repo / 'apps/ccli', source / 'apps/ccli', ignore=ignore)
    for name in ('scripts/generate_mms_certificate_auth.py', 'lab/conformance/CCI_RBAC_TLS_BASELINE.json'):
        target = source / name
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(repo / name, target)
# Repair only files previously omitted by an overly broad build-directory filter.
for item in (repo / 'apps/ccli').rglob('build*.h'):
    if any(part.startswith('build') for part in item.relative_to(repo / 'apps/ccli').parts[:-1]): continue
    target = source / item.relative_to(repo)
    if not target.exists():
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(item, target)
build = a.workspace / ('host-build' if a.host else 'target-build')
args = ['cmake', '-S', str(source / 'apps/ccli'), '-B', str(build), '-G', 'Ninja',
        '-DCMAKE_BUILD_TYPE=Release', '-DCCLI_FORCE_LIBGPIOD=OFF',
        '-DCCLI_BUILD_TESTS=' + ('ON' if a.host else 'OFF')]
env = dict(os.environ)
if not a.host:
    toolchain = next((a.sdk / 'staging_dir').glob('toolchain-aarch64*'))
    target = next((a.sdk / 'staging_dir').glob('target-aarch64*'))
    prefix = toolchain / 'bin/aarch64-openwrt-linux-musl-'
    tc = a.workspace / 'tespro-toolchain.cmake'
    tc.write_text(f'''set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER "{prefix}gcc")
set(CMAKE_CXX_COMPILER "{prefix}g++")
set(CMAKE_FIND_ROOT_PATH "{target}" "{toolchain}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
''')
    args += ['-DCMAKE_TOOLCHAIN_FILE=' + str(tc), '-DCCLI_BUILD_MMS_DEVICE_TEST=ON',
        '-DCCLI_FORCE_LIBMODBUS=ON', '-DCCLI_MODBUS_INCLUDE_DIR=' + str(target / 'usr/include'),
        '-DCCLI_MODBUS_LIBRARY=' + str(target / 'usr/lib/libmodbus.so')]
    env['STAGING_DIR'] = str(target)
subprocess.run(args, check=True, env=env)
targets = ['mms-rbac-dual-host-server', 'tls_allowlist_config_test'] if a.host else ['ccli', 'mms-rbac-device-server']
subprocess.run(['cmake', '--build', str(build), '--target', *targets, '-j', '8'], check=True, env=env)
if a.host:
    subprocess.run(['ctest', '--test-dir', str(build), '-R', 'tls_allowlist_repeated_entries', '--output-on-failure'], check=True)

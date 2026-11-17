"""Backend-private flags must not change classes embedded by services."""
import argparse
import pathlib
import subprocess
import tempfile

repo = pathlib.Path(__file__).resolve().parents[2]
app = repo/'apps/ccli'
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--header-ref', help='Optional Git revision to demonstrate a historical layout failure')
args = parser.parse_args()
with tempfile.TemporaryDirectory() as folder:
    root = pathlib.Path(folder)
    source = root/'layout.cpp'
    source.write_text('#include "svc_modbus.hpp"\n#include <cstdio>\n'
                      'int main(){ std::printf("%zu %zu\\n", '
                      'sizeof(cci::adapters::ModbusAdapter), sizeof(cci::services::ModbusService)); }\n')
    outputs = []
    if args.header_ref:
        (root/'modbus_adapter.hpp').write_bytes(subprocess.check_output(
            ['git', 'show', f'{args.header_ref}:apps/ccli/adapters/modbus_master/modbus_adapter.hpp'], cwd=repo))
    for backend in (False, True):
        binary = root/('backend' if backend else 'caller')
        command = ['g++', '-std=c++17', str(source), '-o', str(binary)]
        if args.header_ref:
            command += ['-I', str(root)]
        for include in ('services', 'core', 'adapters/modbus_master', 'platform/include'):
            command += ['-I', str(app/include)]
        if backend:
            command += ['-DCCLI_MODBUS_LIBMODBUS=1']
        subprocess.run(command, check=True)
        outputs.append(subprocess.check_output([str(binary)], text=True).strip())
    if outputs[0] != outputs[1]:
        raise AssertionError(f'Caller/backend class layout mismatch: {outputs}')
    print('PASS: caller and backend class sizes match:', outputs[0])

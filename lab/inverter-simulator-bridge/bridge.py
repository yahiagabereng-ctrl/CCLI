#!/usr/bin/env python3
"""Run the supplied Huawei simulator with an additional CCI float-register view."""
import argparse
import asyncio
import contextlib
import importlib.util
import io
import json
import math
from pathlib import Path
import struct
import sys
import tempfile

from pymodbus import FramerType
from pymodbus.constants import ExcCodes
from pymodbus.server import StartSerialServer, StartTcpServer

BASE = 60000


def signed32(hi, lo):
    value = (hi << 16) | lo
    return value - 2**32 if value & 2**31 else value


def float_words(value):
    return list(struct.unpack('>HH', struct.pack('>f', value)))


def mirror_values(registers):
    p = signed32(registers[32080], registers[32081]) / 1000
    q = signed32(registers[32082], registers[32083]) / 1000
    voltage = registers[32066] / 10
    return float_words(p) + float_words(q) + float_words(voltage), p, q, voltage


def load_model(source):
    spec = importlib.util.spec_from_file_location('sun2000_user_simulator', source)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def checked_state(path):
    data = json.loads(path.read_text(encoding='utf-8-sig'))
    mode = data.get('mode', 'fixed')
    if mode not in ('fixed', 'daytime', 'daycycle'): raise ValueError('invalid mode')
    values = {key: float(data.get(key, default)) for key, default in [('power_kw', 10), ('q_kvar', 1), ('voltage_v', 400)]}
    if not all(math.isfinite(v) for v in values.values()): raise ValueError('state contains a non-finite number')
    if not 0 <= values['power_kw'] <= 110 or abs(values['q_kvar']) > 66 or not 100 <= values['voltage_v'] <= 800:
        raise ValueError('state outside simulator range: P=0..110 kW, |Q|<=66 kvar, V=100..800')
    return mode, values


def build_device(module, state_path, accept_pq_writes=False):
    mode, values = checked_state(state_path)
    state = module.Sun2000State(values['voltage_v'], values['q_kvar'], mode, values['power_kw'], 7, 0x0200)
    device = module.build_device(state, 0x0200)
    monitoring = module.build_initial_registers(0x0200)
    original_action = device.action

    async def access(function_code, start_address, address, count, current_registers, new_registers):
        cci_overlap = address < BASE+6 and address+count > BASE
        if new_registers is not None and cci_overlap:
            if not accept_pq_writes or function_code != 16:
                return ExcCodes.ILLEGAL_ADDRESS

            p = q = None
            if address == BASE and count == 2 and len(new_registers) == 2:
                p = struct.unpack('>f', struct.pack('>HH', *new_registers))[0]
            elif address == BASE+2 and count == 2 and len(new_registers) == 2:
                q = struct.unpack('>f', struct.pack('>HH', *new_registers))[0]
            elif address == BASE and count == 4 and len(new_registers) == 4:
                p = struct.unpack('>f', struct.pack('>HH', *new_registers[:2]))[0]
                q = struct.unpack('>f', struct.pack('>HH', *new_registers[2:]))[0]
            else:
                return ExcCodes.ILLEGAL_ADDRESS
            if p is not None and (not math.isfinite(p) or not 0 <= p <= 110):
                return ExcCodes.ILLEGAL_VALUE
            if q is not None and (not math.isfinite(q) or abs(q) > 66):
                return ExcCodes.ILLEGAL_VALUE

            try:
                data = json.loads(state_path.read_text(encoding='utf-8-sig'))
                if p is not None:
                    data['mode'] = 'fixed'
                    data['power_kw'] = p
                if q is not None:
                    data['q_kvar'] = q
                pending = state_path.with_suffix('.pq-write.tmp')
                pending.write_text(json.dumps(data, indent=2)+'\n')
                mode, values = checked_state(pending)
                pending.replace(state_path)
                state.mode = mode
                state.fixed_power_kw = values['power_kw']
                state.q_kvar = values['q_kvar']
                state.voltage_v = values['voltage_v']
                print(
                    f'CCI FC16 command applied: P={state.fixed_power_kw:.3f} kW '
                    f'Q={state.q_kvar:.3f} kvar', flush=True)
                return None
            except (OSError, ValueError) as exc:
                print(f'CCI P/Q command failed: {exc}', flush=True)
                return ExcCodes.DEVICE_FAILURE
        try:
            mode, values = checked_state(state_path)
            state.mode = mode
            state.fixed_power_kw = values['power_kw']
            state.q_kvar = values['q_kvar']
            state.voltage_v = values['voltage_v']
        except (OSError, ValueError) as exc:
            print(f'State update rejected; retaining last valid values: {exc}', flush=True)
        if cci_overlap and new_registers is None:
            module.Sun2000State.update_registers(state, monitoring, 0)
            words, p, q, voltage = mirror_values(monitoring)
            offset = BASE - start_address
            if offset < 0 or offset + 6 > len(current_registers): raise ValueError('compatibility view outside current datastore')
            current_registers[offset:offset+6] = words
            print(f'CCI FC{function_code:02} read @{address} count={count}: P={p:.3f} kW Q={q:.3f} kvar V={voltage:.1f} V', flush=True)
            return None
        return await original_action(function_code, start_address, address, count, current_registers, new_registers)

    device.action = access
    return device


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(r'C:\MY\GIT\cci-software\Inverter Simulator\sun2000_100ktl_m2_rtu_real_register_mock.py'))
    parser.add_argument('--state', type=Path, default=Path(__file__).with_name('state.json'))
    parser.add_argument('--host', default='127.0.0.1')
    parser.add_argument('--port', type=int, default=15020)
    parser.add_argument('--transport', choices=['rtu', 'tcp'], default='rtu')
    parser.add_argument('--device', default='COM8')
    parser.add_argument('--baudrate', type=int, default=9600)
    parser.add_argument('--self-test', action='store_true')
    parser.add_argument('--accept-cci-pq-writes', '--accept-cci-q-writes',
                        dest='accept_cci_pq_writes', action='store_true',
                        help='Bench only: FC16 float32 at 60000/60002 changes simulated P/Q; voltage remains read-only')
    args = parser.parse_args()
    if not 1 <= args.port <= 65535: parser.error('invalid port')
    module = load_model(args.source)
    device = build_device(module, args.state, args.accept_cci_pq_writes)
    if args.self_test:
        registers = module.build_initial_registers(0x0200)
        state = module.Sun2000State(400, -12, 'fixed', 75, 7, 0x0200)
        with contextlib.redirect_stdout(io.StringIO()): state.update_registers(registers, 0)
        words, p, q, voltage = mirror_values(registers)
        assert p == 75 and q == -12
        assert struct.unpack('>ff', struct.pack('>4H', *words[:4])) == (75, -12)
        # PyModbus coalesces sparse blocks into one datastore starting at zero.
        result = [0] * (BASE + 6)
        with contextlib.redirect_stdout(io.StringIO()): asyncio.run(device.action(3, 0, BASE, 6, result, None))
        mode, values = checked_state(args.state)
        if mode == 'fixed': assert abs(struct.unpack('>f', struct.pack('>HH', *result[BASE:BASE+2]))[0] - values['power_kw']) < .001
        from pymodbus.simulator.simruntime import SimRuntime
        async def test_writes(path):
            runtime = SimRuntime(build_device(module, path, True))
            assert await runtime.async_setValues(16, BASE+2, float_words(-12.5)) is None
            words = await runtime.async_getValues(3, BASE, 6)
            assert struct.unpack('>f', struct.pack('>HH', *words[2:4]))[0] == -12.5
            assert await runtime.async_setValues(16, BASE, float_words(50)) is None
            words = await runtime.async_getValues(3, BASE, 6)
            assert struct.unpack('>f', struct.pack('>HH', *words[:2]))[0] == 50
            assert struct.unpack('>f', struct.pack('>HH', *words[2:4]))[0] == -12.5
            assert await runtime.async_setValues(16, BASE+4, float_words(400)) == ExcCodes.ILLEGAL_ADDRESS
            assert await runtime.async_setValues(16, BASE+1, [0]) == ExcCodes.ILLEGAL_ADDRESS
            assert await runtime.async_setValues(6, BASE+2, [0]) == ExcCodes.ILLEGAL_ADDRESS
            assert await runtime.async_setValues(16, BASE, float_words(120)) == ExcCodes.ILLEGAL_VALUE
            assert await runtime.async_setValues(16, BASE+2, float_words(100)) == ExcCodes.ILLEGAL_VALUE
        with tempfile.TemporaryDirectory() as temp:
            fixture = Path(temp)/'state.json'
            fixture.write_text(args.state.read_text(encoding='utf-8-sig'))
            asyncio.run(test_writes(fixture))
        print('PASS: Huawei float view, P/Q command round trip, read-only voltage and invalid-write rejection')
        return
    print(f'Using original simulator: {args.source}', flush=True)
    print(f'Modbus RTU endpoint: {args.device}; {args.baudrate} baud 8N1; unit 1' if args.transport == 'rtu' else
          f'Modbus TCP endpoint: {args.host}:{args.port}; unit 1', flush=True)
    print('Huawei registers remain intact. CCI compatibility view: 60000=P(kW), 60002=Q(kvar), 60004=V(V), float32 high-word-first.', flush=True)
    print(f'Edit {args.state} to change the mock behavior. Ctrl+C stops the simulator.', flush=True)
    if args.accept_cci_pq_writes: print('Bench P/Q control enabled: CCI FC16 @60000/@60002 updates simulated output.', flush=True)
    if args.transport == 'rtu':
        StartSerialServer(context=device, framer=FramerType.RTU, port=args.device,
                          baudrate=args.baudrate, bytesize=8, parity='N', stopbits=1)
    else:
        StartTcpServer(context=device, address=(args.host, args.port))


if __name__ == '__main__':
    sys.dont_write_bytecode = True
    main()

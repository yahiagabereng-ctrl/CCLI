#!/usr/bin/env python3
"""Adjust the running COM8 inverter mock without restarting its serial server."""
import argparse
import json
import os
from pathlib import Path
import tempfile
from bridge import checked_state

STATE = Path(__file__).with_name('state.json')


def save(data):
    with tempfile.NamedTemporaryFile(mode='w', encoding='utf-8', dir=STATE.parent, suffix='.json', delete=False) as f:
        temp = Path(f.name)
        json.dump(data, f, indent=2)
        f.write('\n')
    try:
        checked_state(temp)
        os.replace(temp, STATE)
    finally:
        if temp.exists(): temp.unlink()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--power-kw', type=float)
    parser.add_argument('--q-kvar', type=float)
    parser.add_argument('--voltage-v', type=float)
    parser.add_argument('--mode', choices=['fixed', 'daytime', 'daycycle'])
    args = parser.parse_args()
    updates = {k: v for k, v in vars(args).items() if v is not None}
    if updates:
        data = json.loads(STATE.read_text(encoding='utf-8-sig'))
        data.update(updates)
        save(data)
        print('Updated mock:', data)
        return
    print('INVERTER MOCK - COM8, 9600 8N1, unit 1')
    print('Values feed the running CCI control logic. Huawei control writes are currently logged only.')
    while True:
        data = json.loads(STATE.read_text(encoding='utf-8-sig'))
        print(f"\nMode: {data['mode']} | P: {data['power_kw']} kW | Q: {data['q_kvar']} kvar | nominal V: {data['voltage_v']} V")
        print('1 Set active power (fixed mode) | 2 Set reactive power | 3 Set mode | 4 Set voltage | 0 Exit')
        try:
            choice = input('Choose> ').strip()
            if choice == '0': return
            if choice == '1': data['power_kw'] = float(input('Power kW (0..100): ')); data['mode'] = 'fixed'
            elif choice == '2': data['q_kvar'] = float(input('Reactive power kvar (-66..66): '))
            elif choice == '3': data['mode'] = input('Mode (fixed/daytime/daycycle): ').strip()
            elif choice == '4': data['voltage_v'] = float(input('Nominal voltage V (100..800): '))
            else: continue
            save(data)
            print('Updated. CCI will read the change on its next Modbus poll.')
        except ValueError as exc: print(f'Invalid value: {exc}')
        except (EOFError, KeyboardInterrupt): print(); return


if __name__ == '__main__':
    try: main()
    except (OSError, ValueError) as exc: raise SystemExit(str(exc))

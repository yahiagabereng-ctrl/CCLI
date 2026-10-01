#!/usr/bin/env python3
"""Phase 1 Modbus RTU slave — POC meter for TG544 CCLI (pymodbus 3.x).

Register map (simulator_map.yaml):
  40001 offset 0 — P kW float32 BE
  40003 offset 2 — Q kvar float32 BE

  python lab/modbus_rtu_slave.py --port COM5 --power-kw 42 --trace
  python lab/modbus_rtu_slave.py --port COM5 --ramp --trace
"""
from __future__ import annotations

import argparse
import datetime as dt
import struct
import threading
import time

try:
    from pymodbus.datastore import ModbusSequentialDataBlock, ModbusServerContext, ModbusSlaveContext
    from pymodbus.server import StartSerialServer
except ImportError as exc:
    raise SystemExit("pip install 'pymodbus>=3.6,<3.8'") from exc


def float32_to_regs(value: float) -> tuple[int, int]:
    raw = struct.unpack(">I", struct.pack(">f", float(value)))[0]
    return (raw >> 16) & 0xFFFF, raw & 0xFFFF


def regs_to_float32(hi: int, lo: int) -> float:
    raw = ((hi & 0xFFFF) << 16) | (lo & 0xFFFF)
    return struct.unpack(">f", struct.pack(">I", raw))[0]


def write_power(store: ModbusSlaveContext, p_kw: float) -> None:
    q_kvar = p_kw * 0.1
    hi, lo = float32_to_regs(p_kw)
    qhi, qlo = float32_to_regs(q_kvar)
    store.setValues(3, 0, [hi, lo, qhi, qlo])


class TracingDataBlock(ModbusSequentialDataBlock):
    """Log each master read (FC03/04) — pairs with CCLI `modbus.trace` / regulation-check."""

    def __init__(self, trace: bool, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._trace = trace

    def getValues(self, address: int, count: int = 1) -> list:
        values = super().getValues(address, count)
        if not self._trace:
            return values
        ts = dt.datetime.now().strftime("%H:%M:%S.%f")[:-3]
        doc = 40001 + address
        p_txt = ""
        if address <= 0 and count >= 2:
            p_txt = f" P={regs_to_float32(values[0], values[1]):.2f}kW"
        hex_vals = " ".join(f"0x{v:04x}" for v in values)
        print(
            f"[{ts}] SLAVE RX FC03 ReadHolding addr={doc} qty={count} "
            f"-> [{hex_vals}]{p_txt}",
            flush=True,
        )
        return values

    def setValues(self, address: int, values: list) -> None:
        super().setValues(address, values)
        if not self._trace:
            return
        ts = dt.datetime.now().strftime("%H:%M:%S.%f")[:-3]
        doc = 40001 + address
        q_txt = ""
        if address <= 2 and len(values) >= 2:
            q_txt = f" Q={regs_to_float32(values[0], values[1]):.2f}kvar"
        hex_vals = " ".join(f"0x{v:04x}" for v in values)
        print(
            f"[{ts}] SLAVE RX FC16 WriteHolding addr={doc} qty={len(values)} "
            f"-> [{hex_vals}]{q_txt}",
            flush=True,
        )


def ramp_thread(store: ModbusSlaveContext, stop: threading.Event) -> None:
    t0 = time.monotonic()
    while not stop.is_set():
        phase = (time.monotonic() - t0) % 120.0
        p = 400.0 + 550.0 * abs(1.0 - abs(phase / 60.0 - 1.0))
        write_power(store, p)
        stop.wait(0.5)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", required=True)
    parser.add_argument("--baud", type=int, default=9600)
    parser.add_argument("--power-kw", type=float, default=500.0)
    parser.add_argument("--ramp", action="store_true")
    parser.add_argument(
        "--trace",
        action="store_true",
        help="Print each master read (compare with TG544 logread / regulation-check)",
    )
    args = parser.parse_args()

    block_cls = TracingDataBlock if args.trace else ModbusSequentialDataBlock
    if args.trace:
        hr = block_cls(True, 0, [0] * 10)
    else:
        hr = block_cls(0, [0] * 10)
    store = ModbusSlaveContext(hr=hr, zero_mode=True)
    write_power(store, args.power_kw)
    context = ModbusServerContext(slaves=store, single=True)

    stop = threading.Event()
    if args.ramp:
        threading.Thread(target=ramp_thread, args=(store, stop), daemon=True).start()
        print("Ramp: 400..950 kW / 120 s")
    else:
        print(f"Fixed P = {args.power_kw} kW")

    if args.trace:
        print("Trace: ON — each CCLI FC03 poll appears below (master = TG544 A2/B2)")
    print(f"Listening {args.port} @ {args.baud} 8N1 slave 1")
    try:
        StartSerialServer(
            context=context,
            port=args.port,
            baudrate=args.baud,
            bytesize=8,
            parity="N",
            stopbits=1,
            timeout=1,
        )
    finally:
        stop.set()


if __name__ == "__main__":
    main()

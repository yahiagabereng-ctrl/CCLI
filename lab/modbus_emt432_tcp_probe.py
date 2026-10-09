#!/usr/bin/env python3
"""Chronos EMT432 — Modbus TCP bench probe (LAN3 plant network).

Usage (PC on plant LAN3):
  pip install pymodbus
  # Bench A — 192.168.30.x (zone docs):
  python lab/modbus_emt432_tcp_probe.py --host 192.168.30.50
  # Bench B — EMT432 factory-style (SN 261001102952):
  lab/set_pc_lan3_plant_178.cmd  # Admin → 192.168.178.10/24
  python lab/modbus_emt432_tcp_probe.py --host 192.168.178.250

Scan /24 for Modbus TCP slave 1:
  python lab/modbus_emt432_tcp_probe.py --scan --subnet 192.168.178
  python lab/modbus_emt432_tcp_probe.py --scan --subnet 192.168.30

From TG544 (L3 only; needs meter :502 open):
  ping -c 2 192.168.178.250
  # Prefer probe from PC on LAN3 — DUT has no python3/pymodbus by default.
"""
from __future__ import annotations

import argparse
import struct
import sys
from typing import Optional

try:
    from pymodbus.client import ModbusTcpClient
except ImportError:
    print("Install: pip install pymodbus", file=sys.stderr)
    sys.exit(1)


def f32_be(hi: int, lo: int, *, word_swap: bool = True) -> float:
    """EMT432 Modbus TCP uses low register word first (same as ccli float_word_swap)."""
    if word_swap:
        hi, lo = lo, hi
    return struct.unpack(">f", struct.pack(">HH", hi & 0xFFFF, lo & 0xFFFF))[0]


def read_f32(
    client: ModbusTcpClient, offset: int, unit: int, *, word_swap: bool = True
) -> Optional[float]:
    rr = client.read_holding_registers(offset, 2, slave=unit)
    if rr.isError():
        return None
    return f32_be(rr.registers[0], rr.registers[1], word_swap=word_swap)


def read_u16(client: ModbusTcpClient, offset: int, unit: int) -> Optional[int]:
    rr = client.read_holding_registers(offset, 1, slave=unit)
    if rr.isError():
        return None
    return rr.registers[0]


def probe_host(host: str, port: int, unit: int, timeout: float, *, word_swap: bool) -> bool:
    print(f"\n=== {host}:{port} unit={unit} ===")
    client = ModbusTcpClient(host, port=port, timeout=timeout)
    if not client.connect():
        print("  connect: FAIL")
        return False
    print("  connect: OK")

    feat = read_u16(client, 14, unit)  # feature_version @ 40015
    mounted = read_u16(client, 10, unit)  # mounted_version @ 40011
    meas_cfg = read_u16(client, 231, unit)  # 40232
    mode = (meas_cfg >> 1) & 0x3 if meas_cfg is not None else None
    mode_names = {0: "single phase", 1: "3P 3W 2TA", 2: "3P 3W 3TA", 3: "3P 4W 3TA+N"}

    v_ln = read_f32(client, 974, unit, word_swap=word_swap)  # V_L1_N_peak
    freq = read_f32(client, 972, unit, word_swap=word_swap)
    p1 = read_f32(client, 932, unit, word_swap=word_swap)
    q1 = read_f32(client, 940, unit, word_swap=word_swap)

    if feat is not None:
        print(f"  feature_version @40015: {feat} ({'Full' if feat else 'Base'})")
    if mounted is not None:
        print(f"  mounted_version @40011: {mounted} ({'5A TA' if mounted == 0 else '333mV/Rog'})")
    if mode is not None:
        print(f"  Measurement_Config @40232 mode: {mode} = {mode_names.get(mode, '?')}")
    if v_ln is not None:
        print(f"  V_L1_N_peak @40975: {v_ln:.1f} V")
    if freq is not None:
        print(f"  Frequency @40973: {freq:.3f} Hz")
    if p1 is not None:
        print(f"  P1 @40933: {p1:.1f} W ({p1 / 1000.0:.3f} kW)")
    if q1 is not None:
        print(f"  Q1 @40941: {q1:.1f} VAR ({q1 / 1000.0:.3f} kvar)")

    client.close()
    ok = v_ln is not None or p1 is not None
    print(f"  VERDICT: {'PASS (Modbus TCP read)' if ok else 'PART (connected, read failed)'}")
    return ok


def scan_subnet(
    subnet_prefix: str, port: int, unit: int, timeout: float, *, word_swap: bool
) -> None:
    print("Scanning 192.168.30.2–254 (Modbus TCP)...")
    found = 0
    for last in range(2, 255):
        host = f"192.168.30.{last}"
        client = ModbusTcpClient(host, port=port, timeout=timeout)
        if not client.connect():
            continue
        rr = client.read_holding_registers(14, 1, slave=unit)
        client.close()
        if not rr.isError():
            print(f"  candidate: {host}")
            probe_host(host, port, unit, timeout)
            probe_host(host, port, unit, timeout, word_swap=word_swap)
    if found == 0:
        print("  No Modbus TCP slave found on 192.168.30.0/24")
        print("  Check: meter IP, LAN3 cable, PC NIC 192.168.30.10/24")


def main() -> int:
    ap = argparse.ArgumentParser(description="EMT432 Modbus TCP bench probe")
    ap.add_argument("--host", default="192.168.30.50", help="Meter IP (plant LAN3)")
    ap.add_argument("--host", default="192.168.178.250", help="Meter IP (plant LAN3)")
    ap.add_argument(
        "--subnet",
        default="192.168.30",
        help="First three octets for --scan (e.g. 192.168.178)",
    )
    ap.add_argument("--port", type=int, default=502)
    ap.add_argument("--unit", type=int, default=1, help="Modbus unit/slave ID")
    ap.add_argument("--timeout", type=float, default=2.0)
    ap.add_argument(
        "--no-word-swap",
        action="store_true",
        help="Use naive big-endian word order (RTU-style; not EMT432 TCP)",
    )
    ap.add_argument("--scan", action="store_true", help="Scan 192.168.30.0/24")
    args = ap.parse_args()
    word_swap = not args.no_word_swap

    if args.scan:
        scan_subnet(args.subnet, args.port, args.unit, args.timeout, word_swap=word_swap)
        return 0
    return (
        0
        if probe_host(args.host, args.port, args.unit, args.timeout, word_swap=word_swap)
        else 1
    )

if __name__ == "__main__":
    sys.exit(main())

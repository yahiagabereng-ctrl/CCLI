#!/usr/bin/env python3
"""Chronos EMT432.V — read/apply Rogowski bench Modbus config (EMT430_MR Rev2).

Offsets are from 40001 (holding). TCP port 502, unit 1.

  pip install pymodbus
  # PC direct to meter RJ45 — set NIC 192.168.178.10/24 first (Admin):
  #   lab\\set_pc_lan3_plant_178.cmd
  python lab/emt432_rogowski_config.py --scan
  python lab/emt432_rogowski_config.py --host 192.168.178.102 --read
  python lab/emt432_rogowski_config.py --host 192.168.178.102 --apply-1p-rogowski

Registers (see knowledge-base/.../EMT430_MR.pdf):
  40011 mounted_version  RO 0=5A 1=Rogowski
  40232 Measurement_Configuration  bit1-2 connection; bit13 integrator (Rogowski)
  40235 TA_Transducer_ratio  float — Rogowski sensitivity [V/A]
"""
from __future__ import annotations

import argparse
import struct
import sys
from typing import Optional, Tuple

try:
    from pymodbus.client import ModbusTcpClient
except ImportError:
    print("Install: pip install pymodbus", file=sys.stderr)
    sys.exit(1)

OFF_MOUNTED = 10  # 40011
OFF_MEAS_CFG = 231  # 40232
OFF_TA_CHRONOS = 233  # 40234
OFF_TA_SENS = 234  # 40235 float (2 regs)

MODE_NAMES = {0: "single phase", 1: "3P 3W 2TA", 2: "3P 3W 3TA", 3: "3P 4W 3TA+N"}


def f32_be(hi: int, lo: int, *, word_swap: bool = True) -> float:
    if word_swap:
        hi, lo = lo, hi
    return struct.unpack(">f", struct.pack(">HH", hi & 0xFFFF, lo & 0xFFFF))[0]


def pack_f32(value: float, *, word_swap: bool = True) -> Tuple[int, int]:
    hi, lo = struct.unpack(">HH", struct.pack(">f", float(value)))
    if word_swap:
        hi, lo = lo, hi
    return hi & 0xFFFF, lo & 0xFFFF


def read_u16(client: ModbusTcpClient, offset: int, unit: int) -> Optional[int]:
    rr = client.read_holding_registers(offset, 1, slave=unit)
    if rr.isError():
        return None
    return rr.registers[0]


def read_f32(client: ModbusTcpClient, offset: int, unit: int, *, word_swap: bool) -> Optional[float]:
    rr = client.read_holding_registers(offset, 2, slave=unit)
    if rr.isError():
        return None
    return f32_be(rr.registers[0], rr.registers[1], word_swap=word_swap)


def write_u16(client: ModbusTcpClient, offset: int, value: int, unit: int) -> bool:
    wr = client.write_register(offset, value & 0xFFFF, slave=unit)
    return not wr.isError()


def write_f32(client: ModbusTcpClient, offset: int, value: float, unit: int, *, word_swap: bool) -> bool:
    hi, lo = pack_f32(value, word_swap=word_swap)
    wr = client.write_registers(offset, [hi, lo], slave=unit)
    return not wr.isError()


def decode_meas_cfg(reg: int) -> dict:
    return {
        "raw": reg,
        "connection_mode": (reg >> 1) & 0x3,
        "integrator_rogowski": bool((reg >> 13) & 1),
        "hpf_disabled": bool((reg >> 4) & 1),
    }


def apply_1p_rogowski(reg: int) -> int:
    """Single phase (bits 1-2 = 0) + integrator enabled (bit 13 = 1)."""
    reg = reg & ~0x6
    reg = reg | (1 << 13)
    return reg


def mv_per_ka_to_v_per_a(mv_per_ka: float) -> float:
    return mv_per_ka / 1_000_000.0


BENCH_MV_KA_DEFAULT = 80.0


def describe_sensitivity_v_per_a(sens: float) -> str:
    """Human-readable verdict for 40235 Rogowski [V/A] (shared with tcp probe)."""
    if abs(sens - 0.00008) < 1e-8:
        return "OK 80 mV/kA (40235=0.00008). Bench pin."
    if abs(sens - 0.0001) < 1e-8:
        return "OK 100 mV/kA (FRC-350-100mV catalog)."
    if abs(sens - 0.000085) < 1e-8:
        return "WARN 85 mV/kA — no Chronos SKU; confirm coil label."
    if abs(sens - 1.0) < 0.05:
        return f"FAIL factory 1.0 (5A ratio). Use --set-sensitivity-mvka {BENCH_MV_KA_DEFAULT:g} or 100."
    if abs(sens - 85.0) < 0.5 or abs(sens - 100.0) < 0.5:
        return "FAIL typed mV into V/A field. Use --set-sensitivity-mvka, not 85/100 in register."
    return f"WARN unexpected V/A {sens:g} — expected 0.00008 (80) or 0.0001 (100)."


def print_config(host: str, unit: int, *, word_swap: bool, client: ModbusTcpClient) -> None:
    mounted = read_u16(client, OFF_MOUNTED, unit)
    meas = read_u16(client, OFF_MEAS_CFG, unit)
    ta_ch = read_u16(client, OFF_TA_CHRONOS, unit)
    sens = read_f32(client, OFF_TA_SENS, unit, word_swap=word_swap)
    p1 = read_f32(client, 932, unit, word_swap=word_swap)

    print(f"Host: {host} unit={unit}")
    if mounted is not None:
        print(f"  40011 mounted_version: {mounted} ({'Rogowski/V' if mounted else '5A TA — WRONG HW?'})")
    if meas is not None:
        d = decode_meas_cfg(meas)
        print(f"  40232 Measurement_Config: {meas} (0x{meas:x})")
        print(f"         connection: {d['connection_mode']} = {MODE_NAMES.get(d['connection_mode'], '?')}")
        print(f"         integrator (bit13): {'ON' if d['integrator_rogowski'] else 'OFF — enable for FRC'}")
    if ta_ch is not None:
        print(f"  40234 TA_Chronos_register: {ta_ch} (0=user; use Chronos app/NFC for FRC profile)")
    if sens is not None:
        print(f"  40235 Rogowski sensitivity [V/A]: {sens:g}")
        print(f"         {describe_sensitivity_v_per_a(sens)}")
    if p1 is not None:
        print(f"  P1 @932: {p1:.1f} W")


def scan_subnet(subnet: str, port: int, unit: int, timeout: float) -> None:
    prefix = subnet.rstrip(".")
    print(f"Modbus TCP scan {prefix}.1–254 ...")
    for last in range(1, 255):
        host = f"{prefix}.{last}"
        client = ModbusTcpClient(host, port=port, timeout=timeout)
        if not client.connect():
            continue
        rr = client.read_holding_registers(OFF_MOUNTED, 1, slave=unit)
        client.close()
        if not rr.isError():
            print(f"  FOUND {host} mounted_version={rr.registers[0]}")


def main() -> int:
    ap = argparse.ArgumentParser(description="EMT432 Rogowski Modbus config (bench)")
    ap.add_argument("--host", default="192.168.178.102")
    ap.add_argument("--port", type=int, default=502)
    ap.add_argument("--unit", type=int, default=1)
    ap.add_argument("--timeout", type=float, default=2.0)
    ap.add_argument("--scan", action="store_true")
    ap.add_argument("--subnet", default="192.168.178")
    ap.add_argument("--read", action="store_true", help="Read config (default if no --apply)")
    ap.add_argument(
        "--apply-1p-rogowski",
        action="store_true",
        help="Write 40232: single phase + integrator ON (read-modify-write)",
    )
    ap.add_argument(
        "--set-sensitivity-v-a",
        type=float,
        default=None,
        help="Write 40235 float sensitivity [V/A] (e.g. 0.0001 for 100 mV/kA)",
    )
    ap.add_argument(
        "--set-sensitivity-mvka",
        type=float,
        default=None,
        help=f"Write 40235 from mV/kA (bench default {BENCH_MV_KA_DEFAULT:g}; catalog 100 for FRC-350-100mV)",
    )
    ap.add_argument("--no-word-swap", action="store_true")
    args = ap.parse_args()
    word_swap = not args.no_word_swap

    if args.scan:
        scan_subnet(args.subnet, args.port, args.unit, args.timeout)
        return 0

    client = ModbusTcpClient(args.host, port=args.port, timeout=args.timeout)
    if not client.connect():
        print(f"CONNECT FAIL {args.host}:{args.port}")
        print("Set PC NIC 192.168.178.10/24 (Admin: lab/set_pc_lan3_plant_178.cmd)")
        print("Or: python lab/emt432_rogowski_config.py --scan --subnet 192.168.178")
        return 1

    print("CONNECT OK")
    print_config(args.host, args.unit, word_swap=word_swap, client=client)

    if args.apply_1p_rogowski:
        cur = read_u16(client, OFF_MEAS_CFG, args.unit)
        if cur is None:
            print("READ 40232 failed — abort write")
            client.close()
            return 1
        new = apply_1p_rogowski(cur)
        if not write_u16(client, OFF_MEAS_CFG, new, args.unit):
            print("WRITE 40232 FAILED")
            client.close()
            return 1
        print(f"WROTE 40232: {cur} -> {new} (single phase + integrator ON)")

    sens = args.set_sensitivity_v_a
    if args.set_sensitivity_mvka is not None:
        sens = mv_per_ka_to_v_per_a(args.set_sensitivity_mvka)
    if sens is not None:
        if not write_f32(client, OFF_TA_SENS, sens, args.unit, word_swap=word_swap):
            print("WRITE 40235 FAILED")
            client.close()
            return 1
        print(f"WROTE 40235 sensitivity [V/A]: {sens:g}")

    if args.apply_1p_rogowski or sens is not None:
        print("\n--- After write ---")
        print_config(args.host, args.unit, word_swap=word_swap, client=client)

    client.close()
    return 0


if __name__ == "__main__":
    sys.exit(main())

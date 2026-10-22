#!/usr/bin/env python3
"""Cross-check EMT432 Modbus map: EMT430_MR Rev2 ↔ ccli yaml ↔ Basjes schema.

Usage:
  python lab/emt432_validate_register_map.py
  python lab/emt432_validate_register_map.py --live --host 127.0.0.1 --port 15002

MR source: knowledge-base/08-engineering/reference/vendor/chronos/EMT430_MR.pdf
Basjes schema: apps/ccli/config/modbus/chronos_emt432_basjes.yaml
"""
from __future__ import annotations

import argparse
import re
import struct
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]

# EMT430_MR Rev2 — PLC 4xxxx holding addresses (authoritative)
MR_REALTIME = {
    "alarm_u16_not_v": 40906,
    "V_L1_N": 40907,
    "I_L1": 40923,
    "P1": 40933,
    "Q1": 40941,
    "P_SUM": 40939,
    "Frequency": 40973,
}

MR_TRAP = {
    "I_L1_peak": 40987,
    "CF1": 40965,
    "V_L1_L2_peak": 40981,
}

RMS_OFFSETS_FILE = REPO / "lab/conformance/EMT430_RMS_REGISTER_OFFSETS.txt"


def load_mr_rms_offsets() -> dict[str, int]:
    out: dict[str, int] = {}
    for ln in RMS_OFFSETS_FILE.read_text(encoding="utf-8").splitlines():
        ln = ln.strip()
        if not ln or ln.startswith("#"):
            continue
        plc_s, off_s, name = ln.split()
        out[name] = int(off_s)
    return out


def plc_to_offset(plc: int) -> int:
    return plc - 40001


def load_lab_yaml_offsets() -> dict[str, int]:
    import yaml  # type: ignore

    path = REPO / "apps/ccli/config/lab_tr400_emt432_lan3_tcp_102.yaml"
    data = yaml.safe_load(path.read_text(encoding="utf-8"))
    mb = data["modbus"]
    return {
        "P1": int(mb["reg_active_power"]),
        "Q1": int(mb["reg_reactive_power"]),
        "V_L1_N": int(mb["reg_line_voltage"]),
    }


def parse_basjes_hr_fields(path: Path) -> dict[str, int]:
    text = path.read_text(encoding="utf-8")
    fields: dict[str, int] = {}
    current_id: str | None = None
    for line in text.splitlines():
        m = re.match(r"\s+- id: '([^']+)'", line)
        if m:
            current_id = m.group(1)
            continue
        m = re.match(r"\s+expression: 'ieee754_32\(hr:(\d+) # 2\)'", line)
        if m and current_id:
            fields[current_id] = int(m.group(1))
            current_id = None
    return fields


def f32_word_swap(hi: int, lo: int) -> float:
    hi, lo = lo, hi
    return struct.unpack(">f", struct.pack(">HH", hi & 0xFFFF, lo & 0xFFFF))[0]


def live_check(host: str, port: int) -> None:
    from pymodbus.client import ModbusTcpClient

    client = ModbusTcpClient(host, port=port, timeout=3)
    if not client.connect():
        print(f"FAIL: connect {host}:{port}")
        sys.exit(1)
    unit = 1
    checks = [
        ("V_L1_N", plc_to_offset(MR_REALTIME["V_L1_N"]), 200.0, 260.0),
        ("I_L1", plc_to_offset(MR_REALTIME["I_L1"]), 0.0, 50.0),
        ("P1", plc_to_offset(MR_REALTIME["P1"]), -5000.0, 5000.0),
    ]
    print(f"--- live {host}:{port} ---")
    for name, off, lo, hi in checks:
        rr = client.read_holding_registers(off, 2, slave=unit)
        if rr.isError():
            print(f"  {name} @{off}: READ ERR")
            continue
        v = f32_word_swap(rr.registers[0], rr.registers[1])
        ok = lo <= v <= hi or (name == "I_L1" and abs(v) < 0.05)
        flag = "OK" if ok else "WARN"
        print(f"  {flag} {name} PLC {40001 + off} off {off}: {v:g}")
    client.close()


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--live", action="store_true")
    ap.add_argument("--host", default="192.168.178.10")
    ap.add_argument("--port", type=int, default=502)
    args = ap.parse_args()

    basjes = REPO / "apps/ccli/config/modbus/chronos_emt432_basjes.yaml"
    lab = load_lab_yaml_offsets()
    basjes_fields = parse_basjes_hr_fields(basjes)

    expected = {
        "V_L1_N": plc_to_offset(MR_REALTIME["V_L1_N"]),
        "P1": plc_to_offset(MR_REALTIME["P1"]),
        "Q1": plc_to_offset(MR_REALTIME["Q1"]),
    }

    print("=== EMT430_MR Rev2 vs ccli lab yaml ===")
    ok = True
    for key, plc in [
        ("V_L1_N", MR_REALTIME["V_L1_N"]),
        ("P1", MR_REALTIME["P1"]),
        ("Q1", MR_REALTIME["Q1"]),
    ]:
        off = expected[key]
        got = lab[key]
        line_ok = got == off
        ok = ok and line_ok
        print(
            f"  {'PASS' if line_ok else 'FAIL'} {key}: MR {plc} -> off {off}, yaml reg_*={got}"
        )
    if lab["V_L1_N"] == plc_to_offset(MR_REALTIME["alarm_u16_not_v"]):
        print("  FAIL: yaml still points V at 40906 (alarm u16) — use 906 / 40907")
        ok = False

    print("=== MR vs Basjes schema (hr: offsets) ===")
    basjes_map = {
        "V_L1_N RMS": expected["V_L1_N"],
        "P1 RMS": expected["P1"],
        "Q1 RMS": expected["Q1"],
        "I_L1 RMS": plc_to_offset(MR_REALTIME["I_L1"]),
    }
    for fid, want in basjes_map.items():
        got = basjes_fields.get(fid)
        line_ok = got == want
        ok = ok and line_ok
        print(f"  {'PASS' if line_ok else 'FAIL'} {fid}: want hr:{want:05d}, schema hr:{got}")

    print("=== Trap registers (must NOT map to P or I RMS) ===")
    for name, plc in MR_TRAP.items():
        print(f"  INFO {name}: PLC {plc} off {plc_to_offset(plc)}")

    print("=== MR 40907-40985 vs Basjes realtime_rms_mr_full ===")
    mr_rms = load_mr_rms_offsets()
    alias = {"Frequency": "Frequency_mr"}
    for name, want in mr_rms.items():
        fid = alias.get(name, name)
        got = basjes_fields.get(fid)
        if got is None:
            got = basjes_fields.get(f"{name} RMS")
        line_ok = got == want
        ok = ok and line_ok
        print(f"  {'PASS' if line_ok else 'FAIL'} {name}: want hr:{want:05d}, schema hr:{got}")

    if args.live:
        live_check(args.host, args.port)

    print("=== " + ("PASS" if ok else "FAIL") + " ===")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())

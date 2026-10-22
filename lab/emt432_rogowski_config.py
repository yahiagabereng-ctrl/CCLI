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
from pathlib import Path
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
OFF_TA_DELAY = 236  # 40237
OFF_TV_RATIO = 238  # 40239
OFF_TV_DELAY = 240  # 40241
OFF_MIN_V_RIPPLE = 242  # 40243
OFF_MIN_I_RIPPLE = 244  # 40245
OFF_MIN_P_RIPPLE = 246  # 40247
RIPPLE_TOLERANCE = 1e-5
OFF_ENERGY_UNIT = 255  # 40256
ENERGY_UNIT_KWH = 4
OFF_KWH1 = 715  # 40716 int64 x4 regs
OFF_KWH_SUM = 728
OFF_KVARH1 = 764
OFF_KVAH1 = 844
OFF_RMS_BLOCK = 906  # 40907 V_L1_N
RMS_BLOCK_REGS = 80  # through 40985


def load_rms_signal_names() -> list[tuple[int, str]]:
    path = Path(__file__).resolve().parents[1] / "lab/conformance/EMT430_RMS_REGISTER_OFFSETS.txt"
    out: list[tuple[int, str]] = []
    for ln in path.read_text(encoding="utf-8").splitlines():
        ln = ln.strip()
        if not ln or ln.startswith("#"):
            continue
        _plc, off_s, name = ln.split()
        out.append((int(off_s), name))
    return out

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


def read_i64_wh_tenth(
    client: ModbusTcpClient, offset: int, unit: int
) -> Optional[int]:
    rr = client.read_holding_registers(offset, 4, slave=unit)
    if rr.isError():
        return None
    raw = 0
    for w in rr.registers[:4]:
        raw = (raw << 16) | (w & 0xFFFF)
    if raw >= (1 << 63):
        raw -= 1 << 64
    return raw


def wh_tenth_to_kwh(raw: int) -> float:
    return raw / 10000.0


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
        "energy_saving": bool((reg >> 9) & 1),
        "filtered_measurement": bool((reg >> 15) & 1),
    }


def apply_1p_rogowski(reg: int) -> int:
    """Single phase (bits 1-2 = 0) + integrator enabled (bit 13 = 1)."""
    reg = reg & ~0x6
    reg = reg | (1 << 13)
    return reg


BENCH_MV_KA_DEFAULT = 85.0

# CCLI-LAB-EMT432-CERT-PIN-001 — lab/conformance/EMT432_CERT_REGISTER_PIN.md
CERT_MV_PER_KA = 85.0
CERT_SENS_V_PER_A = 0.000085
CERT_MOUNTED_VERSION = 1
CERT_TA_CHRONOS_REG = 0
CERT_SENS_TOLERANCE = 1e-8
CERT_MODBUS_MAP = {
    "p1_w": 932,
    "q1_var": 940,
    "v_l1n_rms_v": 906,
}


def verify_cert_pin(
    client: ModbusTcpClient,
    unit: int,
    *,
    word_swap: bool = True,
    sens_v_per_a: float = CERT_SENS_V_PER_A,
) -> Tuple[bool, list[str]]:
    """Return (ok, messages) against EMT432_CERT_REGISTER_PIN.md."""
    lines: list[str] = []
    ok = True

    mounted = read_u16(client, OFF_MOUNTED, unit)
    if mounted != CERT_MOUNTED_VERSION:
        ok = False
        lines.append(f"FAIL 40011 mounted_version: {mounted} (expected {CERT_MOUNTED_VERSION})")
    else:
        lines.append(f"PASS 40011 mounted_version: {mounted}")

    meas = read_u16(client, OFF_MEAS_CFG, unit)
    if meas is None:
        ok = False
        lines.append("FAIL 40232: read error")
    else:
        d = decode_meas_cfg(meas)
        if d["connection_mode"] != 0 or not d["integrator_rogowski"]:
            ok = False
            lines.append(
                f"FAIL 40232: raw={meas} (0x{meas:x}) mode={d['connection_mode']} "
                f"integrator={d['integrator_rogowski']}"
            )
        else:
            lines.append(
                f"PASS 40232: raw={meas} (0x{meas:x}) 1P + integrator ON"
            )

    ta_ch = read_u16(client, OFF_TA_CHRONOS, unit)
    if ta_ch is not None and ta_ch != CERT_TA_CHRONOS_REG:
        lines.append(f"WARN 40234 TA_Chronos_register: {ta_ch} (typical {CERT_TA_CHRONOS_REG})")
    elif ta_ch is not None:
        lines.append(f"PASS 40234 TA_Chronos_register: {ta_ch}")

    for label, off in (
        ("40243 minimum_voltage_ripple", OFF_MIN_V_RIPPLE),
        ("40245 minimum_current_ripple", OFF_MIN_I_RIPPLE),
        ("40247 minimum_power_ripple", OFF_MIN_P_RIPPLE),
    ):
        rip = read_f32(client, off, unit, word_swap=word_swap)
        if rip is None:
            ok = False
            lines.append(f"FAIL {label}: read error")
        elif abs(rip) > RIPPLE_TOLERANCE:
            ok = False
            lines.append(f"FAIL {label}: {rip:g} (expected 0)")
        else:
            lines.append(f"PASS {label}: {rip:g}")

    sens = read_f32(client, OFF_TA_SENS, unit, word_swap=word_swap)
    if sens is None:
        ok = False
        lines.append("FAIL 40235: read error")
    elif abs(sens - sens_v_per_a) > CERT_SENS_TOLERANCE:
        ok = False
        lines.append(f"FAIL 40235 sensitivity: {sens:g} (expected {sens_v_per_a})")
    else:
        mv = sens_v_per_a * 1_000_000.0
        lines.append(f"PASS 40235 sensitivity: {sens:g} V/A ({mv:g} mV/kA)")

    for name, off in CERT_MODBUS_MAP.items():
        val = read_f32(client, off, unit, word_swap=word_swap)
        if val is None:
            ok = False
            lines.append(f"FAIL offset {off} ({name}): read error")
        else:
            lines.append(f"READ offset {off} ({name}): {val:g}")

    lines.append("CERT PIN: PASS" if ok else "CERT PIN: FAIL")
    return ok, lines


def apply_cert_pin(
    client: ModbusTcpClient,
    unit: int,
    *,
    word_swap: bool = True,
    sens_v_per_a: float = CERT_SENS_V_PER_A,
    force_writes: bool = False,
) -> bool:
    """Apply REQ-EMT-CERT-001/002 then verify. Skips 40232/40235 if already OK (unless force_writes)."""
    import time

    p_before = read_f32(client, CERT_MODBUS_MAP["p1_w"], unit, word_swap=word_swap)

    cur = read_u16(client, OFF_MEAS_CFG, unit)
    if cur is None:
        return False
    new = apply_1p_rogowski(cur)
    d = decode_meas_cfg(new)
    need_meas = force_writes or d["connection_mode"] != 0 or not d["integrator_rogowski"]
    if need_meas and new != cur:
        if not write_u16(client, OFF_MEAS_CFG, new, unit):
            return False
    elif need_meas and new == cur and force_writes:
        if not write_u16(client, OFF_MEAS_CFG, new, unit):
            return False

    cur_sens = read_f32(client, OFF_TA_SENS, unit, word_swap=word_swap)
    need_sens = force_writes or cur_sens is None or abs(cur_sens - sens_v_per_a) > CERT_SENS_TOLERANCE
    if need_sens:
        if not write_f32(client, OFF_TA_SENS, sens_v_per_a, unit, word_swap=word_swap):
            return False
        time.sleep(2.0)

    for off, label in (
        (OFF_MIN_V_RIPPLE, "40243 minimum_voltage_ripple"),
        (OFF_MIN_I_RIPPLE, "40245 minimum_current_ripple"),
        (OFF_MIN_P_RIPPLE, "40247 minimum_power_ripple"),
    ):
        cur_rip = read_f32(client, off, unit, word_swap=word_swap)
        if force_writes or cur_rip is None or abs(cur_rip) > RIPPLE_TOLERANCE:
            if not write_f32(client, off, 0.0, unit, word_swap=word_swap):
                print(f"WRITE {label} FAILED")
                return False

    p_after = read_f32(client, CERT_MODBUS_MAP["p1_w"], unit, word_swap=word_swap)
    if (
        p_before is not None
        and p_after is not None
        and abs(p_before) > 50.0
        and abs(p_after) < 1.0
    ):
        print(
            f"WARN: P1 dropped {p_before:.1f} W -> {p_after:.1f} W after 40235 write "
            f"({sens_v_per_a:g} V/A). Check load, coil mV/kA (FRC-350=100), UI Rapporto TA; "
            "retry with --set-sensitivity-mvka 100 or deploy -YamlOnly (no meter write)."
        )

    ok, _ = verify_cert_pin(
        client, unit, word_swap=word_swap, sens_v_per_a=sens_v_per_a
    )
    return ok


def mv_per_ka_to_v_per_a(mv_per_ka: float) -> float:
    return mv_per_ka / 1_000_000.0


def describe_sensitivity_v_per_a(sens: float) -> str:
    """Human-readable verdict for 40235 Rogowski [V/A] (shared with tcp probe)."""
    if abs(sens - 0.000085) < 1e-8:
        return "OK 85 mV/kA (40235=0.000085). Bench pin."
    if abs(sens - 0.0001) < 1e-8:
        return "OK 100 mV/kA (FRC-350-100mV catalog)."
    if abs(sens - 0.00008) < 1e-8:
        return "OK 80 mV/kA (40235=0.00008). Legacy bench."
    if abs(sens - 1.0) < 0.05:
        return f"FAIL factory 1.0 (5A ratio). Use --set-sensitivity-mvka {BENCH_MV_KA_DEFAULT:g} or 100."
    if abs(sens - 85.0) < 0.5 or abs(sens - 100.0) < 0.5:
        return "FAIL typed mV into V/A field. Use --set-sensitivity-mvka, not 85/100 in register."
    return f"WARN unexpected V/A {sens:g} — expected 0.000085 (85) or 0.0001 (100)."


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
    ta_delay = read_f32(client, OFF_TA_DELAY, unit, word_swap=word_swap)
    tv_ratio = read_f32(client, OFF_TV_RATIO, unit, word_swap=word_swap)
    tv_delay = read_f32(client, OFF_TV_DELAY, unit, word_swap=word_swap)
    min_v_rip = read_f32(client, OFF_MIN_V_RIPPLE, unit, word_swap=word_swap)
    if ta_delay is not None:
        print(f"  40237 TA_Transducer_delay [deg]: {ta_delay:g}")
    if tv_ratio is not None:
        print(f"  40239 TV_Transducer_ratio: {tv_ratio:g}")
    if tv_delay is not None:
        print(f"  40241 TV_Transducer_delay [deg]: {tv_delay:g}")
    if min_v_rip is not None:
        print(f"  40243 minimum_voltage_ripple: {min_v_rip:g}")
    min_i_rip = read_f32(client, OFF_MIN_I_RIPPLE, unit, word_swap=word_swap)
    min_p_rip = read_f32(client, OFF_MIN_P_RIPPLE, unit, word_swap=word_swap)
    if min_i_rip is not None:
        print(f"  40245 minimum_current_ripple: {min_i_rip:g}")
    if min_p_rip is not None:
        print(f"  40247 minimum_power_ripple: {min_p_rip:g}")
    eu = read_u16(client, OFF_ENERGY_UNIT, unit)
    if eu is not None:
        print(f"  40256 Energy_unit_factor: {eu} ({'kWh' if eu == ENERGY_UNIT_KWH else 'see MR'})")
    for off, label in (
        (OFF_KWH1, "KWh1"),
        (OFF_KWH_SUM, "KWh_SUM"),
        (OFF_KVARH1, "KVARh1"),
        (OFF_KVAH1, "KVAh1"),
    ):
        e = read_i64_wh_tenth(client, off, unit)
        if e is not None:
            print(f"  {40001 + off} {label}: {e} Wh/10 -> {wh_tenth_to_kwh(e):.4f} kWh")
    if p1 is not None:
        print(f"  P1 @932: {p1:.1f} W")


def print_full_rms(
    client: ModbusTcpClient, unit: int, *, word_swap: bool
) -> None:
    rr = client.read_holding_registers(OFF_RMS_BLOCK, RMS_BLOCK_REGS, slave=unit)
    if rr.isError():
        print("  FULL RMS: read hr:906 # 80 failed")
        return
    print("  --- MR RMS block 40907-40985 (hr:906 # 80) ---")
    base = OFF_RMS_BLOCK
    for off, name in load_rms_signal_names():
        idx = off - base
        if idx < 0 or (idx + 1) >= len(rr.registers):
            continue
        val = f32_be(rr.registers[idx], rr.registers[idx + 1], word_swap=word_swap)
        print(f"    {40001 + off} {name}: {val:g}")


# CCLI runtime poll block (lab_tr400_emt432_lan3_tcp_102.yaml): min offset 906 .. 940+2
CCLI_POLL_START = 906
CCLI_POLL_COUNT = 36
CCLI_POLL_SIGNALS = (
    (906, "V_L1_N", "reg_line_voltage -> MMS PPV"),
    (922, "I_L1", "diagnostic (not in yaml poll yet)"),
    (932, "P1", "reg_active_power -> MMS TotW"),
    (940, "Q1", "reg_reactive_power -> MMS TotVAr"),
)
CCLI_DIAG_SIGNALS = ((972, "Frequency", "optional diagnostic"),)

# ITA Rev0 map may place KWh1 @ 40721 (offset 720) vs Rev2 pin @ 40716 (715)
OFF_KWH1_REV0 = 720


def _line(msg: str, out) -> None:
    print(msg)
    if out is not None:
        out.write(msg + "\n")


def dump_ccli_registers(
    client: ModbusTcpClient,
    host: str,
    unit: int,
    *,
    word_swap: bool,
    out_path: Optional[Path] = None,
    include_full_rms: bool = False,
) -> int:
    """Read all holding registers used by CCLI commissioning + MMS poll (PC Modbus, no DUT SSH)."""
    out_file = None
    if out_path is not None:
        out_path.parent.mkdir(parents=True, exist_ok=True)
        out_file = out_path.open("w", encoding="utf-8")

    def emit(msg: str) -> None:
        _line(msg, out_file)

    from datetime import datetime, timezone

    emit(f"# CCLI EMT432 register snapshot")
    emit(f"# host={host} unit={unit} word_swap={word_swap}")
    emit(f"# utc={datetime.now(timezone.utc).isoformat()}")
    emit(f"# doc=CCLI-LAB-EMT432-CCLI-COMM-001 rev 5 + lab_tr400_emt432_lan3_tcp_102.yaml")
    emit("")

    emit("## Commissioning / config (holding)")
    emit("| PLC | Offset | Name | Value | Role |")
    emit("|-----|--------|------|-------|------|")

    rows: list[tuple[int, int, str, str, str]] = [
        (40011, OFF_MOUNTED, "mounted_version", "u16", "verify Rogowski=1"),
        (40232, OFF_MEAS_CFG, "Measurement_Config", "u16", "1P+integrator+filter+energy save"),
        (40234, OFF_TA_CHRONOS, "TA_Chronos_register", "u16", "0=user TA"),
        (40235, OFF_TA_SENS, "TA_Transducer_ratio", "f32 V/A", "Rogowski sensitivity"),
        (40237, OFF_TA_DELAY, "TA_Transducer_delay", "f32 deg", "bench 0"),
        (40239, OFF_TV_RATIO, "TV_Transducer_ratio", "f32", "bench 1.0"),
        (40241, OFF_TV_DELAY, "TV_Transducer_delay", "f32 deg", "bench 0"),
        (40243, OFF_MIN_V_RIPPLE, "minimum_voltage_ripple", "f32", "bench 0"),
        (40245, OFF_MIN_I_RIPPLE, "minimum_current_ripple", "f32", "bench 0"),
        (40247, OFF_MIN_P_RIPPLE, "minimum_power_ripple", "f32", "bench 0"),
        (40256, OFF_ENERGY_UNIT, "Energy_unit_factor", "u16", "4=kWh"),
    ]
    for plc, off, name, typ, role in rows:
        if typ.startswith("u16"):
            val = read_u16(client, off, unit)
            disp = "READ_FAIL" if val is None else str(val)
            if plc == 40232 and val is not None:
                d = decode_meas_cfg(val)
                disp = (
                    f"{val} (0x{val:x}) mode={d['connection_mode']} "
                    f"int13={int(d['integrator_rogowski'])} "
                    f"filt15={int(d['filtered_measurement'])} "
                    f"esave9={int(d['energy_saving'])}"
                )
        else:
            val = read_f32(client, off, unit, word_swap=word_swap)
            disp = "READ_FAIL" if val is None else f"{val:g}"
        emit(f"| {plc} | {off} | {name} | {disp} | {role} |")

    emit("")
    emit("## Energy int64 [Wh/10] (commissioning log_energy_readings)")
    emit("| PLC | Offset | Name | Raw | kWh | Note |")
    emit("|-----|--------|------|-----|-----|------|")
    for plc, off, label in (
        (40716, OFF_KWH1, "KWh1 Rev2 pin"),
        (40721, OFF_KWH1_REV0, "KWh1 ITA Rev0 alt"),
        (40729, OFF_KWH_SUM, "KWh_SUM"),
        (40765, OFF_KVARH1, "KVARh1"),
        (40845, OFF_KVAH1, "KVAh1"),
    ):
        raw = read_i64_wh_tenth(client, off, unit)
        if raw is None:
            emit(f"| {plc} | {off} | {label} | READ_FAIL | - | |")
        else:
            emit(
                f"| {plc} | {off} | {label} | {raw} | {wh_tenth_to_kwh(raw):.6f} | |"
            )

    emit("")
    emit(f"## CCLI poll FC3 block (offset {CCLI_POLL_START} count {CCLI_POLL_COUNT})")
    emit("| PLC | Offset | Signal | Value | MMS / note |")
    emit("|-----|--------|--------|-------|------------|")
    rr = client.read_holding_registers(CCLI_POLL_START, CCLI_POLL_COUNT, slave=unit)
    if rr.isError():
        emit(f"| — | {CCLI_POLL_START} | block | READ_FAIL | same as ccli trace addr=40907 qty=36 |")
    else:
        for off, sig, note in CCLI_POLL_SIGNALS:
            idx = off - CCLI_POLL_START
            if idx < 0 or (idx + 1) >= len(rr.registers):
                val_s = "OUT_OF_BLOCK"
            else:
                val = f32_be(rr.registers[idx], rr.registers[idx + 1], word_swap=word_swap)
                val_s = f"{val:g}"
            emit(f"| {40001 + off} | {off} | {sig} | {val_s} | {note} |")

    emit("")
    emit("## CCLI diagnostic floats (outside poll block)")
    emit("| PLC | Offset | Signal | Value | Note |")
    emit("|-----|--------|--------|-------|------|")
    for off, sig, note in CCLI_DIAG_SIGNALS:
        val = read_f32(client, off, unit, word_swap=word_swap)
        val_s = "READ_FAIL" if val is None else f"{val:g}"
        emit(f"| {40001 + off} | {off} | {sig} | {val_s} | {note} |")

    cmd = read_u16(client, 905, unit)  # 40906 Command — not used by CCLI; diagnostic
    emit("")
    emit(f"## Not polled by CCLI (reference only)")
    emit(f"- 40906 Command (offset 905): {cmd if cmd is not None else 'READ_FAIL'} (do not write casually)")

    if include_full_rms:
        emit("")
        emit("## Full MR realtime block 40907-40985 (Basjes realtime_rms_mr_full)")
        rr80 = client.read_holding_registers(OFF_RMS_BLOCK, RMS_BLOCK_REGS, slave=unit)
        if rr80.isError():
            emit("READ_FAIL hr:906 # 80")
        else:
            base = OFF_RMS_BLOCK
            for off, name in load_rms_signal_names():
                idx = off - base
                if idx < 0 or (idx + 1) >= len(rr80.registers):
                    continue
                val = f32_be(rr80.registers[idx], rr80.registers[idx + 1], word_swap=word_swap)
                emit(f"- {40001 + off} ({off}) {name}: {val:g}")

    if out_file is not None:
        out_file.close()
        print(f"Wrote {out_path}")
    return 0


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
        "--apply-cert-pin",
        action="store_true",
        help=f"Write cert pin: 40232 1P+integrator + 40235={CERT_SENS_V_PER_A} ({CERT_MV_PER_KA:g} mV/kA)",
    )
    ap.add_argument(
        "--verify-cert-pin",
        action="store_true",
        help="Verify EMT432_CERT_REGISTER_PIN.md (exit 1 on FAIL)",
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
    ap.add_argument(
        "--full-rms",
        action="store_true",
        help="Print MR floats 40907-40985 (single FC3 read hr:906 # 80)",
    )
    ap.add_argument(
        "--dump-ccli-registers",
        action="store_true",
        help="Table of all CCLI commissioning + poll registers (PC→meter Modbus, no DUT SSH)",
    )
    ap.add_argument(
        "--out",
        type=Path,
        default=None,
        help="Write --dump-ccli-registers markdown to this file",
    )
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

    if args.dump_ccli_registers:
        out = args.out
        if out is None:
            ts = __import__("datetime").datetime.now(__import__("datetime").timezone.utc).strftime(
                "%Y%m%dT%H%M%SZ"
            )
            out = (
                Path(__file__).resolve().parents[1]
                / f"lab/evidence/phase5/EMT432_CCLI_REGISTER_SNAPSHOT_{ts}.md"
            )
        rc = dump_ccli_registers(
            client,
            args.host,
            args.unit,
            word_swap=word_swap,
            out_path=out,
            include_full_rms=args.full_rms,
        )
        client.close()
        return rc

    print_config(args.host, args.unit, word_swap=word_swap, client=client)
    if args.full_rms:
        print_full_rms(client, args.unit, word_swap=word_swap)

    exit_code = 0

    sens_v_per_a = CERT_SENS_V_PER_A
    if args.set_sensitivity_mvka is not None:
        sens_v_per_a = mv_per_ka_to_v_per_a(args.set_sensitivity_mvka)
    elif args.set_sensitivity_v_a is not None:
        sens_v_per_a = args.set_sensitivity_v_a

    if args.apply_cert_pin:
        if not apply_cert_pin(
            client, args.unit, word_swap=word_swap, sens_v_per_a=sens_v_per_a
        ):
            print("APPLY CERT PIN FAILED")
            exit_code = 1
        else:
            print("APPLY CERT PIN OK")
        ok, vlines = verify_cert_pin(
            client, args.unit, word_swap=word_swap, sens_v_per_a=sens_v_per_a
        )
        for line in vlines:
            print(line)
        if not ok:
            exit_code = 1
        client.close()
        return exit_code

    if args.verify_cert_pin:
        ok, vlines = verify_cert_pin(
            client, args.unit, word_swap=word_swap, sens_v_per_a=sens_v_per_a
        )
        for line in vlines:
            print(line)
        client.close()
        return 0 if ok else 1

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
        if args.full_rms:
            print_full_rms(client, args.unit, word_swap=word_swap)

    client.close()
    return 0


if __name__ == "__main__":
    sys.exit(main())

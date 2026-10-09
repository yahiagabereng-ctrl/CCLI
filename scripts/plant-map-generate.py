#!/usr/bin/env python3
"""Generate YAML artifacts from plant CSV mapping files."""
from __future__ import annotations

import csv
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLANT = ROOT / "apps" / "ccli" / "config" / "plant"
OUT = PLANT / "generated"
LAB_WWW = ROOT / "lab" / "tg544-openwrt" / "www" / "ccli"


def load_csv(path: Path) -> list[dict[str, str]]:
    with path.open(newline="", encoding="utf-8-sig") as f:
        return list(csv.DictReader(f))


def yaml_quote(s: str) -> str:
    if not s:
        return '""'
    if any(c in s for c in ':[]{}#&*!|>\'"@`'):
        return '"' + s.replace('"', '\\"') + '"'
    return s


def write_modbus_yaml(rows: list[dict[str, str]]) -> None:
    measures = [r for r in rows if r.get("record_type") in ("MEASURE", "STATUS") and r.get("enabled") == "Y"]
    controls = [r for r in rows if r.get("record_type") == "CONTROL" and r.get("enabled") == "Y"]
    meta = next((r for r in rows if r.get("record_type") == "META"), None)

    lines = [
        "# AUTO-GENERATED from PLANT_ASSIGNMENT_PLANE.csv — do not edit by hand",
        "meta:",
    ]
    if meta:
        lines.append(f"  notes: {yaml_quote(meta.get('notes', ''))}")
    lines.append("measures:")
    for m in measures:
        lines.append(f"  - plane_id: {yaml_quote(m['plane_id'])}")
        lines.append(f"    mms_path: {yaml_quote(m.get('mms_path', ''))}")
        lines.append(f"    slave_id: {yaml_quote(m.get('modbus_slave_id', ''))}")
        lines.append(f"    fc_read: {yaml_quote(m.get('modbus_fc_read', ''))}")
        lines.append(f"    reg_read: {yaml_quote(m.get('modbus_reg_read', ''))}")
        lines.append(f"    type_read: {yaml_quote(m.get('modbus_type_read', ''))}")
        lines.append(f"    gain_read: {yaml_quote(m.get('modbus_gain_read', ''))}")
        lines.append(f"    runtime_status: {yaml_quote(m.get('runtime_status', ''))}")
    lines.append("controls:")
    for c in controls:
        lines.append(f"  - plane_id: {yaml_quote(c['plane_id'])}")
        lines.append(f"    dso_source: {yaml_quote(c.get('dso_source', ''))}")
        lines.append(f"    slave_id: {yaml_quote(c.get('modbus_slave_id', ''))}")
        lines.append(f"    fc_write: {yaml_quote(c.get('modbus_fc_write', ''))}")
        lines.append(f"    reg_write: {yaml_quote(c.get('modbus_reg_write', ''))}")
        lines.append(f"    gain_write: {yaml_quote(c.get('modbus_gain_write', ''))}")
        lines.append(f"    transform: {yaml_quote(c.get('transform', ''))}")

    path = OUT / "plant_modbus_map.yaml"
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Wrote {path}")


def _int_or_none(s: str) -> int | None:
    s = (s or "").strip()
    return int(s) if s.isdigit() else None


def _num_or_none(s: str) -> float | None:
    try:
        return float((s or "").strip())
    except ValueError:
        return None


def apply_lab_bench_overrides(rows: list[dict[str, str]]) -> list[dict[str, str]]:
    """Two-PC bench: ttyS1 + COM5 single slave — one visible inverter (INV01)."""
    out: list[dict[str, str]] = []
    for r in rows:
        row = dict(r)
        pid = row.get("plane_id", "")
        if row.get("record_type") == "META":
            row["notes"] = (
                "9600 8N1 /dev/ttyS1 A1/B1 · PC COM5 lab mock · poll_ms=4000 · lab_combined_slave=1"
            )
        elif re.match(r"^INV(0[2-9]|10)\.", pid):
            row["enabled"] = "N"
        elif pid in ("PdC.TotW", "PdC.TotVAr"):
            row["field_device"] = "Lab RS485 slave (PdC 40001 map)"
            if pid == "PdC.TotW":
                row["notes"] = "Polled by ccli — TotW for MMS/URCB"
        elif pid == "INV01.TotW":
            row["field_device"] = "Huawei SUN2000 lab mock (COM5)"
            row["notes"] = "Reg 32080 on mock · PdC uses 40001 on same unit ID 1"
        elif pid == "GenPV.TotW":
            row["transform"] = "sum(INV01.TotW)"
            row["notes"] = "Lab bench: single inverter mock until multi-slave poll in ccli"
        out.append(row)
    return out


def write_ui_json(rows: list[dict[str, str]], *, lab_bench: bool = False) -> None:
    """Inverter / POC / control map for the lab zone dashboard (read-only view)."""
    if lab_bench:
        rows = apply_lab_bench_overrides(rows)
    meta = next((r for r in rows if r.get("record_type") == "META"), None)
    inverters: dict[str, dict] = {}
    poc: list[dict] = []
    aggregates: list[dict] = []
    controls: list[dict] = []

    for r in rows:
        rt = r.get("record_type", "")
        pid = r.get("plane_id", "")
        if rt in ("MEASURE", "STATUS"):
            m = re.match(r"^(INV\d+)\.(TotW|Status)$", pid)
            if m:
                inv = inverters.setdefault(
                    m.group(1),
                    {
                        "id": m.group(1),
                        "vendor": r.get("field_device", ""),
                        "slave_id": _int_or_none(r.get("modbus_slave_id", "")),
                        "mms_ln": r.get("ied_ld_ln", ""),
                        "enabled": r.get("enabled", "").upper() == "Y",
                        "runtime_status": r.get("runtime_status", ""),
                    },
                )
                if m.group(2) == "TotW":
                    inv["p"] = {
                        "fc": _int_or_none(r.get("modbus_fc_read", "")),
                        "reg": _int_or_none(r.get("modbus_reg_read", "")),
                        "type": r.get("modbus_type_read", ""),
                        "gain": _num_or_none(r.get("modbus_gain_read", "")),
                        "mms_path": r.get("mms_path", ""),
                    }
                    inv["notes"] = r.get("notes", "")
                else:
                    inv["status"] = {
                        "fc": _int_or_none(r.get("modbus_fc_read", "")),
                        "reg": _int_or_none(r.get("modbus_reg_read", "")),
                        "type": r.get("modbus_type_read", ""),
                        "mms_path": r.get("mms_path", ""),
                    }
                if r.get("runtime_status") == "CID_GAP":
                    inv["runtime_status"] = "CID_GAP"
            elif pid.startswith("PdC."):
                poc.append(
                    {
                        "id": pid,
                        "device": r.get("field_device", ""),
                        "slave_id": _int_or_none(r.get("modbus_slave_id", "")),
                        "reg": _int_or_none(r.get("modbus_reg_read", "")),
                        "type": r.get("modbus_type_read", ""),
                        "mms_path": r.get("mms_path", ""),
                        "runtime_status": r.get("runtime_status", ""),
                    }
                )
        elif rt == "AGGREGATE":
            aggregates.append(
                {
                    "id": pid,
                    "mms_path": r.get("mms_path", ""),
                    "transform": r.get("transform", ""),
                    "enabled": r.get("enabled", "").upper() == "Y",
                    "runtime_status": r.get("runtime_status", ""),
                    "notes": r.get("notes", ""),
                }
            )
        elif rt == "CONTROL":
            controls.append(
                {
                    "id": pid,
                    "app_group": r.get("app_group", ""),
                    "dso_source": r.get("dso_source", ""),
                    "clause": r.get("clause", ""),
                    "slave_id": r.get("modbus_slave_id", ""),
                    "fc": _int_or_none(r.get("modbus_fc_write", "")),
                    "reg": _int_or_none(r.get("modbus_reg_write", "")),
                    "gain": _num_or_none(r.get("modbus_gain_write", "")),
                    "transform": r.get("transform", ""),
                    "enabled": r.get("enabled", "").upper() == "Y",
                    "runtime_status": r.get("runtime_status", ""),
                }
            )

    source = (
        "apps/ccli/config/plant/PLANT_ASSIGNMENT_PLANE.csv (lab-bench overlay)"
        if lab_bench
        else "apps/ccli/config/plant/PLANT_ASSIGNMENT_PLANE.csv"
    )
    doc = {
        "schema": "ccli-plant-ui-map/1",
        "source": source,
        "bus_notes": meta.get("notes", "") if meta else "",
        "lab_combined_mock": lab_bench,
        "poc": poc,
        "inverters": sorted(inverters.values(), key=lambda i: i["id"]),
        "aggregates": aggregates,
        "controls": controls,
    }
    text = json.dumps(doc, indent=2, ensure_ascii=False) + "\n"
    for path in (OUT / "plant_ui_map.json", LAB_WWW / "plant_map.json"):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")
        print(f"Wrote {path}")


def write_goose_yaml(rows: list[dict[str, str]]) -> None:
    subs = [
        r
        for r in rows
        if r.get("subscriber_ied") == "CCI016_01"
        and r.get("bind_kind") == "goose_sub"
        and r.get("enabled", "").upper() == "Y"
    ]
    pubs = [r for r in rows if r.get("sending_relay") == "CCI016_01" and r.get("bind_kind") == "mms_int"]

    lines = [
        "# AUTO-GENERATED from GOOSE_DATA_MAP.csv — do not edit by hand",
        "publish:",
    ]
    seen_gcb: set[str] = set()
    for p in pubs:
        gcb = p.get("cb_name", "")
        if gcb in seen_gcb:
            continue
        seen_gcb.add(gcb)
        lines.append(f"  - cb_name: {yaml_quote(gcb)}")
        lines.append(f"    cb_go_id: {yaml_quote(p.get('cb_go_id', ''))}")
        lines.append(f"    dataset: {yaml_quote(p.get('dataset_name', ''))}")
        lines.append(f"    app_id_hex: {yaml_quote(p.get('app_id_hex', ''))}")
        lines.append(f"    mac_address: {yaml_quote(p.get('multicast_address', ''))}")
        lines.append(f"    members:")
        for m in pubs:
            if m.get("cb_name") == gcb:
                lines.append(f"      - index: {m.get('member_index', '0')}")
                lines.append(f"        desc: {yaml_quote(m.get('member_desc', ''))}")
                lines.append(f"        mms_path: {yaml_quote(m.get('mms_path', ''))}")

    lines.append("subscribe:")
    for s in subs:
        lines.append(f"  - map_id: {yaml_quote(s.get('map_id', ''))}")
        lines.append(f"    publisher: {yaml_quote(s.get('sending_relay', ''))}")
        lines.append(f"    dataset: {yaml_quote(s.get('dataset_name', ''))}")
        lines.append(f"    member_index: {s.get('member_index', '0')}")
        lines.append(f"    member_desc: {yaml_quote(s.get('member_desc', ''))}")
        lines.append(f"    app_id_hex: {yaml_quote(s.get('app_id_hex', ''))}")
        lines.append(f"    mac_address: {yaml_quote(s.get('multicast_address', ''))}")
        lines.append(f"    subscriber_local: {yaml_quote(s.get('subscriber_local', ''))}")
        lines.append(f"    mms_path: {yaml_quote(s.get('mms_path', ''))}")

    path = OUT / "goose_subscribe.yaml"
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Wrote {path}")


def write_mms_yaml(rows: list[dict[str, str]]) -> None:
    lines = [
        "# AUTO-GENERATED from MMS_POINT_MAP.csv — do not edit by hand",
        "points:",
    ]
    for r in rows:
        if r.get("Enabled", "").upper() != "Y":
            continue
        lines.append(f"  - name: {yaml_quote(r.get('Name', ''))}")
        lines.append(f"    object_ref: {yaml_quote(r.get('ObjectRef', ''))}")
        lines.append(f"    fc: {yaml_quote(r.get('FC', ''))}")
        lines.append(f"    interval_s: {r.get('Interval', '4')}")
        lines.append(f"    description: {yaml_quote(r.get('Description', ''))}")
        lines.append(f"    access_role: {yaml_quote(r.get('AccessRole', ''))}")
        lines.append(f"    access_right: {yaml_quote(r.get('AccessRight', ''))}")
        lines.append(f"    annex_scope: {yaml_quote(r.get('AnnexScope', ''))}")
        if r.get("OperateObjectRef", "").strip():
            lines.append(f"    operate_object_ref: {yaml_quote(r.get('OperateObjectRef', ''))}")
        lines.append(f"    app_group: {yaml_quote(r.get('AppGroup', ''))}")
        lines.append(f"    function_class: {yaml_quote(r.get('FunctionClass', ''))}")
        lines.append(f"    function_label: {yaml_quote(r.get('FunctionLabel', ''))}")
        lines.append(f"    ln_class: {yaml_quote(r.get('LNClass', ''))}")
        lines.append(f"    ln_ref: {yaml_quote(r.get('LNRef', ''))}")
        lines.append(f"    clause: {yaml_quote(r.get('Clause', ''))}")
        lines.append(f"    point_kind: {yaml_quote(r.get('PointKind', ''))}")
        if r.get("O11Priority", "").strip():
            lines.append(f"    o11_priority: {r.get('O11Priority', '')}")
            lines.append(f"    o11_priority_label: {yaml_quote(r.get('O11PriorityLabel', ''))}")

    path = OUT / "mms_point_map.yaml"
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Wrote {path}")


def main() -> int:
    import argparse

    parser = argparse.ArgumentParser(description="Generate plant mapping artifacts from CSV.")
    parser.add_argument(
        "--lab-bench",
        action="store_true",
        help="Emit plant_map.json for two-PC bench (ttyS1 COM5, INV01 only)",
    )
    args = parser.parse_args()

    OUT.mkdir(parents=True, exist_ok=True)

    assign = load_csv(PLANT / "PLANT_ASSIGNMENT_PLANE.csv")
    goose = load_csv(PLANT / "GOOSE_DATA_MAP.csv")
    mms = load_csv(PLANT / "MMS_POINT_MAP.csv")

    if not args.lab_bench:
        write_modbus_yaml(assign)
    write_ui_json(assign, lab_bench=args.lab_bench)
    if not args.lab_bench:
        write_goose_yaml(goose)
        write_mms_yaml(mms)
    print("Generation complete.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

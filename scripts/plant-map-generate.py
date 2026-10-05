#!/usr/bin/env python3
"""Generate YAML artifacts from plant CSV mapping files."""
from __future__ import annotations

import csv
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLANT = ROOT / "apps" / "ccli" / "config" / "plant"
OUT = PLANT / "generated"


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
    OUT.mkdir(parents=True, exist_ok=True)

    assign = load_csv(PLANT / "PLANT_ASSIGNMENT_PLANE.csv")
    goose = load_csv(PLANT / "GOOSE_DATA_MAP.csv")
    mms = load_csv(PLANT / "MMS_POINT_MAP.csv")

    write_modbus_yaml(assign)
    write_goose_yaml(goose)
    write_mms_yaml(mms)
    print("Generation complete.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

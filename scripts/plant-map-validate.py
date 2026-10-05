#!/usr/bin/env python3
"""Validate plant CSV mapping bundle."""
from __future__ import annotations

import csv
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLANT = ROOT / "apps" / "ccli" / "config" / "plant"

VB_RE = re.compile(r"^VB\d{3}$")
MAC_RE = re.compile(r"^01-0C-CD-01-00-[0-9A-Fa-f]{2}$")
APPID_RE = re.compile(r"^0x[0-9A-Fa-f]+$")


def load_csv(path: Path) -> tuple[list[str], list[dict[str, str]]]:
    with path.open(newline="", encoding="utf-8-sig") as f:
        reader = csv.DictReader(f)
        rows = [dict(r) for r in reader]
        return list(reader.fieldnames or []), rows


def err(msg: str) -> None:
    print(f"ERROR: {msg}", file=sys.stderr)


def validate_goose() -> bool:
    path = PLANT / "GOOSE_DATA_MAP.csv"
    if not path.exists():
        err(f"missing {path}")
        return False

    _, rows = load_csv(path)
    ok = True
    seen_ids: set[str] = set()
    blocks: dict[tuple[str, str, str], dict[str, str]] = {}

    for i, row in enumerate(rows, start=2):
        mid = row.get("map_id", "").strip()
        if not mid:
            err(f"GOOSE line {i}: empty map_id")
            ok = False
            continue
        if mid in seen_ids:
            err(f"GOOSE line {i}: duplicate map_id {mid}")
            ok = False
        seen_ids.add(mid)

        mac = row.get("multicast_address", "").strip()
        ds = row.get("dataset_name", "").strip()
        idx = row.get("member_index", "").strip()
        sub = row.get("subscriber_ied", "").strip()
        local = row.get("subscriber_local", "").strip()
        enabled = row.get("enabled", "").strip().upper()

        if mac and not MAC_RE.match(mac):
            err(f"GOOSE line {i}: bad multicast_address {mac}")
            ok = False

        app_id = row.get("app_id_hex", "").strip()
        if app_id and not APPID_RE.match(app_id):
            err(f"GOOSE line {i}: bad app_id_hex {app_id}")
            ok = False

        if local and not VB_RE.match(local):
            err(f"GOOSE line {i}: subscriber_local must be VBnnn or empty, got {local}")
            ok = False

        if sub == "CCI016_01" and enabled == "Y" and row.get("bind_kind") == "goose_sub":
            if not row.get("mms_path", "").strip() and row.get("member_desc") == "Breaker Failure":
                err(f"GOOSE line {i}: CCI016_01 goose_sub should have mms_path for BF")
                ok = False

        key = (mac, ds, idx)
        if key not in blocks:
            blocks[key] = {
                "member_desc": row.get("member_desc", ""),
                "sending_variable": row.get("sending_variable", ""),
            }
        else:
            ref = blocks[key]
            if ref["member_desc"] != row.get("member_desc", ""):
                err(f"GOOSE line {i}: member_desc mismatch for block {key}")
                ok = False

        dup_key = (ds, idx, sub)
        # checked via map_id uniqueness mostly

    enabled_ccli = [r for r in rows if r.get("sending_relay") == "CCI016_01" and r.get("enabled") == "Y"]
    if len(enabled_ccli) < 4:
        err("GOOSE: expected at least 4 enabled CCLI publish members")
        ok = False

    print(f"GOOSE_DATA_MAP.csv: {len(rows)} rows — {'OK' if ok else 'FAIL'}")
    return ok


def validate_assignment() -> bool:
    path = PLANT / "PLANT_ASSIGNMENT_PLANE.csv"
    if not path.exists():
        err(f"missing {path}")
        return False

    _, rows = load_csv(path)
    ok = True
    seen: set[str] = set()
    ln_refs = 0
    has_meta = False
    has_pdc = False

    for i, row in enumerate(rows, start=2):
        pid = row.get("plane_id", "").strip()
        if not pid:
            err(f"ASSIGN line {i}: empty plane_id")
            ok = False
            continue
        if pid in seen:
            err(f"ASSIGN line {i}: duplicate plane_id {pid}")
            ok = False
        seen.add(pid)

        rtype = row.get("record_type", "")
        if rtype == "META":
            has_meta = True
        if pid.startswith("PdC."):
            has_pdc = True

        if rtype == "LN_REF":
            ln_refs += 1

        dev = row.get("field_device", "")
        reg = row.get("modbus_reg_read", "").strip()
        if "Huawei" in dev and reg:
            try:
                if int(reg) < 30000:
                    err(f"ASSIGN line {i}: Huawei register should be direct >=30000, got {reg}")
                    ok = False
            except ValueError:
                err(f"ASSIGN line {i}: invalid modbus_reg_read {reg}")
                ok = False

        if "Grid analyzer" in dev and reg:
            try:
                rnum = int(reg)
                allowed = {0, 2, 40001, 40003}
                if rnum not in allowed:
                    err(f"ASSIGN line {i}: PdC analyzer reg must be one of {sorted(allowed)}, got {reg}")
                    ok = False
            except ValueError:
                pass

    if not has_meta:
        err("ASSIGN: missing META row")
        ok = False
    if not has_pdc:
        err("ASSIGN: missing PdC row")
        ok = False
    if ln_refs != 31:
        err(f"ASSIGN: expected 31 LN_REF rows, got {ln_refs}")
        ok = False

    print(f"PLANT_ASSIGNMENT_PLANE.csv: {len(rows)} rows — {'OK' if ok else 'FAIL'}")
    return ok


def validate_mms() -> bool:
    import sys

    sys.path.insert(0, str(ROOT / "scripts"))
    from plant_mms_catalog import (
        MMS_HEADER,
        O11_CONTROL_FUNCTIONS,
        VALID_APP_GROUPS,
        build_full_mms_catalog,
        expected_operate_names,
        expected_qt_companion_names,
        parse_cid_datasets,
    )

    path = PLANT / "MMS_POINT_MAP.csv"
    if not path.exists():
        err(f"missing {path}")
        return False

    ok = True
    header, rows = load_csv(path)
    seen_names: set[str] = set()
    seen_refs: set[str] = set()
    csv_by_name = {r.get("Name", "").strip(): r for r in rows if r.get("Name", "").strip()}

    expected = {p["Name"]: p for p in build_full_mms_catalog()}
    dataset_only = {p["Name"]: p for p in parse_cid_datasets() if not p["Name"].endswith(("_q", "_t"))}

    if header != MMS_HEADER:
        err(f"MMS: header mismatch — expected schema v4 columns {len(MMS_HEADER)}")
        ok = False

    for i, row in enumerate(rows, start=2):
        name = row.get("Name", "").strip()
        if not name:
            err(f"MMS line {i}: empty Name")
            ok = False
            continue
        if name in seen_names:
            err(f"MMS line {i}: duplicate Name {name}")
            ok = False
        seen_names.add(name)

        obj = row.get("ObjectRef", "").strip()
        if not obj.startswith("CCI016_01"):
            err(f"MMS line {i}: ObjectRef must start with CCI016_01")
            ok = False
        if obj in seen_refs:
            err(f"MMS line {i}: duplicate ObjectRef {obj}")
            ok = False
        seen_refs.add(obj)

        for col in ("FC", "DataType", "Interval", "ReportKey", "Description"):
            if not row.get(col, "").strip():
                err(f"MMS line {i}: missing {col} for {name}")
                ok = False

        for col in ("AccessRole", "AccessRight", "AnnexScope"):
            if not row.get(col, "").strip():
                err(f"MMS line {i}: missing RBAC column {col} for {name}")
                ok = False

        for col in ("AppGroup", "FunctionClass", "FunctionLabel", "LNClass", "LNRef", "Clause", "PointKind"):
            if not row.get(col, "").strip():
                err(f"MMS line {i}: missing function class column {col} for {name}")
                ok = False

        app_group = row.get("AppGroup", "").strip()
        if app_group not in VALID_APP_GROUPS:
            err(f"MMS line {i}: invalid AppGroup {app_group} for {name}")
            ok = False

        if row.get("AccessRight") == "CONTROL":
            if row.get("AccessRole") != "DSO_OPERATOR":
                err(f"MMS line {i}: CONTROL requires DSO_OPERATOR for {name}")
                ok = False
            if not row.get("OperateObjectRef", "").strip():
                err(f"MMS line {i}: CONTROL row missing OperateObjectRef for {name}")
                ok = False

        function_class = row.get("FunctionClass", "").strip()
        o11 = row.get("O11Priority", "").strip()
        o11_label = row.get("O11PriorityLabel", "").strip()
        if function_class in O11_CONTROL_FUNCTIONS:
            exp_idx, exp_label = O11_CONTROL_FUNCTIONS[function_class]
            if o11 != str(exp_idx):
                err(
                    f"MMS line {i}: O11Priority mismatch for {name} "
                    f"(expected {exp_idx}, got {o11 or 'empty'})"
                )
                ok = False
            if o11_label != exp_label:
                err(f"MMS line {i}: O11PriorityLabel mismatch for {name}")
                ok = False
        elif o11 or o11_label:
            err(f"MMS line {i}: O11Priority set on non-control row {name} ({function_class})")
            ok = False

    missing_dataset = set(dataset_only) - seen_names
    if missing_dataset:
        err(f"MMS: missing CID dataset value points ({len(missing_dataset)}): {sorted(missing_dataset)[:8]}...")
        ok = False

    missing_qt = expected_qt_companion_names() - seen_names
    if missing_qt:
        err(f"MMS: missing q/t companions ({len(missing_qt)}): {sorted(missing_qt)[:8]}...")
        ok = False

    missing_operate = expected_operate_names() - seen_names
    if missing_operate:
        err(f"MMS: missing Operate rows ({len(missing_operate)}): {sorted(missing_operate)[:8]}...")
        ok = False

    missing_catalog = set(expected) - seen_names
    if missing_catalog:
        err(f"MMS: missing catalog points ({len(missing_catalog)}): {sorted(missing_catalog)[:10]}...")
        ok = False

    extra = seen_names - set(expected)
    if extra:
        err(f"MMS: unexpected extra points ({len(extra)}): {sorted(extra)[:8]}...")
        ok = False

    for name, exp in expected.items():
        if name not in csv_by_name:
            continue
        got = csv_by_name[name]
        if got.get("ObjectRef", "").strip() != exp["ObjectRef"]:
            err(f"MMS: ObjectRef mismatch for {name}")
            ok = False
        for col in (
            "AccessRole",
            "AccessRight",
            "AnnexScope",
            "AppGroup",
            "FunctionClass",
            "FunctionLabel",
            "LNClass",
            "LNRef",
            "Clause",
            "PointKind",
            "O11Priority",
            "O11PriorityLabel",
        ):
            if got.get(col, "").strip() != exp.get(col, "").strip():
                err(f"MMS: {col} mismatch for {name}")
                ok = False

    o11_present = {
        r.get("FunctionClass", "")
        for r in rows
        if r.get("O11Priority", "").strip()
    }
    o11_in_csv = {fc for fc in o11_present if fc in O11_CONTROL_FUNCTIONS}
    o11_expected_in_cid = {
        fc
        for fc in O11_CONTROL_FUNCTIONS
        if any(p.get("FunctionClass") == fc for p in expected.values())
    }
    missing_o11 = o11_expected_in_cid - o11_in_csv
    if missing_o11:
        err(f"MMS: control FunctionClass missing O11 rows: {sorted(missing_o11)}")
        ok = False

    enabled = sum(1 for r in rows if r.get("Enabled", "").upper() == "Y")
    qt_count = sum(1 for n in seen_names if n.endswith("_q") or n.endswith("_t"))
    ctl_count = sum(1 for r in rows if r.get("AccessRight") == "CONTROL")
    fn_classes = len({r.get("FunctionClass", "") for r in rows})
    o11_count = sum(1 for r in rows if r.get("O11Priority", "").strip())
    o11_tiers = len({r.get("O11Priority", "") for r in rows if r.get("O11Priority", "").strip()})
    print(
        f"MMS_POINT_MAP.csv: {len(rows)} rows ({enabled} enabled, {qt_count} q/t, "
        f"{ctl_count} CONTROL, {fn_classes} function classes, "
        f"{o11_count} O11-tagged, {o11_tiers} priority tiers) "
        f"catalog={len(expected)} — {'OK' if ok else 'FAIL'}"
    )
    return ok


def validate_index() -> bool:
    path = PLANT / "PLANT_MAP_INDEX.csv"
    if not path.exists():
        err(f"missing {path}")
        return False
    _, rows = load_csv(path)
    ok = len(rows) >= 3
    print(f"PLANT_MAP_INDEX.csv: {len(rows)} rows — {'OK' if ok else 'FAIL'}")
    return ok


def main() -> int:
    results = [
        validate_index(),
        validate_goose(),
        validate_assignment(),
        validate_mms(),
    ]
    if all(results):
        print("All plant CSV validations passed.")
        return 0
    print("Plant CSV validation FAILED.", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main())

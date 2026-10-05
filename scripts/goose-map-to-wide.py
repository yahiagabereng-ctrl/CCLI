#!/usr/bin/env python3
"""Pivot GOOSE_DATA_MAP.csv to wide Berlin-style matrix (CSV output)."""
from __future__ import annotations

import argparse
import csv
from collections import OrderedDict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLANT = ROOT / "apps" / "ccli" / "config" / "plant"


def load_goose() -> list[dict[str, str]]:
    path = PLANT / "GOOSE_DATA_MAP.csv"
    with path.open(newline="", encoding="utf-8-sig") as f:
        return list(csv.DictReader(f))


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument(
        "-o",
        "--output",
        type=Path,
        default=PLANT / "generated" / "GOOSE_DATA_MAP_wide.csv",
    )
    ap.add_argument("--enabled-only", action="store_true")
    args = ap.parse_args()

    rows = load_goose()
    if args.enabled_only:
        rows = [r for r in rows if r.get("enabled", "").upper() == "Y"]

    subscribers: OrderedDict[str, None] = OrderedDict()
    block_rows: OrderedDict[tuple, dict] = OrderedDict()

    for r in rows:
        sub = r.get("subscriber_ied", "").strip()
        if not sub:
            continue
        subscribers[sub] = None

        key = (
            r.get("multicast_address", ""),
            r.get("dataset_name", ""),
            r.get("member_index", ""),
            r.get("member_desc", ""),
            r.get("sending_variable", ""),
            r.get("sending_relay", ""),
            r.get("cb_name", ""),
        )
        if key not in block_rows:
            block_rows[key] = {"_meta": r, "subs": {}}
        block_rows[key]["subs"][sub] = r.get("subscriber_local", "")

    header = [
        "multicast_address",
        "dataset_name",
        "member_index",
        "member_desc",
        "sending_variable",
        "sending_relay",
        "cb_name",
        *subscribers.keys(),
    ]

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open("w", newline="", encoding="utf-8-sig") as f:
        w = csv.writer(f)
        w.writerow(header)
        for key, data in block_rows.items():
            meta = data["_meta"]
            row = [
                meta.get("multicast_address", ""),
                meta.get("dataset_name", ""),
                meta.get("member_index", ""),
                meta.get("member_desc", ""),
                meta.get("sending_variable", ""),
                meta.get("sending_relay", ""),
                meta.get("cb_name", ""),
            ]
            for sub in subscribers:
                row.append(data["subs"].get(sub, ""))
            w.writerow(row)

    print(f"Wrote wide matrix: {args.output} ({len(block_rows)} signal rows × {len(subscribers)} subscribers)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""Regenerate MMS_POINT_MAP.csv from CID catalog (scripts/plant_mms_catalog.py)."""
from __future__ import annotations

import csv
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLANT = ROOT / "apps" / "ccli" / "config" / "plant"
sys.path.insert(0, str(ROOT / "scripts"))

from plant_mms_catalog import MMS_HEADER, build_full_mms_catalog  # noqa: E402


def write_csv(path: Path, rows: list[dict]) -> None:
    with path.open("w", newline="", encoding="utf-8-sig") as f:
        w = csv.DictWriter(f, fieldnames=MMS_HEADER, extrasaction="ignore")
        w.writeheader()
        w.writerows(rows)


def main() -> int:
    rows = build_full_mms_catalog()
    target = PLANT / "MMS_POINT_MAP.csv"
    try:
        write_csv(target, rows)
        enabled = sum(1 for r in rows if r.get("Enabled", "").upper() == "Y")
        print(
            f"Wrote {target} ({len(rows)} rows, {enabled} enabled, "
            f"port {rows[0]['Port'] if rows else '?'})"
        )
    except OSError as exc:
        print(f"Could not write {target} (close Excel/IDE tab): {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

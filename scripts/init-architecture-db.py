#!/usr/bin/env python3
"""Initialize and seed Architecture/architecture.db from corpus + optional RAG probe."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from architecture_db import ArchitectureDB, DEFAULT_DB  # noqa: E402


def main() -> int:
    parser = argparse.ArgumentParser(description="CCLI PF2 architecture SQLite database")
    parser.add_argument("--db", type=Path, default=DEFAULT_DB, help="Database path")
    parser.add_argument("--no-rag-probe", action="store_true", help="Skip RAG ingestion probe")
    parser.add_argument("--query", type=str, help="Run a SQL query and print JSON")
    parser.add_argument("--dashboard", action="store_true", help="Print dashboard counts")
    parser.add_argument("--export", type=Path, help="Export JSON snapshot to path")
    args = parser.parse_args()

    db = ArchitectureDB(args.db)

    if args.query:
        if not args.db.exists():
            print(f"Database not found: {args.db}", file=sys.stderr)
            return 1
        rows = db.query(args.query)
        print(json.dumps(rows, indent=2))
        return 0

    if args.dashboard and args.db.exists():
        stats = db.dashboard(probe_rag=not args.no_rag_probe)
        print(json.dumps(stats, indent=2))
        return 0

    stats = db.reset_and_seed(probe_rag=not args.no_rag_probe)
    print(f"Created {args.db}")
    print(json.dumps({k: v for k, v in stats.items() if k != "rag_probes"}, indent=2))
    if stats.get("rag_probes"):
        not_ingested = [k for k, v in stats["rag_probes"].items() if v != "INGESTED"]
        if not_ingested:
            print(f"\nRAG NOT_IN_RAG ({len(not_ingested)}): {', '.join(not_ingested[:8])}{'...' if len(not_ingested) > 8 else ''}")

    if args.export:
        db.export_json_snapshot(args.export)
        print(f"Exported snapshot → {args.export}")

    meta = {
        "db": str(args.db),
        "schema": str(ROOT / "Architecture" / "schema.sql"),
        "queries": str(ROOT / "Architecture" / "queries.sql"),
        "dashboard": {k: v for k, v in stats.items() if k != "rag_probes"},
    }
    (ROOT / "Architecture" / "_architecture_db_meta.json").write_text(
        json.dumps(meta, indent=2), encoding="utf-8"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

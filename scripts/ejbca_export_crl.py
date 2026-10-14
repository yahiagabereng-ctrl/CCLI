#!/usr/bin/env python3
"""Export EJBCA CA CRL to PEM (lab_crl.pem)."""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(REPO / "scripts"))
from ejbca_lab_common import export_crl_pem_from_ejbca  # noqa: E402


def main() -> int:
    ap = argparse.ArgumentParser(description="Export EJBCA CRL to PEM")
    ap.add_argument("--ca-name", default="CCLI-Lab-CA")
    ap.add_argument(
        "--out",
        type=Path,
        default=REPO / "apps" / "ccli" / "config" / "tls" / "ejbca" / "lab_crl.pem",
    )
    ap.add_argument("--no-publish", action="store_true", help="Only getcrl, do not createcrl")
    args = ap.parse_args()
    export_crl_pem_from_ejbca(
        args.ca_name, args.out, publish=not args.no_publish
    )
    print(f"OK -> {args.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

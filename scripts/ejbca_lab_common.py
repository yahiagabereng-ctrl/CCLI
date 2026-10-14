#!/usr/bin/env python3
"""Shared EJBCA Docker helpers for CCLI lab PKI (CRL export, etc.)."""
from __future__ import annotations

import subprocess
from pathlib import Path

from cryptography import x509
from cryptography.hazmat.primitives import serialization

CONTAINER_DEFAULT = "ccli-ejbca-ce"
EJBCA_SH = "/opt/keyfactor/bin/ejbca.sh"


def run_ejbca(args: list[str], *, container: str = CONTAINER_DEFAULT) -> None:
    cmd = ["docker", "exec", container, EJBCA_SH] + args
    proc = subprocess.run(cmd, capture_output=True, text=True)
    if proc.returncode != 0:
        raise RuntimeError(
            f"ejbca.sh {' '.join(args)} failed ({proc.returncode}): {proc.stderr or proc.stdout}"
        )


def export_crl_pem_from_ejbca(
    ca_name: str,
    dest_pem: Path,
    *,
    container: str = CONTAINER_DEFAULT,
    publish: bool = True,
) -> Path:
    """Fetch CA CRL from EJBCA container and write PEM to dest_pem."""
    dest_pem = Path(dest_pem)
    dest_pem.parent.mkdir(parents=True, exist_ok=True)
    crl_in = "/tmp/ccli-lab.crl"
    if publish:
        run_ejbca(["ca", "createcrl", "--caname", ca_name], container=container)
    run_ejbca(["ca", "getcrl", "--caname", ca_name, "-f", crl_in], container=container)
    der_local = dest_pem.with_name(dest_pem.stem + ".ejbca.der")
    subprocess.run(
        ["docker", "cp", f"{container}:{crl_in}", str(der_local)],
        check=True,
        capture_output=True,
        text=True,
    )
    crl = x509.load_der_x509_crl(der_local.read_bytes())
    dest_pem.write_bytes(crl.public_bytes(serialization.Encoding.PEM))
    return dest_pem

#!/usr/bin/env python3
"""Verify Solution B AARE auth from DUT TX_ACCEPT hex dump."""
from __future__ import annotations

import base64
import hashlib
import re
import sys
from pathlib import Path

from cryptography import x509
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives.asymmetric.utils import Prehashed
from cryptography.hazmat.primitives.serialization import load_pem_private_key

ROOT = Path(__file__).resolve().parents[1]
TLS = ROOT / "apps" / "ccli" / "config" / "tls"
sys.path.insert(0, str(Path(__file__).resolve().parent))
import verify_62351_auth as v  # noqa: E402


def pem_der(path: Path) -> bytes:
    pem = path.read_text()
    b64 = re.search(r"-----BEGIN CERTIFICATE-----(.*?)-----END", pem, re.S).group(1)
    return base64.b64decode(b64.replace("\n", ""))


def try_verify(pub, gt: bytes, sig: bytes) -> list[str]:
    hits: list[str] = []
    candidates = {
        "time_tlv_81": bytes([0x81, len(gt)]) + gt,
        "time_raw_gt": gt,
        "time_tlv_18": bytes([0x18, len(gt)]) + gt,
    }
    for name, signed in candidates.items():
        digest = hashlib.sha256(signed).digest()
        try:
            pub.verify(sig, digest, padding.PKCS1v15(), Prehashed(hashes.SHA256()))
            hits.append(f"{name} (digest)")
            continue
        except Exception:
            pass
        try:
            pub.verify(sig, signed, padding.PKCS1v15(), hashes.SHA256())
            hits.append(f"{name} (message)")
        except Exception:
            pass
    return hits


def main() -> int:
    hex_path = Path(__file__).resolve().parent / "tmp_tx_solution_b_20260929.hex"
    data = bytes.fromhex(hex_path.read_text().strip())
    inners = v.iter_cert_based_inners(data)
    if not inners:
        print("no auth block found")
        return 1
    off, cert_der, gt, sig = inners[0]
    print(f"TX_ACCEPT auth @{off:#x} cert={len(cert_der)} gt={gt!r} sig={sig[:4].hex()}...")
    for name in ("server.pem", "server_tls.pem"):
        der = pem_der(TLS / name)
        print(f"  {name}: der={len(der)} identical={der == cert_der}")
    print("\nSignature verify:")
    for name in ("server.pem", "server_tls.pem"):
        cert = x509.load_pem_x509_certificate((TLS / name).read_bytes())
        hits = try_verify(cert.public_key(), gt, sig)
        print(f"  {name}: {hits or ['FAIL']}")
    key = load_pem_private_key((TLS / "server.key").read_bytes(), password=None)
    hits = try_verify(key.public_key(), gt, sig)
    print(f"  server.key pubkey: {hits or ['FAIL']}")
    return 0 if hits else 1


if __name__ == "__main__":
    raise SystemExit(main())

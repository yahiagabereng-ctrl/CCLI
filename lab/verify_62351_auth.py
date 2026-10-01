#!/usr/bin/env python3
"""Parse 62351-4 MMS auth from CONNECT/ACCEPT hex and verify RSA-SHA256 signatures."""
from __future__ import annotations

import hashlib
import sys
from pathlib import Path

try:
    from cryptography import x509
    from cryptography.hazmat.primitives import hashes
    from cryptography.hazmat.primitives.asymmetric import padding
    from cryptography.hazmat.primitives.asymmetric.utils import Prehashed
    from cryptography.exceptions import InvalidSignature
except ImportError:
    print("pip install cryptography", file=sys.stderr)
    sys.exit(1)

TLS = Path(__file__).resolve().parents[1] / "apps" / "ccli" / "config" / "tls"


def read_tlv(data: bytes, pos: int) -> tuple[int, bytes, int]:
    tag = data[pos]
    pos += 1
    lb = data[pos]
    pos += 1
    if lb & 0x80:
        n = lb & 0x7F
        length = int.from_bytes(data[pos : pos + n], "big")
        pos += n
    else:
        length = lb
    val = data[pos : pos + length]
    return tag, val, pos + length


def iter_cert_based_inners(data: bytes) -> list[tuple[int, bytes, bytes, bytes]]:
    found: list[tuple[int, bytes, bytes, bytes]] = []
    for i in range(len(data) - 10):
        if data[i] != 0xA0:
            continue
        try:
            _, inner, end = read_tlv(data, i)
            p = 0
            t0, cert, p = read_tlv(inner, p)
            t1, gt, p = read_tlv(inner, p)
            t2, sig, p = read_tlv(inner, p)
            if t0 == 0x80 and t1 == 0x81 and t2 == 0x82 and len(sig) == 256:
                found.append((i, cert, gt, sig))
        except Exception:
            continue
    return found


def verify_sig(cert_pem: Path, gt: bytes, sig: bytes, label: str) -> bool:
    cert = x509.load_pem_x509_certificate(cert_pem.read_bytes())
    pub = cert.public_key()
    time_tlv = bytes([0x81, len(gt)]) + gt
    candidates = {
        "time_tlv_81": time_tlv,
        "time_raw_gt": gt,
        "time_tlv_18": bytes([0x18, len(gt)]) + gt,
    }
    print(f"\n=== {label} ===")
    print(f"  gt={gt.decode('ascii', errors='replace')!r} ({len(gt)} bytes)")
    ok = False
    for name, signed in candidates.items():
        digest = hashlib.sha256(signed).digest()
        try:
            pub.verify(sig, digest, padding.PKCS1v15(), Prehashed(hashes.SHA256()))
            print(f"  PASS {name}")
            ok = True
        except InvalidSignature:
            try:
                pub.verify(sig, signed, padding.PKCS1v15(), hashes.SHA256())
                print(f"  PASS {name} (message)")
                ok = True
            except InvalidSignature:
                print(f"  fail {name}")
    return ok


def main() -> int:
    if len(sys.argv) < 2:
        print(f"usage: {sys.argv[0]} <hexfile> [server|client]", file=sys.stderr)
        return 1
    hex_path = Path(sys.argv[1])
    role = sys.argv[2] if len(sys.argv) > 2 else "server"
    cert = TLS / ("server.pem" if role == "server" else "client.pem")
    data = bytes.fromhex(hex_path.read_text().strip())
    inners = iter_cert_based_inners(data)
    if not inners:
        print("no MMS auth inner found", file=sys.stderr)
        return 1
    print(f"found {len(inners)} certificate-based auth block(s)")
    any_ok = False
    for off, cert_der, gt, sig in inners:
        print(f"  @{off:04x} cert={len(cert_der)} sig_prefix={sig[:4].hex()}")
        if verify_sig(cert, gt, sig, f"{role} @{off:04x}"):
            any_ok = True
    return 0 if any_ok else 1


if __name__ == "__main__":
    raise SystemExit(main())

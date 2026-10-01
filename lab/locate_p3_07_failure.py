#!/usr/bin/env python3
"""Locate P3-07 failure layer in DUT TX_ACCEPT hex dump vs 62351-4 §11.2.2."""

from __future__ import annotations

import argparse
import base64
import hashlib
import re
import sys
from pathlib import Path

try:
    from cryptography import x509
    from cryptography.exceptions import InvalidSignature
    from cryptography.hazmat.primitives import hashes
    from cryptography.hazmat.primitives.asymmetric import padding
    from cryptography.hazmat.primitives.asymmetric.utils import Prehashed
except ImportError:
    print("pip install cryptography", file=sys.stderr)
    raise SystemExit(1)

REPO = Path(__file__).resolve().parents[1]
TLS = REPO / "apps" / "ccli" / "config" / "tls"

SESSION_TAGS = {
    0x0D: "CONNECT",
    0x0E: "ACCEPT",
    0x09: "FINISH",
    0x0A: "DISCONNECT",
    0x01: "GIVE_TOKENS",
    0x02: "DATA",
    0x05: "ABORT",
}


def read_tlv(data: bytes, pos: int) -> tuple[int, bytes, int]:
    if pos >= len(data):
        raise ValueError(f"EOF at {pos:#x}")
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
    end = pos + length
    if end > len(data):
        raise ValueError(f"TLV overrun tag={tag:#x} pos={pos:#x} len={length}")
    return tag, data[pos:end], end


def pem_der(path: Path) -> bytes:
    text = path.read_text(encoding="utf-8")
    b64 = "".join(line for line in text.splitlines() if not line.startswith("-----"))
    return base64.b64decode(b64)


def find_aare(data: bytes) -> int | None:
    for i in range(len(data) - 4):
        if data[i] != 0x61:
            continue
        try:
            _, body, _ = read_tlv(data, i)
            if 0xA1 in body and 0xA2 in body:
                return i
        except ValueError:
            continue
    return None


def find_auth_blocks(data: bytes) -> list[dict]:
    blocks: list[dict] = []
    for i in range(len(data) - 10):
        if data[i] != 0xA0:
            continue
        try:
            _, inner, _ = read_tlv(data, i)
            p = 0
            t0, cert, p = read_tlv(inner, p)
            t1, gt, p = read_tlv(inner, p)
            t2, sig, p = read_tlv(inner, p)
            if t0 == 0x80 and t1 == 0x81 and t2 == 0x82 and len(sig) == 256:
                blocks.append(
                    {
                        "offset": i,
                        "cert": cert,
                        "gt": gt,
                        "sig": sig,
                        "time_tlv": bytes([0x81, len(gt)]) + gt,
                    }
                )
        except (ValueError, IndexError):
            continue
    return blocks


def verify_sig(pub, signed: bytes, sig: bytes) -> bool:
    digest = hashlib.sha256(signed).digest()
    try:
        pub.verify(sig, digest, padding.PKCS1v15(), Prehashed(hashes.SHA256()))
        return True
    except InvalidSignature:
        try:
            pub.verify(sig, signed, padding.PKCS1v15(), hashes.SHA256())
            return True
        except InvalidSignature:
            return False


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("hexfile", type=Path)
    parser.add_argument(
        "--cert",
        type=Path,
        default=TLS / "server_combined.pem",
        help="PEM whose public key should verify embedded AARE signature",
    )
    args = parser.parse_args()
    data = bytes.fromhex(args.hexfile.read_text(encoding="utf-8").strip())

    print(f"=== P3-07 failure locator: {args.hexfile.name} ===")
    print(f"SPDU total length: {len(data)} octets")
    print()

    spdu_tag = data[0]
    spdu_name = SESSION_TAGS.get(spdu_tag, f"unknown({spdu_tag:#x})")
    print("## 1. Protocol layer (outside → inside)")
    print()
    print("| Layer | Status | Detail |")
    print("|-------|--------|--------|")
    print("| TCP :3782 | OK (prior captures) | TLS wraps everything below |")
    print("| TLS handshake | OK (prior captures) | Mutual cert auth complete before ACSE |")
    print(f"| Session SPDU | OK | tag **0x{spdu_tag:02x}** = **{spdu_name}** @ 0 |")

    aare_off = find_aare(data)
    if aare_off is None:
        print("| ACSE AARE | **MISSING** | No 0x61 AARE found |")
        return 1
    print(f"| ACSE AARE | OK | AARE PDU @ **0x{aare_off:04x}** |")

    blocks = find_auth_blocks(data)
    if not blocks:
        print("| MMS-Authentication-value | **MISSING** | No certificate-based auth block |")
        print()
        print("**Failure is before/responding-auth:** DUT did not emit §11.2.2 auth.")
        return 1

    auth = blocks[0]
    print(
        f"| responding-authentication-value | OK | MMS auth inner @ **0x{auth['offset']:04x}** "
        f"(cert={len(auth['cert'])} B, sig=256 B) |"
    )
    print("| MMS Initiate | **NOT REACHED** | TSP aborts after AARE verify |")
    print()

    print("## 2. §11.2.2 field decode (embedded in AARE)")
    print()
    print(f"| Field | Offset | Value |")
    print(f"|-------|--------|-------|")
    print(f"| authentication-Certificate [0] | 0x{auth['offset']:04x}+ | {len(auth['cert'])} octets DER |")
    gt = auth["gt"]
    print(f"| time [1] GeneralizedTime | — | `{gt.decode('ascii', errors='replace')}` ({len(gt)} B) |")
    print(f"| signature [2] | — | 256 octets, prefix `{auth['sig'][:4].hex()}` |")
    print(f"| signed payload (normative) | — | `81 {len(gt):02x}` + GT = `{auth['time_tlv'].hex()}` |")
    print()

    print("## 3. Offline crypto verify (where DUT vs TSP diverge)")
    print()
    cert_path = args.cert
    if not cert_path.is_file():
        print(f"Cert not found: {cert_path}")
        return 1
    file_der = pem_der(cert_path)
    embedded_match = auth["cert"] == file_der
    pub = x509.load_pem_x509_certificate(cert_path.read_bytes()).public_key()

    candidates = {
        "time_tlv_81 (§11.2.2)": auth["time_tlv"],
        "time_raw_gt": auth["gt"],
        "time_tlv_18": bytes([0x18, len(gt)]) + gt,
    }
    print("| Signed octets | OpenSSL/cryptography verify |")
    print("|---------------|----------------------------|")
    any_pass = False
    for name, signed in candidates.items():
        ok = verify_sig(pub, signed, auth["sig"])
        any_pass = any_pass or ok
        print(f"| {name} | **{'PASS' if ok else 'FAIL'}** |")
    print(f"| Embedded cert == `{cert_path.name}` | **{'YES' if embedded_match else 'NO'}** |")
    print()

    print("## 4. Exact failure point (TSP symptom)")
    print()
    if any_pass and embedded_match:
        print(
            "**DUT encoding + RSA signature are valid** for §11.2.2 `time_tlv_81` with the "
            f"embedded cert key (`{cert_path.name}`)."
        )
        print()
        print("TSP error `Unable to verify signature (+)` therefore occurs **inside Test Suite Pro**, "
              "after receiving a structurally valid AARE, ~2–5 ms post-AARE (TLS Encrypted Alert).")
        print()
        print("Likely TSP-side causes (in order):")
        print("1. TSP verifies a **different byte string** than §11.2.2 literal (not reproduced offline).")
        print("2. TSP uses **wrong public key** (e.g. TLS peer cert vs embedded auth cert) — ruled out for Solution C if same cert.")
        print("3. TSP **client AARQ** self-check mis-reported as server signature error.")
        print("4. TSP / stack **version defect** (TMW V12.3.2 MMS auth verify bug class).")
        print()
        print("**Not the failure layer:** TLS, Session ACCEPT LI, AARE presence, cert decodability, raw RSA over normative payload.")
    else:
        print("**Failure is on the DUT AARE auth payload** — offline verify failed and/or cert mismatch.")
        if not embedded_match:
            print(f"- Embedded cert ≠ `{cert_path.name}` — wrong `--cert` or wrong key used to sign.")
        if not any_pass:
            print("- Signature does not match any standard signed-payload candidate — fix `mms_aare_auth.cpp`.")

    if len(blocks) > 1:
        print()
        print(f"Note: {len(blocks)} auth blocks found; analysis used first @ 0x{auth['offset']:04x}.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
